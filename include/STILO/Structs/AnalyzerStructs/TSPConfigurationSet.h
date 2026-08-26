#pragma once
#include <set>
#include "STILO/Structs/AnalyzerStructs/DistributionConfiguration.h"

namespace STILO
{
    struct TSPConfigurationSet
    {
        std::set<int> cityCounts;
        std::set<DistributionConfiguration> distanceDistributions;
    };
    
}