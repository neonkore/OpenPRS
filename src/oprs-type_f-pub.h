/*                               -*- Mode: C -*- 
 * oprs-type_f-pub.h -- 
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

Symbol declare_atom(char *id);
Symbol make_atom(char *atom);
Symbol find_atom(char *id);
PString make_string(char *string);


double *make_doubleptr(double i);
void free_doubleptr(double *i);


Term *build_integer(int i);
Term *build_long_long(long long int i);
Term *build_float(double i);
Term *build_l_list(L_List l);
Term *build_c_list(Slist *l);
Term *build_qstring(char *i);
Term *build_string(const char *i);
Term *build_pointer(void *p);
Term *build_term_expr(Expression *termc);
Term *build_id(Symbol i);
Term *build_nil(void);
Term *build_t(void);

Term *build_memory(void *p, int size);
int u_memory_size(Term *t);
void *u_memory_mem(Term *t);

Term *transform_term_c_list_to_term_l_list(Term *res);

TermList build_term_list(TermList tl, Term *t);
Term *build_term_l_list_from_c_list(TermList tl);
Expression *build_expr_pfr_terms(Pred_Func_Rec *pfr, TermList i);
Expression *build_expr_sym_terms(Symbol predicat, TermList i);

Expression *subst_termlist_in_termcomp(Expression *tc, TermList i);

Term *copy_term(Term *term);

void free_term(Term *term);
void free_tl(TermList tl);

void oprs_free_list(Slist *l);


Symbol pred_func_rec_symbol(Pred_Func_Rec *pfr);
Pred_Func_Rec *declare_pred_from_symbol(Symbol pfr_name);
Pred_Func_Rec *find_or_create_pred_func(Symbol pred_func_name);

int hash_a_string(const char *name);

#ifdef SPRINT_RETURN_STRING
int num_char_sprint(char *x);
#else
int num_char_sprint(int x);
#endif

#define TERM_EXPR(term) ((term)->u.expr)
#define EXPR_TERMS(expr) ((TermList)((expr)->terms))
#define EXPR_TERM1(expr) ((Term *)sl_get_slist_head(EXPR_TERMS(expr)))
#define EXPR_EXPR1(expr) (TERM_EXPR((Term *)sl_get_slist_head(EXPR_TERMS(expr))))

#ifdef __cplusplus
}
#endif
