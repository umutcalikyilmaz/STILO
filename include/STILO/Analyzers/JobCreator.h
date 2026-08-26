#pragma once
#include "STILO/Structs/AnalyzerStructs/ACOConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/GAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/SAConfigurationSet.h"
#include "STILO/Structs/AnalyzerStructs/Job.h"

namespace STILO
{
    class JobCreator
    {
    public:

        std::vector<std::unique_ptr<Job>> createACOJobs(const ACOConfigurationSet& params);

        std::vector<std::unique_ptr<Job>> createGAJobs(const GAConfigurationSet& params);

        std::vector<std::unique_ptr<Job>> createSAJobs(const SAConfigurationSet& params);

    private:

        std::vector<std::unique_ptr<Job>>* jobs_;
        SolverAnalysisInput inp_;

        void addACOJob();

        void addGAJob();

        void addSAJob();

        void createACOSub1(const ACOConfigurationSet& params);

        void createACOSub2(const ACOConfigurationSet& params);

        void createGASub1(const GAConfigurationSet& params);

        void createGASub2(const GAConfigurationSet& params);

        void createGASub3(const GAConfigurationSet& params);

        void createSASub1(const SAConfigurationSet& params);

        void createSASub2(const SAConfigurationSet& params);
    };
}