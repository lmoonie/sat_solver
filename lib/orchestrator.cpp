// orchestrator.cpp
// Logan Moonie
// Aug 1, 2024

#include "orchestrator.hpp"
#include "solve.hpp"
#include "solver.hpp"
#include <fstream>
#include <limits>
#include <stdexcept>
#include <unistd.h>
#include "message.hpp"
#include <signal.h>

extern volatile std::sig_atomic_t sig;

namespace solve {

    // constructor
    orchestrator::orchestrator(const program_interface& program_if):
        pif (program_if),
        finished(false),
        started(false),
        active_divided_threads(0),
        active_incomplete_threads(0),
        status(Status::Success)
    {
        threads.reserve(pif.threads);
    }

    inline bool vmem_usage(long int&);


    // run the solvers
    std::pair<Status, sol::solution> orchestrator::operator()(const cnf::cnf_expr& expr) {
        // prepare the default solution if none is found
        sol.set_max_var(expr.get_max_var());
        sol.set_num_clauses(expr.get_num_clauses());
        sol.set_type(static_cast<sol::ProblemType>(expr.get_type()));

        if (sig != 0) {
            if (pif.verbosity > 0) {
                (std::cout << "\nc interrupt signal received\n").flush();
            }
            return {Status::IntSig, sol};
        }

        // set the number of threads used for complete solvers
        uint num_comp_threads(1);
        while (num_comp_threads*2 <= pif.threads) num_comp_threads *= 2;

        if (pif.solver == solver::SolverType::Auto) {
            // set the number of threads used for incomplete solvers
            uint num_inc_threads(pif.threads - num_comp_threads);
            active_incomplete_threads = num_inc_threads;

            // divide the problem and distribute to complete solvers
            auto comp_solvers = solver::dpll(expr, *this).divide(num_comp_threads);
            active_divided_threads = num_comp_threads;
            for (auto& comp_solver : comp_solvers) {
                threads.emplace_back(std::jthread(comp_solver));
            }

            // start random solvers
            auto inc_solver = solver::local_search(expr, *this);
            for (std::size_t i(0); i < num_inc_threads; i++) {
                threads.emplace_back(std::jthread(inc_solver));
            }
        } else if (pif.solver == solver::SolverType::DPLL) {
            // divide the problem and distribute to complete solvers
            auto comp_solvers = solver::dpll(expr, *this).divide(num_comp_threads);
            active_divided_threads = num_comp_threads;
            for (auto& comp_solver : comp_solvers) {
                threads.emplace_back(std::jthread(comp_solver));
            }
        } else if (pif.solver == solver::SolverType::LocalSearch) {
            // start random solvers
            active_incomplete_threads = pif.threads;
            auto inc_solver = solver::local_search(expr, *this);
            for (std::size_t i(0); i < pif.threads; i++) {
                threads.emplace_back(std::jthread(inc_solver));
            }
        } else if (pif.solver == solver::SolverType::BruteForce) {
            // start brute force solver
            threads.emplace_back(std::jthread(solver::brute_force(expr, *this)));
        }

        // get current time
        std::chrono::steady_clock time;
        auto last_monitor_time = time.now();
        auto start_time = time.now();
        uint mem_warn_count(0);

        pif.message(1, "solving...");

        // manage solvers periodically and upon finishing
        std::unique_lock lock(m);
        started = true;
        start.notify_all();
        while (!finished && sig == 0) {
            finish.wait_for(
                lock,
                std::chrono::milliseconds(500),
                [&](){
                    return
                        finished ||
                        time.now() - last_monitor_time >= std::chrono::milliseconds(500);
                }
            );
            last_monitor_time = time.now();
            if (time.now() - start_time >= pif.duration && !finished) {
                status = Status::OutOfTime;
                finished = true;
                pif.message(1, "time limit reached");
            }
            long int mem_usage;
            if (vmem_usage(mem_usage)) {
                mem_warn_count = 0;
                if (mem_usage >= pif.memory && !finished) {
                    status = Status::OutOfMemory;
                    finished = true;
                    pif.message(1, "memory limit reached");
                }
            } else {
                mem_warn_count++;
                if (mem_warn_count >= 5) {
                    throw(std::runtime_error(err::not_read_mem));
                } else {
                    pif.warn(format(
                        "could not get memory usage from system; will try {} more times",
                        5 - mem_warn_count
                    ));
                }
            }
            if (active_incomplete_threads == 0 && active_divided_threads == 0 && !finished) {
                finished = true;
                status = Status::ThreadPanic;
            }
        }
        if (sig != 0) {
            if (pif.verbosity > 0) {
                (std::cout << "\nc interrupt signal received\n").flush();
            }
            status = Status::IntSig;
        }
        // tell running solvers to stop
        pif.message(2, "shutting down solvers");
        for (auto& thread : threads) {
            thread.request_stop();
        }
        lock.unlock();

        // wait for threads to stop
        for (auto& thread : threads) {
            thread.join();
        }
        pif.message(2, "solvers stopped");
        if (status == Status::ThreadPanic) {
            throw std::runtime_error(err::thread_panic);
        }

        // assign arbitrary values to any remaining variables
        for (auto const& var : expr.variables()) {
            if (!sol.map().contains(var)) {
                sol.assign_variable(var, true);
            }
        }

        return std::make_pair(status, sol);
    }

    // report solution
    void orchestrator::report_solution(sol::solution&& proposed_sol, solver::SolverType s) {
        std::scoped_lock lock(m);
        if (!finished) {
            sol = proposed_sol;
            std::string solver_name =
                s == solver::SolverType::DPLL        ? "dpll"s         :
                s == solver::SolverType::LocalSearch ? "local_search"s :
                s == solver::SolverType::BruteForce  ? "brute_force"s  :
                                                            "unknown"s ;
            sol.stats().insert({"SOLVER", solver_name});
            finished = true;
            status = Status::Success;
            if (sol.is_valid())
                pif.message(2, "a solution was found by "s + solver_name);
            finish.notify_all();
        }
    }

    // report no solution
    void orchestrator::report_no_solution() {
        std::scoped_lock lock(m);
        active_divided_threads--;
        if (active_divided_threads == 0 && !finished) {
            finished = true;
            status = Status::Success;
            pif.message(2, "no solution exists");
            finish.notify_all();
        }
    }

    // report solver error
    void orchestrator::report_error(bool is_complete_solver) {
        std::scoped_lock lock(m);
        if (!finished) {
            if (is_complete_solver) {
                finished = true;
                status = Status::ThreadPanic;
                finish.notify_all();
            } else {
                active_incomplete_threads--;
                pif.warn("an error was encountered while executing an incomplete solver");
            }
        }
    }

    // get virtual memory usage from the system
    bool vmem_usage(long int& mem) {
            std::ifstream stat("/proc/self/stat");
            std::string str;
            for (std::size_t i(0); i < 23; i++) {
                stat >> str;
            }
            stat >> mem;
            mem *= std::abs(getpagesize());
            return !stat.bad();
    }

    // report ready to start
    void ready() {
        std::unique_lock lock(m);
        start.wait(lock, [this](){return started;})
    }


}

