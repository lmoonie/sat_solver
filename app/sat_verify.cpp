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
    return verify::verify_solution(sol, expr) ? 0 : 1;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}

