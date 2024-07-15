// cnf.cpp
// Logan Moonie
// Jul 12, 2024

#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <ostream>
#include <iostream>
#include <ios>
#include <string>
#include <stdexcept>
#include <format>
#include "cnf.hpp"
#include "err.hpp"

namespace cnf {

    // forward declaration
    inline cnf_expr& io::extract_cnf_problem(cnf_expr&, std::istream&);

    // will need abs frequently
    using std::abs;

    // input stream constructor
    cnf_expr::cnf_expr(std::istream& istr) {
        io::extract_cnf_problem(*this, istr);
    }

    // add a literal to a clause
    void cnf_expr::add_literal(literal lit, clause cl) {
        if (!literals.contains(abs(lit))) {
            literals.insert({abs(lit), cl_set()});
            literals.insert({-abs(lit), cl_set()});
        }
        literals[lit].insert(cl);
        if (!clauses.contains(cl)) {
            clauses.insert({cl, lit_set()});
        }
        clauses[cl].insert(lit);
    }

    // remove a literal from a clause
    void cnf_expr::remove_literal(literal lit, clause cl) {
        literals[lit].erase(cl);
        clauses[cl].erase(lit);
        if (clauses[cl].empty()) {
            clauses.erase(cl);
        }
    }

    // remove an entire clause
    void cnf_expr::remove_clause(clause cl) {
        for (const literal& lit : clauses[cl]) {
            literals[lit].erase(cl);
        }
        clauses.erase(cl);
    }

    // give the number of active clauses
    std::size_t cnf_expr::num_clauses() {
        return clauses.size();
    }

    // give the number of active variables
    std::size_t cnf_expr::num_variables() {
        return literals.size()/2;
    }

    // used to print cnf_expr
    std::ostream& operator<<(std::ostream& ostr, const cnf_expr& expr) {
        auto clause_iter(expr.clauses.begin());
        while (clause_iter != expr.clauses.end()) {
            ostr << "{ ";
            for (literal lit : clause_iter->second) {
                ostr << lit << " ";
            }
            ostr << "}" << std::endl;
            clause_iter++;
        }
        return ostr;
    }
}

// a collection of functions used to process problems via I/O
namespace cnf::io {

    // Given a suspected CNF problem line, parse_problem_line
    // extracts the number of variables and clauses, throwing
    // exceptions if the line is not formatted correctly.
    inline void parse_problem_line(
        const std::string& str,
        cnf_expr expr&
    ) {
        // check problem format
        try {
            if (str.substr(0, 6) != "p cnf ") {
                throw std::invalid_argument(err::problem_format);
            }
        } catch (std::out_of_range) {
            throw std::out_of_range(err::problem_format);
        }
        // record number of clauses and variables
        std::size_t clause_idx;
        try {
            expr.max_var = std::stoi(str.substr(6), &clause_idx);
            expr.num_clauses = std::stoi(str.substr(6 + clause_idx));
        } catch (std::out_of_range) {
            throw std::out_of_range(err::too_many_cl_var);
        } catch (...) {
            throw std::invalid_argument(err::problem_format);
        }
        if (expr.num_clauses <= 0 || expr.max_var <= 0) {
            throw std::invalid_argument(err::num_clauses_vars);
        }
    }

    // Given a CNF expression body (via input stream), parse_expression
    // extracts it contents into a cnf_expr, throwing errors if the
    // expression is not formatted correctly.
    inline void parse_expression(
        std::istream& istr,
        cnf_expr& expr
    ) {
        literal lit;
        clause cl(0);
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
                if (std::abs(lit) > expr.max_var) {
                    // not a valid literal
                    throw std::invalid_argument(err::invalid_variable);
                } else if (lit == 0) {
                    // increment clause counter
                    cl++;
                } else {
                    // integer is a valid literal
                    expr.add_literal(lit, cl);
                }
            }

        } while (istr.good());

        // ensure the correct number of clauses was provided
        if (cl != expr.num_clauses) {
            throw std::invalid_argument(err::wrong_number_clauses);
        }
    }

    // extract_cnf_problem accepts a CNF-formatted problem
    // specification via an input stream and returns a cnf_expr.
    // It throws exceptions if the provided input cannot be read.
    inline cnf_expr& extract_cnf_problem(cnf_expr& expr, std::istream& istr) {
        // needed for parsing
        std::string line;

        // find and parse the problem line
        while (std::getline(istr, line).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_problem_line(line, expr);
                break;
            }
        }
        // ensure no errors occurred while reading problem
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the expression body
        parse_expression(istr, expr);
        // ensure no errors occurred while reading expression
        if (istr.bad()) {
            // likely an I/O error
            throw std::ios_base::failure(err::io_err);
        } else if (!istr.eof()) {
            // likely an invalid expression
            throw std::invalid_argument(err::expression_format);
        }

        // expr was generated successfully
        return expr;
    }

}
