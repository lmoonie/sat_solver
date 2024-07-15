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

    // used to specify the problem format
    enum ProblemType: bool {
        CNF = true,
        SAT = false
    };

    // a boolean variable; strictly positive
    using variable = int;

     // a disjunctive clause; strictly positive
    using clause = int;

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
        void unassign_variable(variable, bool);
        // give the number of assigned variable
        std::size_t size();
    private:
        // tracks variable assignments
        std::map<variable, bool> variables;
        // largest variable
        variable max_var;
        // number of clauses
        clause num_clauses;
        // problem type
        ProblemType type;
        // used to print solution
        friend std::ostream& operator<<(std::ostream&, const solution&);
        // used to build solution from input stream
        friend solution& io::extract_solution(solution&, std::istream&);
    };

}

#endif
