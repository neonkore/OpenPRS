/*                               -*- Mode: C -*- 
 * op-instance_f.h -- op-instance.c external functions declaration.
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

#include "op-instance_f-pub.h"

Op_Instance *make_op_instance_from_fact(Op_Structure *op, FramePtr frame, List_Envar lenv, Fact *fact);
Op_Instance *make_op_instance_from_goal(Op_Structure *op, FramePtr frame, List_Envar lenv, Goal *goal);
Op_Instance *make_op_instance_for_maint_goal(Op_Structure *op, FramePtr frame, List_Envar lenv, Goal *goal, Expression *maint_expr);
Op_Instance *make_op_instance_from_goal_satisfied_in_db(FramePtr frame, Goal *goal);
Op_Instance *make_op_instance_from_waiting_goal(Goal *goal);
Op_Instance *make_op_instance_for_intention_from_goal(FramePtr frame, Goal *goal);
L_List satisfy_prop_l_list_opi(L_List l, Symbol prop);
L_List fact_invoked_ops_l_list_opi(L_List l);
PBoolean property_p_ep(TermList tl);
PBoolean property_p(Op_Instance *opi, Symbol prop);
Term *property_of_ef(TermList tl);
void free_op_instance(Op_Instance *opi);
void free_op_instance_from_gf(Op_Instance *opi);
void free_op_instance_not_goal(Op_Instance *opi);
Op_Instance *dup_op_instance(Op_Instance *opi);
void fprint_op_instance(FILE *f,Op_Instance *opi);
void sprint_op_instance(Sprinter *sp, Op_Instance *opi);
ListLines pretty_print_op_instance(int width,  Op_Instance *opi);

