// sat_verify.cpp
// Logan Moonie
// July 21, 2024

#include <iostream>
#include "sol.hpp"
#include "cnf.hpp"
#include "verify.hpp"
#include "message.hpp"

int main(int argc, char** argv) try {
    verify::program_interface pif(argc, argv);
    if (pif.print_help) {
        std::cout << pif.desc << std::endl;
        return std::cout ? 0 : 1;
    } else if (pif.print_formats) {
        std::cout << info::available_formats << std::endl;
    } else {
        cnf::cnf_expr expr;
        sol::solution sol;
        if (pif.var_map.count("problem") > 0) {
            expr = pif.pstr;
        } else {
            expr = std::cin;
        }
        if (pif.var_map.count("solution") > 0) {
            sol = pif.sstr;
        } else {
            sol = std::cin;
        }
        bool correct = verify::verify_solution(sol, expr);
        if (!pif.quiet) {
            if (correct) {
                std::cout << "The provided solution is correct." << std::endl;
            } else {
                std::cout << "The provided solution is incorrect." << std::endl;
            }
        }
        return correct ? 0 : 1;
    }
} catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
}

