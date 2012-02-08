static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * conditions.c -- Conditions management.
 * 
 * Copyright (c) 1991-2009 Francois Felix Ingrand.
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
#include <stdio.h>

/*	For timeval definition. */
#ifdef WIN95
#include <winsock.h>
#endif

#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "oprs-type.h"
#include "oprs-pred-func.h"
#include "conditions.h"
#include "fact-goal.h"
#include "intention.h"
#include "oprs.h"
#include "oprs-sprint.h"
#include "soak.h"
#include "oprs-error.h"

#include "oprs-pred-func_f.h"
#include "oprs-type_f.h"
#include "database_f.h"
#include "unification_f.h"
#include "conditions_f.h"
#include "oprs-sprint_f.h"
#include "int-graph_f.h"
#include "activate_f.h"
#include "intention_f.h"
#include "oprs-error_f.h"

Condition_List collect_cond_from_fact(Fact *fact)
{
     return (PFR_PR(fact->statement->pfr) ? PFR_PR(fact->statement->pfr)->conditions : empty_list);
}

void add_condition_to_expr(Expression *expr, Relevant_Condition *rc)
{
     Term *term;

     if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
 	  sl_loop_through_slist(expr->terms, term, Term *)
	       add_condition_to_expr(TERM_EXPR(term), rc);
     } else {
	  Pred_Rec *pr = expr->pfr->u.u.pr;

	  if (!pr)
	       pr = expr->pfr->u.u.pr = make_pred_rec(); 

	  if (! SAFE_SL_IN_SLIST(pr->conditions,rc))
	       SAFE_SL_ADD_TO_TAIL(pr->conditions,rc);
     }
}

void add_condition_to_expr_internal(Relevant_Condition *rc)
{
     add_condition_to_expr(rc->expr, rc);
}

PBoolean evaluable_p_terms(TermList terms, PBoolean ot);

PBoolean evaluable_p_term(Term *term, PBoolean ot)
{
     switch (term->type) {
     case INTEGER:
     case LONG_LONG:
     case FLOAT_ARRAY:
     case INT_ARRAY:
     case TT_FLOAT:
     case LISP_LIST:
     case TT_FACT:
     case TT_GOAL:
     case TT_OP_INSTANCE:
     case TT_INTENTION:
     case U_POINTER:
     case U_MEMORY:
     case STRING:
     case TT_ATOM:
     case C_LIST:
	  return FALSE;
     case VARIABLE: {
	  Term *t = find_binding_or_prog_var(term);
	  return ((t->type == VARIABLE) && /* not bound... or prog_var */
		  (ot || global_variable(t->u.var))); /* other threads or global_var */
     }
     case EXPRESSION:
	  if (PFR_EF(term->u.expr->pfr))
	       return TRUE;
	  return evaluable_p_terms(term->u.expr->terms,ot);
     default:
	  oprs_perror("evaluable_p_terms", PE_UNEXPECTED_TERM_TYPE);
	  return TRUE;
     }
}

PBoolean evaluable_p_terms(TermList terms, PBoolean ot)
{
     Term *term;

     sl_loop_through_slist(terms, term, Term *)
	  if (evaluable_p_term(term,ot))
	       return TRUE;
     
     return FALSE;
}


PBoolean evaluable_p_expr(Expression *expr, PBoolean ot)
{

     if (OR_P(expr) || AND_P(expr) ||  NOT_P(expr)) {
	  Term *term;

	  sl_loop_through_slist(expr->terms, term , Term *)
	       if (evaluable_p_expr(TERM_EXPR(term), ot))
		    return TRUE;
	  return FALSE;
     } else {
	  if (PFR_PR(expr->pfr) && PFR_EP(expr->pfr)) return TRUE;
	  else return  evaluable_p_terms(expr->terms,ot);
     }
}

PBoolean other_sleeping_thread(Intention *in)
{
     Relevant_Condition *rc;

     sl_loop_through_slist(current_oprs->conditions_list, rc, Relevant_Condition *) {
	  if ((rc->type != INTENTION_ACTIVATION) && (rc->u.tib->intention == in))
	       return TRUE;
     }
     return FALSE;
}

Relevant_Condition *make_and_install_condition(Condition_Type type, Intention *in,  
					       Thread_Intention_Block *tib, Expression *expr, Frame *frame)
{
     Relevant_Condition *rc=MAKE_OBJECT(Relevant_Condition);
     PBoolean other_thread = FALSE;

     rc->type = type;
     rc->expr = expr;
     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  rc->u.intention = in;
	  sl_add_to_head(in->activation_conditions_list,rc);
	  force_asleep_intention(current_oprs->intention_graph, in);
	  rc->frame = empty_env;
	  break;
     case TIB_ACTIVATION:
     case MAINTAIN_ACTIVATION:
     case ACTION_WAIT_ACTIVATION:
	  rc->frame = frame;
	  rc->u.tib = tib;
	  other_thread = ((list_length(tib->intention->active_tibs) != 1) ||
			  other_sleeping_thread(tib->intention));
	  break;
     default:
	  oprs_perror("make_condition", PE_UNEXPECTED_CONDITION_TYPE);
	  return NULL;
	  break;
     }

     rc->evolving = !expr || evaluable_p_expr(expr, other_thread);

     sl_add_to_head(current_oprs->conditions_list,rc);

     if (type != ACTION_WAIT_ACTIVATION) SAFE_SL_ADD_TO_HEAD(cond_force_check,rc);

     if (expr) add_condition_to_expr(expr,rc);

     return rc;
}

void desinstall_condition_from_expr(Expression *expr,  Relevant_Condition *rc)
{
     if (OR_P(expr) || AND_P(expr) ||  NOT_P(expr)) {
	  Term *term;

	  sl_loop_through_slist(expr->terms, term, Term *)
	       desinstall_condition_from_expr(TERM_EXPR(term), rc);
     } else {
	  sl_delete_slist_node(expr->pfr->u.u.pr->conditions,rc);
     }
}

void desinstall_and_free_condition(Relevant_Condition *rc)
{
     if (! sl_delete_slist_node(current_oprs->conditions_list,rc))
	  fprintf(stderr,LG_STR("ERROR: desinstall_and_free_condition: Unable to desinstall condition from oprs.\n",
				"ERREUR: desinstall_and_free_condition: Incapable de désinstaller la condition de ce noyau.\n")); 
   
     /* This condition may have been added in the cond_force_check list */
     
     SAFE_SL_DELETE_SLIST_NODE(cond_force_check,rc);

     if (rc->expr) desinstall_condition_from_expr(rc->expr,rc);

     FREE(rc);
}

void desinstall_and_free_satisfied_condition(Relevant_Condition *rc)
{     
     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  if (! sl_delete_slist_node(rc->u.intention->activation_conditions_list,rc))
	       fprintf(stderr,LG_STR("ERROR: desinstall_and_free_satisfied_condition: Unable to desinstall INTENTION_ACTIVATION condition.\n",
				     "ERREUR: desinstall_and_free_satisfied_condition: Incapable de désinstaller une  condition d'INTENTION_ACTIVATION.\n")); 
	  if (sl_slist_empty(rc->u.intention->activation_conditions_list)) {
	       wake_up_intention(rc->u.intention);
	  }
	  break;
     case TIB_ACTIVATION:
     case ACTION_WAIT_ACTIVATION:
	  rc->u.tib->activation_condition = NULL;
	  break;
     case MAINTAIN_ACTIVATION:
	  if (sl_in_slist(current_oprs->conditions_list, rc)) { /* The tib has not been removed by a previous 
								     failed_maintained_tib */
	       failed_maintained_tib(rc->u.tib);
	       rc->u.tib->maintain_condition = NULL;
/*
	       rc->u.tib->maintain = NULL;
	       */
	  }
	  break;
     default:
	  oprs_perror("desinstall_and_free_satisfied_condition", PE_UNEXPECTED_CONDITION_TYPE);
	  return;
	  break;
     }

     desinstall_and_free_condition(rc);
}

PBoolean check_condition(Relevant_Condition *rc)
{
     ExprFrameList efl = NULL;
     PBoolean result=FALSE;
     ExprFrame *ef;

     if (rc->expr)		/* ACTION_WAIT do not have gtexpr. */
	  efl = consult(rc->expr, rc->frame, current_oprs->database, FALSE);

     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  if (!(sl_slist_empty(efl))) {
	       result = TRUE;
	  }
	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       free_frame(ef->frame);
	       FREE(ef);
	  }
	  FREE_SLIST(efl);
	  break;
     case TIB_ACTIVATION:
	  if (!(sl_slist_empty(efl))) {
	       result = TRUE;
	       reinstall_frame(((ExprFrame *)sl_get_slist_head(efl))->frame); /* the first one will do... */
	       wake_up_thread(rc->u.tib);
	  }
	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       free_frame(ef->frame);
	       FREE(ef);
	  }
	  FREE_SLIST(efl);
	  break;
     case MAINTAIN_ACTIVATION:
	  if (sl_slist_empty(efl)) {
	       result = TRUE;
	  } else {
	       ExprFrame *ef;
	       sl_loop_through_slist(efl, ef, ExprFrame *) {
		    free_frame(ef->frame);
		    FREE(ef);
	       }
	  }
	  FREE_SLIST(efl);
	  break;
     case ACTION_WAIT_ACTIVATION:
	  wake_up_thread(rc->u.tib);
	  result = TRUE;
	  break;
     default:
	  oprs_perror("check_condition", PE_UNEXPECTED_CONDITION_TYPE);
	  break;
     }

     return result;     
}

void check_condition_list(Condition_List cl)
{
     Relevant_Condition *rc;

     sl_loop_through_slist(cl, rc, Relevant_Condition *) {
	  if (! sl_in_slist(current_oprs->conditions_list, rc)) {
	       fprintf(stderr, LG_STR("ERROR: check_condition_list: found an obsolete condition.\n",
				      "ERREUR: check_condition_list: trouvé une condition obsolète.\n"));
	  } else if (check_condition(rc)) {
	       desinstall_and_free_satisfied_condition(rc);
	  }
     }
}

void sprint_show_relevant_condition(Sprinter *sp, Relevant_Condition *rc)
{
     SPRINT(sp,26 + MAX_PRINTED_POINTER_SIZE,
	    sprintf(f,LG_STR("<Condition %p> is of type: ",
			     "<Condition %p> est du type: "), rc));
	  
     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  SPRINT(sp,32,sprintf (f,LG_STR("INTENTION ACTIVATION for ",
					 "INTENTION ACTIVATION pour ")));
	  sprint_intention(sp, rc->u.intention);
	  break;
     case TIB_ACTIVATION:
	  SPRINT(sp,32,sprintf (f,LG_STR("THREAD ACTIVATION for ",
					 "THREAD ACTIVATION pour ")));
	  sprint_tib(sp, rc->u.tib);
	  break;
     case MAINTAIN_ACTIVATION:
	  SPRINT(sp,32,sprintf (f,LG_STR("THREAD MAINTAIN for ",
					 "THREAD MAINTAIN pour ")));
	  sprint_tib(sp, rc->u.tib);
	  break;
     case ACTION_WAIT_ACTIVATION:
	  SPRINT(sp,32,sprintf (f,LG_STR("ACTION WAIT for ",
					 "ACTION WAIT pour ")));
	  sprint_tib(sp, rc->u.tib);
	  break;
     }
     SPRINT(sp,32,sprintf (f,LG_STR("\n\tand waits: ",
				    "\n\tet attend: ")));
     sprint_expr(sp, rc->expr);
     SPRINT(sp,48,sprintf (f,LG_STR("\n\tit is %san evolving condition.\n",
				    "\n\t%s une condition évoluante.\n"),
	     (rc->evolving?LG_STR("","c'est"):LG_STR("not ","ce n'est pas"))));
}

void sprint_conditions(Sprinter *sp, Condition_List cl)
{
     if (sl_slist_empty (cl)) {
	  SPRINT(sp,64,sprintf(f,LG_STR("There is no condition in this kernel.\n",
					"Il n'y a actuellement pas de condition dans ce noyau.\n")));
     } else {
	  Relevant_Condition *rc;

	  SPRINT(sp,32,sprintf(f,LG_STR("The conditions are:\n",
					"The conditions are:\n")));
	  sl_loop_through_slist(cl, rc, Relevant_Condition *)
	       sprint_show_relevant_condition(sp, rc);
	  SPRINT(sp,10,sprintf(f,LG_STR("Done.\n",
					"Terminé.\n")));
     }
}

void show_conditions(Condition_List cl)
{
     static Sprinter *sp = NULL;

     if (! sp)
       sp = make_sprinter(0);
     else
       reset_sprinter(sp);

     sprint_conditions(sp,cl);
     printf("%s",sprinter_string(sp));
}

void recheck_all_conditions(void)
{
     Relevant_Condition *rc;

     sl_loop_through_slist(current_oprs->conditions_list, rc, Relevant_Condition *) {
	  if (! (SAFE_SL_IN_SLIST(cond_force_check,rc)))
	       SAFE_SL_ADD_TO_HEAD(cond_force_check,rc);
     }
}

void free_conditions_list(Condition_List cl)
{
     Relevant_Condition *rc;

     sl_loop_through_slist(cl, rc, Relevant_Condition *) {
	  desinstall_and_free_condition(rc);
     }
     
     FREE_SLIST(cl);
}
