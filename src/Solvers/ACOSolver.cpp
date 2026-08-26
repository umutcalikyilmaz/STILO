#include "STILO/Solvers/ACOSolver.h"

using namespace std::chrono_literals;

namespace STILO
{
    void ACOSolver::checkInput(Problem* problem, const ACOConfiguration& config)
    {
        int stateCount = problem->getStateCount();
        int stringLength = problem->getStringLength();
        std::vector<std::vector<std::vector<double>>> heuristic;

        if(problem->getSolutionSpaceType() == SolutionSpaceType::Permutation &&  stateCount < stringLength)
        {
            throw InvalidParameterException((char*)"Invalid solution space");
        }

        if(config.antCount <= 0)
        {
            throw InvalidParameterException((char*)"Ant count must be greater than 0");
        }

        if(config.eliteCount > config.antCount && (config.tourSelectionOperator == TourSelectionOperator::GlobalBest || config.tourSelectionOperator == TourSelectionOperator::IterationBest))
        {
            throw InvalidParameterException((char*)"Elite count cannot be greater than ant count");
        }

        if(config.pheromoneInfluence < 0)
        {
            throw InvalidParameterException((char*)"Pheromone influence cannot be less than 0");
        }

        if(config.heuristicInfluence < 0)
        {
            throw InvalidParameterException((char*)"Heuristic influence cannot be less than 0");
        }

        if(config.evaporationParameter < 0 || config.evaporationParameter > 1)
        {
            throw InvalidParameterException((char*)"Evaporation parameter must be in the range [0,1]");
        }

        if(config.localDecayParameter < 0 || config.localDecayParameter > 1)
        {
            throw InvalidParameterException((char*)"Local decay parameter must be in the range [0,1]");
        }

        if(config.pheromoneConstant <= 0)
        {
            throw InvalidParameterException((char*)"Pheromone constant must be greater than 0");
        }

        if(config.initialPheromone < 0)
        {
            throw InvalidParameterException((char*)"Initial pheromone cannot be less than 0");
        }
    }

    std::unique_ptr<ACOIteratorWrapper> ACOSolver::createIterator(GraphStructure graphStructure,
        SolutionSpaceType solutionSpaceType, PheromoneUpdateOperator pheromoneUpdate, 
        TourSelectionOperator tourSelection, PheromoneCalculationOperator pheromoneCalculation,
        Problem* problem, int stateCount, int stringLength, int antCount, int eliteCount,
        double pheromoneConstant, double pheromoneInfluence, double heuristicInfluence,
        double initialPheromone, double evaporationParameter,
        const std::vector<std::vector<std::vector<double>>>& heuristicValues, double localDecayParameter)
    {
        bool isUnique = solutionSpaceType == SolutionSpaceType::Permutation;
        bool localUpdate = localDecayParameter > 0;
        bool heuristic = heuristicInfluence > 0;

        return dispatchBool(isUnique, [&]<bool kIsUnique>() -> std::unique_ptr<ACOIteratorWrapper>
        {
            return dispatchBool(localUpdate, [&]<bool kLocalUpdate>() -> std::unique_ptr<ACOIteratorWrapper>
            {
                return dispatchBool(heuristic, [&]<bool kHeuristic>() -> std::unique_ptr<ACOIteratorWrapper>
                {
                    return dispatchGraph(graphStructure, [&]<template <class, bool> class GraphType>() -> std::unique_ptr<ACOIteratorWrapper>
                    {
                        return dispatchTourSelectionOperator(tourSelection, [&]<TourSelectionOperator kTourSelection>() -> std::unique_ptr<ACOIteratorWrapper>
                        {
                            return dispatchPheromoneCalculationOperator(pheromoneCalculation, [&]<PheromoneCalculationOperator kPheromoneCalculation>() -> std::unique_ptr<ACOIteratorWrapper> 
                            {
                                return dispatchPheromoneUpdateOperator(pheromoneUpdate, [&]<PheromoneUpdateOperator kPheromoneUpdate>() -> std::unique_ptr<ACOIteratorWrapper>
                                {
                                    return std::make_unique<ACOIterator<GraphType, kIsUnique, kPheromoneUpdate, kLocalUpdate, kHeuristic, kTourSelection, kPheromoneCalculation>>(typename ACOIteratorWrapper::ACOSolverKey{}, problem, stateCount, stringLength, antCount, eliteCount, pheromoneConstant, pheromoneInfluence, heuristicInfluence, initialPheromone, evaporationParameter, heuristicValues, localDecayParameter);
                                });
                            });
                        });
                    });
                });
            });
        });
    }
    
    SolverOutput ACOSolver::solve(const SolverInput& input)
    {     
        double beginning = getTime();
        double averageTime = 0;
        double totalTime = 0;

        checkInput(input.problem, input.ACOConfig);

        std::vector<std::vector<std::vector<double>>> heuristic;

        if(input.ACOConfig.heuristicInfluence > 0)
        {
            input.problem->getHeuristicValues(input.ACOConfig.graphStructure, heuristic);
        }
        
        auto iterator = createIterator(input.ACOConfig.graphStructure, input.problem->getSolutionSpaceType(),
            input.ACOConfig.pheromoneUpdateOperator, input.ACOConfig.tourSelectionOperator,
            input.ACOConfig.pheromoneCalculationOperator, input.problem, input.problem->getStateCount(),
            input.problem->getStringLength(), input.ACOConfig.antCount, input.ACOConfig.eliteCount,
            input.ACOConfig.pheromoneConstant, input.ACOConfig.pheromoneInfluence, input.ACOConfig.heuristicInfluence,
            input.ACOConfig.initialPheromone, input.ACOConfig.evaporationParameter, heuristic,
            input.ACOConfig.localDecayParameter);

        int c = 0;

        while(true)
        {
            iterator->iterate(typename ACOIteratorWrapper::ACOSolverKey{});
            totalTime = getTime() - beginning;
            averageTime = totalTime / ++c;
            
            if(totalTime + averageTime > input.timeLimit.count())
            {
                break;
            }
        }

        auto res = iterator->getOutput(typename ACOIteratorWrapper::ACOSolverKey{});
        res.executionTime = totalTime * 1ms;
        iterator = nullptr;
        return res;
    }

    SolverAnalysisOutput ACOSolver::analyze(const SolverAnalysisInput& input)
    {
        double beginning = getTime();
        double totalTime = 0;

        checkInput(input.problem, input.ACOConfig);

        std::vector<std::vector<std::vector<double>>> heuristic;

        if(input.ACOConfig.heuristicInfluence > 0)
        {
            input.problem->getHeuristicValues(input.ACOConfig.graphStructure, heuristic);
        }
        
        auto iterator = createIterator(input.ACOConfig.graphStructure, input.problem->getSolutionSpaceType(),
            input.ACOConfig.pheromoneUpdateOperator, input.ACOConfig.tourSelectionOperator,
            input.ACOConfig.pheromoneCalculationOperator, input.problem, input.problem->getStateCount(),
            input.problem->getStringLength(), input.ACOConfig.antCount, input.ACOConfig.eliteCount,
            input.ACOConfig.pheromoneConstant, input.ACOConfig.pheromoneInfluence, input.ACOConfig.heuristicInfluence,
            input.ACOConfig.initialPheromone, input.ACOConfig.evaporationParameter, heuristic,
            input.ACOConfig.localDecayParameter);

        auto it = input.timeLimits.begin();
        SolverAnalysisOutput res;
        SolverOutput previousResult = iterator->getOutput(typename ACOIteratorWrapper::ACOSolverKey{});
        std::vector<std::unique_ptr<SolverOutput>> results;
        bool exit = false;
        int c = 0;
        double wasteTime = 0;

        while(true)
        {
            iterator->iterate(typename ACOIteratorWrapper::ACOSolverKey{});
            totalTime = getTime() - beginning + wasteTime;  
            
            double wasteBeg = getTime();

            while(totalTime > it->count())
            {
                if(c > 0)
                {
                    res.results[*it] = std::make_unique<SolverOutput>(previousResult);                    
                }
                else
                {
                    res.results[*it] = nullptr;
                }
                
                it++;

                if(it == input.timeLimits.end())
                {
                    exit = true;
                    break;
                }
            }

            if(exit)
            {
                break;
            }

            previousResult = iterator->getOutput(typename ACOIteratorWrapper::ACOSolverKey{});
            previousResult.executionTime = totalTime * 1ms;
            c++;

            wasteTime += getTime() - wasteBeg;
        }
        
        iterator = nullptr;
        return res;
    }
}