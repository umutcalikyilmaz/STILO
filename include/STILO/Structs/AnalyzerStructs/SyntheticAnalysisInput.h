#pragma once
#include <chrono>
#include <set>
#include "STILO/Structs/AnalyzerStructs/IMSPConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/MCPConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/TSPConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/TxnSPConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/WMCPConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/ACOConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/GAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/SAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/ProblemSelection.h"
#include "STILO/Structs/AnalyzerStructs/SolverSelection.h"

namespace STILO
{
    struct SyntheticAnalysisInput
    {
        std::set<std::chrono::milliseconds> timeLimits;
        ProblemSelection problemSelection;
        IMSPConfigurationSet IMSPconfigurations;
        MCPConfigurationSet MCPConfigurations;
        TSPConfigurationSet TSPConfigurations;
        TxnSPConfigurationSet TxnSPConfigurations;
        WMCPConfigurationSet WMCPConfigurations;
        SolverSelection solverSelection;
        ACOConfigurationSet ACOConfigurations;
        GAConfigurationSet GAConfigurations;
        SAConfigurationSet SAConfigurations;
        int problemCount;
        int threadCount;
        bool useExisting;
    };
}