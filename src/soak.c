/*                               -*- Mode: C -*- 
 * soak.c -- will find all the applicable ops (determine the famous soak "Set Of Applicable OPs")...
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

#ifdef HAS_TIMER
#ifdef VXWORKS
#include <vxWorks.h>
#include <systime.h>
#include <timers.h>
#include <sigLib.h>
#else
#include <sys/time.h>
#endif
#endif

#ifdef HAS_SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif

#include "constant.h"
#include "macro.h"


#ifdef OPRS_PROFILING
#include "user-external.h"
#endif


#include "oprs.h"
#include "oprs-type.h"
#include "oprs-type-pub.h"
#include "oprs-profiling.h"
#include "soak.h"
#include "int-graph.h"
#include "relevant-op.h"
#include "op-structure.h"
#include "op-instance.h"
#include "fact-goal.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-sprint.h"
#include "top-lev.h"
#include "conditions.h"

#include "oprs-profiling_f.h"
#include "oprs_f.h"
#include "soak_f.h"
#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "relevant-op_f.h"
#include "database_f.h"
#include "lisp-list_f.h"
#include "unification_f.h"
#include "op-instance_f.h"
#include "fact-goal_f.h"
#include "int-graph_f.h"
#include "activate_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"
#include "oprs-sprint_f.h"
#include "conditions_f.h"
#include "soak_f.h"
#include "oprs-pred-func_f.h"

#ifdef GRAPHIX
#include "xt-util_f.h"
#endif

/* Declaration of some functions */
#ifdef HAS_TIMER
void set_condition_timer(void);
#endif
FrameList parse_inv_part(Expression *expr, FramePtr fp, List_Envar lenv, Oprs *oprs);
FrameList parse_ip_lmexpr(Expression *expr, Frame *frame, Oprs *oprs);

Op_Structure *soak_parsing_op = NULL;

static PBoolean check_evolving_conditions = TRUE;

Expression *prepare_ip(Op_Structure *op, List_Envar lenv, Expression *trig)
{
     Expression *lmexpr;
     Expression *ip;
     ExprList gl = sl_make_slist();

     ip = subst_lenv_or_remove_expr_in_expr(op->invocation,lenv,trig);

     sl_add_to_tail(gl,ip);
     if (op->context != NULL) {
	  Expression *ctxt;
	  sl_loop_through_slist(op->context, ctxt, Expression *) {
	       if (expr_temporal_type(ctxt) || expr_db_type(ctxt)) 
		    if (TEST_P(ctxt)) {
			 sl_add_to_tail(gl,subst_lenv_in_expr(EXPR_EXPR1(ctxt),lenv,QET_NONE));
		    } else {
			 fprintf(stderr,"prepare_ip: this type of expression: ");
			 fprint_expr(stderr, ctxt);
			 fprintf(stderr," is not currently recognized.\n");
		    }
	       else 
		    sl_add_to_tail(gl,subst_lenv_in_expr(ctxt,lenv,QET_NONE));
	       
	  }
     }

     if (op->setting != NULL)
	  sl_add_to_tail(gl,subst_lenv_in_expr(op->setting,lenv,QET_NONE));

     lmexpr = build_expr_sym_exprs(and_sym,gl);

     return lmexpr;

}

Op_Instance_List find_aop_from_rop_from_fact(Oprs *oprs, Op_Structure *op, Expression *mexpr, Fact *fact, PBoolean trace_forced)
{
     FramePtr fp;
     FrameList fl;
     Expression *lmexpr;
     List_Envar lenv = copy_lenv(op->list_var);
     Expression *mexpr2  = subst_lenv_in_expr(mexpr,lenv,QET_NONE);
     Expression *statement = fact->statement;
     FramePtr frame =  empty_env;
     Op_Instance_List kil = sl_make_slist();

#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     if (unify_expr(mexpr2, statement, fp=make_frame(frame))) {
	  Frame *fr1;
				/* The frame is still installed and the OP unifie the triggering fact */
	  lmexpr = prepare_ip(op,lenv,mexpr); /* This make a copy of the whole thing except mexpr (already parsed). */
       	  fl = parse_inv_part(lmexpr, fp, lenv, oprs);
	  free_expr(lmexpr);	/* The IP has been parsed... free it... */
	  desinstall_frame(fp, frame);	
	  if (sl_slist_empty(fl)) {
	       free_lenv(lenv);
	  } else {
	       PBoolean first = TRUE;

	       sl_loop_through_slist(fl, fr1, Frame *) {
		    if (first) {
			 sl_add_to_head(kil,make_op_instance_from_fact(op,fr1,lenv,fact));
			 first = FALSE;
		    } else {
			 List_Envar lenv2 = copy_lenv(lenv); /* make a new lenv. */
			 
			 sl_add_to_head(kil,
				     make_op_instance_from_fact(op,
								subst_lenv_in_copy_of_frame_up_to_frame(fr1,
													lenv2,
													frame),
								lenv2,fact));
			 free_frame(fr1);
		    }
	       }
	  }

#ifdef  OPRS_PROFILING
	  TIME_STOP(&pd,T_APPLIC);
#endif /* OPRS_PROFILING */
	  if ((debug_trace[SOAK]) || trace_forced) {
	       Frame *fr;
	       printf(LG_STR("The frame(s) in which the OP %s with\n",
			     "Les frames dans lesquels le OP %s avec\n"), op->name);
	       printf(LG_STR("the invocation: ",
			     "l'invocation: ")); print_expr(op->invocation); printf("\n");
	       printf(LG_STR("the call: ",
			     "l'appel: ")); print_expr(op->call); printf("\n");
	       printf(LG_STR("the context: ",
			     "le context: ")); print_exprlist(op->context); printf("\n");
	       printf(LG_STR("the setting: ",
			     "le setting: ")); print_expr(op->setting); printf("\n");
	       printf(LG_STR("and the fact: ",
			     "et le fait: "));
	       print_expr(statement);
	       printf(LG_STR("are applicable:\n",
			     "sont applicables:\n"));
	       if (sl_slist_empty(fl)) printf("NULL\n");
	       sl_loop_through_slist(fl, fr, Frame *) {
		    print_frame(fr);
		    printf("\n");
	       }
	       printf("\n");
	  }

	  FREE_SLIST(fl);
	  
     } else {
	  free_lenv(lenv);
     }
     
     free_expr(mexpr2);
     free_frame(fp);

     return kil;
}

Op_Instance_List find_aop_from_rop_from_goal(Oprs *oprs, Op_Structure *op, Expression *mexpr, Goal *goal, PBoolean trace_forced)
{
     FramePtr fp;
     FrameList fl;
     Expression *lmexpr;
     List_Envar lenv = copy_lenv(op->list_var);
     Expression *mexpr2  = subst_lenv_in_expr(mexpr,lenv,QET_NONE);
     Expression *statement = goal->statement;
     FramePtr frame =  goal->frame;
     Op_Instance_List kil = sl_make_slist();


#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     if (unify_expr(mexpr2,statement, /* L'ordre des args est tres important */
		    fp=make_frame(frame))) {
	  Frame *fr1;
	  Op_Instance *op_tmp;
	  PBoolean present;
	  /* The frame is still installed and the OP unifie the triggering goal */
	  lmexpr = prepare_ip(op,lenv,mexpr);
	 
       	  fl = parse_inv_part(lmexpr, fp, lenv, oprs);
	  free_expr(lmexpr);	/* The IP has been parsed... free it... */

	  desinstall_frame(fp, frame);	  
	  if (sl_slist_empty(fl)) {
	       free_lenv(lenv);
	  } else {
	       PBoolean first = TRUE;
	       PBoolean used_lenv = FALSE;

	       sl_loop_through_slist(fl, fr1, Frame *) {
		    present = FALSE;
		    SAFE_SL_LOOP_THROUGH_SLIST(goal->echec, op_tmp, Op_Instance *) { /* remove the one we have already tried */
			 if (op_tmp->op == op) {
			      present = matching_lenv_between_frames(op_tmp->lenv,op_tmp->frame, lenv, fr1);
			 }
			 if (present) break; /* get out of this loop. */
		    }

		    if (! present) {
			 if (first) { /* First one... use the lenv and the frame */
			      sl_add_to_head(kil,make_op_instance_from_goal(op,fr1,lenv,goal));
			      used_lenv = TRUE; /* We have used the lenv. */
			      first = FALSE; /* Not the first time anymore */
			 } else {
			      PBoolean bool_tmp = check_all_vars_in_subst_lenv;
			      List_Envar lenv2 = copy_lenv(lenv); /* make a new lenv. */
			 
			      check_all_vars_in_subst_lenv = FALSE;
			      
			      sl_add_to_head(kil, /* and make a new frame */
					     make_op_instance_from_goal(op,
									subst_lenv_in_copy_of_frame_up_to_frame(fr1,
														lenv2,
														frame),
									lenv2,goal));
			      check_all_vars_in_subst_lenv = bool_tmp;
			      free_frame(fr1); /* We have done a copy now. */
			 }
		    } else {
			 free_frame(fr1); /* We are not using this frame, this ops has been tried already */
		    }
	       }
	       if (! used_lenv) { /* All of them have been tried, the lenv has not been used. */
		    free_lenv(lenv);
	       }
	  }

	  FREE_SLIST(fl); 

#ifdef  OPRS_PROFILING
	  TIME_STOP(&pd,T_APPLIC);
#endif /* OPRS_PROFILING */

	  if  ((debug_trace[SOAK]) || trace_forced) {
	       printf("The frame(s) in which the OP %s with\n", op->name);
	       printf(LG_STR("the invocation: ",
			     "the invocation: ")); print_expr(op->invocation); printf("\n");
	       printf(LG_STR("the call: ",
			     "l'appel: ")); print_expr(op->call); printf("\n");
	       printf(LG_STR("the context: ",
			     "the context: ")); print_exprlist(op->context); printf("\n");
	       printf(LG_STR("the setting: ",
			     "the setting: ")); print_expr(op->setting); printf("\n");
	       printf(LG_STR("and the goal: ",
			     "and the goal: "));
	       print_expr(statement);
	       printf(LG_STR("are applicable:\n",
			     "are applicable:\n"));
	       if (sl_slist_empty(kil)) printf("NULL\n");
	       else
		    sl_loop_through_slist(kil, op_tmp, Op_Instance *) {
			 print_frame(op_tmp->frame);
			 printf("\n");
		    }
	       printf("\n");
	  }

     } else {			/* the trigering fact does not even unify... */
	  free_lenv(lenv);
     }
     free_expr(mexpr2);
     free_frame(fp);
     return kil;
}

Op_Instance_List find_aop_from_rop_for_call_goal(Oprs *oprs, Op_Structure *op, Goal *goal, PBoolean trace_forced)
{
     FramePtr fp;
     List_Envar lenv = copy_lenv(op->list_var);
     Expression *expr2  = subst_lenv_in_expr(op->call,lenv,QET_NONE);
     Expression *statement = goal->statement;
     FramePtr frame =  goal->frame;
     Op_Instance_List kil = sl_make_slist();


#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     if (unify_expr(expr2,
		    EXPR_EXPR1(statement), /* L'ordre des args est tres important */
		    fp=make_frame(frame))) {

	  PBoolean present;
	  /* The frame is still installed and the OP unifie the triggering goal */
	 

	  desinstall_frame(fp, frame);	  
	  present = FALSE;
	  if (!SAFE_SL_SLIST_EMPTY(goal->echec)) {
	       Op_Instance *op_tmp = (Op_Instance *)sl_get_slist_head(goal->echec);
	       present = (op_tmp->op == op);
		
	  }
	  if (! present) {
	       sl_add_to_head(kil,make_op_instance_from_goal(op,fp,lenv,goal));
	  } else {
	       free_frame(fp);
	       free_lenv(lenv);
	  }
     } else  {
	  free_frame(fp);
	  free_lenv(lenv);
     }


#ifdef  OPRS_PROFILING
     TIME_STOP(&pd,T_APPLIC);
#endif /* OPRS_PROFILING */

     if  ((debug_trace[SOAK]) || trace_forced) {
	  Op_Instance *op_tmp;
	       printf("The frame(s) in which the OP %s with\n", op->name);
	       printf(LG_STR("the invocation: ",
			     "the invocation: ")); print_expr(op->invocation); printf("\n");
	       printf(LG_STR("the call: ",
			     "l'appel: ")); print_expr(op->call); printf("\n");
	       printf(LG_STR("the context: ",
			     "the context: ")); print_exprlist(op->context); printf("\n");
	       printf(LG_STR("the setting: ",
			     "the setting: ")); print_expr(op->setting); printf("\n");
	       printf(LG_STR("and the goal: ",
			     "and the goal: "));
	       print_expr(statement);
	       printf(LG_STR("are applicable:\n",
			     "are applicable:\n"));
	       if (sl_slist_empty(kil)) printf("NULL\n");
	       else
		    sl_loop_through_slist(kil, op_tmp, Op_Instance *) {
			 print_frame(op_tmp->frame);
			 printf("\n");
		    }
	       printf("\n");
	  }

     free_expr(expr2);
     return kil;
}

Op_Instance_List find_aop_from_rop_for_maint_goal(Oprs *oprs, Op_Structure *op,
						  Expression *mexpr, Goal *goal,
						  Expression *sub_goal,
						  Expression *maint_cond,
						  PBoolean trace_forced)
{
     FramePtr fp;
     FrameList fl;
     Expression *lmexpr;
     List_Envar lenv = copy_lenv(op->list_var);
     Expression *mexpr2  = subst_lenv_in_expr(mexpr,lenv,QET_NONE);
     FramePtr frame =  goal->frame;
     Op_Instance_List kil = sl_make_slist();

#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     if (unify_expr(mexpr2,
		    sub_goal, /* L'ordre des args est tres important */
		    fp=make_frame(frame))) {
	  Frame *inv_part_frame;
	  Op_Instance *op_tmp;
	  PBoolean present;
	  ExprFrame *gef;
	  ExprFrameList gefl;

	  /* The frame is still installed and the OP unifie the triggering goal */
	  lmexpr = prepare_ip(op,lenv,mexpr);

       	  fl = parse_inv_part(lmexpr, fp, lenv, oprs);
	  free_expr(lmexpr);	/* The IP has been parsed... free it... */
	  desinstall_frame(fp, frame);	  

	  if (sl_slist_empty(fl)) {
	       free_lenv(lenv);
	  } else {
	       PBoolean first = TRUE;
	       PBoolean used_lenv = FALSE;
	       Frame *maint_frame;

	       /* Check the maintain condition */

	       sl_loop_through_slist(fl, inv_part_frame, Frame *) {
		    reinstall_frame(inv_part_frame);
		    gefl = consult(EXPR_EXPR1(maint_cond), inv_part_frame, oprs->database, FALSE);
		    /* We do not need the gexpr, just the frame. */
		    
		    desinstall_frame(inv_part_frame,frame);

		    sl_loop_through_slist(gefl, gef, ExprFrame *) {
			 maint_frame = gef->frame;
			 FREE(gef);	/* one side points on NULL, the other one on frame which we just grabbed */
			 present = FALSE;
			 SAFE_SL_LOOP_THROUGH_SLIST(goal->echec, op_tmp, Op_Instance *) { /* remove the one we have already tried */
			      if (op_tmp->op == op) {
				   present = matching_lenv_between_frames(op_tmp->lenv,op_tmp->frame,
									  lenv, maint_frame);
			      }
			      if (present) break; /* get out of this loop. */
			 }
			 if (! present) {
			      if (first) { /* First one... use the lenv and the frame */
				   sl_add_to_head(kil,make_op_instance_for_maint_goal(op,maint_frame,lenv,goal, maint_cond));
				   used_lenv = TRUE; /* We have used the lenv. */
				   first = FALSE; /* Not the first time anymore */
				   
			      } else {
				   PBoolean bool_tmp = check_all_vars_in_subst_lenv;

				   List_Envar lenv2 = copy_lenv(lenv);
				   Frame *inv_part_frame_2, *maint_frame_2;
			      
				   check_all_vars_in_subst_lenv = FALSE;
/*
 * The checking of the condition has added a frame in which we do not have
 *  to substitute the lenv !!
 */ 
				   inv_part_frame_2 = subst_lenv_in_copy_of_frame_up_to_frame(inv_part_frame, lenv2, frame);
				   maint_frame_2 = copy_frame_up_to_replaced_frame (maint_frame, inv_part_frame, inv_part_frame_2);
				   sl_add_to_head(kil,
					       make_op_instance_for_maint_goal(op, maint_frame_2,
									       lenv2, goal, maint_cond));
				   check_all_vars_in_subst_lenv = bool_tmp;

				   free_frame(maint_frame); /* We have done a copy now. */
			      }
			 } else {
			      free_frame(maint_frame); /* We are not using this frame, this ops has been tried already */
			 }
		    }
		    FREE_SLIST(gefl);
		    free_frame(inv_part_frame);

	       }
	       if (! used_lenv) { /* All of them have been tried, the lenv has not been used. */
		    free_lenv(lenv);
	       }
	  }
	  FREE_SLIST(fl); 



#ifdef  OPRS_PROFILING
	  TIME_STOP(&pd,T_APPLIC);
#endif /* OPRS_PROFILING */

	  if  ((debug_trace[SOAK]) || trace_forced) {
	       printf(LG_STR("The frame(s) in which the OP %s with\n",
			     "The frame(s) in which the OP %s with\n"), op->name);
	       printf(LG_STR("the invocation: ",
			     "the invocation: ")); print_expr(op->invocation); printf("\n");
	       printf(LG_STR("the call: ",
			     "l'appel: ")); print_expr(op->call); printf("\n");
	       printf(LG_STR("the context: ",
			     "the context: ")); print_exprlist(op->context); printf("\n");
	       printf(LG_STR("the setting: ",
			     "the setting: ")); print_expr(op->setting); printf("\n");
	       printf(LG_STR("and the goal: ",
			     "and the goal: "));print_expr(sub_goal); printf("\n");
	       printf(LG_STR("with the maintenance cond:  ",
			     "with the maintenance cond:  "));print_expr(maint_cond);
	       printf(LG_STR("are applicable:\n",
			     "are applicable:\n"));
	       if (sl_slist_empty(kil)) printf("NULL\n");
	       else
		    sl_loop_through_slist(kil, op_tmp, Op_Instance *) {
			 print_frame(op_tmp->frame);
			 printf("\n");
		    }
	       printf("\n");
	  }

     } else {			/* the trigering fact does not even unify... */
	  free_lenv(lenv);
     }
     free_expr(mexpr2);
     free_frame(fp);
     return kil;
}

void report_find_soak_fact_ctxt_error(Fact *fact) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,128,sprintf(f,LG_STR("The previous error occured in find_soak_fact for fact:\n",
				    "The previous error occured in find_soak_fact for fact:\n")));
     sprint_fact(sp,fact);
     if (fact->statement) {
	  SPRINT(sp,16,sprintf(f,LG_STR(" statement: ",
					" statement: ")));
	  sprint_expr(sp, fact->statement);
     }
     SPRINT(sp,128,sprintf(f,LG_STR("\nThe fact will be ignored.",
				    "\nThe fact will be ignored.")));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

Op_Instance_List find_soak_fact(Fact *fact, Oprs *oprs, PBoolean consulting)
{
     Error_Ctxt previous_error_ctxt = current_error_ctxt;
     Op_Instance_List aop;

     current_error_ctxt = EC_FIND_SOAK_FACT;

     if (setjmp(find_soak_fact_jmp) == 0) {
	  Op_Instance_List kil_f;
	  Op_Expr_List ropl;
	  Op_Expr *opmexpr;
     
	  aop = NULL;
	  conclude(fact->statement,oprs->database); /* we conclude them in the database. */
	  ropl = consult_fact_relevant_op(fact->statement, oprs->relevant_op, FALSE); /* Look for the relevant_op list */
	  /* Do not free ropl, it is the hash table element... */
	  sl_loop_through_slist(ropl, opmexpr, Op_Expr *) { /* For all these relevant op, find those applicable. */
	       soak_parsing_op =  opmexpr->op;
	       kil_f = find_aop_from_rop_from_fact(oprs, opmexpr->op, opmexpr->expr, fact, consulting);

	       SAFE_SL_CONCAT_SLIST(aop,kil_f); /* add them to the result. */
	       
	  }
	  
#ifdef  OPRS_PROFILING
	  TIME_STOP(&fact->creation,T_FACT_CR_SOAK);
	  TIME_STOP(&fact->reception,T_FACT_REC_SOAK);
	  TIME_STAMP(&fact->soak);	/* for statistics */
#endif /* OPRS_PROFILING */

	  fact->pris_en_compte = TRUE; /* It has been taken into account */
	  
     } else {
	  report_find_soak_fact_ctxt_error(fact);
	  aop  = NULL;
     }
     current_error_ctxt = previous_error_ctxt;
     return aop;
}

void report_find_soak_goal_ctxt_error(Goal *goal) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,128,sprintf(f,LG_STR("The previous error occured in find_soak_goal for goal:\n",
				    "The previous error occured in find_soak_goal for goal:\n")));
     sprint_goal(sp,goal);
     if (goal->statement) {
	  SPRINT(sp,16,sprintf(f,LG_STR(" statement: ",
					" statement: ")));
	  sprint_expr(sp, goal->statement);
     }
     SPRINT(sp,128,sprintf(f,LG_STR("\nThe goal will be considered as failed.\n",
				    "\nThe goal will be considered as failed.\n")));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

void keep_track_of_goal_failure(Goal *goal)
{
     if (goal->interne) {
	  if (debug_trace[INT_FAIL_BIND]) {
	       char *op_name =  goal->tib->curr_op_inst->op->name;
	       Sprinter *sp = goal->interne->failed_goal_sprinter; 

	       if (SPRINTER_SIZE(sp)) 
		    SPRINT(sp, 40,sprintf(f,LG_STR("\n\t\ttrying to achieve: ",
						   "\n\t\tessayant de réaliser: ")));

	       sprint_expr(sp,goal->statement);
	       SPRINT(sp,8 + strlen(op_name),sprintf(f,LG_STR(" in OP %s",
							      " in OP %s"), op_name));

	  } 

	  if (debug_trace[INTENTION_FAILURE]) {
	       OPRS_LIST stack = goal->interne->failed_goal_stack; 

	       sl_add_to_tail(stack, dup_expr(goal->statement));
	       sl_add_to_tail(stack, goal->tib->curr_op_inst->op->name);
	  }
     }
}

Op_Instance_List find_soak_goal(Goal *goal, Oprs *oprs, PBoolean consulting)
{
     Op_Instance_List kil_g;
     Error_Ctxt previous_error_ctxt = current_error_ctxt;
     DB_Ope dop;
     Temporal_Ope top;
     current_error_ctxt = EC_FIND_SOAK_GOAL;

     if (setjmp(find_soak_goal_jmp) == 0) {
	  Op_Expr_List ropl;
	  Op_Expr *opmexpr;
	  Thread_Intention_Block *tib;
#ifdef OPRS_PROFILING
	  PDate pd;
#endif /* OPRS_PROFILING */

	  kil_g = NULL;

	  if ((tib = goal->tib) && (tib->status != TIBS_ACTIVE)) {
	       /* this thread has either been suspended or removed after posting this goal */
#ifdef VIANNEY
	       fprintf (stderr, LG_STR("Obsolete Goal: ",
				       "Obsolete Goal: "));
	       fprint_gtexpr(stderr, goal->statement);
	       fprintf (stderr, "\n");
#endif
	  } else if ((dop = expr_db_type(goal->statement))) {
	       
	       if (dop == DB_CONC) {
		    if (consulting)
			 printf(LG_STR("The conclude goals are always applicable.\n",
				       "Les buts d'assertion sont toujours satisfaits.\n"));
		    else 
			 fprintf (stderr,LG_STR("ERROR: find_soak_goal: Now we did not have to get here (add_fact before find_soak).\n",
						"ERREUR: find_soak_goal: On n'a rien a faire ici.\n"));
		    
	       } else if (dop == DB_RET) {
		    if (consulting)
			 printf(LG_STR("The retract goals are always applicable.\n",
				       "The retract goals are always applicable.\n"));
		    else
			 fprintf (stderr,LG_STR("Now we did not have to get here (add_fact before find_soak)\n",
						"Now we did not have to get here (add_fact before find_soak)\n"));
	       }
	  } else if ((top = expr_temporal_type(goal->statement))) {

	       ExprFrameList gefl;
	       ExprFrame *gef;

	       switch (top) {
	       case CALL:
		    ropl = consult_goal_relevant_op(goal->statement, oprs->relevant_op, FALSE);
		    /* A priori... one or none... */
		    if (! SAFE_SL_SLIST_EMPTY(ropl)) {
			 Op_Instance_List aopg;

			 opmexpr = (Op_Expr *)sl_get_slist_head(ropl);
			 
			 soak_parsing_op =  opmexpr->op;
			 aopg = find_aop_from_rop_for_call_goal(oprs, opmexpr->op, goal, consulting);
			 SAFE_SL_CONCAT_SLIST(kil_g,aopg);
		    }
		    if (SAFE_SL_SLIST_EMPTY(kil_g)) {
			 goal->echoue = NORMAL_FAILURE; /* Nobody knows how to achieve it... */
			 if (debug_trace[INTENTION_FAILURE] ||
			     debug_trace[INT_FAIL_BIND]) keep_track_of_goal_failure(goal);
		    }
		    break;

	       case TEST:		
	       case WAIT:
	       case ACHIEVE:
	       case ACT_MAINT:
#ifdef OPRS_PROFILING
		    TIME_START(&pd);
#endif /* OPRS_PROFILING */

#ifdef BENCH
		    TIME_START(&pd2);
#endif /* BENCH */
		    gefl =  consult(EXPR_EXPR1(goal->statement), 
				    goal->frame, oprs->database, FALSE); /* Look if they are satisfied already... */

#ifdef BENCH
		    if (top == TEST) 
			 TIME_STOP(&pd2, T_TEST);
#endif /* BENCH */


#ifdef OPRS_PROFILING
		    TIME_STOP(&pd,T_HANDLE);
#endif /* OPRS_PROFILING */
		    if (!(sl_slist_empty(gefl))) { /* If they are... */
			 if ((debug_trace[SOAK]) || consulting) { /* debug info */
			      printf(LG_STR("The frame(s) in which the the goal: ",
					    "The frame(s) in which the the goal: "));
			      print_expr(goal->statement);
			      printf(LG_STR("is true in the database are:\n",
					    "is true in the database are:\n"));
			      sl_loop_through_slist(gefl,  gef, ExprFrame *) {
				   print_frame(gef->frame);
				   printf("\n");
			      }
			      printf("\n");
			 }
			 goal->etat = TRUE; /* The goal is satisfied in the db */
			 sl_loop_through_slist(gefl, gef, ExprFrame *) {
			      SAFE_SL_ADD_TO_HEAD(kil_g,make_op_instance_from_goal_satisfied_in_db(gef->frame,goal));
			      FREE(gef);
			 }
		    } else {		/* It is not satisfied int the DB... */
			 if (top == WAIT) {
			      SAFE_SL_ADD_TO_HEAD(kil_g,make_op_instance_from_waiting_goal(goal));
			 } else {
			      ropl = consult_goal_relevant_op(goal->statement, oprs->relevant_op, FALSE);
			      sl_loop_through_slist(ropl, opmexpr, Op_Expr *) {
				   Op_Instance_List aopg;
			      
				   soak_parsing_op =  opmexpr->op;
				   aopg = find_aop_from_rop_from_goal(oprs, opmexpr->op,
								      opmexpr->expr, goal, consulting);
				   SAFE_SL_CONCAT_SLIST(kil_g,aopg);
			      }
			 }
		    }
		    FREE_SLIST(gefl); 
		    if (SAFE_SL_SLIST_EMPTY(kil_g)) {
			 goal->echoue = NORMAL_FAILURE; /* Nobody knows how to achieve it... */
			 if (debug_trace[INTENTION_FAILURE] ||
			     debug_trace[INT_FAIL_BIND]) keep_track_of_goal_failure(goal);
		    }
		    break;
	       case PASS_MAINT:
		    fprintf(stderr,LG_STR("Maintenance temporal operator must be used in a Logical Temporal Expression.\n",
					  "Maintenance temporal operator must be used in a Logical Temporal Expression.\n"));
		    goal->echoue = FORBIDDEN_SYNTAX;
		    break;
	       default:
		    fprintf(stderr,LG_STR("Unknown temporal operator in find_soak...\n",
					  "Unknown temporal operator in find_soak...\n"));
		    break;
	       }
	  } else { /* We only accept the AND operator on conlude or retract goals,
		      or a preserve goal : a maintenance and an achieve goal.*/
	       if (AND_P(goal->statement)) {
			 Expression *maint_goal = NULL;
			 Expression *maint_cond = NULL;
			 PBoolean conc_or_retract = FALSE;
			 PBoolean one_achieve = FALSE;
			 PBoolean one_maint = FALSE;
			 PBoolean accepted_ltexpr = TRUE;
			 Term *term;

			 ExprFrameList gefl;
			 ExprFrame *gef;

			 sl_loop_through_slist(EXPR_TERMS(goal->statement),term, Term *) {
			      Expression *expr = TERM_EXPR(term);

			      if ((dop = expr_db_type(expr))) {
				   if ((dop == DB_CONC) || (dop == DB_RET)) {
					conc_or_retract = TRUE;
				   }
			      } else if ((top = expr_temporal_type(expr))) {
				   
				   switch (top) {	   
				   case TEST:		
				   case WAIT:
					fprintf(stderr,LG_STR("You cannot use this temporal operator in a LTEXPR in find_soak ...\n",
							      "You cannot use this temporal operator in a LTEXPR in find_soak ...\n"));
					accepted_ltexpr = FALSE;
					break;
				   case ACHIEVE:
					if (one_achieve) {
					     accepted_ltexpr = FALSE;
					} else {
					     one_achieve = TRUE;
					     maint_goal = expr;
					}
					break;
				   case ACT_MAINT:
				   case PASS_MAINT:
					if (one_maint) {
					     accepted_ltexpr = FALSE;
					} else {
					     maint_cond = expr;
					     one_maint = TRUE; 
					}
					break;
				   default:
					fprintf(stderr,LG_STR("Unknown temporal operator in find_soak...\n",
							      "Unknown temporal operator in find_soak...\n"));
					accepted_ltexpr = FALSE;
					break;
				   }
			      } else {
				   fprintf(stderr,LG_STR("type of goal must be TEXPR in find_soak...\n",
							 "type of goal must be TEXPR in find_soak...\n"));
				   accepted_ltexpr = FALSE;
			      }
			 }
			 
			 if (accepted_ltexpr) { /* some other verifications */
			      if ((conc_or_retract && (one_maint || one_achieve))
				  || (one_maint && (! one_achieve)) 
				  || ((! one_maint) && one_achieve)) {
			 
				   accepted_ltexpr = FALSE;
			      }
			 }

			 if (accepted_ltexpr) {
			      if (conc_or_retract) {
				   if (consulting)
					printf(LG_STR("The conclude or retract goals are always applicable.\n",
						      "The conclude or retract goals are always applicable.\n"));
				   else
					fprintf (stderr,LG_STR("Now we did not have to get here (add_fact before find_soak)\n",
							       "Now we did not have to get here (add_fact before find_soak)\n"));
			      } else { /* maintenance goal */

				   /* First Look if the goal AND the condition are satisfied already... */

				   gefl =  consult(EXPR_EXPR1(maint_goal), 
						   goal->frame, oprs->database, FALSE);
				   if (!(sl_slist_empty(gefl))) { /* If it is... check the condition with each frame. */
					ExprFrameList gefl2 = sl_make_slist();

					sl_loop_through_slist(gefl, gef, ExprFrame *) {
					     reinstall_frame(gef->frame);
					     gefl2 = sl_concat_slist(gefl2,
								 consult(EXPR_EXPR1(maint_cond),
									 gef->frame, oprs->database, FALSE));
					     desinstall_frame(gef->frame, goal->frame);
					}
						       
					if (!(sl_slist_empty(gefl2))) {
					     if ((debug_trace[SOAK]) || consulting) { /* debug info */
						  printf(LG_STR("The frame(s) in which the the goal: ",
								"The frame(s) in which the the goal: "));
						  print_expr(goal->statement);
						  printf(LG_STR("is true in the database are:\n",
								"is true in the database are:\n"));
						  sl_loop_through_slist(gefl2,  gef, ExprFrame *) {
						       print_frame(gef->frame);
						       printf("\n");
						  }
						  printf("\n");
					     }
					     goal->etat = TRUE; /* The goal is satisfied in the db */
					     sl_loop_through_slist(gefl2, gef, ExprFrame *) {
						  SAFE_SL_ADD_TO_HEAD(kil_g,make_op_instance_from_goal_satisfied_in_db(gef->frame,goal));
						  FREE(gef);
					     }
					}
					FREE_SLIST(gefl2); 

					sl_loop_through_slist(gefl, gef, ExprFrame *) {
					     free_frame(gef->frame);
					     FREE(gef);
					}
				   }
				   FREE_SLIST(gefl); 

				   if (SAFE_SL_SLIST_EMPTY(kil_g)) {  /* Not  satisfied in DB */
					Expression *maint_goal_gtexpr = dup_expr(maint_goal);
 
					ropl = consult_goal_relevant_op(maint_goal_gtexpr, oprs->relevant_op, FALSE);
					free_expr(maint_goal_gtexpr);

					sl_loop_through_slist(ropl, opmexpr, Op_Expr *) {
					     Op_Instance_List aopmg;

					     soak_parsing_op =  opmexpr->op;
					     aopmg = find_aop_from_rop_for_maint_goal(oprs, opmexpr->op, 
										      opmexpr->expr, goal,
										      maint_goal, maint_cond, 
										      consulting);
					     SAFE_SL_CONCAT_SLIST(kil_g,aopmg);
					}
				   }
				   if (SAFE_SL_SLIST_EMPTY(kil_g)) {
					goal->echoue = NORMAL_FAILURE; /* Nobody knows how to achieve it... */
				   }
			      }
			 } else { /* refused ltexpr */
			      fprintf(stderr,LG_STR("You can either use a conjonction of conclude and retract goals,\n or the conjonction of one achieve goal and one maintenance goal !\n",
						    "You can either use a conjonction of conclude and retract goals,\n or the conjonction of one achieve goal and one maintenance goal !\n"));

			      goal->echoue = FORBIDDEN_SYNTAX; 
			 }
		    } else {
			 fprintf(stderr,LG_STR("The type the goal should be TEXPR, or an AND LTEXPR in find_soak...\n",
					       "The type the goal should be TEXPR, or an AND LTEXPR in find_soak...\n"));
			 goal->echoue = FORBIDDEN_SYNTAX;
		    }
	  }
	  goal->pris_en_compte = TRUE;
#ifdef  OPRS_PROFILING
	  TIME_STOP(&goal->creation,T_GOAL_CR_SOAK);
	  TIME_STOP(&goal->reception,T_GOAL_REC_SOAK);
	  TIME_STAMP(&goal->soak);	/* for statistics */
#endif /* OPRS_PROFILING */

	  if ((goal->echoue != NOT_FAILED) && ! goal->interne && debug_trace[SUC_FAIL]) {
	       static Sprinter *sp = NULL;

	       if (!sp) sp = make_sprinter(0);
	       else reset_sprinter(sp);
	       sprint_expr(sp, goal->statement);
	       SPRINT(sp, 64,
		      sprintf(f, LG_STR(" is not satisfied and does not make any OP applicable.\n",
					" n'est pas satisfait et ne rend pas de OP applicable.\n")));
	       printf("%s",SPRINTER_STRING(sp));
	  }	       
     } else {
	  report_find_soak_goal_ctxt_error(goal);
	  goal->echoue = ERROR_HANDLER; /* kind of normal... */
	  kil_g = NULL;
     }
     current_error_ctxt = previous_error_ctxt;
     return kil_g;
}

void post_fact_inv_ops_meta_fact(Op_Instance_List fact_inv_ops, Oprs *oprs)
/*
 * post_fact_inv_ops_meta_fact - will post a fact (fact-invoked-ops <fact_inv_ops>) in the OPRS oprs.
 *                       The fact_inv_ops list is
 *                       first transformed as a lisp list, because most access functions
 *                       used in OPs are lisp like function...
 *                       Return void.
 */
{
     TermList tl;

     if (oprs->critical_section) return;

     tl = sl_make_slist();
     /* Transform the C list in Lisp list. */
     /* Will dup each elt... */
     sl_add_to_head(tl, build_l_list(make_l_list_from_c_list_type(fact_inv_ops, TT_OP_INSTANCE))); 
     add_fact(make_fact_from_expr(build_expr_pfr_terms(fact_invoked_ops_pred, tl)), oprs);
     oprs->posted_meta_fact = TRUE;
}

void post_goal_inv_ops_meta_fact(Op_Instance_List goal_inv_ops, Oprs *oprs)
/*
 * post_goal_inv_ops_meta_fact - will post a fact (goal-invoked-ops <goal_inv_ops>) in the OPRS oprs.
 *                       The goal_inv_ops list is
 *                       first transformed as a lisp list, because most access functions
 *                       used in OPs are lisp like function...
 *                       Return void.
 */
{
     TermList tl;

     if (oprs->critical_section) return;

     tl = sl_make_slist();

     sl_add_to_head(tl,
		    build_l_list(make_l_list_from_c_list_type(goal_inv_ops, 
							      TT_OP_INSTANCE))); /* Will dup each elt... */
     add_fact(make_fact_from_expr(build_expr_pfr_terms(goal_invoked_ops_pred, tl)),oprs);
     oprs->posted_meta_fact = TRUE;
}

void post_meta_fact_applicable_ops_goal(Goal *goal, Op_Instance_List op_list, Oprs *oprs)
{
     TermList tl;

     if (oprs->critical_section) return;

     tl = sl_make_slist();

     sl_add_to_head(tl, build_l_list(make_l_list_from_c_list_type(op_list, TT_OP_INSTANCE)));
     sl_add_to_head(tl, build_goal_term(dup_goal(goal)));
     add_fact(make_fact_from_expr(build_expr_pfr_terms(app_ops_goal_pred, tl)), oprs);
     oprs->posted_meta_fact = TRUE;
}

void post_meta_fact_applicable_ops_fact(Fact *fact, Op_Instance_List ak, Oprs *oprs)
{
     TermList tl;

     if (oprs->critical_section) return;

     tl = sl_make_slist();
     sl_add_to_head(tl, build_l_list(make_l_list_from_c_list_type(ak, TT_OP_INSTANCE)));
     sl_add_to_head(tl, build_fact_term(dup_fact(fact)));
     add_fact(make_fact_from_expr(build_expr_pfr_terms(app_ops_fact_pred, tl)), oprs);
     oprs->posted_meta_fact = TRUE;
}     

Condition_List cond_force_check = NULL;

Op_Instance_List find_soak(Oprs *oprs)
/* 
 * find_soak - will return the current set of applicable OPs. This is a rather important 
 *             function in the system.
 */
{
	  Fact *fact;
	  Goal *goal;
	  Op_Instance_List tmp_soak;
	  Slist *tmp_list;
	  Relevant_Condition *rc;
#ifdef GRAPHIX
	  int i = 0;
#endif

	  Op_Instance_List kil_f = NULL;
	  Op_Instance_List kil_g = NULL;
	  Condition_List cond, cond_tmp;

	  if (cond_force_check) {
	       cond_tmp = COPY_SLIST(cond_force_check);
	       check_condition_list(cond_tmp); /* This may remove the condition from cond_force_check... */
	       FREE_SLIST(cond_tmp);
	       FREE_SLIST(cond_force_check);
	       cond_force_check = NULL;
	  }
 
	  oprs->posted_meta_fact = FALSE;

	  sl_loop_through_slist(oprs->facts, fact, Fact *) { /* For all the fact */

#ifdef GRAPHIX
	       if (i++ == 5) {
		    i = 0;
		    process_xt_events();
	       }
#endif

	       tmp_soak=find_soak_fact(fact,oprs, FALSE);

	       cond_tmp = collect_cond_from_fact(fact);
	       cond = NULL;
	       SAFE_SL_LOOP_THROUGH_SLIST(cond_tmp, rc, Relevant_Condition *) {
		    if (! (SAFE_SL_IN_SLIST(cond,rc)))
			 SAFE_SL_ADD_TO_HEAD(cond,rc);
	       }
	
	       if (cond) {
		    check_condition_list(cond);
		    FREE_SLIST(cond);
	       }
   
	       if ((meta_option[META_LEVEL] && meta_option[APP_OPS_FACT]) &&
		   (! (SAFE_SL_SLIST_EMPTY(tmp_soak))) &&
		   (! oprs->critical_section)) 
		    post_meta_fact_applicable_ops_fact(fact,tmp_soak,oprs);
	  	  
	       SAFE_SL_CONCAT_SLIST(kil_f,tmp_soak);
	  }
	  
	  cond = NULL;
	  if (check_evolving_conditions) {
	       sl_loop_through_slist(oprs->conditions_list, rc, Relevant_Condition *) 
		    if (rc->evolving && ! (SAFE_SL_IN_SLIST(cond,rc)))
			 SAFE_SL_ADD_TO_HEAD(cond,rc);
#ifdef HAS_TIMER
	       check_evolving_conditions = FALSE;
#endif
	  }
	  
	  if (cond) {
	       check_condition_list(cond);
	       FREE_SLIST(cond);
	  }

	  if ((meta_option[META_LEVEL] && meta_option[FACT_INV]) &&
	      (! (SAFE_SL_SLIST_EMPTY(kil_f))) &&
	      (! oprs->critical_section))
	       post_fact_inv_ops_meta_fact(kil_f, oprs);

	  tmp_list = COPY_SLIST(oprs->goals ); /* We copy it to be able to loop through it recursively */
	  /* 
	   * Loop_through_list is a bitch... It tooks me 5 hours to find out that if there 
	   * is a nested call on the same list... you loose... I knew it, but it was deep inside (in parse_ip I guess)
	   * so, I forgot. Damnit, I am going to rewrite a cleaner macro. For now I copy when I 
	   * consult the list only.
	   */
	  sl_loop_through_slist(tmp_list, goal, Goal *) {

#ifdef GRAPHIX
	       if (i++ == 5) {
		    i = 0;
		    process_xt_events();
	       }
#endif

	       tmp_soak = find_soak_goal(goal,oprs, FALSE);
	  
	       if ((meta_option[META_LEVEL] && meta_option[APP_OPS_GOAL]) &&
		   (! (SAFE_SL_SLIST_EMPTY(tmp_soak))) &&
		   (! oprs->critical_section))
		    post_meta_fact_applicable_ops_goal(goal,tmp_soak,oprs);

	       SAFE_SL_CONCAT_SLIST(kil_g,tmp_soak);
	  }
	  if ((meta_option[META_LEVEL] && meta_option[GOAL_INV]) && 
	      (! (SAFE_SL_SLIST_EMPTY(kil_g))) &&
	      (! oprs->critical_section))
	       post_goal_inv_ops_meta_fact(kil_g, oprs);

	  FREE_SLIST(tmp_list ); 

	  /* For all the fact, we delete basic events, clean up, and post meta fact */
	  delete_basic_events(oprs);

	 return  SAFE_SL_CONCAT_SLIST (kil_f, kil_g);     
}

FrameList parse_ip_or(ExprList gmel, Frame *frame, Oprs *oprs);
FrameList parse_ip_and(ExprList gmel, Frame *frame, Oprs *oprs);
FrameList parse_ip_expr(Expression *mexpr, Frame *frame, Oprs *oprs);

FrameList parse_inv_part(Expression *lmexpr, FramePtr fp, List_Envar lenv, Oprs *oprs)
{
     return  parse_ip_expr(lmexpr, fp, oprs);
}

/* 
 * This code is quite similar to the consult code in the db. If you find a bug here or 
 * there, most likely the bug is also in the other part...
 */

FrameList parse_ip_expr(Expression *mexpr, Frame *frame, Oprs *oprs) 
{
     FrameList fl_res = sl_make_slist();
     
     if (mexpr == NULL) {
	  sl_add_to_head(fl_res, dup_frame(frame)); /* This is the triggering fact or goal which has been replaced by NULL */
     } else {
	  ExprFrameList gefl;
	  ExprFrame *gef;

	  if (expr_temporal_type(mexpr)) {

	       Goal *goal;
	       FramePtr fp;

	       sl_loop_through_slist(oprs->goals, goal, Goal *) {
		    if (unify_expr(mexpr,
				   goal->statement,
				   fp=make_frame(frame))) {
			 desinstall_frame(fp,frame);
			 sl_add_to_head(fl_res,fp);
		    } else free_frame(fp);
	       }
	  } else if (!expr_db_type(mexpr)){
	       ExprList el;
	       FrameList res;
	       if (AND_P(mexpr)){
		    el = make_el_from_tl(EXPR_TERMS(mexpr));
		    res = parse_ip_and(el, frame, oprs);
		    FREE_SLIST(el); FREE_SLIST(fl_res);
		    return res;
	       } else if (OR_P(mexpr)){
		    el = make_el_from_tl(EXPR_TERMS(mexpr));
		    res = parse_ip_or(el, frame, oprs);
		    FREE_SLIST(el); FREE_SLIST(fl_res);
		    return res; 
	       } else {
		    gefl = consult(mexpr,frame,oprs->database,FALSE); /* We do not need the gexpr, just the frame. */
		    sl_loop_through_slist(gefl, gef, ExprFrame *) {
			 sl_add_to_head(fl_res, gef->frame);
			 FREE(gef);	/* one side points on NULL, the other one on frame which we just grabbed */
		    }
		    FREE_SLIST(gefl);
	       }
	  }
     }
     return fl_res;
}

FrameList parse_ip_and(ExprList gmel, Frame *frame, Oprs *oprs) 
{

     FrameList fl, fl_res;
     Expression *gmexpr;
     Frame *fr;
     ExprList gmel2;
     
     if (sl_slist_length(gmel) == 1) { /* C'est le dernier de la list... */
	  /* A priori il n'est pas necessaire de creer un frame ici. */
	  fl_res =  parse_ip_expr((Expression *)sl_get_from_head(gmel),frame,oprs);
     } else {
	  fl_res = sl_make_slist();
	  gmexpr = (Expression *)sl_get_from_head(gmel);
	  fl = parse_ip_expr(gmexpr, frame ,oprs); /* idem... */
	  sl_loop_through_slist(fl, fr, Frame *) {
	       reinstall_frame(fr); /* si on ne la copie pas, elle va etre reduite par les appels recursifs */
	       fl_res = sl_concat_slist(fl_res,parse_ip_and(gmel2 = (ExprList)COPY_SLIST(gmel), fr, oprs)); 
	       FREE_SLIST(gmel2);
	       desinstall_frame(fr,frame);
	       free_frame(fr);
	  }
	  FREE_SLIST(fl);
     }
     return fl_res;
}

FrameList parse_ip_sub_or(Expression* gmexpr, ExprList gmel, Frame *frame, Oprs *oprs) 
{
     FrameList fl_res,  fl;
     ExprList gmel2,gmel3;
     Expression *gme;
     FramePtr fr;

     fl_res = sl_make_slist();
     fl = parse_ip_expr(gmexpr, frame ,oprs);
     sl_loop_through_slist(fl, fr, Frame *) {
	  sl_add_to_head(fl_res,fr);
	  if (! (sl_slist_empty(gmel))) {
	       reinstall_frame(fr);
	       gmel2 = COPY_SLIST(gmel);
	       gme = (Expression *)sl_get_from_head(gmel2);
	       while (gme != NULL) {
		    fl_res = sl_concat_slist(fl_res,parse_ip_sub_or(gme,gmel3=COPY_SLIST(gmel2), fr, oprs));
		    FREE_SLIST(gmel3);
		    gme = (Expression *)sl_get_from_head(gmel2);
	       }
	       FREE_SLIST(gmel2);
	       desinstall_frame(fr,frame);
	  }
     }
     FREE_SLIST(fl);
     return fl_res;
}

FrameList parse_ip_or(ExprList gmel, Frame *frame, Oprs *oprs) 
{
     FrameList fl_res;
     Expression *gme;
     ExprList gmel2;
     PBoolean empty;

     fl_res = sl_make_slist();
     empty = sl_slist_empty(gmel);
     gme =(Expression *)sl_get_from_head(gmel);
     while (! empty) {
	  fl_res = sl_concat_slist(fl_res,parse_ip_sub_or(gme, gmel2 = COPY_SLIST(gmel), frame, oprs));
	  empty = sl_slist_empty(gmel);
	  gme = (Expression *)sl_get_from_head(gmel);
	  FREE_SLIST(gmel2);
     }
     return fl_res;
}

FrameList parse_ip_not(ExprList gmel, Frame *frame, Oprs *oprs) 
{
     FrameList fl,fl_res;
     Expression *gmexpr;
     FramePtr fr;

     fl_res = sl_make_slist();
     gmexpr = (Expression *)sl_get_from_head(gmel);
     if (sl_slist_empty(fl = parse_ip_expr(gmexpr,frame,oprs))) {
	  sl_add_to_head(fl_res,dup_frame(frame));
     }

     sl_loop_through_slist(fl, fr, Frame *) {
	  free_frame(fr);
     }
     FREE_SLIST(fl);
     
     return fl_res;
}
void test_find_soak_expr(Expression *expr, Oprs *oprs)
{     
     if (expr_temporal_type(expr) || expr_db_type(expr)) 
	  test_find_soak_goal(make_goal_from_expr(expr, empty_env), oprs);
     else 
	  test_find_soak_fact(make_fact_from_expr(expr), oprs); 
}

void test_find_soak_goal(Goal *goal, Oprs *oprs)
{
     Op_Instance_List aop;
     Op_Instance *opi;

     aop = find_soak_goal(goal,oprs,TRUE);

     SAFE_SL_LOOP_THROUGH_SLIST(aop, opi, Op_Instance *)
	  free_op_instance(opi);
     SAFE_SL_FREE_SLIST(aop);
}

void test_find_soak_fact(Fact *fact, Oprs *oprs)
{
     Op_Instance_List aop;
     Op_Instance *opi;

     aop = find_soak_fact(fact,oprs,TRUE);

     SAFE_SL_LOOP_THROUGH_SLIST(aop, opi, Op_Instance *)
	  free_op_instance(opi);
     SAFE_SL_FREE_SLIST(aop);
     
     if (PFR_BE(fact->statement->pfr))
	  delete_expr(fact->statement, oprs->database); /* we delete it from the database. */
}

#ifdef HAS_TIMER
#ifdef HAS_SIGACTION
sigset_t sigalarm_set;

void block_sigalarm(void)
{
     if (sigprocmask(SIG_BLOCK, &sigalarm_set , NULL) < 0)
	  perror("block_sigalarm: sigprocmask");
}

void unblock_sigalarm(void)
{
     if (sigprocmask(SIG_UNBLOCK, &sigalarm_set, NULL) < 0)
	  perror("unblock_sigalarm: sigprocmask");
}
#else
void block_sigalarm(void)
{
}

void unblock_sigalarm(void)
{
}
#endif

#ifdef VXWORKS
timer_t interval_timer_id;
struct itimerspec interval_timer_val;
struct itimerspec null_interval_timer_val;
#else
struct itimerval null_interval_timer_val;
struct itimerval interval_timer_val;
# endif


#ifdef HAS_SIGACTION
static struct sigaction act;
#endif

void arm_condition_timer(void)
{
#ifdef VXWORKS
     if (timer_settime(interval_timer_id,!TIMER_ABSTIME,&interval_timer_val,NULL) != OK)
	  perror("set_condition_timer: timer_settime");
#else
     if (setitimer(ITIMER_REAL, &interval_timer_val, NULL) == -1)
	  perror("arm_condition_timer: setitimer");
#endif
}

#ifdef HAS_SIGACTION
static void condition_timer_handler(int sig)
#elif defined(VXWORKS)
void condition_timer_handler(int sig)
#else
static void condition_timer_handler(int sig)
#endif /* SVR4 */     
{
     check_evolving_conditions = TRUE;
     check_the_stdin = TRUE;
     stop_checking_the_stdin = TRUE;
} 

void install_condition_timer_handler(void)
{
#ifdef HAS_SIGACTION
    sigaction(SIGALRM, &act, NULL);
#else /* SVR4 */
    signal(SIGALRM, condition_timer_handler);
#endif /* SVR4 */
}

void set_interval_timer()
{
#ifdef VXWORKS
     interval_timer_val.it_value.tv_nsec = main_loop_pool_usec*1000L;	
     interval_timer_val.it_interval.tv_nsec = main_loop_pool_usec*1000L;	

     null_interval_timer_val.it_value.tv_nsec = 0;	
     null_interval_timer_val.it_interval.tv_nsec = 0;	
#else
     interval_timer_val.it_value.tv_usec = main_loop_pool_usec;	
     interval_timer_val.it_interval.tv_usec = main_loop_pool_usec;	

     null_interval_timer_val.it_value.tv_usec = 0;	
     null_interval_timer_val.it_interval.tv_usec = 0;	
#endif

     interval_timer_val.it_value.tv_sec =  main_loop_pool_sec;	
     interval_timer_val.it_interval.tv_sec = main_loop_pool_sec;	

     null_interval_timer_val.it_value.tv_sec =  0;	
     null_interval_timer_val.it_interval.tv_sec = 0;	
}

void install_condition_timer_handler_set_arm(void)
{
#ifdef HAS_SIGACTION
   /* Install the handler. */
    act.sa_handler = condition_timer_handler;
    sigemptyset(&act.sa_mask);

    sigemptyset(&sigalarm_set);
    sigaddset(&sigalarm_set, SIGALRM);

#ifdef VXWORKS
    /* Due to the way VxWorks restart system call (reusing the initial timeout value) we need not to
       restart system call. */
    act.sa_flags = SA_INTERRUPT; /* (Not posix) Don't restart the function */
#else
    act.sa_flags = 0; /* SA_RESTART;*/	/* Restart system call without generating EINTR... */
#endif
#endif

#ifdef VXWORKS
    /* This will create a timer. */
    /* the definition of timer_create has changed between 5.1 and 5.2,
       therefore this assume > 5.2. */
    if ((timer_create(CLOCK_REALTIME,NULL, &interval_timer_id)) != OK) /* NULL will generate a SIGALRM */
	 perror("install_condition_timer_handler: timer_create");

#endif

    /* This will just install the handler function */
    install_condition_timer_handler();

    /* This will set the interval timer */
    set_interval_timer(); 

    /* This will install the interval timer in the timer */
    arm_condition_timer();
}

void kill_condition_timer(void)
{
     desarm_condition_timer ();
#ifdef VXWORKS
     if ((timer_cancel(interval_timer_id)) != OK)
	  perror("kill_condition_timer: timer_delete");
#endif
}

void desarm_condition_timer(void)
{
#ifdef VXWORKS
     if (timer_settime(interval_timer_id,!TIMER_ABSTIME,&null_interval_timer_val,NULL) != OK)
	  perror("desarm_condition_timer: timer_settime");
#else
     if (setitimer(ITIMER_REAL, &null_interval_timer_val, NULL) == -1)
	  perror("desarm_condition_timer: setitimer");
#endif
}
#endif /* HAS_TIMER */

