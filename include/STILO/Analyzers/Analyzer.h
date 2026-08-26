#pragma once
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <syncstream>
#include <queue>
#include <thread>
#include <tuple>
#include "STILO/Analyzers/JobCreator.h"
#include "STILO/ProblemModels/IdenticalMachinesSchedulingProblem.h"
#include "STILO/ProblemModels/MaxCutProblem.h"
#include "STILO/ProblemModels/TransactionSchedulingProblem.h"
#include "STILO/ProblemModels/TravelingSalespersonProblem.h"
#include "STILO/ProblemModels/WeightedMaxCutProblem.h"
#include "STILO/Solvers/ACOSolver.h"
#include "STILO/Solvers/GASolver.h"
#include "STILO/Solvers/SASolver.h"
#include "STILO/Structs/AnalyzerStructs/JobResult.h"
#include "STILO/Structs/AnalyzerStructs/SyntheticAnalysisInput.h"

namespace STILO
{
    class Analyzer
    {
    protected:
        
        std::queue<std::unique_ptr<Job>> jobQueue;
        SolverSelection solverSelection;
        ACOConfigurationSet acoConfigurations;
        GAConfigurationSet gaConfigurations;
        SAConfigurationSet saConfigurations;
        std::set<std::chrono::milliseconds> timeLimits;
        int threadCount;
        int runCount;        
        bool minimization;

        void createJobs();

        std::unique_ptr<Solver> createSolver(SolverType solverType);

        bool pullJob(std::unique_ptr<Job>& job);

        void pushResult(JobResult result);

        void initializeJobResult(JobResult& result);

        void initializeJob(Job* job, JobResult& result);

        void updateJobResult(const SolverAnalysisOutput& analysisOutput, JobResult& result, bool minimum);

        void finalizeJobResult(JobResult& result, int count);

        void displayResult(int& batchCount);

        void displayFinal(int batchCount);

        bool compareObjectiveValues(double currentValue, double newValue);

        void runAnalyzerThread();

        void initializeRun();

        std::string getACOString(const ACOConfiguration& acoConfig);

        std::string getGAString(const GAConfiguration& gaConfig);

        std::string getSAString(const SAConfiguration& saConfig);

        void printRawResults(const std::string& analysisType, const std::string& problemClass,
            const std::string& instanceClass);

        void printAnalysisResults(const std::string& analysisType, const std::string& problemClass,
            const std::string& instanceClass);

    private:
        
        std::mutex jobQueueLock_;
        std::mutex resultQueueLock_;
        std::mutex displayLock_;
        std::condition_variable resultQueueCondition_;        
        JobCreator jobCreator_;        
        std::queue<JobResult> resultQueue_;
        std::queue<JobResult> writeQueue_;        
        std::map<std::chrono::milliseconds, std::tuple<double, double, double, int, ACOConfiguration>> bestACOConfigurations_;
        std::map<std::chrono::milliseconds, std::tuple<double, double, double, int, GAConfiguration>> bestGAConfigurations_;
        std::map<std::chrono::milliseconds, std::tuple<double, double, double, int, SAConfiguration>> bestSAConfigurations_;
        int jobCount_;
        int completed_;
        int completedThreadCount_;
    };
}