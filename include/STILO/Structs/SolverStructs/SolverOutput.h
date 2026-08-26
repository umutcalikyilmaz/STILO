#pragma once
#include <chrono>
#include <vector>

namespace STILO
{
    struct SolverOutput
    {
        std::vector<int> bestSolution;
        std::chrono::duration<double, std::milli> executionTime;
        double cost;
        double value;
        int iterationCount;

        SolverOutput(double cost, double value, const std::vector<int>& bestSolution, int iterationCount) 
            : cost(cost),
              value(value),
              bestSolution(bestSolution),
              iterationCount(iterationCount) { }
    };    
}