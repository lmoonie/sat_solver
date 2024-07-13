#include "cnf.hpp"
#include "io.hpp"

#include <iostream>
#include <istream>
#include <ostream>

void print_cnf_expr(ostream& ostr, const cnf::cnf_expr& expr) {
    auto clause_iter(expr.begin());
    if (clause_iter == expr.end()) return;
    auto lit_iter(*clause_iter.begin());
    while (clause_iter != expr.end()) {
        std::cout << "{ ";
        while (lit_iter != clause_iter->end()) {
            syd::cout << *lit_iter << " ";
            lit_iter++;
        }
        std::cout << "}" << std::endl;
        clause_iter++;
    }
}

int main() {
    try {
        cnf::cnf_expr expr = io::cnf::extract_cnf_problem(std::cin);
        print_cnf_expr(expr);
    } catch (...) return 1;
    return 0;
}
