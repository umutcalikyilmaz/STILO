#pragma once
#include "STILO/Shared/Parameters.h"
#include "STILO/Shared/Random.h"

namespace STILO
{
    template <DistanceCalculationOperator distanceCalculation>
    class DistanceCalculator;
    
    template <>
    class DistanceCalculator<DistanceCalculationOperator::Cauchy>
    {
    private: 
        CauchyRandomIntGenerator rnd_;
        
        DistanceCalculator(int min, int max)
            : rnd_(0, 1, min, max) { }
        
        int calculateDistance(double T)
        {
            rnd_.changeGamma(T);
            return rnd_.generate();
        }
        
        template<
            EvolvingParameterType Cooling,
            DistanceCalculationOperator DistanceCalculation>
        friend class SAIterator;
    };

    template<>
    class DistanceCalculator<DistanceCalculationOperator::Constant>
    {
    private:

        int constantValue_;

        DistanceCalculator(int min, int max)
            : constantValue_(min) { }

        int calculateDistance()
        {
            return constantValue_;
        }
        
        template<
            EvolvingParameterType Cooling,
            DistanceCalculationOperator DistanceCalculation>
        friend class SAIterator;
    };

    template<>
    class DistanceCalculator<DistanceCalculationOperator::Gaussian>
    {
    private:

        NormalRandomIntGenerator rnd_;

        DistanceCalculator(int min, int max)
            : rnd_(0, 1, min, max) { }

        int calculateDistance(double T)
        {
            rnd_.changeStd(T);
            return rnd_.generate();
        }
        
        template<
            EvolvingParameterType Cooling,
            DistanceCalculationOperator DistanceCalculation>
        friend class SAIterator;
    };

    template<>
    class DistanceCalculator<DistanceCalculationOperator::Uniform>
    {
    private:

        UniformRandomIntGenerator rnd_;

        DistanceCalculator(int min, int max)
            : rnd_(min, max) { }

        int calculateDistance()
        {
            return rnd_.generate();
        }
        
        template<
            EvolvingParameterType Cooling,
            DistanceCalculationOperator DistanceCalculation>
        friend class SAIterator;
    };
}