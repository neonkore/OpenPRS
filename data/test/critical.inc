declare ff DELAY 0

include "new-default.sym"
load opf "new-default.opf"
include "meta-intended-goal.sym"
load opf "meta-intended-goal.opf"
set soak off
set meta goal off
load opf "new-meta-selector.opf"
load opf "critical.opf"
;
;
;
trace goal on
;trace graphic op |critical 3| on
;trace graphic op |critical 3 bis| on
;trace graphic op |critical 3 ter| on
;trace graphic op |critical 4| on
;trace graphic op |critical 4 bis| on
;trace graphic op |critical 5| on
;trace text op |kill intention id| on
trace graphic opf "data/test/critical.opf" on
