
include "mini-default.inc"

set meta off
trace goal on
trace intend on
trace suc_fail on
;trace soak on

load opf "maint.opf"
load db "maint.db"
trace graphic op |Simple Tests| on
trace graphic op |Tests| on
trace step op |Simple Tests| on
trace step op |Tests| on
;trace graphic opf "maint.opf" on
;trace step opf "maint.opf" on
;trace text opf "maint.opf" on
