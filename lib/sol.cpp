
#include <map>
#include <istream>
#include <ostream>
#include <ios>
#include <stdexcept>
#include <format>
#include "sol.hpp"

namespace sol {

    // will need abs frequently
    using std::abs;

    // input stream constructor
    solution::solution(std::istream& istr) {
        io::extract_solution(*this, istr);
    }

    // assign a variable
    void solution::assign_variable(variable var, bool val) {
        variables.insert({variable, val});
    }

    // unassign a variable
    void solution::unassign_variable(variable, bool) {
        variables.erase(variable);
    }

    // give the number of assigned variables
    std::size_t solution::size() {
        return variables.size();
    }

    // used to print solution
    std::ostream& operator<<(std::ostream&, const solution&);

}

// a collection of functions used to process SOL format via I/O
namespace sol::io {

    // Given a suspected solution line, parse_solution_line
    // extracts the number of variables and clauses, throwing
    // exceptions if the line is not formatted correctly.
    inline void parse_solution_line(
        const std::string& str,
        solution& sol,
        variable& max_var,
        clause& clauses
    ) {
        // must indicate solution line
        if (str.at(0) != 's') {
            throw std::invalid_argument(err::problem_format);
        }
        try {
            // set solution type
            if (str.substr(2, 5) == std::string("cnf")) {
                sol.type = ProblemType::CNF;
            } else if (str.substr(2, 5) == std::string("sat")) {
                sol.type = ProblemType::SAT;
            } else {
                throw std::invalid_argument(err::solution_format);
            }
            // solution must claim to be valid
            if (str.at(6) != 1) {
                throw std::invalid_argument(err::solution_invalid);
            }
        } catch (...) {
            throw std::invalid_argument(err::solution_format);
        }
        // set the number of variables and clauses
        std::size_t clause_idx;
        try {
            max_var = (std::stoi(str.substr(8), &clause_idx));
            clauses = std::stoi(str.substr(8 + clause_idx));
        } catch (std::out_of_range) {
            throw std::out_of_range(err::too_many_cl_var);
        } catch (...) {
            throw std::invalid_argument(err::solution_format);
        }
    }

    // Given a solution body (via input stream), parse_solution_body
    // extracts it contents into a solution object, throwing errors if
    // the solution body is not formatted correctly.
    inline void parse_solution_body(
        std::istream& istr,
        solution& sol,
        variable& max_var
    ) {
        // a string to hold the current line
        std::string line;

        // parse each variable line
        while (std::getline(istr, line).good()) {
            // empty lines, comment lines, and timing lines are ignored
            if (
                line.size() == 0 ||
                line.at(0) == 'c' ||
                line.at(0) == 't'
            ) continue;
            // remaining lines must be variable lines
            if (line.at(0) != 'v') {
                throw std::invalid_argument(err::sol_body_format);
            }
            // record the variable value
            try {
                literal lit = std::stoi(line.substr(2));
                if (std::abs(lit) > max_var) {
                    // variable name is too large
                    throw std::out_of_range(err::invalid_variable);
                }
                if (lit > 0) {
                    // variable is true
                    sol.assign_variable(abs(lit), true);
                } else if (lit < 0) {
                    // variable is false
                    sol.assign_variable(std::abs(lit), false);
                } else {
                    // variable is zero (invalid)
                    throw std::invalid_argument(err::sol_var_zero);
                }
            } catch (std::out_of_range) {
                throw std::out_of_range(err::invalid_variable);
            } catch (...) {
                throw std::invalid_argument(err::sol_body_format);
            }
        }
    }

    // extract_solution accepts a SOL-formatted suspected solution
    // specification via an input stream and returns a solution object.
    // It throws exceptions if the provided input cannot be read.
    solution& extract_solution(solution& sol, std::istream& istr) {
        // needed for parsing
        std::string line;

        // find and parse the solution line
        while (std::getline(istr, line).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_solution_line(line, sol, sol.max_var, sol.clauses);
                break;
            }
        }
        // ensure no errors occurred while reading solution
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the solution body
        parse_solution_body(istr, sol, sol.max_var);
        // ensure no errors occurred while reading expression
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // sol was generated successfully
        return sol;
    }

}
