declare cwp PROPERTY-P
trace load op off
include "data/new-default.sym"
load opf "data/new-default.opf"

declare op_predicate FACTORIAL
declare op_predicate PRINT-FACTORIAL
declare symbol RECURSIVE
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE

load opf "data/fact-meta-if-par.opf"
trace all off

trace user trace on
trace user op on
trace user intend on
trace user suc_fail on
trace user send on
trace user receive on
trace user fact on
trace user goal on
trace user relevant op on
trace user applicable op on
trace user thread on
