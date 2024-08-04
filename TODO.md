# solvers
- add statistics
- catch and report errors via ThreadPanic status

# sat_solve
- implement ctrl+c
- messages of varying verbosity
- adjust polling interval to feel more responsive
    - can use condition variable at the same time?

# cnf
- consider not deleting all literals before deleting clause
- CONSIDER +() AND \*() WHEN PROCESSING STRINGS
    - replace with variables
    - odd vars greater than max_var are false, even are true
    - use cnf facilities to simplify

# Next steps
- CDCL
- video presentation
- README
- review spec for final tasks

# Future plans
- perf tuning
    - random restarts on local_search
