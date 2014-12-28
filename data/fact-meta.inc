declare cwp PROPERTY-P
trace load op off

include "new-default.sym"
load opf "new-default.opf"

include "meta-intended-goal.inc"

declare op_predicate FACTORIAL
declare predicate RECURSIVE
declare op_predicate PRINT-FACTORIAL
declare predicate SSSOAK
declare op_predicate TAS-FACT
declare predicate FOO
declare predicate TEST_PAR
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE
load opf "fact-meta.opf"

trace graphic opf "fact-meta.opf" on
trace text opf "fact-meta.opf" on
trace graphic op |Print Factorial| off
