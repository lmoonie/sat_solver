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
        desc(info::verify_description),
        quiet(false),
        print_help(false),
        print_formats(false)
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
        if (sol.get_num_clauses() != cnf.get_num_clauses() &&
            sol.get_type() == sol::ProblemType::CNF
        ) {
            throw std::invalid_argument(err::solution_clauses);
        }
        if (sol.get_max_var() != cnf.get_max_var()) {
            throw std::invalid_argument(err::solution_vars);
        }
        if (static_cast<bool>(sol.get_type()) !=
            static_cast<bool>(cnf.get_type())
        ) {
            throw std::invalid_argument(err::type_mismatch);
        }
        try {
            return cnf.eval(sol.map());
        } catch (...) {
            throw std::invalid_argument(err::wrong_variables);
        }
    }

    namespace cli {

        inline void extract_program_options(program_interface& pif, int argc, char** argv) {
            // available options
            pif.desc.add_options()
                ("help,h", info::v_flags::help.c_str())
                ("available-formats,f", info::v_flags::available_formats.c_str())
                ("problem,p", opts::value<std::string>(), info::v_flags::problem.c_str())
                ("solution,s", opts::value<std::string>(), info::v_flags::solution.c_str())
                ("quiet,q", info::v_flags::quiet.c_str());

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
            if (pif.var_map.count("problem") + pif.var_map.count("solution") == 0 &&
                !pif.print_help && !pif.print_formats
            ) {
                throw std::invalid_argument(err::need_file);
            }

        }


    }

}

