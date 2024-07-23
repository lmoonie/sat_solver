// cnf.cpp
// Logan Moonie
// Jul 12, 2024

#include "cnf.hpp"

namespace cnf {

    // will need abs frequently
    using std::abs;

    // a boolean variable; strictly positive
    using variable = int;

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
    }

    // remove an entire clause
    void cnf_expr::remove_clause(clause cl) {
        for (const literal& lit : clauses[cl]) {
            literals[lit].erase(cl);
        }
        clauses.erase(cl);
    }

    // assign a variable and simplify the expression
    void cnf_expr::assign_and_simplify(variable var, bool val) {
        // keep a record of what to remove
        cl_set clauses_to_delete;
        std::unordered_map<literal, clause> literals_to_delete;

        // for each clause in which the positive literal appears
        for (auto const& cl : literals.at(var)) {
            if (val) {
                // literal is true; can delete the clause
                clauses_to_delete.insert(cl);
            } else {
                // literal is false; can remove literal from clause
                literals_to_delete.insert(var, cl);
            }
        }

        // for each clause in which the negative literal appears
        for (auto const& cl : literals.at(-var)) {
            if (!val) {
                // literal is true; can delete the clause
                clauses_to_delete.insert(cl);
            } else {
                // literal is false; can remove literal from clause
                literals_to_delete.insert(-var, cl);
            }
        }
        
        // delete the clauses and literals
        // this is delayed to prevent use-after-free errors
        for (auto const& cl : clauses_to_delete) {
            remove_clause(cl);
        }
        for (auto const& [lit, cl] : literals_to_delete) {
            remove_literal(lit, cl);
        }
    }

    // return an iterator to a unit clause
    std::unordered_map<clause, lit_set>::const_iterator cnf_expr::unit_clause() const {
        auto iter(clauses.begin());
        while (iter != clauses.end()) {
            if (iter->second.size() == 1) {
                return iter;
            }
            iter++;
        }
        return iter;
    }

    // return a pure literal
    literal cnf_expr::pure_literal() const {
        for (auto const& var : variables()) {
            if (!literals.at(var).empty() && literals.at(-var).empty()) {
                return var;
            }
            if (!literals.at(-var).empty() && literals.at(var).empty()) {
                return -var;
            }
        }
        return 0;
    }

    // end iterators for the maps
    std::unordered_map<literal, cl_set>::const_iterator cnf_expr::literals_end() const {
        return literals.end();
    }
    std::unordered_map<clause, lit_set>::const_iterator cnf_expr::clauses_end() const {
        return clauses.end();
    }

    // give the number of active clauses
    std::size_t cnf_expr::get_num_clauses() const {
        return clauses.size();
    }

    // give the number of active variables
    std::size_t cnf_expr::num_variables() const {
        return literals.size()/2;
    }

    // give the maximum variable key
    variable cnf_expr::get_max_var() const {
        return max_var;
    }

    // evaluate the expression
    bool cnf_expr::eval(const std::map<variable, bool>& assigns) const {
        for (auto const& [key, cl]: clauses) {
            bool clause_is_true(false);
            for (auto const& lit : cl) {
                if (
                    lit > 0 && assigns.at(abs(lit)) ||
                    lit < 0 && !assigns.at(abs(lit))
                ) {
                    clause_is_true = true;
                    break;
                }
            }
            if (!clause_is_true) return false;
        }
        return true;
    }

    // return a set of the expression's variables
    std::set<variable> cnf_expr::variables() const {
        std::set<variable> var_set;
        for (auto const& [lit, cl] : literals) {
            if (lit > 0) {
                var_set.insert(lit);
            }
        }
        return var_set;
    }

    // check for empty clauses
    bool cnf_expr::empty_clause() const {
        for (auto const& [cl, lset] : clauses) {
            if (lset.empty()) {
                return true;
            }
        }
        return false;
    }

    // return an active variable
    variable cnf_expr::pick_var() const {
        return abs(*(clauses.begin()->second.begin()));
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
        cnf_expr& expr,
        variable& max_var,
        std::size_t& clauses
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
            max_var = std::stoi(str.substr(6), &clause_idx);
            clauses = std::stoi(str.substr(6 + clause_idx));
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
        variable& max_var,
        const std::size_t& clauses
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
                if (std::abs(lit) > max_var) {
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
        if (cl != clauses) {
            throw std::invalid_argument(err::wrong_number_clauses);
        }
    }

    // extract_cnf_problem accepts a CNF-formatted problem
    // specification via an input stream and returns a cnf_expr.
    // It throws exceptions if the provided input cannot be read.
    cnf_expr& extract_cnf_problem(cnf_expr& expr, std::istream& istr) {
        // needed for parsing
        std::string line;
        std::size_t clauses;

        // find and parse the problem line
        while (std::getline(istr, line).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_problem_line(line, expr, expr.max_var, clauses);
                break;
            }
        }
        // ensure no errors occurred while reading problem
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the expression body
        parse_expression(istr, expr, expr.max_var, clauses);
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
