#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    Problem::Problem(const Problem& original)
        : stateCount(original.stateCount),
          stringLength(original.stringLength),
          solutionSpaceType(original.solutionSpaceType),
          pstate(original.stringLength)
    {
        if(solutionSpaceType == SolutionSpaceType::Permutation)
        {
            slist.resize(stringLength);
        }
    }

    Problem::Problem(int stringLength, int stateCount, SolutionSpaceType solutionSpaceType) 
        : stringLength(stringLength),
          stateCount(stateCount),
          solutionSpaceType(solutionSpaceType)
    {
        if(solutionSpaceType == SolutionSpaceType::Permutation)
        {
            pstate.resize(stringLength);
            slist.resize(stringLength);
        }
        else if(solutionSpaceType == SolutionSpaceType::Vector)
        {
            pstate.resize(stringLength);
        }
    }

    int Problem::getStringLength() 
    {
        return stringLength;
    }

    int Problem::getStateCount() 
    {
        return stateCount;
    }

    SolutionSpaceType Problem::getSolutionSpaceType()
    {
        return solutionSpaceType;
    }
    void Problem::getHeuristicValues(GraphStructure gt, std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        switch (gt)
        {
        case GraphStructure::MultiStage:
            return getHeuristicValuesMultiStage(heuristic);

        case GraphStructure::SingleStage:
            return getHeuristicValuesSingleStage(heuristic);
        }
    }
}