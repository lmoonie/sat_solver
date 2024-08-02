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
#include "sol.hpp"
#include "cnf.hpp"

namespace solve {

    class program_interface;

    enum Status: int {
        Success = 0,
        OutOfTime = 1,
        OutOfMemory = 2,
        ThreadPanic = 3
    };

    class orchestrator {
    public:
        // no default constructor
        orchestrator() = delete;
        // constructor
        orchestrator(program_interface&);
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
        void report_solution(sol::solution&&);
    private:
        std::vector<std::jthread> threads;
        sol::solution sol;
        bool finished;
        Status status;
        program_interface& pif;
        mutable std::mutex m;
    };

}

#endif
