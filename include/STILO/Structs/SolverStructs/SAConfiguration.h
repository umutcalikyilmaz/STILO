#pragma once
#include "STILO/Shared/Parameters.h"

namespace STILO
{
    struct SAConfiguration
    {
        double pointMoveCoefficient;
        double insertMoveCoefficient;
        double invertMoveCoefficient;
        double swapMoveCoefficient;
        double initialTemperature;
        double coolingParameter;
        int minimumNeighborhoodSize;
        int maximumNeighborhoodSize;
        EvolvingParameterType coolingSchedule;
        DistanceCalculationOperator distanceCalculationOperator;
    };
}