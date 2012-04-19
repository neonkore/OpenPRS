/*                               -*- Mode: C -*- 
 * fact-goal.c -- 
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


/*	For timeval definition. */
#ifdef WIN95
#endif


#include "constant.h"


#include "fact-goal.h"

#include "fact-goal_f.h"
#include "op-instance_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#include "oprs-type_f.h"
#include "op-instance_f.h"

/*
 * Facts, goals and OPInstance are pointing each other, as a result you have to free them in a
 * specific order, or you will loose... This explain the different functions for when you free
 * from a op-instance and from somewhere else.
 */

Fact *make_fact_from_expr(Expression *expr)
/* Make a fact. The gexpr is expected to be usable... */
{
     Fact *fact = MAKE_OBJECT(Fact);
     
     fact->statement = expr;
     fact->pris_en_compte = FALSE;
#ifdef OPRS_PROFILING
     TIME_STAMP(&fact->creation);
     fact->reception = zero_date;
     fact->soak = zero_date;
     fact->reponse = zero_date;
#endif
     fact->executed_ops = 0;
     fact->message_sender = NULL;

     return fact;
}

Fact *dup_fact(Fact *fact)
/* dup a fact */
{
     DUP(fact);
     return fact;
}

void free_fact(Fact *fact)
{
     if (LAST_REF(fact)) {
	       free_expr(fact->statement);
     }
     FREE(fact);
}

void sprint_fact(Sprinter *sp, Fact *fact)
{
     SPRINT(sp,8 + MAX_PRINTED_POINTER_SIZE,sprintf(f,LG_STR("<Fact %p>",
							     "<Fait %p>"), fact));
}

void fprint_fact(FILE *fact,Fact* fa)
{
     fprintf(fact,LG_STR("<Fact: ",
			 "<Fait: "));
     fprint_expr(fact,fa->statement);
     fprintf(fact,">");
}

void print_fact(Fact* fact)
{
     fprint_fact(stdout,fact);
}

Goal *make_goal_from_expr(Expression *expr, FramePtr frame)
/* Make a goal. Here also the gtexpr is usable but the frame is duped... */
{
     Goal *goal = MAKE_OBJECT(Goal);

     goal->statement = expr;
     goal->pris_en_compte = FALSE;
#ifdef OPRS_PROFILING
     TIME_STAMP(&goal->creation);
     goal->reception = zero_date;
     goal->soak = zero_date;
     goal->reponse = zero_date;
#endif
     goal->frame = dup_frame(frame);
     goal->etat = FALSE;
     goal->interne = NULL;
     goal->tib = NULL;
     goal->succes = NULL;
     goal->echec = NULL;
     goal->echoue = NOT_FAILED;
     goal->cp = NULL;
     goal->edge = NULL;

     return goal;
}

FramePtr copy_frame(FramePtr frame)
{
     FramePtr fp = MAKE_OBJECT(Frame);

     fp->list_undo = NULL;
     fp->previous_frame = dup_frame(frame->previous_frame); 
     fp->installe = TRUE;

     return fp;
}

Goal *copy_goal(Goal *old_goal)
{
     Goal *goal = MAKE_OBJECT(Goal);

     goal->statement = dup_expr(old_goal->statement);
     goal->pris_en_compte = FALSE;
#ifdef OPRS_PROFILING
     TIME_STAMP(&goal->creation);
     goal->reception = zero_date;
     goal->soak = zero_date;
     goal->reponse = zero_date;
#endif
     goal->frame = dup_frame(old_goal->frame);
     goal->etat = FALSE;
     goal->interne = NULL;
     goal->tib = NULL;
     goal->succes = NULL;
     goal->echec = NULL;
     goal->echoue = NOT_FAILED;
     goal->cp = NULL;
     goal->edge = NULL;

     return goal;
}

Goal *dup_goal(Goal *goal)
/* dup a goal */
{
     DUP(goal);
     return goal;
}

void free_goal_from_op_instance(Goal *goal)
{
      if (REF(goal) == (SAFE_SL_SLIST_LENGTH(goal->echec))) {
	  Op_Instance *opi;
	  PBoolean pointed = FALSE;
	  int i;

	  SAFE_SL_LOOP_THROUGH_SLIST(goal->echec, opi, Op_Instance *)
	       pointed = pointed || (! LAST_REF(opi));
	       
	  if (! pointed) {
	       if (goal->echec) sl_loop_through_slist(goal->echec, opi, Op_Instance *)
		    free_op_instance_not_goal(opi);
	       
	       free_expr(goal->statement);
	       SAFE_SL_FREE_SLIST(goal->echec); 
	       free_frame(goal->frame);

	       /* Nobody points on it now... Give it back to the memory manager. */
	       if (goal->tib) {
		    FREE(goal->tib);
	       }
	       for (i = REF (goal); i > 0 ; i--){
		    FREE(goal);
	       }

	  } 
     }
}

void free_goal(Goal *goal)
{
     if (REF(goal) == (SAFE_SL_SLIST_LENGTH(goal->echec) + 1)) {
	  Op_Instance *opi;
	  PBoolean pointed = FALSE;

	  SAFE_SL_LOOP_THROUGH_SLIST(goal->echec, opi, Op_Instance *)
	       pointed = pointed || (! LAST_REF(opi)); /*  (REF(opi) == 2); */
	       
	  if (! pointed) {
	       SAFE_SL_LOOP_THROUGH_SLIST(goal->echec, opi, Op_Instance *) {
		    free_op_instance_not_goal(opi);
		    FREE(goal);
	       }

	       free_expr(goal->statement);
	       SAFE_SL_FREE_SLIST(goal->echec); 
	       free_frame(goal->frame);
	  } 
     }
     if (LAST_REF(goal) && goal->tib) {
	  FREE(goal->tib);
     }
     FREE(goal);
}

void sprint_goal(Sprinter *sp, Goal *g)
{ 
     SPRINT(sp,8 + MAX_PRINTED_POINTER_SIZE,sprintf(f,LG_STR("<Goal %p>",
							     "<But %p>"), g));
}
 
void fprint_goal(FILE *f,Goal* goal)
{
     fprintf(f,LG_STR("<Goal: ",
		      "<But: "));
     fprint_expr(f,goal->statement);
     fprintf(f,">");
}

void print_goal(Goal* goal)
{
     fprint_goal(stdout,goal);
}

Goal *make_goal_from_edge(Expression *expr, FramePtr frame, Thread_Intention_Block *thread, Control_Point *cp, Edge *edge)
{
     Goal *goal = make_goal_from_expr(expr,frame);

     if (! expr) 
	  goal->succes = (Op_Instance *)TRUE;

     goal->interne = thread->intention;
     goal->tib = thread;
     DUP(thread);

     goal->cp = cp;
     goal->edge = edge;
     
     return goal;
}

PString fact_sender(Fact *fact)
{
     return fact->message_sender;
}

Expression *fact_statement(Fact *fact)
{
     return fact->statement;
}

Expression *goal_statement(Goal *goal)
{
     return goal->statement;
}

Intention *goal_intention(Goal *goal)
{
     return goal->interne;
}

#ifdef OPRS_PROFILING
PDate fact_soak(Fact *fact)
{
	return fact->soak;
}

PDate fact_creation(Fact *fact)
{
	return fact->creation;
}

PDate fact_reception(Fact *fact)
{
	return fact->reception;
}

PDate fact_response(Fact *fact)
{
	return fact->reponse;
}

PDate goal_soak(Goal *goal)
{
	return goal->soak;
}

PDate goal_creation(Goal *goal)
{
	return goal->creation;
}

PDate goal_reception(Goal *goal)
{
	return goal->reception;
}

PDate goal_response(Goal *goal)
{
	return goal->reponse;
}

#endif
