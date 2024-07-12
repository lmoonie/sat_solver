// io.hpp
// Logan Moonie
// Jul 12, 2024

namespace io {

    // extract_cnf_problem accepts a CNF-formatted problem
    // specification via an input stream and returns a cnf_expr.
    // It throws exceptions if the provided input cannot be read.
    cnf::cnf_expr extract_cnf_problem(std::istream&);

    // a collection of error messages related to I/O processing
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
            "An I/O Error was encountered while parsing the problem."
        );
        const std::string num_clauses_vars(
            "The number of variables and clauses must be positive."
        );
        
    }

}
