declare cwp PROPERTY-P
trace load op off

include "data/new-default.sym"
load opf "data/new-default.opf"

include "data/meta-intended-goal.inc"

declare op_predicate FACTORIAL
declare predicate RECURSIVE
declare op_predicate PRINT-FACTORIAL
declare predicate SSSOAK
declare op_predicate TAS-FACT
declare predicate FOO
declare predicate TEST_PAR
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE
load opf "data/fact-meta.opf"

trace graphic opf "data/fact-meta.opf" on
trace text opf "data/fact-meta.opf" on
trace graphic op |Print Factorial| off
