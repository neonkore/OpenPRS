declare cwp PROPERTY-P
trace load op off
include "data/new-default.sym"
load opf "data/new-default.opf"

declare op_predicate FACTORIAL
declare op_predicate PRINT-FACTORIAL
declare symbol RECURSIVE
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE

load opf "data/test/meta-fact-goal.opf"

trace step opf "data/test/meta-fact-goal.opf" on
trace graphic opf "data/test/meta-fact-goal.opf" on
trace text opf "data/test/meta-fact-goal.opf" on

set meta fact op on
set meta goal op on
