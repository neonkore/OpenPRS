declare cwp PROPERTY-P
trace load op off
include "data/new-default.sym"
load opf "data/new-default.opf"

declare predicate FACTORIAL
declare predicate IT-FACTORIAL
declare symbol RECURSIVE
declare symbol PREFER-ITERATIVE
declare symbol PREFER-RECURSIVE
declare symbol PRINT-FACTORIAL

declare predicate FOO
declare predicate FOO1
declare predicate FOO2

declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE
declare op_predicate PRINT-FACTORIAL
declare op_predicate PRINT-IT-FACTORIAL
load opf "data/fact.opf"
