#include "STILO/Analyzers/InstanceAnalyzer.h"

namespace STILO
{
    void InstanceAnalyzer::createProblems(std::unique_ptr<Problem> problem)
    {
        problems_.push_back(move(problem));
        
        switch (problemType_)
        {
        case ProblemType::IdenticalMachinesScheduling:
            for(int i = 1; i < threadCount; i++)
            {
                problems_.emplace_back(std::make_unique<IdenticalMachinesSchedulingProblem>(*static_cast<IdenticalMachinesSchedulingProblem*>(problems_[0].get())));
            }
            break;

        case ProblemType::MaxCut:
            for(int i = 1; i < threadCount; i++)
            {
                problems_.emplace_back(std::make_unique<MaxCutProblem>(*static_cast<MaxCutProblem*>(problems_[0].get())));
            }
            break;

        case ProblemType::TransactionScheduling:
            for(int i = 1; i < threadCount; i++)
            {
                problems_.emplace_back(std::make_unique<TransactionSchedulingProblem>(*static_cast<TransactionSchedulingProblem*>(problems_[0].get())));
            }
            break;

        case ProblemType::TravelingSalesperson:
            for(int i = 1; i < threadCount; i++)
            {
                problems_.emplace_back(std::make_unique<TravelingSalespersonProblem>(*static_cast<TravelingSalespersonProblem*>(problems_[0].get())));
            }
            break;

        case ProblemType::WeightedMaxCut:
            for(int i = 1; i < threadCount; i++)
            {
                problems_.emplace_back(std::make_unique<WeightedMaxCutProblem>(*static_cast<WeightedMaxCutProblem*>(problems_[0].get())));
            }
            break;
        
        default:
            throw InvalidParameterException((char*)"Invalid problem type");
        }

        
    }

    void InstanceAnalyzer::runSolverThread(Problem* problem, int solutionCount, bool min)
    {
        std::unique_ptr<Solver> solver;
        std::unique_ptr<Job> job;
        SolverType solverType = (SolverType)4;
        int batchCount = 0;

        while(pullJob(job))
        {
            if(job->solverType != solverType)
            {
                solverType = job->solverType;
                solver = createSolver(solverType);
            }

            JobResult jobResult;
            initializeJob(job.get(), jobResult);

            try
            {
                job->input.problem = problem;           

                for(int i = 0; i < solutionCount; i++)
                {
                    auto output = solver->analyze(job->input);
                    updateJobResult(output, jobResult, min);
                }

                finalizeJobResult(jobResult, solutionCount);
                pushResult(std::move(jobResult));   
            }
            catch(InvalidParameterException& e) { }
            
            displayResult(++batchCount);
        }

        displayFinal(batchCount);
    }

    void InstanceAnalyzer::runThreads()
    {
        initializeRun();
        std::vector<std::jthread> threads;
        threads.reserve(threadCount);

        for(int i = 0; i < threadCount; i++)
        {
            threads.emplace_back(std::jthread([this, problem = problems_[i].get()]()
            {
                this->runSolverThread(problem, runCount, minimization);
            }));
        }

        runAnalyzerThread();
        threads.clear();
    }

    void InstanceAnalyzer::runProblems()
    {
        auto readerResults = reader_.readInstances(problemType_, instanceDirectory_);
        fs::path directoryPath = instanceDirectory_;
        std::string directoryName = directoryPath.filename().string();

        for(auto& result : readerResults)
        {
            std::string fileName = get<1>(result);
            std::cout << "\n\n----Working on " + fileName + "----\n";
            createProblems(move(get<0>(result)));
            createJobs();
            runThreads();
            printRawResults("instance", directoryName, fileName);
            printAnalysisResults("instance", directoryName, fileName);
            problems_.clear();
        }
    }

    void InstanceAnalyzer::analyze(const InstanceAnalysisInput& input)
    {
        solverSelection = input.solverSelection;
        acoConfigurations = input.ACOConfigurations;
        gaConfigurations = input.GAConfigurations;
        saConfigurations = input.SAConfigurations;
        timeLimits = input.timeLimits;
        threadCount = input.threadCount;
        runCount = input.solutionCount;
        minimization = input.problemType == ProblemType::IdenticalMachinesScheduling ||
            input.problemType == ProblemType::TransactionScheduling ||
            input.problemType == ProblemType::TravelingSalesperson;

        instanceDirectory_ = input.instanceDirectory;
        problemType_ = input.problemType;

        runProblems();
    }
}