include "data/new-default.sym"
load opf "data/new-default.opf"

declare op_predicate PRINT-FIBONACCI
declare op_predicate FIBONACCI

load opf "data/fib-par.opf"
trace goal on
trace suc_fail on
trace fact on
set meta off
