# cnf
- CONSIDER +() AND \*() WHEN PROCESSING STRINGS
    - replace with variables
    - odd vars greater than max_var are false, even are true
    - use cnf facilities to simplify

- cdcl

# Next steps
- video presentation
- README
- review spec for final tasks
- code comments
- testing

# Future plans
- perf tuning
    - random restarts on local_search
    - replace .at() with []
    - cnf: consider not deleting all literals before deleting clause
    - assign_and_simplify notifies of empty clause
    - cache-efficient data structures
- more solution statistics
