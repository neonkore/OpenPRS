set oprs_data_path "data:${HOME}/openrobots/share/openprs/data"

declare cwp PROPERTY-P
trace load op off
include "new-default.sym"
load opf "new-default.opf"

declare op_predicate FACTORIAL
declare op_predicate PRINT-FACTORIAL
declare symbol RECURSIVE
declare cwp PREFER-RECURSIVE
declare cwp PREFER-ITERATIVE

load opf "fact-meta-if-par.opf"
trace all off

trace step opf "fact-meta-if-par.opf" on
trace graphic opf "fact-meta-if-par.opf" on
trace text opf "fact-meta-if-par.opf" on
trace graphic op |Print Factorial| off
