#include "STILO/SolverModels/GAModels/Individual.h"

namespace STILO
{
    void Individual::pointMutate()
    {
        for(int i = 0; i < stringLength_; i++)
        {
            double p = probRnd_.generate();

            if(p < pointMutationProbability_)
            {
                pointRandom(state_, i, stateRnd_);
            }
        }
    }

    void Individual::insertMutate()
    {
        insertRandom(state_, stringRnd_);
    }

    void Individual::invertMutate()
    {
        invertRandom(state_, stringLength_, stringRnd_);
    }

    void Individual::swapMutate()
    {
        randomize(state_, stringRnd_);
    }

    Individual::Individual(PoolKey, int stateCount, int stringLength,
        double mutationProbability, double pointMutationProbability, int mutationOperatorCount,
        const std::vector<MutationPointer>& mutationPointers, const std::vector<double>& mutationPropensities,
        bool unique, bool initialize)
        : stateCount_(stateCount),
          stringLength_(stringLength),
          mutationProbability_(mutationProbability),
          pointMutationProbability_(pointMutationProbability),
          mutationOperatorCount_(mutationOperatorCount),
          mutationPointers_(mutationPointers),
          mutationPropensities_(mutationPropensities),
          state_(stringLength),
          probRnd_(0, 1),
          stateRnd_(0, stateCount - 1),
          stringRnd_(0, stringLength - 1),
          elite_(false),
          eliminated_(false),
          evaluate_(true)
    {
        if(initialize)
        {
            if(unique)
            {
                for(int i = 0; i < stringLength_; i++)
                {
                    state_[i] = i;
                }

                for(int i = 0; i < 3 * stateCount_; i++)
                {
                    randomize(state_, stringRnd_);
                }
            }
            else
            {
                for(int i = 0; i < stringLength_; i++)
                {
                    state_[i] = stateRnd_.generate();
                }
            }
        }
    }

    void Individual::mutate(PopulationKey)
    {
        double p = probRnd_.generate();

        if(p < mutationProbability_)
        {
            evaluate_ = true;
            double pp = probRnd_.generate();

            for(int i = 0; i < mutationOperatorCount_ - 1; i++)
            {
                if(pp < mutationPropensities_[i])
                {
                    return (this->*mutationPointers_[i])();
                }
            }

            return (this->*mutationPointers_[mutationOperatorCount_ - 1])();
        }
    }

    void Individual::evaluate(PopulationKey, Problem* problem)
    {
        if(evaluate_)
        {
            cost_ = problem->getCost(state_);
            evaluate_ = false;
        }
    }

    double Individual::getCost(PopulationKey)
    {
        return cost_;
    }

    std::vector<int>& Individual::getState(PopulationKey)
    {
        return state_;
    }

    void Individual::setElite(PopulationKey)
    {
        elite_ = true;
    }

    void Individual::setNormal(PopulationKey)
    {
        elite_ = false;
    }

    bool Individual::isElite(PopulationKey)
    {
        return elite_;
    }

    void Individual::eliminate(PopulationKey)
    {
        eliminated_ = true;
        evaluate_ = true;
    }

    void Individual::deliminate(PoolKey)
    {
        eliminated_ = false;
    }

    bool Individual::isEliminated(PopulationKey)
    {
        return eliminated_;
    }
}