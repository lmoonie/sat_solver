// cnf.cpp
// Logan Moonie
// Jul 12, 2024

#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <ostream>
#include <iostream>
#include "cnf.hpp"
#include "io.hpp"

using std::abs;

class cnf_expr {
    public:
        // default constructor
        cnf_expr() = default;
        // istream constructor
        cnf_expr(std::istream& istr) {
            ::io::prob::extract_cnf_problem(istr, *this);
        }
        // copy constructor
        cnf_expr(const cnf_expr&) = default;
        // move constructor
        cnf_expr(cnf_expr&&) = default;
        // no assignment permitted
        cnf_expr& operator=(const cnf_expr&) = delete;
        cnf_expr& operator=(cnf_expr&&) = delete;

        void add_literal(literal lit, clause cl) {
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

        void remove_literal(literal lit, clause cl) {
            literals[lit].erase(cl);
            clauses[cl].erase(lit);
        }

        void remove_clause(clause cl) {
            for (const literal& lit : clauses[cl]) {
                literals[lit].erase(cl);
            }
            clauses.erase(cl);
        }

        std::size_t num_clauses() {
            return clauses.size();
        }

        variable num_variables() {
            return literals.size()/2;
        }

    private:
        // tracks which clauses a given literal is in
        unordered_map<literal, cl_set> literals;
        // tracks which literals are in a given clause
        unordered_map<clause, lit_set> clauses;

        friend std::ostream& operator<<(std::ostream& ostr, const cnf_expr&) {
            auto clause_iter(clauses.begin());
            while (clause_iter != clauses.end()) {
                std::cout << "{ ";
                for (literal lit : clause_iter->second) {
                    std::cout << lit << " ";
                }
                std::cout << "}" << std::endl;
                clause_iter++;
            }
        }
    }
