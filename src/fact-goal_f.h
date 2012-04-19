/*                               -*- Mode: C -*- 
 * fact-goal_f.h -- Declaration of external fonctions for fact-goal.c
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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

#include "fact-goal_f-pub.h"

Goal *make_goal_from_expr(Expression *expr, FramePtr frame);
Goal *make_goal_from_edge(Expression *expr, FramePtr frame, Thread_Intention_Block *thread, Control_Point *cp, Edge *edge);
Fact *make_fact_from_expr(Expression *expr);
void free_fact(Fact *f);
void free_goal(Goal *g);
void fprint_goal(FILE *f,Goal* g);
void print_goal(Goal* g);
void fprint_fact(FILE *f,Fact* fa);
void print_fact(Fact* fa);
void free_goal_from_op_instance(Goal *goal);
Goal *copy_goal(Goal *goal);
Goal *dup_goal(Goal *goal);
void free_fact_from_op_instance(Fact *fact);
Fact *dup_fact(Fact *fact);
void sprint_fact(Sprinter *sp, Fact *fact);
void sprint_goal(Sprinter *sp, Goal *g);
