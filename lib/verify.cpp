// verify.cpp
// Logan Moonie
// Jul 15, 2024

#include <boost/program_options.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include "verify.hpp"
#include "message.hpp"

namespace verify {

    // forward declaration
    inline void cli::extract_program_options(program_interface&, int, char**);

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
        if (sol.num_clauses() != cnf.num_clauses()) {
            throw std::invalid_argument(err::solution_clauses);
        }
        if (sol.get_max_var() != cnf.get_max_var()) {
            throw std::invalid_argument(solution_vars);
        }
        return cnf.eval(sol.map());
    }

    namespace cli {

        inline void extract_program_options(program_interface& pif, int argc, char** argv) {
            // available options
            pif.desc.add_options()
                ("help,h", cli::help)
                ("available-formats,f", cli::available_formats)
                ("solution,s", opts::value<std::string>(), cli::solution)
                ("quiet,q", cli::quiet);

            // a value without corresponding flag is assumed to be the solution file
            pif.pos.add("solution", -1);

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
            if (pif.var_map.count("solution") == 1) {
                pif.solstr = std::fstream(pif.var_map[solution].as<std::string>());
            } else if (pif.var_map.count("solution") > 1) {
                throw std::invalid_argument(err::too_many_solutions);
            } else {
                throw std::invalid_argument(err::need_solution);
            }
            if (!pif.solstr) {
                throw std::filesystem::filesystem_error(err::not_open_file);
            }

        }


    }

}

