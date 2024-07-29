// cnf.hpp
// Logan Moonie
// Jul 12, 2024

#ifndef INC_CNF
#define INC_CNF

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <istream>
#include <ostream>
#include <ios>
#include <string>
#include <stdexcept>
#include <format>
#include <cctype>
#include <stack>
#include "message.hpp"

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

    // used to specify the problem format
    enum ProblemType: bool {
        CNF = true,
        SAT = false
    };

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
        // assignment
        cnf_expr& operator=(const cnf_expr&) = default;
        cnf_expr& operator=(cnf_expr&&) = default;
        // add a literal to a clause
        void add_literal(literal, clause);
        // remove a literal from a clause
        void remove_literal(literal, clause);
        // remove an entire clause
        void remove_clause(clause);
        // assign a variable and simplify the expression
        void assign_and_simplify(variable, bool);
        // return an iterator to a unit clause
        std::unordered_map<clause, lit_set>::const_iterator unit_clause() const;
        // return a pure literal
        literal pure_literal() const;
        // end iterators for the maps
        std::unordered_map<literal, cl_set>::const_iterator literals_end() const;
        std::unordered_map<clause, lit_set>::const_iterator clauses_end() const;
        // give the number of active clauses
        std::size_t get_num_clauses() const;
        // give the number of active variables
        std::size_t num_variables() const;
        // give the maximum variable key
        variable get_max_var() const;
        // evaluate the expression
        bool eval(const std::map<variable, bool>& assigns) const;
        // return a set of the expression's variables
        std::set<variable> variables() const;
        // return a set of unsatisfied clauses
        std::set<clause> unsatisfied_clauses(const std::map<variable, bool>&) const;
        // check for empty clauses
        bool empty_clause() const;
        // return a reference to the requested clause
        const lit_set& get_clause(clause) const;
        // return a reference to the clause list of the requested literal
        const cl_set& get_literal(literal) const;
        // return an active variable
        variable pick_var() const;
    private:
        // tracks which clauses a given literal is in
        std::unordered_map<literal, cl_set> literals;
        // tracks which literals are in a given clause
        std::unordered_map<clause, lit_set> clauses;
        // maximum variable value
        variable max_var;
        // format of provided problem
        ProblemType type;
        // used to print cnf_expr
        friend std::ostream& operator<<(std::ostream&, const cnf_expr&);
        // used to build problem from input stream
        friend cnf_expr& io::extract_cnf_problem(cnf_expr&, std::istream&);
    };

}

#endif
