// solve.cpp
// Logan Moonie
// Jul 22, 2024

#include "solve.hpp"

namespace solve {

    // solve the problem
    bool run_portfolio(const program_interface& pif, std::istream& istr, std::ostream& ostr) {
        cnf::cnf_expr expr(istr);
        sol::solution sol;
        sol.set_type(sol::ProblemType::CNF);

        try {
            if (pif.solver == solver::SolverType::BruteForce || pif.solver == solver::SolverType::All) {
                sol = solver::brute_force(expr)();
            }
            ostr << sol;
        } catch (...) {
            ostr << std::format(
                "s {} {} {} {}",
                sol.type == ProblemType::CNF ? "cnf" : "sat",
                "-1",
                std::to_string(expr.get_max_var()),
                std::to_string(expr.get_num_clauses())
            ) << std::endl;
            throw;
        }
        return sol.is_valid();
    }

    // forward declaration
    namespace cli {
        void extract_program_options(program_interface&, int, char**);
    }

    // CLI constructor
    program_interface::program_interface(int argc, char** argv):
        desc("The following options are available:")
    {
        cli::extract_program_options(*this, argc, argv);
    }

    namespace cli {

        namespace flag_desc {
            const std::string help(
                ""
            );
            const std::string available_formats(
                ""
            );
            const std::string verbose(
                ""
            );
            const std::string quiet(
                ""
            );
            const std::string solver(
                ""
            );
            const std::string list_solvers(
                ""
            );
            const std::string incomplete(
                ""
            );
            const std::string threads(
                ""
            );
            const std::string duration(
                ""
            );
            const std::string memory(
                ""
            );
        }

        inline void extract_program_options(program_interface& pif, int argc, char** argv) {
            // available options
            pif.desc.add_options()
                ("help,h", flag_desc::help.c_str())
                ("available-formats,f", flag_desc::available_formats.c_str())
                ("verbose,v", opts::value<ushort>(), flag_desc::verbose.c_str())
                ("quiet,q", flag_desc::quiet.c_str())
                ("solver,s", opts::value<std::string>(), flag_desc::solver.c_str())
                ("list-solvers,l", flag_desc::list_solvers.c_str())
                ("incomplete,i", flag_desc::incomplete.c_str())
                ("threads,t", opts::value<uint>(), flag_desc::threads.c_str())
                ("duration,d", opts::value<std::chrono::duration<int64_t>>(), flag_desc::duration.c_str())
                ("memory,m", opts::value<unsigned long long int>(), flag_desc::memory.c_str())

            // parse command line options
            opts::store(
                opts::command_line_parser(argc, argv)
                    .options(pif.desc)
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
                } else {
                    throw std::invalid_argument(err::invalid_solver);
                }
            } else if (pif.var_map.count("solver") > 1) {
                throw std::invalid_argument(err::repeat_options);
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
                pif.duration = pif.var_map["duration"].as<std::chrono::duration<int64_t>>();
            } else if (pif.var_map.count("duration") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }
            // set memory
            if (pif.var_map.count("memory") == 1) {
                pif.memory = pif.var_map["memory"].as<unsigned long long int>();
            } else if (pif.var_map.count("memory") > 1) {
                throw std::invalid_argument(err::repeat_options);
            }

        }


    }

}
