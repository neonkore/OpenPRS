declare cwp PROPERTY-P
trace load op off
include "data/new-default.sym"
load opf "data/new-default.opf"
include "data/meta-intended-goal.sym"
load opf "data/meta-intended-goal.opf"
declare symbol (FACTORIAL RECURSIVE PREFER-ITERATIVE PREFER-RECURSIVE PRINT-FACTORIAL SSSOAK TAS-FACT FOO TEST_PAR TT)
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE
load opf "data/fact-meta-if.opf"
trace graphic op == on
trace text op = on
trace graphic opf "data/fact-meta-if.opf" on
trace text opf "data/fact-meta-if.opf" on
trace graphic op |Print Factorial| off
