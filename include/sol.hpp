// sol.hpp
// Logan Moonie
// Jul 13, 2024

#ifndef INC_SOL
#define INC_SOL

#include <map>
#include <istream>
#include <ostream>
#include <ios>
#include <stdexcept>
#include <format>
#include "message.hpp"

// a collection of types used to store CNF problems
namespace sol {

    // forward declaration
    class solution;
    namespace io {
        solution& extract_solution(solution&, std::istream&);
    }

    // used to specify the problem format
    enum ProblemType: bool {
        CNF = true,
        SAT = false
    };

    // a boolean variable; strictly positive
    using variable = int;

     // a disjunctive clause; strictly positive
    using clause = int;

    // a boolean literal; negative values indicate negation
    // is never zero
    using literal = int;

    // a complete boolean CNF expression
    class solution {
    public:
        // no default constructor
        solution() = default;
        // stream constructor
        solution(std::istream&);
        // copy constructor
        solution(const solution&) = default;
        // move constructor
        solution(solution&&) = default;
        // no assignment permitted
        solution& operator=(const solution&) = delete;
        solution& operator=(solution&&) = delete;
        // assign a variable
        bool assign_variable(variable, bool);
        // unassign a variable
        bool unassign_variable(variable);
        // reassign a variable
        void reassign_variable(variable, bool);
        // set status flag
        void set_valid(bool);
        // give the number of assigned variables
        std::size_t size() const;
        // provide variable assignments
        const std::map<variable, bool>& map() const;
        // give maximum variable key
        variable get_max_var() const;
        // give number of clauses
        clause get_num_clauses() const;
        // give problem type
        ProblemType get_type() const;
        // set maximum variable key
        void set_max_var(variable);
        // set number of clauses
        void set_num_clauses(clause);
        // set problem type
        void set_type(ProblemType);

    private:
        // tracks variable assignments
        std::map<variable, bool> variables;
        // store statistics
        std::map<std::string, std::string> stats;
        // largest variable
        variable max_var;
        // number of clauses
        clause clauses;
        // problem type
        ProblemType type;
        // valid flag
        bool valid;
        // used to print solution
        friend std::ostream& operator<<(std::ostream&, const solution&);
        // used to build solution from input stream
        friend solution& io::extract_solution(solution&, std::istream&);
    };

}

#endif
