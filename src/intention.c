/*                               -*- Mode: C -*- 
 * intention.c -- Functions and utilities to manipulate intentions.
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

#include "config.h"

/*	For timeval definition. */
#ifdef WIN95
#include <winsock.h>
#endif


#ifdef GRAPHIX
#ifdef GTK
#else
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif
#endif

#include "constant.h"
#include "macro.h"

#include "oprs.h"
#include "intention.h"
#include "int-graph.h"
#include "lisp-list.h"
#include "fact-goal.h"
#include "op-structure.h"
#include "op-instance.h"
#include "oprs-profiling.h"
#include "oprs-print.h"
#include "oprs-sprint.h"

#include "op-instance_f.h"
#include "intention_f.h"
#include "conditions_f.h"
#ifdef GRAPHIX
#ifdef GTK
#include "gope-graphic.h"
#include "goprs-intention.h"
#include "goprs-intention_f.h"
#include "xm2gtk_f.h"
#else
#include "xoprs-intention_f.h"
#endif
#endif
#include "op-structure_f.h"
#include "oprs-profiling_f.h"
#include "unification_f.h"
#include "oprs-type_f.h"
#include "fact-goal_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "oprs-pprint_f.h"
#include "activate_f.h"

Int_Draw_Data *global_int_draw_data = NULL;
Thread_Intention_Block *current_tib = NULL;
Intention *current_intention = NULL;


Thread_Intention_Block *fork_thread_intention_block(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *res =  MAKE_OBJECT(Thread_Intention_Block);
     
     res->curr_op_inst = dup_op_instance(tib->curr_op_inst);
     res->current_node = tib->current_node;
     res->current_goal = NULL;
     res->list_to_try = sl_make_slist(); /* There are good reason to think it will be used... */
     res->pere = tib->pere;
     res->intention = tib->intention;
     res->activation_condition = NULL;
     res->maintain_condition = NULL;
     res->maintain = NULL;
     res->fils = sl_make_slist();
     res->num_called = 0;
     res->status = TIBS_ACTIVE;
     res->suspended = NULL;
     if (tib->intention->critical_section) 
	  printf (LG_STR("WARNING: Spliting in a critical section.\n\
\tThis is very dangerous.\n",
			 "ATTENTION: Création de thread dans une section critique,\n\
\tc'est très risqué.\n"));
     return res;
}

Thread_Intention_Block *split_thread_intention_block(Thread_Intention_Block *tib)
{
     Thread_Intention_Block *res =  MAKE_OBJECT(Thread_Intention_Block);

     *res = *tib;
     res->curr_op_inst = dup_op_instance(tib->curr_op_inst);
     res->list_to_try = sl_make_slist();
     res->activation_condition = NULL;
     res->maintain_condition = NULL;

     res->maintain = NULL;
     res->fils = sl_make_slist();
     res->num_called = 0;
/*     res->status = tib->status; */
     res->suspended = tib;

     return res;
}

Thread_Intention_Block *make_thread_intention_block(Op_Instance *op_inst)
{
     Thread_Intention_Block *t_int_bl =  MAKE_OBJECT(Thread_Intention_Block);

     if (op_inst->intended) 
       fprintf(stderr,LG_STR("WARNING: This OP Instance has already been intended.\n",
			     "ATTENTION: Cette OP Instance a déjà été intendée.\n"));
     
     t_int_bl->curr_op_inst = dup_op_instance(op_inst);
     op_inst->intended = TRUE;
     t_int_bl->current_node = op_inst->op->start_point;
     t_int_bl->current_goal = NULL;
     t_int_bl->list_to_try = NULL;
     t_int_bl->pere = NULL;
     t_int_bl->intention = NULL;
     t_int_bl->activation_condition= NULL;
     t_int_bl->maintain_condition= NULL;
     t_int_bl->maintain = NULL;
     t_int_bl->fils = sl_make_slist();
     t_int_bl->num_called = 0;
     t_int_bl->status = TIBS_ACTIVE;
     t_int_bl->suspended = NULL;
     reinstall_frame(TIB_FRAME(t_int_bl));

     return t_int_bl;
}

Op_Instance *tib_op_instance(Thread_Intention_Block *tib)
{
     return tib->curr_op_inst;
}

Op_Instance *intention_bottom_op_instance(Intention *in)
{
     if (! sl_slist_empty(in->fils))
	  return tib_op_instance((Thread_Intention_Block *)sl_get_slist_head(in->fils));
     else
	  return NULL;
}

void fprint_tib(FILE *f,Thread_Intention_Block *tib)
{
     fprintf(f,LG_STR("<Thread %p>",
		      "<Thread %p>"), tib);
}

void sprint_tib(Sprinter *sp, Thread_Intention_Block *tib)
{
     SPRINT(sp, 14 + MAX_PRINTED_POINTER_SIZE,sprintf(f,LG_STR("<Thread %p>",
							       "<Thread %p>"), tib));
}

void print_tib(Thread_Intention_Block *tib)
{
     fprint_tib(stdout, tib);
}

void fprint_intention(FILE *f,Intention *in)
{
     if (in && in->id)
	  fprintf(f,LG_STR("<Intention %p (%s)>",
			   "<Intention %p (%s)>"), in, in->id);
     else
	  fprintf(f,LG_STR("<Intention %p>",
			   "<Intention %p>"), in);
}

void print_intention(Intention *in)
{
     fprint_intention(stdout, in);
}

void sprint_intention(Sprinter *sp, Intention *in)
{
     if (in->id == NULL) {
	  SPRINT(sp,13 + MAX_PRINTED_POINTER_SIZE,
				    sprintf(f,LG_STR("<Intention %p>",
						     "<Intention %p>"),in));
     } else {
	  SPRINT(sp,15  + strlen (in->id) + MAX_PRINTED_POINTER_SIZE,
				    sprintf(f,LG_STR("<Intention %p (%s)>",
						     "<Intention %p (%s)>"),in, in->id));
     } 
}

void free_thread_intention_block(Thread_Intention_Block *t_int_bl)
{
     if (t_int_bl->activation_condition) 
	  desinstall_and_free_condition(t_int_bl->activation_condition);
     if (t_int_bl->maintain_condition) 
	  desinstall_and_free_condition(t_int_bl->maintain_condition);
     free_op_instance(t_int_bl->curr_op_inst);
     FREE_SLIST(t_int_bl->fils);
     SAFE_SL_FREE_SLIST(t_int_bl->list_to_try);
     if (t_int_bl->current_goal) free_goal(t_int_bl->current_goal);

/* the thread has been removed, set this flag if anybody still points to it.
 */
     t_int_bl->status = TIBS_DEAD;
     FREE(t_int_bl);
}

void begin_critical_section(Thread_Intention_Block *tib)
{
     Intention_Graph *ig = current_oprs->intention_graph;

     current_oprs->critical_section = tib->pere;     /* */
     
     /* Clean the current intentions list. */

     FREE_SLIST(ig->current_intentions);
     ig->current_intentions = sl_make_slist();
     sl_add_to_tail(ig->current_intentions, current_tib->intention);

     tib->intention->critical_section = tib;
}

void end_critical_section(Thread_Intention_Block *tib)
{
     current_oprs->critical_section = NULL;
     tib->intention->critical_section = NULL;
}

void propagate_critical_section(Thread_Intention_Block *tib)
{
     if (current_oprs->critical_section == tib) { 
	  printf (LG_STR("WARNING:  Exiting the OP where we start the critical section.\n\
\tForce exit critical section.\n",
			 "ATTENTION:  Sortie du OP où a été démarré la section critique.\n\
\tSortie forcée de la section critique.\n"));
	  end_critical_section(tib);
     } else 
	  tib->intention->critical_section = tib->pere;
}

void mark_critical_section(Thread_Intention_Block *tib)
{
     tib->intention->critical_section = tib;
}

void unmark_critical_section(Thread_Intention_Block *tib)
{
     tib->intention->critical_section = NULL;
}

Intention *make_intention_for_goal(Op_Instance *op_inst,Intention_Graph *ig, int prio, Goal *goal)
{
     Thread_Intention_Block *t_in_bl;
     Intention *res = make_intention(op_inst,ig,prio);
     
     if (goal->interne){
	  fprintf(stderr,LG_STR("ERROR: Should not create an intention for\n\
\tan already intended goal.\n",
				"ERREUR: Ne devrait pas créer une intention\n\
\tpour un but déjà intendé.\n"));
     }
     goal->interne = res;	/* This is to make sure it will be posted in the same... */
     t_in_bl = (Thread_Intention_Block *)sl_get_slist_head(res->fils);
     t_in_bl->current_goal = goal; /* But the goal is ready to be executed... */
     goal->tib = t_in_bl;
     DUP(t_in_bl);

     return res;
}

Intention *make_intention(Op_Instance *op_inst,Intention_Graph *ig, int prio)
{
     Intention *in =  MAKE_OBJECT(Intention);
     Thread_Intention_Block *tib;

     in->fils = sl_make_slist();
     sl_add_to_tail(in->fils,tib = make_thread_intention_block(op_inst));
     tib->intention = in;
     in->fact = (op_inst->fact? dup_fact(op_inst->fact):NULL);
     in->goal = (op_inst->goal? dup_goal(op_inst->goal):NULL);
     in->active_tibs = sl_make_slist() ;
     sl_add_to_tail(in->active_tibs,tib);
     in->activation_conditions_list = sl_make_slist();
     TIME_STAMP(&in->creation);
#ifdef OPRS_PROFILING
     in->active = zero_date;
#endif
     in->top_op = op_inst;
     in->priority = prio;
     in->id = NULL;
     in->critical_section = NULL;
     in->status = IS_ACTIVE;
     in->failed_goal_sprinter = make_sprinter(0);
     in->failed_goal_stack = sl_make_slist();
#ifdef GRAPHIX
#ifdef GTK
     in->iog =  (debug_trace[GRAPHIC_INTEND] && global_int_draw_data ? create_ginode(global_int_draw_data, inCGCsp, in): NULL);
#else
     in->iog =  (debug_trace[GRAPHIC_INTEND] && global_int_draw_data ? create_ginode(global_int_draw_data, in): NULL);
#endif
     in->trace_dialog = NULL;
     in->trace_scrl_txt = NULL;
#endif

     return in;
}

void delete_intention(Intention *in)
{
     Relevant_Condition *rc;

#ifdef GRAPHIX
     if (global_int_draw_data && in->iog) {
#ifdef GTK
       destroy_ginode(global_int_draw_data, inCGCsp, in->iog);
#else
       destroy_ginode(global_int_draw_data, in->iog);
#endif
       in->iog = NULL;
     }
#endif

     remove_and_free_tib_list(in->fils); /* In case our sons are still around ... */
     
     FREE_SLIST(in->fils); 

     FREE_SLIST(in->active_tibs);

     sl_loop_through_slist(in->activation_conditions_list, rc, Relevant_Condition *) {
	  desinstall_and_free_condition(rc);
     }

     FREE_SLIST(in->activation_conditions_list);
     
     free_intention (in);
}

void free_intention(Intention *in)
{ 
     if (LAST_REF(in)){
	  whipe_out_goal_failure(in);
	  free_sprinter(in->failed_goal_sprinter);
	  sl_free_slist(in->failed_goal_stack);
	  if (in->fact) free_fact(in->fact);
	  if (in->goal) free_goal(in->goal);
#ifdef GRAPHIX
	  if (global_int_draw_data && in->trace_dialog)
	       XtDestroyWidget(in->trace_dialog);
#endif
     }
     FREE(in);
}

Intention *dup_intention(Intention *in)
{
     DUP(in);
     return in;
}

void set_intention_id(Intention *in, PString id)
{
     in->id = id;
#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
       touch_intention_ginode(in);
       draw_intention_graph(global_int_draw_data);
     }
#endif
}

void set_intention_priority(Intention *in, int priority)
{
     in->priority = priority;
#ifdef GRAPHIX
     if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
       touch_intention_ginode(in);
       draw_intention_graph(global_int_draw_data);
     }
#endif
}

PBoolean action_first_call()
{
     return (current_tib && (current_tib->num_called == 0));
}

int action_number_called()
{
     return (current_tib ? current_tib->num_called : 0);
}

int intention_priority(Intention *in)
{
	return in->priority;
}

Fact *intention_fact(Intention *in)
{
	return in->fact;
}

Goal *intention_goal(Intention *in)
{
	return in->goal;
}

PString intention_id(Intention *in)
{
	return in->id;
}

long intention_time(Intention *in)
{
	return in->creation.tv_sec;
}

void sprint_show_tib(Sprinter *sp, Thread_Intention_Block *tib, int indent)
{
     Op_Structure *op;

     SPRINT(sp,indent,sprintf(f,"%*s", indent,""));
     sprint_tib(sp,tib);
     SPRINT(sp,16+strlen(tib->curr_op_inst->op->name),
	    sprintf(f,LG_STR(" in OP: %s,",
			     " dans le OP: %s,"), tib->curr_op_inst->op->name));
     SPRINT(sp,16,sprintf(f,LG_STR(" which is: ",
				   " qui est: ")));
     switch(tib->status) {
     case TIBS_SUSP_ACTIVE:
	  SPRINT(sp,64,sprintf(f,LG_STR("SUSPENDED by an active maintenance retry but is ",
					"SUSPENDU par un rétablissement de maintance active, mais est ")));
     case TIBS_ACTIVE:
	  op = tib->curr_op_inst->op;

	  if (op == goal_satisfied_in_db_op_ptr) /* Satisfied already in the DB */
	       SPRINT(sp,64,sprintf(f,LG_STR("ACTIVE and has satisfied its goal in the database.",
					     "ACTIF et a satisfait son but dans la base de données.")));
	  else if (op == goal_waiting_op_ptr) /* Waiting goal */
	       SPRINT(sp,64,sprintf(f,LG_STR("ACTIVE but is going to sleep.",
					     "ACTIF et va s'endormir.")));
	  else if (op->action != NULL) /*Action OP */
	       SPRINT(sp,64,sprintf(f,LG_STR("ACTIVE and executes or will execute an action.",
					     "ACTIF et exécute ou va exécuter une action.")));
	  else {		/* standard OP */
	       if (tib->current_goal) {
		    SPRINT(sp,indent + 64,sprintf(f,LG_STR("ACTIVE and has posted the goal:\n%*s",
							   "ACTIF et a posté le but:\n%*s"), indent,""));
		    sprint_goal(sp,tib->current_goal);
		    SPRINT(sp,2,sprintf(f,": "));
		    sprint_expr(sp,tib->current_goal->statement);
	       } else {
		   SPRINT(sp,64,sprintf(f,LG_STR("ACTIVE but has not posted any goal yet.",
						 "ACTIF mais n'a pas encore posté de but.")));
	       }
	  }
	  break; 
     case TIBS_SUSP_SLEEPING:
	  SPRINT(sp,64,sprintf(f,LG_STR("SUSPENDED by an active maintenance retry but is ",
					"SUSPENDU par un rétablissement de maintance active, mais est ")));
     case TIBS_SLEEPING:
	  SPRINT(sp,64 + indent,sprintf(f,LG_STR("SLEEPING on the condition:\n%*s",
						 "ENDORMI sur la condition:\n%*s"), indent,""));
	  sprint_expr(sp,tib->activation_condition->expr);
	  break;
     case TIBS_JOINING:
	  SPRINT(sp,64 + 32 + strlen(tib->curr_op_inst->op->name),
		 sprintf(f,LG_STR("JOINING on node %p in OP %s.",
				  "EN ATTENTE d'etre rejoint sur le noeud %p in OP %s."),
			 tib->current_node,
			 tib->curr_op_inst->op->name));
	  break;
     case TIBS_ACTION_WAIT:
	  SPRINT(sp,64 + 32 + strlen(tib->curr_op_inst->op->name),
		 sprintf(f,LG_STR("ACTION WAITING in OP %s.",
				  "ATTENTE d'ACTION dans l'OP %s."),
			 tib->curr_op_inst->op->name));
	  break;
     case TIBS_DEAD:
	  SPRINT(sp, 6, sprintf(f,LG_STR("DEAD.",
					 "MORTE.")));
	  break;
     }

     if (tib->suspended) {
	  SPRINT(sp,64 + indent,sprintf(f,LG_STR("\n%*sand has the following tib suspended:\n",
						 "\n%*set a les tibs suivant suspendu:\n"), indent, ""));
	  SPRINT(sp,indent + 6,sprintf(f,"%*s{\n", indent + 3, ""));
	  sprint_show_tib(sp, tib->suspended, indent + 4);
	  SPRINT(sp,indent + 6,sprintf(f,"%*s}\n", indent + 3, ""));
     }

     if (tib->maintain) {
	  SPRINT(sp,indent + 64,sprintf(f,LG_STR("\n%*sand has the following maintenance condition: ",
						 "\n%*set a les conditions de maintance suivantes: "), indent, ""));
	  sprint_expr(sp,tib->maintain);
     }

     if  (! (sl_slist_empty(tib->fils))) {
	  SPRINT(sp,64 + indent,sprintf(f,LG_STR("\n%*sand has the following son(s):\n",
						 "\n%*set a les fils suivants:\n"), indent, ""));
	  sprint_show_tib_list(sp,tib->fils, indent + 4);
     } else
	  SPRINT(sp,1,sprintf(f,"\n"));
}


void sprint_show_tib_list(Sprinter *sp, Thread_Intention_Block_List tib_list, int indent)
{
     Thread_Intention_Block *tib;

     SPRINT(sp,indent + 2,sprintf(f,"%*s{\n", indent - 1, ""));
     sl_loop_through_slist(tib_list, tib, Thread_Intention_Block *)
	  sprint_show_tib(sp,tib, indent);
     SPRINT(sp,indent + 2,sprintf(f,"%*s}\n", indent - 1 , ""));
}
