// orchestrator.hpp
// Logan Moonie
// Aug 1, 2024

#ifndef INC_ORCHESTRATOR
#define INC_ORCHESTRATOR

#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <utility>
#include <condition_variable>
#include "sol.hpp"
#include "cnf.hpp"
#include "solve.hpp"

namespace solve {

    enum Status: int {
        Success = 0,
        OutOfTime = 1,
        OutOfMemory = 2,
        ThreadPanic = 3,
        IntSig = 4
    };

    class orchestrator {
    public:
        // hook to program interface
        const program_interface& pif;
        // no default constructor
        orchestrator() = delete;
        // constructor
        orchestrator(const program_interface&);
        // no copy constructor
        orchestrator(const orchestrator&) = delete;
        // no move constructor
        orchestrator(orchestrator&&) = delete;
        // no assignment
        orchestrator& operator=(const orchestrator&) = delete;
        orchestrator& operator=(orchestrator&&) = delete;
        // run the solvers
        std::pair<Status, sol::solution> operator()(const cnf::cnf_expr&);
        // report solution
        void report_solution(sol::solution&&, solver::SolverType);
        // report no solution
        void report_no_solution();
        // report solver error
        void report_error(bool);
    private:
        std::vector<std::jthread> threads;
        uint active_divided_threads;
        uint active_incomplete_threads;
        sol::solution sol;
        bool finished;
        Status status;
        mutable std::mutex m;
        mutable std::condition_variable finish;
    };

}

#endif
