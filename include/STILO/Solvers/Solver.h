#pragma once
#include <random>
#include <time.h>
#include "STILO/Shared/Exceptions.h"
#include "STILO/Structs/SolverStructs/SolverInput.h"
#include "STILO/Structs/SolverStructs/SolverAnalysisInput.h"
#include "STILO/Structs/SolverStructs/SolverAnalysisOutput.h"

namespace STILO
{
    class Solver
    {
    public:

        virtual SolverOutput solve(const SolverInput& inp) = 0;

        virtual SolverAnalysisOutput analyze(const SolverAnalysisInput& inp) = 0;

    protected:
        
        Problem* prb;

        double getTime();
    };
}