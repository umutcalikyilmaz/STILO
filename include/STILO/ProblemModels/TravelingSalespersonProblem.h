#pragma once
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    class TravelingSalespersonProblem final : public Problem
    {
    public:

        TravelingSalespersonProblem(const TravelingSalespersonProblem& original);
    
        TravelingSalespersonProblem(int cityCount, const Matrix<double>& distances);

        TravelingSalespersonProblem(int cityCount, ProbabilityDistribution distanceDistribution,
            double distributionParameter1, double distributionParameter2);	

        double getCost(std::span<const int> solution) override;

        double getValue(double cost) override;

        double getScaleFactor() override;

        friend class ProblemCreator;

    private:

        Matrix<double> distances_;
        std::vector<uint8_t> blist_;
        double maxVal_;       
        int cityCount_;

        void getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;

        void getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;
    };
}