
include "mini-default.sym"
load opf "mini-default.opf"

set meta off
trace all off
declare predicate  TEST-BOUNDP
load opf "boundp.opf"
trace graphic op |Test BOUNDP| on
add (! (PRINT "Post the fact (TEST-BOUNDP)."))
