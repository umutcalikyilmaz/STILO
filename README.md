# STILO
STILO is a metaheuristic framework for strict time-limited optimization. It is designed as a general-purpose optimization environment for a broad range of single-objective discrete problems, including a fine-grained configuration space for multiple algorithm families: ant colony optimization, genetic algorithm, and simulated annealing. During optimization, STILO measures the elapsed CPU time and average iteration duration to terminate execution before a user-defined time limit is exceeded. The framework is also equipped with analysis tools to estimate the solution quality achievable by a given solver configuration under various time limits.

## Installation
STILO is designed for Debian and currently it is not compatible with Windows or MacOS. For standard installation, the following bash instructions should be executed in the project's root folder.

```bash
mkdir build
cd build
cmake ..
sudo make install
```

## Usage
### Importing the Library
After installation, STILO can be imported to a project by adding the following lines in the CMakeLists.txt file.

```cmake
find_package(STILO REQUIRED)
target_link_libraries(my_project STILO::stilo)
```

Note that STILO requires to be compiled using C++20.

The framework headers is included in the project using the code below.

```c++
#include <STILO/STILO.h>
```
### Creating Problem Instances

STILO includes a generic Problem class that serves as a template for any problem class to be integrated into the framework. It also includes 5 classes derived from the base Problem class, each representing a different discrete optimization problem. Instances of these problems can be created by either providing all the necessary information, or providing some problem parameters to be used for random generation of the instance parameters. The derived problem classes included in STILO are introduced below.

#### Identical Machine Scheduling Problem

Identical Machine Scheduling problem aims to find efficient schedules for the processing of jobs on parallel identical machines without additional constraints. The objective is to minimize the makespan, which is the minimum time required for all jobs to be completed.

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

Max-Cut Problem is the problem of dividing the vertices of an undirected graph into two mutually exclusive sets, such that the edge weights between the vertices of different sets are maximized. In STILO, Max-Cut Problem refers to a subclass of this defined problem, in which the weights are constricted to the set {0,1}, For the version of the problem where edges can assume real values, Weighted Max Cut Problem is used.

The example C++ code used to create Max Cut Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::MaxCutProblem mcp(
    verticeCount,   // (int) number of vertices
    edges           // (std::vector<std::vector<uint8_t>>) the connectedness between vertices
);
```

The example C++ code used to create Max Cut Problem instances by randomly generating the weight values in STILO is given below.

```c++
STILO::MaxCutProblem mcp(
    verticeCount,   // (int) number of vertices
    edges           // (std::vector<std::vector<uint8_t>>) edge weights (restricted to 0 and 1)
);
```

#### Weighted Max-Cut Problem

Weighted Max-Cut Problem is a subclass of the Max-Cut Problem, in which edge weights can assume real values.

The example C++ code used to create Weighted Max Cut Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::WeightedMaxCutProblem wmcp(
    verticeCount,   // (int) number of vertices
    weights         // (std::vector<std::vector<double>>) edge weights
);
```

The example C++ code used to create Weighted Max Cut Problem instances by randomly generating the weight values in STILO is given below.

```c++
STILO::WeightedMaxCutProblem wmcp(
    verticeCount,           // (int) number of vertices
    lengthDistribution,     // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random weight generation
    distributionParameter1, // (double) the first parameter used for random length generation
    distributionParameter2  // (double) the second parameter used for random length generation
);
```

#### Transaction Scheduling Problem

Transaction Scheduling Problem is the problem of scheduling jobs to be processed in parallel identical machines, where some jobs are not allowed to be processed concurrently. The objective is to minimize the makespan, which is the minimum time required for all jobs to be completed.

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
    conflictPropensity,     // (double) this parameter is used as the probability of any two jobs conflict during random instance generation
    lengthDistribution,     // (STILO::ProbabilityDistribution) the type of probability distribution to be used for random length generation
    distributionParameter1, // (double) the first parameter used for random length generation
    distributionParameter2  // (double) the second parameter used for random length generation
);
```

#### Traveling Salesperson Problem

Traveling Salesperson Problem is the problem of finding the Hamiltonian cycle with the minimum total weight for a given undirected weighted graph. It can also be thought as finding the minimum length cyclic tour between given cities by visiting each city.

The example C++ code used to create Traveling Salesperson Problem instances using all necessary problem data in STILO is given below.

```c++
STILO::TravelingSalespersonProblem tsp(
    cityCount,  // (int) number of cities
    distances,  // (std::vector<std::vector<double>>) distance matrix
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
    * ACOSolver (Ant Colony Optimization Solver) 
    * GASolver (Genetic Algorithm Solver)
    * SASolver (Simulated Annealing Solver)

Each of them are derived from the base class Solver, and each have the functions to solve a given problem instance using the given solver configurations. The SolverInput class is used as a template whose instances are used as the input to use the solve function of all solvers. The C++ code below explains the steps to use each solver, explaining the types of the configuration parameters. The purpose of each configuration parameter and how it affects the solution process is explained in detail in the referenced paper, and is omitted here.

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

Similar to the SolverInput class, the output of the solvers are standardized using the SolverOutput class. An instance of this class contains the information about the obtained solution and the execution process, as shown below.

```c++
double cost = output.cost;                                    // cost of the best solution found by the solver (natural objective value for minimization problems)
double value = output.value;                                  // value of the best solution found by the solver (natural objective value for maximization problems)
std::vector<int> bestSolution = output.bestSolution;          // the integer string representing the best solution found by the solver
std::chrono::duration executionTime = output.executionTime;   // total execution time
int iterationCount = output.iterationCount;                   // number of iterations used before the time limit is reached
```

### Analyzing Solver Configurations

STILO is also equiped with tools to analyze the relative effectiveness of given set of configurations over synthetically created instance classes and on preset problem instances, under a set of time limits. The two analyzer classes created for this purpose is the SyntheticAnalyzer and InstanceAnalyzer.

#### Configuration Analysis using Synthetic Instances

The Synthetic analyzer class is designed to analyze a given set of configurations using synthetic instances. This class has the analyze member function, which takes an instance of the SyntheticAnalysisInput class as its input. Such an instance contains:
    * The selection of problems to be used in the analysis,
    * The selection of  solvers to be analyzed,
    * The set of problem parameters to create different instance classes of each selected problems,
    * The set of solver hyperparameters to create the different solver configurations to test, 
    * The set of time limits to be used in the analysis,
    * And additional settings for how to perform the analysis. 

An example C++ code snippet showing the use of the SyntheticAnalyzer class is presented below.

```c++
STILO::SyntheticAnalysisInput input;
STILO::SyntheticAnalyzer syntheticAnalyzer;

input.problemSelection.analyzeMCP = true;    // Only MCP is selected for analysis. The other problems are not selected by default.
input.solverSelection.analyzeSA = true;      // Only SA configurations are to be analyzed. Other solvers are not selected by default.

input.problemCount = 10;        // The number of synthetic instances created for each combination of problem parameters.
input.threadCount = 8;          // The number of computation threads used in the analysis. Particularly useful for analyzing large number of solver configurations.

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

The analysis function does not return any result. Instead, the results of the analysis are recorded to a static csv file.

#### Configuration Analysis using Preset Instances

The InstanceAnalyzer class is designed to analyze a given set of configurations using existing instances. With this functionality, it is possible to use benchmark instances for analysis. The InstanceAnalyzer class has the analyze function, which takes an instance of the InstanceAnalysisInput class. The instance of this class contains:
    * The address of the directory to the files that contain the benchmark instances, 
    * The type of the problem that the instances are belong to,
    * The selection of solvers to be analyzed,
    * The set of solver hyperparameters to create the different solver configurations to test, 
    * The set of time limits to be used in the analysis,
    * And additional settings for how to perform the analysis. 
 
 An example C++ code snippet showing the use of the InstanceAnalyzer class is presented below.


    
