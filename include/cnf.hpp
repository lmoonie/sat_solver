// cnf.hpp
// Logan Moonie
// Jul 12, 2024

#include <unordered_set>
#include <map>

// a collection of types used to store CNF problems
namespace cnf {

    // a boolean variable;
    using variable = int;

    // a boolean literal; negative values indicate negation
    using literal = int;

    // a disjunction of literals
    using disjunctive_clause = std::unordered_set<literal>;

    // a cnf expression
    using cnf_expr = std::unordered_set<disjunctive_clause>;

    // a map of variables to their proposed value
    using solution = std::map<variable, bool>;

}

