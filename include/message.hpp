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
        "The number of clauses and variables must not exceed the signed 32-bit integer."
    );
    const std::string io_err(
        "An I/O Error was encountered while parsing input."
    );
    const std::string num_clauses_vars(
        "The number of variables and clauses must be positive."
    );
    const std::string solution_format(
        "The solution line is not formatted correctly."
    );
    const std::string solution_type(
        "The solution type does not match the problem type."
    );
    const std::string solution_invalid(
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
    const std::string sol_var_zero(
        "0 is not a valid variable name."
    );
    const std::string repeat_var(
        "A variable appears more than once in the solution."
    );
    const std::string too_many_solutions(
        "Only one solution may be checked."
    );
    const std::string need_solution(
        "Please provide a path to a SOL file."
    );
    const std::string not_open_file(
        "Could not open solution file."
    );


}

#endif
