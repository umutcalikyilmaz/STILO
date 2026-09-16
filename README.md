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


