trace all off
;trace receive on
include "../../../data/new-default.sym"
load opf "../../../data/new-default.opf"
load opf "../../../data/common-meta-processes.opf"
include "data/truck-simu.sym"
load opf "data/truck-simu.opf"
load db "data/truck-simu.db"
set soak off
set meta goal off
trace graphic op |Truck| on
trace send on
trace receive on
