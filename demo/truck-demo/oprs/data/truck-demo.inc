trace all off
;trace receive on
; 
; default OPs
;
include "new-default.sym"
load opf "new-default.opf"
;
; meta OPs
;
include "common-meta-processes.sym"
load opf "common-meta-processes.opf"
;
; The truck application
;
include "data/truck-demo.sym"
load opf "data/truck-demo.opf"
load db "data/truck-demo.db"
; Meta Options
set soak off
;set meta goal off
set meta off
