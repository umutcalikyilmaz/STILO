#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include "STILO/ProblemModels/IdenticalMachinesSchedulingProblem.h"
#include "STILO/ProblemModels/MaxCutProblem.h"
#include "STILO/ProblemModels/TransactionSchedulingProblem.h"
#include "STILO/ProblemModels/TravelingSalespersonProblem.h"
#include "STILO/ProblemModels/WeightedMaxCutProblem.h"
#include "STILO/Shared/Parameters.h"
#include "STILO/Shared/Parse.h"

namespace fs = std::filesystem;

namespace STILO
{
    class ProblemCreator
    {
    public:
    
        std::vector<std::unique_ptr<Problem>> createIMSP(int problemCount, bool useExisting, int jobCount,
            int machineCount, ProbabilityDistribution lengthDistribution, double distributionParanmeter1,
            double distributionParameter2);

        std::vector<std::unique_ptr<Problem>> createMCP(int problemCount, bool useExisting, int verticeCount,
            double edgePropensity);

        std::vector<std::unique_ptr<Problem>> createTxnSP(int problemCount, bool useExisting, int jobCount,
            int machineCount, double conflictPropensity, ProbabilityDistribution lengthDistribution,
            double distributionParameter1, double distributionParameter2);

        std::vector<std::unique_ptr<Problem>> createTSP(int problemCount, bool useExisting, int cityCount,
            ProbabilityDistribution distanceDistribution, double distributionParameter1,
            double distributionParameter2);

        std::vector<std::unique_ptr<Problem>> createWMCP(int problemCount, bool useExisting, int verticeCount,
            ProbabilityDistribution weightDistribution, double distributionParameter1,
            double distributionParameter2);

    private:

        std::fstream file_;

        void openFile(std::string problem, std::string fileName, bool in);

        std::unique_ptr<Problem> pullIMSP(int jobCount, int machineCount, const std::string& line);

        std::unique_ptr<Problem> pushIMSP(int jobCount, int machineCount,
            ProbabilityDistribution lengthDistribution, double distributionParanmeter1,
            double distributionParameter2);

        std::unique_ptr<Problem> pullMCP(int verticeCount, const std::string& line);

        std::unique_ptr<Problem> pushMCP(int verticeCount, double edgePropensity);

        std::unique_ptr<Problem> pullTxnSP(int jobCount, int machineCount, const std::string& line);

        std::unique_ptr<Problem> pushTxnSP(int jobCount, int machineCount, double conflictPropensity,
            ProbabilityDistribution lengthDistribution, double distributionParameter1,
            double distributionParameter2);

        std::unique_ptr<Problem> pullTSP(int cityCount, const std::string& line);

        std::unique_ptr<Problem> pushTSP(int cityCount, ProbabilityDistribution distanceDistribution,
            double distributionParameter1, double distributionParameter2);

        std::unique_ptr<Problem> pullWMCP(int verticeCount, const std::string& line);

        std::unique_ptr<Problem> pushWMCP(int verticeCount, ProbabilityDistribution weightDistribution,
            double distributionParameter1, double distributionParameter2);
    };
}