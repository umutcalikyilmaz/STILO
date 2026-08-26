#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include "STILO/ProblemModels/IdenticalMachinesSchedulingProblem.h"
#include "STILO/ProblemModels/MaxCutProblem.h"
#include "STILO/ProblemModels/TransactionSchedulingProblem.h"
#include "STILO/ProblemModels/TravelingSalespersonProblem.h"
#include "STILO/ProblemModels/WeightedMaxCutProblem.h"
#include "STILO/Shared/Exceptions.h"
#include "STILO/Shared/Parameters.h"

namespace fs = std::filesystem;

namespace STILO
{
    class InstanceReader
    {
    public:
    
        std::vector<std::tuple<std::unique_ptr<Problem>, std::string, double>> readInstances(ProblemType pt, std::string instanceDirectory);

        ProblemType getProblemType();

    private:

        ProblemType lastProbemType_;
        std::tuple<std::unique_ptr<Problem>, std::string, double> (InstanceReader::*readFilePtr_)(std::fstream&, std::string&);

        std::tuple<std::unique_ptr<Problem>, std::string, double> readIMSP(std::fstream& file, std::string& instanceName);

        std::tuple<std::unique_ptr<Problem>, std::string, double> readMCP(std::fstream& file, std::string& instanceName);

        std::tuple<std::unique_ptr<Problem>, std::string, double> readTxnSP(std::fstream& file, std::string& instanceName);

        std::tuple<std::unique_ptr<Problem>, std::string, double> readTSP(std::fstream& file, std::string& instanceName);

        std::tuple<std::unique_ptr<Problem>, std::string, double> readWMCP(std::fstream& file, std::string& instanceName);
    };
}