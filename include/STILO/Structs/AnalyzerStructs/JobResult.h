#pragma once
#include <chrono>
#include <map>
#include "STILO/Structs/SolverStructs/SolverAnalysisInput.h"

namespace STILO
{
    struct JobOutput
    {
        double averageTime;
        double averageCost;
        double averageIterationCount;
        double successRate;
    };
    
    struct JobResult
    {
        SolverType solverType;
        SolverAnalysisInput input;
        std::map<std::chrono::milliseconds, JobOutput> results;
    };   
}