// solver.hpp
// Logan Moonie
// Jul 22, 2024

#ifndef INC_SOLVER
#define INC_SOLVER

#include <utility>
#include <random>
#include <climits>
#include <iterator>
#include <algorithm>
#include <thread>
#include <vector>
#include "cnf.hpp"
#include "sol.hpp"
#include "orchestrator.hpp"

namespace solver {

    // ProblemType enum
    using ProblemType = sol::ProblemType;

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
        basic_solver(const cnf::cnf_expr&, solve::orchestrator&);
        // copy constructor
        basic_solver(const basic_solver&) = default;
        // move constructor
        basic_solver(basic_solver&&) = default;
        // virtual destructor
        virtual ~basic_solver() {};
        // assignment
        basic_solver& operator=(const basic_solver&) = default;
        basic_solver& operator=(basic_solver&&) = default;
        // run the solver
        virtual void operator()(std::stop_token) = 0;
    protected:
        cnf::cnf_expr expr;
        sol::solution sol;
        solve::orchestrator& orc;
        std::chrono::steady_clock time;
        std::chrono::time_point<std::chrono::steady_clock> last_stop_check;
    };

    class brute_force : public basic_solver {
    public:
        // problem constructor
        brute_force(const cnf::cnf_expr&, solve::orchestrator&);
        // run the algorithm
        void operator()(std::stop_token);
        // destructor
        ~brute_force() {};
    };

    class dpll : public basic_solver {
    public:
        // problem constructor
        dpll(const cnf::cnf_expr&, solve::orchestrator&);
        // run the algorithm
        void operator()(std::stop_token);
        // divide the problem
        std::vector<dpll> divide(uint);
        // destructor
        ~dpll() {};
    };

    class local_search : public basic_solver {
    public:
        // problem constructor
        local_search(const cnf::cnf_expr&, solve::orchestrator&);
        // run the algorithm
        void operator()(std::stop_token);
        // destructor
        ~local_search() {};
    private:
        std::mt19937_64 rand;
    };

}

#endif
