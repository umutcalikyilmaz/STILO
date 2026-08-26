#pragma once
#include "STILO/Shared/Parameters.h"

namespace STILO
{
    template <EvolvingParameterType type>
    class EvolvingParameter
    {
    public:

        EvolvingParameter(double initialValue, double parameter)
            : value_(initialValue),
              initialValue_(initialValue),
              parameter_(parameter),
              count_(1) { }
        
        double getValue()
        {
            return value_;
        }

        void evolve()
        {
            if constexpr (type == EvolvingParameterType::Constant)
            {

            }
            else if constexpr (type == EvolvingParameterType::Exponential)
            {
                value_ *= parameter_;
            }
            else if constexpr (type == EvolvingParameterType::Slow)
            {
                value_ = initialValue_ / log2(1 + count_++);
            }
            if constexpr (type == EvolvingParameterType::Fast)
            {
                value_ = initialValue_ / count_++;
            }
        }

    private:

        double value_;
        double initialValue_;
        double parameter_;
        int count_;
    };    
}