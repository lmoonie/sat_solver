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
    class cnf_expr;

}

