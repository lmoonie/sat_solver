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
add_library(cnf include/cnf.hpp)
add_library(io include/io.hpp lib/io/cnf.cpp lib/io/sol.cpp)

# Set target language
set_target_properties(cnf PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(io PROPERTIES LINKER_LANGUAGE CXX)

# Include directories
target_include_directories(cnf PUBLIC include)
target_include_directories(io PUBLIC include)

# Enable external libraries
# find_package(Catch2 3 REQUIRED)
find_package(Boost 1.54.0 REQUIRED COMPONENTS)

# Define executables
# add_executable(sat_verify app/sat_verify.cpp)
# add_executable(sat_solve app/sat_solve.cpp)

# Link the libraries
# target_link_libraries(test_ex PRIVATE lib_ex Catch2::Catch2WithMain)
# target_link_libraries(sat_verify PRIVATE lib_ex)
# target_link_libraries(sat_solve PRIVATE lib_ex)

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
