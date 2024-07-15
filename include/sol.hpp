// sol.hpp
// Logan Moonie
// Jul 13, 2024

#ifndef INC_SOL
#define INC_SOL

#include <map>
#include <istream>
#include <ostream>

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
        void assign_variable(variable, bool);
        // unassign a variable
        void unassign_variable(variable);
        // set status flag
        void set_valid(bool is_valid) {
            valid = is_valid;
        }
        // give the number of assigned variable
        std::size_t size() const;

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
