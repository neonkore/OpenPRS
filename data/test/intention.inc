
include "new-default.sym"
load opf "new-default.opf"

set soak on
set meta goal off
set meta fact off
load opf "new-meta-selector.opf"

declare predicate CREATE-NB-KOS
declare cwp STOP-CREATE-KOS
declare cwp STOP-CREATE-NB-KOS
declare cwp END-KOS

declare predicate KILL-OR-SLEEP-2
declare cwp CHANGE-TAG-KILL-OR-SLEEP
declare ff CHANGE-TAG-KILL-OR-SLEEP 0


declare predicate MANAGE
declare op_predicate MANAGE-IN
declare cwp STOP-MANAGE

declare symbol TEST-INT
declare op_predicate ASLEEPS
declare op_predicate ASLEEPS-COND
declare op_predicate ASLEEP
declare op_predicate ASLEEP-COND
declare predicate CHP
declare predicate KILL-ME
declare be CHANGE-TAG-KILL-ME

declare op_predicate KILL
declare op_predicate KILLS

declare predicate COND-SLEEP
declare function COND-SLEEP
declare predicate TEST-ASLEEP
declare predicate TEST-ASLEEP-COND
declare predicate TEST-ASLEEP-COND2
declare predicate TEST-ASLEEP-COND3
declare predicate TEST-ASLEEP-COND4
declare predicate TEST-ASLEEPS-COND
declare op_predicate WAKE-UP


declare symbol BAR
declare symbol BOO
declare symbol FOO

load opf "intention.opf"

trace text op |intention to kill| on
trace graphic op |test asleep cond| on
trace text op |test asleep cond| on
trace graphic op |test asleep| on
trace text op |test asleep| on
trace text op |kill intention id| on
