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
add_library(message include/message.hpp)
add_library(verify lib/verify.cpp)

# Set target language
set_target_properties(message PROPERTIES LINKER_LANGUAGE CXX)

# Include directories
target_include_directories(cnf PUBLIC include)
target_include_directories(sol PUBLIC include)
target_include_directories(message PUBLIC include)
target_include_directories(verify PUBLIC include)

# Enable external libraries
# find_package(Catch2 3 REQUIRED)
find_package(Boost 1.54.0 REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

# Define executables
add_executable(test_cnf_input app/test_cnf_input.cpp)
add_executable(test_sol_io app/test_sol_io.cpp)
add_executable(sat_verify app/sat_verify.cpp)

# Link the libraries
target_link_libraries(test_cnf_input PRIVATE cnf)
target_link_libraries(test_sol_io PRIVATE sol)
target_link_libraries(sat_verify PRIVATE verify sol cnf message)

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
