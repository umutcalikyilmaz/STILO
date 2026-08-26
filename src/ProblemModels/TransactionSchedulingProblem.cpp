#include "STILO/ProblemModels/TransactionSchedulingProblem.h"

namespace STILO
{
    void TransactionSchedulingProblem::getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(stringLength);
		heuristic[0].resize(1);
		heuristic[0][0].resize(stateCount);

        for(int i = 0; i < stateCount; i++)
        {
            heuristic[0][0][i] = 1;
        }

        for(int i = 1; i < stringLength; i++)
        {
            heuristic[i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[i][j].resize(stateCount);

                for(int k = 0; k < stateCount; k++)
                {
                    heuristic[i][j][k] = (j == k) ? 0 : (conflicts_[j][k] ? 0.5 : 1);
                }
            }
        }
    }

    void TransactionSchedulingProblem::getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic)
    {
        heuristic.resize(1);
        heuristic[0].resize(stateCount + 1);
        heuristic[0][0].resize(stateCount);

        for(int i = 0; i < stateCount; i++)
        {
            heuristic[0][0][i] = 1;
        }

        for(int i = 1; i < stateCount + 1; i++)
        {
            heuristic[0][i].resize(stateCount);

            for(int j = 0; j < stateCount; j++)
            {
                heuristic[0][i][j] = conflicts_[i - 1][j] ? 0.5 : 1; 
            }
        }
    }

    TransactionSchedulingProblem::TransactionSchedulingProblem(const TransactionSchedulingProblem& original)
        : Problem(original),
          machineCount_(original.machineCount_),
          lengths_(original.lengths_),
          conflicts_(original.conflicts_),
          processingTimes_(original.machineCount_),
          lastJobs_(original.machineCount_),
          machineIndices_(original.machineCount_),
          blist_(original.stateCount) { }

    TransactionSchedulingProblem::TransactionSchedulingProblem(int jobCount, int machineCount,
        const std::vector<double>& lengths, const Matrix<uint8_t>& conflicts)
        : Problem(jobCount, jobCount, SolutionSpaceType::Permutation),
          machineCount_(machineCount),
          lengths_(lengths),
          conflicts_(conflicts),
          processingTimes_(machineCount),
          lastJobs_(machineCount),
          machineIndices_(machineCount),
          blist_(jobCount) { }

    TransactionSchedulingProblem::TransactionSchedulingProblem(int jobCount, int machineCount, 
        double conflictPropensity, ProbabilityDistribution lengthDistribution, double distributionParameter1,
        double distributionParameter2)
        : Problem(jobCount, jobCount, SolutionSpaceType::Permutation),
          machineCount_(machineCount),
          lengths_(jobCount),
          conflicts_(jobCount, jobCount),
          processingTimes_(machineCount),
          lastJobs_(machineCount),
          machineIndices_(machineCount),
          blist_(jobCount)
    {
        UniformRandomDoubleGenerator rnd(0, 1);

        if (lengthDistribution == ProbabilityDistribution::Normal) 
		{
			NormalRandomDoubleGenerator lrnd(distributionParameter1, distributionParameter2);

            for (int i = 0; i < stateCount; i++)
            {
                lengths_[i] = lrnd.generate();
                conflicts_[i][i] = false;

                for (int j = 0; j < i; j++)
                {
                    double p = rnd.generate();
                    conflicts_[i][j] = p < conflictPropensity ? 1 : 0;
                }
            }
		}
		else
		{
			UniformRandomDoubleGenerator lrnd(distributionParameter1, distributionParameter2);

            for (int i = 0; i < stateCount; i++)
            {
                lengths_[i] = lrnd.generate();
                conflicts_[i][i] = false;

                for (int j = 0; j < i; j++)
                {
                    double p = rnd.generate();
                    conflicts_[i][j] = p < conflictPropensity ? 1 : 0;
                }
            }
		}
		
		for (int i = 0; i < stateCount; i++)
		{
			for (int j = i + 1; j < stateCount; j++)
			{
				conflicts_[i][j] = conflicts_[j][i];
			}
		}
    }

    double TransactionSchedulingProblem::getCost(std::span<const int> solution)
    {
		for (int i = 0; i < stateCount; i++)
		{
			blist_[i] = false;
		}

        for (int i = 0; i < stateCount; i++)
		{
			if (solution[i] >= stateCount)
			{
				return DBL_MAX;
			}

			if (blist_[solution[i]])
			{
				return DBL_MAX;
			}

            blist_[solution[i]] = true;	
		}

        for(int i = 0; i < machineCount_; i++)
        {
            int job = solution[i];
            double temp = 0;

            for(int j = 0; j < i; j++)
            {
                if(conflicts_[job][lastJobs_[j]] && temp < processingTimes_[j])
                {
                    temp = processingTimes_[j];
                }
            }

            lastJobs_[i] = job;
            processingTimes_[i] = temp + lengths_[job];
            int ind = findPlace2(processingTimes_, machineIndices_, processingTimes_[i], i);
            shiftRightPartial<int>(machineIndices_, ind, i);
            machineIndices_[ind] = i;
        }

        for(int i = machineCount_; i < stringLength; i++)
        {
            int job = solution[i];
            double temp = processingTimes_[machineIndices_[0]];

            for(int j = machineCount_ - 1; j > 0; j--)
            {
                if(conflicts_[job][lastJobs_[machineIndices_[j]]])
                {
                    temp = processingTimes_[machineIndices_[j]];
                    break;
                }
            }

            lastJobs_[machineIndices_[0]] = job;
            processingTimes_[machineIndices_[0]] = temp + lengths_[job];
            int tempind = machineIndices_[0];
            int ind = findPlace(processingTimes_, machineIndices_, processingTimes_[machineIndices_[0]], machineCount_);
            shift(machineIndices_, ind);
            machineIndices_[ind] = tempind;
        }

        return processingTimes_[machineIndices_[machineCount_ - 1]];
    }

    double TransactionSchedulingProblem::getValue(double cost)
    {
        return -cost;
    }

    double TransactionSchedulingProblem::getScaleFactor()
    {
        std::vector<int> solution(stringLength);

        for(int i = 0; i < stringLength; i++)
        {
            solution[i] = i;
        }

        return getCost(solution);
    }
}