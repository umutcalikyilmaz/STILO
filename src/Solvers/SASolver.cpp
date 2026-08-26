#include "STILO/Solvers/SASolver.h"

using namespace std::chrono_literals;

namespace STILO
{
    void SASolver::checkInput(Problem* problem, const SAConfiguration& config)
    {
        if(config.pointMoveCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Point move coeffficient cannot be less than 0");
        }

        if(config.insertMoveCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Insert move coeffficient cannot be less than 0");
        }

        if(config.invertMoveCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Invert move coeffficient cannot be less than 0");
        }

        if(config.swapMoveCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Swap move coeffficient cannot be less than 0");
        }

        if(config.initialTemperature < 0)
        {
            throw InvalidParameterException((char*)"Initial temperature cannot be less than 0");
        }

        double minNeighborhoodSize = config.minimumNeighborhoodSize < 1 ? 1 : config.minimumNeighborhoodSize;
        double maxNeighborhoodSize = config.maximumNeighborhoodSize;
        double tot = config.pointMoveCoefficient + config.insertMoveCoefficient + config.invertMoveCoefficient
            + config.swapMoveCoefficient;        
        int stateCount = problem->getStateCount();
        int stringLength = problem->getStringLength();

        if(tot == 0)
        {
            throw InvalidParameterException((char*)"Propensities of all move operators are set to 0");
        }

        if(config.distanceCalculationOperator != DistanceCalculationOperator::Constant)
        {
            if(config.minimumNeighborhoodSize <= 0)
            {
                throw InvalidParameterException((char*)"Minimum neighborhood size must be greater than 0");
            }

            if(config.distanceCalculationOperator == DistanceCalculationOperator::Cauchy || config.distanceCalculationOperator == DistanceCalculationOperator::Gaussian)
            {
                if(config.maximumNeighborhoodSize <= config.minimumNeighborhoodSize)
                {
                    throw InvalidParameterException((char*)"Maximum neighborhood size must be greater than minimum neighborhood size");
                }
            }
        }

        if(problem->getSolutionSpaceType() == SolutionSpaceType::Permutation)
        {
            if(config.pointMoveCoefficient > 0)
            {
                throw InvalidParameterException((char*)"Point move operator is not applicable to permutation solution spaces");
            }

            if(stateCount < stringLength)
            {
                throw InvalidParameterException((char*)"Invalid solution space");
            }
        }

        if(config.coolingSchedule == EvolvingParameterType::Exponential)
        {
            if(config.coolingParameter < 0 || config.coolingParameter > 1)
            {
                throw InvalidParameterException((char*)"Cooling parameter must be in the range [0,1]");
            }
        }
    }

    std::unique_ptr<SAIteratorWrapper> SASolver::createIterator(EvolvingParameterType cooling,
        DistanceCalculationOperator distanceCalculation, Problem* problem,
        int initialTemperature, int minimumNeighorhoodSize, int maximumNeighorhoodSize,
        double coolingParameter, double pointMoveCoefficient, double insertMoveCoefficient,
        double invertMoveCoefficient, double swapMoveCoefficient)
    {
        return dispatchEvolvingParameterType(cooling, [&]<EvolvingParameterType kCooling>() -> std::unique_ptr<SAIteratorWrapper>
        {
            return dispatchDistanceCalculationOperator(distanceCalculation, [&]<DistanceCalculationOperator kDistanceCalculation>() -> std::unique_ptr<SAIteratorWrapper>
            {
                return std::make_unique<SAIterator<kCooling, kDistanceCalculation>>(
                    typename SAIteratorWrapper::SASolverKey{}, problem, initialTemperature,
                    minimumNeighorhoodSize, maximumNeighorhoodSize, coolingParameter, pointMoveCoefficient,
                    insertMoveCoefficient, invertMoveCoefficient, swapMoveCoefficient);
            });
        });
    }

    SolverOutput SASolver::solve(const SolverInput& input)
    {
        double beginning = getTime();
        double totalTime = 0;
        double averageTime = 0;

        checkInput(input.problem, input.SAConfig);

        auto iterator = createIterator(input.SAConfig.coolingSchedule, input.SAConfig.distanceCalculationOperator,
            input.problem, input.SAConfig.initialTemperature, input.SAConfig.minimumNeighborhoodSize,
            input.SAConfig.maximumNeighborhoodSize, input.SAConfig.coolingParameter,
            input.SAConfig.pointMoveCoefficient, input.SAConfig.insertMoveCoefficient,
            input.SAConfig.invertMoveCoefficient, input.SAConfig.swapMoveCoefficient);

        int c = 0;

        while(true)
        {
            iterator->iterate(typename SAIteratorWrapper::SASolverKey{});
            totalTime = getTime();
            averageTime = totalTime / ++c;

            if(totalTime + averageTime > input.timeLimit.count())
            {
                break;
            }
        }

        auto res = iterator->getOutput(typename SAIteratorWrapper::SASolverKey{});
        res.executionTime = totalTime * 1ms;
        iterator = nullptr;
        return res;
    }

    SolverAnalysisOutput SASolver::analyze(const SolverAnalysisInput& input)
    {
        double beginning = getTime();
        double totalTime = 0;

        checkInput(input.problem, input.SAConfig);

        auto iterator = createIterator(input.SAConfig.coolingSchedule, input.SAConfig.distanceCalculationOperator,
            input.problem, input.SAConfig.initialTemperature, input.SAConfig.minimumNeighborhoodSize,
            input.SAConfig.maximumNeighborhoodSize, input.SAConfig.coolingParameter,
            input.SAConfig.pointMoveCoefficient, input.SAConfig.insertMoveCoefficient,
            input.SAConfig.invertMoveCoefficient, input.SAConfig.swapMoveCoefficient);

        auto it = input.timeLimits.begin();
        SolverAnalysisOutput res;
        SolverOutput previousResult = iterator->getOutput(typename SAIteratorWrapper::SASolverKey{});
        std::vector<std::unique_ptr<SolverOutput>> results;
        bool exit = false;
        int c = 0;
        double wasteTime = 0;
    
        while(true)
        {
            iterator->iterate(typename SAIteratorWrapper::SASolverKey{});
            totalTime = getTime() - beginning + wasteTime;  
                
            double wasteBeg = getTime();
    
            while(totalTime > it->count())
            {
                if(c > 0)
                {
                    res.results[*it] = std::make_unique<SolverOutput>(previousResult);                    
                }
                else
                {
                    res.results[*it] = nullptr;
                }
                    
                it++;
    
                if(it == input.timeLimits.end())
                {
                    exit = true;
                    break;
                }
            }
    
            if(exit)
            {
                break;
            }
    
            previousResult = iterator->getOutput(typename SAIteratorWrapper::SASolverKey{});
            previousResult.executionTime = totalTime * 1ms;
            c++;
    
            wasteTime += getTime() - wasteBeg;
        }
            
        iterator = nullptr;
        return res;
    }
}