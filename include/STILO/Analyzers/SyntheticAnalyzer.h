#pragma once
#include "STILO/Analyzers/Analyzer.h"
#include "STILO/Analyzers/ProblemCreator.h"

namespace STILO
{
    class SyntheticAnalyzer final : public Analyzer
    {
    public:
        
        void analyze(const SyntheticAnalysisInput& input);

    private:

        ProblemCreator problemCreator_;
        ProblemSelection problemSelection_;
        IMSPConfigurationSet imspConfigurations_;
        MCPConfigurationSet mcpConfigurations_;
        TxnSPConfigurationSet txnspConfigurations_;
        TSPConfigurationSet tspConfigurations_;
        WMCPConfigurationSet wmcpConfigurations_;
        std::fstream ACOFile_;
        std::fstream GAFile_;
        std::fstream SAFile_;
        std::fstream rawFile_;
        std::vector<std::vector<std::unique_ptr<Problem>>> problems_;
        std::stringstream sstream_;
        std::stringstream sstream2_;
        bool useExisting_;
        
        void createIMSP(int jobCount, int machineCount, ProbabilityDistribution lengthDistribution,
            double distributionParameter1, double distributionParameter2);

        void createMCP(int verticeCount, double edgePropensity);

        void createTxnSP(int jobCount, int machineCount, double conflictPropensity,
            ProbabilityDistribution lengthDistribution, double distributionParameter1, double distributionParameter2);

        void createTSP(int cityCount, ProbabilityDistribution distanceDistribution,
            double distributionParameter1, double distributionParameter2);

        void createWMCP(int verticeCount, ProbabilityDistribution weightDistribution,
            double distributionParameter1, double distributionParameter2);

        void runSolverThread(const std::vector<std::unique_ptr<Problem>>& problems, int problemCount, bool min);

        void runThreads();

        void runIMSP(const IMSPConfigurationSet& params);

        void runMCP(const MCPConfigurationSet& params);

        void runTxnSP(const TxnSPConfigurationSet& params);

        void runTSP(const TSPConfigurationSet& params);

        void runWMCP(const WMCPConfigurationSet& params);

        void runProblems();
    };
}