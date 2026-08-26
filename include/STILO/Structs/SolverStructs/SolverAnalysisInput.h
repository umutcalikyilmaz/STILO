#pragma once
#include <memory>
#include <set>
#include "STILO/Structs/SolverStructs/ACOConfiguration.h"
#include "STILO/Structs/SolverStructs/GAConfiguration.h"
#include "STILO/Structs/SolverStructs/SAConfiguration.h"
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    struct SolverAnalysisInput
    {
        Problem* problem;
        ACOConfiguration ACOConfig;
        GAConfiguration GAConfig;
        SAConfiguration SAConfig;
        std::set<std::chrono::milliseconds> timeLimits;
    };    
}