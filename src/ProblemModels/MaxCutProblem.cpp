#include "STILO/ProblemModels/MaxCutProblem.h"

namespace STILO
{
    void MaxCutProblem::getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(stringLength);
        heuristic[0].resize(1);
        heuristic[0][0].resize(stateCount);
        heuristic[0][0][0] = 1;
        heuristic[0][0][1] = 0;

        for(int i = 1; i < stringLength; i++)
        {
            heuristic[i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[i][j].resize(stateCount);

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][k] = ((j == k && edges_[i - 1][i - 1]) || (j != k && !edges_[i - 1][i - 1])) ? 0.5 : 1;                   
                }
            }
        }
    }

    void MaxCutProblem::getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(1);
        heuristic[0].resize(stateCount + 1);
        heuristic[0][0].resize(stateCount);
        heuristic[0][0][0] = 1;
        heuristic[0][0][1] = 0;

        for(int i = 1; i < stateCount + 1; i++)
        {
            heuristic[0][i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[0][i][j] = 1;
            }
        }
    }

    MaxCutProblem::MaxCutProblem(const MaxCutProblem& original)
        : Problem(original),
          maxEdge_(original.maxEdge_),
          edges_(original.edges_) { }

    MaxCutProblem::MaxCutProblem(int verticeCount, const std::vector<std::vector<uint8_t>>& edges)
        : Problem(verticeCount, 2, SolutionSpaceType::Vector),
          edges_(edges),
          maxEdge_(0)
    {
        for(int i = 0; i < stringLength - 1; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                if(edges_[i][j])
                {
                    maxEdge_++;
                }                
            }
        }
    }

    MaxCutProblem::MaxCutProblem(int verticeCount, double edgeProb)
        : Problem(verticeCount, 2, SolutionSpaceType::Vector),
          maxEdge_(0),
          edges_(stringLength - 1)
    {
        double p;
        UniformRandomDoubleGenerator rnd(0, 1);

        for(int i = 0; i < stringLength - 1; i++)
        {
            edges_[i].resize(i + 1);

            for(int j = 0; j <= i; j++)
            {
                p = rnd.generate();
                edges_[i][j] = p < edgeProb ? 1 : 0;

                if(edges_[i][j])
                {
                    maxEdge_++;
                }
            }
        }
    }

    double MaxCutProblem::getCost(std::span<const int> solution)
    {
        double cut = 0;

        for(int i = 0; i < stringLength; i++)
        {
            if(solution[i] >= stateCount)
            {
                return maxEdge_;
            }
        }

        for(int i = 0; i < stringLength - 1; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                if(edges_[i][j] && solution[i + 1] != solution[j])
                {
                    cut++;
                }                
            }
        }

        return maxEdge_ - cut;
    }

    double MaxCutProblem::getValue(double cost)
    {
        return maxEdge_ - cost;
    }

    double MaxCutProblem::getScaleFactor()
    {
        std::vector<int> randState(stringLength);

        for(int i = 0; i < stringLength / 2; i++)
        {
            randState[i] = 0;
        }

        for(int i = stringLength / 2; i < stringLength; i++)
        {
            randState[i] = 1;
        }

        return getCost(randState);
    }
}