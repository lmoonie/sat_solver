// local_search.cpp
// Logan Moonie
// Jul 23, 2024

#include "solver.hpp"

namespace solver {

    using std::abs;

    // problem constructor
    local_search::local_search(const cnf::cnf_expr& prob, solve::orchestrator& orchestrator):
        basic_solver(prob, orchestrator),
        rand(std::random_device()())
    {}

    inline literal select_optimal_literal(
        const cnf::cnf_expr& expr,
        const sol::solution& sol,
        clause target_cl
    ) {
        unsigned long int min_new_unsat = ULONG_MAX;
        literal target_lit;
        for (auto const& lit : expr.get_clause(target_cl)) {
            // examine each literal
            unsigned long int will_unsatisfy_clauses = 0;
            if (sol.map().at(abs(lit))) {
                // variable is currently assigned true
                for (auto const& cl : expr.get_literal(abs(lit))) {
                    // examine each clause in which the positive literal appears
                    bool satisfied(false);
                    for (auto const& other_lit : expr.get_clause(cl)) {
                        // check for another literal satisfying the clause
                        if (other_lit == lit) continue;
                        if (
                            other_lit > 0 ?
                            sol.map().at(abs(other_lit)) :
                            !sol.map().at(abs(other_lit))
                        ) {
                            satisfied = true;
                            break;
                        }
                    }
                    if (!satisfied) will_unsatisfy_clauses++;
                }
            } else {
                // variable is currently assigned false
                for (auto const& cl : expr.get_literal(-abs(lit))) {
                    // examine each clause in which the negative literal appears
                    bool satisfied(false);
                    for (auto const& other_lit : expr.get_clause(cl)) {
                        // check for another literal satisfying the clause
                        if (other_lit == lit) continue;
                        if (
                            other_lit > 0 ?
                            sol.map().at(abs(other_lit)) :
                            !sol.map().at(abs(other_lit))
                        ) {
                            satisfied = true;
                            break;
                        }
                    }
                    if (!satisfied) will_unsatisfy_clauses++;
                }
            }
            // check if this literal is better than previous
            if (will_unsatisfy_clauses < min_new_unsat) {
                min_new_unsat = will_unsatisfy_clauses;
                target_lit = lit;
            }
        }
        return target_lit;
    }

    // a stochastic local search algorithm implementing WalkSAT
    void local_search::operator()(std::stop_token token) {
        const double RAND_LIT_PROB = 0.2;
        const std::size_t MAX_RAND_ADVANCE = 20;
        std::chrono::steady_clock time;
        auto last_stop_check = time.now();

        // a functor to provide a random bool value
        std::uniform_int_distribution<ushort> bit_dist(0, 1);
        auto rand_bool = [&]() -> bool {
            return bit_dist(rand) == 1 ? true : false;
        };
        // assign random values to variables
        for (auto const& var : expr.variables()) {
            sol.assign_variable(var, rand_bool());
        }
        // while the solution is invalid
        while (!expr.eval(sol.map())) {
            // pick a clause
            auto unsat_clauses = expr.unsatisfied_clauses(sol.map());
            auto cl_iter = unsat_clauses.begin();
            std::advance(
                cl_iter,
                std::uniform_int_distribution<std::size_t>(
                    0,
                    std::min(MAX_RAND_ADVANCE, unsat_clauses.size()-1)
                )(rand)
            );
            clause target_cl = *cl_iter;
            // pick a variable
            std::uniform_real_distribution<double> real_dist(0, 1);
            if (real_dist(rand) > RAND_LIT_PROB) {
                // select a literal carefully
                literal target_lit = select_optimal_literal(expr, sol, target_cl);
                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            } else {
                // randomly select a literal
                auto lit_iter = expr.get_clause(target_cl).begin();
                std::advance(
                    lit_iter,
                    std::uniform_int_distribution<std::size_t>(
                        0,
                        std::min(MAX_RAND_ADVANCE, expr.get_clause(target_cl).size()-1)
                    )(rand)
                );
                literal target_lit = *lit_iter;
                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            }
            // check for a stop signal
            if (time.now() - last_stop_check > std::chrono::milliseconds(100)) {
                last_stop_check = time.now();
                if (token.stop_requested()) {
                    return;
                }
            }
        }
        // report the solution
        sol.set_valid(true);
        orc.report_solution(std::move(sol));
    }

}
