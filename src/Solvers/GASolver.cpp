#include "STILO/Solvers/GASolver.h"

using namespace std::chrono_literals;

namespace STILO
{
    void GASolver::checkInput(Problem* problem, const GAConfiguration& config)
    {
        if(config.mutationProbability < 0 || config.mutationProbability > 1)
        {
            throw InvalidParameterException((char*)"Mutation probability must be in the range [0,1]");
        }

        if(config.pointMutationCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Point mutation coefficient cannot be less than 0");
        }

        if(config.insertMutationCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Insert mutation coefficient cannot be less than 0");
        }

        if(config.invertMutationCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Invert mutation coefficient cannot be less than 0");
        }

        if(config.swapMutationCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Swap mutation coefficient cannot be less than 0");
        }
        
        if(config.cycleCrossoverCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Cycle crossover coefficient cannot be less than 0");
        }

        if(config.kpointCrossoverCoefficient < 0)
        {
            throw InvalidParameterException((char*)"KPoint crossover coefficient cannot be less than 0");
        }

        if(config.OXCrossoverCoefficient < 0)
        {
            throw InvalidParameterException((char*)"OX crossover coefficient cannot be less than 0");
        }

        if(config.PMXCrossoverCoefficient < 0)
        {
            throw InvalidParameterException((char*)"PMX crossover coefficient cannot be less than 0");
        }

        if(config.uniformCrossoverCoefficient < 0)
        {
            throw InvalidParameterException((char*)"Uniform crossover coefficient cannot be less than 0");
        }

        if(config.populationSize <= 0)
        {
            throw InvalidParameterException((char*)"Population size must be greater than 0");
        }

        if(config.matingCount < 0)
        {
            throw InvalidParameterException((char*)"Mating count cannot be less than 0");
        }

        if(config.eliteCount < 0)
        {
            throw InvalidParameterException((char*)"Elite count cannot be less than 0");
        }

        double totCrossover = config.cycleCrossoverCoefficient + config.kpointCrossoverCoefficient + config.OXCrossoverCoefficient
            + config.PMXCrossoverCoefficient + config.uniformCrossoverCoefficient;
        double totMutation = config.pointMutationCoefficient + config.insertMutationCoefficient + config.invertMutationCoefficient
            + config.swapMutationCoefficient;
        int stringLength = problem->getStringLength();
        int stateNum = problem->getStateCount();
        int eliminationLimit = config.populationSize - 2 * config.matingCount;
        bool unique = problem->getSolutionSpaceType() == SolutionSpaceType::Permutation;

        if(totCrossover == 0)
        {
            throw InvalidParameterException((char*)"Propensities of all crossover operators are set to zero");
        }

        if(totMutation == 0)
        {
            throw InvalidParameterException((char*)"Propensities of all mutation operators are set to zero");
        }

        if(unique)
        {
            if(stateNum < stringLength)
            {
                throw InvalidParameterException((char*)("Invalid solution space"));
            }

            if(config.kpointCrossoverCoefficient > 0 || config.uniformCrossoverCoefficient > 0)
            {
                throw InvalidParameterException((char*)"KPoint and Uniform crossover operators are not applicable to permutation solution spaces");
            }
            
            if(config.pointMutationCoefficient > 0)
            {
                throw InvalidParameterException((char*)"Point mutation operator is not applicable to permutation solution spaces");
            }
        }
        else
        {
            if(config.cycleCrossoverCoefficient > 0 || config.OXCrossoverCoefficient > 0 || config.PMXCrossoverCoefficient > 0)
            {
                throw InvalidParameterException((char*)("Cycle, OX and PMX crossover operators are only applicable on permutation solution spaces"));
            }

            if(config.pointMutationCoefficient > 0 && (config.pointMutationProbability < 0 || config.pointMutationProbability > 1))
            {
                throw InvalidParameterException((char*)"Point mutation probability must be in the range [0,1]");
            }

            if(config.kpointCrossoverCoefficient > 0)
            {
                if(config.k > stringLength)
                {
                    throw InvalidParameterException((char*)("The k value for the KPoint crossover operator cannot be greater than the string lenght"));
                }
                
                if(config.k <= 0)
                {
                    throw InvalidParameterException((char*)"The k value for the KPoint crossover operator cannot be less than 0");
                }
            }
        }        

        if(eliminationLimit < 0 || eliminationLimit < config.eliteCount)
        {
            throw InvalidParameterException((char*)("Mating count is too large for given parameters."));
        }

        if(config.selectionOperator == SelectionOperator::Tournament)
        {
            if(config.tournamentSize <= 0)
            {
                throw InvalidParameterException((char*)"Tournament size must be greater than 0");
            }

            if(config.tournamentSize > config.populationSize)
            {
                throw InvalidParameterException((char*)"Tournament size cannot be greater than the population size");
            }            
        }
    }

    SolverOutput GASolver::solve(const SolverInput& input)
    {
        double beginning = getTime();
        double averageTime = 0;
        double totalTime = 0;    
        
        checkInput(input.problem, input.GAConfig);

        auto population = dispatchSelectionOperator(input.GAConfig.selectionOperator, [&]<SelectionOperator kSelectionOperator>() -> std::unique_ptr<PopulationWrapper>
        {
            return std::make_unique<Population<kSelectionOperator>>(typename PopulationWrapper::GASolverKey{},
                input.problem, input.GAConfig.populationSize, input.GAConfig.matingCount,
                input.GAConfig.eliteCount, input.GAConfig.mutationProbability,
                input.GAConfig.pointMutationProbability, input.GAConfig.pointMutationCoefficient, 
                input.GAConfig.insertMutationCoefficient, input.GAConfig.invertMutationCoefficient, 
                input.GAConfig.swapMutationCoefficient, input.GAConfig.cycleCrossoverCoefficient,
                input.GAConfig.kpointCrossoverCoefficient, input.GAConfig.OXCrossoverCoefficient,
                input.GAConfig.PMXCrossoverCoefficient, input.GAConfig.uniformCrossoverCoefficient,
                input.GAConfig.k, input.GAConfig.tournamentSize);
        });

        int c = 0;

        while(true)
        {
            population->evolve(typename PopulationWrapper::GASolverKey{});
            totalTime = getTime() - beginning;
            averageTime = totalTime / ++c;
            
            if(totalTime + averageTime > input.timeLimit.count())
            {
                break;
            }
        }

        auto res = population->getOutput(typename PopulationWrapper::GASolverKey{});
        res.executionTime = totalTime * 1ms;
        population = nullptr;
        return res;
    }

    SolverAnalysisOutput GASolver::analyze(const SolverAnalysisInput& input)
    {
        double beginning = getTime();
        double averageTime = 0;
        double totalTime = 0;    
        
        checkInput(input.problem, input.GAConfig);

        auto population = dispatchSelectionOperator(input.GAConfig.selectionOperator, [&]<SelectionOperator kSelectionOperator>() -> std::unique_ptr<PopulationWrapper>
        {
            return std::make_unique<Population<kSelectionOperator>>(typename PopulationWrapper::GASolverKey{},
                input.problem, input.GAConfig.populationSize, input.GAConfig.matingCount,
                input.GAConfig.eliteCount, input.GAConfig.mutationProbability,
                input.GAConfig.pointMutationProbability, input.GAConfig.pointMutationCoefficient, 
                input.GAConfig.insertMutationCoefficient, input.GAConfig.invertMutationCoefficient, 
                input.GAConfig.swapMutationCoefficient, input.GAConfig.cycleCrossoverCoefficient,
                input.GAConfig.kpointCrossoverCoefficient, input.GAConfig.OXCrossoverCoefficient,
                input.GAConfig.PMXCrossoverCoefficient, input.GAConfig.uniformCrossoverCoefficient,
                input.GAConfig.k, input.GAConfig.tournamentSize);
        });

        auto it = input.timeLimits.begin();
        SolverAnalysisOutput res;
        SolverOutput previousResult = population->getOutput(typename PopulationWrapper::GASolverKey{});
        std::vector<std::unique_ptr<SolverOutput>> results;
        bool exit = false;
        int c = 0;
        double wasteTime = 0;

        while(true)
        {
            population->evolve(typename PopulationWrapper::GASolverKey{});
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

            previousResult = population->getOutput(typename PopulationWrapper::GASolverKey{});
            previousResult.executionTime = totalTime * 1ms;
            c++;

            wasteTime += getTime() - wasteBeg;
        }
        
        population = nullptr;
        return res;
    }
}