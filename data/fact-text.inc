
declare op_predicate FACTORIAL
declare op_predicate PRINT-FACTORIAL
declare symbol RECURSIVE
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE

declare op_predicate INTENDED-OP
declare op_predicate PRINT

load opf "data/fact-text.opf"

trace step opf "data/fact-text.opf" on
trace graphic opf "data/fact-text.opf" on
;trace text opf "data/fact-text.opf" on
trace graphic op |Print Factorial| off

set meta on
set soak off
set meta fact off
set meta goal off
set meta goal op on

