#pragma once
#include <chrono>
#include <time.h>
#include "STILO/Shared/Containers.h"
#include "STILO/SolverModels/ACOModels/ACOIteratorWrapper.h"

namespace STILO
{
    template<
        template <class, bool> class GraphType,
        bool IsUnique,
        PheromoneUpdateOperator PheromoneUpdate,
        bool LocalUpdate,
        bool Heuristic,
        TourSelectionOperator TourSelection,
        PheromoneCalculationOperator PheromoneCalculation>
    class ACOIterator final : public ACOIteratorWrapper
    {
        using PheromoneClass = Pheromone<GraphType, IsUnique, PheromoneUpdate, LocalUpdate, Heuristic>;

    public:

        ACOIterator(ACOSolverKey, Problem* problem, int stateCount, int stringLength, int antCount,
            int eliteCount, double pheromoneConstant, double pheromoneInfluence,
            double heuristicInfluence, double initialPheromone, double evaporationParameter,
            const std::vector<std::vector<std::vector<double>>>& heuristicValues, double localDecayParameter)
            : problem_(problem),
              stateCount_(stateCount),
              stringLength_(stringLength),              
              antCount_(antCount),
              eliteCount_(eliteCount),
              costs_(antCount),
              paths_(antCount, stringLength),
              indices_(antCount),
              pheromone_(typename PheromoneClass::ACOIteratorKey{}, stateCount, stringLength, pheromoneInfluence,
                heuristicInfluence, initialPheromone, evaporationParameter, heuristicValues, localDecayParameter),
              iterationCount_(0)
        {
            if constexpr (TourSelection == TourSelectionOperator::GlobalBest)
            {
                globalBestCosts_.resize(eliteCount_);
                globalBestPaths_.resize(eliteCount_);

                for(int i = 0; i < eliteCount_; i++)
                {
                    globalBestCosts_[i] = DBL_MAX;
                    globalBestPaths_[i].resize(stringLength);                    
                }
            }
            else
            {
                globalBestCosts_.resize(1);
                globalBestPaths_.resize(1);
                globalBestCosts_[0] = DBL_MAX;
                globalBestPaths_[0].resize(stringLength);
            }

            scale_ = problem->getScaleFactor();

            if constexpr (PheromoneCalculation == PheromoneCalculationOperator::CostBased)
            {
                pheromoneConstant_ = pheromoneConstant * scale_;
            }
            else
            {
                if constexpr (TourSelection == TourSelectionOperator::EveryAnt)
                {
                    pheromoneConstant_ = pheromoneConstant / antCount;
                }
                else
                {
                    pheromoneConstant_ = pheromoneConstant / eliteCount;
                }
            }
            
        }

        void iterate(ACOSolverKey) override
        {
            for(int i = 0; i < antCount_; i++)
            {
                pheromone_.traverse(typename PheromoneClass::ACOIteratorKey{}, paths_[i]);
                costs_[i] = problem_->getCost(paths_[i]);
                int ind = findMinimum(costs_, indices_, costs_[i], i);
                shiftRightPartial<int>(indices_, ind, i);
                indices_[ind] = i;
            }

            pheromone_.evaporate(typename PheromoneClass::ACOIteratorKey{});
            findBestSolutions();
            updatePheromone();
            pheromone_.setBestSolution(typename PheromoneClass::ACOIteratorKey{}, globalBestCosts_[0] / scale_);
            pheromone_.applyUpdates(typename PheromoneClass::ACOIteratorKey{});
            iterationCount_++;
        }

        SolverOutput getOutput(ACOSolverKey) override
        {
            return SolverOutput(globalBestCosts_[0], problem_->getValue(globalBestCosts_[0]),
                globalBestPaths_[0], iterationCount_);
        }

    private:
    
        Problem* problem_;
        Pheromone<GraphType, IsUnique, PheromoneUpdate, LocalUpdate, Heuristic> pheromone_;
        std::vector<double> costs_;
        std::vector<double> globalBestCosts_;
        Matrix<int> paths_;
        std::vector<std::vector<int>> globalBestPaths_;
        std::vector<int> indices_;
        double pheromoneConstant_;
        double scale_;
        int stringLength_;
        int stateCount_;
        int antCount_;
        int eliteCount_;
        int iterationCount_;        

        void findBestSolutions()
        {
            if constexpr (TourSelection == TourSelectionOperator::GlobalBest)
            {
                int ind;

                for(int i = 0; i < antCount_; i++)
                {
                    ind = findElement<double>(globalBestCosts_, costs_[indices_[i]], eliteCount_);

                    if(ind == eliteCount_)
                    {
                        break;
                    }
        
                    shiftRight<double>(globalBestCosts_, ind, eliteCount_);
                    shiftListRight(globalBestPaths_, ind, eliteCount_);
                    globalBestCosts_[ind] = costs_[indices_[i]];
                    copy(paths_[indices_[i]], globalBestPaths_[ind], stringLength_);
                }
            }
            else
            {
                if(costs_[indices_[0]] < globalBestCosts_[0])
                {
                    globalBestCosts_[0] = costs_[indices_[0]];
                    copy(paths_[indices_[0]], globalBestPaths_[0], stringLength_);
                }
            }
        }

        double calculatePheromone(double cost, int rank)
        {
            if constexpr (PheromoneCalculation == PheromoneCalculationOperator::CostBased)
            {
                return pheromoneConstant_ / cost;                
            }
            else if constexpr(PheromoneCalculation == PheromoneCalculationOperator::RankBased)
            {
                return pheromoneConstant_ * rank;
            }
        }

        void updatePheromone()
        {
            if constexpr (TourSelection == TourSelectionOperator::EveryAnt)
            {
                for(int i = 0; i < antCount_; i++)
                {
                    pheromone_.addUpdate(typename PheromoneClass::ACOIteratorKey{},
                        calculatePheromone(costs_[indices_[i]], antCount_ - i), paths_[indices_[i]]);
                }
            }
            else if constexpr (TourSelection == TourSelectionOperator::IterationBest)
            {
                for(int i = 0; i < eliteCount_; i++)
                {
                    pheromone_.addUpdate(typename PheromoneClass::ACOIteratorKey{},
                        calculatePheromone(costs_[indices_[i]], eliteCount_ - i), paths_[indices_[i]]);
                }
            }
            else if constexpr (TourSelection == TourSelectionOperator::GlobalBest)
            {
                for(int i = 0; i < eliteCount_; i++)
                {
                    pheromone_.addUpdate(typename PheromoneClass::ACOIteratorKey{}, 
                        calculatePheromone(globalBestCosts_[i], eliteCount_ - i), globalBestPaths_[i]);
                }
            }
        }
    };
}