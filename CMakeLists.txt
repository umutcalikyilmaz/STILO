cmake_minimum_required(VERSION 3.20.0)
project(STILO VERSION 1.0.0 LANGUAGES C CXX)
set(PROJECT_NAME "STILO")
set(BINARY_NAME "stilo")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

add_library(${BINARY_NAME} SHARED src/Analyzers/Analyzer.cpp src/Analyzers/InstanceAnalyzer.cpp
src/Analyzers/InstanceReader.cpp src/Analyzers/JobCreator.cpp src/Analyzers/ProblemCreator.cpp
src/Analyzers/SyntheticAnalyzer.cpp src/ProblemModels/IdenticalMachinesSchedulingProblem.cpp
src/ProblemModels/MaxCutProblem.cpp src/ProblemModels/Problem.cpp
src/ProblemModels/TransactionSchedulingProblem.cpp src/ProblemModels/TravelingSalespersonProblem.cpp
src/ProblemModels/WeightedMaxCutProblem.cpp src/Shared/Exceptions.cpp
src/SolverModels/GAModels/Individual.cpp src/SolverModels/GAModels/IndividualPool.cpp
src/Solvers/ACOSolver.cpp src/Solvers/GASolver.cpp src/Solvers/SASolver.cpp src/Solvers/Solver.cpp)

target_compile_features(${BINARY_NAME}
    PUBLIC
        cxx_std_20
)

target_include_directories(${BINARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${BINARY_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1.0.0
  PUBLIC_HEADER include/STILO/STILO.h    
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/STILOConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/STILOConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/STILO
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/STILOConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/STILO)

install(TARGETS ${BINARY_NAME}
  EXPORT STILOTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/STILO
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT STILOTargets
  FILE STILOTargets.cmake
  NAMESPACE STILO::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/STILO)