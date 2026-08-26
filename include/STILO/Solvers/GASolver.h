#pragma once
#include "STILO/Shared/Dispatch.h"
#include "STILO/Solvers/Solver.h"
#include "STILO/SolverModels/GAModels/Population.h"

namespace STILO
{
    class GASolver final : public Solver
    {
    public:

        SolverOutput solve(const SolverInput& input) override;

        SolverAnalysisOutput analyze(const SolverAnalysisInput& input) override;

    private:

        void checkInput(Problem* problem, const GAConfiguration& config);
    };
}