include "mini-default.sym"
load opf "mini-default.opf"

set meta off
declare predicate TEST-VAR
declare op_predicate TEST-GEN
load opf "special.opf"
trace graphic op TEST-VAR on
