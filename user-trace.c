static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * user-trace.c -- 
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

/*
  Here is a first shoot at a user called trace facility. 

  Basically, the OPRS code has been modified to call some predefined function
  when some flags are on. As there is no pointin making such feature in the
  standard kernel, this only works in non standard oprs/xoprs kernel (i.e. it
  only works with the one you build with the relocatables).

  The traces which now accept hooks are close to the standard oprs trace. They can
  be set on or off using the following commands (a similar menu is now
  available under xoprs).

  trace user trace on		; this is the global switch to enable/disable
                                ; the whole mecanism.
  trace user op on              ; trace op execution (goal posting/unposting, action).
  trace user intend on          ; trace when a new op instance is intended
  trace user suc_fail on        ; trace op success/failure.
  trace user send on            ; trace messages sent/broadcasted/multicasted/
  trace user receive on         ; trace messages received.
  trace user fact on            ; trace the new facts posted in the kernel.
  trace user goal on            ; trace the new goals posted in the kernel.
  trace user relevant op on     ; undefinef for now
  trace user applicable op on   ; undefined for now
  trace user thread on          ; undefined for now

  examples of the functions which are called by these user trace are defined
  below. Their prototype are defined in user-trace_f.h.

  each function is exlpained sparately.

*/

#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "fact-goal_f-pub.h"
#include "user-trace.h"
#include "user-trace_f.h"
#include "oprs-print_f-pub.h"


void user_trace_post_goal(Goal *goal)
{
     printf("User Trace: Posting the goal: ");
     print_expr(goal_statement(goal));
     printf("\n");
 
     return;
}

void user_trace_post_fact(Fact *fact)
{
     printf("User Trace: Posting the fact: ");
     print_expr(fact_statement(fact));
     printf("\n");
     return;
}

void user_trace_receive_message(Fact *fact, PString sender)
{
     printf("User Trace: getting the message: ");
     print_expr(fact_statement(fact));
     printf(" from %s.\n", sender);
    return;
}

void user_trace_send_message(PString message, unsigned int nb_recs, PString *recs)
{
     /* 0 is broadcast, 1 is message to one repository, n is multicast. */
     printf("User Trace: sending the message: %s to: ", message);
     
     if (! nb_recs)
	  printf("everybody (broadcast).\n");
     else {
	  int i;
	  for (i=0; i!= nb_recs; i++)
	       printf(" %s", recs[i]);
	  printf(".\n");
     }
     
     return;
}

void user_trace_suc_fail(Op_Instance *op_ins, Goal* goal, Fact *fact, PBoolean success)
{
     /* Will trace when a op_ins lead to a succes or a failure.
	the goal OR a fact (one will be NULL) which lead to this op_instance is
	also passed. */
     printf("User Trace: OP Instance %p, Goal %p, Fact %p : %s.\n",
	     op_ins, goal, fact, (success? "Succeed":"Failed"));

     return;
}

void user_trace_intend(Thread_Intention_Block *tib,Op_Instance *op_inst)
{
     /* Will be called when intending a new op_instance. 
	If tib == NULL intending in a new intention, otherwise it points to the
	tib in which it will be intended. */
     printf("User Trace: Intending op_inst %p in tib %p.\n",
	     op_inst, tib);

     return;
}

void user_trace_op(Thread_Intention_Block *tib,Op_Instance *op_inst, Goal *goal, int what)
{
     /* 0 for action op, 1 posting a goal/edge, 2 unposting a goal/edge. */
     switch (what) {
     case 0:
	  printf("User Trace: Action OP in tib %p, op_inst %p.\n",
		 tib,op_inst);
	  break;
     case 1:
	  printf("User Trace: Posting goal %p in tib %p, op_inst %p.\n",
		 goal, tib, op_inst);
	  break;
     case 2:
	  printf("User Trace: Unposting goal  %p in OP in tib %p, op_inst %p.\n",
		 goal, tib, op_inst);
	  break;
     }
     return;
}
