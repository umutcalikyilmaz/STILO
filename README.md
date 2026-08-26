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
