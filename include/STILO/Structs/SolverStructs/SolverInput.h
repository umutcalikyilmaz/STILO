#pragma once
#include <memory>
#include "STILO/Structs/SolverStructs/ACOConfiguration.h"
#include "STILO/Structs/SolverStructs/GAConfiguration.h"
#include "STILO/Structs/SolverStructs/SAConfiguration.h"
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    struct SolverInput
    {
        Problem* problem;
        ACOConfiguration ACOConfig;
        GAConfiguration GAConfig;
        SAConfiguration SAConfig;
        std::chrono::milliseconds timeLimit;
    };    
}