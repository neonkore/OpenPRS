set meta on
set predicate off
set symbol off

trace thread on
trace goal on
declare op_predicate PRINT
load opf "data/test/text-opf-torture.opf"
trace graphic opf "text-opf-torture.opf" on
trace graphic op PRINT off
trace step opf "text-opf-torture.opf" on
trace step op PRINT off

add (! (PRINT "post the fact (TOPF) to run the tests "))
