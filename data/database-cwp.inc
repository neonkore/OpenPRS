;;; this module is supposed to test the closed world predicate mechanism.
declare cwp foo
consult (~ (foo a b))
conclude (foo a c)
consult (~ (foo a c))
consult (foo $x $y)
consult (~ (foo $x $y))
consult (~ (foo a $x))
consult (~ (foo b $x))
