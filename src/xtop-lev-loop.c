static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*-
 * top-lev-loop.c -- Main loop of a OPRS agent
 *
 * Copyright (c) 1991-2003 Francois Felix Ingrand.
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#include "constant.h"
#include "macro.h"


#include "oprs-type.h"
#include "oprs.h"
#include "top-lev.h"

#include "oprs-type_f.h"
#include "intend_f.h"
#include "oprs_f.h"
#include "top-lev_f.h"

#include "soak_f.h"

PBoolean xoprs_top_level_loop(Oprs *oprs)
{
     if (!flushing_xt_events) { /* We are flushing from yyparse... so do not screw up it environement. */

	  Op_Instance_List soak;
	  Op_Instance *opi1, *opi2;
	  PBoolean busy = FALSE;

	  if ( !meta_option[META_LEVEL] || !oprs->posted_meta_fact ) /* We check the stdin only if 
								    the Meta OPs have been intended. */
#ifdef HAS_TIMER
	  if (check_the_stdin) {
	       check_the_stdin = FALSE;
	       check_stdin();
	  }
#else
	  check_stdin();
#endif

	  if (oprs_run_mode != HALT) {
	       shift_facts_goals(oprs);

	       soak = find_soak(oprs);

	       if (meta_option[META_LEVEL]) { /* Note this is not a while, because we may hang the kernel for ever... */
		    if (!(SAFE_SL_SLIST_EMPTY(soak)) ) { 
			 if (meta_option[META_LEVEL] && meta_option[SOAK_MF] &&
			     (! oprs->critical_section)) {
			      post_soak_meta_fact(soak, oprs);
			      oprs->posted_meta_fact = TRUE;
			 }
			 busy = TRUE;
		    } else {
			 if (!(SAFE_SL_SLIST_EMPTY(previous_soak))) { /* soak empty but previous soak non empty */
			      if (run_option[PAR_INTEND])  {
				   previous_soak = reshuffle_randomly_soak_list(previous_soak);
				   sl_loop_through_slist(previous_soak, opi1, Op_Instance *)
					intend(oprs->intention_graph, opi1, empty_list,empty_list,0);
			      } else
				   intend(oprs->intention_graph, (Op_Instance *)select_randomly_c_list(previous_soak),
					  empty_list,empty_list,0);
			 }
			 busy = activate(oprs->intention_graph);
		    }
	       } else {		/* no option[META_LEVEL] */
		    if (!(SAFE_SL_SLIST_EMPTY(soak))) {
			 if (run_option[PAR_INTEND]) {
			      soak = reshuffle_randomly_soak_list(soak);
			      sl_loop_through_slist(soak, opi1, Op_Instance *)
				   intend(oprs->intention_graph, opi1, empty_list,empty_list,0);
			 } else
			      intend(oprs->intention_graph, (Op_Instance *) select_randomly_c_list(soak),
				     empty_list,empty_list,0);
		    }
		    busy = activate(oprs->intention_graph);
	       }
	       SAFE_SL_LOOP_THROUGH_SLIST(previous_soak, opi2, Op_Instance *)
		    free_op_instance(opi2);
	       SAFE_SL_FREE_SLIST(previous_soak);
	       previous_soak = soak;

	  }
	  if (oprs_run_mode == STEP) set_oprs_run_mode(STEP_HALT);
	  if (! busy) {		/* Nothing to do... */
	       check_and_sometimes_compact_list();
	       deregister_main_loop(oprs);
	       return TRUE;
	  }
     }
     return FALSE;		/* This is a work procedure.... Never deregister it */
}
