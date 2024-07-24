// verify.hpp
// Logan Moonie
// Jul 15, 2024

#ifndef INC_VERIFY
#define INC_VERIFY

#include <boost/program_options.hpp>
#include <fstream>
#include <string>
#include "sol.hpp"
#include "cnf.hpp"
#include "message.hpp"

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
        std::fstream pstr;
        std::fstream sstr;

        // Boost program options
        opts::options_description desc;
        opts::positional_options_description pos;
        opts::variables_map var_map;
    };



}

#endif
