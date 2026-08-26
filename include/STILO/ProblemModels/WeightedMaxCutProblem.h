#pragma once
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    class WeightedMaxCutProblem final : public Problem
    {
    public:

        WeightedMaxCutProblem(const WeightedMaxCutProblem& original);

        WeightedMaxCutProblem(int verticeCount, const std::vector<std::vector<double>>& weights);

        WeightedMaxCutProblem(int verticeCount, ProbabilityDistribution weightDistribution,
            double distributionParameter1, double distributionParameter2);

        double getCost(std::span<const int> solution) override;

        double getValue(double cost) override;

        double getScaleFactor() override;

        friend class ProblemCreator;

    private:

        std::vector<std::vector<double>> weights_;
        double maxWeight_;
        double minValue_;

        void getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;

        void getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;

        void calculateMinimumValue();
    };
}