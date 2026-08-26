#pragma once
#include "STILO/ProblemModels/Problem.h"

namespace STILO
{
    class IdenticalMachinesSchedulingProblem final : public Problem
    {
    public:

        IdenticalMachinesSchedulingProblem(const IdenticalMachinesSchedulingProblem& original);

        IdenticalMachinesSchedulingProblem(int jobCount, int machineCount, std::vector<double>& lengths);

        IdenticalMachinesSchedulingProblem(int jobCount, int machineCount,
            ProbabilityDistribution lengthDistribution, double distributionParameter1,
            double distributionParameter2);

        double getCost(std::span<const int> solution) override;

        double getValue(double cost) override;        

        double getScaleFactor() override;

        friend class ProblemCreator;

    private:

        std::vector<double> lengths_;
        std::vector<double> times_;        
        double maxCost_;

        void getHeuristicValuesMultiStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;

        void getHeuristicValuesSingleStage(std::vector<std::vector<std::vector<double>>>& heuristic) override;
    };    
}