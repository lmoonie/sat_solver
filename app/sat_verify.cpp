// sat_verify.cpp
// Logan Moonie
// July 21, 2024

#include <iostream>
#include "sol.hpp"
#include "cnf.hpp"
#include "verify.hpp"
#include "message.hpp"

int main(int argc, char** argv) try {
    cnf::cnf_expr expr(std::cin);
    verify::program_interface pif(argc, argv);
    sol::solution sol(pif.sol_str);
    sol.set_valid(true);
    if (sol.get_type() != sol::ProblemType::CNF) {
        throw std::invalid_argument(err::solution_type);
    }
    return verify::verify_solution(sol, expr) ? 0 : 1;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}

