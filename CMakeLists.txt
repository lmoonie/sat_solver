# CMakeLists.txt - cpp_project
# Logan Moonie - 2024/06/17

# Project configuration
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(cpp_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Coverage targets
#option(EN_RAND_COV "Enable coverage for random lib" false)

# Enable code sanitization
include(Sanitizers.cmake)

# Coverage pre-compilation configuration
# if(EN_RAND_COV OR EN_RAT_COV)
#     set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set build type" FORCE)
#     include(CodeCoverage.cmake)
#     append_coverage_compiler_flags()
# endif()

# Define libraries
add_library(cnf lib/cnf.cpp)
add_library(sol lib/sol.cpp)
add_library(verify lib/verify.cpp)
add_library(solve lib/solve.cpp lib/orchestrator.cpp)
add_library(solver lib/solver/basic_solver.cpp lib/solver/brute_force.cpp lib/solver/dpll.cpp lib/solver/local_search.cpp lib/solver/cdcl.cpp)

# Include directories
target_include_directories(cnf PUBLIC include)
target_include_directories(sol PUBLIC include)
target_include_directories(verify PUBLIC include)
target_include_directories(solve PUBLIC include)
target_include_directories(solver PUBLIC include)

# Enable external libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(Boost 1.54.0 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

# Define executables
add_executable(test_cnf_input app/test_cnf_input.cpp)
add_executable(test_sol_io app/test_sol_io.cpp)
add_executable(sat_verify app/sat_verify.cpp)
add_executable(sat_solve app/sat_solve.cpp)

# Link the libraries
target_link_libraries(test_cnf_input PRIVATE cnf)
target_link_libraries(test_sol_io PRIVATE sol)
target_link_libraries(sat_verify PRIVATE verify sol cnf Boost::program_options)
target_link_libraries(sat_solve PRIVATE solve solver sol cnf Boost::program_options Threads::Threads)

# Install the executables, demo script, and example files
install(TARGETS sat_verify sat_solve DESTINATION bin)
install(FILES "examples/demo/cnf_1" DESTINATION example)
install (FILES demo DESTINATION bin)

# Generate coverage report
# if(ENABLE_COVERAGE)
#     setup_target_for_coverage_lcov(
#         NAME random_cov
# 	EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/test_random
# 	EXECUTABLE_ARGS ""
# 	LCOV_ARGS -rc lcov_branch_coverage=1
# 	GENHTML_ARGS --branch-coverage
# 	DEPENDENCIES test_random
#     )
#     setup_target_for_coverage_lcov(
#         NAME rational_cov
# 	EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/test_rational
# 	EXECUTABLE_ARGS ""
# 	LCOV_ARGS -rc lcov_branch_coverage=1
# 	GENHTML_ARGS --branch-coverage
# 	DEPENDENCIES test_rational
#     )
# endif()
