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
input.SAConfig.maximumNeighborhoodSize = maximumNeighborhoodSize;            // int
input.SAConfig.minimumNeighborhoodSize = minimumNeighborhoodSize;            // int

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
