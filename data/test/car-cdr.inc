include "new-default.sym"
load opf "new-default.opf"

declare predicate  TEST-C-R
declare symbol A
declare symbol B
declare symbol C
declare symbol D
declare symbol E
declare symbol F
load opf "car-cdr.opf"

add (! (PRINT "To run the test, post the fact (TEST-C-R)."))
