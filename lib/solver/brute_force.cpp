// brute_force.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"
#include "solve.hpp"

namespace solver {

    // problem constructor
    brute_force::brute_force(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        basic_solver(prob, orchestrator)
    {
        for (const auto& var : expr.variables()) {
            sol.assign_variable(var, false);
        }
    }

    // run the algorithm
    void brute_force::operator()(std::stop_token token) {
        orc.pif.message(2, "brute_force solver starting");
        auto start_time = time.now();
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
        // report the solution
        std::chrono::duration<double> elapsed_time = time.now() - start_time;
        sol.stats().insert({"ELAPSED_TIME_SECONDS", std::to_string(elapsed_time.count())});
        sol.set_valid(expr.eval(sol.map()));
        orc.report_solution(std::move(sol), SolverType::BruteForce);
    }

}
