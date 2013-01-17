/*                               -*- Mode: C -*- 
 * intention.h -- Definition of intention and its components.
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


#ifndef INCLUDE_intention
#define INCLUDE_intention

#include "slistPack.h"
#include "slistPack_f.h"

#ifdef GRAPHIX
#ifdef GTK
#include <gtk/gtk.h>
#include "xm2gtk.h"
#else
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif
#endif

#include "opaque.h"
#include "oprs-type.h"
#include "oprs-profiling.h"
#include "conditions.h"

/*
 * An intention has one of the following status :
 * IS_ACTIVE : default status
 * IS_SLEEPING : the whole intention has been asleep on one (or more) particular condition
 * IS_SUSP_ACTIVE : the intention is active, but all its active threads are sleeping.
 * IS_SUSP_SLEEPING : the intention has no more active threads,
 *                    and has been asleep on a condition.
 */

typedef enum {IS_ACTIVE, IS_SLEEPING, IS_SUSP_ACTIVE, IS_SUSP_SLEEPING} Intention_Status;

/*
 * A thread intention block has one of the following status :
 * TIBS_ACTIVE : the thread is active
 * TIBS_JOINING : the thread has been succeded, and is waiting for its brothers.
 * TIBS_SLEEPING : the thread has been asleep by a waiting goal.
 * TIBS_SUSP_ACTIVE : the thread is active, but has been suspended by the failure
 *                    of an active maintain condition.
 * TIBS_SUSP_SLEEPING : the thread has been asleep by a waiting goal,
 *                      and has been suspended by the failure of an active maintain condition. 
 * TIBS_DEAD : the thread has been removed, set this flag if anybody still points to it.
 */

typedef enum {TIBS_ACTIVE, TIBS_SLEEPING, TIBS_JOINING, TIBS_SUSP_ACTIVE, TIBS_SUSP_SLEEPING, TIBS_DEAD, TIBS_ACTION_WAIT} Thread_Intention_Status;

struct intention {
     Fact * fact;
     Goal * goal;
     Thread_Intention_Block_List fils;
     Thread_Intention_Block_List active_tibs;
     Op_Instance *top_op;
     short priority;
     Intention_Status status;
     Symbol id;
     Thread_Intention_Block *critical_section;
     PDate creation;	
     Condition_List activation_conditions_list;
#ifdef OPRS_PROFILING
     PDate active;	
#endif
     Sprinter *failed_goal_sprinter; 
     OPRS_LIST failed_goal_stack; 
#ifdef GRAPHIX
     IOG *iog;
     Widget trace_dialog;
     Widget trace_scrl_txt;
#endif
};

struct thread_intention_block {
     Op_Instance *curr_op_inst;
     Intention *intention;	/* Its intention. */
     Thread_Intention_Block *pere; /* Pere */
     Thread_Intention_Block_List fils; /* Fils */
     Node *current_node;	
     Goal *current_goal;
     EdgeList list_to_try;
     Expression *maintain; /* For preserve */
     Relevant_Condition *activation_condition;
     Relevant_Condition *maintain_condition;
     Thread_Intention_Block *suspended;
     Thread_Intention_Status status;
     unsigned short num_called;
};

extern Thread_Intention_Block *current_tib;
extern Intention *current_intention;

typedef ExprFrame * ExprFrame_Ptr;

extern const ExprFrame_Ptr force_sleeping_condition_ef_ptr;

#define TIB_FRAME(tib) ((tib)->curr_op_inst->frame)
#define TIB_GOAL(tib) ((tib)->curr_op_inst->goal)
#define TIB_INTENTION(tib) ((tib)->intention)

#endif /* INCLUDE_intention */
