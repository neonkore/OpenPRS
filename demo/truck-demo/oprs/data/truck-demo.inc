trace all off
;trace receive on
; 
; default OPs
;
require "new-default.inc"
;
; meta OPs
;
include "common-meta-processes.sym"
load opf "common-meta-processes.opf"
;
; The truck application
;
load external "/home/felix/work/openrobots/lib/truck-demo-oprs" "init_truck_demo"
include "/home/felix/work/openrobots/share/truck-demo/data/truck-demo.sym"
load db "/home/felix/work/openrobots/share/truck-demo/data/truck-demo.db"
load opf "/home/felix/work/openrobots/share/truck-demo/data/truck-demo.opf"

; Meta Options
set soak off
;set meta goal off
set meta off
