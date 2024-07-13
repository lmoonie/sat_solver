// cnf.cpp
// Logan Moonie
// Jul 12, 2024

#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <ostream>
#include <iostream>
#include "io.hpp"
#include "cnf.hpp"

namespace cnf {

    using std::abs;

    // input stream constructor
    cnf_expr::cnf_expr(std::istream& istr) {
        ::io::prob::extract_cnf_problem(istr, *this);
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
    variable cnf_expr::num_variables() {
        return literals.size()/2;
    }

    // used to print cnf_expr
    std::ostream& operator<<(std::ostream& ostr, const cnf_expr& expr) {
        auto clause_iter(expr.clauses.begin());
        while (clause_iter != expr.clauses.end()) {
            std::cout << "{ ";
            for (literal lit : clause_iter->second) {
                std::cout << lit << " ";
            }
            std::cout << "}" << std::endl;
            clause_iter++;
        }
        return ostr;
    }
}
