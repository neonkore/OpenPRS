
include "mini-default.sym"
load opf "mini-default.opf"
set meta off

declare op_predicate TEST-FAILED
declare op_predicate TEST-FAILED-2
load opf "fail.opf"

;add (! (PRINT "Post the goal (! (TEST-FAILED))."))

trace goal on
trace intend on
trace suc_fail on
;trace soak on
