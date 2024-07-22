// solver.hpp
// Logan Moonie
// Jul 22, 2024

#include "cnf"
#include "sol"

namespace solver {

    // used to specify the problem format
    enum ProblemType: bool {
        CNF = true,
        SAT = false
    };

    // a boolean variable; strictly positive
    using variable = int;

     // a disjunctive clause; strictly positive
    using clause = int;

    // a boolean literal; negative values indicate negation
    // is never zero
    using literal = int;

    class solver {
    public:
        // no default constructor
        brute_force() = delete;
        // problem constructor
        brute_force(const cnf::cnf_expr&);
        // copy constructor
        brute_force(const brute_force&) = default;
        // move constructor
        brute_force(brute_force&&) = default;
        // assignment
        brute_force& operator=(const brute_force&) = default;
        brute_force& operator=(brute_force&&) = default;
        virtual sol::solution operator()();
    private:
        cnf::cnf_expr expr;
        sol::solution sol;
    };

    class brute_force : public solver {
    public:
        // problem constructor
        brute_force(const cnf::cnf_expr&);
        // run the algorithm
        sol::solution operator()();
    };

    // solver problem constructor
    solver::solver(const cnf::cnf_expr& prob):
        expr(prob)
    {
        sol.set_max_var(expr.get_max_var());
        sol.set_num_clauses(expr.get_num_clauses());
        sol.set_type(ProblemType::CNF);

    }

}
