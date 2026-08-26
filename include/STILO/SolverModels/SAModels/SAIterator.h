#pragma once
#include "STILO/SolverModels/SAModels/SAIteratorWrapper.h"

namespace STILO
{
    template<
        EvolvingParameterType Cooling,
        DistanceCalculationOperator DistanceCalculation>
    class SAIterator final : public SAIteratorWrapper
    {
        using MovePointer = void (SAIterator::*)();

    public:

        SAIterator(SASolverKey, Problem* problem, double initialTemperature, int minimumNeighorhoodSize,
            int maximumNeighorhoodSize, double coolingParameter, double pointMoveCoefficient,
            double insertMoveCoefficient, double invertMoveCoefficient,
            double swapMoveCoefficient)
            : problem_(problem),
              stateCount_(problem->getStateCount()),
              stringLength_(problem->getStringLength()),
              T_(initialTemperature),
              coolingSchedule_(initialTemperature, coolingParameter),
              distanceCalculator_(minimumNeighorhoodSize, maximumNeighorhoodSize),
              prbRnd_(0, 1),
              stateRnd_(0, problem->getStateCount() - 1),
              stringRnd_(0, problem->getStringLength() - 1),
              scale_(problem->getScaleFactor()),
              iterationCount_(0),
              newSolution_(problem->getStringLength()),
              bestSolution_(problem->getStringLength())
        {
            double total = pointMoveCoefficient + insertMoveCoefficient + invertMoveCoefficient + swapMoveCoefficient;
            double prob = 0;
            moveCount_ = 0;

            if(pointMoveCoefficient > 0)
            {
                movePointers_.push_back(&SAIterator::pointMove);
                movePropensities_.push_back(pointMoveCoefficient / total);
                prob = movePropensities_[moveCount_++];
            }

            if(insertMoveCoefficient > 0)
            {
                movePointers_.push_back(&SAIterator::insertMove);
                movePropensities_.push_back(prob + insertMoveCoefficient / total);
                prob = movePropensities_[moveCount_++];
            }

            if(invertMoveCoefficient > 0)
            {
                movePointers_.push_back(&SAIterator::invertMove);
                movePropensities_.push_back(prob + invertMoveCoefficient / total);
                prob = movePropensities_[moveCount_++];
            }

            if(swapMoveCoefficient > 0)
            {
                movePointers_.push_back(&SAIterator::swapMove);
                movePropensities_.push_back(prob + swapMoveCoefficient / total);
                moveCount_++;
            }

            if(problem_->getSolutionSpaceType() == SolutionSpaceType::Permutation)
            {
                for(int i = 0; i < stringLength_; i++)
                {
                    currentSolution_.push_back(i);
                }

                for(int i = 0; i < stringLength_; i++)
                {
                    randomize(currentSolution_, stringRnd_);
                }
            }
            else
            {
                for(int i = 0; i < stringLength_; i++)
                {
                    currentSolution_.push_back(stateRnd_.generate());
                }
            }

            currentCost_ = problem_->getCost(currentSolution_);
            bestCost_ = currentCost_;
            bestSolution_ = currentSolution_;
        }

        void iterate(SASolverKey) override
        {
            coolDown();
            newSolution_ = currentSolution_;
            move(calculateDistance());
            newCost_ = problem_->getCost(newSolution_);
            accept();
            iterationCount_++;
        }

        SolverOutput getOutput(SASolverKey) override
        {
            return SolverOutput(bestCost_, problem_->getValue(bestCost_), bestSolution_, iterationCount_);
        }

    private:

        Problem* problem_;
        UniformRandomDoubleGenerator prbRnd_;
        UniformRandomIntGenerator stateRnd_;
        UniformRandomIntGenerator stringRnd_;
        EvolvingParameter<Cooling> coolingSchedule_;
        DistanceCalculator<DistanceCalculation> distanceCalculator_;
        std::vector<MovePointer> movePointers_;
        std::vector<double> movePropensities_;
        std::vector<int> currentSolution_;
        std::vector<int> newSolution_;
        std::vector<int> bestSolution_;
        double T_;
        double scale_;
        double currentCost_;
        double newCost_;
        double bestCost_;
        int stateCount_;
        int stringLength_;
        int moveCount_;
        int iterationCount_;

        void pointMove()
        {
            randomize(newSolution_, stringRnd_, stateRnd_);
        }

        void insertMove()
        {
            insertRandom(newSolution_, stringRnd_);
        }

        void invertMove()
        {
            invertRandom(newSolution_, stringLength_, stringRnd_);
        }

        void swapMove()
        {
            randomize(newSolution_, stringRnd_);
        }        

        void coolDown()
        {
            coolingSchedule_.evolve();
            T_ = coolingSchedule_.getValue(); 
        }

        int calculateDistance()
        {
            if constexpr (DistanceCalculation == DistanceCalculationOperator::Cauchy)
            {
                return distanceCalculator_.calculateDistance(T_);
            }
            else if constexpr (DistanceCalculation == DistanceCalculationOperator::Constant)
            {
                return distanceCalculator_.calculateDistance();
            }
            else if constexpr (DistanceCalculation == DistanceCalculationOperator::Gaussian)
            {
                return distanceCalculator_.calculateDistance(T_);
            }
            else if constexpr (DistanceCalculation == DistanceCalculationOperator::Uniform)
            {
                return distanceCalculator_.calculateDistance();
            }            
            
            throw InvalidParameterException((char*)"Invalid enum value");            
        }

        void move(int distance)
        {
            double p = prbRnd_.generate();

            for(int i = 0; i < moveCount_ - 1; i++)
            {
                if(p < movePropensities_[i])
                {
                    return (this->*movePointers_[i])();
                }
            }

            return (this->*movePointers_[moveCount_ - 1])();
        }

        void accept()
        {
            if (newCost_ < currentCost_)
            {
                currentSolution_ = newSolution_;
                currentCost_ = newCost_;
    
                if (currentCost_ < bestCost_)
                {
                    bestSolution_ = currentSolution_;
                    bestCost_ = currentCost_;
                }
            }
            else
            {
                double p = prbRnd_.generate();
                double pp = exp((currentCost_ - newCost_) / scale_ / T_);
    
                if (p < pp)
                {
                    currentSolution_ = newSolution_;
                    currentCost_ = newCost_;
                }
            }
        }
    };
}