// message.hpp
// Logan Moonie
// Jul 12, 2024

#ifndef INC_ERR
#define INC_ERR

// error messages
namespace err {

    const std::string problem_format(
        "The problem line is not formatted correctly."
    );
    const std::string expression_format(
        "The problem expression is not formatted correctly."
    );
    const std::string wrong_number_clauses(
        "The problem expression does not contain the correct number of clauses."
    );
    const std::string invalid_variable(
        "All named variables must not exceed the provided maximum."
    );
    const std::string too_many_cl_var(
        "The number of clauses and/or variables must not exceed the signed 32-bit integer."
    );
    const std::string io_err(
        "An I/O Error was encountered while parsing input."
    );
    const std::string num_clauses_vars(
        "The number of variables and/or clauses must be strictly positive."
    );
    const std::string solution_format(
        "The solution line is not formatted correctly."
    );
    const std::string solution_type(
        "The solution type does not match the problem type."
    );
    const std::string invalid_solution(
        "The proposed solution does not claim to be valid."
    );
    const std::string solution_vars(
        "The maximum variable name must match between the problem and solution."
    );
    const std::string solution_clauses(
        "The solution must report the same number of clauses as the problem."
    );
    const std::string sol_body_format(
        "The solution body is not formatted correctly."
    );
    const std::string wrong_variables(
        "The list of variables given in the solution does not match the problem."
    );
    const std::string sol_var_zero(
        "0 is not a valid variable name."
    );
    const std::string repeat_var(
        "A variable appears more than once in the solution."
    );
    const std::string too_many_solutions(
        "Only one solution may be checked."
    );
    const std::string too_many_problems(
        "Only one problem may be checked."
    );
    const std::string need_file(
        "Please provide a path to a solution or problem file."
    );
    const std::string not_open_file(
        "Could not open file. Please check that the file path is correct."
    );
    const std::string repeat_options(
        "One or more program options appears more than once."
    );
    const std::string verbosity(
        "Valid verbosity settings are 0, 1, or 2."
    );
    const std::string invalid_solver(
        "The requested solver does not exist."
    );
    const std::string type_mismatch(
        "The type of the problem and solution must be the same."
    );
    const std::string invalid_duration(
        "The provided maximum duration is incorrectly formatted."
    );
    const std::string invalid_duration(
        "The provided maximum memory usage is incorrectly formatted."
    );


}

#endif
