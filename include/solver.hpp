// solver.hpp
// Logan Moonie
// Jul 22, 2024

#ifndef INC_SOLVER
#define INC_SOLVER

#include "cnf.hpp"
#include "sol.hpp"

namespace solver {

    // ProblemType enum
    using ProblemType = sol::ProblemType;

    enum SolverType: ushort {
        All = 0,
        BruteForce = 1
    };

    // a boolean variable; strictly positive
    using variable = int;

     // a disjunctive clause; strictly positive
    using clause = int;

    // a boolean literal; negative values indicate negation
    // is never zero
    using literal = int;

    class basic_solver {
    public:
        // no default constructor
        basic_solver() = delete;
        // problem constructor
        basic_solver(const cnf::cnf_expr&);
        // copy constructor
        basic_solver(const basic_solver&) = default;
        // move constructor
        basic_solver(basic_solver&&) = default;
        // assignment
        basic_solver& operator=(const basic_solver&) = default;
        basic_solver& operator=(basic_solver&&) = default;
        virtual sol::solution operator()();
    protected:
        cnf::cnf_expr expr;
        sol::solution sol;
    };

    class brute_force : public basic_solver {
    public:
        // problem constructor
        brute_force(const cnf::cnf_expr&);
        // run the algorithm
        sol::solution operator()();
    };

}

#endif
