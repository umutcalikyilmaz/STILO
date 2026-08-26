#pragma once
#include <memory>
#include "STILO/ProblemModels/Problem.h"
#include "STILO/Shared/List.h"
#include "STILO/Structs/SolverStructs/GAConfiguration.h"



namespace STILO
{
    class Individual
    {        
        using MutationPointer = void (Individual::*)();

    public:

        class PoolKey
        {
        private:

            PoolKey() { }

            friend class IndividualPool;
        };

        class PopulationKey
        {
        private:

            PopulationKey() { }

            template<SelectionOperator selection>
            friend class Population;
        };

        Individual(PoolKey, int stateCount, int stringLength, double mutationProbability,
            double pointMutationProbability, int mutationOperationCount,
            const std::vector<MutationPointer>& mutationPointers, const std::vector<double>& mutationPropensities,
            bool unique, bool initialize);

        void mutate(PopulationKey);        

        void evaluate(PopulationKey, Problem* problem);

        double getCost(PopulationKey);

        std::vector<int>& getState(PopulationKey);

        void setElite(PopulationKey);

        void setNormal(PopulationKey);

        bool isElite(PopulationKey);

        void eliminate(PopulationKey);

        void deliminate(PoolKey);

        bool isEliminated(PopulationKey);

    private:
    
        UniformRandomDoubleGenerator probRnd_;
        UniformRandomIntGenerator stateRnd_;
        UniformRandomIntGenerator stringRnd_;        
        std::vector<MutationPointer> mutationPointers_;
        std::vector<double> mutationPropensities_;
        std::vector<int> state_;
        double cost_;
        double mutationProbability_;
        double pointMutationProbability_;
        int stateCount_;
        int stringLength_;
        int mutationOperatorCount_;
        bool binary_;
        bool dense_;
        bool unique_;
        bool elite_;
        bool eliminated_;
        bool evaluate_;
        
        void pointMutate();

        void insertMutate();

        void invertMutate();

        void swapMutate();

        friend class IndividualPool;
    };
}