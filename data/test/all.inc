include "new-default.sym"
load opf "new-default.opf"

declare predicate BOO
declare predicate BAR
declare function BAR
declare predicate BAR2
declare function BAR2

declare predicate TBAR
declare predicate BAR-LIST
declare predicate FOO
declare predicate FOOO
declare predicate FOOOO

load opf "all.opf"
conclude (BAR 1)
conclude (BAR 2)
conclude (BAR 3)
conclude (BOO 1)
conclude (BAR2 1 2)
conclude (BAR2 1 3)
conclude (BAR2 2 4)
