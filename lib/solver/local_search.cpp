// local_search.cpp
// Logan Moonie
// Jul 23, 2024

#include "solver.hpp"

namespace solver {

    using std::abs;

    const pick_random_literal_prob = 0.2;

    // problem constructor
    local_search::local_search(const cnf::cnf_expr& prob):
        basic_solver(prob),
        rand(std::random_device()())
    {}

    // a stochastic local search algorithm implementing WalkSAT
    sol::solution local_search::operator()() {
        // record problem parameters
        sol.set_num_clauses(expr.get_num_clauses());
        sol.set_max_var(expr.get_max_var());

        // a functor to provide a random bool value
        std::uniform_int_distribution<ushort> bit_dist(0, 1);
        auto rand_bool = [&rand, &bit_dist]() -> bool {
            return bit_dist(rand) == 1 ? true : false;
        };

        // assign random values to variables
        for (auto const& var : expr.variables()) {
            sol.assign_variable(var, rand_bool());
        }

        // while the solution is invalid
        while (!expr.eval(sol.map())) {
            // pick a clause
            auto unsat_clauses = expr.unsatisfied_clauses(sol);
            std::discrete_distribution<clause> cl_dist(
                unsat_clauses.begin(),
                unsat_clauses.end()
            );
            clause target_cl = cl_dist(rand);

            // pick a variable
            std::uniform_real_distribution<double> real_dist(0, 1);
            if (real_dist(rand) > pick_random_literal_prob) {
                // select a literal carefully
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
                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            } else {
                // randomly select a literal
                std::discrete_distribution<literal> lit_dist(
                    target_cl.begin(),
                    target_cl.end()
                );
                literal target_lit = lit_dist(rand);

                // flip the selected variable
                sol.reassign_variable(abs(target_lit), !sol.map().at(abs(target_lit)));
            }
        }

        // a solution was found
        sol.set_valid(true);
        return sol;

    }

}
