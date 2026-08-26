#pragma once
#include <cstdint>
#include <memory>
#include "STILO/Shared/List.h"
#include "STILO/Shared/Parameters.h"
#include "STILO/Shared/Random.h"
#include "STILO/SolverModels/GraphModels/Vertex.h"

namespace STILO
{
    template<
        PheromoneUpdateOperator PheromoneUpdate,
        bool LocalUpdate,
        bool Heuristic>
    class PheromoneNode final : public Vertex<PheromoneNode<PheromoneUpdate, LocalUpdate, Heuristic>>
    {
    public:

        class PheromoneKey
        {
        private:

            PheromoneKey() {}

            template<
                template <class, bool> class PGraphType,
                bool PIsUnique,
                PheromoneUpdateOperator PPheromoneUpdate,
                bool PLocalUpdate,
                bool PHeuristic>
            friend class Pheromone;
        };

        class VertexKey
        {
        private:
            
            VertexKey() {}

            friend class Vertex<PheromoneNode>;
        };

        PheromoneNode(PheromoneKey, int stateCount, double alpha, double beta, double initialPheromone,
            double evaporation, double localDecay, const std::vector<double>& heuristicInfos)
            : Vertex<PheromoneNode>(stateCount),
            alpha_(alpha),
            beta_(beta),
            initialPheromone_(initialPheromone),
            pheromoneLevels_(stateCount, initialPheromone),
            evaporation_(1 - evaporation),
            pvalues_(stateCount),
            heuristicInfos_(heuristicInfos),
            probabilities_(stateCount),
            localUpdate_(localDecay > 0),                
            localDecay_(localDecay),
            probRnd_(0, 1),
            stateRnd_(0, stateCount - 1),
            needCalculation_(true),
            calculateIndex_(0),
            evaporationCount_(0)
        {                
            if constexpr (!LocalUpdate)
            {
                aliases_.resize(stateCount);
                tempProb_.resize(stateCount);
                smallIndices_.reserve(stateCount);
                largeIndices_.reserve(stateCount);
            } 
        }

        int measureExplicit(VertexKey)
        {        
            evaporateAll();

            if constexpr (LocalUpdate)
            {
                calculateProbabilities();
                double p = probRnd_.generate() * probabilities_[this->stateCount - 1];
                return localUpdate(findElement<double>(probabilities_, p, this->stateCount));
            }
            else
            {
                if(needCalculation_)
                {
                    formAliasTable();
                }
    
                int ind = stateRnd_.generate();
                double p = probRnd_.generate();
                return p < probabilities_[ind] ? ind : aliases_[ind];
            }
        }

        int measureExplicit(VertexKey, const std::vector<int>& permitted, int pnum)
        {
            evaporateAll();

            if(needCalculation_)
            {
                calculatePValues();
            }

            probabilities_[0] = pvalues_[permitted[0]];

            for (int i = 1; i < pnum; i++)
            {
                probabilities_[i] = pvalues_[permitted[i]] + probabilities_[i - 1];
            }
                
            double p = probRnd_.generate() * probabilities_[pnum - 1];

            if constexpr (LocalUpdate)
            {
                return localUpdate(findElement<double>(probabilities_, p, pnum));
            }
            else
            {
                return findElement<double>(probabilities_, p, pnum);
            }
        }

        void addUpdateExplicit(VertexKey, double value, int edge)
        {
            int c = 0;

            for(const int& i : rotedge_)
            {
                if(i == edge)
                {
                    rotval_[c] += value;
                    return;
                }

                c++;
            }

            rotedge_.push_back(edge);
            rotval_.push_back(value);
        }

        void setLimits(PheromoneKey, double minPheromone, double maxPheromone)
        {
            minPheromone_ = minPheromone;
            maxPheromone_ = maxPheromone;
        }

        void evaporate(PheromoneKey)
        {
            evaporationCount_++;
        }

        void applyUpdates(PheromoneKey)
        {
            if(rotval_.empty())
            {
                return;
            }

            evaporateAll();
            addPheromones();           
        }
        
    private:

        UniformRandomDoubleGenerator probRnd_;
        UniformRandomIntGenerator stateRnd_;
        std::vector<double> rotval_;
        std::vector<double> pheromoneLevels_;
        std::vector<double> heuristicInfos_;
        std::vector<double> pvalues_;
        std::vector<double> probabilities_;
        std::vector<double> tempProb_;
        std::vector<int> rotedge_;
        std::vector<int> aliases_;
        std::vector<int> smallIndices_;
        std::vector<int> largeIndices_;
        double alpha_;
        double beta_;
        double initialPheromone_;
        double minPheromone_;
        double maxPheromone_;
        double evaporation_;
        double localDecay_;
        double totalPValue;
        int calculateIndex_;
        int evaporationCount_;
        bool localUpdate_;
        bool needCalculation_;

        void calculatePValue(int edge)
        {
            if constexpr (Heuristic)
            {
                pvalues_[edge] = std::pow(pheromoneLevels_[edge], alpha_) * std::pow(heuristicInfos_[edge], beta_);
            }
            else
            {
                pvalues_[edge] = std::pow(pheromoneLevels_[edge], alpha_);
            }
        }

        void calculatePValues()
        {
            totalPValue = 0;

            for(int i = 0; i < this->stateCount; i++)
            {
                calculatePValue(i);
                totalPValue += pvalues_[i];
            }

            needCalculation_ = false;
        }

        void calculateProbabilities()
        {
            if(calculateIndex_ == this->stateCount)
            {
                return;
            }

            double prob = calculateIndex_ == 0 ? 0 : probabilities_[calculateIndex_ - 1];

            if(needCalculation_)
            {
                for(int i = calculateIndex_; i < this->stateCount; i++)
                {
                    calculatePValue(i);
                    probabilities_[i] = prob + pvalues_[i];
                    prob = probabilities_[i];                    
                }

                needCalculation_ = false;
            }
            else
            {
                for(int i = calculateIndex_; i < this->stateCount; i++)
                {
                    probabilities_[i] = prob + pvalues_[i];
                    prob = probabilities_[i];                    
                }
            }

            calculateIndex_ = this->stateCount;
        }

        void formAliasTable()
        {
            calculatePValues();

            double stdProb = 1 / (double)this->stateCount;

            for(int i = 0; i < this->stateCount; i++)
            {
                tempProb_[i] = pvalues_[i] / totalPValue - stdProb;
                probabilities_[i] = 1;

                if(tempProb_[i] < 0)
                {
                    smallIndices_.push_back(i);
                }
                else
                {
                    largeIndices_.push_back(i);
                }
            }

            while(!smallIndices_.empty() && !largeIndices_.empty())
            {
                int largeInd = largeIndices_.back();                    
                int smallInd = smallIndices_.back();
                smallIndices_.pop_back();

                aliases_[smallInd] = largeInd;
                probabilities_[smallInd] += tempProb_[smallInd] * this->stateCount;
                tempProb_[largeInd] += tempProb_[smallInd];

                if(tempProb_[largeInd] < 0)
                {
                    largeIndices_.pop_back();
                    smallIndices_.push_back(largeInd);
                }
                else if(tempProb_[largeInd] == 0)
                {
                    largeIndices_.pop_back();
                }
            }

            largeIndices_.clear();
            smallIndices_.clear();
        }

        void evaporateAll()
        {
            if(evaporationCount_ == 0)
            {
                return;
            }

            double val = pow(evaporation_, evaporationCount_);

            for(int i = 0; i < this->stateCount; i++)
            {
                if constexpr (PheromoneUpdate == PheromoneUpdateOperator::MaxMin)
                {
                    pheromoneLevels_[i] = std::max(minPheromone_, pheromoneLevels_[i] * val);
                }
                else
                {
                    pheromoneLevels_[i] *= val;
                }
            }

            evaporationCount_ = 0;
            needCalculation_ = true;
            calculateIndex_ = 0;
        }

        int localUpdate(int edge)
        {
            pheromoneLevels_[edge] = (1 - localDecay_) * pheromoneLevels_[edge] + localDecay_ * initialPheromone_;
            calculateIndex_ = edge;
            return edge;
        }

        void addPheromones()
        {
            int c = 0;

            for(int edge : rotedge_)
            {
                if constexpr (PheromoneUpdate == PheromoneUpdateOperator::MaxMin)
                {
                    pheromoneLevels_[edge] = std::min(maxPheromone_, pheromoneLevels_[edge] + rotval_[c]);
                }
                else
                {
                    pheromoneLevels_[edge] += rotval_[c];
                }
                
                c++;
            }

            rotedge_.clear();
            rotval_.clear();
        }
    };
}