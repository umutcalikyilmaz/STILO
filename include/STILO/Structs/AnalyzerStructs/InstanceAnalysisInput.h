#pragma once
#include <chrono>
#include <set>
#include "STILO/Structs/AnalyzerStructs/ACOConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/GAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/SAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/SolverSelection.h"

namespace STILO
{
    struct InstanceAnalysisInput
    {
        std::set<std::chrono::milliseconds> timeLimits;
        SolverSelection solverSelection;
        ACOConfigurationSet ACOConfigurations;
        GAConfigurationSet GAConfigurations;
        SAConfigurationSet SAConfigurations;
        std::string instanceDirectory;
        int solutionCount;
        int threadCount;
        ProblemType problemType;
    };
}