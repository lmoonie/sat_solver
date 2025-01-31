// test_cnf_input.cpp
// Logan Moonie
// July 13, 2024

#include "cnf.hpp"

#include <iostream>

int main() {
    try {
        cnf::cnf_expr expr(std::cin);
        std::cout << expr;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
