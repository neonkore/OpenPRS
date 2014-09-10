/*                               -*- Mode: C -*- 
 * oprs-pred-func_f.h -- 
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
#include "oprs-type_f-pub.h"

#define PRED_NAME(pred_rec) ((pred_rec)->name)

#define DECLARE_PRED_EP(pred_rec,evp) ((pred_rec)->ep = (evp))

#define DECLARE_PRED_FF(pred_rec,ar) ((pred_rec)->ff = (ar))

#define DECLARE_PRED_CWP(pred_rec) ((pred_rec)->cwp = TRUE)

#define UNDECLARE_PRED_CWP(pred_rec) ((pred_rec)->cwp = FALSE)

#define DECLARE_PRED_OP(pred_rec) ((pred_rec)->op_pred = TRUE)

#define UNDECLARE_PRED_OP(pred_rec) ((pred_rec)->op_pred = FALSE)

#define DECLARE_PRED_BE(pred_rec) ((pred_rec)->be = TRUE)

#define UNDECLARE_PRED_BE(pred_rec) ((pred_rec)->be = FALSE)

#define PFR_PR(pfr) ((pfr)->u.u.pr)
#define PFR_EP(pfr) ((pfr)->u.u.pr?(pfr)->u.u.pr->ep:NULL)
#define PFR_OP_PRED(pfr) ((pfr)->u.u.pr?(pfr)->u.u.pr->op_pred:0)
#define PFR_FF(pfr) ((pfr)->u.u.pr?(pfr)->u.u.pr->ff:-2)
#define PFR_CWP(pfr) ((pfr)->u.u.pr?(pfr)->u.u.pr->cwp:0)
#define PFR_BE(pfr) ((pfr)->u.u.pr?(pfr)->u.u.pr->be:FALSE)
#define PFR_AC(pfr) ((pfr)->u.u.act)
#define PFR_VAR(pfr) ((pfr)->u.envar)
#define PFR_EF(pfr) ((pfr)->u.u.ef)
#define PFR_NAME(pfr) ((pfr)->name)

void make_pred_func_hash(void);
Pred_Rec *make_pred_rec(void);
void init_hash_size_pred_func(int hash_size);
void declare_cwp(Pred_Func_Rec *pred);
void declare_ff(Pred_Func_Rec *pr, int ar);
void declare_be(Pred_Func_Rec *pr);
void declare_predicate_op(Pred_Func_Rec *pr);
void un_declare_be(Pred_Func_Rec *pr);
void un_declare_func(Pred_Func_Rec *pr);
void check_or_set_pred_arity(Pred_Func_Rec *pr, int arity);
void check_or_set_pred_func_arity(Pred_Func_Rec *pfr, int arity);
void free_pred_hash(void);
void free_pred_func_hash(void);

void list_cwp(void);
void list_ff(void);
void list_be(void);
void list_ef(void);
void list_op_predicate(void);
void list_ep(void);
void list_function(void);
void list_predicate(void);
void list_ac(void);

void stat_pred_hashtable(void);

void un_declare_predicate_op(Pred_Func_Rec *pr);


Pred_Func_Rec *find_or_create_pred_func(Symbol pred_func_name);
Pred_Func_Rec *declare_pred_from_symbol(Symbol pfr_name);
Pred_Func_Rec *find_pred_func(Symbol pred_func_name);
Pred_Func_Rec *create_var_pred_func(Envar *var);

Term *evaluate_term_function(Eval_Funct *ef, Symbol ef_name, TermList tl);
Term *evaluate_term_action(Action *ef, Symbol ac_name, TermList tl);

Pred_Rec *declare_pred(Pred_Func_Rec *pfr);

void stat_pred_func_hashtable(void);
int stat_pred_func_hash_bucket (int *stat_array, Pred_Func_Rec *pr);
