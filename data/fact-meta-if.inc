declare cwp PROPERTY-P
trace load op off
include "new-default.sym"
load opf "new-default.opf"
include "meta-intended-goal.sym"
load opf "meta-intended-goal.opf"
declare symbol (FACTORIAL RECURSIVE PREFER-ITERATIVE PREFER-RECURSIVE PRINT-FACTORIAL SSSOAK TAS-FACT FOO TEST_PAR TT)
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE
load opf "fact-meta-if.opf"
trace graphic op == on
trace text op = on
trace graphic opf "fact-meta-if.opf" on
trace text opf "fact-meta-if.opf" on
trace graphic op |Print Factorial| off
