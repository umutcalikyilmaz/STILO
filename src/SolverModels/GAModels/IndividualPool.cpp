#include "STILO/SolverModels/GAModels/IndividualPool.h"

namespace STILO
{
    IndividualPool::IndividualPool(int stateCount, int stringLength, double mutationProbability,
        double pointMutationProbability, double pointMutationCoefficient, double insertMutationCoefficient,
        double invertMutationCoefficient, double swapMutationCoefficient, bool unique)
        : stateCount_(stateCount),
          stringLength_(stringLength),
          mutationProbability_(mutationProbability),
          pointMutationProbability_(pointMutationProbability),        
          unique_(unique),
          total_(0),
          inUse_(0)
    {
        double total = insertMutationCoefficient + invertMutationCoefficient + pointMutationCoefficient + swapMutationCoefficient;
        double prob = 0;
        mutationOperatorCount_ = 0;

        if(pointMutationCoefficient > 0)
        {
            mutationPropensities_.push_back(pointMutationCoefficient / total);
            mutationPointers_.push_back(&Individual::pointMutate);
            prob = mutationPropensities_[mutationOperatorCount_++];
        }

        if(insertMutationCoefficient > 0)
        {
            mutationPropensities_.push_back(prob + insertMutationCoefficient / total);
            mutationPointers_.push_back(&Individual::insertMutate);
            prob = mutationPropensities_[mutationOperatorCount_++];
        }

        if(invertMutationCoefficient > 0)
        {
            mutationPropensities_.push_back(prob + invertMutationCoefficient / total);
            mutationPointers_.push_back(&Individual::invertMutate);
            prob = mutationPropensities_[mutationOperatorCount_++];
        }

        if(swapMutationCoefficient > 0)
        {
            mutationPropensities_.push_back(prob + swapMutationCoefficient / total);
            mutationPointers_.push_back(&Individual::swapMutate);
            prob = mutationPropensities_[mutationOperatorCount_++];
        }
    }

    std::unique_ptr<Individual> IndividualPool::getIndividual(bool init)
    {
        if(individualQueue_.size() == 0)
        {
            inUse_++;
            total_++;
            return std::make_unique<Individual>(typename Individual::PoolKey {}, stateCount_, stringLength_,
                mutationProbability_, pointMutationProbability_, mutationOperatorCount_, mutationPointers_,
                mutationPropensities_, unique_, init);
        }
        else
        {
            inUse_++;
            std::unique_ptr<Individual> res = move(individualQueue_.front());
            individualQueue_.pop();
            return res;
        }
    }

    void IndividualPool::returnIndividual(std::unique_ptr<Individual> individual)
    {
        individual->deliminate(typename Individual::PoolKey {});
        individualQueue_.push(move(individual));        
        inUse_--;
    }
}