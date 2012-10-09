
/*                               -*- Mode: C -*- 
 * oprs.c -- The big boss
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

#include <sys/socket.h>

#include <netinet/in.h>

#ifdef VXWORKS
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef WIN95
#else
#include <unistd.h>
#ifdef VXWORKS
#else
#include <netdb.h>
#endif
#endif


#include "fact-goal.h"
#include "oprs.h"
#include "soak.h"
#include "oprs-main.h"
#include "oprs-pred-func.h"

#include "top-lev.h"


#include "mp-register_f.h"
#include "int-graph_f.h"
#include "relevant-op_f.h"
#include "fact-goal_f.h"
#include "unification_f.h"
#include "oprs_f.h"
#include "database_f.h"
#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#ifndef WINSOCK
#endif
#include "oprs-send-m_f.h"
#include "oprs-pred-func_f.h"
#include "lisp-list_f.h"
#include "top-lev_f.h"
#include "int-graph_f.h"
#include "conditions_f.h"
#include "user-trace_f.h"
#include "type_f.h"

Oprs *current_oprs;

#ifdef VXWORKS
static SEM_ID add_fact_sem = NULL;
#endif     

int ps_socket;

void declare_predicat()
{
     declare_pred_from_symbol(soak_sym);	/* Declare the soak symbol */
     declare_pred_from_symbol(app_ops_fact_sym);
     declare_pred_from_symbol(fact_invoked_ops_sym);
     declare_pred_from_symbol(db_satisf_goal_sym);
     declare_pred_from_symbol(app_ops_goal_sym);
     declare_pred_from_symbol(goal_invoked_ops_sym);
     declare_pred_from_symbol(failed_goal_sym);
     declare_pred_from_symbol(failed_sym);
     declare_pred_from_symbol(request_sym);
     declare_pred_from_symbol(achieved_sym);
     declare_pred_from_symbol(wake_up_intention_sym);
     declare_pred_from_symbol(read_response_id_sym);
     declare_pred_from_symbol(read_response_sym);
     declare_pred_from_symbol(or_sym);
     declare_pred_from_symbol(and_sym);
     declare_pred_from_symbol(not_sym);
     declare_pred_from_symbol(cond_add_sym);
     declare_pred_from_symbol(cond_del_sym);
     declare_pred_from_symbol(temp_conclude_sym);
     declare_pred_from_symbol(retract_sym);
     declare_pred_from_symbol(achieve_sym);
     declare_pred_from_symbol(test_sym);
     declare_pred_from_symbol(preserve_sym);
     declare_pred_from_symbol(twait_sym);
     declare_pred_from_symbol(maintain_sym);
     declare_pred_from_symbol(call_sym);
}

void declare_be_pred()
{
     declare_be(soak_pred = declare_pred_from_symbol(soak_sym));
     declare_be(app_ops_fact_pred = declare_pred_from_symbol(app_ops_fact_sym));
     declare_be(fact_invoked_ops_pred = declare_pred_from_symbol(fact_invoked_ops_sym));
     declare_be(db_satisf_goal_pred = declare_pred_from_symbol(db_satisf_goal_sym));
     declare_be(app_ops_goal_pred = declare_pred_from_symbol(app_ops_goal_sym));
     declare_be(goal_invoked_ops_pred = declare_pred_from_symbol(goal_invoked_ops_sym));
     declare_be(failed_goal_pred = declare_pred_from_symbol(failed_goal_sym));
     declare_be(failed_pred = declare_pred_from_symbol(failed_sym));
     declare_be(request_pred = declare_pred_from_symbol(request_sym));
     declare_be(achieved_pred = declare_pred_from_symbol(achieved_sym));
     declare_be(wake_up_intention_pred = declare_pred_from_symbol(wake_up_intention_sym));
     declare_be(read_response_id_pred = declare_pred_from_symbol(read_response_id_sym));
     declare_be(read_response_pred = declare_pred_from_symbol(read_response_sym));
}

void free_oprs(Oprs *oprs)
{
     Type *t;

     reset_oprs_kernel(oprs);
     
     sl_loop_through_slist(oprs->types, t, Type *) {
	  free_type(t);
     }
     FREE_SLIST(oprs->types);

     free_database(oprs->database);
     free_intention_graph(oprs->intention_graph);
     free_relevant_op(oprs->relevant_op);
     free_conditions_list(oprs->conditions_list);

#ifdef VXWORKS
     take_or_create_sem(&add_fact_sem);
#endif     
     FREE_SLIST(oprs->new_facts);
#ifdef VXWORKS
     give_sem(add_fact_sem);
#endif     

     FREE_SLIST(oprs->locked_messages);
     FREE_SLIST(oprs->facts);

     FREE_SLIST(oprs->new_goals);
     FREE_SLIST(oprs->goals);

     FREE_SLIST(oprs->concluded_be);
     if (oprs->included_file_name) 
         FREE_SLIST(oprs->included_file_name);
     
     FREE_OBJECT(oprs);
}

char *kernel_name()
{
     return current_oprs->name;
}

Oprs *make_oprs(PString name)
{
/* 
 * make_oprs - Create and return a oprs object named "name".
 *            It creates all the necessary objects present in a OPRS agent.
 */
     Oprs *oprs=MAKE_OBJECT(Oprs);

     oprs->name = name;
     oprs->types = sl_make_slist();

     oprs->database =  make_database();

     oprs->posted_meta_fact = FALSE;
     oprs->critical_section = FALSE;
     oprs->intention_graph = make_intention_graph();
     oprs->relevant_op =  make_relevant_op(); 
     oprs->new_facts = sl_make_slist();
     oprs->locked_messages = sl_make_slist();
     oprs->facts = sl_make_slist();
     oprs->new_goals = sl_make_slist();
     oprs->goals = sl_make_slist();
     oprs->conditions_list = sl_make_slist();
     oprs->concluded_be = sl_make_slist();
     oprs->included_file_name = NULL;
     return oprs;
}

void send_message_term(Expression *exp, PString rec)
{
     static Sprinter *message = NULL;
     PBoolean save_pvv =  print_var_value;

     if (! message) 
	  message = make_sprinter(BUFSIZ);
     else
	  reset_sprinter(message);

     print_var_value = TRUE;
     sprint_expr(message, exp);
     print_var_value = save_pvv;

     oprs_send_message_string(SPRINTER_STRING(message),rec);
}

void multicast_message_term(Expression *exp, unsigned int nb_recs, PString *recs)
{
     static Sprinter *message = NULL;
     PBoolean save_pvv =  print_var_value;


     if (! message) 
	  message = make_sprinter(BUFSIZ);
     else
	  reset_sprinter(message);

     reset_sprinter(message);
     print_var_value = TRUE;
     sprint_expr(message, exp);
     print_var_value = save_pvv;

     oprs_multicast_message_string(SPRINTER_STRING(message), nb_recs, recs);
}

void broadcast_message_term(Expression *exp)
{
     static Sprinter *message = NULL;
     PBoolean save_pvv =  print_var_value;


     if (! message) 
	  message = make_sprinter(BUFSIZ);
     else
	  reset_sprinter(message);

     reset_sprinter(message);
     print_var_value = TRUE;
     sprint_expr(message, exp);
     print_var_value = save_pvv;

     oprs_broadcast_message_string(SPRINTER_STRING(message));
}

void send_message(Fact *fact, PString rec)
{
     send_message_expr(fact->statement,rec);
}

void get_message(Expression *expr, Oprs *oprs, PString sender)
{
     Fact *fact;
     if (debug_trace[RECEIVE_MESSAGE]) {
	  printf(LG_STR("%s: getting the message: ",
			"%s: getting the message: "), oprs_prompt);
	  print_expr(expr);
	  printf(LG_STR(" from %s.\n",
			" from %s.\n"), sender);
     }

     fact = make_fact_from_expr(expr);
     fact->message_sender = sender;

     if (user_trace[USER_TRACE] && user_trace[UT_RECEIVE_MESSAGE])
	  user_trace_receive_message(fact,sender);

     if (oprs->critical_section) { /* we do not conclude messages in critical section. */
	  sl_add_to_tail(oprs->locked_messages, fact);
     } else {

#ifdef VXWORKS
	  take_or_create_sem(&add_fact_sem);
#endif     
	  sl_add_to_tail(oprs->new_facts,fact);
#ifdef VXWORKS
	  give_sem(add_fact_sem);
#endif     
     }
}

TermList make_external_term_list(int nb_arg, ...)
{
     va_list list_arg;
     int param_cour;
     TermList param_list = sl_make_slist();

     va_start(list_arg, nb_arg);
  
     for(param_cour = 1; param_cour <= nb_arg; param_cour++) {
	  Term_Type type;
	  type = va_arg(list_arg, Term_Type);

	  switch(type) {
	  case TT_FLOAT:
	       add_to_tail(param_list, build_float(va_arg(list_arg, double)));
	       break;
	  case INTEGER:
	       add_to_tail(param_list, build_integer(va_arg(list_arg, int)));
	       break;
	  case LONG_LONG:
	       add_to_tail(param_list, build_long_long(va_arg(list_arg, long long int)));
	       break;
	  case STRING:
	       add_to_tail(param_list, build_string(va_arg(list_arg, char *)));
	       break;
	  case EXPRESSION:
	       add_to_tail(param_list, build_term_expr(va_arg(list_arg, Expression *)));
	  case U_POINTER:
	       add_to_tail(param_list, build_pointer(va_arg(list_arg, void *)));
	       break;
	  case TT_ATOM:
	       add_to_tail(param_list, 
			   build_id(find_atom(va_arg(list_arg, char *))));
	       break;
	  case LISP_LIST:
	       add_to_tail(param_list, build_l_list(va_arg(list_arg,  L_List)));
	       break;
	  case U_MEMORY:
	  default:
	       fprintf(stderr,"make_external_term_list: Unknown/Unsupported type.\n");
	       va_end(list_arg);
	       return param_list;
	  }
     }
  
     va_end(list_arg);
     return param_list;
}

void add_external_fact(char *predicat, TermList param_list)
{
     Pred_Func_Rec *pred = find_or_create_pred_func(declare_atom(predicat));

     add_fact(make_fact_from_expr
	      (build_expr_pfr_terms(pred,param_list))
	      ,current_oprs);
}

void add_external_goal(External_Temporal_Operator_Type etot, char *predicat, TermList param_list)
{
     Symbol to; 

     switch (etot) {
     case ETOT_ACHIEVE: 
	  to = achieve_sym;
	  break;
     case ETOT_CALL: 
	  to = call_sym;
	  break;
     case ETOT_TEST:
	  to = test_sym;
	  break;
     case ETOT_CONCLUDE:
	  to = temp_conclude_sym;
	  break;
     case ETOT_RETRACT:
	  to = retract_sym;
	  break;
     default:
	  fprintf(stderr,LG_STR("Unknown external temporal operator in add_external_goal.\n",
				"Operator temporel externe inconnu dans add_external_goal.\n"));
	  return;

     }

     add_goal(make_goal_from_expr(build_expr_sym_expr(to,
						      build_expr_sym_terms(predicat,
									   param_list)),
				  empty_env)
	      ,current_oprs);
}

L_List make_external_lisp_list(TermList param_list)
{
     return  transform_c_list_to_l_list(param_list);
}

Expression *make_external_expr(char *function, TermList param_list)
{
     Pred_Func_Rec *func = find_or_create_pred_func(declare_atom(function));

     return build_expr_pfr_terms(func,param_list);
}

void add_expr(Expression *expr, Oprs *oprs)
{
     
     if (expr_temporal_type(expr) || expr_db_type(expr)) 
	  add_goal(make_goal_from_expr(expr, empty_env), oprs);
     else 
	  add_fact(make_fact_from_expr(expr), oprs); 
}

void add_fact(Fact *fact, Oprs *oprs)
/* 
 * add_fact - Add a fact pointed by "fact" to a oprs pointed by "oprs". Does 
 *            not return anything. 
 */
{

     if (debug_trace[POST_FACT]) {
	  printf(LG_STR("Posting the fact: ",
			"Posting the fact: "));
	  print_expr(fact->statement);
	  printf("\n");
     }
     
     if (user_trace[USER_TRACE] && user_trace[UT_POST_FACT])
	  user_trace_post_fact(fact);

#ifdef VXWORKS
     take_or_create_sem(&add_fact_sem);
#endif     
     sl_add_to_tail(oprs->new_facts,fact);
#ifdef VXWORKS
     give_sem(add_fact_sem);
#endif     
}

void add_goal(Goal *goal, Oprs *oprs)
/* 
 * add_goal - Add a goal pointed by "goal" to a oprs pointed by "oprs". 
 *            Return void.
 */
{
     Temporal_Ope tope;
     DB_Ope dope;

     if (goal->succes) return;	/* If it is already satisfied (for dummy edges). */

     if (debug_trace[POST_GOAL]) {
	  printf(LG_STR("Posting the goal: ",
			"Posting the goal: "));
	  print_expr(goal->statement);
	  printf("\n");
     }
     
     if (user_trace[USER_TRACE] && user_trace[UT_POST_GOAL])
	  user_trace_post_goal(goal);

     /* If the goal is a conclude or delete goal, we do not post the goal,
	but post or delete the fact. */

     dope = expr_db_type(goal->statement);
     tope = expr_temporal_type(goal->statement);

     if (dope) {
	  switch (dope) {
	       /* For these 2, I satisfy them right away. */
	  case DB_CONC: {
	  Expression *expr2;
#ifdef  OPRS_PROFILING
	       TIME_STOP(&goal->creation,T_GOAL_CR_REC);
	       TIME_STAMP(&goal->reception);
#endif /* OPRS_PROFILING */
	       goal->pris_en_compte = TRUE;
#ifdef  OPRS_PROFILING
	       TIME_STOP(&goal->creation,T_GOAL_CR_SOAK);
	       TIME_STOP(&goal->reception,T_GOAL_REC_SOAK);
	       TIME_STAMP(&goal->soak);	/* for statistics */
#endif /* OPRS_PROFILING */

	       expr2 = find_binding_expr(EXPR_EXPR1(goal->statement));
	       add_fact(make_fact_from_expr(expr2), oprs);
	       /* It will be concluded automatically next loop. */
	       goal->echoue = NOT_FAILED; /* This is not enough... we need to set succes but not op avail... */
	       goal->succes = (Op_Instance *)TRUE;
#ifdef  OPRS_PROFILING
	       TIME_STOP(&goal->creation,T_GOAL_CR_REP);
	       TIME_STOP(&goal->reception,T_GOAL_REC_REP);
	       TIME_STOP(&goal->soak,T_GOAL_SOAK_REP);
	       TIME_STAMP(&goal->reponse);
#endif /* OPRS_PROFILING */
	       break;
	  }
	  case DB_RET: {
	  Condition_List cond_tmp;
	  Relevant_Condition *rc;
	  Pred_Rec *pr;

#ifdef  OPRS_PROFILING
	       TIME_STOP(&goal->creation,T_GOAL_CR_REC);
	       TIME_STAMP(&goal->reception);
#endif /* OPRS_PROFILING */
	       goal->pris_en_compte = TRUE;
#ifdef  OPRS_PROFILING
	       TIME_STOP(&goal->creation,T_GOAL_CR_SOAK);
	       TIME_STOP(&goal->reception,T_GOAL_REC_SOAK);
	       TIME_STAMP(&goal->soak);	/* for statistics */
#endif /* OPRS_PROFILING */

		    delete_fact_expr(EXPR_EXPR1(goal->statement), oprs->database);

		    /* Need to remember which condition may be changed by this retraction */

		    pr = PFR_PR(EXPR_EXPR1(goal->statement)->pfr);

		    cond_tmp = (pr?pr->conditions:NULL);

		    SAFE_SL_LOOP_THROUGH_SLIST(cond_tmp, rc, Relevant_Condition *) {
			 if (! (SAFE_SL_IN_SLIST(cond_force_check,rc)))
			      SAFE_SL_ADD_TO_HEAD(cond_force_check,rc);
		    }

		    goal->echoue = NOT_FAILED;
		    goal->succes = (Op_Instance *)TRUE;
#ifdef  OPRS_PROFILING
		    TIME_STOP(&goal->creation,T_GOAL_CR_REP);
		    TIME_STOP(&goal->reception,T_GOAL_REC_REP);
		    TIME_STOP(&goal->soak,T_GOAL_SOAK_REP);
		    TIME_STAMP(&goal->reponse);
#endif /* OPRS_PROFILING */
	       break;
	       }
	  case DB_COND_CONC:
	  case DB_COND_RET:
	  default:
	       break;
	  }
     } else if (tope) {
	  switch(tope) {

	  case TEST:		
	  case WAIT:
	  case ACHIEVE:
	  case CALL:
	  case ACT_MAINT:
	  case PASS_MAINT:
	       sl_add_to_tail(oprs->new_goals,dup_goal(goal)); 
	       /* For goals, we have to dup them... because most of them come from */
	       /* the system itself and are pointed by other structure. */
	       break;
	  }
     } else { /* We only accept the AND operator on conlude or retract goals,
		 or a preserve goal : a maintenance and an achieve goal.*/
	  if (AND_P(goal->statement))
	  {
	       PBoolean accepted_ltexpr = TRUE;
	       PBoolean conc_or_retract = FALSE;
	       PBoolean one_achieve = FALSE;
	       PBoolean one_maint = FALSE;
	       Expression *expr;
	       Term *term;

	       sl_loop_through_slist(EXPR_TERMS(goal->statement), term, Term *) {
		    expr = TERM_EXPR(term);
		    dope = expr_db_type(expr);
		    tope = expr_temporal_type(expr);
		    if (dope) {
			 switch (dope) {
			      /* For these 2, I satisfy them right away. no AOP. */
			 case DB_CONC:
			      conc_or_retract = TRUE;
			      break;
			 case DB_RET:
			      conc_or_retract = TRUE;
			      break;
			 case DB_COND_CONC:
			 case DB_COND_RET:
			 default:
			      break;
			 }
		    }else if (tope) {
			 switch (tope) {
			 case TEST:		
			 case WAIT:
			 case CALL:
			      fprintf(stderr,LG_STR("You cannot use this temporal operator in a LTEXPR in add_goal ...\n",
						    "You cannot use this temporal operator in a LTEXPR in add_goal ...\n"));
			      accepted_ltexpr = FALSE;
			      break;
			 case ACHIEVE:
			      if (one_achieve) {
				   accepted_ltexpr = FALSE;
			      } else
				   one_achieve = TRUE;
			      break;
			 case ACT_MAINT:
			 case PASS_MAINT:
			      if (one_maint) {
				   accepted_ltexpr = FALSE;
			      } else
				   one_maint = TRUE;
			      break;
			 }
		    } else {
			 fprintf(stderr,LG_STR("type of goal must be TEXPR in add_goal\n",
					       "type of goal must be TEXPR in add_goal\n"));
			 accepted_ltexpr = FALSE;
		    }
	       }
	       if (accepted_ltexpr) { /* some other verifications */
		    if ((conc_or_retract && (one_maint || one_achieve))
			|| (one_maint && (! one_achieve)) 
			|| ( (! one_maint) && one_achieve)) {

			 accepted_ltexpr = FALSE;
		    }
	       }
	       if (accepted_ltexpr) {
		    if (! conc_or_retract) 
			 sl_add_to_tail(oprs->new_goals,dup_goal(goal)); 
		    else {                /* the ltexpr is correct , we can conclude or retract all the facts */
#ifdef  OPRS_PROFILING
			 TIME_STOP(&goal->creation,T_GOAL_CR_REC);
			 TIME_STAMP(&goal->reception);
#endif /* OPRS_PROFILING */
			 goal->pris_en_compte = TRUE;
#ifdef  OPRS_PROFILING
			 TIME_STOP(&goal->creation,T_GOAL_CR_SOAK);
			 TIME_STOP(&goal->reception,T_GOAL_REC_SOAK);
			 TIME_STAMP(&goal->soak);	/* for statistics */
#endif /* OPRS_PROFILING */
			 sl_loop_through_slist(EXPR_TERMS(goal->statement), term, Term *) {
			      expr = TERM_EXPR(term);
			      dope = expr_db_type(expr);
			      switch (dope) {
			      case DB_CONC: {
				   Expression *expr2;
				   expr2 = find_binding_expr(EXPR_EXPR1(expr));
				   add_fact(make_fact_from_expr(expr2), oprs);
				   break;
			      }
			      case DB_RET:{ 
				   Condition_List cond_tmp;
				   Relevant_Condition *rc;

				/* There is a bug here... this will not wake up conditions.. */
				   delete_fact_expr(EXPR_EXPR1(expr), oprs->database);
				/* Need to remember which condition may be changed by this retraction */
				   cond_tmp =  PFR_PR(EXPR_EXPR1(expr)->pfr)->conditions;

				   SAFE_SL_LOOP_THROUGH_SLIST(cond_tmp, rc, Relevant_Condition *) {
					if (! (SAFE_SL_IN_SLIST(cond_force_check,rc)))
					     SAFE_SL_ADD_TO_HEAD(cond_force_check,rc);
				   }
			      }
			      break;
			      default:
				   fprintf(stderr,LG_STR("Serious error in add_goal (ltexpr). \n",
							 "Serious error in add_goal (ltexpr). \n"));
				   break;
			      }
			 }
			 goal->echoue = NOT_FAILED;
			 goal->succes = (Op_Instance *)TRUE;
#ifdef  OPRS_PROFILING
			 TIME_STOP(&goal->creation,T_GOAL_CR_REP);
			 TIME_STOP(&goal->reception,T_GOAL_REC_REP);
			 TIME_STOP(&goal->soak,T_GOAL_SOAK_REP);
			 TIME_STAMP(&goal->reponse);
#endif /* OPRS_PROFILING */
		    }
	       } else {
		    fprintf(stderr,LG_STR("You can either use a conjonction of conclude and retract goals,\n or the conjonction of one achieve goal and one maintenance goal !\n",
					  "You can either use a conjonction of conclude and retract goals,\n or the conjonction of one achieve goal and one maintenance goal !\n"));

		    goal->echoue = FORBIDDEN_SYNTAX;
	       }
	  } else {
	       fprintf(stderr,LG_STR("The type the goal should be TEXPR, or an AND LTEXPR in add_goal\n",
				     "The type the goal should be TEXPR, or an AND LTEXPR in add_goal\n"));
	       goal->echoue = FORBIDDEN_SYNTAX;
	  }
     }
}

void shift_facts_goals(Oprs *oprs)
/* 
 * shift_facts_goals - This function shifts the new-facts/goals lists in the 
 *                     facts/goals lists. 
 *                     Return void.
 */
{
     Fact *fact;
     Goal *goal;
     Slist *tmp;

     sl_loop_through_slist(oprs->facts, fact, Fact *)
	  free_fact(fact);
     FLUSH_SLIST(oprs->facts);	/* will empty it. */

     tmp = oprs->facts;		/* save this empty list */

#ifdef VXWORKS
     take_or_create_sem(&add_fact_sem);
#endif     
     oprs->facts = oprs->new_facts;
     oprs->new_facts = tmp;	/* point to the empty list */
#ifdef VXWORKS
     give_sem(add_fact_sem);
#endif     
     if ((! oprs->critical_section) && (! sl_slist_empty(oprs->locked_messages))) {
	  sl_concat_slist(oprs->facts,oprs->locked_messages);
	  oprs->locked_messages = sl_make_slist();
     }

#ifdef OPRS_PROFILING
     sl_loop_through_slist(oprs->facts, fact, Fact *){
	  TIME_STOP(&fact->creation,T_FACT_CR_REC);
	  TIME_STAMP(&fact->reception);
     }
#endif /* OPRS_PROFILING */

     sl_loop_through_slist(oprs->goals, goal, Goal *)
	 free_goal(goal);
     FLUSH_SLIST(oprs->goals);	

     tmp = oprs->goals;
     oprs->goals = oprs->new_goals;
     oprs->new_goals = tmp;
#ifdef OPRS_PROFILING
     sl_loop_through_slist(oprs->goals, goal, Goal *){
	  TIME_STOP(&goal->creation,T_GOAL_CR_REC);
	  TIME_STAMP(&goal->reception);
     }
#endif /* OPRS_PROFILING */
}

void init_client_socket(char *name, PBoolean use_x_window, char *server_hostname, int server_port)
/*
 * init_client_socket - This function create a socket, on SOCKET_ADDRESS, connect it and 
 *                      then will dup2 it on stdin (not anymore).
 *                      Return void.
 */
{
     int port = server_port;
     struct sockaddr_in socket_addr;
#ifdef VXWORKS
     int getpid(void);
     int host_addr;
#else
     struct hostent *host_entry; /* host entry */
#endif
     int len, size_message;
     char *message;
     int pid = getpid();

     if ((ps_socket = socket(
			       AF_INET,
			       SOCK_STREAM, 0)) < 0) {
	  perror("init_client_socket: socket");
	  exit(1);
     }
     
     
#ifdef VXWORKS
     if ((host_addr =  hostGetByName(server_hostname)) == NULL) {
	  perror("init_client_socket: gethostbyname");
	  exit(1);
     }
     BZERO((char *)&socket_addr, sizeof(socket_addr));
     BCOPY((char *)&host_addr, (char *)&socket_addr.sin_addr,
	   sizeof(host_addr));
#else
     if ((host_entry = gethostbyname(server_hostname)) == NULL) {
	  perror("init_client_socket: gethostbyname");
	  exit(1);
     }
     BZERO(&socket_addr, sizeof(socket_addr));
     BCOPY(host_entry->h_addr, &socket_addr.sin_addr, host_entry->h_length);
#endif
     socket_addr.sin_family = AF_INET;
     socket_addr.sin_port = htons(port);

     len = sizeof(socket_addr);

     if (connect(ps_socket,  (struct sockaddr *)&socket_addr, len) < 0) {
	  perror("client: connect");
	  exit(1);
     }

#ifdef HAS_FORK
     fcntl(ps_socket, F_SETFD, 1);      /* this is to avoid that the ns will be dup when we fork */
#endif

/* we wait for the father */

     message = read_string_from_socket(ps_socket,&size_message);
     free(message);
     
/* now, we can send our name, our pid and our type (child using X or not) */

     _write_string_to_socket(ps_socket,name);
     write_int_to_socket(ps_socket,pid);
     write_int_to_socket(ps_socket,( (int)(use_x_window)) );
     create_yy_stdin_and_ps();
}


Intention_Graph *oprs_intention_graph(Oprs *oprs)
{
	return oprs->intention_graph;
}

