#include "STILO/ProblemModels/WeightedMaxCutProblem.h"

namespace STILO
{
    void WeightedMaxCutProblem::getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        calculateMinimumValue();
        double avg = 0;
        double denom = 0;
        double max;
        double scale;

        for(int i = 0; i < stringLength - 1; i++)
        {
            denom += i + 1;

            for(int j = 0; j <= i; j++)
            {
                avg += (weights_[i][j] - minValue_);
            }
        }

        avg = avg / denom;

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
                max = 0;

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][k] = (j == k) ? avg / (weights_[i - 1][i - 1] - minValue_ + 1) : (weights_[i - 1][i - 1] - minValue_ + 1) / avg;
                    
                    if(heuristic[i][j][k] > max)
                    {
                        max = heuristic[i][j][k];
                    }
                }

                scale = 1 / max;

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][k] *= scale;
                }
            }
        }
    }

    void WeightedMaxCutProblem::getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        calculateMinimumValue();
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

    void WeightedMaxCutProblem::calculateMinimumValue()
    {
        minValue_ = DBL_MAX;

        for(int i = 0; i < stringLength - 1; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                if(weights_[i][j] < minValue_)
                {
                    minValue_ = weights_[i][j];
                }
            }
        }
    }

    WeightedMaxCutProblem::WeightedMaxCutProblem(const WeightedMaxCutProblem& original)
        : Problem(original),
          weights_(original.weights_),
          maxWeight_(original.maxWeight_) { }

    WeightedMaxCutProblem::WeightedMaxCutProblem(int verticeCount,
        const std::vector<std::vector<double>>& weights)
        : Problem(verticeCount, 2, SolutionSpaceType::Vector),
          weights_(weights),
          maxWeight_(0)
    {
        for(int i = 0; i < stringLength - 1; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                maxWeight_ += weights[i][j];
            }
        }
    }

    WeightedMaxCutProblem::WeightedMaxCutProblem(int verticeNum, ProbabilityDistribution weightDistribution,
        double distributionParameter1, double distributionParameter2)
        : Problem(verticeNum, 2, SolutionSpaceType::Vector),
          weights_(verticeNum - 1),
          maxWeight_(0)
    {
        if(weightDistribution == ProbabilityDistribution::Normal)
        {
            NormalRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

            for(int i = 0; i < stringLength - 1; i++)
            {
                weights_[i].resize(i + 1);
    
                for(int j = 0; j <= i; j++)
                {
                    weights_[i][j] = rnd.generate();
                    maxWeight_ += weights_[i][j];
                }
            }
        }
        else
        {
            UniformRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

            for(int i = 0; i < stringLength - 1; i++)
            {
                weights_[i].resize(i + 1);
    
                for(int j = 0; j <= i; j++)
                {
                    weights_[i][j] = rnd.generate();
                    maxWeight_ += weights_[i][j];
                }
            }
        }
    }

    double WeightedMaxCutProblem::getCost(std::span<const int> solution)
    {
        double cut = 0;

        for(int i = 0; i < stringLength; i++)
        {
            if(solution[i] >= stateCount)
            {
                return maxWeight_;
            }
        }        

        for(int i = 0; i < stringLength - 1; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                if(solution[i + 1] != solution[j])
                {
                    cut += weights_[i][j];
                }                
            }
        }

        return maxWeight_ - cut;
    }

    double WeightedMaxCutProblem::getValue(double cost)
    {
        return maxWeight_ - cost;
    }

    double WeightedMaxCutProblem::getScaleFactor()
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

        double cost = getCost(randState);
        return cost > 0 ? cost : 1;
    }
}