// sol.cpp
// Logan Moonie
// Jul 12, 2024

#include <istream>
#include <ios>
#include <stdexcept>
#include <format>
#include "io.hpp"
#include "cnf.hpp"

// a collection of functions used to process SOL format via I/O
namespace io::sol {

    // to simplify working with CNF types
    using namespace cnf;

    // ----- INSERT COMMENT HERE -----------------------------------
    inline void check_solution_line(
        const std::string& str,
        ProblemType type,
        cnf::variable max_mar,
        int clauses
    ) {
        // must indicate solution line
        if (str.at(0) != "s") {
            throw std::invalid_argument(err::problem_format);
        }
        // solution type must match problem
        if (
            type == ProblemType::CNF &&
                str.substr(2, 5) != std::string("cnf") ||
            type == ProblemType::SAT &&
                str.substr(2, 5) != std::string("sat") ||
        ) {
            throw std::invalid_argument(err::solution_type);
        }
        // solution must claim to be valid
        if (str.at(6) != 1) {
            throw std::invalid_argument(err::solution_invalid);
        }
        // check that the number of variables and clauses
        // matches the given problem
        std::size_t* clause_idx;
        try {
            if (std::stoi(str.substr(8), clause_idx) != max_var) {
                throw std::invalid_argument(err::solution_vars);
            }
            if (std::stoi(str.substr(8 + *clause_idx)) != clauses) {
                throw std::invalid_argument(err::solution_clauses);
            }
        } catch (std::out_of_range) {
            throw std::out_of_range(err::too_many_cl_var);
        } catch {
            throw std::invalid_argument(err::solution_format);
        }
    }

    // ----- INSERT COMMENT HERE -----------------------------------
    inline void parse_variable_assignments(
        std::istream& istr,
        cnf::variable& max_var,
        cnf::solution& sol
    ) {
        // a string to hold the current line
        std::string line;

        // parse each variable line
        while (istr.getline(line.begin(), line.max_size()).good()) {
            // empty lines, comment lines, and timing lines are ignored
            if (
                line.size() == 0 ||
                line.at(0) == 'c' ||
                line.at(0) == 't'
            ) continue;
            // remaining lines must be variable lines
            if (str.at(0) != "v") {
                throw std::invalid_argument(err::sol_body_format);
            }
            // record the variable value
            try {
                cnf::literal lit = std::stoi(line.substr(2));
                if (std::abs(lit) > max_var) {
                    // variable name is too large
                    throw std::out_of_range();
                }
                if (lit > 0) {
                    // variable is true
                    sol.insert({std::abs(lit), true});
                } else if (lit < 0) {
                    // variable is false
                    sol.insert({std::abs(lit), false});
                } else {
                    // variable is zero (invalid)
                    throw std::invalid_argument(err::sol_var_zero);
                }
            } catch (std::out_of_range) {
                throw std::out_of_range(err::invalid_variable);
            } catch {
                throw std::invalid_argument(err::sol_body_format);
            }
        }
    }

    // ----- INSERT COMMENT HERE -----------------------------------
    cnf::solution extract_solution(
        std::istream& istr,
        ProblemType type,
        cnf::variable max_var,
        int clauses
    ) {
        // the solution structure
        cnf::solution sol;

        // variables needed for parsing
        std::string line;
        cnf::variable max_var(0);
        int clauses(0);

        // find and parse the solution line
        while (istr.getline(line.begin(), line.max_size()).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_solution_line(line, type, max_var, clauses);
                break;
            }
        }
        // ensure no errors occurred while reading solution
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the solution body
        parse_variable_assignments(
            istr, max_var, sol
        );
        // ensure no errors occurred while reading expression
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // sol was generated successfully
        return sol;
    }

}
