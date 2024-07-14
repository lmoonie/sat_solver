#include "io.hpp"
#include "cnf.hpp"

#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>

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
