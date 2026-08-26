#pragma once
#include <vector>
#include "STILO/SolverModels/GAModels/IndividualPool.h"
#include "STILO/SolverModels/GAModels/PopulationWrapper.h"

namespace STILO
{
    template<SelectionOperator selection>
    class Population final : public PopulationWrapper
    {
        using CrossoverPointer = void (Population::*)(int,int);

    public:

        Population(GASolverKey, Problem* problem, int populationSize, int matingCount, int eliteCount,
            double mutationProbability, double pointMutationProbability, double pointMutationCoefficient,
            double insertMutationCoefficient, double invertMutationCoefficient, double swapMutationCoefficient, 
            double cycleCrossoverCoefficient, double kpointCrossoverCoefficient, double OXCrossoverCoefficient,
            double PMXCrossoverCoefficient, double uniformCrossoverCoefficient, int k, int tournamentSize)
            : problem_(problem),
              stateCount_(problem->getStateCount()),
              stringLength_(problem->getStringLength()),
              populationSize_(populationSize),
              matingCount_(matingCount),
              eliteCount_(eliteCount),
              ip_(problem->getStateCount(), problem->getStringLength(), mutationProbability, pointMutationProbability,
                pointMutationCoefficient, insertMutationCoefficient, invertMutationCoefficient,
                swapMutationCoefficient, problem->getSolutionSpaceType() == SolutionSpaceType::Permutation),
              k_(k),
              tournamentSize_(tournamentSize),
              costs_(populationSize),
              indices_(populationSize),
              bestSolution_(problem->getStringLength()),
              bestCost_(DBL_MAX),
              probRnd_(0, 1),
              stringRnd_(0, problem->getStringLength() - 1),
              individualRnd_(0, populationSize - 1),
              iterationCount_(0)
        {
            for(int i = 0; i < populationSize; i++)
            {
                individuals_.emplace_back(ip_.getIndividual(true));
            }

            double total = cycleCrossoverCoefficient + kpointCrossoverCoefficient + OXCrossoverCoefficient + 
                PMXCrossoverCoefficient + uniformCrossoverCoefficient;

            crossoverCount_ = 0;
            double prob = 0;
    
            if(cycleCrossoverCoefficient > 0)
            {
                crossoverPointers_.push_back(&Population::crossoverCycle);
                crossoverPropensities_.push_back(prob + cycleCrossoverCoefficient / total);
                prob = crossoverPropensities_[crossoverCount_++];
            }
    
            if(kpointCrossoverCoefficient > 0)
            {
                crossoverPointers_.push_back(&Population::crossoverKpoint);
                crossoverPropensities_.push_back(prob + kpointCrossoverCoefficient / total);
                prob = crossoverPropensities_[crossoverCount_++];
                kpoints_.resize(k);
            }
    
            if(OXCrossoverCoefficient > 0)
            {
                crossoverPointers_.push_back(&Population::crossoverOX);
                crossoverPropensities_.push_back(prob + OXCrossoverCoefficient / total);
                prob = crossoverPropensities_[crossoverCount_++];
            }

            if(PMXCrossoverCoefficient > 0)
            {
                crossoverPointers_.push_back(&Population::crossoverPMX);
                crossoverPropensities_.push_back(prob + PMXCrossoverCoefficient / total);
                prob = crossoverPropensities_[crossoverCount_++];
            }
    
            if(uniformCrossoverCoefficient > 0)
            {
                crossoverPointers_.push_back(&Population::crossoverUniform);
                crossoverPropensities_.push_back(prob + uniformCrossoverCoefficient / total);
                prob = crossoverPropensities_[crossoverCount_++];
            }

            if(problem_->getSolutionSpaceType() == SolutionSpaceType::Permutation)
            {
                perm1_.assign(stringLength_, 1);
                perm2_.assign(stringLength_, 1);
            }

            if constexpr (selection == SelectionOperator::Roulette)
            {
                fitnesses_.resize(populationSize_);
                probabilities_.resize(populationSize_);
                aliases_.resize(populationSize_);
                smallIndices_.reserve(populationSize_);
                largeIndices_.reserve(populationSize_);
                tempProb_.resize(populationSize_);
            }
    
            if constexpr (selection == SelectionOperator::ByRank)
            {
                probabilities_.reserve(populationSize_);
                aliases_.reserve(populationSize_);
                aliasLimit_ = (populationSize_ + 1) / 2;

                double total = (double)populationSize_ * (double)(populationSize_ + 1) / 2;
                double stdProb = 1 / (double)populationSize_;

                for(int i = 0; i < aliasLimit_; i++)
                {
                    probabilities_.push_back(1);
                    aliases_.push_back(0);
                }

                for(int i = aliasLimit_; i < populationSize_; i++)
                {
                    probabilities_.push_back((double)(populationSize_ - i) * (double)populationSize_ / total);
                    aliases_.push_back(populationSize_ - i - 1);
                }
            }

            eliminationLimit_ = 0;
            evaluate();
            eliminationLimit_ = populationSize_ - 2 * matingCount_;
        }

        void evolve(GASolverKey) override
        {
            int c = 0;

            for(int i = 0; i < matingCount_; i++)
            {
                int p1 = select(c++);
                int p2;
    
                do
                {
                    p2 = select(c);
                }
                while(p1 == p2);
    
                c++;
    
                crossover(p1, p2);      
            }
    
            eliminate();
            mutate();
            evaluate();

            if(costs_[indices_[0]] < bestCost_)
            {
                bestCost_ = costs_[indices_[0]];
                bestSolution_ = individuals_[indices_[0]]->getState(typename Individual::PopulationKey{});
            }

            iterationCount_++;
        }

        SolverOutput getOutput(GASolverKey) override
        {
            return SolverOutput(bestCost_, problem_->getValue(bestCost_),
                bestSolution_, iterationCount_);
        }

    private:

        UniformRandomDoubleGenerator probRnd_;
        UniformRandomIntGenerator stringRnd_;
        UniformRandomIntGenerator individualRnd_;
        std::vector<std::unique_ptr<Individual>> individuals_;
        IndividualPool ip_;
        Problem* problem_;
        std::vector<CrossoverPointer> crossoverPointers_;
        std::vector<double> crossoverPropensities_;
        std::vector<double> probabilities_;
        std::vector<double> fitnesses_;
        std::vector<double> costs_;
        std::vector<double> tempProb_;
        std::vector<int> indices_;
        std::vector<int> kpoints_;
        std::vector<int> bestSolution_;
        std::vector<int> aliases_;
        std::vector<int> smallIndices_;
        std::vector<int> largeIndices_;
        std::vector<uint8_t> perm1_;
        std::vector<uint8_t> perm2_;  
        double bestCost_;      
        double totalFitness_;
        int stateCount_;
        int stringLength_;        
        int k_;
        int populationSize_;
        int matingCount_;
        int tournamentSize_;
        int eliteCount_;
        int crossoverCount_;
        int eliminationLimit_;
        int iterationCount_;
        int aliasLimit_;

        int select(int num)
        {
            if constexpr (selection == SelectionOperator::ByRank)
            {
                int ind = individualRnd_.generate();

                if(ind < aliasLimit_)
                {
                    return indices_[ind];
                }
                else
                {
                    double p = probRnd_.generate();
                    return p < probabilities_[ind] ? indices_[ind] : indices_[aliases_[ind]];
                }
            }
            else if constexpr (selection == SelectionOperator::Roulette)
            {
                int ind = individualRnd_.generate();
                double p = probRnd_.generate();

                if(p < probabilities_[ind])
                {
                    return ind;
                }
                else
                {
                    return aliases_[ind];
                }
            }
            else if constexpr (selection == SelectionOperator::Steady)
            {
                return indices_[num];
            }
            else if constexpr (selection == SelectionOperator::Tournament)
            {
                int bestParticipant = populationSize_;

                for(int i = 0; i < tournamentSize_; i++)
                {
                    int participant = individualRnd_.generate();

                    if(participant < bestParticipant)
                    {
                        bestParticipant = participant;
                    }
                }

                return indices_[bestParticipant];
            }
        }

        void crossover(int ind1, int ind2)
        {
            double p = probRnd_.generate();

            for(int i = 0; i < crossoverCount_ - 1; i++)
            {
                if(p < crossoverPropensities_[i])
                {
                    return (this->*crossoverPointers_[i])(ind1, ind2);
                }
            }

            return (this->*crossoverPointers_[crossoverCount_ - 1])(ind1, ind2);
        }

        void crossoverCycle(int ind1, int ind2)
        {
            std::unique_ptr<Individual> individual1 = ip_.getIndividual(false);
            std::unique_ptr<Individual> individual2 = ip_.getIndividual(false);
            auto& state1 = individuals_[ind1]->getState(typename Individual::PopulationKey{});
            auto& state2 = individuals_[ind2]->getState(typename Individual::PopulationKey{});
            auto& nstate1 = individual1->getState(typename Individual::PopulationKey{});
            auto& nstate2 = individual2->getState(typename Individual::PopulationKey{});
            int iind = stringRnd_.generate();
    
            while(true)
            {
                int val = state1[iind];
    
                if(!perm1_[iind])
                {
                    break;
                }
    
                nstate1[iind] = val;
                perm1_[iind] = 0;
    
                for(int i = 0; i < stringLength_; i++)
                {
                    if(val == state2[i])
                    {
                        iind = i;                    
                        break;
                    }
                }
            }
    
            for(int i = 0; i < stringLength_; i++)
            {
                if(perm1_[i])
                {
                    nstate1[i] = state2[i];
                }
                else
                {
                    perm1_[i] = 1;
                }            
            }
    
            iind = stringRnd_.generate();
    
            while(true)
            {
                int val = state2[iind];
    
                if(!perm1_[iind])
                {
                    break;
                }
    
                nstate2[iind] = val;
                perm1_[iind] = 0;
    
                for(int i = 0; i < stringLength_; i++)
                {
                    if(val == state1[i])
                    {
                        iind = i;                    
                        break;
                    }
                }
            }
    
            for(int i = 0; i < stringLength_; i++)
            {
                if(perm1_[i])
                {
                    nstate2[i] = state1[i];
                }
                else
                {
                    perm1_[i] = 1;
                }            
            }
    
            individuals_.push_back(move(individual1));
            individuals_.push_back(move(individual2));
        }

        void crossoverKpoint(int ind1, int ind2)
        {
            for(int i = 0; i < k_; i++)
            {
                int newPoint;
                bool matched;

                do
                {
                    newPoint = stringRnd_.generate();
                    matched = false;

                    for(int j = 0; j < i ; j++)
                    {
                        if(newPoint == kpoints_[j])
                        {
                            matched = true;
                            break;
                        }
                    }
                }
                while(matched);

                int ind = findElement<int>(kpoints_, newPoint, i);
                shiftRightPartial<int>(kpoints_, ind, i);
                kpoints_[ind] = newPoint;
            }
    
            std::unique_ptr<Individual> individual1 = ip_.getIndividual(false);
            std::unique_ptr<Individual> individual2 = ip_.getIndividual(false);
            int inds[2] {ind1, ind2};
            auto& state1 = individuals_[ind1]->getState(typename Individual::PopulationKey{});
            auto& state2 = individuals_[ind2]->getState(typename Individual::PopulationKey{});
            auto& nstate1 = individual1->getState(typename Individual::PopulationKey{});
            auto& nstate2 = individual2->getState(typename Individual::PopulationKey{});
            int c = 0;
    
            for(int i = 0; i < stringLength_; i++)
            {
                if(kpoints_[c] == i)
                {
                    c++;                
                }

                if(c % 2 == 0)
                {
                    nstate1[i] = state1[i];
                    nstate2[i] = state2[i];
                }
                else
                {
                    nstate1[i] = state2[i];
                    nstate2[i] = state1[i];
                }
            }
    
            individuals_.push_back(move(individual1));
            individuals_.push_back(move(individual2));
        }

        void crossoverOX(int ind1, int ind2)
        {
            std::unique_ptr<Individual> individual1 = ip_.getIndividual(false);
            std::unique_ptr<Individual> individual2 = ip_.getIndividual(false);
            auto& state1 = individuals_[ind1]->getState(typename Individual::PopulationKey{});
            auto& state2 = individuals_[ind2]->getState(typename Individual::PopulationKey{});
            auto& nstate1 = individual1->getState(typename Individual::PopulationKey{});
            auto& nstate2 = individual2->getState(typename Individual::PopulationKey{});
            int iind1 = stringRnd_.generate();
            int iind2;
    
            do
            {
                iind2 = stringRnd_.generate();
            }
            while(iind1 == iind2);
    
            int c1 = iind2;
            int c2 = iind2;
    
            if(iind1 > iind2)
            {
                iind2 = iind2 + stringLength_;
            }
    
            for(int i = iind1; i < iind2; i++)
            {
                int tempi = i % stringLength_;
                nstate1[tempi] = state1[tempi];
                nstate2[tempi] = state2[tempi];
                perm1_[state1[tempi]] = 0;
                perm2_[state2[tempi]] = 0;
            }
    
            for(int i = 0; i < stringLength_; i++)
            {
                if(perm1_[state2[i]])
                {
                    int tempi = c1 % stringLength_;
                    nstate1[tempi] = state2[i];
                    c1++;
                }
                else
                {
                    perm1_[state2[i]] = 1;
                }
    
                if(perm2_[state1[i]])
                {
                    int tempi = c2 % stringLength_;
                    nstate2[tempi] = state1[i];
                    c2++;
                }
                else
                {
                    perm2_[state1[i]] = 1;
                }
            }
    
            individuals_.push_back(move(individual1));
            individuals_.push_back(move(individual2));
        }

        void crossoverPMX(int ind1, int ind2)
        {
            std::unique_ptr<Individual> individual1 = ip_.getIndividual(false);
            std::unique_ptr<Individual> individual2 = ip_.getIndividual(false);
            auto& state1 = individuals_[ind1]->getState(typename Individual::PopulationKey{});
            auto& state2 = individuals_[ind2]->getState(typename Individual::PopulationKey{});
            auto& nstate1 = individual1->getState(typename Individual::PopulationKey{});
            auto& nstate2 = individual2->getState(typename Individual::PopulationKey{});
            int iind1 = stringRnd_.generate();
            int iind2;
    
            for(int i = 0; i < stringLength_; i++)
            {
                nstate1[i] = -1;
                nstate2[i] = -1;
            }
    
            do
            {
                iind2 = stringRnd_.generate();
            }
            while(iind1 == iind2);
    
            int c = iind2;
    
            if(iind1 > iind2)
            {
                iind2 += stringLength_;
            }
    
            int tempi;
            int tempi2;
    
            for(int i = iind1; i <= iind2; i++)
            {
                tempi = i % stringLength_;
                nstate1[tempi] = state1[tempi];
                nstate2[tempi] = state2[tempi];
                perm1_[state1[tempi]] = 0;
                perm2_[state2[tempi]] = 0;
            }
    
            for(int i = iind1; i <= iind2; i++)
            {
                tempi = i % stringLength_;
                
                if(perm1_[state2[tempi]])
                {
                    int val = state2[tempi];
                    int idx = tempi;
    
                    while(nstate1[idx] != -1)
                    {
                        for(int j = 0; j < stringLength_; j++)
                        {
                            if(state1[idx] == state2[j])
                            {
                                idx = j;
                                break;
                            }
                        }
                    }
    
                    nstate1[idx] = val;
                }
    
                if(perm2_[state1[tempi]])
                {
                    int val = state1[tempi];
                    int idx = tempi;
    
                    while(nstate2[idx] != -1)
                    {
                        for(int j = 0; j < stringLength_; j++)
                        {
                            if(state2[idx] == state1[j])
                            {
                                idx = j;
                                break;
                            }
                        }
                    }
    
                    nstate2[idx] = val;
                }
            }
    
            for(int i = c; i < iind1 + stringLength_; i++)
            {
                tempi = i % stringLength_;
    
                if(nstate1[tempi] == -1)
                {
                    nstate1[tempi] = state2[tempi];
                }
    
                if(nstate2[tempi] == -1)
                {
                    nstate2[tempi] = state1[tempi];
                }
            }
    
            for(int i = 0; i < stateCount_; i++)
            {
                perm1_[i] = 1;
                perm2_[i] = 1;
            }
    
            individuals_.push_back(move(individual1));
            individuals_.push_back(move(individual2));
        }

        void crossoverUniform(int ind1, int ind2)
        {
            std::unique_ptr<Individual> individual1 = ip_.getIndividual(false);
            std::unique_ptr<Individual> individual2 = ip_.getIndividual(false);
            auto& state1 = individuals_[ind1]->getState(typename Individual::PopulationKey{});
            auto& state2 = individuals_[ind2]->getState(typename Individual::PopulationKey{});
            auto& nstate1 = individual1->getState(typename Individual::PopulationKey{});
            auto& nstate2 = individual2->getState(typename Individual::PopulationKey{});
    
            for(int i = 0; i < stringLength_; i++)
            {
                double p = probRnd_.generate();
    
                if(p > 0.5)
                {
                    nstate1[i] = state1[i];
                    nstate2[i] = state2[i];
                }
                else
                {
                    nstate2[i] = state1[i];
                    nstate1[i] = state2[i];
                }
            }
    
            individuals_.push_back(move(individual1));
            individuals_.push_back(move(individual2));
        }

        void evaluate()
        {
            if constexpr (selection == SelectionOperator::Roulette)
            {
                totalFitness_ = 0;
            }

            for(int i = 0; i < populationSize_; i++)
            {
                individuals_[i]->evaluate(typename Individual::PopulationKey{}, problem_);
                costs_[i] = individuals_[i]->getCost(typename Individual::PopulationKey{});
                int ind = findMinimum(costs_, indices_, costs_[i], i);
                shiftRightPartial<int>(indices_, ind, i);
                indices_[ind] = i;
                
                if constexpr (selection == SelectionOperator::Roulette)
                {                    
                    fitnesses_[i] = 1 / costs_[i];
                    totalFitness_ += fitnesses_[i];
                }
            }

            if constexpr (selection == SelectionOperator::Roulette)
            {                
                double stdProb = 1.0 / (double)populationSize_;

                for(int i = 0; i < populationSize_; i++)
                {
                    tempProb_[i] = fitnesses_[i] / totalFitness_ - stdProb;
                    probabilities_[i] = 1;

                    if(tempProb_[i] < 0)
                    {
                        smallIndices_.push_back(i);
                    }
                    else
                    {
                        largeIndices_.push_back(i);
                    }
                }

                while(!smallIndices_.empty() && !largeIndices_.empty())
                {
                    int largeInd = largeIndices_.back();                    
                    int smallInd = smallIndices_.back();
                    smallIndices_.pop_back();

                    aliases_[smallInd] = largeInd;
                    probabilities_[smallInd] += tempProb_[smallInd] * populationSize_;
                    tempProb_[largeInd] += tempProb_[smallInd];

                    if(tempProb_[largeInd] < 0)
                    {
                        largeIndices_.pop_back();
                        smallIndices_.push_back(largeInd);
                    }
                    else if(tempProb_[largeInd] == 0)
                    {
                        largeIndices_.pop_back();
                    }
                }

                largeIndices_.clear();
                smallIndices_.clear();
            }
    
            for(int i = 0; i < eliteCount_; i++)
            {
                individuals_[indices_[i]]->setElite(typename Individual::PopulationKey{});
            }
        }

        void eliminate()
        {
            for(int i = eliminationLimit_; i < populationSize_; i++)
            {
                individuals_[indices_[i]]->eliminate(typename Individual::PopulationKey{});
            }

            int c = 0;
            int ind = 0;

            while(c < populationSize_ - eliminationLimit_)
            {
                if(individuals_[ind]->isEliminated(typename Individual::PopulationKey{}))
                {
                    ip_.returnIndividual(move(individuals_[ind]));
                    individuals_.erase(individuals_.begin() + ind);
                    c++;                
                }
                else
                {
                    ind++;
                }
            }
        }

        void mutate()
        {
            for(int i = 0; i < populationSize_; i++)
            {
                if(individuals_[i]->isElite(typename Individual::PopulationKey{}))
                {
                    individuals_[i]->setNormal(typename Individual::PopulationKey{});
                }
                else
                {
                    individuals_[i]->mutate(typename Individual::PopulationKey{});
                }            
            }
        }
    };
}
