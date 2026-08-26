#pragma once
#include "STILO/Shared/Dispatch.h"
#include "STILO/Solvers/Solver.h"
#include "STILO/SolverModels/SAModels/SAIterator.h"

namespace STILO
{
    class SASolver final : public Solver
    {
    public:

        SolverOutput solve(const SolverInput& input) override;

        SolverAnalysisOutput analyze(const SolverAnalysisInput& input) override;

    private:

        void checkInput(Problem* problem, const SAConfiguration& config);

        std::unique_ptr<SAIteratorWrapper> createIterator(EvolvingParameterType cooling,
            DistanceCalculationOperator distanceCalculation, Problem* problem,
            int initialTemperature, int minimumNeighorhoodSize, int maximumNeighorhoodSize,
            double coolingParameter, double pointMoveCoefficient, double insertMoveCoefficient,
            double invertMoveCoefficient, double swapMoveCoefficient);
    };
}