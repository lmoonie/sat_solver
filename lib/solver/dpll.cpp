// dpll.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"

namespace solver {

    using std::abs;

    using problem = std::pair<cnf::cnf_expr, sol::solution>;

    // problem constructor
    dpll::dpll(const cnf::cnf_expr& prob):
        basic_solver(prob)
    {}

    sol::solution dpll::operator()() {
        // perform unit propagation
        for (auto ucl(expr.unit_clause()); ucl != expr.clauses_end(); ucl = expr.unit_clause()) {
            literal unit_lit = *((ucl->second).begin());
            bool unit_val = unit_lit > 0 ? true : false;
            sol.assign_variable(abs(unit_lit), unit_val);
            expr.assign_and_simplify(abs(unit_lit), unit_val);
        }

        // perform pure literal deletion
        for (literal plit(expr.pure_literal()); plit != 0; plit = expr.pure_literal()) {
            bool pure_val = plit > 0 ? true : false;
            sol.assign_variable(abs(plit), pure_val);
            expr.assign_and_simplify(abs(plit), pure_val);
        }

        // find the solution to the reduced problem
        return sub_dpll({expr, sol});
    }

    inline problem reduce_problem(problem prob, variable var, bool val) {
        auto& sub_expr = prob.first;
        auto& curr_sol = prob.second;

        // assign the value to the provided variable
        curr_sol.assign_variable(var, val);
        sub_expr.assign_and_simplify(var, val);

        // perform unit propagation
        for (auto ucl(sub_expr.unit_clause()); ucl != sub_expr.clauses_end(); ucl = sub_expr.unit_clause()) {
            literal unit_lit = *((ucl->second).begin());
            bool unit_val = unit_lit > 0 ? true : false;
            curr_sol.assign_variable(abs(unit_lit), unit_val);
            sub_expr.assign_and_simplify(abs(unit_lit), unit_val);
        }

        // perform pure literal deletion
        for (literal plit(sub_expr.pure_literal()); plit != 0; plit = sub_expr.pure_literal()) {
            bool pure_val = plit > 0 ? true : false;
            curr_sol.assign_variable(abs(plit), pure_val);
            sub_expr.assign_and_simplify(abs(plit), pure_val);
        }

        // return the sub-problem
        return prob;
    }

    // find the solution for a reduced problem
    sol::solution sub_dpll(problem prob) {
        auto& sub_expr = prob.first;
        auto& curr_sol = prob.second;

        // check for empty expression
        if (sub_expr.get_num_clauses() == 0) {
            curr_sol.set_valid(true);
            curr_sol.set_num_clauses(sub_expr.get_num_clauses());
            curr_sol.set_max_var(sub_expr.get_max_var());
            return curr_sol;
        }

        // check for empty clauses
        if (sub_expr.empty_clause()) {
            // return an invalid solution
            return curr_sol;
        }

        // pick a branch variable
        variable branch_var = sub_expr.pick_var(); // need to write

        // branch left
        auto sol_left = sub_dpll(reduce_problem(prob, branch_var, false));
        if (sol_left.is_valid()) return sol_left;

        // branch right
        auto sol_right = sub_dpll(reduce_problem(prob, branch_var, true));
        if (sol_right.is_valid()) return sol_right;

        return curr_sol;
    }

}
