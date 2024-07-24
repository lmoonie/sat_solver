// local_search.cpp
// Logan Moonie
// Jul 23, 2024

#include "solver.hpp"

namespace solver {

    using std::abs;

    // problem constructor
    local_search::local_search(const cnf::cnf_expr& prob):
        basic_solver(prob),
        rand(std::random_device()())
    {}

    inline literal select_optimal_literal(const cnf::cnf_expr& expr, const sol::solution& sol) {
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
    sol::solution local_search::operator()() {
        const double RAND_LIT_PROB = 0.2;
        const uint MAX_RAND_ADVANCE = 20;

        // record problem parameters
        sol.set_num_clauses(expr.get_num_clauses());
        sol.set_max_var(expr.get_max_var());

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
            clause target_cl = std::advance(
                unsat_clauses.begin(),
                std::uniform_int_distribution(
                    0,
                    std::min(MAX_RAND_ADVANCE, unsat_clauses.size()-1)
                )(rand)
            );
            // pick a variable
            std::uniform_real_distribution<double> real_dist(0, 1);
            if (real_dist(rand) > RAND_LIT_PROB) {
                // select a literal carefully
                literal target_lit = select_optimal_literal(expr, sol);
                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            } else {
                // randomly select a literal
                literal target_lit = std::advance(
                    expr.get_clause(target_cl).begin(),
                    std::uniform_int_distribution(
                        0,
                        std::min(MAX_RAND_ADVANCE, expr.get_clause(target_cl).size()-1)
                    )(rand)
                );
                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            }
        }
        // a solution was found
        sol.set_valid(true);
        return sol;
    }

}
