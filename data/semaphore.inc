;How to create a semaphore?
;(! (SEM-CREATE $SEM $NUM $TYPE))
;where $SEM is the name of the semaphore 
;      $NUM is the number of token
;      $TYPE is SEM-BASIC or SEM-FIFO
;  in the database there will be the description of the semaphore 
;  ex:(semaphore foo 3)
;     (semaphore-type foo SEM-FIFO)
;     and some other datas such as (ancien_sem number)...
;
;How to ask for a token?
; (!(SEM-P $SEM))
;
;    there will be in database, for example 
;    (SEMAPHORE-ACTIVE foo 840 <Intention 0x24de08>) 
;      at the time 840 is a number which differentiate each token
;   
;How to free a token?
; (!(SEM-V $SEM))
;    
;    this module actually frees at random one of the token of current intention
;    (not the older)
; You cannot free a token in an other intention than the taking one!!!
;
; How to reset a semaphore?
; (!(SEM-RESET $SEM $NUM))
;   after reaching this goal, database is left with the same facts
;   as after a first call of (!(sem-create $SEM $NUM $TYPE))
;   all tokens are free

require "new-default.inc"
include "semaphore.sym"
load opf "semaphore.opf"
