#pragma once
#include "STILO/Structs/SolverStructs/SolverAnalysisInput.h"

namespace STILO
{
    struct Job
    {
        SolverType solverType;
        SolverAnalysisInput input;
    };
}