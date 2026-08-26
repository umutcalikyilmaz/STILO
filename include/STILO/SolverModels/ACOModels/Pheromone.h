#pragma once
#include <type_traits>
#include "STILO/SolverModels/ACOModels/PheromoneNode.h"
#include "STILO/SolverModels/GraphModels/MultiStageGraph.h"
#include "STILO/SolverModels/GraphModels/SingleStageGraph.h"

namespace STILO
{
    template<
        template <class, bool> class GraphType,
        bool IsUnique,
        PheromoneUpdateOperator PheromoneUpdate,
        bool LocalUpdate,
        bool Heuristic>
    class Pheromone
    {
        using NodeClass = PheromoneNode<PheromoneUpdate, LocalUpdate, Heuristic>;
        using GraphClass = Graph<GraphType, NodeClass, IsUnique>;
        
    public:

        class ACOIteratorKey
        {
        private:

            ACOIteratorKey() { }

            template <
                template <class, bool> class PGraphType,
                bool PIsUnique,
                PheromoneUpdateOperator PPheromoneUpdate,
                bool PLocalUpdate,
                bool PHeuristic,
                TourSelectionOperator PTourSelection,
                PheromoneCalculationOperator PPheromoneCalculation>
            friend class ACOIterator;
        };

        Pheromone(ACOIteratorKey, int stateNum, int stringLength, double pheromoneInfluence,
            double heuristicInfluence, double initialPheromone, double evaporationParameter,
            const std::vector<std::vector<std::vector<double>>>& heuristicValues,
            double localDecayParameter) 
            : stateCount_(stateNum),
              stringLength_(stringLength),
              pheromoneInfluence_(pheromoneInfluence),
              heuristicInfluence_(heuristicInfluence),
              evaporationParameter_(evaporationParameter),
              localDecayParameter_(localDecayParameter),
              bestSolution_(-1)
        {
            if constexpr(PheromoneUpdate == PheromoneUpdateOperator::Classical)
            {
                initialPheromone_ = initialPheromone;
            }
            else if constexpr(PheromoneUpdate == PheromoneUpdateOperator::MaxMin)
            {
                initialPheromone_ = 1 / evaporationParameter;
            }

            if(heuristicInfluence > 0)
            {
                heuristicValues_ = heuristicValues;
            }

            createGraph();
        }

        void traverse(ACOIteratorKey, std::span<int> path)
        {
            graph_->obtainString(typename GraphClass::SolverKey{}, path);
        }

        void addUpdate(ACOIteratorKey, double value, std::span<const int> solution)
        {
            graph_->updateGraph(typename GraphClass::SolverKey{}, value, solution);
        }

        void setBestSolution(ACOIteratorKey, double bestSolution)
        {
            if constexpr (PheromoneUpdate == PheromoneUpdateOperator::MaxMin)
            {
                if(bestSolution_ != bestSolution)
                {
                    maxPheromone_ = 1 / (evaporationParameter_ * bestSolution);
                    double pb = pow(0.05, (double)1 / stringLength_);
                    minPheromone_ = maxPheromone_ * (1 - pb) / ((((double)stringLength_ / 2) - 1) * pb);

                    for(int i = 0; i < nodeCount_; i++)
                    {
                        nodes_[i]->setLimits(typename NodeClass::PheromoneKey{}, minPheromone_, maxPheromone_);
                    }

                    bestSolution_ = bestSolution;
                }
            }
        }
        
        void applyUpdates(ACOIteratorKey)
        {            
            for(int i = 0; i < nodeCount_; i++)
            {
                nodes_[i]->applyUpdates(typename NodeClass::PheromoneKey{});
            }
        }
        
        void evaporate(ACOIteratorKey)
        {
            for(int i = 0; i < nodeCount_; i++)
            {
                nodes_[i]->evaporate(typename NodeClass::PheromoneKey{});
            }
        }        

    private:

        std::vector<std::vector<std::vector<double>>> heuristicValues_;
        std::vector<int> bin_;
        std::unique_ptr<GraphClass> graph_;
        std::vector<std::unique_ptr<NodeClass>> nodes_;
        double pheromoneInfluence_;
        double heuristicInfluence_;
        double evaporationParameter_;
        double localDecayParameter_;
        double initialPheromone_;
        double maxPheromone_;
        double minPheromone_;
        double bestSolution_;
        int stringLength_;
        int stateCount_;
        int nodeCount_;
        bool unique_;
        bool local_;

        void createNode(int ind1, int ind2)
        {            
            if constexpr (Heuristic)
            {
                nodes_.emplace_back(std::make_unique<NodeClass>(
                    typename NodeClass::PheromoneKey{}, stateCount_, pheromoneInfluence_, heuristicInfluence_,
                    initialPheromone_, evaporationParameter_, localDecayParameter_, heuristicValues_[ind1][ind2]));
            }
            else
            {
                nodes_.emplace_back(std::make_unique<NodeClass>(
                    typename NodeClass::PheromoneKey{}, stateCount_, pheromoneInfluence_, heuristicInfluence_,
                    initialPheromone_, evaporationParameter_, localDecayParameter_, std::vector<double>()));
            }            
        }

        void createGraph()
        {
            if constexpr (std::is_same_v<GraphClass, Graph<MultiStageGraph, NodeClass, IsUnique>>)
            {
                graph_ = std::make_unique<MultiStageGraph<NodeClass, IsUnique>>(typename GraphClass::SolverKey{},
                    stringLength_, stateCount_);
                nodeCount_ = graph_->getVertexCount(typename GraphClass::SolverKey{});
                nodes_.reserve(nodeCount_);

                createNode(0, 0);
                graph_->insertVertex(typename GraphClass::SolverKey{}, 0, 0, nodes_.back().get());

                for(int i = 1; i < stringLength_; i++)
                {
                    for(int j = 0; j < stateCount_; j++)
                    {
                        createNode(i, j);
                        graph_->insertVertex(typename GraphClass::SolverKey{}, i, j, nodes_.back().get());
                    }
                }
            }
            else if constexpr (std::is_same_v<GraphClass, Graph<SingleStageGraph, NodeClass, IsUnique>>)
            {
                graph_ = std::make_unique<SingleStageGraph<NodeClass, IsUnique>>(typename GraphClass::SolverKey{},
                    stringLength_, stateCount_);
                nodeCount_ = graph_->getVertexCount(typename GraphClass::SolverKey{});
                nodes_.reserve(nodeCount_);

                for(int i = 0; i < stateCount_ + 1; i++)
                {
                    createNode(0, i);
                    graph_->insertVertex(typename GraphClass::SolverKey{}, 0, i, nodes_.back().get());
                }
            }
        }
    };
}