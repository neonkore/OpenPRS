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
include "data/truck-demo-fr.sym"
load opf "data/truck-demo-fr.opf"
load db "data/truck-demo-fr.db"
; Meta Options
set soak off
set meta goal off
