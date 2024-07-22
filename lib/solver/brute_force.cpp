// brute_force.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"

namespace solver {

    // problem constructor
    brute_force::brute_force(const cnf::cnf_expr& prob):
        solver(prob)
    {
        for (const auto& var : expr.variables()) {
            sol.assign_variable(var, false);
        }
    }

    // run the algorithm
    sol::solution brute_force::operator()() {
        while (!expr.eval(sol.map())) {
            auto iter(sol.map().begin());
            while (iter != sol.map().end() && iter->second) {
                sol.reassign_variable((iter++)->first, false);
            }
            if (iter != sol.map().end()) {
                sol.reassign_variable(iter->first, true);
            } else break;
        }
        sol.set_valid(expr.eval(sol.map()));
        return sol;
    }

}
