#pragma once
#include "STILO/Shared/Dispatch.h"
#include "STILO/SolverModels/ACOModels/ACOIterator.h"
#include "STILO/Solvers/Solver.h"

namespace STILO
{
    class ACOSolver final : public Solver
    {
    public:

        SolverOutput solve(const SolverInput& input) override;

        SolverAnalysisOutput analyze(const SolverAnalysisInput& input) override;

    private:

        void checkInput(Problem* problem, const ACOConfiguration& config);

        std::unique_ptr<ACOIteratorWrapper> createIterator(GraphStructure graphStructure,
            SolutionSpaceType solutionSpaceType, PheromoneUpdateOperator pheromoneUpdate, 
            TourSelectionOperator tourSelection, PheromoneCalculationOperator pheromoneCalculation,
            Problem* problem, int stateCount, int stringLength, int antCount, int eliteCount,
            double pheromoneConstant, double pheromoneInfluence, double heuristicInfluence,
            double initialPheromone, double evaporationParameter,
            const std::vector<std::vector<std::vector<double>>>& heuristicValues,
            double localDecayParameter);
    };
}