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
    if (pif.pstr) {
        expr = pif.pstr;
    } else {
        expr = std::cin;
    }
    if (pif.sstr) {
        sol = pif.sstr;
    } else {
        sol = std::cin;
    }
    sol.set_valid(true);
    if (sol.get_type() != sol::ProblemType::CNF) {
        throw std::invalid_argument(err::solution_type);
    }
    return verify::verify_solution(sol, expr) ? 0 : 1;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}

