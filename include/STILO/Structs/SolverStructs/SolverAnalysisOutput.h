#pragma once
#include <map>
#include "STILO/Structs/SolverStructs/SolverOutput.h"

namespace STILO
{
    struct SolverAnalysisOutput
    {
        std::map<std::chrono::milliseconds, std::unique_ptr<SolverOutput>> results;
    };    
}