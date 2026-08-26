#pragma once
#include <set>
#include "STILO/Structs/AnalyzerStructs/DistributionConfiguration.h"

namespace STILO
{
    struct WMCPConfigurationSet
    {
        std::set<int> vertexCounts;
        std::set<DistributionConfiguration> weightDistributions;
    };
    
}