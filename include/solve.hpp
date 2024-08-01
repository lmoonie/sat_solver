// solve.hpp
// Logan Moonie
// Jul 22, 2024


#ifndef INC_SOLVE
#define INC_SOLVE

#include <boost/program_options.hpp>
#include <string>
#include <chrono>
#include <cstdint>
#include <istream>
#include <ostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "sol.hpp"
#include "cnf.hpp"
#include "message.hpp"
#include "solver.hpp"

namespace solve {

    namespace opts = boost::program_options;

    class program_interface {
    public:
        // no default constructor
        program_interface() = delete;
        // CLI constructor
        program_interface(int, char**);
        // copy constructor
        program_interface(const program_interface&) = delete;
        // move constructor
        program_interface(program_interface&&) = default;
        // no assignment permitted
        program_interface& operator=(const program_interface&) = delete;
        program_interface& operator=(program_interface&&) = delete;

        // program parameters
        bool print_help;
        bool print_formats;
        bool print_solvers;
        ushort verbosity;
        solver::SolverType solver;
        bool incomplete;
        uint threads;
        std::chrono::duration<int64_t> duration;
        unsigned long long int memory;

        // Boost program options
        opts::options_description desc;
        opts::variables_map var_map;
    };

    class orchestrator {
    public:
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
        bool operator()();
    private:
        std::vector<std::jthread> threads;
        sol::solution sol;
        const program_interface& pif;
        std::mutex solution_lock;
        std::condition_variable finished;
    };

    // solve the problem
    bool run_portfolio(const program_interface&, std::istream&, std::ostream&);



}

#endif
