# STILO
STILO is a metaheuristic framework for strict time-limited optimization. It is designed as a general-purpose optimization environment for a broad range of single-objective discrete problems, including a fine-grained configuration space for multiple algorithm families: ant colony optimization, genetic algorithm, and simulated annealing. During optimization, STILO measures the elapsed CPU time and average iteration duration to terminate execution before a user-defined time limit is exceeded. The framework is also equipped with analysis tools to estimate the solution quality achievable by a given solver configuration under various time limits.


## Reference

For a detailed description of STILO, its algorithm configuration spaces, and the experimental evaluation, see:

**[A Metaheuristic Optimization Framework for Discrete Optimization under Strict Time Limits](https://arxiv.org/abs/2609.18702)**

## Citation

If you use STILO in your research, please cite:

```bibtex
@article{calikyilmaz2026metaheuristic,
  title={A Metaheuristic Optimization Framework for Discrete Optimization under Strict Time Limits},
  author={{\c{C}}al{\i}ky{\i}lmaz, Umut and Nayak, Nitin and Groppe, Sven},
  journal={arXiv preprint arXiv:2609.18702},
  year={2026}
}
```

## Installation
STILO is designed for Debian-based systems and is currently not compatible with Windows or macOS. STILO requires C++20. For standard installation, the following bash instructions should be executed in the project's root folder.

```bash
mkdir build
cd build
cmake ..
sudo make install
```

## Quick Start

The following example creates a small Identical Machine Scheduling Problem and solves it using simulated annealing under a 100 ms time limit.

```c++
#include <STILO/STILO.h>

#include <chrono>
#include <vector>

int main()
{
    std::vector<double> lengths = {10.0, 8.0, 7.0, 6.0, 4.0};

    STILO::IdenticalMachinesSchedulingProblem problem(
        5,          // number of jobs
        2,          // number of machines
        lengths     // job lengths
    );

    STILO::SASolver solver;
    STILO::SolverInput input;

    input.problem = &problem;
    input.timeLimit = std::chrono::milliseconds(100);

    input.SAConfig.coolingSchedule = STILO::EvolvingParameterType::Fast;
    input.SAConfig.distanceCalculationOperator = STILO::DistanceCalculationOperator::Gaussian;
    input.SAConfig.pointMoveCoefficient = 1.0;
    input.SAConfig.insertMoveCoefficient = 0.0;
    input.SAConfig.invertMoveCoefficient = 0.0;
    input.SAConfig.swapMoveCoefficient = 0.0;
    input.SAConfig.initialTemperature = 2.0;
    input.SAConfig.coolingParameter = 0.99;
    input.SAConfig.minimumNeighborhoodSize = 1;
    input.SAConfig.maximumNeighborhoodSize = 2;

    STILO::SolverOutput output = solver.solve(input);

    double bestCost = output.cost;
}
```

More detailed examples for creating problem instances, configuring solvers, and analyzing solver configurations are provided in the sections below.


## Usage
### Importing the Library
After installation, STILO can be imported to a project by adding the following lines in the `CMakeLists.txt` file.

```cmake
find_package(STILO REQUIRED)
target_link_libraries(my_project STILO::stilo)
```

The framework headers can be included using:

```c++
#include <STILO/STILO.h>
```
### Creating Problem Instances

STILO includes a generic `Problem` class that serves as a base for any problem to be integrated into the framework. It also includes five classes derived from the base `Problem` class, each representing a different discrete optimization problem. Instances can be created either from complete problem data or by providing parameters from which STILO randomly generates an instance. The derived problem classes included in STILO are introduced below.

#### Identical Machine Scheduling Problem

The Identical Machine Scheduling Problem (IMSP) assigns jobs to identical parallel machines with the objective of minimizing the makespan, i.e., the completion time of the last job.

The example C++ code used to create Identical Machine Scheduling Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::IdenticalMachinesSchedulingProblem imsp(
    jobCount,       // (int) number of jobs
    machineCount,   // (int) number of machines
    lengths         // (std::vector<double>) vector of jobLengths
);
```
The example C++ code used to create Identical Machine Scheduling Problem instances by randomly generating the length values in STILO is given below.

```c++
STILO::IdenticalMachinesSchedulingProblem imsp(
    jobCount,               // (int) number of jobs
    machineCount,           // (int) number of machines
    lengthDistribution,     // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random length generation
    distributionParameter1, // (double) the first parameter used for random length generation
    distributionParameter2  // (double) the second parameter used for random length generation
);
```

#### Max-Cut Problem

The Max-Cut Problem divides the vertices of an undirected graph into two disjoint sets such that the total weight of the edges connecting vertices in different sets is maximized. In STILO, `MaxCutProblem` represents the version in which edge weights are restricted to 0 and 1.

The example C++ code used to create Max-Cut Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::MaxCutProblem mcp(
    vertexCount,   // (int) number of vertices
    edges           // (std::vector<std::vector<uint8_t>>) edge weights (restricted to 0 and 1)
);
```

The example C++ code used to create Max-Cut Problem instances by randomly generating the edge weights in STILO is given below.

```c++
STILO::MaxCutProblem mcp(
    vertexCount,   // (int) number of vertices
    edgeDensity     // (double) used as the probability of assigning 1 as the weight value to an edge during random weight generation
);
```

#### Weighted Max-Cut Problem

The Weighted Max-Cut Problem extends the Max-Cut formulation by allowing edge weights to assume real values.

The example C++ code used to create Weighted Max-Cut Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::WeightedMaxCutProblem wmcp(
    vertexCount,   // (int) number of vertices
    weights         // (std::vector<std::vector<double>>) edge weights
);
```

The example C++ code used to create Weighted Max-Cut Problem instances by randomly generating the edge weights in STILO is given below.

```c++
STILO::WeightedMaxCutProblem wmcp(
    vertexCount,           // (int) number of vertices
    weightDistribution,     // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random weight generation
    distributionParameter1, // (double) the first parameter used for random weight generation
    distributionParameter2  // (double) the second parameter used for random weight generation
);
```

#### Transaction Scheduling Problem

Transaction Scheduling Problem is the problem of scheduling jobs for processing on identical parallel machines subject to conflicts that prevent certain pairs of jobs from being processed concurrently. The objective is to minimize the makespan, which is the minimum time required for all jobs to be completed.

The example C++ code used to create Transaction Scheduling Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::TransactionSchedulingProblem txnsp(
    jobCount,       // (int) number of jobs
    machineCount,   // (int) number of machines
    lengths,        // (std::vector<double>) job lengths
    conflicts       // (std::vector<std::vector<uint8_t>>) conflict matrix
);
```

The example C++ code used to create Transaction Scheduling Problem instances by randomly generating the length values and the conflict matrix in STILO is given below.

```c++
STILO::TransactionSchedulingProblem txnsp(
    jobCount,               // (int) number of jobs
    machineCount,           // (int) number of machines
    conflictPropensity,     // (double) this parameter is used as the probability that any two jobs conflict during random instance generation
    lengthDistribution,     // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random length generation
    distributionParameter1, // (double) the first parameter used for random length generation
    distributionParameter2  // (double) the second parameter used for random length generation
);
```

#### Traveling Salesperson Problem

Traveling Salesperson Problem is the problem of finding the Hamiltonian cycle with the minimum total weight for a given undirected weighted graph. It can also be viewed as finding the minimum length cyclic tour between given cities by visiting each city.

The example C++ code used to create Traveling Salesperson Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::TravelingSalespersonProblem tsp(
    cityCount,  // (int) number of cities
    distances   // (std::vector<std::vector<double>>) distance matrix
);
```

The example C++ code used to create Traveling Salesperson Problem instances by randomly generating the distance values in STILO is given below.

```c++
STILO::TravelingSalespersonProblem tsp(
    cityCount,  // (int) number of cities
    distanceDistribution,   // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random distance generation
    distributionParameter1, // (double) the first parameter used for random distance generation
    distributionParameter2  // (double) the second parameter used for random distance generation
);
```

### Solving Problem Instances

STILO contains three solver classes:
- `ACOSolver` (Ant Colony Optimization Solver)
- `GASolver` (Genetic Algorithm Solver)
- `SASolver` (Simulated Annealing Solver)

Each solver is derived from the base `Solver` class and provides a `solve()` member function for solving a given problem instance under a specified configuration. Solver inputs are standardized through the `SolverInput` class. The examples below demonstrate how to configure and use each solver. The purpose and behavior of the individual configuration parameters are described in detail in the referenced paper.

```c++
STILO::ACOSolver acoSolver;
STILO::SolverInput input;
    
input.problem = &problemInstance;   // Problem*
input.timeLimit = timeLimit;        // std::chrono::milliseconds

input.ACOConfig.graphStructure = graphStructure;                                // STILO::GraphStructure (enum)
input.ACOConfig.tourSelectionOperator = tourSelectionOperator;                  // STILO::TourSelectionOperator (enum)
input.ACOConfig.pheromoneCalculationOperator = pheromoneCalculationOperator;    // STILO::PheromoneCalculationOperator (enum)
input.ACOConfig.pheromoneUpdateOperator = pheromoneUpdateOperator;              // STILO::PheromoneUpdateOperator (enum)
input.ACOConfig.antCount = antCount;                                            // int
input.ACOConfig.eliteCount = eliteCount;                                        // int
input.ACOConfig.evaporationParameter = evaporationParameter;                    // double
input.ACOConfig.pheromoneInfluence = pheromoneInfluence;                        // double
input.ACOConfig.heuristicInfluence = heuristicInfluence;                        // double
input.ACOConfig.initialPheromone = initialPheromone;                            // double
input.ACOConfig.pheromoneConstant = pheromoneConstant;                          // double
input.ACOConfig.localDecayParameter = localDecayParameter;                      // double
    
STILO::SolverOutput output = acoSolver.solve(input);
```

```c++
STILO::GASolver gaSolver;
STILO::SolverInput input;
    
input.problem = &problemInstance;   // Problem*
input.timeLimit = timeLimit;        // std::chrono::milliseconds
    
input.GAConfig.selectionOperator = selectionOperator;                         // STILO::SelectionOperator (enum)
input.GAConfig.cycleCrossoverCoefficient = cycleCrossoverCoefficient;         // double
input.GAConfig.kpointCrossoverCoefficient = kpointCrossoverCoefficient;       // double      
input.GAConfig.PMXCrossoverCoefficient = PMXCrossoverConstant;                // double
input.GAConfig.OXCrossoverCoefficient = OXCrossoverCoefficient;               // double
input.GAConfig.uniformCrossoverCoefficient = uniformCrossoverCoefficient;     // double
input.GAConfig.pointMutationCoefficient = pointMutationCoefficient;           // double
input.GAConfig.insertMutationCoefficient = insertMutationCoefficient;         // double
input.GAConfig.invertMutationCoefficient = invertMutationCoefficient;         // double
input.GAConfig.swapMutationCoefficient = swapMutationCoefficient;             // double
input.GAConfig.populationSize = populationSize;                               // int
input.GAConfig.matingCount = matingCount;                                     // int
input.GAConfig.mutationProbability = mutationProbability;                     // double
input.GAConfig.eliteCount = eliteCount;                                       // int
input.GAConfig.tournamentSize = tournamentSize;                               // int
input.GAConfig.k = kValue;                                                    // int
input.GAConfig.pointMutationProbability = pointMutationProbability;           // double
    
STILO::SolverOutput output = gaSolver.solve(input);
```

```c++
STILO::SASolver saSolver;
STILO::SolverInput input;

input.problem = &problemInstance;   // Problem*
input.timeLimit = timeLimit;        // std::chrono::milliseconds

input.SAConfig.coolingSchedule = coolingSchedule;                            // STILO::EvolvingParameterType (enum)
input.SAConfig.distanceCalculationOperator = distanceCalculationOperator;    // STILO::DistanceCalculationOperator (enum)
input.SAConfig.pointMoveCoefficient = pointMoveCoefficient;                  // double
input.SAConfig.insertMoveCoefficient = insertMoveCoefficient;                // double
input.SAConfig.invertMoveCoefficient = invertMoveCoefficient;                // double
input.SAConfig.swapMoveCoefficient = swapMoveCoefficient;                    // double
input.SAConfig.initialTemperature = initialTemperature;                      // double
input.SAConfig.coolingParameter = coolingParameter;                          // double
input.SAConfig.minimumNeighborhoodSize = minimumNeighborhoodSize;            // int
input.SAConfig.maximumNeighborhoodSize = maximumNeighborhoodSize;            // int

STILO::SolverOutput output = saSolver.solve(input); 
```

Solver outputs are standardized through the `SolverOutput` class. An instance of this class contains information about the obtained solution and the execution process, as shown below.

```c++
double cost = output.cost;                                    // cost of the best solution found by the solver (natural objective value for minimization problems)
double value = output.value;                                  // value of the best solution found by the solver (natural objective value for maximization problems)
std::vector<int> bestSolution = output.bestSolution;          // the integer string representing the best solution found by the solver
auto executionTime = output.executionTime;                    // total execution time
int iterationCount = output.iterationCount;                   // number of iterations used before the time limit is reached
```

### Analyzing Solver Configurations

STILO is also equipped with tools for analyzing the relative effectiveness of a given set of configurations across synthetic instance classes and predefined problem instances under different time limits. The two analyzer classes created for this purpose are the `SyntheticAnalyzer` and `InstanceAnalyzer`.

#### Configuration Analysis using Synthetic Instances

The `SyntheticAnalyzer` class is designed to analyze a given set of configurations using synthetic instances. This class provides an `analyze()` member function, which takes an instance of the `SyntheticAnalysisInput` class as its input. Such an instance contains:
- The selection of problems to be used in the analysis,
- The selection of  solvers to be analyzed,
- The set of problem parameters to create different instance classes of each selected problem,
- The set of solver hyperparameters to create the different solver configurations to test, 
- The set of time limits to be used in the analysis,
- Additional settings controlling how the analysis is performed.

An example C++ code snippet showing the use of the `SyntheticAnalyzer` class is presented below.

```c++
STILO::SyntheticAnalysisInput input;
STILO::SyntheticAnalyzer syntheticAnalyzer;

input.problemSelection.analyzeMCP = true;    // Only MCP is selected for analysis. The other problems are not selected by default.
input.solverSelection.analyzeSA = true;      // Only SA configurations are to be analyzed. Other solvers are not selected by default.

input.problemCount = 10;        // The number of synthetic instances created for each combination of problem parameters.
input.threadCount = 8;          // The number of computation threads used in the analysis. Particularly useful for analyzing large numbers of solver configurations.

for(int i = 50; i <= 500; i+=5)
{
    input.timeLimits.insert(std::chrono::milliseconds(i));        // The set of time limits are created. In the analysis, the solution quality under each time limit
}                                                                 // is evaluated for each solver configuration.

input.MCPConfigurations.vertexCounts.insert(50);            // Various problem parameters to generate synthetic problem instances are inserted. Solver configurations 
input.MCPConfigurations.vertexCounts.insert(100);           // are tested separately for each combination of problem parameters.
input.MCPConfigurations.edgePropensities.insert(0.25);
input.MCPConfigurations.edgePropensities.insert(0.50);

input.SAConfigurations.coolingSchedules.insert(STILO::EvolvingParameterType::Fast);                          // Various hyperparameters to create solver configurations are
input.SAConfigurations.coolingSchedules.insert(STILO::EvolvingParameterType::Exponential);                   // inserted. A configuration is created for each combination of
input.SAConfigurations.distanceCalculationOperators.insert(STILO::DistanceCalculationOperator::Gaussian);    // hyperparameters. The combinations that do not constitute 
input.SAConfigurations.distanceCalculationOperators.insert(STILO::DistanceCalculationOperator::Cauchy);      // valid configurations are automatically eliminated.
input.SAConfigurations.moveConfigurations.insert(STILO::MoveConfiguration(1,0,0,0));                        
input.SAConfigurations.moveConfigurations.insert(STILO::MoveConfiguration(1,1,0,0));
input.SAConfigurations.initialTemperatures.insert(1.2);
input.SAConfigurations.initialTemperatures.insert(2);
input.SAConfigurations.coolingParameters.insert(0.99);
input.SAConfigurations.minimumNeighborhoodSizes.insert(1);
input.SAConfigurations.maximumNeighborhoodSizes.insert(2);
input.SAConfigurations.maximumNeighborhoodSizes.insert(3);

syntheticAnalyzer.analyze(input);
```

Here, `analyze()` does not return the analysis results directly. Instead, the results are written to CSV files in `~/.stilo/synthetic/analysis`. A separate file is created for each pair of problem configuration and solver. Each file contains the results and the hyperparameters of the best configurations identified for each time limit.

#### Configuration Analysis using Predefined Instances

The `InstanceAnalyzer` class is designed to analyze solver configurations using predefined problem instances, including benchmark instances. It provides an `analyze()` member function, which takes an instance of the `InstanceAnalysisInput` class. Such an instance contains:
- The directory containing the problem instances, 
- The type of the problem to which the instances belong,
- The selection of solvers to be analyzed,
- The set of solver hyperparameters to create the different solver configurations to test, 
- The set of time limits to be used in the analysis,
- Additional settings controlling how the analysis is performed.
 
 An example C++ code snippet showing the use of the `InstanceAnalyzer` class is presented below.

```c++
STILO::InstanceAnalysisInput input;
STILO::InstanceAnalyzer instanceAnalyzer;

input.problemType = STILO::ProblemType::TravelingSalesperson;    // Problem type for the predefined instances
input.instanceDirectory = "/home/user/TSPInstances";             // The directory containing the instance files
input.solverSelection.analyzeSA = true;                          // Only SA configurations are to be analyzed. Other solvers are not selected by default.

input.solutionCount = 10;       // Number of times each instance is solved using each solver configuration. Average of the objective values obtained is used for analysis.
input.threadCount = 8;          // The number of computation threads used in the analysis. Particularly useful for analyzing large numbers of solver configurations.

for(int i = 50; i <= 500; i+=5)
{
    input.timeLimits.insert(std::chrono::milliseconds(i));        // The set of time limits are created. In the analysis, the solution quality under each time limit
}                                                                 // is evaluated for each solver configuration.

input.SAConfigurations.coolingSchedules.insert(STILO::EvolvingParameterType::Fast);                          // Various hyperparameters to create solver configurations are
input.SAConfigurations.coolingSchedules.insert(STILO::EvolvingParameterType::Exponential);                   // inserted. A configuration is created for each combination of
input.SAConfigurations.distanceCalculationOperators.insert(STILO::DistanceCalculationOperator::Gaussian);    // hyperparameters. The combinations that do not constitute 
input.SAConfigurations.distanceCalculationOperators.insert(STILO::DistanceCalculationOperator::Cauchy);      // valid configurations are automatically eliminated.
input.SAConfigurations.moveConfigurations.insert(STILO::MoveConfiguration(1,0,0,0));                        
input.SAConfigurations.moveConfigurations.insert(STILO::MoveConfiguration(1,1,0,0));
input.SAConfigurations.initialTemperatures.insert(1.2);
input.SAConfigurations.initialTemperatures.insert(2);
input.SAConfigurations.coolingParameters.insert(0.99);
input.SAConfigurations.minimumNeighborhoodSizes.insert(1);
input.SAConfigurations.maximumNeighborhoodSizes.insert(2);
input.SAConfigurations.maximumNeighborhoodSizes.insert(3);

instanceAnalyzer.analyze(input);
```


Here, `analyze()` does not return the analysis results directly. Instead, the results are written to CSV files in `~/.stilo/instance/analysis`. A separate file is created for each pair of problem instance and solver. Each file contains the results and the hyperparameters of the best configurations identified for each time limit.

