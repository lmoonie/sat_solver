#include "cnf.hpp"
#include "io.hpp"

#include <iostream>
#include <istream>
#include <ostream>

int main() {
    try {
        cnf::cnf_expr expr(std::cin);
        std::cout << expr;
    } catch (...) return 1;
    return 0;
}
