// test_sol_io.cpp
// Logan Moonie
// July 13, 2024

#include "sol.hpp"

#include <iostream>

int main() {
    try {
        sol::solution sol(std::cin);
        std::cout << sol;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

