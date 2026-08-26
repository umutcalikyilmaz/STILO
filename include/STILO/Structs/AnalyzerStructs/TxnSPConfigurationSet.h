#pragma once
#include <set>
#include "STILO/Structs/AnalyzerStructs/DistributionConfiguration.h"

namespace STILO
{
    struct TxnSPConfigurationSet
    {
        std::set<int> jobCounts;
        std::set<int> machineCounts;
        std::set<double> conflictPopensities;
        std::set<DistributionConfiguration> jobLengthDistributions;
    };
    
}