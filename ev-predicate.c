static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * ev-predicate.c -- Handle evaluable predicates.
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
#include "ctype.h"

#include "shashPack.h"
#include "shashPack_f.h"

#include "constant.h"
#include "macro.h"

#include "oprs-type.h"
#include "ev-predicate.h"
#include "soak.h"
#include "op-instance.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-profiling.h"

#include "oprs-type_f.h"
#include "op-instance_f.h"
#include "ev-predicate_f.h"
#include "unification_f.h"
#include "lisp-list_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"
#include "oprs-profiling_f.h"
#include "type_f.h"

PBoolean boundp_ep(TermList tl)
/* True if the object is a variable (i.e. unbound). */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if (t->type == VARIABLE) return FALSE; /* If we get a variable here... it means it was not bound. */
     else return TRUE;
}

PBoolean numberp_ep(TermList tl)
/* True if the object is a number. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if (t && (t->type == INTEGER || t->type == LONG_LONG || t->type == TT_FLOAT)) return TRUE;
     else return FALSE;
}

PBoolean stringp_ep(TermList tl)
/* True if the object is a string. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if (t && t->type == STRING) return TRUE;
     else return FALSE;
}

PBoolean atomp_ep(TermList tl)
/* True if the object is a string. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if (t && t->type == TT_ATOM) return TRUE;
     else return FALSE;
}

PBoolean consp_ep(TermList tl)
/* True if the object is a LISP_LIST. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if (t && t->type == LISP_LIST) return TRUE;
     else return FALSE;
}

PBoolean is_of_type_ep(TermList tl)
{
     Term *t1,*t2;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     t2 = (Term *)sl_get_slist_pos(tl, 2);
     if ((t1->type != ATOM) || (t2->type != ATOM))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));

     return is_of_type_sym(t1->u.id,t2->u.id);
}

PBoolean null_c_list_ep(TermList tl)
/* True if the object is the Symbol NIL. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if ((t->type == TT_ATOM)  && (t->u.id == nil_sym)) return TRUE;
     else return FALSE;
}

PBoolean null_l_list_ep(TermList tl)
/* True if the LISP_LIST is emty. */
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if ((t->type == LISP_LIST)  && (t->u.l_list == l_nil)) return TRUE;
     else return FALSE;
}

PBoolean null_l_car_ep(TermList tl)
/* True if the LISP_CAR is emty. */
{
     return ((Term *)sl_get_slist_head(tl) != NULL);
}

PBoolean equal_l_list_ep(TermList tl)
/* Equal Lisp List ... to be reconsidered... */
{
     Term *t1,*t2;

     t1 = (Term *)sl_get_slist_pos(tl,1);
     t2 = (Term *)sl_get_slist_pos(tl,2);

     return (equal_term(t1,t2));
}

PBoolean ep_gt (TermList terms)
/* Greater than > */
{
     Term *t1, *t2;

     t1 = (Term *)sl_get_slist_next(terms, NULL);
     t2 = (Term *)sl_get_slist_next(terms, t1);

     switch (t1->type) {
     case INTEGER:
	  switch (t2->type) {
	  case INTEGER:
	       return ((int)t1->u.intval) > ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((int)t1->u.intval) > ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     case TT_FLOAT:
	  switch (t2->type) {
	  case INTEGER:
	       return ((double)*t1->u.doubleptr) > ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((double)*t1->u.doubleptr) > ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     default:
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	  break;
     }
}

PBoolean ep_gte (TermList terms)
/* Greater than or equal >= */
{
     Term *t1, *t2;

     t1 = (Term *)sl_get_slist_next(terms, NULL);
     t2 = (Term *)sl_get_slist_next(terms, t1);

     switch (t1->type) {
     case INTEGER:
	  switch (t2->type) {
	  case INTEGER:
	       return ((int)t1->u.intval) >= ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((int)t1->u.intval) >= ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     case TT_FLOAT:
	  switch (t2->type) {
	  case INTEGER:
	       return ((double)*t1->u.doubleptr) >= ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((double)*t1->u.doubleptr) >= ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     default:
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	  break;
     }
}

PBoolean ep_lt (TermList terms)
/* Less than < */
{
     Term *t1, *t2;

     t1 = (Term *)sl_get_slist_next(terms, NULL);
     t2 = (Term *)sl_get_slist_next(terms, t1);

     switch (t1->type) {
     case INTEGER:
	  switch (t2->type) {
	  case INTEGER:
	       return ((int)t1->u.intval) < ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((int)t1->u.intval) < ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     case TT_FLOAT:
	  switch (t2->type) {
	  case INTEGER:
	       return ((double)*t1->u.doubleptr) < ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((double)*t1->u.doubleptr) < ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     default:
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	  break;
     }
}

PBoolean ep_lte (TermList terms)
/* Less than or equal <= */
{
     Term *t1, *t2;

     t1 = (Term *)sl_get_slist_next(terms, NULL);
     t2 = (Term *)sl_get_slist_next(terms, t1);

     switch (t1->type) {
     case INTEGER:
	  switch (t2->type) {
	  case INTEGER:
	       return ((int)t1->u.intval) <= ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((int)t1->u.intval) <= ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     case TT_FLOAT:
	  switch (t2->type) {
	  case INTEGER:
	       return ((double)*t1->u.doubleptr) <= ((int)t2->u.intval);
	       break;
	  case TT_FLOAT:
	       return ((double)*t1->u.doubleptr) <= ((double)*t2->u.doubleptr);
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     default:
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	  break;
     }
}

unsigned long get_mtime ();

PBoolean elapsed_mtime_ep(TermList tl)
{
     Term *t1, *t2;
     unsigned int mtime, i1, i2, i3;
     unsigned long ltime;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     t2 = (Term *)sl_get_slist_pos(tl, 2);
     if ((t1->type != INTEGER) || (t2->type != INTEGER))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));

     ltime = get_mtime();
     i1 = (unsigned int)(t1->u.intval);
     i2 = (unsigned int)(t2->u.intval);
     i3 = i1 + i2;
     mtime = (unsigned int)ltime;

     if ((i3 < i1) && (i3 < i2)) { /* We are wrapping */
	  /* This code is making very strong assumption regaring the values
	   of i1 and i2. We assume that i2 is far less than the wrap value. */
	  if ((mtime >= i3) && (mtime < i1))
	       return TRUE;
	  else
	       return FALSE;
     } else {
	  if (i3 <= mtime ) return TRUE;
	  else return FALSE;
     }
}

long get_time ();

PBoolean elapsed_time_ep(TermList tl)
{
     Term *t1, *t2;
     int time;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     t2 = (Term *)sl_get_slist_pos(tl, 2);
     if ((t1->type != INTEGER) || (t2->type != INTEGER))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));

     time = (int)get_time();

     return ((t1->u.intval + t2->u.intval) <= time );
}

PBoolean not_myself_ep(TermList terms)
/* Return t, if while parsing invocation part it points to the same OP. */
{
     Term *t1;
     Op_Instance *opi;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_OP_INSTANCE) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_OP_INSTANCE_TERM_TYPE));
     }

     opi = t1->u.opi;
     return (opi->op != soak_parsing_op);
}

PBoolean fact_invoked_op_ep(TermList terms)
/* Return T if it is a fact invoked OP. */
{
     Term *t1;
     Op_Instance *opi;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_OP_INSTANCE) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_OP_INSTANCE_TERM_TYPE));
     }

     opi = t1->u.opi;
     return ((opi->fact) != NULL);
}

PBoolean goal_invoked_op_ep(TermList terms)
/* Return T if it is a goal invoked OP. */
{
     Term *t1;
     Op_Instance *opi;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_OP_INSTANCE) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_OP_INSTANCE_TERM_TYPE));
     }

     opi = t1->u.opi;
     return ((opi->goal) != NULL);
}

PBoolean memq_ep(TermList terms)
/* Return a LISP_LIST object which is the CONS of first and element of the LISP_LIST. */
{
     Term *t1, *t2;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     if (t2->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     return memq(t1, t2->u.l_list);
}

PBoolean set_ep(Frame *fr, TermList tl)
{
     Term *t1,*t2, *free1, *free2;
     PBoolean res;

     t1 = (Term *)sl_get_slist_pos(tl,1);
     t2 = (Term *)sl_get_slist_pos(tl,2);
     
     free1 = find_binding_ef_or_prog_var(t1);
     free2 = find_binding_ef(t2);
     res = unify_term(free1, free2, fr);
     
     free_term(free1);
     free_term(free2);

     return res;
}

PBoolean unify_ep(Frame *fr, TermList tl)
{
     Term *t1,*t2, *free1, *free2;
     PBoolean res;

     t1 = (Term *)sl_get_slist_pos(tl,1);
     t2 = (Term *)sl_get_slist_pos(tl,2);
     
     free1 = find_binding_ef_or_prog_var(t1);
     free2 = find_binding_ef_or_prog_var(t2);
     res = unify_term(free1, free2, fr);

     free_term(free1);
     free_term(free2);

     return res;
}

PBoolean evaluate_expression_predicate(Eval_Pred *ep, char *ep_name, TermList tl)
/* This is the main function when one wants to evaluate an evaluable predicate.*/
{
     PBoolean res;
#ifdef OPRS_PROFILING     
     PDate tp;
#endif

     current_error_type= EVAL_PREDICATE;
     current_eval_pred = ep_name;

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EP]) 
	  GET_PROFILE_TIMER(&tp);
#endif

     res =  (PBoolean)(* (ep->predicat))(tl);

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EP]) {
	  ADD_PROFILE_TIMER(&tp,ep->time_active);
	  ep->nb_call++;
     }
#endif

     current_error_type = ET_NONE;

     return res;
}

PBoolean evaluate_expression_predicate_frame(Frame *fr, Eval_Pred *ep, char *ep_name, TermList tl)
/* This is the main function when one wants to evaluate an evaluable predicate (! eval_var).*/
{
     PBoolean res;
#ifdef OPRS_PROFILING     
     PDate tp;
#endif

     current_error_type= EVAL_PREDICATE;
     current_eval_pred = ep_name;

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EP]) 
	  GET_PROFILE_TIMER(&tp);
#endif

     res =  (PBoolean)(* (ep->predicat))(fr, tl);

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EP]) {
	  ADD_PROFILE_TIMER(&tp,ep->time_active);
	  ep->nb_call++;
     }
#endif

     current_error_type = ET_NONE;

     return res;
}

void declare_eval_pred(void)
{
     make_and_declare_eval_pred(">",ep_gt, 2,TRUE);
     make_and_declare_eval_pred(">=",ep_gte, 2, TRUE);
     make_and_declare_eval_pred("<",ep_lt, 2, TRUE);
     make_and_declare_eval_pred("<=",ep_lte, 2, TRUE);
     
     make_and_declare_eval_pred("NULL_C",null_c_list_ep, 1, TRUE);
     make_and_declare_eval_pred("NOT-AN-INSTANCE-OF-ME",not_myself_ep, 1, TRUE);
     make_and_declare_eval_pred("IS-FACT-INVOKED",fact_invoked_op_ep, 1, TRUE);
     make_and_declare_eval_pred("IS-GOAL-INVOKED",goal_invoked_op_ep, 1, TRUE);

     make_and_declare_eval_pred("BOUNDP",boundp_ep, 1, TRUE);

     make_and_declare_eval_pred("NULL", null_l_list_ep, 1, TRUE);
     make_and_declare_eval_pred("NULL_CAR", null_l_car_ep, 1, TRUE);
     make_and_declare_eval_pred("MEMQ", memq_ep, 2, TRUE);
     make_and_declare_eval_pred("EQUAL",equal_l_list_ep, 2, TRUE);
     make_and_declare_eval_pred("PROPERTY-P",property_p_ep, 2, TRUE);
     make_and_declare_eval_pred("ELAPSED-TIME",elapsed_time_ep, 2, TRUE);
     make_and_declare_eval_pred("ELAPSED-MTIME",elapsed_mtime_ep, 2, TRUE);

     make_and_declare_eval_pred("NUMBERP",numberp_ep, 1, TRUE);
     make_and_declare_eval_pred("CONSP",consp_ep, 1, TRUE);
     make_and_declare_eval_pred("STRINGP",stringp_ep, 1, TRUE);
     make_and_declare_eval_pred("ATOMP",atomp_ep, 1, TRUE);

     make_and_declare_eval_pred("IS-OF-TYPE",is_of_type_ep, 2, TRUE);

     make_and_declare_eval_pred_no_eval_var("=", set_ep, 2, TRUE);
     make_and_declare_eval_pred_no_eval_var("==", unify_ep, 2, TRUE);

     declare_user_eval_pred();

     return;
}
