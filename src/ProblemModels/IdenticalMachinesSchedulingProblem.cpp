#include "STILO/ProblemModels/IdenticalMachinesSchedulingProblem.h"

namespace STILO
{
    void IdenticalMachinesSchedulingProblem::getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(stringLength);
        heuristic[0].resize(1);
        heuristic[0][0].resize(stateCount);
        heuristic[0][0][0] = 1;

        for(int i = 1; i < stateCount; i++)
        {
            heuristic[0][0][i] = 0;
        }

        double avg = 0;

        for(int i = 0; i < stringLength; i++)
        {
            for(int j = 0; j < i; j++)
            {
                avg += abs(lengths_[i] - lengths_[j]);
            }
        }

        avg /= (stringLength * (stringLength - 1) / 2);

        for(int i = 1; i < stateCount; i++)
        {
            heuristic[i].resize(stateCount);

            for(int j = 0; j < i; j++)
            {
                heuristic[i][j].resize(stateCount);
                double max = 0;

                for(int k = 0; k <= i; k++)
                {
                    if(j == k)
                    {
                        heuristic[i][j][k] =(lengths_[i - 1] - lengths_[i] == 0 ? 0.1 : abs(lengths_[i - 1] - lengths_[i]) / avg);
                    }
                    else
                    {
                        heuristic[i][j][k] = (lengths_[i - 1] - lengths_[i] == 0 ? 1 : avg / abs(lengths_[i - 1] - lengths_[i]));
                    }
                    
                    if(max < heuristic[i][j][k])
                    {
                        max = heuristic[i][j][k];
                    }
                }

                for(int k = 0; k <= i; k++)
                {
                    heuristic[i][j][j] /= max;
                }

                for(int k = i + 1; k < stateCount; k++)
                {
                    heuristic[i][j][k] = 0;
                }
            }

            for(int j = i; j < stateCount; j++)
            {
                heuristic[i][j].resize(stateCount);                

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][k] = 0;
                }
            }
        }

        for(int i = stateCount; i < stringLength; i++)
        {
            heuristic[i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[i][j].resize(stateCount);
                double max = 0;

                for(int k = 0; k < stateCount; k++)
                {
                    if(j == k)
                    {
                        heuristic[i][j][k] =(lengths_[i - 1] - lengths_[i] == 0 ? 0.1 : abs(lengths_[i - 1] - lengths_[i]) / avg);
                    }
                    else
                    {
                        heuristic[i][j][k] = (lengths_[i - 1] - lengths_[i] == 0 ? 1 : avg / abs(lengths_[i - 1] - lengths_[i]));
                    }
                    
                    if(max < heuristic[i][j][k])
                    {
                        max = heuristic[i][j][k];
                    }
                }

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][j] /= max;
                }
            }
        }
    }

    void IdenticalMachinesSchedulingProblem::getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(1);
        heuristic[0].resize(stateCount + 1);
        heuristic[0][0].resize(stateCount);
        heuristic[0][0][0] = 1;

        for(int i = 1; i < stateCount; i++)
        {
            heuristic[0][0][i] = 0;
        }

        for(int i = 1; i < stateCount + 1; i++)
        {
            heuristic[0][i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[0][i][j] = 1;
            }
        }
    }
    
    IdenticalMachinesSchedulingProblem::IdenticalMachinesSchedulingProblem(const IdenticalMachinesSchedulingProblem& original)
        : Problem(original),
          lengths_(original.lengths_),       
          maxCost_(original.maxCost_),
          times_(original.stateCount, 0) { }

    IdenticalMachinesSchedulingProblem::IdenticalMachinesSchedulingProblem(int jobCount, int machineCount,
        std::vector<double>& lengths) 
        : Problem(jobCount, machineCount, SolutionSpaceType::Vector),
          lengths_(lengths),  
          times_(machineCount, 0),
          maxCost_(0)
    {
        for(int i = 0; i < stringLength; i++)
        {
            maxCost_ += lengths[i];
        }
    }

    IdenticalMachinesSchedulingProblem::IdenticalMachinesSchedulingProblem(int jobCount, int machineCount,
        ProbabilityDistribution distribution, double distributionParameter1, double distributionParameter2)
        : Problem(jobCount, machineCount, SolutionSpaceType::Vector),
          lengths_(jobCount),
          times_(machineCount, 0),
          maxCost_(0)
    {
        maxCost_ = 0;

        if (distribution == ProbabilityDistribution::Normal)
        {
            NormalRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

            for (int i = 0; i < stringLength; i++)
            {                
                lengths_[i] = rnd.generate();
                maxCost_ += lengths_[i];
            }
        }
        else if (distribution == ProbabilityDistribution::Uniform)
        {
            UniformRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

            for (int i = 0; i < stringLength; i++)
            {                
                lengths_[i] =  rnd.generate();
                maxCost_ += lengths_[i];
            }
        }
    }

    double IdenticalMachinesSchedulingProblem::getCost(std::span<const int> solution)
    {
        for(int i = 0; i < stringLength; i++)
        {
            if(solution[i] >= stateCount)
            {
                return maxCost_;
            }

            times_[solution[i]] += lengths_[i];
        }

        double cost = 0;

        for(int i = 0; i < stateCount; i++)
        {
            if(times_[i] > cost)
            {
                cost = times_[i];
            }

            times_[i] = 0;
        }

        return cost;
    }

    double IdenticalMachinesSchedulingProblem::getValue(double cost)
    {
        return maxCost_ - cost;
    }

    double IdenticalMachinesSchedulingProblem::getScaleFactor()
    {
        double* pt = new double[stateCount];

        for(int i = 0; i < stateCount; i++)
        {
            pt[i] = 0;
        }

        for(int i = 0; i < stringLength; i++)
        {
            double best = pt[0];
            int ind = 0;

            for(int j = 1; j < stateCount; j++)
            {
                if(best > pt[j])
                {
                    best = pt[j];
                    ind = j;
                }
            }

            pt[ind] += lengths_[i];
        }

        double cost = 0;

        for(int i = 0; i < stateCount; i++)
        {
            if(pt[i] > cost)
            {
                cost = pt[i];
            }
        }

        delete[] pt;
        return cost;
    }
}