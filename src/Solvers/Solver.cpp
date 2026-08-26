#include "STILO/Solvers/Solver.h"

namespace STILO
{
    double Solver::getTime()
    {
        timespec ts{};
  		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);

  		return static_cast<double>(ts.tv_sec) * 1e3 + static_cast<double>(ts.tv_nsec) / 1e6;
    }
}