// io.cpp
// Logan Moonie
// Jul 12, 2024

#include <istream>
#include <ios>
#include <stdexcept>
#include <format>
#include "io.hpp"
#include "cnf.hpp"

// a collection of functions used to process problems via I/O
namespace io::prob {

    // to simplify working with CNF types
    using namespace cnf;

    // Given a suspected CNF problem line, parse_problem_line
    // extracts the number of variables and clauses, throwing
    // exceptions if the line is not formatted correctly.
    inline void parse_problem_line(
        const std::string& str,
        int& max_var,
        int& clauses
    ) {
        // check problem format
        if (str.substr(0, 6) != "p cnf ") {
            throw std::invalid_argument(err::problem_format);
        }
        // record number of clauses and variables
        std::size_t* clause_idx;
        try {
            max_var = std::stoi(str.substr(6), clause_idx);
            clauses = std::stoi(str.substr(6 + *clause_idx));
        } catch (std::out_of_range) {
            throw std::out_of_range(err::too_many_cl_var);
        } catch (...) {
            throw std::invalid_argument(err::problem_format);
        }
        if (clauses <= 0 || max_var <= 0) {
            throw std::invalid_argument(err::num_clauses_vars);
        }
    }

    // Given a CNF expression body (via input stream), parse_expression
    // extracts it contents into a cnf_expr, throwing errors if the
    // expression is not formatted correctly.
    inline void parse_expression(
        std::istream& istr,
        cnf_expr& expr,
        disjunctive_clause& dis_clause,
        literal& lit,
        variable& max_var,
        clauses& clauses
    ) {
        do {
            // check for a comment line
            if (istr.peek() == 'c') {
                // discard the comment line
                istr.ignore(
                    std::numeric_limits<std::streamsize>::max(),
                    '\n'
                );
                continue;
            }
            // check that literal is an integer
            if (istr >> lit) {
                if (std::abs(lit) > max_var) {
                    // not a valid literal
                    throw std::invalid_argument(err::invalid_variable);
                } else if (lit == 0) {
                    // integer was zero; marking end of clause
                    if (dis_clause.empty()) {
                        // count the clause without adding to expr
                        clauses--;
                    } else {
                        // add clause to expr and reset
                        expr.insert(dis_clause);
                        dis_clause.clear();
                    }
                } else {
                    // integer is a valid literal
                    dis_clause.insert(lit);
                }
            }
        } while (istr.good());

        // ensure the correct number of clauses was provided
        if (cnf_expr.size() != clauses) {
            throw std::invalid_argument(err::wrong_number_clauses);
        }
    }

    // extract_cnf_problem accepts a CNF-formatted problem
    // specification via an input stream and returns a cnf_expr.
    // It throws exceptions if the provided input cannot be read.
    cnf_expr extract_cnf_problem(std::istream& istr) {
        // cnf data structures
        cnf_expr expr;
        disjunctive_clause dis_clause;
        literal lit;

        // variables needed for parsing
        std::string line;
        variable max_var(0);
        int clauses(0);

        // find and parse the problem line
        while (istr.getline(line.begin(), line.max_size()).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_problem_line(line, max_var, clauses);
                break;
            }
        }
        // ensure no errors occurred while reading problem
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the expression body
        parse_expression(
            istr, expr, dis_clause, lit, max_var, clauses
        );
        // ensure no errors occurred while reading expression
        if (istr.bad()) {
            // likely an I/O error
            throw std::ios_base::failure(err::io_err);
        } else if (!istr) {
            // likely an invalid expression
            throw std::invalid_argument(err::expression_format);
        }

        // expr was generated successfully
        return expr;
    }

}
