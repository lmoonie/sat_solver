// message.hpp
// Logan Moonie
// Jul 12, 2024

#ifndef INC_ERR
#define INC_ERR

// error messages
namespace err {

    const std::string problem_format(
        "the problem line is not formatted correctly."
    );
    const std::string expression_format(
        "the problem expression is not formatted correctly."
    );
    const std::string wrong_number_clauses(
        "the problem expression does not contain the correct number of clauses."
    );
    const std::string invalid_variable(
        "named variables must not exceed the provided maximum"
    );
    const std::string too_many_cl_var(
        "the number of clauses and/or variables must not exceed the signed 32-bit integer."
    );
    const std::string io_err(
        "an I/O Error was encountered while parsing input"
    );
    const std::string num_clauses_vars(
        "the number of variables and/or clauses must be strictly positive."
    );
    const std::string solution_format(
        "the solution line is not formatted correctly."
    );
    const std::string solution_type(
        "the solution type does not match the problem type."
    );
    const std::string invalid_solution(
        "the proposed solution does not claim to be valid."
    );
    const std::string solution_vars(
        "the maximum variable name must match between the problem and solution."
    );
    const std::string solution_clauses(
        "the solution must report the same number of clauses as the problem."
    );
    const std::string sol_body_format(
        "the solution body is not formatted correctly."
    );
    const std::string wrong_variables(
        "the list of variables given in the solution does not match the problem."
    );
    const std::string sol_var_zero(
        "0 is not a valid variable name"
    );
    const std::string repeat_var(
        "a variable appears more than once in the solution"
    );
    const std::string too_many_solutions(
        "more than one solution was provided."
    );
    const std::string too_many_problems(
        "more than one problem was provided"
    );
    const std::string need_file(
        "a path to a solution or problem file was not provided"
    );
    const std::string not_open_file(
        "could not open file; check that file path is correct"
    );
    const std::string repeat_options(
        "one or more program options appears more than once."
    );
    const std::string verbosity(
        "valid verbosity settings are 0, 1, or 2."
    );
    const std::string invalid_solver(
        "the requested solver does not exist."
    );
    const std::string type_mismatch(
        "the type of the problem and solution must be the same."
    );
    const std::string invalid_duration(
        "the provided maximum duration is incorrectly formatted."
    );
    const std::string invalid_memory(
        "the provided maximum memory usage is incorrectly formatted."
    );
    const std::string not_read_mem(
        "the virtual memory usage could not be read from \"/proc/self/stat/\"."
    );
    const std::string mem_too_big(
        "the virtual memory usage could not be read from \"/proc/self/stat/\"."
    );


}

#endif
