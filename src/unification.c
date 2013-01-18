
/*                               -*- Mode: C -*-
 * unification.c -- Unification pour OPRS
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


#include "constant.h"
#include "oprs-type.h"
#include "unification.h"
#include "oprs-error.h"

#include "lisp-list_f.h"
#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-array_f.h"
#include "oprs-pred-func_f.h"
#include "type_f.h"
#include "oprs-error_f.h"

#include "unification_f.h"

PBoolean unify_expr(Expression *t1, Expression *t2, FramePtr frame);
PBoolean unify_expr_from_db(Expression *t1, Expression *t2,
				   FramePtr frame, PBoolean dup_var);
PBoolean unify_l_list(L_List l1, L_List l2, FramePtr frame);

FramePtr bind_var(Envar *ev, Term *t, FramePtr fp)
{
     Undo *u = make_undo(ev);

     u->value = dup_term(t);		/* This adds one to t reference count. */
     ev->value = dup_term(t);		/* This adds one to t reference count. */
     SAFE_SL_ADD_TO_TAIL(fp->list_undo, u);
     return fp;
}

PBoolean compatible_type(Envar *ev, Term *t)
{
     if (ev->unif_type) {
	  while (t->type == VARIABLE) {
	       if (t->u.var->unif_type)
		    return (t->u.var->unif_type == ev->unif_type);
	       if (t->u.var->value)
		    t = t->u.var->value;
	       else
		    return TRUE; /* No value, no unif_type */
	  } /* T is not a variable */
	  if ((t->type == TT_ATOM))
	       return is_of_type(t->u.id,ev->unif_type);
     }
     return TRUE;
}

Undo *unbind_prog_var(Envar *ev)
{
     Undo *u = make_undo(ev);

     u->value = ev->value;	/* No need to dup it as we are going to unbind it right now... */
     ev->value = (Term *)NULL;	/* No need to make a dup followed by a free. */
     
     return u;	  
}

void unbind_var(Envar *ev)
{
     free_term(ev->value);
     ev->value = (Term *) NULL;
}

void unbind_lenv(List_Envar le)
{
     Envar *ev;
     sl_loop_through_slist(le, ev, Envar *)
	  unbind_var(ev);
}

void rebind_var(Undo * un)
{
     if (un->envar->value) {
	  free_term(un->envar->value);
     }
     un->envar->value = dup_term(un->value);
}

FramePtr install_frame(FramePtr fp)
{
     Undo *undo;

     if (!(fp->installe)) {
	  SAFE_SL_LOOP_THROUGH_SLIST(fp->list_undo, undo, Undo *)
	       rebind_var(undo);

	  fp->installe = TRUE;
     }
     return fp;
}

FramePtr uninstall_frame(FramePtr fp)
{
     Undo *undo;

     if (fp->installe) {
	  SAFE_SL_LOOP_THROUGH_SLIST(fp->list_undo, undo, Undo *)
	       unbind_var(undo->envar);
	       
	  fp->installe = FALSE;
     }
     return fp;
}

void desinstall_frame(FramePtr fp1, FramePtr fp2)
{
     while (fp1 != empty_env && fp1 != fp2) {
	  uninstall_frame(fp1);
	  fp1 = fp1->previous_frame;;
     }
}

FramePtr reinstall_frame(FramePtr fp1)
{
     while ((fp1 != empty_env) && !(fp1->installe)) {
	  install_frame(fp1);
	  fp1 = fp1->previous_frame;
     }
     return fp1;
}

PBoolean match_var_name_undo(Symbol name, Undo *u)
{
     return (name == u->envar->name);
}

Term *find_binding_or_prog_var(Term *t)
/* find_binding - return a pointer on the binding of the term or its prog var (no copy here) */
{
     if (t->type == VARIABLE) /* It is a variable */
	  if (t->u.var->type != PROGRAM_VARIABLE) {
	       if (t->u.var->value != (Term *) NULL)
		    return (find_binding_or_prog_var(t->u.var->value));
	       else
		    return t;
	  } else	
	       if (locked_local_var) /* locked */
		    if (! sl_search_slist(locked_local_var,t->u.var->name,match_var_name_undo)) { /* Newly locked */
			 sl_add_to_tail(locked_local_var, unbind_prog_var(t->u.var));
			 return t;
		    } else	/*  old locked variable */
			 if (t->u.var->value != (Term *) NULL) /* point to something real.. */
			      return (find_binding_or_prog_var(t->u.var->value));
			 else	/* Otherwise... */
			      return t;
	       else		/* program not locked */
		    return t;
     else
	  return t;
}

PBoolean unify_expr(Expression *e1, Expression *e2, FramePtr frame)
{
     void *ptr1, *ptr2;

     if ((e1->pfr != e2->pfr) || ((sl_slist_length(e1->terms) != sl_slist_length(e2->terms))))
	  return (FALSE);

     ptr1 = (void *)sl_get_slist_next(e1->terms, NULL);
     ptr2 = (void *)sl_get_slist_next(e2->terms, NULL);
     while (ptr1 != NULL && ptr2 != NULL)
	  if (unify_term(find_binding_or_prog_var((Term *) ptr1),	/* on unifie toujours une variable programme */
			 find_binding_or_prog_var((Term *) ptr2), frame)) {
	       ptr1 = (void *)sl_get_slist_next(e1->terms, ptr1);
	       ptr2 = (void *)sl_get_slist_next(e2->terms, ptr2);
	  } else {
	       uninstall_frame(frame);	/* parfois, il y a eu des variable binde... fonc il faut les defaire */
	       /* free_frame(frame); c'est pas a lui de free un object qu'il n'a pas cree. */
	       return (FALSE);
	  }
     return (TRUE);
}

PBoolean unify_expr_from_db(Expression *e1, Expression *e2, FramePtr frame, PBoolean dup_var)
{
/* with the variable in the DB we may now get a literal variable in e2... */
     void *ptr1, *ptr2;

     ptr1 = (void *)sl_get_slist_next(e1->terms, NULL);
     ptr2 = (void *)sl_get_slist_next(e2->terms, NULL);
     while (ptr1 != NULL)
	  if (unify_term_from_db(find_binding_or_prog_var((Term *) ptr1), /* on unifie toujours une var. program */
				 (Term *) ptr2, frame, dup_var)) { /* The second one comes from the DB */
	       ptr1 = (void *)sl_get_slist_next(e1->terms, ptr1);
	       ptr2 = (void *)sl_get_slist_next(e2->terms, ptr2);
	  } else {
	       uninstall_frame(frame);	/* parfois, il y a eu des variable binde... fonc il faut les defaire */
	       return (FALSE);
	  }
     return (TRUE);
}

Term *find_binding_envar(Envar *envar)
{
     if (envar->value)
	  return (find_binding(envar->value));
     else
	  return NULL;
}

Term *find_binding(Term *t)
/* find_binding - return a pointer on the binding of the term (no copy here) */
{
     if ((t->type == VARIABLE) && (t->u.var->value != (Term *) NULL))
	  return (find_binding(t->u.var->value));
     else
	  return t;
}

Term *find_binding_ef_or_prog_var(Term *t)
/* find_binding_ef_or_prog_var - return a NEW term (either a new one or a DUP of the old one). */
{
     Eval_Funct *ef;

     if (t->type == EXPRESSION) {
	  /* if t is a expr */
	  TermList tl;
	  Term *res;

	  if ((ef = PFR_EF(t->u.expr->pfr))) {
	       tl = find_binding_ef_tl(t->u.expr->terms);
	       res = evaluate_term_function(ef, t->u.expr->pfr->name, tl);
	       free_tl(tl);
	  } else {		/* Non evaluable term comp */
	       Expression *tc;

	       tl = find_binding_ef_or_prog_var_tl(t->u.expr->terms);
	       res = MAKE_OBJECT(Term);
	       res->type = EXPRESSION;
	       tc = MAKE_OBJECT(Expression);
	       res->u.expr = tc;
	       *tc = *(t->u.expr);
	       tc->terms = tl;
	  }
	  return res;
     } else if (t->type == VARIABLE)
	  if (t->u.var->type != PROGRAM_VARIABLE) {
	       if (t->u.var->value != (Term *) NULL)
		    return (find_binding_ef_or_prog_var(t->u.var->value));
	       else
		    return dup_term(t);
	  } else	
	       if (locked_local_var) /* locked */
		    if (! sl_search_slist(locked_local_var,t->u.var->name,match_var_name_undo)) { /* Newly locked */
			 sl_add_to_tail(locked_local_var, unbind_prog_var(t->u.var));
			 return dup_term(t);
		    } else	/*  old locked variable */
			 if (t->u.var->value != (Term *) NULL) /* point to something real.. */
			      return (find_binding_ef_or_prog_var(t->u.var->value));
			 else	/* Otherwise... */
			      return dup_term(t);
	       else		/* program not locked */
		    return dup_term(t);
     else
	  return dup_term(t);
}

TermList find_binding_ef_or_prog_var_tl(TermList tl)
/*
 * find_binding_ef_or_prog_var_tl - return a new termlist with each term replaced with a
 *                                  the result of its call to find_binding_ef_or_prog_var.
 *
 */
{
     return sl_copy_slist_func(tl,find_binding_ef_or_prog_var);
}

Term *find_binding_ef(Term *t)
/* find_binding_ef - return a NEW term (either a new one or a DUP of the old one). */
{
     Eval_Funct *ef;
     TermList tl;
     Term *res;
     Expression *tc;

     if (t->type == EXPRESSION) {
	  tl = find_binding_ef_tl(t->u.expr->terms);
	  if ((ef = PFR_EF(t->u.expr->pfr))) {
	       res = evaluate_term_function(ef, t->u.expr->pfr->name, tl); /* This is a new term. */
	       free_tl(tl);
	  } else {
	       res = MAKE_OBJECT(Term);
	       res->type = EXPRESSION;
	       tc = MAKE_OBJECT(Expression);
	       res->u.expr = tc;
	       *tc = *(t->u.expr);
	       tc->terms = tl;
	  }
	  return res; 
     } else if ((t->type == VARIABLE) && (t->u.var->value != (Term *) NULL))
	  return (find_binding_ef(t->u.var->value));
     else
	  return dup_term(t);
}

Expression *find_binding_expr(Expression *expr)
{
     return build_expr_pfr_terms(expr->pfr, find_binding_ef_tl(EXPR_TERMS(expr)));
}

TermList find_binding_ef_tl(TermList tl)
{
     return sl_copy_slist_func(tl,find_binding_ef);
}

PBoolean unify_term(Term *t1, Term *t2, FramePtr frame)
{
     if (t1 == t2) return TRUE;
     if (t1->type != t2->type) { /* C'est mal barre Roger... */
	  if (t1->type == VARIABLE && compatible_type(t1->u.var,t2)) {
	       if ((t1->u.var->type == PROGRAM_VARIABLE) && (! locked_local_var)) { 
		    unbind_var(t1->u.var);
	       }
	       bind_var(t1->u.var, t2, frame);
	       return (TRUE);
	  } else if (t2->type == VARIABLE  && compatible_type(t2->u.var,t1)) {
	       if (t2->u.var->type == PROGRAM_VARIABLE) { 
		    unbind_var(t2->u.var);
	       }
	       bind_var(t2->u.var, t1, frame);
	       return (TRUE);
	  } else
	       return FALSE;
     } else {
	  switch (t1->type) {	/* Beware... some int and long long could be equal... I need to check that. FFI */
	  case INTEGER:
	       return ((t1->u.intval) == (t2->u.intval));
	       break;
	  case LONG_LONG:
	       return ((t1->u.llintval) == (t2->u.llintval));
	       break;
	  case U_MEMORY:
	  case U_POINTER:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case TT_FLOAT:
	       return ((*t1->u.doubleptr) == (*t2->u.doubleptr));
	       break;
	  case FLOAT_ARRAY:
	       return (equal_float_array(t1->u.float_array,t2->u.float_array));
	       break;
	  case INT_ARRAY:
	       return (equal_int_array(t1->u.int_array,t2->u.int_array));
	       break;
	  case LISP_LIST:
	       return (unify_l_list(t1->u.l_list,t2->u.l_list,frame));
	       break;
	  case TT_FACT:
	  case TT_GOAL:
	  case TT_OP_INSTANCE:
	  case TT_INTENTION:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case C_LIST:
	       return ((t1->u.c_list) == (t2->u.c_list));
	       break;
	  case STRING:
	       return (strcmp(t1->u.string, t2->u.string) == 0);
	       break;
	  case TT_ATOM:
	       return (t1->u.id == t2->u.id);
	       break;
	  case VARIABLE:
	       /*
		*	       if ((t1->u.var->type == PROGRAM_VARIABLE)) {
		*	       bind_var_and_change_var_type(t1->u.var, t2, frame); return(TRUE);
		*	       } else 
		*/
	       if (compatible_type(t1->u.var,t2)) {
		    if (t1->u.var->type == PROGRAM_VARIABLE) { 
			 unbind_var(t1->u.var);
		    }
		    bind_var(t1->u.var, t2, frame);
		    return (TRUE);
	       } else return FALSE;
		
	       break;
	  case EXPRESSION:
	       return (unify_expr(t1->u.expr, t2->u.expr, frame));
	       break;
	  default:
	       oprs_perror("unify_term", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
	  return (FALSE);	/* on devrait pas arriver ici */
     }
}

PBoolean unify_l_list(L_List l1, L_List l2, FramePtr frame)
{
     if (L_LENGTH(l1) != L_LENGTH(l2))
	  return (FALSE);

     if (l1 == l_nil) return TRUE; /* They are the same length.. therefore both nil. */

     while (l1 != l_nil) {
	  if (unify_term(find_binding_or_prog_var(CAR(l1)),
			 find_binding_or_prog_var(CAR(l2)), frame)) {
	       l1 = CDR(l1);
	       l2 = CDR(l2);
	  } else 
	       return FALSE;
     }
     return TRUE;	  
}


PBoolean unify_term_from_db(Term *t1, Term *t2, FramePtr frame, PBoolean dup_var)
{
     if (t1 == t2) return TRUE;
     if (t1->type != t2->type) { 
	  if (t1->type == VARIABLE && compatible_type(t1->u.var,t2)) {
	       if ((t1->u.var->type == PROGRAM_VARIABLE) && (! locked_local_var)) { 
		    unbind_var(t1->u.var);
	       }
	       bind_var(t1->u.var, t2, frame);
	       return (TRUE);
	  } else		/* t2 cannot be a variable with variable semantique. */
	       return FALSE;	/* and they are not the same type */
     } else if (dup_var) {	/* we have duplicate var in this expr... too bad... */
	  switch (t1->type) {	/* These may now result from a find_binding... */
	  case INTEGER:
	       return ((t1->u.intval) == (t2->u.intval));
	       break;
	  case LONG_LONG:
	       return ((t1->u.llintval) == (t2->u.llintval));
	       break;
	  case TT_FLOAT:
	       return ((*t1->u.doubleptr) == (*t2->u.doubleptr));
	       break;
 	  case FLOAT_ARRAY:
	       return (equal_float_array(t1->u.float_array,t2->u.float_array));
	       break;
	  case INT_ARRAY:
	       return (equal_int_array(t1->u.int_array,t2->u.int_array));
	       break;
	  case LISP_LIST:
	       return (unify_l_list(t1->u.l_list, t2->u.l_list, frame));
	       break;
	  case TT_FACT:
	  case TT_GOAL:
	  case TT_OP_INSTANCE:
	  case TT_INTENTION:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case C_LIST:
	       return ((t1->u.c_list) == (t2->u.c_list));
	       break;
	  case STRING:
	       return (strcmp(t1->u.string, t2->u.string) == 0);
	       break;
	  case TT_ATOM:
	       return (t1->u.id == t2->u.id);
	       break;
	  case EXPRESSION:
	       return (unify_expr(t1->u.expr, t2->u.expr, frame));
	       break;
	  case VARIABLE:
	  case U_MEMORY:
	  case U_POINTER:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  default:
	       oprs_perror("unify_term_from_db", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
	  return (FALSE);	/* on devrait pas arriver ici */
     } else {
	  if (t1->type == EXPRESSION)
	       return (unify_expr_from_db(t1->u.expr, t2->u.expr, frame, dup_var));
	  else
	       return (TRUE);	/* No duplicate_var, and same type... they ought to be the same. */
     }
}

void test_unify_expr(Expression *e1, Expression *e2)
{
     FramePtr fp;

     printf(LG_STR("The expression ",
		   "L'expression "));
     print_expr(e1);
     printf(LG_STR(" and the expression ",
		   " et l'expression "));
     print_expr(e2);
     if (!(unify_expr(e1, e2, fp = make_frame(empty_env))))
	  printf(LG_STR(" do not unify.\n",
			" ne s'unifie pas.\n"));
     else {
	  printf(LG_STR(" unify.\nWith the following frame:\n",
			" s'unifie.\nAvec le frame:\n"));
	  print_frame(fp);
	  printf("\n");
     }
     free_frame(fp);
}

PBoolean literal_var = FALSE;

PBoolean equal_expr_literal_var(Expression *e1, Expression *e2)
{
     PBoolean res;
     PBoolean previous_value = literal_var;
     
     literal_var = TRUE;
     res = equal_expr(e1,e2);
     literal_var = previous_value;

     return res;
}

PBoolean equal_l_list_literal_var(L_List l1, L_List l2)
{
     PBoolean res;
     PBoolean previous_value = literal_var;
     
     literal_var = TRUE;
     res = equal_l_list(l1,l2);
     literal_var = previous_value;

     return res;
}

PBoolean equal_expr(Expression *t1, Expression *t2);

PBoolean equal_term(Term *t1, Term *t2)
{
     if (t1 == t2) return TRUE;	/* Just in case... */

     if (t1->type != t2->type) {/* C'est mal barre roge... */
	  return (FALSE);		  /* niet */
     } else {			
	  switch (t1->type) {
	  case INTEGER:
	       return ((t1->u.intval) == (t2->u.intval));
	       break;
	  case LONG_LONG:
	       return ((t1->u.llintval) == (t2->u.llintval));
	       break;
	  case U_MEMORY:
	  case U_POINTER:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case TT_FLOAT:
	       return ((*t1->u.doubleptr) == (*t2->u.doubleptr));
	       break;
	  case FLOAT_ARRAY:
	       return (equal_float_array(t1->u.float_array,t2->u.float_array));
	       break;
	  case INT_ARRAY:
	       return (equal_int_array(t1->u.int_array,t2->u.int_array));
	       break;
	  case LISP_LIST:
	       return (equal_l_list(t1->u.l_list,t2->u.l_list));
	       /* return ((t1->u.l_list) == (t2->u.l_list)); */
	       break;
	  case TT_FACT:
	  case TT_GOAL:
	  case TT_OP_INSTANCE:
	  case TT_INTENTION:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case C_LIST:
	       return ((t1->u.c_list) == (t2->u.c_list));
	       break;
	  case STRING:
	       return (strcmp(t1->u.string, t2->u.string) == 0);
	       break;
	  case TT_ATOM:
	       return (t1->u.id == t2->u.id);
	       break;
	  case VARIABLE:
	       if (literal_var) return TRUE; /* Lieral var are always alike. */
	       return (t1->u.var == t2->u.var); /* We need this... for frame comparisons... */
	       break;
	  case EXPRESSION:
	       return (equal_expr(t1->u.expr, t2->u.expr));
	       break;
	  default:
	       oprs_perror("equal_term", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
	  return (FALSE);	/* niet */
     }
}

PBoolean equal_expr(Expression *e1, Expression *e2)
{
     void *ptr1, *ptr2;

     if (e1 == e2) return TRUE;	/* Just in case... */

     if ((e1->pfr != e2->pfr) ||
	 (sl_slist_length(e1->terms) != sl_slist_length(e2->terms)))
	  return (FALSE);
     ptr1 = (void *)sl_get_slist_next(e1->terms, NULL);
     ptr2 = (void *)sl_get_slist_next(e2->terms, NULL);
     while (ptr1 != NULL && ptr2 != NULL)
	  if (equal_term((Term *) ptr1, (Term *) ptr2) == TRUE) {
	       ptr1 = (void *)sl_get_slist_next(e1->terms, ptr1);
	       ptr2 = (void *)sl_get_slist_next(e2->terms, ptr2);
	  } else
	       return (FALSE);
     return (TRUE);
}

PBoolean equal_expr_for_given_depth(Expression *e1, Expression *e2, int depth)
/* enzo:
 * This is quite similar to 'equal_expr', but we simply compare the 'depth' first terms.
 */
{
     void *ptr1, *ptr2;

     if (e1 == e2) return TRUE;	/* Just in case... */

     if ((e1->pfr != e2->pfr) || (sl_slist_length(e1->terms) != sl_slist_length(e2->terms)))
	  return (FALSE);
     ptr1 = (void *)sl_get_slist_next(e1->terms, NULL);
     ptr2 = (void *)sl_get_slist_next(e2->terms, NULL);
     while (depth && ptr1 != NULL && ptr2 != NULL)
	  if (equal_term((Term *) ptr1, (Term *) ptr2) == TRUE) {
	       ptr1 = (void *)sl_get_slist_next(e1->terms, ptr1);
	       ptr2 = (void *)sl_get_slist_next(e2->terms, ptr2);
	       depth--;
	  } else
	       return (FALSE);
     return (TRUE);
}

PBoolean opposite_expr(Expression *e1, Expression *e2)
/*
 * enzo:
 * Same as 'equal_expr', but with opposite signs for e2 and e1
 */
{
     Expression *ne1;
     PBoolean res;

     ne1 = not_expr_dup(e1);
     res = equal_expr(ne1, e2);
     free_expr(ne1);
     return res;
}

PBoolean equal_expr_wo_sign(Expression *e1, Expression *e2)
/* enzo:
 * This is similar to equal_expr, but we do not check the sign compability.
 */
{
     void *ptr1, *ptr2;

     if (e1 == e2) return TRUE;	/* Just in case... */

     if (sl_slist_length(e1->terms) != sl_slist_length(e2->terms))
	  return (FALSE);
     if (e1->pfr == e2->pfr) {
	  ptr1 = (void *)sl_get_slist_next(e1->terms, NULL);
	  ptr2 = (void *)sl_get_slist_next(e2->terms, NULL);
	  while (ptr1 != NULL && ptr2 != NULL)
	       if (equal_term((Term *) ptr1, (Term *) ptr2) == TRUE) {
		    ptr1 = (void *)sl_get_slist_next(e1->terms, ptr1);
		    ptr2 = (void *)sl_get_slist_next(e2->terms, ptr2);
	       } else
		    return (FALSE);
	  return (TRUE);
     } else
	  return (FALSE);
}


PBoolean matching_lenv_between_frames(List_Envar le1, Frame *fr1,List_Envar le2,  Frame *fr2)
{

     if (sl_slist_empty(le1) && sl_slist_empty(le2))
	  return TRUE;
     else {
	  Frame *prev_inst_frame;
	  Envar *ev;
	  TermList tl1 , tl2;
	  Term *t1, *t2;
	  Slist *unbound1, *unbound2;
	  void *pos1 = 0;
	  void *pos2 = 0;
	  PBoolean match = TRUE;

	  /* find binding for first frame */
	  prev_inst_frame = reinstall_frame(fr1);
	  tl1 = sl_make_slist();
	  unbound1 = sl_make_slist();
	  sl_loop_through_slist(le1, ev, Envar *) {
	       pos1++;
	       if (ev->value != (Term *) NULL) {
		    t1 = find_binding(ev->value);
		    if (t1->type == VARIABLE) /* The variable is bound to an unbound variable */
			 sl_add_to_tail(unbound1, pos1);
		    else
			 sl_add_to_tail(tl1, t1);
	       } else
		    sl_add_to_tail(unbound1, pos1);
	  }
	  desinstall_frame(fr1, prev_inst_frame);

	  /* same thing for the other one */
	  prev_inst_frame = reinstall_frame(fr2);
	  tl2 = sl_make_slist();
	  unbound2 = sl_make_slist();
	  sl_loop_through_slist(le2, ev, Envar *) {
	       pos2++;
	       if (ev->value != (Term *) NULL) {
		    t2 = find_binding(ev->value);
		    if (t2->type == VARIABLE) /* The variable is bound to an unbound variable */
			 sl_add_to_tail(unbound2, pos2);
		    else
			 sl_add_to_tail(tl2, t2);
	       } else
		    sl_add_to_tail(unbound2, pos2);
	  }
	  desinstall_frame(fr2, prev_inst_frame);

	  pos1 =  (void *)sl_get_from_head(unbound1);
	  pos2 =  (void *)sl_get_from_head(unbound2);
	  while ((pos1 != 0) || (pos2 != 0)) {
	       if (pos1 !=  pos2) {
		    match = FALSE;
		    break;
	       }
	       pos1 =  (void *)sl_get_from_head(unbound1);
	       pos2 =  (void *)sl_get_from_head(unbound2);
	  }
	  FREE_SLIST(unbound1);
	  FREE_SLIST(unbound2);

	  if (match) {
	       t1 = (Term *) sl_get_from_head(tl1);
	       t2 = (Term *) sl_get_from_head(tl2);
	       
	       while ((t1 != NULL) && (t2 != NULL)) {
		    if (equal_term(t1, t2) != 0) {
			 t1 = (Term *) sl_get_from_head(tl1);
			 t2 = (Term *) sl_get_from_head(tl2);
		    } else {
			 match = FALSE;
			 break;
		    }
	       }
	       if (match && ((t1 != NULL) || (t2 != NULL))) { /* ome list is longer than the other */
		    match = FALSE;
	       }
	  }
	  FREE_SLIST(tl1);
	  FREE_SLIST(tl2);
	  return match;
     }
}

