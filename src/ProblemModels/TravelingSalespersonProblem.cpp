#include "STILO/ProblemModels/TravelingSalespersonProblem.h"

namespace STILO
{
    void TravelingSalespersonProblem::getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic)
	{
		heuristic.resize(stringLength);
		heuristic[0].resize(1);
		heuristic[0][0].resize(stateCount);
		double max = 0;

		for(int i = 0; i < stateCount; i++)
		{
			heuristic[0][0][i] = (distances_[0][i + 1] == 0 ? 1 : 1 / distances_[0][i + 1]);

			if(heuristic[0][0][i] > max)
			{
				max = heuristic[0][0][i];
			}
		}

		double scale = 1 / max;

		for(int i = 0; i < stateCount; i++)
		{
			heuristic[0][0][i] *= scale;
		}

		for(int i = 1; i < stringLength; i++)
		{
			heuristic[i].resize(stateCount);

			for(int j = 0; j < stateCount; j++)			
			{
				heuristic[i][j].resize(stateCount);
				max = 0;

				for(int k = 0; k < stateCount; k++)
				{
					heuristic[i][j][k] = (j == k) ? 0 : (distances_[j + 1][k + 1] == 0 ? 1 : 1 / distances_[j + 1][k + 1]);

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

    void TravelingSalespersonProblem::getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic)
	{
		heuristic.resize(1);
		heuristic[0].resize(stateCount + 1);
		double max;
		double scale;

		for(int i = 0; i < stateCount + 1; i++)
		{
			heuristic[0][i].resize(stateCount);
			max = 0;

			for(int j = 0; j < stateCount; j++)
			{
				heuristic[0][i][j] = (i - 1 == j) ? 0 : (distances_[i][j + 1] == 0 ? 1.0 : 1.0 / distances_[i][j + 1]);

				if(heuristic[0][i][j] > max)
				{
					max = heuristic[0][i][j];
				}
			}

			scale = 1 / max;

			for(int j = 0; j < stateCount; j++)
			{
				heuristic[0][i][j] *= scale;
			}
		}
	}

	TravelingSalespersonProblem::TravelingSalespersonProblem(const TravelingSalespersonProblem& original)
		: Problem(original),
		  cityCount_(original.cityCount_),
		  maxVal_(original.maxVal_),
		  distances_(original.distances_),
		  blist_(original.stateCount)	{ }

	TravelingSalespersonProblem::TravelingSalespersonProblem(int cityCount,
		const Matrix<double>& distances) 
		: Problem(cityCount - 1, cityCount - 1, SolutionSpaceType::Permutation),
		  cityCount_(cityCount),
		  distances_(distances),
		  blist_(cityCount - 1) { }

    TravelingSalespersonProblem::TravelingSalespersonProblem(int cityCount,
		ProbabilityDistribution distanceDistribution, double distributionParameter1, double distributionParameter2)
		: Problem(cityCount - 1, cityCount - 1, SolutionSpaceType::Permutation),
		  cityCount_(cityCount),
		  distances_(cityCount, cityCount),
		  blist_(cityCount - 1)
    {
		if (distanceDistribution == ProbabilityDistribution::Normal)
		{
			NormalRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

			for (int i = 0; i < cityCount; i++)
			{	
				auto row = distances_[i];
				
				for (int j = 0; j < i; j++)
				{
					do
					{
						row[j] = rnd.generate();
					}
					while(row[j] <= 0);
				}			
			}
		}
		else
		{
			UniformRandomDoubleGenerator rnd(distributionParameter1, distributionParameter2);

			for (int i = 0; i < cityCount; i++)
			{
				auto row = distances_[i];

				for (int j = 0; j < i; j++)
				{
					do
					{
						row[j] = rnd.generate();
					}
					while(row[j] <= 0);
				}			
			}					
		}

		for (int i = 0; i < cityCount; i++)
		{
			distances_[i][i] = 0;

			for (int j = cityCount - 1; j > i; j--)
			{
				distances_[i][j] = distances_[j][i];
			}
		}
    }

    double TravelingSalespersonProblem::getCost(std::span<const int> solution)
    {
        double res = 0;
		int curr = 0;	

		for (int i = 0; i < stateCount; i++)
		{
			blist_[i] = 0;
		}

		for (int i = 0; i < stateCount; i++)
		{
			if (solution[i] >= stateCount)
			{
				return DBL_MAX;
			}

			if (blist_[solution[i]] != 0)
			{
				return DBL_MAX;
			}
			
			res += distances_.getElement(curr, solution[i] + 1);
            blist_[solution[i]] = 1;		
			curr = solution[i] + 1;				
		}

        res += distances_[curr][0];
		
		return res;
    }

	double TravelingSalespersonProblem::getValue(double cost)
	{
		return maxVal_ - cost;
	}

	double TravelingSalespersonProblem::getScaleFactor()
    {
		double length = 0;
		int last = 0;
		std::vector<int> allowed;

		for(int i = 0; i < stringLength - 1; i++)
		{
			allowed.push_back(i);
		}

		for(int i = 0; i < stringLength - 1; i++)
		{
			int best;
			int pos;
			int c = 0;
			double bestLength = DBL_MAX;

			for(int city : allowed)
			{
				if(distances_[last][city + 1] < bestLength)
				{
					best = city;
					pos = c;
					bestLength = distances_[last][city + 1];
				}

				c++;
			}

			length += bestLength;
			allowed.erase(allowed.begin() + pos);
		}

		return length;
    }
}