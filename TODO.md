# Admin
- Review project spec
- write README

# sat_verify
- Implement remaining CLI
- output false clause?
- implement ctrl+c

# brute_force
- add statistics

# dpll
- add statistics
- may need to expand the stack

# sat_solve
- implement remaining CLI
    - including custom parsers
- implement ctrl+c
- memory watching
- time watching

# cnf
- consider not deleting all literals before deleting clause
- CONSIDER +() AND \*() WHEN PROCESSING STRINGS
    - replace with variables
    - odd vars greater than max_var are false, even are true
    - use cnf facilities to simplify

# solvers
- catch errors and return ThreadPanic status

# Next steps
- cli
- timing/mem. usage
- CDCL
- video presentation

# Future plans
- perf tuning
    - random restarts on local_search
