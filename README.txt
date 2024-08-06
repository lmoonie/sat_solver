Portfolio SAT Solver Application
Logan Moonie - August 5, 2024

The project can be built using the following commands from the repository root:

cmake -S. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
cmake --build build --clean-first --target $INSTALL_DIR

The executables sat_solve and sat_verify are found under "$INSTALL_DIR/bin/", in addition to a demonstration script called "demo".

--------------------------------
The sat_solve interface
--------------------------------

sat_solve accepts a problem via standard input or the '-p' option in CNF or SAT format, and provides a solution report, if possible, in SOL format via standard output. The tool outputs warnings and errors via standard error.

sat_solve may exit with the following statuses:
- 0: a solution was found
- 1: the problem was proven unsatisfiable
- 2: the solver could not finish (possibly due to a memory limit, time limit, malformed input, etc.)

The tool accepts several command line options in conventional format. Run "sat_solve --help" for a list of available options. The "--incomplete" flag currently has no effect on operation: In theory, it allows the tool to assume the problem is satisfiable, and select solvers that may find a solution faster. However, DPLL tends to be the fastest solver in the current implementation, and it can prove unsatisfiability regardless.

--------------------------------
The sat_verify interface
--------------------------------

sat_verify accepts both a solution and problem in SOL and CNF or SAT format, respectively. Both these arguments can be provided via command line options ('-s' and '-p', respectively). If only one is provided via the command line, the other must be provided via standard input. It simply reports whether or not the solution is valid.

sat_verify may exit with the following statuses:
- 0: the solution is valid
- 1: the solution is not valid
- 2: the solution's validity could not be verified (possibly due to malformed input)

A suggested use of sat_verify is to pass a problem to sat_solve, pipe the output (solution) into sat_verify, and pass sat_verify the same problem. Thus, the problem can be solved and its solution verified in a single command.

The tool accepts several command line options in conventional format. Run "sat_verify --help" for a list of available options.

--------------------------------
CNF format
--------------------------------

CNF files describe a satisfiability problem in conjunctive normal form. CNF files contain two sections: the problem line and the problem definition.

The problem line is a single line of the form

p cnf VARIABLES CLAUSES

where VARIABLES is a natural number representing the maximum number of variables and CLAUSES is a natural number representing the number of clauses in the problem expression. For example, if the problem line is "p cnf 22 7", the user may use variables with names that are integers ranging from 1 to 22, inclusive, and the expression must contain exactly 7 clauses; the user is not required to use all 22 possible variables.

The file ends with any number of lines representing the clauses that comprise the problem expression. This section can contain positive and negative integers, spaces, tabs, and newlines. Each integer is a literal corresponding to the variable of the same name. If the integer is negative, that literal has the negated value of that variable. For example, -11 is the literal that holds the logical negation of the variable 11. Each clause ends with the integer 0. Clauses may span multiple lines, and each line may contain all or part of the definition for multiple clauses.

Comment lines begin with a 'c' character followed by a space (this must be at the beginning of the line). They are intended to convey information to human readers and are ignored by the computer. Comments may appear anywhere in the file. Blank lines may also appear anywhere in the file. An example CNF file is given below. Note that '+' is logical OR, '*' is logical AND, and '~' is logical NOT.

(1 + ~(3) + 7) * (2 + 9) * (7 + ~(7) + ~(3) + 10)

is given by
--------------------------------
c example.cnf
c This is a comment
c Blank lines are acceptable

p cnf 11 3
1 -3 7 0
2 9 0
c Notice not all variables must be used
7 -7 -3 10 0
--------------------------------

--------------------------------
SAT format
--------------------------------

SAT files describe a satisfiability problem in an arbitrary form. SAT files contain two sections: the problem line and the problem definition.

Comments are allowed in exactly the same form as described in CNF (as are blank lines). The problem declaration line takes the form:

p sat VARIABLES

where VARIABLES is the maximum number of variables. Like in CNF, variables takes the name of a natural number from 1 to the maximum number of variables, inclusive, and not all possible variables must be used.

The problem definition specifies the logical expression of the problem. The expression must be specified as follows:

- x and -x are valid formulas f representing the value of variable x and its negation, respectively.
- +(g) is a valid formula f if g is a list of zero or more valid formulas separated by whitespace. f is evaluated as the boolean sum of those formulas.
- *(g) is a valid formula f if g is a list of zero or more valid formulas separated by whitespace. f is evaluated as the boolean product of those formulas.
- (f) is a valid formula equivalent to f if f is a valid formula.
- -(f) is a valid formula representing the negation of f if f is a valid formula.
- +() and *() are both valid formulas evaluating to false and true, respectively.
- The problem definition must be a valid formula.

Whitespace can appear anywhere it does not change the meaning of the expression (ie. not in the middle of a literal). It can also be omitted where the meaning is not changed. For example, (7 -13) and '(7-13)' are valid and equivalent. An example CNF file is given below. Note that '+' is logical OR, '*' is logical AND, and '~' is logical NOT.

(((1 * ~(3)) + 2 + 7) * (5 * ~(2))

is given by

--------------------------------
c example.sat

p sat 11

*(+(*(1 -3)2 7)
c Formulas can be broken over multiple lines

*(5-2))
--------------------------------

--------------------------------
SOL format
--------------------------------

SOL files describe whether a solution to a CNF or SAT problem was found, what solution was found, and additional metrics that may be useful to the user. The file may contain any number of comments or blank lines in any location; comments are formatted as in CNF. Remaining lines appear in the following order:

- a single solution line
- zero or more statistics lines
- zero or more variable lines

The solution line takes the form
--------------------------------
s TYPE SOLUTION VARIABLES CLAUSES
--------------------------------

where

- TYPE is either "cnf" or "sat" depending on the problem format provided.
- SOLUTION is 1 if a solution was found, 0 if the problem was found to be unsatisfiable, and -1 if no conclusion could be made.
- VARIABLES and CLAUSES are identical to the values provided in the problem declaration. CLAUSES is omitted if the TYPE is "sat".

The statistics lines take the form "t KEY VALUE". KEY is the name of some measurable quantity and VALUE is the result corresponding to that quantity, where both contain only letters, numbers, and underscores. For example, "t wall_time 23s" is a valid statistics line. Statistics lines may or may not appear in any case.

Variable lines appear only if a solution was found. Each line takes the form "v X", where X is the name of a variable with or without a prepended negative sign. The result x means the value of variable x must be set to true; -x means the value of variable x must be set to false. Variable lines may appear in any order. All variables used in the problem must appear in the solution.

An example solution file is given below (assume the variables 2, 5, and 9 were used in the problem):

--------------------------------
c example.sol
s cnf 1 12 3
t wall_time 2352ms

v 2
v 5
v -9
--------------------------------

--------------------------------
Third-party libraries
--------------------------------

This software uses the Boost Program Options library (Copyright 2002-2004 Vladimir Prus) to aid in handling user interaction. The Boost Program Options library is not distributed as part of this project and must be installed by the user in order to build the project.
