static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-free.c -- Most of the free functions
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

#include "config.h"

#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "oprs-error.h"

#include "oprs-type_f.h"
#include "unification_f.h"
#include "lisp-list_f.h"
#include "oprs-array_f.h"
#include "fact-goal_f.h"
#include "op-instance_f.h"
#include "intention_f.h"
#include "oprs-error_f.h"

/* Free functions... */

void free_property(Property *p)
{
     if (LAST_REF(p))
	  free_term(p->value);
     FREE(p);
}

void free_list_property(PropertyList pl)
{
     Property *p;
     
     if (LAST_REF_SLIST(pl)) 
	  sl_loop_through_slist(pl, p, Property *)
	       free_property(p);

     FREE_SLIST(pl);
}

void free_envar(Envar *ev)
{
     /* The name is now an ID. */
     if (LAST_REF(ev)) free_term(ev->value);
     FREE(ev);
}

void free_lenv(List_Envar le)
{
     Envar *ptr1;

     if (le != empty_list) {
	  if (LAST_REF_SLIST(le)) 
	       sl_loop_through_slist(le, ptr1, Envar *)
		    free_envar(ptr1);

	  FREE_SLIST(le);
     }
}

void free_undo(Undo *ud)
{
     if (LAST_REF(ud))
	  free_term(ud->value);
     FREE(ud);
}

void oprs_free_undo_list(List_Undo lu)
{
     Undo *u;

     if (!lu) return;

     if (LAST_REF_SLIST(lu)) 
	  sl_loop_through_slist(lu, u, Undo *)
	       free_undo(u);

     FREE_SLIST(lu);
}

void free_frame(FramePtr fp)
{
     if (fp != empty_env) {
	  if (LAST_REF(fp)) {
	       /* Last ref baby, time to free. */
	       oprs_free_undo_list(fp->list_undo);
	       free_frame(fp->previous_frame);
	  }
	  FREE(fp);
     }
}

void free_tl(TermList tl)
{
     Term *t;

     if (LAST_REF_SLIST(tl)) 
	  sl_loop_through_slist(tl, t, Term *)
	       free_term(t);
     FREE_SLIST(tl);
}

void free_expr_list(ExprList tl)
{
     Expression *expr;

     if (LAST_REF_SLIST(tl)) 
	  sl_loop_through_slist(tl, expr, Expression *)
	       free_expr(expr);
     FREE_SLIST(tl);
}

void free_expr(Expression *expr)
{
     if (LAST_REF(expr)) {
	  free_tl(expr->terms);
     }
     FREE(expr);
}

void free_action_field(Action_Field *ac)
{
     if (LAST_REF(ac)) {
	  if (ac->special) {
	       if (ac->multi) free_lenv(ac->u.list_envar);
	       else free_envar(ac->u.envar);
	  }
	  
	  free_expr(ac->function);
     }
     FREE(ac);
}

void free_memory(U_Memory *um)
{
     if (LAST_REF(um)) FREE(um->mem);
     FREE(um);
}

void free_term(Term *term)
{
     if (term == (Term *) NULL)
	  return;

     if (LAST_REF(term)) {

	  switch (term->type) {

	  case INTEGER:
	  case U_POINTER:
	  case TT_ATOM:
	       break;
	  case TT_FLOAT:
	       free_doubleptr(term->u.doubleptr);
	       break;
	  case STRING:
	       FREE(term->u.string);
	       break;
	  case VARIABLE:
	       free_envar(term->u.var); /* do not touch this... */
	       break;
	  case EXPRESSION:
	       free_expr(term->u.expr);
	       break;
/* 	  case LENV: */
/* 	       free_lenv(term->u.var_list); */
/* 	       break; */
	  case FLOAT_ARRAY:
	       free_float_array(term->u.float_array);
	       break;
	  case INT_ARRAY:
	       free_int_array(term->u.int_array);
	       break;
	  case LISP_LIST:
	       free_l_list(term->u.l_list);
	       break;
	  case TT_FACT:
	       free_fact(term->u.fact);
	       break;
	  case TT_GOAL:
	       free_goal(term->u.goal);
	       break;
	  case TT_OP_INSTANCE:
	       free_op_instance(term->u.opi);
	       break;
	  case TT_INTENTION:
	       free_intention(term->u.in);
	       break;
	  case U_MEMORY:
	       free_memory(term->u.u_memory);
	       break;
	  case C_LIST:
	       FREE_SLIST(term->u.c_list); /* We are not freeing the element, we do not know what they are... */
	       break;
	  default:
	       oprs_perror("free_term", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
     }
     FREE(term);
}

void free_expr_frame(ExprFrame *ef)
{
     if (LAST_REF(ef)) {
	  free_expr(ef->expr);
	  free_frame(ef->frame);
     }
     FREE(ef);
}
