static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*-
 * oprs-copy.c -- Functions to copy oprs structures.
 *
 * Copyright (c) 1991-2005 Francois Felix Ingrand.
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

#include "slistPack.h"
#include "slistPack_f.h"
#include "oprs-error.h"

#include "oprs-type_f.h"
#include "unification_f.h"
#include "lisp-list_f.h"
#include "fact-goal_f.h"
#include "op-instance_f.h"
#include "intention_f.h"
#include "oprs-error_f.h"

List_Envar copy_var_uniquely = NULL;

/* copy function... */

Expression *copy_expr(Expression *expr)
{
     Expression *t = MAKE_OBJECT(Expression);

     t->pfr = expr->pfr;
     t->terms = copy_terms(expr->terms);

     return t;
}

ExprList copy_expr_list(ExprList exprs)
{
     return sl_copy_slist_func(exprs, copy_expr);
}

TermList copy_terms(TermList terms)
{
     return sl_copy_slist_func(terms, copy_term);
}

Term *copy_term(Term *term)
{
     if (term == NULL)
	  return NULL;			  /* Sometimes we have a null term
					   * pointed by an unbound variable */
     else {
	  Term *res = MAKE_OBJECT(Term);

	  *res = *term;

	  switch (term->type) {
	  case INTEGER:
	       break;
	  case LONG_LONG:
	       break;
	  case FLOAT_ARRAY:
	       res->u.float_array = copy_float_array(term->u.float_array);
	       break;
	  case INT_ARRAY:
	       res->u.int_array = copy_int_array(term->u.int_array);
	       break;
	  case U_POINTER:
	       break;
	  case U_MEMORY:
	       DUP(term->u.u_memory);
	       break;
	  case TT_FLOAT:
	       res->u.doubleptr = make_doubleptr(*term->u.doubleptr);
	       break;
	  case LISP_LIST:
	       res->u.l_list = copy_l_list(term->u.l_list);
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
	       NEWSTR(term->u.string, res->u.string);	/* These are not
							 * hashed... You have to
							 * make a real copy. */
	       break;
	  case TT_ATOM:
	       break;
	  case VARIABLE:
	       res->u.var = copy_envar(term->u.var);
	       break;
/* 	  case LENV: */
/* 	       res->u.var_list = copy_lenv(term->u.var_list); */
/* 	       break; */
	  case EXPRESSION:
	       res->u.expr = copy_expr(term->u.expr);
	       break;
	  default:
	       oprs_perror("copy_term",PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
	  return res;
     }
}

List_Envar copy_lenv(List_Envar le)
{
     return sl_copy_slist_func(le, copy_envar);
}

Envar *copy_envar(Envar *ev)
{
     Envar *res;

     if (copy_var_uniquely && (res = (Envar *)sl_search_slist(copy_var_uniquely, ev->name, match_var)))
	       return dup_envar(res);
     else {
	  res = MAKE_OBJECT(Envar);
	  
	  *res = *ev;
	  res->value = copy_term(ev->value);
	  
	  if (copy_var_uniquely) sl_add_to_head(copy_var_uniquely, res);

	  return res;
     }
}

Undo *copy_undo(Undo *und)
{
     Undo *u = MAKE_OBJECT(Undo);

     *u = *und;
     u->value = dup_term(und->value);

     return u;
}

List_Undo sl_copy_slist_undo(List_Undo lu)
{
     if (lu)
	  return  sl_copy_slist_func(lu, copy_undo);
     else
	  return NULL;
}

Frame *copy_frame_up_to_replaced_frame (Frame *fr1, Frame *prev, Frame *new_prev)
{
     if (fr1 != prev) {
	  Frame *new_fp = MAKE_OBJECT(Frame);

	  *new_fp = *fr1;
	  new_fp->previous_frame = copy_frame_up_to_replaced_frame(fr1->previous_frame, prev, new_prev);
	  new_fp->list_undo = sl_copy_slist_undo(fr1->list_undo);
	  return new_fp;
     } else {
	   return new_prev;
      }
}
