;load opf "new-default.sgraph"
load opf (

	     (|Meta Selector (safety preferred rest)|
	      (S4 (PROCESS) (S4 E0175 S5 ! (INTENDED-ALL-AS-ROOT $P)))
	      (CONTEXT (CONTEXT *FACT (> (LENGTH $X) 1)))
	      (S8 (PROCESS) (S8 E0207 END6 ! (INTEND (CURRENT (SELECT-RANDOMLY $P)))))
	      (N3 (PROCESS) (N3 E0168 S3 ! (= $S (CURRENT (SAFETY-HANDLERS-OF $F)))))
	      (S1 (PROCESS) (S1 E0039 S2 ! (= $R (CURRENT (LIST-DIFFERENCE (LIST-DIFFERENCE $F $S) $P)))))
	      (S7 (PROCESS) (S7 E0218 END5 ! (INTEND (CURRENT (SELECT-RANDOMLY $X)))))
	      (START (START) (START |e7750| N1 ! (= $F (CURRENT (FACT-INVOKED-OPS-OF $X))))) (END5 (END))
	      (N5 (PROCESS) (N5 E0188 S6 ! (= $P (CURRENT (PREFERRED-OF $X)))))
	      (S6 (PROCESS) (S6 E0192 S7 ? (NULL $P)) (S6 E0194 S8 ? (> (CURRENT (LENGTH $P)) 0)))
	      (PROPERTIES (PROPERTIES (DECISION-PROCEDURE T)))
	      (N4 (PROCESS) (N4 |e7755| N6 ? (> (CURRENT (LENGTH $S)) 0)) (N4 |e7754| N5 ? (NULL $S)))
	      (S3 (PROCESS) (S3 E0171 S1 ! (= $P (CURRENT (PREFERRED-OF $F)))))
	      (N1 (PROCESS) (N1 |Mike-Error| N3 ? (> (CURRENT (LENGTH $F)) 0)) (N1 |e7751| N2 ! (NULL $F)))
	      (EFFECTS (EFFECTS)) (N6 (PROCESS) (N6 |e7756| END2 ! (INTEND (CURRENT (SELECT-RANDOMLY $S)))))
	      (S5 (PROCESS) (S5 E0185 END4 ! (INTENDED-ALL-AS-ROOT $S))) (END4 (END))
	      (ACHIEVER (ACHIEVER . T)) (END2 (END)) (S2 (PROCESS) (S2 E0177 S4 ! (INTENDED-ALL $R)))
	      (DOCUMENTATION (DOCUMENTATION . 
					    "Meta OP used whenever
there are more than
one OPs applicable."
					    ))
	      (END6 (END)) (N2 (PROCESS) (N2 |e7753| N4 ! (= $S (CURRENT (SAFETY-HANDLERS-OF $X)))))
	      (INVOCATION (INVOCATION *FACT (SOAK $X))))

	     (|Print Factorial| (S1 (PROCESS) (S1 E0155 END1 ! (PRINT $N))) (ACHIEVER (ACHIEVER . T))
				(START (START) (START E0131 S1 ! (FACTORIAL $X $N))) (END1 (END)) (EFFECTS (EFFECTS))
				(INVOCATION (INVOCATION *GOAL (! (PRINT-FACTORIAL $X)))) (CONTEXT (CONTEXT))
				(DOCUMENTATION (DOCUMENTATION . "This OP just looks for the factorial of $x and prints the result."))
				(PROPERTIES (PROPERTIES)))

	     (|Print Iterative Factorial| (S1 (PROCESS) (S1 E0155 END1 ! (PRINT $N))) (ACHIEVER (ACHIEVER . T))
					  (START (START) (START E0131 S1 ! (IT-FACTORIAL $X $N))) (END1 (END)) (EFFECTS (EFFECTS))
					  (INVOCATION (INVOCATION *GOAL (! (PRINT-IT-FACTORIAL $X)))) (CONTEXT (CONTEXT))
					  (DOCUMENTATION (DOCUMENTATION . "This OP just looks for the factorial of $x and prints the result."))
					  (PROPERTIES (PROPERTIES)))

	     (|Recursive Factorial| (S3 (PROCESS) (S3 E0175 S4 ! (FACTORIAL (CURRENT (- $N 1)) $RES2)))
				    (S1 (PROCESS) (S1 E0168 END1 ! (= $RESULT $N))) (ACHIEVER (ACHIEVER . T))
				    (START (START) (START E0159 S1 ? (<= $N 1)) (START E0171 S3 ? (> $N 1))) (END1 (END))
				    (S4 (PROCESS) (S4 E0177 END2 ! (= $RESULT (* $N $RES2))))
				    (EFFECTS (EFFECTS))	; FACTORIAL $N $RESULT
				    (INVOCATION (INVOCATION *GOAL (! (FACTORIAL $N $RESULT)))) (END2 (END)) (CONTEXT (CONTEXT))
				    (DOCUMENTATION (DOCUMENTATION . "This OP computes the Factorial of $n in a recursive manner."
								  ))
				    (PROPERTIES (PROPERTIES (RECURSIVE T))))

	     (|Iterative Factorial| (S6 (PROCESS) (S6 E0128 S1 ! (= @TMP (- (CURRENT @TMP) 1))))
				    (S1 (PROCESS) (S1 E0072 S2 ? (<= @TMP 1)) (S1 E0114 S5 ? (> @TMP 1)))
				    (ACHIEVER (ACHIEVER . T)) (START (START) (START E0087 S4 ! (= @TMP $N))) (END1 (END))
				    (S4 (PROCESS) (S4 E0109 S1 ! (= @RES 1)))
				    (S5 (PROCESS) (S5 E0127 S6 ! (= @RES (* (CURRENT @RES) @TMP))))
				    (EFFECTS (EFFECTS FACTORIAL $N $RESULT))
				    (INVOCATION (INVOCATION *GOAL (! (IT-FACTORIAL $N $RESULT)))) (CONTEXT (CONTEXT))
				    (DOCUMENTATION (DOCUMENTATION . "This OP  computes the Factorial of $n and unifies the result with $result.
 It uses an iterative algorithm operating on local variables."))
				    (S2 (PROCESS) (S2 E0086 END1 ! (= $RESULT @RES))) (PROPERTIES (PROPERTIES (RECURSIVE NIL))))

	     (|test @|
	      (INVOCATION (INVOCATION *GOAL (! (foo $N)))) (CONTEXT (CONTEXT))
	      (START (START) (START E0087 S4 ! (= @TMP $N))) 
	      (S4 (PROCESS) (S4 E0109 S6 ! (= @RES 1)))
	      (S6 (PROCESS) (S6 E0128 S5 ! (= @TMP (- (CURRENT @TMP) 1))))
	      (S5 (PROCESS) (S5 E0127 END1 ! (= @RES (* (CURRENT @RES) @TMP))))
	      (END1 (END))
	      (ACHIEVER (ACHIEVER . T)) 
	      (EFFECTS (EFFECTS FOO @RES @TMP)))

	     (|test2 @|
	      (INVOCATION (INVOCATION *GOAL (! (foo1 $X)))) (CONTEXT (CONTEXT))
	      (START (START) (START E0087 S1 ! (= @TMP $X))) 
	      (S1 (PROCESS) (S1 E0109 S2 ! (PRINT @TMP)))
	      (S2 (PROCESS) (S2 E0128 S3 ! (= @TMP (+ (CURRENT @TMP) 1))))
	      (S3 (PROCESS) (S3 E0111 S4 ! (PRINT @TMP)))
	      (S4 (PROCESS) (S4 E0110 S5 ? (FOO @TMP)))
	      (S5 (PROCESS) (S5 E0112 END1 ! (PRINT @TMP)))
	      (END1 (END)))

	     )
