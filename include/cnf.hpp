// cnf.hpp
// Logan Moonie
// Jul 12, 2024

#include <map>

// a collection of types used to store CNF problems
namespace cnf {

    // a boolean variable; strictly positive
    using variable = int;

    // a disjunctive clause; strictly positive
    using clause = int;

    // a boolean literal; negative values indicate negation
    // is never zero
    using literal = int;

    // a collection of literals
    using lit_set = std::unordered_set<literal>;

    // a collection of clauses
    using cl_set = std::unordered_set<clause>;

    // a map of variables to their proposed value
    using solution = std::map<variable, bool>;

    // a complete boolean CNF expression
    class cnf_expr {
        cnf_expr() = default;
        cnf_expr(const cnf_expr&) = default;
        cnf_expr(cnf_expr&&) = default;
        cnf_expr& operator=(const cnf_expr&) = delete;
        cnf_expr& operator=(cnf_expr&&) = delete;
        void add_literal(literal lit, clause cl);
        void remove_literal(literal lit, clause cl);
        void remove_clause(clause cl);
        clause num_clauses();
        variable num_variables();
    }

}

