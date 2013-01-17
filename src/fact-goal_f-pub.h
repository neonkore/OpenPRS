/*                               -*- Mode: C -*- 
 * fact-goal_f-pub.h -- Declaration of public external functions for fact-goal.c
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifdef  __cplusplus
extern "C"  {
#endif

void fprint_goal(FILE *f,Goal* g);
void print_goal(Goal* g);
void fprint_fact(FILE *f,Fact* fa);
void print_fact(Fact* fa);
PString fact_sender(Fact *fact);
PDate fact_soak(Fact *fact);
PDate fact_creation(Fact *fact);
PDate fact_reception(Fact *fact);
PDate fact_response(Fact *fact);
Expression *fact_statement(Fact *fact);
PDate goal_soak(Goal *goal);
PDate goal_creation(Goal *goal);
PDate goal_reception(Goal *goal);
PDate goal_response(Goal *goal);
Intention *goal_intention(Goal *goal);
Expression *goal_statement(Goal *goal);

Fact *make_fact_from_expr(Expression *expr);

#ifdef __cplusplus
}
#endif
