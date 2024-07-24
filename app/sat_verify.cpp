// sat_verify.cpp
// Logan Moonie
// July 21, 2024

#include <iostream>
#include <istream>
#include "sol.hpp"
#include "cnf.hpp"
#include "verify.hpp"
#include "message.hpp"

int main(int argc, char** argv) try {
    verify::program_interface pif(argc, argv);
    std::istream prob_str;
    std::istream sol_str;
    if (pif.pstr) {
        prob_str = pif.pstr;
    } else {
        prob_str = std::cin;
    }
    if (pif.sstr) {
        sol_str = pif.sstr;
    } else {
        sol_str = std::cin;
    }
    cnf::cnf_expr expr(prob_str);
    sol::solution sol(sol_str);
    sol.set_valid(true);
    if (sol.get_type() != sol::ProblemType::CNF) {
        throw std::invalid_argument(err::solution_type);
    }
    return verify::verify_solution(sol, expr) ? 0 : 1;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}

