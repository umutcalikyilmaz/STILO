#pragma once
#include "STILO/SolverModels/ACOModels/Pheromone.h"
#include "STILO/ProblemModels/Problem.h"
#include "STILO/Structs/SolverStructs/SolverOutput.h"

namespace STILO
{
    class ACOIteratorWrapper
    {
    public:

        class ACOSolverKey
        {
        private:
            
            ACOSolverKey() { }

            friend class ACOSolver;
        };

        virtual void iterate(ACOSolverKey) = 0;
        
        virtual SolverOutput getOutput(ACOSolverKey) = 0;

        virtual ~ACOIteratorWrapper() = default;
    };
}