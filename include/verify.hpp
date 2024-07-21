// verify.hpp
// Logan Moonie
// Jul 15, 2024

#ifndef INC_VERIFY
#define INC_VERIFY

#include <fstream>
#include <string>

namespace verify {

    namespace opts = boost::program_options;

    // verify the correctness of a solution
    bool verify_solution(const sol::solution&, const cnf::cnf_expr&);

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
        bool quiet;
        bool print_help;
        bool print_formats;
        std::fstream sol_str;

        // Boost program options
        opts::options_description desc;
        opts::positional_options_description pos;
        opts::variables_map var_map;
    }



}

#endif
