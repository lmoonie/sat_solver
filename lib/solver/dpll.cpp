// dpll.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"
#include "solve.hpp"

namespace solver {

    using std::abs;
    inline sol::solution sub_dpll(problem, std::stop_token&);

    // problem constructor
    dpll::dpll(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        basic_solver(prob, orchestrator)
    {}

    inline void simplify(cnf::cnf_expr& expr, sol::solution& sol) {
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
    }

    void dpll::operator()(std::stop_token token) {
        orc.pif.message(2, "dpll solver starting");
        auto start_time = time.now();
        last_stop_check = time.now();

        // apply heuristics
        simplify(expr, sol);

        // find the solution to the reduced problem
        auto final_sol = sub_dpll({expr, sol}, token);
        if (token.stop_requested()) {
            final_sol.set_valid(false);
            return;
        }

        // assign arbitrary values to the remaining variables
        for (auto const& var : expr.variables()) {
            if (!final_sol.map().contains(var)) {
                final_sol.assign_variable(var, true);
            }
        }

        // report the solution
        if (final_sol.is_valid()) {
            std::chrono::duration<double> elapsed_time = time.now() - start_time;
            final_sol.stats().insert({"ELAPSED_TIME_SECONDS", std::to_string(elapsed_time.count())});
            orc.report_solution(std::move(final_sol), SolverType::DPLL);
        } else {
            orc.report_no_solution();
        }
    }

    inline problem reduce_problem(problem prob, variable var, bool val) {
        auto& sub_expr = prob.first;
        auto& curr_sol = prob.second;

        // assign the value to the provided variable
        curr_sol.assign_variable(var, val);
        sub_expr.assign_and_simplify(var, val);

        // apply heuristics
        simplify(sub_expr, curr_sol);

        // return the sub-problem
        return prob;
    }

    // find the solution for a reduced problem
    sol::solution dpll::sub_dpll(problem prob, std::stop_token& token) {
        auto& sub_expr = prob.first;
        auto& curr_sol = prob.second;

        // check for a stop signal
        if (time.now() - last_stop_check > std::chrono::milliseconds(100)) {
            last_stop_check = time.now();
            if (token.stop_requested()) {
                curr_sol.set_valid(true);
                return curr_sol;
            }
        }

        // check for empty expression
        if (sub_expr.get_num_clauses() == 0) {
            curr_sol.set_valid(true);
            return curr_sol;
        }

        // check for empty clauses
        if (sub_expr.empty_clause()) {
            // return an invalid solution
            return curr_sol;
        }

        // pick a branch variable
        variable branch_var = sub_expr.pick_var();

        // branch left
        auto sol_left = sub_dpll(reduce_problem(prob, branch_var, false), token);
        if (sol_left.is_valid()) return sol_left;

        // branch right
        auto sol_right = sub_dpll(reduce_problem(prob, branch_var, true), token);
        if (sol_right.is_valid()) return sol_right;

        return curr_sol;
    }

    std::vector<dpll> dpll::divide(uint num_sub_problems) {
        std::vector<dpll> reduced_solvers;
        // variables in expression
        auto var_list = expr.variables();
        // divide the problem log_2(num_sub_problems) times
        for (std::size_t i(0); i < num_sub_problems; i++) {
            auto solver_copy(*this);
            auto& reduced_sol = solver_copy.sol;
            auto& reduced_expr = solver_copy.expr;
            uint j(i);
            auto var_iter = var_list.begin();
            for (uint k(num_sub_problems - 1); k > 0; k /= 2) {
                if (var_iter == var_list.end()) break;
                if (j % 2 == 0) {
                    // branch left
                    reduced_sol.assign_variable(*var_iter, false);
                    reduced_expr.assign_and_simplify(*var_iter, false);

                } else {
                    // branch right
                    reduced_sol.assign_variable(*var_iter, true);
                    reduced_expr.assign_and_simplify(*var_iter, true);
                }
                j /= 2;
                var_iter++;
            }
            reduced_solvers.push_back(solver_copy);
        }
        return reduced_solvers;
    }

}
