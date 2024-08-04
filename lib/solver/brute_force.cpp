// brute_force.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"
#include "solve.hpp"

namespace solver {

    // problem constructor
    brute_force::brute_force(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        basic_solver(prob, orchestrator)
    {}

    // run the algorithm
    void brute_force::operator()(std::stop_token token) try {
        orc.pif.message(2, "brute_force solver starting");
        auto start_time = time.now();
        // check for empty expression
        if (expr.get_num_clauses() == 0) {
            sol.set_valid(true);
        } else if (expr.empty_clause()) {
            // check for empty clauses
            sol.set_valid(false);
        } else {
            for (const auto& var : expr.variables()) {
                sol.assign_variable(var, false);
            }
            while (!expr.eval(sol.map())) {
                auto iter(sol.map().begin());
                while (iter != sol.map().end() && iter->second) {
                    sol.reassign_variable((iter++)->first, false);
                }
                if (iter != sol.map().end()) {
                    sol.reassign_variable(iter->first, true);
                } else break;
                // check for a stop signal
                if (time.now() - last_stop_check > std::chrono::milliseconds(100)) {
                    last_stop_check = time.now();
                    if (token.stop_requested()) {
                        return;
                    }
                }
            }
            sol.set_valid(expr.eval(sol.map()));
        }
        // report the solution
        std::chrono::duration<double> elapsed_time = time.now() - start_time;
        sol.stats().insert({"ELAPSED_TIME_SECONDS", std::to_string(elapsed_time.count())});
        if (sol.is_valid()) {
            orc.report_solution(std::move(sol), SolverType::BruteForce);
        } else {
            orc.report_no_solution();
        }
    } catch (...) {
        orc.report_error(true);
        return;
    }

    std::vector<brute_force> brute_force::divide(uint num_sub_problems) {
        std::vector<brute_force> reduced_solvers;
        // variables in expression
        auto var_list = expr.variables();
        // divide the problem log_2(num_sub_problems) times
        for (std::size_t i(0); i < num_sub_problems; i++) {
            auto solver_copy(*this);
            auto& reduced_sol = solver_copy.sol;
            auto& reduced_expr = solver_copy.expr;
            uint j(i);
            auto var_iter = var_list.begin();
            for (uint k(num_sub_problems - 1); k > 0; k /= 2) {
                if (var_iter == var_list.end()) break;
                if (j % 2 == 0) {
                    // branch left
                    reduced_sol.assign_variable(*var_iter, false);
                    reduced_expr.assign_and_simplify(*var_iter, false);

                } else {
                    // branch right
                    reduced_sol.assign_variable(*var_iter, true);
                    reduced_expr.assign_and_simplify(*var_iter, true);
                }
                j /= 2;
                var_iter++;
            }
            reduced_solvers.push_back(solver_copy);
        }
        return reduced_solvers;
    }

}
