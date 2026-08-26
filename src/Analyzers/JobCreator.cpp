#include "STILO/Analyzers/JobCreator.h"

namespace STILO
{
    std::vector<std::unique_ptr<Job>> JobCreator::createACOJobs(const ACOConfigurationSet& params)
    {
        std::vector<std::unique_ptr<Job>> jobs;
        jobs_ = &jobs;       
        
        for(GraphStructure gs : params.graphStructures)
        {
            inp_.ACOConfig.graphStructure = gs;

            for(PheromoneCalculationOperator pco : params.pheromoneCalculationOperators)
            {
                inp_.ACOConfig.pheromoneCalculationOperator = pco;

                for(TourSelectionOperator tso  :params.tourSelectionOperators)
                {
                    inp_.ACOConfig.tourSelectionOperator = tso;

                    if(tso == TourSelectionOperator::EveryAnt)
                    {
                        inp_.ACOConfig.eliteCount = 0;
                        createACOSub1(params);
                    }
                    else
                    {
                        for(int en : params.eliteCounts)
                        {
                            inp_.ACOConfig.eliteCount = en;
                            createACOSub1(params);
                        }                        
                    }
                }
            }
        }

        return jobs;
    }

    void JobCreator::createACOSub1(const ACOConfigurationSet& params)
    {
        for(PheromoneUpdateOperator puo : params.pheromoneUpdateOperators)
        {
            inp_.ACOConfig.pheromoneUpdateOperator = puo;

            if(puo == PheromoneUpdateOperator::Classical)
            {
                for(double ip : params.initialPheromones)
                {
                    inp_.ACOConfig.initialPheromone = ip;
                    createACOSub2(params);
                }
            }
            else
            {
                inp_.ACOConfig.initialPheromone = 0;
                createACOSub2(params);
            }
        }
    }

    void JobCreator::createACOSub2(const ACOConfigurationSet& params)
    {
        for(int an : params.antCounts)
        {
            inp_.ACOConfig.antCount = an;

            for(double ep : params.evaporationParameters)
            {
                inp_.ACOConfig.evaporationParameter = ep;

                for(double pi : params.pheromoneInfluences)
                {
                    inp_.ACOConfig.pheromoneInfluence = pi;
    
                    for(double hi : params.heuristicInfluences)
                    {
                        inp_.ACOConfig.heuristicInfluence = hi;

                        for(double pc : params.pheromoneConstants)
                        {
                            inp_.ACOConfig.pheromoneConstant = pc;

                            for(double ldp : params.localDecayParameters)
                            {
                                inp_.ACOConfig.localDecayParameter = ldp;
                                addACOJob();
                            }
                        }
                    }
                }
            }
            
        }
    }    

    void JobCreator::addACOJob()
    {
        jobs_->emplace_back(std::make_unique<Job>());
        jobs_->back()->input.ACOConfig = inp_.ACOConfig;
        jobs_->back()->solverType = SolverType::AntColonyOptimization;
    }

    std::vector<std::unique_ptr<Job>> JobCreator::createGAJobs(const GAConfigurationSet& params)
    {
        std::vector<std::unique_ptr<Job>> jobs;
        jobs_ = &jobs;

        for(SelectionOperator so : params.selectionOperators)
        {
            inp_.GAConfig.selectionOperator = so;

            if(so == SelectionOperator::Tournament)
            {
                for(int ts : params.tournamentSizes)
                {
                    inp_.GAConfig.tournamentSize = ts;
                    createGASub1(params);
                }
            }
            else
            {
                inp_.GAConfig.tournamentSize = 0;
                createGASub1(params);
            }
        }

        return jobs;
    }

    void JobCreator::createGASub1(const GAConfigurationSet& params)
    {
        for(auto cc : params.crossoverConfigurations)
        {
            inp_.GAConfig.cycleCrossoverCoefficient = cc.cycleCrossoverCoefficient;
            inp_.GAConfig.kpointCrossoverCoefficient = cc.kpointCrossoverCoefficient;
            inp_.GAConfig.OXCrossoverCoefficient = cc.OXCrossoverCoefficient;
            inp_.GAConfig.PMXCrossoverCoefficient = cc.PMXCrossoverCoefficient;
            inp_.GAConfig.uniformCrossoverCoefficient = cc.uniformCrossoverCoefficient;

            if(cc.kpointCrossoverCoefficient > 0)
            {
                for(int k : params.kValues)
                {
                    inp_.GAConfig.k = k;
                    createGASub2(params);
                }
            }
            else
            {
                createGASub2(params);
            }
        }
    }

    void JobCreator::createGASub2(const GAConfigurationSet& params)
    {
        for(auto mc : params.mutationConfigurations)
        {
            inp_.GAConfig.insertMutationCoefficient = mc.insertMutationCoefficient;
            inp_.GAConfig.invertMutationCoefficient = mc.invertMutationCoefficient;
            inp_.GAConfig.pointMutationCoefficient = mc.pointMutationCoefficient;
            inp_.GAConfig.swapMutationCoefficient = mc.swapMutationCoefficient;

            if(mc.pointMutationCoefficient > 0)
            {
                for(double pmp : params.pointMutationProbabilities)
                {
                    inp_.GAConfig.pointMutationProbability = pmp;
                    createGASub3(params);
                }
            }
            else
            {
                inp_.GAConfig.pointMutationProbability = 0;
                createGASub3(params);
            }
        }
    }

    void JobCreator::createGASub3(const GAConfigurationSet& params)
    {
        for(int ps : params.populationSizes)
        {
            inp_.GAConfig.populationSize = ps;

            for(double mp : params.mutationProbabilities)
            {
                inp_.GAConfig.mutationProbability = mp;

                for(int mn : params.matingCounts)
                {
                    inp_.GAConfig.matingCount = mn;

                    for(int en : params.eliteCounts)
                    {
                        inp_.GAConfig.eliteCount = en;
                        addGAJob();
                    }
                }
            }
        }
    }

    void JobCreator::addGAJob()
    {
        jobs_->emplace_back(std::make_unique<Job>());
        jobs_->back()->input.GAConfig = inp_.GAConfig;
        jobs_->back()->solverType = SolverType::GeneticAlgorithm;
    }

    std::vector<std::unique_ptr<Job>> JobCreator::createSAJobs(const SAConfigurationSet& params)
    {
        std::vector<std::unique_ptr<Job>> jobs;
        jobs_ = &jobs;

        for(EvolvingParameterType cs : params.coolingSchedules)
        {
            inp_.SAConfig.coolingSchedule = cs;

            if(cs == EvolvingParameterType::Exponential)
            {
                for(double cp : params.coolingParameters)
                {
                    inp_.SAConfig.coolingParameter = cp;
                    createSASub1(params);
                }                
            }
            else
            {
                inp_.SAConfig.coolingParameter = 0;
                createSASub1(params);
            }
        }

        return jobs;
    }

    void JobCreator::createSASub1(const SAConfigurationSet& params)
    {
        for(DistanceCalculationOperator dc : params.distanceCalculationOperators)
        {
            inp_.SAConfig.distanceCalculationOperator = dc;

            for(int mns : params.minimumNeighborhoodSizes)
            {
                inp_.SAConfig.minimumNeighborhoodSize = mns;

                if(dc == DistanceCalculationOperator::Constant)
                {
                    inp_.SAConfig.maximumNeighborhoodSize = inp_.SAConfig.minimumNeighborhoodSize;
                    createSASub2(params);
                }
                else
                {
                    for(int mans : params.maximumNeighborhoodSizes)
                    {
                        inp_.SAConfig.maximumNeighborhoodSize = mans;
                        createSASub2(params);
                    }
                }
            }
        }
    }

    void JobCreator::createSASub2(const SAConfigurationSet& params)
    {
        for(auto mc : params.moveConfigurations)
        {
            inp_.SAConfig.insertMoveCoefficient = mc.insertMoveCoefficient;
            inp_.SAConfig.invertMoveCoefficient = mc.invertMoveCoefficient;
            inp_.SAConfig.pointMoveCoefficient = mc.pointMoveCoefficient;
            inp_.SAConfig.swapMoveCoefficient = mc.swapMoveCoefficient;

            for(double it : params.initialTemperatures)
            {
                inp_.SAConfig.initialTemperature = it;
                addSAJob();
            }
        }
    }

    void JobCreator::addSAJob()
    {
        jobs_->emplace_back(std::make_unique<Job>());
        jobs_->back()->input.SAConfig = inp_.SAConfig;
        jobs_->back()->solverType = SolverType::SimulatedAnnealing;
    }
}