// dpll.cpp
// Logan Moonie
// Jul 22, 2024

#include "solver.hpp"

namespace solver {

    using std::abs;
    using problem = std::pair<cnf::cnf_expr, sol::solution>;
    inline sol::solution sub_dpll(problem, std::stop_token&);

    // global timer
    std::chrono::steady_clock time;
    std::chrono::time_point<std::chrono::steady_clock> last_stop_check;

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
        last_stop_check = time.now();

        // apply heuristics
        simplify(expr, sol);

        // find the solution to the reduced problem
        auto final_sol = sub_dpll({expr, sol}, token);
        if (token.stop_requested()) {
            sol.set_valid(false);
            return;
        }

        // assign arbitrary values to the remaining variables
        for (auto const& var : expr.variables()) {
            if (!final_sol.map().contains(var)) {
                final_sol.assign_variable(var, true);
            }
        }

        // report the solution
        orc.report_solution(std::move(sol));
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
    sol::solution sub_dpll(problem prob, std::stop_token& token) {
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
        // sub problems to be found
        std::vector<dpll> sub_probs;
        // apply heuristics to full problem
        simplify(expr, sol);
        // divide the problem log_2(num_sub_problems) times
        for (std::size_t(i); i < num_sub_problems; i++) {
            sub_probs.push_back(*this);
            problem prob = { sub_probs[i].expr, sub_probs[i].sol };
            auto& sub_expr = prob.first;
            auto& curr_sol = prob.second;
            uint j(i);
            for (uint k(num_sub_problems - 1); k > 0; k /= 2) {
                // check for empty expression
                if (sub_expr.get_num_clauses() == 0) {
                    curr_sol.set_valid(true);
                    break;
                }
                // check for empty clauses
                if (sub_expr.empty_clause()) {
                    // return an invalid solution
                    break;
                }
                // pick a branch variable
                variable branch_var = sub_expr.pick_var();
                if (j % 2 == 0) {
                    // branch left
                    prob = reduce_problem(prob, branch_var, false);
                } else {
                    // branch right
                    prob = reduce_problem(prob, branch_var, true);
                }
                j /= 2;
            }
            sub_probs[i].expr = sub_expr;
            sub_probs[i].sol = curr_sol;
        }
        return sub_probs;
    }

}
