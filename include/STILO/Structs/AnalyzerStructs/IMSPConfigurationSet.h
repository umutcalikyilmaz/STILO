#pragma once
#include <set>
#include "STILO/Structs/AnalyzerStructs/DistributionConfiguration.h"

namespace STILO
{
    struct IMSPConfigurationSet
    {
        std::set<int> jobCounts;
        std::set<int> machineCounts;
        std::set<DistributionConfiguration> jobLengthDistributions;
    };
    
}