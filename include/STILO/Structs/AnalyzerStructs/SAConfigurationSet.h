#pragma once
#include <set>
#include <tuple>
#include "STILO/Structs/SolverStructs/SAConfiguration.h"

namespace STILO
{
    struct MoveConfiguration
    {
        double pointMoveCoefficient;
        double insertMoveCoefficient;
        double invertMoveCoefficient;
        double swapMoveCoefficient;

        MoveConfiguration(double pointMoveCoefficient, double insertMoveCoefficient,
            double invertMoveCoefficient, double swapMoveCoefficient)
            : pointMoveCoefficient(pointMoveCoefficient),
              insertMoveCoefficient(insertMoveCoefficient),
              invertMoveCoefficient(invertMoveCoefficient),
              swapMoveCoefficient(swapMoveCoefficient) { }

        auto operator<=>(const MoveConfiguration&) const = default;
    };
    

    struct SAConfigurationSet
    {
        std::set<MoveConfiguration> moveConfigurations;
        std::set<double> initialTemperatures;
        std::set<double> coolingParameters;
        std::set<int> minimumNeighborhoodSizes;
        std::set<int> maximumNeighborhoodSizes;
        std::set<EvolvingParameterType> coolingSchedules;
        std::set<DistanceCalculationOperator> distanceCalculationOperators;
    };
}