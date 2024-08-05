// cdcl.cpp
// Logan Moonie
// Jul 22, 2024

#include <deque>
#include "solver.hpp"
#include "solve.hpp"

namespace solver {

    using std::abs;

    struct assignment {
        variable var;
        bool val;
        int decision_level;
        clause reason_clause;
    };

    std::ostream& operator<<(std::ostream& ostr, const assignment& ass) {
        ostr << std::format(
            "{} {} {} {}",
            ass.var,
            ass.val,
            ass.decision_level,
            ass.reason_clause
        ) << std::endl;
        return ostr;
    }

    std::ostream& operator<<(std::ostream& ostr, const std::unordered_set<literal>& clause) {
        for (auto const& lit : clause) {
            ostr << lit << " ";
        }
        ostr << std::endl;
        return ostr;
    }

    // problem constructor
    cdcl::cdcl(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        basic_solver(prob, orchestrator)
    {}

    inline bool unit_propagate(
        cnf::cnf_expr& expr,
        std::deque<assignment>& trail,
        const int& decision_level
    ) {
        // perform unit propagation
        for (auto ucl(expr.unit_clause()); ucl != expr.clauses_end(); ucl = expr.unit_clause()) {
            literal unit_lit = *((ucl->second).begin());
            bool unit_val = unit_lit > 0 ? true : false;
            trail.push_back({abs(unit_lit), unit_val, decision_level, ucl->first});
            std::cout << trail.back();
            expr.assign_and_simplify(abs(unit_lit), unit_val);
            if (expr.empty_clause()) return false;
        }
        // return false on conflict
        return !expr.empty_clause();
    }

    inline bool first_uip(
        const std::unordered_set<literal>& con_clause,
        const std::deque<assignment>& trail,
        const int& decision_level
    ) {
        int num_lit_at_dec_level = 0;
        for (auto const& lit : con_clause) {
            // is this literal at the current decision level?
            auto iter = trail.rbegin();
            while (iter != trail.rend() && iter->decision_level == decision_level) {
                if (iter->var == abs(lit)) {
                    num_lit_at_dec_level++;
                    break;
                }
                iter++;
            }
            if (num_lit_at_dec_level > 1) return false;
        }
        return true;
    }

    inline std::unordered_set<literal> resolve_clauses(
        const std::unordered_set<literal>& con_clause,
        const std::unordered_set<literal>& reason_clause
    ) {
        std::unordered_set<literal> resolved_clause = con_clause;
        for (auto const& lit : reason_clause) {
            if (resolved_clause.contains(-lit)) {
                // the complementary literals cancel each other
                resolved_clause.erase(-lit);
            } else {
                resolved_clause.insert(lit);
            }
        }
        return resolved_clause;
    }

    inline int analyze_conflict(
        cnf::cnf_expr& expr,
        std::deque<assignment>& trail,
        int decision_level,
        const cnf::cnf_expr& original_expr
    ) {
        clause empty_clause = expr.get_empty_clause();
        std::unordered_set<literal> conflict_clause = original_expr.get_clause(empty_clause);
        std::cout << conflict_clause;
        auto iter = trail.rbegin();
        while (!first_uip(conflict_clause, trail, decision_level) && iter->reason_clause != 0) {
            conflict_clause = resolve_clauses(conflict_clause, original_expr.get_clause(iter->reason_clause));
            iter++;
        }
        // find second-greatest decision level in clause
        int backjump_level = -1;
        for (auto const& lit : conflict_clause) {
            for (auto iter = trail.rbegin(); iter != trail.rend(); iter++) {
                if (iter->var == abs(lit)) {
                    if (
                        iter->decision_level > backjump_level &&
                        iter->decision_level < decision_level
                    ) {
                        backjump_level = iter->decision_level;
                    }
                    break;
                }
            }
        }
        return backjump_level;
    }

    void cdcl::operator()(std::stop_token token) try {
        orc.pif.message(2, "cdcl solver starting");
        auto start_time = time.now();
        last_stop_check = time.now();

        std::deque<assignment> trail;
        std::deque<cnf::cnf_expr> expr_record;
        std::size_t num_var = expr.variables().size();
        bool sol_found = true;
        int decision_level = 0;

        if (!unit_propagate(expr, trail, expr_record.size())) {
            // unsatisfiable
            // skip the while loop
            num_var = 0;
            sol_found = false;
        }

        bool next_val = false;

        // until all variables assigned
        while (expr.get_num_clauses() > 0) {
            expr_record.push_back(expr);
            decision_level++;
            variable branch_var = expr.pick_var();
            trail.push_back({branch_var, next_val, expr_record.size(), 0});
            std::cout << trail.back();
            expr.assign_and_simplify(branch_var, next_val);
            if (next_val) next_val = false;
            if (!unit_propagate(expr, trail, expr_record.size())) {
                // on conflict
                int backjump_level = analyze_conflict(expr, trail, expr_record.size(), expr_record[0]);
                if (backjump_level < 0) {
                    sol_found = false;
                    break;
                } else {
                    // backjump to backjump_level
                    while (expr_record.size() > backjump_level) expr_record.pop_back();
                    while (trail.back().decision_level >= backjump_level) trail.pop_back();
                    expr = expr_record.back();
                    decision_level = expr_record.size();
                    next_val = true;
                }
            }
            // check for a stop signal
            if (time.now() - last_stop_check > std::chrono::milliseconds(100)) {
                last_stop_check = time.now();
                if (token.stop_requested()) {
                    sol_found = false;
                    break;
                }
            }
        }

        if (sol_found) {
            for (auto const& ass : trail) {
                sol.assign_variable(ass.var, ass.val);
            }
            sol.set_valid(true);
        }

        // report the solution
        if (sol.is_valid()) {
            std::chrono::duration<double> elapsed_time = time.now() - start_time;
            sol.stats().insert({"ELAPSED_TIME_SECONDS", std::to_string(elapsed_time.count())});
            orc.report_solution(std::move(sol), SolverType::CDCL);
        } else {
            orc.report_no_solution();
        }
    } catch (std::exception& e) {
        orc.report_error(true);
        std::cout << e.what() << std::endl;
        return;
    }

    // std::vector<cdcl> cdcl::divide(uint num_sub_problems) {
    //     // perform pure literal deletion
    //     for (literal plit(expr.pure_literal()); plit != 0; plit = expr.pure_literal()) {
    //         bool pure_val = plit > 0 ? true : false;
    //         sol.assign_variable(abs(plit), pure_val);
    //         expr.assign_and_simplify(abs(plit), pure_val);
    //     }
    //     std::vector<cdcl> reduced_solvers;
    //     // variables in expression
    //     auto var_list = expr.variables();
    //     // divide the problem log_2(num_sub_problems) times
    //     for (std::size_t i(0); i < num_sub_problems; i++) {
    //         auto solver_copy(*this);
    //         auto& reduced_sol = solver_copy.sol;
    //         auto& reduced_expr = solver_copy.expr;
    //         uint j(i);
    //         auto var_iter = var_list.begin();
    //         for (uint k(num_sub_problems - 1); k > 0; k /= 2) {
    //             if (var_iter == var_list.end()) break;
    //             if (j % 2 == 0) {
    //                 // branch left
    //                 reduced_sol.assign_variable(*var_iter, false);
    //                 reduced_expr.assign_and_simplify(*var_iter, false);

    //             } else {
    //                 // branch right
    //                 reduced_sol.assign_variable(*var_iter, true);
    //                 reduced_expr.assign_and_simplify(*var_iter, true);
    //             }
    //             j /= 2;
    //             var_iter++;
    //         }
    //         reduced_solvers.push_back(solver_copy);
    //     }
    //     return reduced_solvers;
    // }

}
