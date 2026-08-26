#pragma once
#include "STILO/Shared/Parameters.h"

namespace STILO
{
    struct GAConfiguration
    {
        double mutationProbability;
        double pointMutationProbability;
        double pointMutationCoefficient;
        double insertMutationCoefficient;
        double invertMutationCoefficient;
        double swapMutationCoefficient;
        double cycleCrossoverCoefficient;
        double kpointCrossoverCoefficient;
        double OXCrossoverCoefficient;
        double PMXCrossoverCoefficient;
        double uniformCrossoverCoefficient;
        int populationSize;
        int matingCount;
        int eliteCount; 
        int k;
        int tournamentSize;
        SelectionOperator selectionOperator;
    };
    
}