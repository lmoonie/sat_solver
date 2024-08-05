// cnf.cpp
// Logan Moonie
// Jul 12, 2024

#include "cnf.hpp"

namespace cnf {

    // will need abs frequently
    using std::abs;

    // a boolean variable; strictly positive
    using variable = int;

    // input stream constructor
    cnf_expr::cnf_expr(std::istream& istr) {
        io::extract_cnf_problem(*this, istr);
    }

    // add a literal to a clause
    void cnf_expr::add_literal(literal lit, clause cl) {
        if (!literals.contains(abs(lit))) {
            literals.insert({abs(lit), cl_set()});
            literals.insert({-abs(lit), cl_set()});
        }
        literals[lit].insert(cl);
        if (!clauses.contains(cl)) {
            clauses.insert({cl, lit_set()});
        }
        clauses[cl].insert(lit);
    }

    // remove a literal from a clause
    void cnf_expr::remove_literal(literal lit, clause cl) {
        literals[lit].erase(cl);
        clauses[cl].erase(lit);
    }

    // remove an entire clause
    void cnf_expr::remove_clause(clause cl) {
        for (const literal& lit : clauses[cl]) {
            literals[lit].erase(cl);
        }
        clauses.erase(cl);
    }

    // assign a variable and simplify the expression
    void cnf_expr::assign_and_simplify(variable var, bool val) {
        // keep a record of what to remove
        cl_set clauses_to_delete;
        std::unordered_multimap<literal, clause> literals_to_delete;

        // for each clause in which the negative literal appears
        for (auto const& cl : literals.at(-var)) {
            if (!val) {
                // literal is true; can delete the clause
                clauses_to_delete.insert(cl);
            } else {
                // literal is false; can remove literal from clause
                literals_to_delete.insert({-var, cl});
            }
        }
        // for each clause in which the positive literal appears
        for (auto const& cl : literals.at(var)) {
            if (val) {
                // literal is true; can delete the clause
                clauses_to_delete.insert(cl);
            } else {
                // literal is false; can remove literal from clause
                literals_to_delete.insert({var, cl});
            }
        }

        
        // delete the clauses and literals
        // this is delayed to prevent use-after-free errors
        for (auto const& [lit, cl] : literals_to_delete) {
            remove_literal(lit, cl);
        }
        for (auto const& cl : clauses_to_delete) {
            remove_clause(cl);
        }
    }

    // return an iterator to a unit clause
    std::unordered_map<clause, lit_set>::const_iterator cnf_expr::unit_clause() const {
        auto iter(clauses.begin());
        while (iter != clauses.end()) {
            if (iter->second.size() == 1) {
                return iter;
            }
            if (iter->second.empty()) {
                return clauses.end();
            }
            iter++;
        }
        return iter;
    }

    // return a pure literal
    literal cnf_expr::pure_literal() const {
        for (auto const& var : variables()) {
            if (!literals.at(var).empty() && literals.at(-var).empty()) {
                return var;
            }
            if (!literals.at(-var).empty() && literals.at(var).empty()) {
                return -var;
            }
        }
        return 0;
    }

    // end iterators for the maps
    std::unordered_map<literal, cl_set>::const_iterator cnf_expr::literals_end() const {
        return literals.end();
    }
    std::unordered_map<clause, lit_set>::const_iterator cnf_expr::clauses_end() const {
        return clauses.end();
    }

    // give the number of active clauses
    std::size_t cnf_expr::get_num_clauses() const {
        return clauses.size();
    }

    // give the number of active variables
    std::size_t cnf_expr::num_variables() const {
        return literals.size()/2;
    }

    // give the maximum variable key
    variable cnf_expr::get_max_var() const {
        return max_var;
    }

    // evaluate the expression
    bool cnf_expr::eval(const std::map<variable, bool>& assigns) const {
        for (auto const& [key, cl]: clauses) {
            bool clause_is_true(false);
            for (auto const& lit : cl) {
                if (
                    lit > 0 && assigns.at(abs(lit)) ||
                    lit < 0 && !assigns.at(abs(lit))
                ) {
                    clause_is_true = true;
                    break;
                }
            }
            if (!clause_is_true) return false;
        }
        return true;
    }

    // return a set of the expression's variables
    std::set<variable> cnf_expr::variables() const {
        std::set<variable> var_set;
        for (auto const& [lit, cl] : literals) {
            if (lit > 0) {
                var_set.insert(lit);
            }
        }
        return var_set;
    }

    // return a set of unsatisfied clauses
    std::set<clause> cnf_expr::unsatisfied_clauses(
        const std::map<variable, bool>& assigns
    ) const {
        std::set<clause> unsat_clauses;
        for (auto const& [key, cl]: clauses) {
            bool clause_is_true(false);
            for (auto const& lit : cl) {
                if (
                    lit > 0 && assigns.at(abs(lit)) ||
                    lit < 0 && !assigns.at(abs(lit))
                ) {
                    clause_is_true = true;
                    break;
                }
            }
            if (!clause_is_true) {
                unsat_clauses.insert(key);
            };
        }
        return unsat_clauses;
    }

    // check for empty clauses
    bool cnf_expr::empty_clause() const {
        for (auto const& [cl, lset] : clauses) {
            if (lset.empty()) {
                return true;
            }
        }
        return false;
    }

    // return an active variable
    variable cnf_expr::pick_var() const {
        return abs(*(clauses.begin()->second.begin()));
    }

    // return a reference to the requested clause
    const lit_set& cnf_expr::get_clause(clause cl) const {
        return clauses.at(cl);
    }

    // return a reference to the clause list of the requested literal
    const cl_set& cnf_expr::get_literal(literal lit) const {
        return literals.at(lit);
    }

    // get the problem type
    ProblemType cnf_expr::get_type() const {
        return type;
    }

    // find an empty clause, presuming there is one
    clause cnf_expr::get_empty_clause() const {
        for (auto const& [cl, lset] : clauses) {
            if (lset.empty()) {
                return cl;
            }
        }
        return 0;
    }

    // used to print cnf_expr
    std::ostream& operator<<(std::ostream& ostr, const cnf_expr& expr) {
        auto clause_iter(expr.clauses.begin());
        while (clause_iter != expr.clauses.end()) {
            ostr << "{ ";
            for (literal lit : clause_iter->second) {
                ostr << lit << " ";
            }
            ostr << "}" << std::endl;
            clause_iter++;
        }
        return ostr;
    }
}

// a collection of functions to convert SAT formatted input to CNF
namespace cnf::sat {

    // Remove extra whitespace, add helpful whitespace, remove extra
    // parentheses, remove extra formula nesting, etc.
    bool clean_sat_str(std::string& str) {
        std::string clean_str;
        std::size_t i = str.find_first_not_of(" \t\n");
        std::stack<bool> parenth;
        int negatives(0);
        std::stack<bool> in_conjunctive_clause;
        std::stack<bool> in_disjunctive_clause;
        in_conjunctive_clause.push(false);
        in_disjunctive_clause.push(false);
        while (i != std::string::npos) {
            if (str.at(i) == '*' || str.at(i) == '+') {
                if (!parenth.top()) {

                }
                if (
                    str.at(i) == '*' && in_conjunctive_clause.top() ||
                    str.at(i) == '+' && in_disjunctive_clause.top()
                ) {
                    parenth.push(false);
                } else {
                    if (negatives % 2 == 1) {
                        clean_str.push_back('-');
                        negatives = 0;
                    }
                    in_conjunctive_clause.push(str.at(i) == '*');
                    in_disjunctive_clause.push(str.at(i) == '+');
                    clean_str.push_back(str.at(i));
                    parenth.push(true);
                }
                i = str.find_first_not_of(" \t\n", i+1);
                if (str.at(i) == '(') {
                    if (parenth.top()) clean_str.append("( ");
                } else {
                    throw std::invalid_argument(err::expression_format);
                }
            } else if (str.at(i) == '(') {
                parenth.push(false);
            } else if (str.at(i) == ')') {
                if (parenth.empty()) {
                    throw std::invalid_argument(err::expression_format);
                }
                if (parenth.top()) {
                    clean_str.append(") ");
                    in_conjunctive_clause.pop();
                    in_disjunctive_clause.pop();
                }
                parenth.pop();
            } else if (str.at(i) == '-') {
                // check that the negative formula contains a valid formula
                std::size_t scan_depth(1);
                bool inner_formula(false);
                while (!inner_formula) {
                    std::size_t j(str.find_first_not_of(" \t\n", i+1));
                    if (j == str.npos || str.at(j) != '(') break;
                    j = str.find_first_not_of(" \t\n", j+1);
                    if (j == str.npos) throw std::invalid_argument(err::expression_format);
                    std::size_t clause_depth(1);
                    while (clause_depth > 0 && j < str.size()) {
                        if (str.at(j) == '(') clause_depth++;
                        if (str.at(j) == ')') clause_depth--;
                        if (clause_depth == scan_depth) {
                            if (str.at(j) == '+' ||
                                str.at(j) == '*'
                            ) {
                                if (inner_formula) {
                                    throw std::invalid_argument(err::expression_format);
                                } else {
                                    inner_formula = true;
                                }
                            } else if (std::isdigit(str.at(j))) {
                                if (inner_formula) {
                                    throw std::invalid_argument(err::expression_format);
                                } else {
                                    inner_formula = true;
                                    while (std::isdigit(str.at(j))) j++;
                                }
                            }
                        }
                        j++;
                    }
                }
                negatives++;
            } else {
                std::size_t num_len;
                try {
                    std::stoi(str.substr(i), &num_len);
                } catch (...) {
                    throw std::invalid_argument(err::expression_format);
                }
                if (negatives % 2 == 1) {
                    clean_str.push_back('-');
                    negatives = 0;
                }
                clean_str.append(str.substr(i, num_len));
                clean_str.push_back(' ');
                i += num_len - 1;
            }
            i = str.find_first_not_of(" \t\n", i+1);
        }
        if (!parenth.empty()) throw std::invalid_argument(err::expression_format);
        if (negatives > 0) throw std::invalid_argument(err::expression_format);
        // remove clauses containing only one element
        i = 0;
        bool string_changed = str != clean_str;
        str.clear();
        while (i < clean_str.size()) {
            if (clean_str.at(i) == '*' || clean_str.at(i) == '+') {
                std::size_t num_elements(0);
                std::size_t j(i + 2);
                std::size_t clause_depth(1);
                while (clause_depth > 0) {
                    if (clause_depth == 1) {
                        if (std::isdigit(clean_str.at(j))) {
                            num_elements++;
                            while (std::isdigit(clean_str.at(j))) j++;
                        } else if (clean_str.at(j) == '(') {
                            num_elements++;
                        }
                    }
                    if (clean_str.at(j) == '(') clause_depth++;
                    if (clean_str.at(j) == ')') clause_depth--;
                    if (num_elements > 1) break;
                    j++;
                }
                if (num_elements > 1) {
                    str.push_back(clean_str.at(i++));
                    parenth.push(true);
                } else {
                    i += 3;
                    parenth.push(false);
                    string_changed = true;
                    continue;
                }
            } else if (clean_str.at(i) == ')') {
                if (parenth.top()) {
                    parenth.pop();
                } else {
                    parenth.pop();
                    i += 2;
                    continue;
                }
            }
            str.push_back(clean_str.at(i));
            i++;
        }
        return string_changed;
    }

    bool diminish_complement(std::string& str) {
        // find the first negative sign not part of a literal
        std::size_t i(0);
        while (i < str.size()) {
            if (
                str.at(i) == '-' &&
                !std::isdigit(str.at(i + 1))
            ) break;
            i++;
        }
        // done diminishing complements
        if (i >= str.size()) return false;
        // remove complement
        str.erase(i, 1);
        // change clause type
        if (str.at(i) == '*') {
            str.replace(i, 1, "+");
            i++;
        } else if (str.at(i) == '+') {
            str.replace(i, 1, "*");
            i++;
        }
        // distribute complement
        i++;
        std::size_t open_clauses(1);
        while (open_clauses > 0) {
            // check for end of clause
            if (str.at(i) == ')') {
                open_clauses--;
            } else if (str.at(i) == '(') {
                open_clauses++;
            }
            if (open_clauses == 1) {
                if (
                    str.at(i) == '*' ||
                    str.at(i) == '+'
                ) {
                    str.insert(i, 1, '-');
                    i++;
                } else if (str.at(i) == '-') {
                    str.erase(i, 1);
                    while (std::isdigit(str.at(i))) i++;
                } else if (std::isdigit(str.at(i))) {
                    str.insert(i, 1, '-');
                    while (std::isdigit(str.at(++i)));
                }
            }
            i++;
        }
        return true;
    }

    bool distribute(std::string& str) {
        // find conjunctive clauses containing no nested conjunctive clauses
        // skip the first level
        std::size_t con_start(1);
        while (con_start < str.size()) {
            if (str.at(con_start) == '*') {
                std::size_t i(con_start + 2);
                std::size_t clause_depth(1);
                while(clause_depth > 0 && clause_depth < 3) {
                    if (str.at(i) == '(') clause_depth++;
                    if (str.at(i) == ')') clause_depth--;
                    i++;
                }
                if (clause_depth == 0) break;
                else con_start = i;
            }
            con_start++;
        }
        // done distributing; will wrap lone literals in disjunctions
        if (con_start >= str.size()) {
            std:size_t i(2);
            std::size_t clause_depth(1);
            while(clause_depth > 0) {
                if (str.at(i) == '(') clause_depth++;
                if (str.at(i) == ')') clause_depth--;
                if (clause_depth == 1) {
                    if (str.at(i) == '-' || std::isdigit(str.at(i))) {
                        str.insert(i, "+( ");
                        i += 3;
                        while (std::isdigit(str.at(++i)));
                        str.insert(i, " )");
                        i += 2;
                    }
                }
                i++;
            }
            return false;
        }
        // find start of parent disjunctive clause
        std::size_t dis_start(con_start);
        while (str.at(dis_start) != '+') dis_start--;
        // look for adjacent literal or conjunction
        bool val_before(false);
        bool val_after(false);
        std::size_t val_start(con_start);
        while (val_start > dis_start) {
            // check for literal
            if (std::isdigit(str.at(val_start))) {
                val_before = true;
                do val_start--; while (
                    std::isdigit(str.at(val_start)) ||
                    str.at(val_start) == '-'
                );
                val_start++;
                break;
            }
            // check for conjunction
            if (str.at(val_start) == ')') {
                val_before = true;
                do val_start--; while (
                    str.at(val_start) != '*'
                );
                break;
            }
            val_start--;
        }
        // search after conjunction in disjunction
        if (!val_before) {
            val_start = con_start + 2;
            // skip past conjunction
            std::size_t clause_depth(1);
            while(clause_depth > 0) {
                if (str.at(val_start) == '(') clause_depth++;
                if (str.at(val_start) == ')') clause_depth--;
                val_start++;
            }
            val_start++;
            // search for literal or conjunction
            while (str.at(val_start) != ')') {
                if (
                    std::isdigit(str.at(val_start)) ||
                    str.at(val_start) == '-' ||
                    str.at(val_start) == '*'
                ) {
                    val_after = true;
                    break;
                }
                val_start++;
            }
        }
        // find the end of the value
        std::size_t val_end(val_start + 1);
        std::string value;
        if (val_before || val_after) {
            if (std::isdigit(str.at(val_end))) {
                while (std::isdigit(str.at(val_end))) val_end++;
            } else if (str.at(val_end) == '(') {
                val_end++;
                std::size_t clause_depth(1);
                while(clause_depth > 0) {
                    if (str.at(val_end) == '(') clause_depth++;
                    if (str.at(val_end) == ')') clause_depth--;
                    val_end++;
                }
                val_end++;
            }
            value = str.substr(val_start, val_end - val_start);
            str.erase(val_start, val_end - val_start);
            if (val_before) {
                con_start -= val_end - val_start;
            }
        }
        // distribute the value over the conjunction
        std::size_t i(con_start + 2);
        std::size_t clause_depth(1);
        while (clause_depth > 0) {
            if (str.at(i) == '(') clause_depth++;
            if (str.at(i) == ')') clause_depth--;
            if (
                // literal
                std::isdigit(str.at(i)) ||
                str.at(i) == '-'
            ) {
                str.insert(i, "+( ");
                i += 2;
                str.insert(i, value);
                i += val_end - val_start + 1;
                while (
                    std::isdigit(str.at(i)) ||
                    str.at(i) == '-'
                ) i++;
                str.insert(i, ")");
                i++;
            } else if (
                // disjunction
                str.at(i) == '+'
            ) {
                str.insert(i, "+( ");
                i += 2;
                str.insert(i, value);
                i += val_end - val_start + 1;
                while (
                    str.at(i) != ')'
                ) i++;
                str.insert(i, ")");
                i++;
            }
            i++;
        }
        return true;
    }

    void remove_operators(std::string& str) {
        std::string cnf_str;
        std::size_t i(2);
        std::size_t clause_depth(1);
        while(clause_depth > 0) {
            if (clause_depth == 2) {
                if (str.at(i) != ')') {
                    cnf_str.push_back(str.at(i));
                } else {
                    cnf_str.push_back('0');
                }
            }
            if (str.at(i) == '(') clause_depth++;
            if (str.at(i) == ')') clause_depth--;
            i++;
        }
        str = cnf_str;
    }

    inline void convert_str_to_cnf(std::string& str) {
        // clean string and check formatting
        while (clean_sat_str(str));
        // move negation to leaves with Demorgan's Laws
        while (diminish_complement(str));
        // distribute disjunctions over conjunctions
        do {
            while(clean_sat_str(str));
        } while (distribute(str));
        remove_operators(str);
    }

}

// a collection of functions used to process problems via I/O
namespace cnf::io {

    // Given a suspected CNF problem line, parse_problem_line
    // extracts the number of variables and clauses, throwing
    // exceptions if the line is not formatted correctly.
    inline void parse_problem_line(
        const std::string& str,
        variable& max_var,
        ProblemType& type,
        std::size_t& clauses
    ) {
        // check problem format
        try {
            if (str.substr(0, 6) == "p cnf ") {
                type = ProblemType::CNF;
            } else if (str.substr(0, 6) == "p sat ") {
                type = ProblemType::SAT;
            } else {
                throw std::invalid_argument(err::problem_format);
            }
        } catch (std::out_of_range) {
            throw std::out_of_range(err::problem_format);
        }
        // record number of clauses and variables
        std::size_t clause_idx;
        try {
            max_var = std::stoi(str.substr(6), &clause_idx);
            if (type == ProblemType::CNF) {
                clauses = std::stoi(str.substr(6 + clause_idx));
            }
        } catch (std::out_of_range) {
            throw std::out_of_range(err::too_many_cl_var);
        } catch (...) {
            throw std::invalid_argument(err::problem_format);
        }
        if ((clauses <= 0 && type == ProblemType::CNF) || (max_var <= 0)) {
            throw std::invalid_argument(err::num_clauses_vars);
        }
    }

    // Given a CNF expression body (via input stream), parse_expression
    // extracts it contents into a cnf_expr, throwing errors if the
    // expression is not formatted correctly.
    inline void parse_expression(
        std::istream& istr,
        cnf_expr& expr,
        variable& max_var,
        const ProblemType& type,
        const std::size_t& clauses
    ) {
        literal lit;
        clause cl(1);
        std::string sat_str;
        do {
            // check for a comment line
            if (istr.peek() == 'c') {
                // discard the comment line
                istr.ignore(
                    std::numeric_limits<std::streamsize>::max(),
                    '\n'
                );
                continue;
            } else if (type == ProblemType::CNF) {
                // check that literal is an integer
                if (istr >> lit) {
                    if (std::abs(lit) > max_var) {
                        // not a valid literal
                        throw std::invalid_argument(err::invalid_variable);
                    } else if (lit == 0) {
                        // increment clause counter
                        cl++;
                    } else {
                        // integer is a valid literal
                        expr.add_literal(lit, cl);
                    }
                }
            } else {
                // remove comments and newlines from SAT formatted string
                std::string line;
                std::getline(istr, line);
                sat_str += line;
            }

        } while (istr.good());

        if (type == ProblemType::SAT) {
            // convert the expression to be in conjunctive normal form
            sat::convert_str_to_cnf(sat_str);
            // assign the string to the cnf_expr
            // check that literal is an integer
            std::stringstream ss(sat_str);
            do {
                if (ss >> lit) {
                    if (std::abs(lit) > max_var) {
                        // not a valid literal
                        throw std::invalid_argument(err::invalid_variable);
                    } else if (lit == 0) {
                        // increment clause counter
                        cl++;
                    } else {
                        // integer is a valid literal
                        expr.add_literal(lit, cl);
                    }
                }
            } while (ss.good());
        }

        // ensure the correct number of clauses was provided
        if (type == ProblemType::CNF && cl != clauses + 1) {
            throw std::invalid_argument(err::wrong_number_clauses);
        }
    }

    // extract_cnf_problem accepts a CNF-formatted problem
    // specification via an input stream and returns a cnf_expr.
    // It throws exceptions if the provided input cannot be read.
    cnf_expr& extract_cnf_problem(cnf_expr& expr, std::istream& istr) {
        // needed for parsing
        std::string line;
        std::size_t clauses;

        // find and parse the problem line
        while (std::getline(istr, line).good()) {
            // first non-ignored line must be problem line
            // empty lines and comment lines are ignored
            if (line.size() != 0 && line.at(0) != 'c') {
                parse_problem_line(line, expr.max_var, expr.type, clauses);
                break;
            }
        }
        // ensure no errors occurred while reading problem
        if (!istr) {
            throw std::ios_base::failure(err::io_err);
        }

        // parse the expression body
        parse_expression(istr, expr, expr.max_var, expr.type, clauses);
        // ensure no errors occurred while reading expression
        if (istr.bad()) {
            // likely an I/O error
            throw std::ios_base::failure(err::io_err);
        } else if (!istr.eof()) {
            // likely an invalid expression
            throw std::invalid_argument(err::expression_format);
        }

        // expr was generated successfully
        return expr;
    }

}
