/*                               -*- Mode: C -*- 
 * oprs-type_f.h -- Declaration of external function to oprs-type.c
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

#include "oprs-type_f-pub.h"


Property *build_property(PString name, Term *term);
FramePtr make_frame(FramePtr  pf);
FramePtr make_frame_not_inst(FramePtr  pf);
Undo *make_undo(Envar *ev);
void build_frame();

Term *build_term_from_var_list(VarList vl);
Term *build_term_l_list_from_c_list(TermList tl);
Term *build_goal_term(Goal *l);
Term *build_opi_term(Op_Instance *l);
Term *build_intention_term(Intention *l);
Term *build_fact_term(Fact *l);
Term *build_qstring(char *i);
Term *build_term_from_expr(Expression *expr);
Term *build_term_from_var(Envar* var);

Envar *build_envar(Symbol i,  Variable_Type flag);
Envar *build_var_in_frame(Symbol i,  Variable_Type flag);
Expression *build_simple_action_cached(Pred_Func_Rec *fr, TermList i);
Expression *build_expr_pfr_terms(Pred_Func_Rec *fr, TermList i);
Expression *build_expr(Symbol predicat, TermList i);
Expression *build_expr_to_cached_pred(Symbol predicat, TermList i, PBoolean signe);
Action_Field *build_action_field(Expression *tc, PBoolean special, PBoolean multi, List_Envar lv, Envar *var);
void make_id_hash();
int hash_a_string(PString name);
List_Envar copy_lenv(List_Envar le);
Frame *copy_frame_up_to_replaced_frame (Frame *fr1, Frame *prev, Frame *new_prev);
Frame *subst_lenv_in_copy_of_frame_up_to_frame(Frame *fr1,List_Envar lenv, Frame *frame);

VarList subst_lenv_in_vars_list(VarList vars, List_Envar lenv);
Envar * subst_lenv_in_envar(Envar * ev, List_Envar lenv);
Expression *subst_lenv_in_expr(Expression *expr, List_Envar lenv, Quote_Eval_Type quote_eval);
Expression *subst_lenv_or_remove_expr_in_expr(Expression *expr, List_Envar lenv, Expression *exprr);

/* nzo */
Term *subst_lenv_in_term(Term *term, List_Envar lenv, Quote_Eval_Type quote_eval);

PBoolean matching_lenv(List_Envar l1, List_Envar l2);
PBoolean match_var(Symbol name, Envar *ev);

void *select_randomly_c_list(Slist *l);

void free_property(Property *p);
void free_list_property(PropertyList pl);
void free_var_list(VarList vl);
void free_frame(FramePtr fp);
void free_lenv(List_Envar le);
void free_envar(Envar *ev);
void free_undo_list(List_Undo lu);
void free_undo(Undo *ud);
void free_expr(Expression *expr);
void free_expr_list(ExprList tl);
void free_expr_frame(ExprFrame *ef);
void free_action_field(Action_Field *ac);


Expression *copy_expr(Expression *expr);
ExprList copy_expr_list(ExprList exprs);
TermList copy_terms(TermList terms);
Float_array *copy_float_array(Float_array *fa);
Int_array *copy_int_array(Int_array *ia);
Envar * copy_envar(Envar * ev);

Frame *dup_frame(Frame *fp);

List_Envar dup_lenv(List_Envar le);
Expression *dup_expr(Expression *expr);
Term *dup_term(Term *term);
Envar * dup_envar(Envar * ev);
TermList dup_terms(TermList terms);
VarList dup_vars_list(VarList vars);

Expression *dup_expr_ct(Expression *expr);

void free_frame_until_empty(FramePtr fp);
void free_frame_until_frame(FramePtr fp1,FramePtr fp2);
void free_frame_and_envar_until_empty(FramePtr fp);
PString get_and_check_from_hashtable(Shash *hash, PString from);
void add_frame_to_free(Frame *to_add, Frame *until, FrameList fl);

Expression *build_expr_from_used_expr(Expression *t);

void yy_begin_COMMAND(void);
void init_id_hash(void);
void free_id_hash(void);

ExprFrame *make_ef(Expression *expr, Frame *f);
ExprListFrame *make_elf(ExprList exprl, Frame *f);

PString get_and_check_from_hashtable(Shash *hash, PString from);
int debackslashify(char *string);
PBoolean equal_string(PString s1, PString s2);

void make_global_var_list(void);
void free_global_var_list(void);

void enable_variable_parsing(void);
void disable_variable_parsing(void);

PBoolean check_and_set_id_case_option(PString id_case_option);
PBoolean expr_sort_pred(Expression *e1, Expression *e2);
int compare_term(Term *terms1, Term *terms2);
void select_language(char *language_str, PBoolean resource);
PBoolean global_variable(Envar *ev);

void init_hash_size_id(int hash_size);

PString remove_vert_bar(PString name);

void show_global_variable(void);
void stat_id_hashtable(void);

#ifdef VXWORKS
void take_or_create_sem(SEM_ID *sem);
void give_sem(SEM_ID sem);
#endif

/* Add_Del_List make_add_del_list(void); */

/* void free_add_del_list(Add_Del_List *adl); */
Expression *not_expr_not (Expression *expr);
Expression *not_expr_copy (Expression *expr);
Expression *not_expr_dup (Expression *expr);

#define OR_P(expr) ((expr)->pfr->name == or_sym)
#define AND_P(expr) ((expr)->pfr->name == and_sym)
#define NOT_P(expr) ((expr)->pfr->name == not_sym)
#define COND_ADD_P(expr) ((expr)->pfr->name == cond_add_sym)
#define COND_DEL_P(expr) ((expr)->pfr->name == cond_del_sym)
#define TEMP_CONCLUDE_P(expr) ((expr)->pfr->name == temp_conclude_sym)
#define RETRACT_P(expr) ((expr)->pfr->name == retract_sym)
#define ACHIEVE_P(expr) ((expr)->pfr->name == achieve_sym)
#define TEST_P(expr) ((expr)->pfr->name == test_sym)
#define PRESERVE_P(expr) ((expr)->pfr->name == preserve_sym)
#define WAIT_P(expr) ((expr)->pfr->name == twait_sym)
#define MAINTAIN_P(expr) ((expr)->pfr->name == maintain_sym)
#define CALL_P(expr) ((expr)->pfr->name == call_sym)

Expression *build_expr_sym_expr(Symbol predicat, Expression *i);

ExprList expr_list_from_term_list(TermList tl);

Temporal_Ope expr_temporal_type(Expression *expr);
DB_Ope expr_db_type (Expression *expr);

Expression *build_expr_pfr_expr(Pred_Func_Rec *predicat, Expression *i);
Expression *build_expr_pfr_terms(Pred_Func_Rec *pfr, TermList i);
Expression *build_expr_sym_exprs(Symbol predicat, ExprList i);


TermList transform_el_tl(ExprList tl);
Expression *build_expr_pfr_terms_external(Pred_Func_Rec *pfr, TermList i);
Expression *build_expr_sym_terms_external(Symbol predicat, TermList i);
ExprList make_el_from_tl(TermList tl);
