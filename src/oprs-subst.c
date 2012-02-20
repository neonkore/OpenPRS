
/*                               -*- Mode: C -*- 
 * oprs-subst.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
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

#include "config.h"

#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"

#include "oprs-type_f.h"
#include "unification_f.h"
#include "lisp-list_f.h"
#include "fact-goal_f.h"
#include "op-instance_f.h"
#include "intention_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"


/*
 ****************************************************************
 * Function to copy various expression while we do variable replacement.
 **************************************************************** 
 */

TermList subst_lenv_in_terms(TermList terms, List_Envar lenv, Quote_Eval_Type quote_eval)
{
     return sl_copy_slist_func2(terms, lenv, (void *)quote_eval, subst_lenv_in_term);
}

ExprList subst_lenv_in_expr_list(ExprList exprs, List_Envar lenv, Quote_Eval_Type quote_eval)
{
     return sl_copy_slist_func2(exprs, lenv, (void *)quote_eval, subst_lenv_in_expr);
}

Expression *subst_lenv_in_expr(Expression *expr, List_Envar lenv, Quote_Eval_Type quote_eval)
{
     Expression *t = MAKE_OBJECT(Expression);

     t->pfr = expr->pfr;


     t->terms = subst_lenv_in_terms(expr->terms, lenv, quote_eval);

     if (!t->pfr->name) { /* No name means a variable... */
	  Envar *envar = subst_lenv_in_envar(t->pfr->u.envar, lenv);
	  Term *tt = find_binding_envar(envar);
	  free_envar(envar);
	  if (!tt || tt->type != TT_ATOM) {
	       report_fatal_external_error("subst_lenv_in_expr_ct: variable in predicate position not properly bound.");
	  }
	  t->pfr = find_or_create_pred_func(tt->u.id);
	  check_or_set_pred_func_arity(t->pfr, sl_slist_length(t->terms));
     }
     return t;
}


extern PBoolean match_var(Symbol name, Envar *ev);

PBoolean check_all_vars_in_subst_lenv = TRUE;

Envar *subst_lenv_in_envar(Envar *ev, List_Envar lenv)
{
     Envar *var;

     if (((var = (Envar *)sl_search_slist(lenv, ev->name, match_var)) == NULL) &&
	 ((var = (Envar *)sl_search_slist(global_var_list, ev->name, match_var)) == NULL)) {
	  if (check_all_vars_in_subst_lenv) 
	       fprintf(stderr, LG_STR("subst_lenv_in_envar: Warning var %s\n\
\tnot found in the list envar (local and global).\n",
				      "subst_lenv_in_envar: Attention, la variable %s\n\
\tn'a pas été trouvé dans la liste des variables (locales et globales).\n"), ev->name);
	  return dup_envar(ev);
     } else
	  return dup_envar(var);
}


VarList subst_lenv_in_vars_list(VarList vars, List_Envar lenv)
{
     return sl_copy_slist_func1(vars, lenv, subst_lenv_in_envar);
}

/* **************************************************************
 * For wipping out the mexpr already unified
 * ************************************************************** */

TermList subst_lenv_or_remove_expr_in_terms(ExprList terms, List_Envar lenv, Expression *expr);

Expression *subst_lenv_or_remove_expr_in_expr(Expression *expr, List_Envar lenv, Expression *exprr)
{
  if (expr == exprr)
    return NULL;
  else {
    if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
      return build_expr_pfr_terms(expr->pfr,
				  subst_lenv_or_remove_expr_in_terms(expr->terms, lenv, exprr));
    } else {
      return subst_lenv_in_expr(expr,lenv,QET_NONE);
    }
  }
}

Term *subst_lenv_or_remove_expr_in_term(Term *t, List_Envar lenv, Expression *exprr)
{
     return build_term_expr(subst_lenv_or_remove_expr_in_expr(TERM_EXPR(t),lenv, exprr));
}

TermList subst_lenv_or_remove_expr_in_terms(TermList terms, List_Envar lenv, Expression *expr)
{
     return sl_copy_slist_func2(terms, lenv, expr, subst_lenv_or_remove_expr_in_term);
}

L_List subst_lenv_in_l_list(L_List list, List_Envar lenv, Quote_Eval_Type quote_eval)
{
     L_List tmp, previous, new_list = l_nil;
     
     tmp = l_nil;		/* Just to avoid gcc warning. */
     previous = l_nil;		/* Just to avoid gcc warning. */
     
     if (list != l_nil) {
	  while (list != l_nil) {
	       tmp = MAKE_OBJECT(struct l_list);
	       
	       tmp->car = subst_lenv_in_term(list->car, lenv, quote_eval);
	       tmp->size = L_SIZE(list); 
	       
	       if(new_list == l_nil)
		    new_list = tmp;
	       else
		    previous->cdr = tmp;

	       list = CDR(list);
	       previous = tmp;
	  }
	  tmp->size = 1; /* The last one */
	  tmp->cdr = l_nil;
     }
     return new_list;
}

Term *subst_lenv_in_term(Term *term, List_Envar lenv, Quote_Eval_Type quote_eval)
{
     Term *res = MAKE_OBJECT(Term);

     *res = *term;

     switch (term->type) {
     case INTEGER:
     case LONG_LONG:
     case U_POINTER:
     case U_MEMORY:
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(*term->u.doubleptr);
	  break;
     case FLOAT_ARRAY:
	  res->u.float_array = copy_float_array(term->u.float_array);
	  break;
     case INT_ARRAY:
	  res->u.int_array = copy_int_array(term->u.int_array);
	  break;
     case LISP_LIST:
	  res->u.l_list = subst_lenv_in_l_list(term->u.l_list, lenv, quote_eval);
	  break;
     case TT_FACT:
	  dup_fact(term->u.fact);
	  break;
     case TT_GOAL:
	  dup_goal(term->u.goal);
	  break;
     case TT_OP_INSTANCE:
	  dup_op_instance(term->u.opi);
	  break;
     case TT_INTENTION:
	  dup_intention(term->u.in);
	  break;
     case C_LIST:
	  DUP_SLIST(term->u.c_list);
	  break;
     case STRING:
	  DUP(term->u.string);
	  break;
     case TT_ATOM:
	  break;
     case VARIABLE:
	  res->u.var = subst_lenv_in_envar(term->u.var, lenv);
	  break;
/*      case LENV: */
/* 	  res->u.var_list = subst_lenv_in_vars_list(term->u.var_list, lenv); */
/* 	  break; */
     case EXPRESSION:
	  if (quote_eval == QET_NONE) {
	       res->u.expr = subst_lenv_in_expr(term->u.expr, lenv, quote_eval);
	  } else {
	       if (term->u.expr->pfr->name == quote_sym) {
		    quote_eval = QET_QUOTE;
		    FREE(res);
		    res = subst_lenv_in_term((Term *)sl_get_slist_head(term->u.expr->terms),
								lenv, quote_eval);
	       } else {
		    if (quote_eval == QET_QUOTE) {
			 if (term->u.expr->pfr->name == current_sym) {
			      Term *tmp_res;
			  
			      quote_eval = QET_EVAL;
			      FREE(res);
			      tmp_res = subst_lenv_in_term(
				   (Term *)sl_get_slist_head(term->u.expr->terms),
				   lenv, quote_eval);
			      res = find_binding_ef_or_prog_var(tmp_res);
			      free_term(tmp_res);
			 } else {
			      res->u.expr = subst_lenv_in_expr(term->u.expr, lenv,
								    quote_eval);
			 }
		
		    } else {
			 Term *tmp_res;
		     
			 if (term->u.expr->pfr->name == current_sym) {
			      FREE(res);
			      tmp_res = subst_lenv_in_term((Term *)
							   sl_get_slist_head(term->u.expr->terms),
							   lenv, quote_eval);
			      res = find_binding_ef_or_prog_var(tmp_res);
			      free_term(tmp_res);
			 } else {
			      res->u.expr  =  subst_lenv_in_expr(term->u.expr, lenv,
								      quote_eval);
			      tmp_res = res;
			      res = find_binding_ef_or_prog_var(tmp_res);
			      free_term(tmp_res);
			 }
		    }
	       }		
	  }	
	  break;
     default:
	  oprs_perror("subst_lenv_in_term",PE_UNEXPECTED_TERM_TYPE);
	  break;
     }
     return res;
}

Undo *subst_lenv_in_copy_undo(Undo *und, List_Envar lenv)
{
     Undo *u = MAKE_OBJECT(Undo);
     Envar *var;

     if (((var = (Envar *)sl_search_slist(lenv, und->envar->name, match_var)) == NULL) &&
	 ((var = (Envar *)sl_search_slist(global_var_list, und->envar->name, match_var)) == NULL)) {
	  if (check_all_vars_in_subst_lenv) 
	       fprintf(stderr, LG_STR("subst_lenv_in_copy_undo: Warning var %s\n\
\tnot found in the list envar (local and global).\n",
				      "subst_lenv_in_copy_undo: Attention, la variable %s\n\
\tn'a pas été trouvé dans la liste des variables (locales et globales).\n"), und->envar->name);
	  u->envar = und->envar;
	  u->value = dup_term(und->value);

	  return u;
     }
     u->envar = var;
     u->value = dup_term(und->value);

     return u;
}

List_Undo subst_lenv_in_sl_copy_slist_undo(List_Undo lu, List_Envar lenv)
{
     if (lu)
	  return sl_copy_slist_func1(lu, lenv, subst_lenv_in_copy_undo);
     else 
	  return  NULL;
}

Frame *subst_lenv_in_copy_of_frame_up_to_frame(Frame *fr1, List_Envar lenv, Frame *fr2)
{
     if (fr1 != fr2) {
	  Frame *new_fp = MAKE_OBJECT(Frame);

	  *new_fp = *fr1;
	  new_fp->previous_frame = subst_lenv_in_copy_of_frame_up_to_frame(fr1->previous_frame,lenv, fr2);
	  new_fp->list_undo = subst_lenv_in_sl_copy_slist_undo(fr1->list_undo, lenv);

	  return new_fp;
     } else
	  return dup_frame(fr1);
}
