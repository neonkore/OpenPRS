
include "mini-default.sym"
load opf "mini-default.opf"
set soak off
set meta goal off
trace goal on
trace intend on
trace suc_fail on

declare predicate VAL-X
declare predicate VAL-Y
load db "bind.db"

declare op_predicate TESTB1
declare op_predicate BIND1
declare op_predicate BIND2
declare op_predicate BIND3
declare op_predicate TESTB
declare op_predicate TESTB1
declare op_predicate TESTB2
declare op_predicate TESTB3
declare op_predicate TESTB4

load opf "bind.opf"
