
/*                               -*- Mode: C -*-
 * top-lev-loop.c -- Main loop of a OPRS agent
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

#include "config.h"

#ifdef WINSOCK
#include <winsock.h>
#else
#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#include "selectLib.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#endif
#endif

#include "string.h"
#include <errno.h>

#include "slistPack.h"
#include "slistPack_f.h"

#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "oprs.h"
#include "top-lev.h"

#include "activate_f.h"
#include "oprs-type_f.h"
#include "oprs_f.h"
#include "top-lev_f.h"
#include "soak_f.h"
#include "op-instance_f.h"
#include "intend_f.h"

extern Slist *_dynamic_slist_list;

void let_the_good_time_roll(Oprs *oprs)
/*
 * let_the_good_time_roll - will wait until there is something to do for the main loop. It
 *                          will timeout after a while so the sleeping intention could be checked.
 */
{
     fd_set readfds;
     struct timeval pool_tv;
     int max_fds;
     /*
      * We end up here if we have nothing to do... just sit and relax... We first check for new facts or new goals,
      * and if there is nothing, we might as well wait on stdin and mp_oscket to see if a command is comming. This
      * select is timing out on pool_tv. If we time out, go and check the waiting condition. This is done in
      * find_soak();
      */

     if ((!(sl_slist_empty(oprs->new_facts))) ||
	       (!(sl_slist_empty(oprs->new_goals))))
	  return;

     check_and_sometimes_compact_list();

     set_readfds(&readfds, &max_fds, FALSE);

     pool_tv.tv_sec = main_loop_pool_sec;
     pool_tv.tv_usec = main_loop_pool_usec;

#if defined(HAVE_SETITIMER) && defined(WANT_TRIGGERED_IO) && defined(HAVE_SIGACTION)
     block_sigalarm();		/* We should sleep/select for the remaining of the itimer time. */
#endif

     int found;

     PROTECT_SYSCALL_FROM_EINTR(found,select(max_fds, &readfds, NULL, NULL, &pool_tv))
     if (found == -1)
	  perror("top-lev-loop: select NULL");

#if defined(HAVE_SETITIMER) && defined(WANT_TRIGGERED_IO) && defined(HAVE_SIGACTION)
     unblock_sigalarm();	/* We should restart the timer with the remaining of pool_tv... */
#endif
}

void client_oprs_top_level_loop(Oprs *oprs)
/*
 * client_oprs_top_level_loop - This is the real main loop of any oprs agent. It should
 *                             never return... except when you quit of course.
 */
{
     Op_Instance_List soak;
     Op_Instance *opi1, *opi2;
#if defined(HAVE_SETITIMER) && defined(WANT_TRIGGERED_IO)
     long last_main_loop_pool_sec  = main_loop_pool_sec;
     long last_main_loop_pool_usec = main_loop_pool_usec;
#endif

     while (TRUE) {
#if defined(HAVE_SETITIMER) && defined(WANT_TRIGGERED_IO)
         if (last_main_loop_pool_sec != main_loop_pool_sec ||
	     last_main_loop_pool_usec != main_loop_pool_usec)
         {
	       desarm_condition_timer();
	       set_interval_timer();
	       arm_condition_timer();
	       last_main_loop_pool_sec  = main_loop_pool_sec;
	       last_main_loop_pool_usec = main_loop_pool_usec;
          }
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
	       if (meta_option[META_LEVEL]) {
		    while (!(SAFE_SL_SLIST_EMPTY(soak))) {
			 if (meta_option[META_LEVEL] && meta_option[SOAK_MF] &&
			     (! oprs->critical_section)) post_soak_meta_fact(soak, oprs);
		    
			 SAFE_SL_LOOP_THROUGH_SLIST(previous_soak, opi2, Op_Instance *)
			      free_op_instance(opi2);
			 SAFE_SL_FREE_SLIST(previous_soak);
			 previous_soak = soak;
		    
			 shift_facts_goals(oprs);
			 soak = find_soak(oprs);
		    }
		    if (!(SAFE_SL_SLIST_EMPTY(previous_soak))) {	/* soak empty but previous soak non empty */
			 if (run_option[PAR_INTEND]) {
			      previous_soak = reshuffle_randomly_soak_list(previous_soak);
			      sl_loop_through_slist(previous_soak, opi1, Op_Instance *)
				   intend(oprs->intention_graph, opi1, empty_list,empty_list,0);
			 } else
			      intend(oprs->intention_graph, (Op_Instance *)select_randomly_c_list(previous_soak),
				     empty_list,empty_list,0);
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
	       }
	       if (! activate(oprs->intention_graph)) let_the_good_time_roll(oprs); /*Nothing to do */;

	       SAFE_SL_LOOP_THROUGH_SLIST(previous_soak, opi2, Op_Instance *)
		    free_op_instance(opi2);
	       SAFE_SL_FREE_SLIST(previous_soak);
	       previous_soak = soak;
	  }
	  if (oprs_run_mode == STEP) set_oprs_run_mode(STEP_HALT);

     }
}
