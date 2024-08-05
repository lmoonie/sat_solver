// sat_solve.cpp
// Logan Moonie
// July 22, 2024

#include <iostream>
#include <format>
#include "solve.hpp"

volatile std::sig_atomic_t sig = 0;

void sig_handler(int signal) {
    sig = signal;
}

int main(int argc, char** argv) try {
    // register signal handler
    std::signal(SIGINT, sig_handler);

    // configure program from CLI
    solve::program_interface pif(argc, argv);

    // proceed with the indicated mode
    if (pif.print_help) {
        std::cout << pif.desc << std::endl;
        return std::cout ? 0 : 1;
    } else if (pif.print_formats) {
        std::cout << info::available_formats << std::endl;
        return std::cout ? 0 : 1;
    } else if (pif.print_solvers) {
        std::cout << info::available_solvers << std::endl;
        return std::cout ? 0 : 1;
    } else {
        // solve the provided problem
        return solve::run_portfolio(pif, std::cin, std::cout);
    }
} catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
}

