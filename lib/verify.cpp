// verify.cpp
// Logan Moonie
// Jul 15, 2024

#include "verify.hpp"

namespace verify {

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

    // verify the correctness of a solution
    bool verify_solution(
        const sol::solution& sol,
        const cnf::cnf_expr& cnf
    ) {
        if (!sol.is_valid()) {
            throw std::invalid_argument(err::invalid_solution);
        }
        if (sol.get_num_clauses() != cnf.get_num_clauses()) {
            throw std::invalid_argument(err::solution_clauses);
        }
        if (sol.get_max_var() != cnf.get_max_var()) {
            throw std::invalid_argument(err::solution_vars);
        }
        try {
            return cnf.eval(sol.map());
        } catch (...) {
            throw std::invalid_argument(err::wrong_variables);
        }
    }

    namespace cli {

        namespace flag_desc {
            const std::string help(
                ""
            );
            const std::string available_formats(
                ""
            );
            const std::string problem(
                ""
            );
            const std::string solution(
                ""
            );
            const std::string quiet(
                ""
            );
        }

        inline void extract_program_options(program_interface& pif, int argc, char** argv) {
            // available options
            pif.desc.add_options()
                ("help,h", flag_desc::help.c_str())
                ("available-formats,f", flag_desc::available_formats.c_str())
                ("problem,p", opts::value<std::string>(), flag_desc::problem.c_str())
                ("solution,s", opts::value<std::string>(), flag_desc::solution.c_str())
                ("quiet,q", flag_desc::quiet.c_str());

            // a value without corresponding flag is assumed to be the solution file
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

            // set program values
            if (pif.var_map.count("help")) {
                pif.print_help = true;
            }
            if (pif.var_map.count("available-formats")) {
                pif.print_formats = true;
            }
            if (pif.var_map.count("quiet")) {
                pif.quiet = true;
            }
            if (pif.var_map.count("problem") == 1) {
                pif.pstr = std::fstream(pif.var_map["problem"].as<std::string>());
                if (!pif.pstr) {
                    throw std::invalid_argument(err::not_open_file);
                }
            } else if (pif.var_map.count("problem") > 1) {
                throw std::invalid_argument(err::too_many_problems);
            }
            if (pif.var_map.count("solution") == 1) {
                pif.sstr = std::fstream(pif.var_map["solution"].as<std::string>());
                if (!pif.sstr) {
                    throw std::invalid_argument(err::not_open_file);
                }
            } else if (pif.var_map.count("solution") > 1) {
                throw std::invalid_argument(err::too_many_solutions);
            }
            if (pif.var_map.count("problem") + pif.var_map.count("solution") == 0) {
                throw std::invalid_argument(err::need_file);
            }

        }


    }

}

