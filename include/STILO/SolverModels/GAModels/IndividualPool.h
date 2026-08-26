#include <queue>
#include "STILO/SolverModels/GAModels/Individual.h"

namespace STILO
{
    using MutationPointer = void (Individual::*)();

    class IndividualPool
    {
    private:

        std::queue<std::unique_ptr<Individual>> individualQueue_;        
        std::vector<MutationPointer> mutationPointers_;
        std::vector<double> mutationPropensities_;
        double mutationProbability_;
        double pointMutationProbability_;        
        int stringLength_;
        int stateCount_;
        int mutationOperatorCount_;
        int inUse_;
        int total_;
        bool unique_;

        IndividualPool(int stateNum, int stringLength, double mutationProbability,
            double pointMutationProbability, double pointMutationCoefficient, double insertMutationCoefficient,
            double invertMutationCoefficient, double swapMutationCoefficient, bool unique);

        std::unique_ptr<Individual> getIndividual(bool init);

        void returnIndividual(std::unique_ptr<Individual> individual);

        template<SelectionOperator selection>
        friend class Population;
    };
}