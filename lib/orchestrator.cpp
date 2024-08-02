// orchestrator.cpp
// Logan Moonie
// Aug 1, 2024

#include "orchestrator.hpp"
#include "solve.hpp"

namespace solve {

    // constructor
    orchestrator::orchestrator(const program_interface& program_if):
        pif (program_if),
        finished(false)
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
        for (auto& solver : comp_solvers) {
            threads.emplace_back(std::jthread(solver));
        }

        // start random solvers
        for (std::size_t i(0); i < num_inc_threads; i++) {
            threads.emplace_back(std::jthread(solver::local_search(expr, *this)));
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
            }
            if (finished) {
                // tell running solvers to stop
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
        return std::make_pair(status, sol);
    }

    // report solution
    void orchestrator::report_solution(sol::solution&& proposed_sol) {
        std::scoped_lock(m);
        if (!finished) {
            sol = proposed_sol;
            finished = true;
            status = Status::Success;
        }
    }

}

