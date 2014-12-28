include "new-default.sym"
load opf "new-default.opf"

declare op_predicate PRINT-FIBONACCI
declare op_predicate FIBONACCI

load opf "fib-par.opf"
trace goal on
trace suc_fail on
trace fact on
set meta off
