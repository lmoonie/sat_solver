# local_search
- check how it handles empty clauses/expressions

# cnf
- consider not deleting all literals before deleting clause
- CONSIDER +() AND \*() WHEN PROCESSING STRINGS
    - replace with variables
    - odd vars greater than max_var are false, even are true
    - use cnf facilities to simplify
- sat not pointing out negative misformat properly

# Next steps
- CDCL
- video presentation
- README
- review spec for final tasks

# Future plans
- perf tuning
    - random restarts on local_search
    - replace .at() with []
- more solution statistics
