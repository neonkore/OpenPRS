;;; This module is supposed to test the functional fact mechanism...
declare ff foo 1
declare ff bar 2
declare ff boo 0
conclude (foo a b)
consult (foo $x $y)
conclude (foo a c)
consult (foo $x $y)
conclude (foo a d)
consult (foo $x $y)
conclude (bar a b c d)
consult (bar $x $y $z $w)
conclude (bar a c e f)
consult (bar $x $y $z $w)
conclude (bar a b c f)
consult (bar $x $y $z $w)
conclude (bar a c f g)
consult (bar $x $y $z $w)
conclude (boo a)
consult (boo $x)
conclude (boo b)
consult (boo $x)
conclude (boo c)
consult (boo $x)
