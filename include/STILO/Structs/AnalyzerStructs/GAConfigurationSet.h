#pragma once
#include <set>
#include "STILO/Structs/SolverStructs/GAConfiguration.h"

namespace STILO
{
    struct CrossoverConfiguration
    {
        double cycleCrossoverCoefficient;
        double kpointCrossoverCoefficient;
        double OXCrossoverCoefficient;
        double PMXCrossoverCoefficient;
        double uniformCrossoverCoefficient;

        CrossoverConfiguration(double cycleCrossoverCoefficient, double kpointCrossoverCoefficient,
            double OXCrossoverCoefficient, double PMXCrossoverCoefficient, double uniformCrossoverCoefficient)
            : cycleCrossoverCoefficient(cycleCrossoverCoefficient),
              kpointCrossoverCoefficient(kpointCrossoverCoefficient),
              OXCrossoverCoefficient(OXCrossoverCoefficient),
              PMXCrossoverCoefficient(PMXCrossoverCoefficient),
              uniformCrossoverCoefficient(uniformCrossoverCoefficient) { }

        auto operator<=>(const CrossoverConfiguration&) const = default;
    };
    

    struct MutationConfiguration
    {
        double pointMutationCoefficient;
        double insertMutationCoefficient;
        double invertMutationCoefficient;
        double swapMutationCoefficient;

        MutationConfiguration(double pointMutationCoefficient, double insertMutationCoefficient,
            double invertMutationCoefficient, double swapMutationCoefficient)
            : pointMutationCoefficient(pointMutationCoefficient),
              insertMutationCoefficient(insertMutationCoefficient),
              invertMutationCoefficient(invertMutationCoefficient),
              swapMutationCoefficient(swapMutationCoefficient) { }

        auto operator<=>(const MutationConfiguration&) const = default;
    };
    

    struct GAConfigurationSet
    {
        std::set<double> mutationProbabilities;
        std::set<double> pointMutationProbabilities;   
        std::set<CrossoverConfiguration> crossoverConfigurations;
        std::set<MutationConfiguration> mutationConfigurations;
        std::set<int> populationSizes;
        std::set<int> matingCounts;
        std::set<int> eliteCounts; 
        std::set<int> kValues;
        std::set<int> tournamentSizes;
        std::set<SelectionOperator> selectionOperators;
    };
    
}