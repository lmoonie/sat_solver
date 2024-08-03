// orchestrator.cpp
// Logan Moonie
// Aug 1, 2024

#include "orchestrator.hpp"
#include "solve.hpp"
#include "solver.hpp"

namespace solve {

    // constructor
    orchestrator::orchestrator(const program_interface& program_if):
        pif (program_if),
        finished(false),
        active_divided_threads(0)
    {
        threads.reserve(pif.threads);
    }

    // run the solvers
    std::pair<Status, sol::solution> orchestrator::operator()(const cnf::cnf_expr& expr) {
        // set the number of threads used for complete solvers
        uint num_comp_threads(1);
        while (num_comp_threads*2 <= pif.threads) num_comp_threads *= 2;
        // set the number of threads used for incomplete solvers
        uint num_inc_threads(pif.threads - num_comp_threads);

        // divide the problem and distribute to complete solvers
        auto comp_solvers = solver::dpll(expr, *this).divide(num_comp_threads);
        pif.message(2, format("Starting {} complete solvers", num_comp_threads));
        active_divided_threads = num_comp_threads;
        for (auto& comp_solver : comp_solvers) {
            threads.emplace_back(std::jthread(comp_solver));
        }

        // start random solvers
        pif.message(2, format("Starting {} incomplete solvers", num_inc_threads));
        auto inc_solver = solver::local_search(expr, *this);
        for (std::size_t i(0); i < num_inc_threads; i++) {
            threads.emplace_back(std::jthread(inc_solver));
        }

        // get current time
        std::chrono::steady_clock time;
        auto start_time = time.now();

        // periodically monitor solvers
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::scoped_lock lock(m);
            if (time.now() - start_time >= pif.duration) {
                status = Status::OutOfTime;
                finished = true;
                pif.message(2, "Time limit reached");
            }
            if (finished) {
                // tell running solvers to stop
                pif.message(2, "Shutting down solvers");
                for (auto& thread : threads) {
                    thread.request_stop();
                }
                break;
            }
        } while (true);

        // wait for threads to stop
        for (auto& thread : threads) {
            thread.join();
        }
        pif.message(2, "Solvers stopped");

        // assign arbitrary values to any remaining variables
        for (auto const& var : expr.variables()) {
            if (!sol.map().contains(var)) {
                sol.assign_variable(var, true);
            }
        }

        return std::make_pair(status, sol);
    }

    // report solution
    void orchestrator::report_solution(sol::solution&& proposed_sol) {
        std::scoped_lock lock(m);
        if (!finished) {
            sol = proposed_sol;
            finished = true;
            status = Status::Success;
            if (sol.is_valid())
                pif.message(2, "Solution found");
        }
    }

    // report no solution
    void orchestrator::report_no_solution() {
        std::scoped_lock lock(m);
        active_divided_threads--;
        if (active_divided_threads == 0) {
            finished = true;
            status = Status::Success;
            pif.message(2, "No solution exists");
        }
    }

}

