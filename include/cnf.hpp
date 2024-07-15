// cnf.hpp
// Logan Moonie
// Jul 12, 2024

#ifndef INC_CNF
#define INC_CNF

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <istream>
#include <ostream>

// a collection of types used to store CNF problems
namespace cnf {

    // forward declaration
    class cnf_expr;
    namespace io {
        cnf_expr& extract_cnf_problem(cnf_expr&, std::istream&);
    }

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
    public:
        // default constructor
        cnf_expr() = default;
        // input stream constructor
        cnf_expr(std::istream&);
        // copy constructor
        cnf_expr(const cnf_expr&) = default;
        // move constructor
        cnf_expr(cnf_expr&&) = default;
        // no assignment permitted
        cnf_expr& operator=(const cnf_expr&) = delete;
        cnf_expr& operator=(cnf_expr&&) = delete;
        // add a literal to a clause
        void add_literal(literal, clause);
        // remove a literal from a clause
        void remove_literal(literal, clause);
        // remove an entire clause
        void remove_clause(clause);
        // give the number of active clauses
        std::size_t num_clauses();
        // give the number of active variables
        std::size_t num_variables();
    private:
        // tracks which clauses a given literal is in
        std::unordered_map<literal, cl_set> literals;
        // tracks which literals are in a given clause
        std::unordered_map<clause, lit_set> clauses;
        // maximum variable value
        variable max_var;
        // used to print cnf_expr
        friend std::ostream& operator<<(std::ostream&, const cnf_expr&);
        // used to build problem from input stream
        friend cnf_expr& io::extract_cnf_problem(cnf_expr&, std::istream&);
    };

}

#endif
