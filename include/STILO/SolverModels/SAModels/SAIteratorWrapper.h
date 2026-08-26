#pragma once
#include "STILO/ProblemModels/Problem.h"
#include "STILO/Shared/Parameters.h"
#include "STILO/Shared/EvolvingParameter.h"
#include "STILO/SolverModels/SAModels/DistanceCalculator.h"

namespace STILO
{
    class SAIteratorWrapper
    {
    public:

        class SASolverKey
        {
        private:

            SASolverKey() { }

            friend class SASolver;
        };
    
        virtual void iterate(SASolverKey) = 0;

        virtual SolverOutput getOutput(SASolverKey) = 0;

        virtual ~SAIteratorWrapper() = default;
    };
}