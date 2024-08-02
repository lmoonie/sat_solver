// sat_solve.cpp
// Logan Moonie
// July 22, 2024

#include <iostream>
#include <format>
#include "solve.hpp"

int main(int argc, char** argv) try {
    // configure program from CLI
    solve::program_interface pif(argc, argv);

    // proceed with the indicated mode
    if (false) {
        true; // no-op
    } else {
        // solve the provided problem
        return solve::run_portfolio(pif, std::cin, std::cout);
    }
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 2;
}

