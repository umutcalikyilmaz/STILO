#pragma once
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    class TransactionSchedulingProblem final : public Problem
    {
    public:

        TransactionSchedulingProblem(const TransactionSchedulingProblem& original);

        TransactionSchedulingProblem(int jobCount, int machineCount, const std::vector<double>& lengths,
            const Matrix<uint8_t>& conflicts);

		TransactionSchedulingProblem(int jobCount, int machineCount, double conflictPropensity,
            ProbabilityDistribution lengthDistribution, double distributionParameter1,
            double distributionParameter2);

        double getCost(std::span<const int> solution) override;

        double getValue(double cost) override;

        double getScaleFactor() override;

        friend class ProblemCreator;

    private:

        std::vector<double> lengths_;
        Matrix<uint8_t> conflicts_;
        std::vector<double> processingTimes_;
        std::vector<int> lastJobs_;
        std::vector<int> machineIndices_;
        std::vector<uint8_t> blist_;
        int machineCount_;

        void getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;

        void getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;
    };
}