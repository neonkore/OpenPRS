;Ce fichier contient un certain nombre de statements qui doivent TOUS etre unifiable 2 a 2.
; Il suffit de le feeder a la version courante de OPRS pour voir les resultats.

; declare predicate and functions to avoid warnings.
declare predicate P
declare predicate P2
declare predicate P1
declare predicate Q
declare function f
declare function f
declare function g
declare function k
declare function j

unify (P a b c) (P a b c) 
unify (P $x b c) (P $y b c)
unify (P a b c) (P a  @b c) 
unify (P a $z c) (P a (f 1 2 3) c) 
unify (P a $x c) (P a "alsd aksd this is a string" c) 
unify (P a "This is a String" c) (P a  "This is a String" c) 
unify (P a (f $x b) c) (P a (f a b) c) 
unify (P a b (g h @e j (k 1 $z 3 ) (j 345.345 "asdasd" $y))) (P a b (g h i j (k 1 2 3 ) (j 345.345 "asdasd" c) ))
unify (Q a b c) (Q $a $b $c) 
unify (~ (Q a b c)) (~ (Q $a $b $c))
unify (P2 b c) (P2 b c) 
unify (P1 c) (P1 c) 
unify (P a c c) (P a  @b @b) 
unify (P a b (g h $e j (k 1 $e 3 ) (j 345.345 "asdasd" $e))) (P a b (g h i j (k 1 i 3 ) (j 345.345 "asdasd" i) ))
