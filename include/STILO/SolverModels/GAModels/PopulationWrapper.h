#pragma once
#include "STILO/Structs/SolverStructs/SolverOutput.h"

namespace STILO
{
    class PopulationWrapper
    {
    public:

        class GASolverKey
        {
        private:

            GASolverKey() { }

            friend class GASolver;
        };

        virtual void evolve(GASolverKey) = 0;

        virtual SolverOutput getOutput(GASolverKey) = 0;

        virtual ~PopulationWrapper() = default;
    };
}
