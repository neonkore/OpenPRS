trace all off
set meta off
include "data/new-default.sym"
load opf "data/new-default.opf"
declare op_predicate TEST-ALL
declare op_predicate REVERSE
declare op_predicate TEST1
declare op_predicate TEST2
declare op_predicate TEST3

load opf "data/example-list.opf"

