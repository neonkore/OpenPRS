include "new-default.sym"
load opf "new-default.opf"
include "meta-intended-goal.sym"
load opf "meta-intended-goal.opf"

declare op_predicate TEST-GOAL
declare op_predicate TEST-GOAL2
declare op_predicate TEST-GOALS
declare op_predicate TEST-GOALS2
declare op_predicate IT-FACTORIAL
declare op_predicate PRINT-FACTORIAL
load opf "goal.opf"
set soak off
set meta goal off
;trace graphic opf "goal.opf"

