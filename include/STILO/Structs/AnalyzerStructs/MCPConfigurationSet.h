#pragma once
#include <set>

namespace STILO
{
    struct MCPConfigurationSet
    {
        std::set<int> vertexCounts;
        std::set<double> edgePropensities;
    };
    
}