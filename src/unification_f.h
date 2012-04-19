/*                               -*- Mode: C -*- 
 * unification_f.h -- declaration des fonctions externes a unification.
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2003 Francois Felix Ingrand.
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

PBoolean unify_term(Term *t1, Term *t2, FramePtr frame);
PBoolean unify_term_from_db(Term *t1, Term *t2, FramePtr frame, PBoolean dup_var);
void test_unify_expr(Expression *e1, Expression *e2);

PBoolean unify_expr(Expression *e1, Expression *e2, FramePtr frame);
PBoolean unify_expr_from_db(Expression *e1, Expression *e2, FramePtr frame, PBoolean dup_var);

PBoolean equal_expr_literal_var(Expression *e1, Expression *e2);
PBoolean equal_l_list_literal_var(L_List l1, L_List l2);

PBoolean equal_expr(Expression *e1, Expression *e2);
PBoolean opposite_expr(Expression *e1, Expression *e2);
/* enzo: similar to equal_expr... */
PBoolean equal_term(Term *t1, Term *t2);
PBoolean equal_l_list(L_List l1, L_List l2);
PBoolean equal_l_car(Term *car1, Term *car2);

void unbind_lenv(List_Envar le);
void desinstall_frame(FramePtr fp1, FramePtr fp2);
FramePtr reinstall_frame(FramePtr fp1);
PBoolean matching_lenv_between_frames(List_Envar le1, Frame *fr1, List_Envar le2, Frame *fr2);
void rebind_var(Undo * un);
FramePtr bind_var(Envar *ev, Term *t, FramePtr fp);

Term *find_binding(Term *t);
Term *find_binding_envar(Envar *envar);
Term *find_binding_ef(Term *t);
Term *find_binding_or_prog_var(Term *t);
Term *find_binding_ef_or_prog_var(Term *t);
TermList find_binding_ef_tl(TermList tl);
TermList find_binding_ef_or_prog_var_tl(TermList tl);
Expression *find_binding_expr(Expression *expr);

/* enzo */
PBoolean equal_expr_wo_sign(Expression*, Expression*);
PBoolean equal_expr_for_given_depth(Expression *, Expression *, int);

Expression *find_binding_expr(Expression *expr);
