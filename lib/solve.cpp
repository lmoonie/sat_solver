// solve.cpp
// Logan Moonie
// Jul 22, 2024

#include "solve.hpp"
#include "solver.hpp"

extern volatile std::sig_atomic_t sig;

namespace solve {

    // solve the problem
    int run_portfolio(const program_interface& pif, std::istream& istr, std::ostream& ostr) {
        if (sig != 0) {
            if (pif.verbosity > 0) {
                (std::cout << "\nc interrupt signal received\n").flush();
            }
            return 2;
        }
        cnf::cnf_expr expr(istr);
        orchestrator orc(pif);
        auto result = orc(expr);
        auto& sol = result.second;
        if (result.first == Status::Success) {
            ostr << sol;
            return sol.is_valid() ? 0 : 1;
        } else {
            ostr << std::format(
                "s {} {} {} {}",
                sol.get_type() == sol::ProblemType::CNF ? "cnf" : "sat",
                "-1",
                std::to_string(expr.get_max_var()),
                sol.get_type() == sol::ProblemType::CNF ?
                    std::to_string(expr.get_num_clauses()) : ""
            ) << std::endl;
            return 2;
        }
    }

    // forward declaration
    namespace cli {
        void extract_program_options(program_interface&, int, char**);
    }

    // CLI constructor
    program_interface::program_interface(int argc, char** argv):
        desc(info::solve_description),
        duration(std::chrono::minutes(5)),
        memory(2 * 1'000'000'000),
        threads(std::jthread::hardware_concurrency()),
        verbosity(1),
        incomplete(false),
        print_help(false),
        print_formats(false),
        print_solvers(false),
        solver(solver::SolverType::Auto)
    {
        cli::extract_program_options(*this, argc, argv);
        message(2, format("the verbosity is set to {}", verbosity));
        message(2, "the solver is set to "s + (
            solver == solver::SolverType::Auto        ? "auto"s         :
            solver == solver::SolverType::DPLL        ? "dpll"s         :
            solver == solver::SolverType::LocalSearch ? "local_search"s :
            solver == solver::SolverType::BruteForce  ? "brute_force"s  :
            solver == solver::SolverType::CDCL        ? "cdcl"s         :
                                                        "undefined"s
        ));
        message(2, format("the portfolio is set to use {} threads", threads));
        if (incomplete) {
            message(2, format("the portfolio is allowed to never prove unsatisfiability", threads));
        }
        message(1, format("the portfolio has a time limit of {} seconds", duration.count()));
        message(1, format("the portfolio has a memory limit of {}kB", memory / 1000));
    }

    // send a message to the user
    void program_interface::message(int v, const std::string& m) const {
        if (v <= verbosity) {
            (std::cout << format("c {}\n", m)).flush();
        }
    }

    // send a warning to the user
    void program_interface::warn(const std::string& w) const {
        if (verbosity > 0) {
            (std::cerr << format("Warning: {}\n", w)).flush();
        }
    }

    namespace cli {
        
        // needed to overload validate
        struct duration_t {
            std::chrono::duration<int64_t> val;
        };
        struct memory_t {
            long int val;
        };

        // validator for duration argument
        void validate(
            boost::any& v,
            const std::vector<std::string>& args,
            duration_t*, int)
        {
            // get the provided argument
            opts::validators::check_first_occurrence(v);
            const std::string& str = opts::validators::get_single_string(args);

            // parse the value
            std::size_t unit_idx;
            try {
                long time_steps = std::stoll(str, &unit_idx);
                if (str.at(unit_idx) == 's') {
                    v = duration_t{std::chrono::seconds(time_steps)};
                } else if (str.at(unit_idx) == 'm') {
                    v = duration_t{std::chrono::minutes(time_steps)};
                } else if (str.at(unit_idx) == 'h') {
                    v = duration_t{std::chrono::hours(time_steps)};
                } else {
                    throw std::invalid_argument(err::invalid_duration);
                }
            } catch (...) {
                throw std::invalid_argument(err::invalid_duration);
            }
        }

        // validator for memory argument
        void validate(
            boost::any& v,
            const std::vector<std::string>& args,
            memory_t*, int)
        {
            // get the provided argument
            opts::validators::check_first_occurrence(v);
            const std::string& str = opts::validators::get_single_string(args);

            // parse the value
            std::size_t unit_idx;
            try {
                long int mem = {std::stol(str, &unit_idx)};
                if (str.at(unit_idx) == 'k') {
                    v = memory_t{mem * 1'000};
                } else if (str.at(unit_idx) == 'm') {
                    v = memory_t{mem * 1'000'000};
                } else if (str.at(unit_idx) == 'g') {
                    v = memory_t{mem * 1'000'000'000};
                } else {
                    throw std::invalid_argument(err::invalid_memory);
                }
            } catch (...) {
                throw std::invalid_argument(err::invalid_memory);
            }
        }

        inline void extract_program_options(program_interface& pif, int argc, char** argv) {
            // available options
            pif.desc.add_options()
                ("help,h", info::s_flags::help.c_str())
                ("available-formats,f", info::s_flags::available_formats.c_str())
                ("verbose,v", opts::value<ushort>()->implicit_value(2), info::s_flags::verbose.c_str())
                ("quiet,q", info::s_flags::quiet.c_str())
                ("solver,s", opts::value<std::string>(), info::s_flags::solver.c_str())
                ("problem,p", opts::value<std::string>(), info::s_flags::problem.c_str())
                ("list-solvers,l", info::s_flags::list_solvers.c_str())
                ("incomplete,i", info::s_flags::incomplete.c_str())
                ("threads,t", opts::value<uint>(), info::s_flags::threads.c_str())
                ("duration,d", opts::value<duration_t>(), info::s_flags::duration.c_str())
                ("memory,m", opts::value<memory_t>(), info::s_flags::memory.c_str());

            // a value without corresponding flag is assumed to be the problem file
            pif.pos.add("problem", -1);

            // parse command line options
            opts::store(
                opts::command_line_parser(argc, argv)
                    .options(pif.desc)
                    .positional(pif.pos)
                    .run()
                ,
                pif.var_map
            );
            opts::notify(pif.var_map);

            // set print_help
            if (pif.var_map.count("help")) {
                pif.print_help = true;
            }
            // set print_formats
            if (pif.var_map.count("available-formats")) {
                pif.print_formats = true;
            }
            // set verbosity
            if (pif.var_map.count("verbose") == 1) {
                pif.verbosity = pif.var_map["verbose"].as<ushort>();
                if (pif.verbosity > 2) {
                    throw std::invalid_argument(err::verbosity);
                }
            } else if (pif.var_map.count("verbose") > 1) {
                throw std::invalid_argument(err::repeat_options);
            } else if (pif.var_map.count("quiet") == 1) {
                pif.verbosity = 0;
            } else if (pif.var_map.count("quiet") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }
            // set solver
            if (pif.var_map.count("solver") == 1) {
                std::string solver = pif.var_map["solver"].as<std::string>();
                if (solver == std::string("brute_force")) {
                    pif.solver = solver::SolverType::BruteForce;
                } else if (solver == std::string("dpll")) {
                    pif.solver = solver::SolverType::DPLL;
                } else if (solver == std::string("local_search")) {
                    pif.solver = solver::SolverType::LocalSearch;
                } else if (solver == std::string("auto")) {
                    pif.solver = solver::SolverType::Auto;
                } else if (solver == std::string("cdcl")) {
                    pif.solver = solver::SolverType::CDCL;
                } else {
                    throw std::invalid_argument(err::invalid_solver);
                }
            } else if (pif.var_map.count("solver") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }
            // set problem file
            if (pif.var_map.count("problem") == 1) {
                pif.pstr = std::fstream(pif.var_map["problem"].as<std::string>());
                if (!pif.pstr) {
                    throw std::invalid_argument(err::not_open_file);
                }
            } else if (pif.var_map.count("problem") > 1) {
                throw std::invalid_argument(err::too_many_problems);
            }
            // set list-solvers
            if (pif.var_map.count("list-solvers")) {
                pif.print_solvers = true;
            }
            // set incomplete
            if (pif.var_map.count("incomplete")) {
                pif.incomplete = true;
            }
            // set threads
            if (pif.var_map.count("threads") == 1) {
                pif.threads = pif.var_map["threads"].as<uint>();
            } else if (pif.var_map.count("threads") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }
            // set duration
            if (pif.var_map.count("duration") == 1) {
                pif.duration = pif.var_map["duration"].as<duration_t>().val;
            } else if (pif.var_map.count("duration") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }
            // set memory
            if (pif.var_map.count("memory") == 1) {
                pif.memory = pif.var_map["memory"].as<memory_t>().val;
            } else if (pif.var_map.count("memory") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }

        }


    }

}
