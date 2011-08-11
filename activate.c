/*                               -*- Mode: C -*- 
 * activate.c -- Executes the intentions in the intention graph.
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

#include <stdlib.h>

#include "opaque.h"
#include "constant.h"
#include "macro.h"

#include "user-external.h"
#include "oprs-profiling.h"

#include "oprs.h"
#include "intention.h"
#include "int-graph.h"
#include "op-instance.h"
#include "soak.h"
#include "op-structure.h"
#include "fact-goal.h"
#include "activate.h"
#include "action.h"
#include "top-lev.h"
#include "oprs-pred-func.h"
#include "oprs-print.h"
#include "oprs-sprint.h"
#include "oprs-error.h"
#include "oprs-rerror.h"

#include "int-graph_f.h"
#include "activate_f.h"
#include "unification_f.h"
#include "oprs_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "intention_f.h"
#include "oprs-profiling_f.h"
#include "fact-goal_f.h"
#include "database_f.h"
#include "top-lev_f.h"
#include "op-structure_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"
#include "conditions_f.h"
#include "op-instance_f.h"
#include "user-trace_f.h"

Thread_Execution_Result execute_intention(Intention *in);
Thread_Execution_Result execute_intention_body(Thread_Intention_Block *t_in_bl);
Thread_Execution_Result execute_thread(Thread_Intention_Block *t_in_bl);
Thread_Execution_Result execute_top_thread(Thread_Intention_Block *tib);
PBoolean conclude_effects(ExprList effs, List_Envar lenv, Oprs *oprs);
void remove_and_free_tib(Thread_Intention_Block *t_in_bl);
void restore_suspended_tib(Thread_Intention_Block *tib);
void restore_and_fail_suspended_tib(Thread_Intention_Block *tib);
void remove_from_active_tib(Thread_Intention_Block *tib);
void add_in_active_tib(Thread_Intention_Block *tib);
void check_last_active_tib(Thread_Intention_Block *tib);

void report_execute_intention_ctxt_error(Intention *in, Intention_Graph *ig) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,128,
	    sprintf(f,LG_STR("The previous error occured in intention: ",
			     "La précédente erreur s'est produite dans l'intention: ")));
     sprint_intention(sp, in);
     SPRINT(sp,2,sprintf(f,"\n\t"));
     sprint_show_intention(sp, in, ig);
     SPRINT(sp,64,sprintf(f,LG_STR("\n\tThis intention is cancelled.\n",
				   "\n\tCette intention est retirée et détruite.\n")));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

void report_intention_failure(Intention *in)
{
     OPRS_LIST stack = in->failed_goal_stack;

     printf(LG_STR("FAILURE: ",
		   "ECHEC: "));
     print_intention(in); 
     printf(LG_STR(" has failed ",
		   " a échouée "));
     if (in->goal) {
	  printf(LG_STR("to satisfy the goal: ",
			"l'accomplissement du but: "));
	  print_expr(in->goal->statement);
	  printf("\n");
     } else if (in->fact) {
	  printf(LG_STR("in response to the fact: ",
			"en réponse au fait: "));
	  print_expr(in->fact->statement);
	  printf("\n");
     }
     if (debug_trace[INT_FAIL_BIND]) {
	  if (SPRINTER_SIZE(in->failed_goal_sprinter))
	       printf(LG_STR("\tprobably because it could not achieve\
\n\tthe goal: %s.\n",
			     "\tprobablement parce qu'elle n'a pu satisfaire\
\n\tle but: %s.\n"),
		      SPRINTER_STRING(in->failed_goal_sprinter));
     } else { /* Without binding... */
	  if (! (sl_slist_empty(stack))) {
	       Expression *expr;
	       printf(LG_STR("\tprobably because it could not achieve\
\n\tthe goal: ",
			     "\tprobablement parce qu'elle n'a pu satisfaire\
\n\tle but: "));
	       print_expr(expr = sl_get_from_head(stack));
	       free_expr(expr);
	       printf(LG_STR(" in OP %s",
			     " dans le OP %s"),
		      (char *)sl_get_from_head(stack));
	       while (! (sl_slist_empty(stack))) {
		    printf(LG_STR("\n\t\ttrying to achieve: ",
				  "\n\t\tessayant de réaliser: "));
		    print_expr(expr = sl_get_from_head(stack));
		    free_expr(expr);
		    printf(LG_STR(" in OP %s",
				  " dans le OP %s"),
			   (char *)sl_get_from_head(stack));
			
	       }
	       printf(".\n");
	  }
     }
}


PBoolean activate(Intention_Graph *ig)
/* activate - is the main function called in this file.  Return TRUE if it did something, FALSE otherwise. */
{
     find_current_intentions(ig);	/* Look for a current intention. */

     if (sl_slist_empty(ig->current_intentions)) {
	  return FALSE; /* Nothing to do. */
     } else {
	  Intention *in;
	  PBoolean res = FALSE;
	  Error_Ctxt previous_error_ctxt = current_error_ctxt;

	  current_error_ctxt = EC_EXECUTE_INTENTION;

	  sl_loop_through_slist(ig->current_intentions, in, Intention *) {
	       if (setjmp(execute_intention_jmp) == 0) {
		    res = execute_intention(in);
		    if ((res == TER_OP_SUCCESS) || (res == TER_OP_FAILURE)) { /* If success or failure, remove it. */
#ifdef  OPRS_PROFILING
			 TIME_REPORT(&in->active,T_INTENTION_ACTIVE);
#endif /* OPRS_PROFILING */
			 if (! sl_in_slist(ig->list_condamned,in)) 
			      /* Add if it was not already here */ 
			      sl_add_to_head(ig->list_condamned,in);  
			 if ((res == TER_OP_FAILURE) && (debug_trace[INTENTION_FAILURE] ||
							 debug_trace[INT_FAIL_BIND]))
			      report_intention_failure(in);
		    } else if (in->critical_section) {
			 res = TRUE;
			 sl_deprotect_loop(ig->current_intentions);
			 break;
		    } 
		    res = TRUE;
	       } else {
		    report_execute_intention_ctxt_error(in, ig);
		    if (! sl_in_slist(ig->list_condamned,in)) 
			 /* Add if it was not already here */ 
			 sl_add_to_head(ig->list_condamned,in);			      
		    current_intention = NULL;
		    current_tib = NULL;

		    res = TRUE;
	       }
	  }
	  
	  current_error_ctxt = previous_error_ctxt;
	  
	  sl_loop_through_slist(ig->list_condamned, in, Intention *) 
	       delete_intention_from_ig(in,ig);
	  
	  FLUSH_SLIST(ig->list_condamned);

	  return res;
     }
}

void remove_and_free_tib(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *pere;
     Intention *int_pere = NULL;

     pere = tib->pere;
     int_pere = tib->intention;

     if (int_pere->critical_section) { /* Check the critical section */
	  if (current_oprs->critical_section == tib) { 
	       printf (LG_STR("WARNING:  Exiting the OP where we start the critical section.\
\n\tForce exit critical section.\n",
			      "ATTENTION:  Nous quittons le OP d'où a été démarré la section critique.\
\n\tSortie de section critique forcée.\n"));
	       end_critical_section(tib);
	  } else if (int_pere->critical_section == tib)
	       unmark_critical_section(tib);
     }
     if (pere) { 
	  sl_delete_slist_node(pere->fils,tib);
	  if (pere->suspended) { 
	       sl_delete_slist_node(pere->suspended->fils,tib);
	  }      
     } else {
	  sl_delete_slist_node(int_pere->fils,tib);
     }
     remove_and_free_tib_list(tib->fils);
     remove_from_active_tib(tib); /* Just in case it was still there. */

     if (tib->suspended) { /* there is a suspended tib. */
	  remove_and_free_tib(tib->suspended);
     }
     free_thread_intention_block(tib);
}

void remove_and_free_tib_list(Thread_Intention_Block_List list)
{
     Thread_Intention_Block *tib;

     while ((tib = ((Thread_Intention_Block *)sl_get_from_head(list)))) {
	  remove_and_free_tib(tib);
     }
}

void remove_and_free_all_brothers_and_me(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *pere;
     Thread_Intention_Block_List list;

     pere = tib->pere;
     if (pere) { 
	  list = pere->fils;
     } else {
	  list = tib->intention->fils;
     }
     remove_and_free_tib_list(list);
}

Thread_Execution_Result satisfied_thread(Thread_Intention_Block *tib)
/* satified_goal - is called upon success of a OP ( all the thread where successfull ).
 */
{
     if (tib->activation_condition) { /* If there is an activation condition, we must be waiting... */
	  return TER_WAITING;
     } else {
	  Op_Instance *op_ins;
	  Goal *goal;
	  Fact *fact;
	  Thread_Execution_Result result;
	  PBoolean posted_fact_in_effects_part = FALSE;
	  Thread_Intention_Block *pere;
	  Intention *int_pere = NULL;
	  PBoolean last;

	  pere = tib->pere;
	  if (!pere) int_pere = tib->intention;

	  last = (sl_slist_length(pere ? pere->fils : int_pere->fils) == 1);
     
	  if (last) {
	       op_ins = tib->curr_op_inst;

	       if ((goal = op_ins->goal) != NULL) {
		    /* If it was a goal, we succeeded it */
		    goal->succes = op_ins;
#ifdef  OPRS_PROFILING
		    TIME_STOP(&goal->creation,T_GOAL_CR_REP);
		    TIME_STOP(&goal->reception,T_GOAL_REC_REP);
		    TIME_STOP(&goal->soak,T_GOAL_SOAK_REP);
		    TIME_STAMP(&goal->reponse);
#endif /* OPRS_PROFILING */

		    if (debug_trace[SUC_FAIL]) {
			 static Sprinter *sp = NULL;

			 if (!sp) sp = make_sprinter(0);
			 else reset_sprinter(sp);
			 sprint_expr(sp, goal->statement);
			 SPRINT(sp, 24 + strlen(op_ins->op->name),
				sprintf(f, LG_STR(" achieved by %s.\n",
						  " satisfait par %s.\n"), op_ins->op->name));
#ifdef GRAPHIX
			 if (tib->intention->trace_dialog) 
			      xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
			 else 
#endif
			 printf("%s",SPRINTER_STRING(sp));
		    }

		    if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
			 user_trace_suc_fail(op_ins,goal,NULL, TRUE);

	       } else if  ((fact = op_ins->fact) != NULL) {
#ifdef  OPRS_PROFILING
		    TIME_STOP(&fact->creation,T_FACT_CR_REP);
		    TIME_STOP(&fact->reception,T_FACT_REC_REP);
		    TIME_STOP(&fact->soak,T_FACT_SOAK_REP);
		    TIME_STAMP(&fact->reponse);
#endif /* OPRS_PROFILING */
	  
		    if (debug_trace[SUC_FAIL]) {
			 static Sprinter *sp = NULL;
			 
			 if (!sp) sp = make_sprinter(0);
			 else reset_sprinter(sp);
			 sprint_expr(sp, fact->statement);
			 SPRINT(sp, 64 + strlen(op_ins->op->name),
				sprintf(f, LG_STR("led to the successful execution of %s.\n",
						  "a conduit à l'exécution satisfaisante de %s.\n"), op_ins->op->name));
#ifdef GRAPHIX
			 if (tib->intention->trace_dialog) 
			      xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
			 else 
#endif
			 printf("%s",SPRINTER_STRING(sp));
		    }

		    if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
			 user_trace_suc_fail(op_ins,NULL,fact,TRUE);

	       }
	       else fprintf(stderr,LG_STR("ERROR: satisfy_thread: Expecting a fact or a goal in the OP instance\n",
					  "ERREUR: satisfy_thread: Fait ou but manquant dans la OP instance\n"));
	  
	       if ((op_ins->op != goal_satisfied_in_db_op_ptr) && /* if it is a real OP... */
		   (op_ins->op != goal_waiting_op_ptr) &&
		   (op_ins->op != goal_for_intention_op_ptr) &&
		   op_ins->op->effects)
		    /* Conclude the effect part */
		    posted_fact_in_effects_part = conclude_effects(op_ins->op->effects,op_ins->lenv,
								   current_oprs);
	  }
	  if (posted_fact_in_effects_part) {
	       if (tib->pere) {
		    add_in_active_tib(tib->pere);
		    if (TIB_INTENTION(tib)->critical_section) 
			 propagate_critical_section(tib);
		    remove_and_free_tib(tib); /* But we do not call execute_top_thread on the father. */
		    result = TER_EXECUTING; /* Next time, it will find an achieved goal below. */
	       } else { 
		    remove_and_free_tib(tib); /* In this case we do not care when the facts are posted.. no race. */
		    result = TER_OP_SUCCESS;
	       }
	  } else if (last) {
	       if (tib->pere) {	/* If we have a father */
		    Thread_Intention_Block *tib2;

		    add_in_active_tib(tib2 = tib->pere);
		    if (TIB_INTENTION(tib)->critical_section) 
			 propagate_critical_section(tib);
		    remove_and_free_tib(tib);
		    result = execute_top_thread(tib2); /* we call it. */
	       } else {		/* The last thread in the last OP...*/
		    remove_and_free_tib(tib); /* This is all over for this lucky one. */
		    result = TER_OP_SUCCESS;
	       }
	  } else {
	       remove_and_free_tib(tib);
	       result = TER_EXECUTING;	       
	  }

	  return result;
     }
}

Thread_Execution_Result failed_thread(Thread_Intention_Block *tib)
/* failed_thread - is called whenver a goal has been failed by a op instance... */
{
     Op_Instance *op_ins;
     Goal *goal;
     Fact *fact;
     Thread_Execution_Result result;
     Thread_Intention_Block *pere;
     Intention *int_pere = NULL;

     pere = tib->pere;
     if (!pere) int_pere = tib->intention;

     op_ins = tib->curr_op_inst;

     if ((goal = op_ins->goal) != NULL) {
	 /* Note that we add it to the list of OP which failed it... May be another one will be luckier */
	  SAFE_SL_ADD_TO_HEAD(goal->echec,dup_op_instance(op_ins));

	  if (debug_trace[SUC_FAIL]) {
	       static Sprinter *sp = NULL;

	       if (!sp) sp = make_sprinter(0);
	       else reset_sprinter(sp);
	       sprint_expr(sp, goal->statement);
	       SPRINT(sp, 24 + strlen(op_ins->op->name),
		      sprintf(f, LG_STR(" failed by %s.\n",
					" échoué par %s.\n"), op_ins->op->name));
#ifdef GRAPHIX
	       if (tib->intention->trace_dialog) 
		    xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
	       else 
#endif
	       printf("%s",SPRINTER_STRING(sp));
	  }
	  if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
	       user_trace_suc_fail(op_ins,goal,NULL,FALSE);

     } else if  ((fact = op_ins->fact) != NULL) {

	  if (debug_trace[SUC_FAIL]) {
	       static Sprinter *sp = NULL;

	       if (!sp) sp = make_sprinter(0);
	       else reset_sprinter(sp);
	       sprint_expr(sp, fact->statement);
	       SPRINT(sp, 32 + strlen(op_ins->op->name),
		      sprintf(f, LG_STR("led to the failure of %s.\n",
					"a conduit à l'échec de %s.\n"), op_ins->op->name));
#ifdef GRAPHIX
			 if (tib->intention->trace_dialog) 
			      xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
			 else 
#endif
			 printf("%s",SPRINTER_STRING(sp));
	  }
	  if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
	       user_trace_suc_fail(op_ins,NULL,fact,FALSE);

     } else fprintf(stderr,LG_STR("ERROR: failed_thread: Expecting a fact or a goal in the OP instance\n",
				  "ERREUR: failed_thread: Fait ou but manquant dans la OP instance\n"));
     
     /* We need to desinstall the environement because it was a failure */
     desinstall_frame(TIB_FRAME(tib), (pere ? TIB_FRAME(pere) : (op_ins->goal ? op_ins->goal->frame : empty_env)));
     unbind_lenv(op_ins->lenv);

     if (pere) {	/* We have a father... */
	  Thread_Intention_Block *tib2;
	  
	  add_in_active_tib(tib2 = tib->pere);
	  if (TIB_INTENTION(tib)->critical_section) 
	       propagate_critical_section(tib);
	  remove_and_free_all_brothers_and_me(tib);
	  result = execute_top_thread(tib2); /* May be he has some alternative... */
     } else {
	  remove_and_free_all_brothers_and_me(tib);
	  result = TER_OP_FAILURE; /* This is all over for this intention... too bad. */
     }
     
     return result;
}

PBoolean conclude_effects(ExprList effs, List_Envar lenv, Oprs *oprs)
/* conclude_effects - is called to conclude the effet part (if any) of a successful OP Instance. */
{
     Expression *expr,*expr2;
     PBoolean posted_fact = FALSE;

     sl_loop_through_slist(effs, expr, Expression *) {
	  Expression *expr1;

	  /* Apply the lenv substitution */
	  expr2 = subst_lenv_in_expr(expr, lenv, QET_NONE);
	  switch (expr_db_type(expr2)) {
	  case DB_CONC:		/* Conclude */
	       expr1 = find_binding_expr(EXPR_EXPR1(expr2));
	       /* The conclude is added with a new fact... */
	       add_fact(make_fact_from_expr(expr1), oprs);
	       posted_fact = TRUE;
	       break;
	  case DB_RET: {		/* Retract */			  
	       Condition_List cond_tmp;
	       Relevant_Condition *rc;
	       expr1 = EXPR_EXPR1(expr2);
	       delete_fact_expr(expr1, oprs->database);

	       /* Need to remember which condition may be changed by this retraction */
	       cond_tmp = (PFR_PR(expr1->pfr)?PFR_PR(expr1->pfr)->conditions:NULL);

	       SAFE_SL_LOOP_THROUGH_SLIST(cond_tmp, rc, Relevant_Condition *) {
		    if (! (SAFE_SL_IN_SLIST(cond_force_check,rc)))
			 SAFE_SL_ADD_TO_HEAD(cond_force_check,rc);
	       }

	       break;
	  }
	  default:
	       fprintf(stderr,LG_STR("ERROR: conclude_effects: Unknown operator in effects part.\n",
				     "ERREUR: conclude_effects: Opérateur inconnu dans la partie effets.\n")); 
	       break;
	  }
	  free_expr(expr2);
     }
     return (posted_fact);
}

Thread_Execution_Result execute_intention_goal_satisfied_in_db(Thread_Intention_Block *tib)
/* 
 * execute_intention_goal_satisfied_in_db - execute OP Instance of goal which are satisfied in the DB 
 *                                          already. 
 */
{
     return satisfied_thread(tib);
}

Thread_Execution_Result execute_intention_goal_waiting(Thread_Intention_Block *tib)
/* 
 * execute_intention_goal_waiting - execute waiting goal. The first call, we put the 
 *				    intention to sleep, and store the activation 
 *				    condition. The second call, we succeeded (because we woke 
 *				    up), therefore the activation consition has been achieved.
 */
{
     if (tib->num_called == 0) {
	  tib->num_called++;
	  asleep_tib_on_cond(tib, EXPR_EXPR1(tib->curr_op_inst->goal->statement));
	  return TER_WAITING;
     } else			/* Second call... We are waking up. Therefore, we succeeded. */
	  return satisfied_thread(tib);
}

void asleep_tib_on_cond(Thread_Intention_Block *tib, Expression *condition)
{
     if (tib->intention->critical_section == tib) {
	  printf (LG_STR("WARNING: Puting asleep the thread in a critical section is dangerous.\n",
			 "ATTENTION: Endormir un thread dans une section critique est très dangereux.\n"));
     }

     tib->activation_condition = make_and_install_condition(TIB_ACTIVATION, NULL, tib, condition, TIB_FRAME(tib));
     tib->status = TIBS_SLEEPING;
     remove_from_active_tib(tib);
     check_last_active_tib(tib);
}

void asleep_tib_on_action_wait(Thread_Intention_Block *tib)
{
     tib->activation_condition = make_and_install_condition(ACTION_WAIT_ACTIVATION, NULL, tib, NULL, TIB_FRAME(tib));
     tib->status = TIBS_ACTION_WAIT;
     remove_from_active_tib(tib);
     check_last_active_tib(tib);
}

void trace_action_op(Thread_Intention_Block *tib)
{
     Op_Instance *op_ins = tib->curr_op_inst;

     if (debug_trace[TEXT_OP] && op_ins->op->text_traced) {
	  static Sprinter *sp = NULL;

	  if (!sp) sp = make_sprinter(0);
	  else reset_sprinter(sp);
	  SPRINT(sp,32 + strlen(op_ins->op->name),
		 sprintf(f,LG_STR("Executing OP Action: %s\n",
				  "Exécute le OP Action: %s\n"), op_ins->op->name));
	  
#ifdef GRAPHIX
	  if (tib->intention->trace_dialog) 
	       xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
	  else 
#endif
	  printf("%s",SPRINTER_STRING(sp));
     }
     
     if (user_trace[USER_TRACE] && user_trace[UT_OP])
	  user_trace_op(tib,op_ins,NULL,0);

     if (op_ins->op->step_traced && (oprs_run_mode == STEP_NEXT)) set_oprs_run_mode(STEP_HALT);

#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_OP] && op_ins->op->graphic_traced) {
	  display_op_no_dd(op_ins->op);
     }
#endif
}

void report_execute_action_ctxt_error(Thread_Intention_Block *tib) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);
     
     SPRINT(sp,128,sprintf(f,LG_STR("The previous error occured while executing an action in the tib: ",
				    "La précédente erreur s'est produite lors de l'exécution d'une action dans le tib: ")));
     sprint_tib(sp,tib);
     SPRINT(sp,32,sprintf(f,LG_STR("\n\tof the intention: ",
				   "\n\tde l'intention: ")));
     sprint_intention(sp, tib->intention);
     SPRINT(sp,1,sprintf(f,"\n"));
     
     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

Thread_Execution_Result execute_intention_action(Thread_Intention_Block *tib)
/* execute_intention_action - execute action OP. */
{
     Thread_Execution_Result result;
     Error_Ctxt previous_error_ctxt;

     Action_Field *action;
     Op_Instance *op_ins;
     TermList terms;
     Term *term;
     TermList terml;
     Action *ac;
     Expression *tc;

#ifdef OPRS_PROFILING     
     PDate tp;
#endif
     op_ins = tib->curr_op_inst;
     action = op_ins->op->action;

     trace_action_op(tib);     
#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_ACTION_OP])
	  GET_PROFILE_TIMER(&tp);
#endif
     if (! ((ac = PFR_AC(action->function->pfr)) || (ac = PFR_EF(action->function->pfr)))) {
	  fprintf(stderr,LG_STR("ERROR: execute_intention_action: The function %s\
\n\tin the action is not known as evaluable\
\n\t(no code linked in this kernel).\n",
				"ERREUR: execute_intention_action: La fonction %s\
\n\tde l'action n'est pas connue comme étant évaluable\
\n\t(pas de code linké dans ce noyau).\n"), 
		  action->function->pfr->name);
	  return (failed_thread(tib));
     }
     tc = subst_lenv_in_expr(action->function, op_ins->lenv, QET_NONE);
     terms = find_binding_ef_tl(tc->terms);

     previous_error_ctxt = current_error_ctxt;
     current_error_ctxt = EC_EXECUTE_ACTION;
     
     if (setjmp(execute_action_jmp) == 0) {
	  term = evaluate_term_action(ac,action->function->pfr->name,terms);
     } else {
	  report_execute_action_ctxt_error(tib);
	  propagate_error(previous_error_ctxt);
     }
     current_error_ctxt = previous_error_ctxt;

     free_tl(terms);
     free_expr(tc);
     tib->num_called++; /* This is how the function will figure out the number of call... */

     if ((term->type == TT_ATOM) && (term->u.id == wait_sym)) { /* Waiting? */
	  asleep_tib_on_action_wait(tib);
	  result = TER_ACTION_WAIT;
     } else if ((term->type == TT_ATOM) && (term->u.id == wait_son_sym)) { /* Waiting? */
	  result = TER_EXECUTING;
     } else if (action->special) { /* If it is a special OP...(with *=* ) */
	  if (action->multi) {
	       if (term->type != C_LIST) {
		    fprintf(stderr,LG_STR("ERROR: execute_intention_action: Expecting a C_LIST in the term\
\n\treturned by the multi-variable special action.\n",
					  "ERREUR: execute_intention_action: C_LIST attendu dans le term\
\n\trenvoyé par l'action spéciale multi-variables.\n"));
		    result = failed_thread(tib);
	       } else if (sl_slist_length(terml = term->u.c_list) != sl_slist_length(action->u.list_envar)) {
		    fprintf(stderr,LG_STR("ERROR: execute_intention_action: Number of values returned\
\n\tby a multi-variable special action differs\
\n\tfrom the number of variables to bind.\n",
					  "ERREUR: execute_intention_action: Le nombre de valeurs renvoyées\
\n\tpar l'action spéciale multi-variables est\
\n\tdifférent du nombre de variables à lier.\n"));
		    result = failed_thread(tib);
	       } else {
		    Envar *var;
		    Term *t2;
		    PBoolean failed = FALSE;
		    List_Envar lenv = subst_lenv_in_vars_list(action->u.list_envar,op_ins->lenv);
			 
		    var = (Envar *)sl_get_slist_next(lenv, NULL);
		    t2 = (Term *)sl_get_slist_next(terml, NULL);
		    while (var != NULL && t2 != NULL && ! failed) {
			 Term *t1;

			 t1 = build_term_from_var(var);
			 if (unify_term(find_binding_or_prog_var(t1),	
					find_binding_or_prog_var(t2),  TIB_FRAME(tib))) {
			      var = (Envar *)sl_get_slist_next(lenv, var);
			      t2 = (Term *)sl_get_slist_next(terml, t2);
			 } else {
			      failed = TRUE;
			 }
			 FREE(t1);
		    }
		    if (failed)
			 result = failed_thread(tib);
		    else 
			 result = satisfied_thread(tib);
		    free_lenv(lenv);
	       }
	  } else {
	       Term *t1;
	       Envar *env = subst_lenv_in_envar(action->u.envar, op_ins->lenv);
		    
	       t1 = build_term_from_var(env);
	       if (unify_term(find_binding_or_prog_var(t1), /* Unify? */
			      find_binding_or_prog_var(term), TIB_FRAME(tib))) {
		    result = satisfied_thread(tib);
	       } else {		/* Otherwise, we failed. */
		    result = failed_thread(tib);
	       }
	       free_envar(env);
	       FREE(t1);
	  }

     } else {			/* It is a standard action OP */
	  if ((term->type == TT_ATOM) && (term->u.id == nil_sym)) { /* Nil? */
	       result = failed_thread(tib);
	  } else {		/* Otherwise, we succeeded */
	       result = satisfied_thread(tib);
	  }
     }
     free_term(term);

#ifdef OPRS_PROFILING     
	  if (profiling_option[PROFILING] && profiling_option[PROF_ACTION_OP])
	       ADD_PROFILE_TIMER(&tp,op_ins->op->time_active);
#endif

     return result;
}

Goal *build_goal_from_suspended_tib(Thread_Intention_Block *tib)
{
     Goal *susp_goal =  tib->suspended->current_goal;
     Expression *maint_cond = tib->suspended->maintain;
     Goal *goal;

     goal = make_goal_from_expr(dup_expr(maint_cond), susp_goal->frame);

     goal->interne = susp_goal->interne;
     goal->tib = tib;
     DUP(tib);
     goal->cp = susp_goal->cp;
     goal->edge = susp_goal->edge;
     
     return goal;
}

void report_posting_goal_ctxt_error(Expression *expr, List_Envar lenv, Thread_Intention_Block *tib) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,128,sprintf(f,LG_STR("The previous error occured while posting the goal: ",
				    "La précédent erreur s'est produite en postant le but: ")));
     sprint_expr(sp,expr);
     SPRINT(sp,32,sprintf(f,LG_STR("\n\twith the binding: ",
				   "\n\tavec les liaisons: ")));
     sprint_lenv(sp,lenv);

     if (tib->current_node)
	  SPRINT(sp,48 + 32 + strlen(tib->curr_op_inst->op->name),
		 sprintf(f,LG_STR("\n\toutgoing from the node %p in OP %s",
				  "\n\tce but provient du noeud %p du OP %s"),
			 tib->current_node, tib->curr_op_inst->op->name));

     SPRINT(sp,16,sprintf(f,LG_STR("\n\tin the tib: ",
				   "\n\tdans le tib: ")));
     sprint_tib(sp,tib);
     SPRINT(sp,1,sprintf(f,"\n"));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

void whipe_out_goal_failure(Intention *in)
{
     OPRS_LIST stack = in->failed_goal_stack;

     if (SPRINTER_SIZE(in->failed_goal_sprinter))
	  reset_sprinter(in->failed_goal_sprinter);
     
     while (! (sl_slist_empty(stack))) {
	  free_expr(sl_get_from_head(stack));
	  sl_get_from_head(stack); /* Op name. */
     }
}

Goal *build_goal_from_edge(Edge *edge, List_Envar lenv, FramePtr frame, Thread_Intention_Block *tib)
{
     Goal *res;

     if (debug_trace[INTENTION_FAILURE] || debug_trace[INT_FAIL_BIND])
	  whipe_out_goal_failure(tib->intention); /* At this point we are trying a new alternative... */

     if (! edge->expr) {
	  res = make_goal_from_edge(NULL, frame, tib, edge->out, edge);
     } else {
	  Error_Ctxt previous_error_ctxt = current_error_ctxt;

	  current_error_ctxt = EC_GOAL_POSTING;
	  if (setjmp(goal_posting_jmp) == 0) {

	       if (run_option[EVAL_ON_POST] )
		    res =  make_goal_from_edge(subst_lenv_in_expr(edge->expr, lenv, QET_EVAL),
					       frame, tib, edge->out, edge);
	       else
		    res = make_goal_from_edge(subst_lenv_in_expr(edge->expr, lenv, QET_QUOTE),
					      frame, tib, edge->out, edge);
	  } else {
	       report_posting_goal_ctxt_error(edge->expr,lenv,tib);
	       propagate_error(previous_error_ctxt);
	  }
	  current_error_ctxt = previous_error_ctxt;
     }
     
     return res;
}

EdgeList make_edge_list(Control_Point *node)
/* 
 * make_edge_list - return a list of Goals, created from a list of "edges" outgoing from a 
 *                  OP node. 
 */
{
     LogicList ll;
     Logic *logic;
     EdgeList gl = sl_make_slist();

     if (node) {
	  ll = node->out;
	  sl_loop_through_slist(ll, logic, Logic *) {
	       if (RANDOM() & 16) 
		    sl_add_to_head(gl, logic);
	       else
		    sl_add_to_tail(gl, logic);
	  }
     }
     return gl;
}

void trace_op_edge(Op_Instance *op_ins, Thread_Intention_Block *tib)
/* trace function */
{

     Goal *goal = tib->current_goal;

     if (debug_trace[TEXT_OP] && op_ins->op->text_traced && goal->statement) {
	  static Sprinter *sp = NULL;

	  if (!sp) sp = make_sprinter(0);
	  else reset_sprinter(sp);
	  SPRINT(sp,8,sprintf(f,LG_STR("Post: ",
				       "Poste: ")));
	  sprint_expr(sp,goal->statement);
	  SPRINT(sp,8 + strlen(op_ins->op->name),
				    sprintf(f,LG_STR(" from %s.\n",
						     " de %s.\n"), op_ins->op->name));
					 
#ifdef GRAPHIX
	  if (tib->intention->trace_dialog) 
	       xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
	  else 
#endif
	       printf("%s",SPRINTER_STRING(sp));
     }

     if (user_trace[USER_TRACE] && user_trace[UT_OP])
	  user_trace_op(tib,op_ins,goal,1);

     if (op_ins->op->step_traced && goal->statement && (oprs_run_mode == STEP_NEXT))
	  set_oprs_run_mode(STEP_HALT);

#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_OP] && op_ins->op->graphic_traced && goal->statement) {
	  display_op_edge(op_ins->op,goal->edge);
     }
#endif
}

void untrace_op_edge(Op_Instance *op_ins,Thread_Intention_Block *tib)
{
     Goal *goal = tib->current_goal;

     if (debug_trace[TEXT_OP] && op_ins->op->text_traced && goal->statement) {
	  static Sprinter *sp = NULL;

	  if (!sp) sp = make_sprinter(0);
	  else reset_sprinter(sp);
	  
	  SPRINT(sp,16,sprintf(f,LG_STR("Unpost: ",
					"Déposte: ")));
	  sprint_expr(sp,goal->statement);
	  SPRINT(sp,16 + strlen(op_ins->op->name),
				    sprintf(f,LG_STR(" from %s.\n",
						     " de %s.\n"), op_ins->op->name));
	  
#ifdef GRAPHIX
	  if (tib->intention->trace_dialog)
	       xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
	  else
#endif
	       printf("%s", SPRINTER_STRING(sp));
     }

     if (user_trace[USER_TRACE] && user_trace[UT_OP])
	  user_trace_op(tib,op_ins,goal,2);

#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_OP] && op_ins->op->graphic_traced && goal->statement && goal->edge) {
	  undisplay_op_edge(op_ins->op,goal->edge);
     }
#endif
}

PBoolean make_new_threads_and_post_goals(Thread_Intention_Block *tib, Op_Instance *opi, Oprs *oprs)
{
     LogicList ll;
     Logic *logic;
     Thread_Intention_Block *new_tib;
     PBoolean first = TRUE;
   
     if (sl_slist_empty(ll = tib->current_node->out)) return FALSE;
     
     sl_loop_through_slist(ll, logic, Logic *) {
	  if (first) {
	       first = FALSE;
	       new_tib = tib;

	       if (debug_trace[THREADING]) {
		    static Sprinter *sp = NULL;

		    if (!sp) sp = make_sprinter(0);
		    else reset_sprinter(sp);
		    SPRINT(sp,4,sprintf(f,LG_STR("The ",
						 "Le ")));
		    sprint_tib(sp,tib);
		    SPRINT(sp,4,sprintf(f,LG_STR(" in ",
						 " de ")));
		    sprint_intention(sp,tib->intention);
		    SPRINT(sp,40 + 32 + strlen(tib->curr_op_inst->op->name),
			   sprintf(f,LG_STR(" is proceeding at node %p from OP %s.\n",
					    " poursuit au noeud %p du OP %s.\n"),
				   tib->current_node,
				   tib->curr_op_inst->op->name));
		       
#ifdef GRAPHIX
		    if (tib->intention->trace_dialog)
			 xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
		    else
#endif
			 printf("%s", SPRINTER_STRING(sp));
	       }
	       add_goal((new_tib->current_goal = build_goal_from_edge(logic,
								      opi->lenv,
								      opi->frame,
								      new_tib)),
			oprs);	/* Post one. */
	       trace_op_edge(opi,new_tib);
	  } else {
	       new_tib = fork_thread_intention_block(tib);

	       if (debug_trace[THREADING]) {
		    static Sprinter *sp = NULL;
		  
		    if (!sp) sp = make_sprinter(0);
		    else reset_sprinter(sp);
		    SPRINT(sp,4,sprintf(f,LG_STR("The ",
						 "Le ")));
		    sprint_tib(sp,tib);
		    SPRINT(sp,4,sprintf(f,LG_STR(" in ",
						 " de ")));
		    sprint_intention(sp,tib->intention);
		    SPRINT(sp,40 + 32 + strlen(tib->curr_op_inst->op->name),
			   sprintf(f,LG_STR(" is created at node %p from OP %s.\n",
					    " est créé au noeud %p du OP %s.\n"),
				   tib->current_node,
				   tib->curr_op_inst->op->name));
		  
#ifdef GRAPHIX
		    if (tib->intention->trace_dialog) 
			 xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
		    else
#endif
			 printf("%s",SPRINTER_STRING(sp));
	       }
	     
	       sl_add_to_tail((tib->pere ? 
			       tib->pere->fils :  
			       tib->intention->fils),
			      new_tib);
	       add_in_active_tib(new_tib);
	       new_tib->current_goal = build_goal_from_edge(logic,
							    opi->lenv,
							    opi->frame,
							    new_tib);
	       if (run_option[PAR_POST]) {
		    add_goal(new_tib->current_goal, oprs);
		    trace_op_edge(opi,new_tib);
	       }
	  }
     }
     return TRUE;
}

Thread_Intention_Block_List joining_tib(Thread_Intention_Block_List tib_list, Node *node)
{
     Thread_Intention_Block *tib;
     Thread_Intention_Block_List res = sl_make_slist();

     sl_loop_through_slist(tib_list, tib, Thread_Intention_Block *) {
	  if ((tib->status == TIBS_JOINING) && (tib->current_node == node)) 
	       sl_add_to_tail(res, tib);
     }
     
     return res;
}

Thread_Execution_Result analyze_node_and_execute_intention_body(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *tib_tmp;

     if (! tib->current_node) return satisfied_thread(tib); /* No node... Goal intention... */

     if (tib->current_node->join) {
	  Thread_Intention_Block_List fils = (tib->pere ? 
						     tib->pere->fils :  
						     tib->intention->fils);
	  Thread_Intention_Block_List joining_tib_list;

	  joining_tib_list = joining_tib(fils, tib->current_node);
	  
	  if ((sl_slist_length(tib->current_node->in) - 1) != sl_slist_length(joining_tib_list)) {
	       
	       if (debug_trace[THREADING]) {
		  static Sprinter *sp = NULL;
		  
		  if (!sp) sp = make_sprinter(0);
		  else reset_sprinter(sp);
		  SPRINT(sp,4,sprintf(f,LG_STR("The ",
					       "Le ")));
		  sprint_tib(sp,tib);
		  SPRINT(sp,4,sprintf(f,LG_STR(" in ",
					       " de ")));
		  sprint_intention(sp,tib->intention);
		  SPRINT(sp,40 + 32 + strlen(tib->curr_op_inst->op->name),
			 sprintf(f,LG_STR(" is joining at node %p in OP %s.\n",
					  " rejoint en attente au noeud %p du OP %s.\n"),
				 tib->current_node,
				 tib->curr_op_inst->op->name));
		  
#ifdef GRAPHIX
		  if (tib->intention->trace_dialog) 
		       xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
		  else
#endif
		       printf("%s",SPRINTER_STRING(sp));
	       }
	       FREE_SLIST(joining_tib_list);

	       if (tib->intention->critical_section == tib) {
		    printf (LG_STR("The thread is joining in a critical section.\
\n\tForce exit of the critical section.\n",
				   "Le thread rejoint en attente alors qu'il est en section critique.\
\n\tSortie forcé de la section critique.\n"));
		    end_critical_section(tib);
	       }
	       tib->status = TIBS_JOINING;
	       remove_from_active_tib(tib);
	       check_last_active_tib(tib);
	       return TER_JOINING;
	  } else {
	       if (debug_trace[THREADING]) {
		    static Sprinter *sp = NULL;
		  
		    if (!sp) sp = make_sprinter(0);
		    else reset_sprinter(sp);
		    SPRINT(sp,4,sprintf(f,LG_STR("The ",
						 "Le ")));
		    sprint_tib(sp,tib);
		    SPRINT(sp,4,sprintf(f,LG_STR(" in ",
						 " de ")));
		    sprint_intention(sp,tib->intention);
		    SPRINT(sp,64 + 32 + strlen(tib->curr_op_inst->op->name),
			   sprintf(f,LG_STR(" is the last thread joining at node %p in OP %s.\n",
					    " est le dernier thread a rejoindre au noeud %p du OP %s.\n"),
				   tib->current_node,
				   tib->curr_op_inst->op->name));
		  
#ifdef GRAPHIX
		    if (tib->intention->trace_dialog) 
			 xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
		    else
#endif
			 printf("%s",SPRINTER_STRING(sp));
	       }
	       sl_loop_through_slist(joining_tib_list, tib_tmp, Thread_Intention_Block *) {
		    remove_and_free_tib(tib_tmp);
	       }
	       FREE_SLIST(joining_tib_list);
	  }
     }
     return execute_intention_body(tib); /* If not split or last join, we continue execution */
}

Thread_Execution_Result execute_intention_body(Thread_Intention_Block *tib)
/* 
 * execute_intention_body - is one of the most important function in the whole kernel. It 
 *                          is responsible for the execution of procedure...
 *                          Return an Thread_Execution_Result.
 *                          
 */
{
     Op_Instance *op_ins;
     List_Envar lenv;
     Op_Structure *op;
     FramePtr frame;

     op_ins = tib->curr_op_inst;
     lenv = op_ins->lenv;
     op = op_ins->op;
     frame = op_ins->frame;

     if ((SAFE_SL_SLIST_EMPTY(tib->list_to_try)) && ! (tib->current_goal)) { /* Has not been seen before, and no goal... */
	  if (tib->current_node->split) {
	       if (make_new_threads_and_post_goals(tib, op_ins, current_oprs))
		    return TER_EXECUTING;
	       else 
		    return satisfied_thread(tib);
	  } else {
	       SAFE_SL_FREE_SLIST(tib->list_to_try);
	       if (sl_slist_empty(tib->list_to_try = make_edge_list(tib->current_node))) { /* No outgoing edge. */
		    return satisfied_thread(tib);
	       }
	  }
     }
     if (tib->current_goal == NULL) {	/* No current goal? */
	  add_goal((tib->current_goal = build_goal_from_edge((Edge *)sl_get_slist_head(tib->list_to_try),
							     lenv,
							     frame,
							     tib)),
		   current_oprs);	/* Post one. */
	  trace_op_edge(op_ins,tib);
	  if (tib->current_goal->succes) 
	       return execute_intention_body(tib); /* this recursive call seems to be superfluous and add problems with list of assert. */
	  else
	       return TER_EXECUTING;    
     } else if (tib->current_goal->echoue != NOT_FAILED) { 
	  if (tib->current_goal->echoue == ACT_MAINT_FAILURE) { 
	       /* We post the preservation goal to restore it.  */
	       if (tib->suspended) {
		    tib->current_goal = build_goal_from_suspended_tib(tib);
		    add_goal(tib->current_goal, current_oprs);
		    return TER_EXECUTING;
	       } else {
		    /* when did we get here ?? */
		    return TER_EXECUTING;
	       }
	  } else { /* We failed it (completly). */
	       if (tib->suspended) { /* this goal was posted to restore a maintain condition */
		    free_goal(tib->current_goal); /* This is all over for this one */
		    tib->current_goal = NULL;
		    restore_and_fail_suspended_tib(tib);
		    return TER_EXECUTING;
	       } else
		    /* list_to_try may be empty or NULL for new thread... */
		    SAFE_SL_GET_FROM_HEAD(tib->list_to_try); /* We know it is the first one in the list. */

	       untrace_op_edge(op_ins,tib);
	       if ((tib->current_goal->edge) && 
		   (tib->current_goal->edge->type == ET_IF)) {
		    tib->current_node = else_node_from_if_node(tib->current_goal->cp);
		    if (tib->maintain_condition) {
			 desinstall_and_free_condition(tib->maintain_condition);
			 tib->maintain_condition = NULL;
			 tib->maintain = NULL;
		    }
		    free_goal(tib->current_goal); /* This is all over for this one */
		    tib->current_goal = NULL;
		    SAFE_SL_FLUSH_SLIST(tib->list_to_try);
		    return analyze_node_and_execute_intention_body(tib);
	       } else {
		    if (tib->maintain_condition) {
			 desinstall_and_free_condition(tib->maintain_condition);
			 tib->maintain_condition = NULL;
			 tib->maintain = NULL;
		    }
		    free_goal(tib->current_goal); /* This is all over for this one */
		    tib->current_goal = NULL;
		    if (SAFE_SL_SLIST_EMPTY(tib->list_to_try)) { /* It was our last chance... we have failed this OP. */
			 return failed_thread(tib);
		    } else {		/* we still have some possibilities. */
			 add_goal((tib->current_goal = build_goal_from_edge((Edge *)sl_get_slist_head(tib->list_to_try),
									    lenv,
									    frame,
									    tib)),
				  current_oprs); /* post one */
			 trace_op_edge(op_ins,tib);
			 return TER_EXECUTING;
		    }		
	       }
	  }
     } else if (tib->current_goal->succes) { /* The goal was successfuly achieved... */
	  if (tib->suspended) { /* this goal was posted to restore a maintain condition */
	       free_goal(tib->current_goal); /* This is all over for this one */
	       tib->current_goal = NULL;

	       restore_suspended_tib(tib);
	       return TER_EXECUTING;
	  }

	  untrace_op_edge(op_ins,tib);
	  if (tib->current_goal->edge) {
	       if (tib->current_goal->edge->type == ET_IF)
		    tib->current_node = then_node_from_if_node(tib->current_goal->cp);
	       else
		    tib->current_node = tib->current_goal->cp;
	  } else
	       tib->current_node = NULL; /*  No edge, it is a goal intention */
	  if (tib->maintain_condition) {
	       desinstall_and_free_condition(tib->maintain_condition);
	       tib->maintain_condition = NULL;
	       tib->maintain = NULL;
	  }
	  free_goal(tib->current_goal); /* This is all over for this one */
	  tib->current_goal = NULL;
	  SAFE_SL_FLUSH_SLIST(tib->list_to_try);
	  return analyze_node_and_execute_intention_body(tib); /* If this is the last node, then the first test in this function will catch it. */
     } else {			/* The goal has not been failed nor achieved... we still have "options". */
	  if (tib->maintain_condition) {
	       desinstall_and_free_condition(tib->maintain_condition);
	       tib->maintain_condition = NULL;
	       tib->maintain = NULL;
	  }
	  add_goal(tib->current_goal, current_oprs); /* We repost it damnit. */
	  return TER_EXECUTING;
     }
}

Thread_Execution_Result execute_top_thread(Thread_Intention_Block *tib)
{
 
     Op_Structure *op;
     Thread_Execution_Result result;
#ifdef OPRS_PROFILING
     PDate pd;
     Intention *in;
#endif /* OPRS_PROFILING */

     current_tib = tib; 

     if (tib->status != TIBS_ACTIVE) {
	  fprintf(stderr,LG_STR("ERROR: execute_top_thread: Executing an inactive TIB.\n",
				"ERREUR: execute_top_thread: Exécute un TIB non actif.\n"));
     }

#ifdef OPRS_PROFILING
     TIME_STAMP(&pd);
     in = tib->intention;	/* tib may be freed before the end... */
#endif /* OPRS_PROFILING */

     op = tib->curr_op_inst->op;
     if (op == goal_satisfied_in_db_op_ptr) /* Satisfied already in the DB */
	  result = execute_intention_goal_satisfied_in_db(tib);
     else if (op == goal_waiting_op_ptr) /* Waiting goal */
	  result = execute_intention_goal_waiting(tib);
     else if (op->action != NULL) /*Action OP */
	  result = execute_intention_action(tib);
     else result = execute_intention_body(tib); /* Standard Body OP... */

#ifdef OPRS_PROFILING
     TIME_ADD(&(in->active),&pd);
#endif /* OPRS_PROFILING */

     current_tib = NULL; 
     
     return(result);
}

Thread_Execution_Result execute_active_thread_list(Thread_Intention_Block_List tibl, Thread_Intention_Block_List orig_tibl)
{
     Thread_Intention_Block *tib;
     Thread_Execution_Result tmp, result = TER_EXECUTING;
     PBoolean exit = FALSE;
     PBoolean one_done = FALSE;
     Intention *in = NULL;
#ifdef GRAPHIX
     int i = 0;
#endif

     sl_loop_through_slist(tibl, tib, Thread_Intention_Block *) {

	  if (!in) in = TIB_INTENTION(tib);

#ifdef GRAPHIX
	  if (i++ == 5) {
	       i = 0;
	       process_xt_events();
	  }
#endif

	  if (! (sl_in_slist(orig_tibl,tib))) break; /* Has been deleted since we created the list. */

	  tmp = execute_top_thread(tib);

	  switch (tmp) {
	  case TER_OP_SUCCESS:	/* Real success... */
	  case TER_OP_FAILURE:
	       exit = TRUE;
	       result = tmp;
	       break;
	  case TER_EXECUTING:
	       one_done = TRUE;
	       break;
	  case TER_ACTION_WAIT:
	  case TER_WAITING:
	  case TER_JOINING:
	       break;
	  default:
	       fprintf(stderr,LG_STR("ERROR: execute_active_thread_list: Reaching an unallowed case in switch.\n",
				     "ERREUR: execute_active_thread_list: cas non prévu.\n"));
	       break;
	  }
	  if (exit || (!run_option[PAR_POST] && one_done)) break;
	  else if (in->critical_section) {	
	       result =  TER_EXECUTING;
	       break;
	  } else 
	       result =  TER_EXECUTING;
     }
     
     return result;
}

Thread_Intention_Block_List copy_and_shuffle_tib_list(Thread_Intention_Block_List l)
{
    Thread_Intention_Block *tib;
    Thread_Intention_Block_List res = sl_make_slist();
    
    sl_loop_through_slist(l,  tib, Thread_Intention_Block *) 
	 if (RANDOM() & 16) 
	      sl_add_to_head(res, tib);
	 else
	      sl_add_to_tail(res, tib);

    return res;
}

Thread_Execution_Result execute_intention(Intention *in)
/* execute_intention - is the top level execute intention function. According to the type of the op
			it will call the appropriate execute intention function. */
{
     Thread_Execution_Result result;
     Thread_Intention_Block_List tib_list;

     current_intention = in;

     if (in->critical_section) {
	  tib_list = sl_make_slist();
	  sl_add_to_head(tib_list, in->critical_section);
     } else 
	  tib_list= copy_and_shuffle_tib_list(in->active_tibs);     
     result = execute_active_thread_list(tib_list, in->active_tibs);
     FREE_SLIST(tib_list);

     current_intention = NULL;

     return result;
}

void add_in_active_tib(Thread_Intention_Block *tib)
{
     sl_add_to_head(TIB_INTENTION(tib)->active_tibs, tib);
}

void remove_from_active_tib(Thread_Intention_Block *tib)
{
     sl_delete_slist_node(TIB_INTENTION(tib)->active_tibs, tib);
}

void check_last_active_tib(Thread_Intention_Block *tib)
{
     if (sl_slist_empty(TIB_INTENTION(tib)->active_tibs)) {
	  /* There is no more active threads, suspend the intention. */
	  suspend_intention(TIB_INTENTION(tib));
     }
}

void continue_tib_list(Thread_Intention_Block_List list);

void continue_tib(Thread_Intention_Block *tib)
{
     Thread_Intention_Block_List list;

     list = tib->fils;
     if (! (sl_slist_empty(list))) {
	  if (tib->status != TIBS_SUSP_ACTIVE)
	       fprintf(stderr,LG_STR("ERROR: continue_tib: Bizare, status should be TIBS_SUSP_ACTIVE.\n",
				     "ERREUR: continue_tib: Bizarre, état devrait être TIBS_SUSP_ACTIVE.\n"));
	  continue_tib_list(list);
	  tib->status = TIBS_ACTIVE;
     } else {/* no sons */
	  switch(tib->status) {
	  case TIBS_JOINING:   /* do nothing */
	       break;
	  case TIBS_SUSP_ACTIVE: 
	       tib->status = TIBS_ACTIVE;
	       add_in_active_tib(tib);
	       break;
	  case TIBS_SUSP_SLEEPING: 
	       tib->status = TIBS_SLEEPING;
	       break;
	  case TIBS_ACTIVE: 
	  case TIBS_SLEEPING:
	       fprintf(stderr,LG_STR("ERROR: continue_tib: Inappropriate tib status.\n",
				     "ERREUR: continue_tib: tib status inadéquat.\n"));
	       break;
	  default:
	       fprintf(stderr,LG_STR("ERROR: continue_tib: Unknown tib status.\n",
				     "ERREUR: continue_tib: tib status inconnu. continue_tib\n"));
	       break;
	  }
     }
}
void continue_tib_list(Thread_Intention_Block_List tib_list)
{
     Thread_Intention_Block *tib;

     sl_loop_through_slist(tib_list, tib, Thread_Intention_Block *) {
	  continue_tib(tib);
     }
}

void suspend_tib_list(Thread_Intention_Block_List list);

void suspend_tib(Thread_Intention_Block *tib)
{
    switch(tib->status) {
     case TIBS_ACTIVE: 
	  tib->status = TIBS_SUSP_ACTIVE;
	  break;
     case TIBS_SLEEPING:
	  tib->status = TIBS_SUSP_SLEEPING;
	  break;
     case TIBS_JOINING:  /* if it is joining , it already succeds don't suspend it */
	  break;
     case TIBS_SUSP_ACTIVE: 
     case TIBS_SUSP_SLEEPING: 
	  fprintf(stderr,LG_STR("ERROR: suspend_tib: Already suspended tib.\n",
				"ERREUR: suspend_tib: tib déjà suspendu.\n"));
	  break;
     default:
	  fprintf(stderr,LG_STR("ERROR: suspend_tib: Unknown tib status.\n",
				"ERREUR: suspend_tib: tib status inconnu.\n"));
	  break;
     }
     suspend_tib_list(tib->fils);
     remove_from_active_tib(tib);
}

void suspend_tib_list(Thread_Intention_Block_List tib_list)
{
     Thread_Intention_Block *tib;

     sl_loop_through_slist(tib_list, tib, Thread_Intention_Block *) {
	  suspend_tib(tib);
     }
}

void failed_maintained_tib(Thread_Intention_Block *tib)
{
     if (PRESERVE_P(tib->maintain)) { /* Passive preserve: fail the sons */
	  Thread_Intention_Block_List list;

	  list = tib->fils;
	  if (! (sl_slist_empty(list))) {
	       Op_Instance *op_ins;
	       Goal *goal;
	       Thread_Intention_Block *tib_child = (Thread_Intention_Block *) sl_get_slist_head(list);

	       /* All the brothers have the same op_instance, fail one. */
	       op_ins = tib_child->curr_op_inst;

	       if ((goal = op_ins->goal) != NULL) {
		    SAFE_SL_ADD_TO_HEAD(goal->echec,dup_op_instance(op_ins));

		    if (debug_trace[SUC_FAIL]) {
			 static Sprinter *sp = NULL;

			 if (!sp) sp = make_sprinter(0);
			 else reset_sprinter(sp);
			 sprint_expr(sp, goal->statement);
			 SPRINT(sp, 64 + strlen(op_ins->op->name),
				sprintf(f, LG_STR(" failed by OP %s,\
\n\tbecause the maintained condition became false.\n",
						  " a été échoué par le OP %s,\
\n\tparceque la condition de maintenance a échoué.\n"), op_ins->op->name));
#ifdef GRAPHIX
			 if (tib->intention->trace_dialog) 
			      xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
			 else 
#endif
			 printf("%s",SPRINTER_STRING(sp));
		    }
		    if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
			 user_trace_suc_fail(op_ins,goal,NULL,FALSE);

	       } else fprintf(stderr,LG_STR("ERROR: failed_maintained_tib: Expecting a goal in a maintained son.\n",
					    "ERREUR: failed_maintained_tib: But attendu dans un tib de maintenance.\n"));

	       /* We need to desinstall the environement because it was a failure */
	       desinstall_frame(TIB_FRAME(tib_child), TIB_FRAME(tib));
	       unbind_lenv(op_ins->lenv);

	       if (tib->intention->critical_section) {
		    remove_and_free_tib_list(list);
		    if ((current_oprs->critical_section) &&
			(tib->intention->critical_section == NULL)) { 
			 /* We are still in critical section and the critical thread was one of our sons. */
			 mark_critical_section(tib);
		    }
	       } else
		    remove_and_free_tib_list(list);

	       if (tib->status == TIBS_ACTIVE) {/* not suspended */
		    add_in_active_tib(tib); 
		    continue_intention(tib->intention); /* If the intention has no active thread previously. */
	       }
	  }
     } else if (MAINTAIN_P(tib->maintain)) { /* Active preserve: suspend the sons */

	  Thread_Intention_Block *new_tib  = split_thread_intention_block(tib);
	  Thread_Intention_Block *pere;
	  Intention *int_pere = NULL;

/* use this later
	  sl_add_to_tail((tib->pere ? tib->pere->fils : tib->intention->fils),
			 new_tib);
	  sl_delete_slist_node((tib->pere ? tib->pere->fils : tib->intention->fils),
			 tib);
 */
	  
	  pere = tib->pere;
	  if (pere) {
	       sl_replace_slist_node (pere->fils, tib, new_tib);
	  } else {
	       int_pere = tib->intention;

	       sl_replace_slist_node (int_pere->fils, tib, new_tib);
	  }

	  if (tib->status == TIBS_ACTIVE) {/* not suspended */
	       if (tib->intention->critical_section) {
		     /* the critical section do not shift facts : We may not restore the condition */
		    printf (LG_STR("WARNING: We may not restore the condition in critical section.\
\n\tForce exit critical section.\n",
				   "ATTENTION: La condition de maintance ne sera sans doute pas\
\n\trétablie en section critique.\
\n\tSortie forcée de la section critique.\n"));
		    end_critical_section(tib);
	       }
	       suspend_tib(tib);
	       add_in_active_tib(new_tib);                
	       continue_intention(new_tib->intention); /* If the intention has no active thread previously. */
	  } 
	  tib->current_goal->echoue = ACT_MAINT_FAILURE;

     } else {
	  fprintf(stderr,LG_STR("ERROR: failed_maintained_tib: Bad temporal operator in the maintain texpr.\n",
				"ERREUR: failed_maintained_tib: Opérateur temporel inconnu dans la TEXPR de maintenance.\n"));
     }
}

void restore_suspended_tib(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *susp_tib = tib->suspended;

     Thread_Intention_Block *pere;
     Intention *int_pere = NULL;

     if (! susp_tib ) {
	  fprintf(stderr,LG_STR("ERROR: restore_suspended_tib: Nothing to restore.\n",
				"ERREUR: restore_suspended_tib: Rien à restorer.\n"));
	  return;
     }
     if (!MAINTAIN_P(susp_tib->maintain)) { 
	  fprintf(stderr,LG_STR("ERROR: restore_suspended_tib: Bad temporal operator in the maintain texpr.\n",
				"ERREUR: restore_suspended_tib: Opérateur temporel inconnu dans la TEXPR de maintenance.\n"));
	  return;
     }

     /* Active preserve: restore the suspended tib */

     pere = tib->pere;
     if (pere) {
	  sl_replace_slist_node (pere->fils, tib, susp_tib);
     } else {
	  int_pere = tib->intention;
	  
	  sl_replace_slist_node (int_pere->fils, tib, susp_tib);
     }

     continue_tib(susp_tib);
     susp_tib->current_goal->echoue = NOT_FAILED;
     susp_tib->maintain_condition = make_and_install_condition(MAINTAIN_ACTIVATION, NULL, susp_tib, EXPR_EXPR1(susp_tib->maintain), TIB_FRAME(susp_tib));

     /* delete this one */
     remove_from_active_tib(tib);
     free_thread_intention_block(tib);
     
     /* All the threads have been restored, check if there is an active one */
     check_last_active_tib(susp_tib);


}

void restore_and_fail_suspended_tib(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *susp_tib = tib->suspended;

     Thread_Intention_Block_List list;
     
     Thread_Intention_Block *pere;
     Intention *int_pere = NULL;

     if (! susp_tib ) {
	  fprintf(stderr,LG_STR("ERROR: restore_and_fail_suspended_tib: Nothing to restore.\n",
				"ERREUR: restore_and_fail_suspended_tib: Rien à restorer.\n"));
	  return;
     }
     if (!MAINTAIN_P(susp_tib->maintain)) { 
	  fprintf(stderr,LG_STR("ERROR: restore_and_fail_suspended_tib: Bad temporal operator in the maintain texpr.\n",
				"ERREUR: restore_and_fail_suspended_tib: Opérateur temporel inconnu dans la TEXPR de maintenance.\n"));
	  return;
     }

     /* Active preserve: restore and fail the suspended tib */

     pere = tib->pere;
     if (pere) {
	  sl_replace_slist_node (pere->fils, tib, susp_tib);
     } else {
	  int_pere = tib->intention;

	  sl_replace_slist_node (int_pere->fils, tib, susp_tib);
     }

     list = susp_tib->fils;
     if (! (sl_slist_empty(list))) {
	  Op_Instance *op_ins;
	  Goal *goal;
	  Thread_Intention_Block *tib_child = (Thread_Intention_Block *) sl_get_slist_head(list);

	  /* All the brothers have the same op_instance, fail one. */
	  op_ins = tib_child->curr_op_inst;

	  if ((goal = op_ins->goal) != NULL) {
	       SAFE_SL_ADD_TO_HEAD(goal->echec,dup_op_instance(op_ins));
	       
	       if (debug_trace[SUC_FAIL]) {
		    static Sprinter *sp = NULL;

		    if (!sp) sp = make_sprinter(0);
		    else reset_sprinter(sp);
		    sprint_expr(sp, goal->statement);
		    SPRINT(sp,16 + strlen(op_ins->op->name),
			   sprintf(f, LG_STR(" failed by %s,\n",
					     " échoué par %s,\n"), op_ins->op->name));
		    SPRINT(sp, 64,
			   sprintf(f, LG_STR("because it fails to restore the maintained condition.\n",
					     "parcequ'il a échouer à restorer la condition de maintenance.\n")));
#ifdef GRAPHIX
		    if (tib->intention->trace_dialog) 
			 xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
		    else 
#endif
		    printf("%s",SPRINTER_STRING(sp));
	       }
	       if (user_trace[USER_TRACE] && user_trace[UT_SUC_FAIL])
		    user_trace_suc_fail(op_ins,goal,NULL,FALSE);

	  } else fprintf(stderr,LG_STR("ERROR: restore_and_fail_suspended_tib: Expecting a goal in a maintained son.\n",
				       "ERREUR: restore_and_fail_suspended_tib:  But attendu dans un tib de maintenance.\n"));

	  /* We need to desinstall the environement because it was a failure */
	  desinstall_frame(TIB_FRAME(tib_child), TIB_FRAME(susp_tib));
	  unbind_lenv(op_ins->lenv);

	  remove_and_free_tib_list(list);
     }
     continue_tib(susp_tib);
     susp_tib->current_goal->echoue = NOT_FAILED;

     /* delete this one */
     remove_from_active_tib(tib);
     free_thread_intention_block(tib);
}



