#pragma once
#include <set>
#include "STILO/Structs/SolverStructs/ACOConfiguration.h"

namespace STILO
{
    struct ACOConfigurationSet
    {
        std::set<double> pheromoneInfluences;
        std::set<double> heuristicInfluences;
        std::set<double> evaporationParameters;
        std::set<double> localDecayParameters;
        std::set<double> pheromoneConstants;
        std::set<double> initialPheromones;
        std::set<int> antCounts;
        std::set<int> eliteCounts;
        std::set<GraphStructure> graphStructures;
        std::set<PheromoneUpdateOperator> pheromoneUpdateOperators;
        std::set<PheromoneCalculationOperator> pheromoneCalculationOperators;
        std::set<TourSelectionOperator> tourSelectionOperators;
    };
    
}