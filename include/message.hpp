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

// information messages
namespace info {

    const std::string available_formats(
        "The following problem formats are accepted:\n"
        "   CNF\n"
        "   SAT"
    );
    const std::string available_solvers(
        "The following solvers are available:\n"
        "   brute_force\n"
        "   local_search\n"
        "   dpll"
    );
    const std::string verify_description(
        "Usage: sat_verify [[-p] PROBLEM_FILE] [-s SOLUTION_FILE] [-q]\n"
        "Verify the solution to a SAT or CNF problem.\n"
        "Example: sat_verify problem.cnf -s solution.sol\n"
        "\n"
        "At least one of PROBLEM_FILE and SOLUTION_FILE must be provided;\n"
        "the other will be accepted via standard input if only one is given.\n"
        "\n"
        "The following options are accepted"
    );
    const std::string solve_description(
        "Usage: sat_solve [OPTION]...\n"
        "Solve a SAT or CNF problem.\n"
        "Example: sat_verify < problem\n"
        "\n"
        "a problem must be provided via standard input\n"
        "\n"
        "The following options are accepted"
    );

    namespace v_flags {
        const std::string help(
            "display this help text and exit"
        );
        const std::string available_formats(
            "display the accepted problem formats and exit"
        );
        const std::string problem(
            "refer to the problem at the provided file path"
        );
        const std::string solution(
            "verify the solution at the provided file path"
        );
        const std::string quiet(
            "suppress all (non-error) output"
        );
    }

    namespace s_flags {
        const std::string help(
            "display this help text and exit"
        );
        const std::string available_formats(
            "display the accepted problem formats and exit"
        );
        const std::string verbose(
            "set the level of output verbosity; options are\n"
            "   0: print only the solution and errors\n"
            "   1: print basic orchestration info, solution, warnings, and errors\n"
            "       (default if this option is not provided)\n"
            "   2: print detailed orchestration info, solution, warnings, and errors\n"
            "       (default if no argument is provided)"
        );
        const std::string quiet(
            "equivalent to '--verbose 0'"
        );
        const std::string solver(
            "allow only a particular algorithm to be used; see '--list-solvers'"
        );
        const std::string list_solvers(
            "display the available solver algorithms and exit"
        );
        const std::string incomplete(
            "do not require unsatisfiability to be proven; this currently has no effect"
        );
        const std::string threads(
            "the maximum number of threads to use for solving\n"
            "   (default is all available threads)"
        );
        const std::string duration(
            "the (approximate) maximum wall time the solvers may use\n"
            "   must be a whole number followed immediately by\n"
            "   's': seconds\n"
            "   'm': minutes\n"
            "   'h': hours\n"
            "   (default is '5m')"
        );
        const std::string memory(
            "the (approximate) maximum memory the program may use\n"
            "   must be a whole number followed immediately by\n"
            "   'k': kilobytes\n"
            "   'm': megabytes\n"
            "   'g': gigabytes\n"
            "   (default is '2g')"
        );
    }

}

#endif
