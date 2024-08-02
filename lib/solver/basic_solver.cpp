// basic_solver.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"

namespace solver {
    
    // basic_solver problem constructor
    basic_solver::basic_solver(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        expr(prob),
        orc (orchestrator)
    {
        sol.set_max_var(expr.get_max_var());
        sol.set_num_clauses(expr.get_num_clauses());
        sol.set_type(static_cast<sol::ProblemType>(expr.get_type()));
    }
    
}
