
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

#ifdef WIN95
#include "winsock.h"
#else
#ifdef VXWORKS
#include "vxWorks.h"
#include "ioLib.h"
#include "sockLib.h"
#include "selectLib.h"
#include <systime.h>
#include <socket.h>
#include <types.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdlib.h>
#endif
#endif

#include <errno.h>

#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "oprs.h"
#include "top-lev.h"
#include "int-graph.h"
#include "op-instance.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-sprint.h"
#include "oprs-main.h"
#include "parser-funct.h"

#include "oprs-init_f.h"
#include "top-lev_f.h"
#include "soak_f.h"
#include "oprs_f.h"
#include "intend_f.h"
#include "activate_f.h"
#include "lisp-list_f.h"
#include "oprs-type_f.h"
#include "fact-goal_f.h"
#include "socket_f.h"
#include "int-graph_f.h"
#include "op-instance_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"
#include "oprs-sprint_f.h"
#include "parser-funct_f.h"

#include "top-lev_f-pub.h"

#ifdef GRAPHIX
#include "xoprs-main_f.h"
#endif

long main_loop_pool_sec = 0L;
long main_loop_pool_usec = 100000L;

Oprs_Run_Type oprs_run_mode = RUN;
PBoolean flushing_xt_events = FALSE;

PBoolean check_the_stdin = TRUE;
PBoolean stop_checking_the_stdin = TRUE;

void report_check_stdin_ctxt_error(void) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,128,sprintf(f,LG_STR("The previous error occurred while parsing external inputs.\n\
Proceed at your own risk.\n",
				    "The previous error occurred while parsing external inputs.\n\
Proceed at your own risk.\n")));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

PBoolean safe_parse_one_or_more()
{
     PBoolean leave_it = FALSE;
     Error_Ctxt previous_error_ctxt = current_error_ctxt;

     current_error_ctxt = EC_CHECK_STDIN;

     stop_checking_the_stdin = FALSE;

     if (setjmp(check_stdin_jmp) == 0) 
	  leave_it = parse_one_or_more();
     else
	  report_check_stdin_ctxt_error();

     current_error_ctxt = previous_error_ctxt;

     return leave_it;
}

static fd_set external_readfds;
static int max_external_readfds = -1; /* -1 means these 2 values have not been set yet... */

void add_external_readfds(int ext_fds)
{
     if (max_external_readfds == -1) {
	  FD_ZERO(&external_readfds);
	  max_external_readfds = 0;
     }

     FD_SET(ext_fds, &external_readfds);
     max_external_readfds = MAX(max_external_readfds,ext_fds+1);
#ifdef GRAPHIX
     register_other_inputs(current_oprs);
#endif
}

void remove_external_readfds(int ext_fds)
{
     if (max_external_readfds == -1) {
	  return;		/* This should be an error... */
     }

     FD_CLR(ext_fds, &external_readfds);

     int i, new_max_external_readfds = 0;
     /* we loop thru the new set and find the new max... */
     for (i = 0; i < max_external_readfds; i++) {
	  if (FD_ISSET(i, &external_readfds))
	       new_max_external_readfds = i+1;
     }
     max_external_readfds = new_max_external_readfds;
#ifdef GRAPHIX
     register_other_inputs(current_oprs);
#endif
}

void set_readfds(fd_set *readfdsp, int *max_fds, PBoolean stdin_only)
{
     FD_ZERO(readfdsp);
     *max_fds = 0;
     if (register_to_server) {
	  FD_SET(ps_socket, readfdsp);
	  *max_fds = MAX(*max_fds,ps_socket+1);
     }
     if (register_to_mp) {
	  FD_SET(mp_socket, readfdsp);
	  *max_fds = MAX(*max_fds,mp_socket+1);
     }
     
     if ((! stdin_only) &&  (max_external_readfds != -1)) {
	  int i;
	  for (i = 0; i < max_external_readfds; i++) {
	       if (FD_ISSET(i, &external_readfds))
		    FD_SET(i, readfdsp);
	  }
	  *max_fds = MAX(*max_fds,max_external_readfds);
     }
}


void check_stdin()
{
     fd_set readfds, save_readfds;
     struct timeval zero_tv;
     int nfound, max_fds;
     char buf = '\0';

     zero_tv.tv_sec = 0;
     zero_tv.tv_usec = 0;

     set_readfds(&save_readfds,&max_fds,TRUE);
     
     readfds = save_readfds;

     /* The following part of the code is used only to get command
      * while we are running... Hopefully, we will only get a few of
      * theses, and by all means, we should not be blocking here on
      * the yyparse (called in parse_one_or_more).  */

     while ((nfound = select(max_fds, &readfds, NULL, NULL, &zero_tv)) != 0) {
	  /* There is something on the socket... */

	  Parse_Source_Type previous_parse_source;
	  int nbc;

	  if (nfound < 0)
	       if (errno != EINTR) {
		    perror("check_stdin: select");
	       }

	  if (register_to_server && FD_ISSET(ps_socket, &readfds)) {
	       if ((nbc = recv(ps_socket, &buf, 1, MSG_PEEK)) < 0)
		    /* Lets peek a char and see what is it? */
#ifndef WINSOCK
		    if (errno != ENOTSOCK) {
#endif
			 perror("check_stdin: ps_socket PEEK: recv");
#ifndef WINSOCK
		    }
#endif
	       if (!nbc) {	/* The socket has been closed. */
		    wrap_up();
		    exit(1);
	       }

	       switch (buf) {
	       case ' ':
	       case '\n':
	       case '\t':
		    if (recv(ps_socket, &buf, 1, 0x0) < 0)	/* lets discard uninteresting char */
			 perror("check_stdin: ps_socket READ: recv");
		    break;
	       default:
		    previous_parse_source = parse_source;
		    parse_source = PS_SOCKET;
		    yy_switch_to_yy_ps(); 
		    safe_parse_one_or_more();	/* The char is interesting... Hum! commands are coming baby, we
						 * parse. */
		    parse_source = previous_parse_source ;

		    break;
	       }
	  }
	  if (register_to_mp && FD_ISSET(mp_socket, &readfds)) {
	       PBoolean leave_it = FALSE;

	       if ((nbc = recv(mp_socket, &buf, 1, MSG_PEEK)) < 0)	/* Lets clean the socket? */
		    perror("check_stdin: mp_socket PEEK: recv");

	       if (!nbc) {	/* The socket has been closed. */
		    wrap_up();
		    exit(1);
	       }

	       switch (buf) {
	       case ' ':
	       case '\n':
	       case '\t':
		    if (recv(mp_socket, &buf, 1, 0x0) < 0)	/* lets discard uninteresting char */
			 perror("check_stdin: mp_socket READ: recv");
		    break;
	       default:
		    previous_parse_source = parse_source;
		    parse_source = PS_SOCKET;
		    yy_switch_to_yy_mp(); 
		    leave_it = safe_parse_one_or_more();
		    parse_source = previous_parse_source ;

		    break;
	       }
	       if (leave_it) return;
	  }
	  readfds = save_readfds;
	  zero_tv.tv_sec = 0;	/* better safe than sorry */
	  zero_tv.tv_usec = 0;

     }
}

void post_soak_meta_fact(Op_Instance_List soak, Oprs *oprs)
/*
 * post_soak_meta_fact - will post a fact (soak <soak>) in the OPRS oprs. The soak list is
 *                       first transformed as a lisp list, because most access functions
 *                       used in OPs are lisp like function...
 *                       Return void.
 */
{
     TermList tl;

     if (oprs->critical_section) return;

     tl = sl_make_slist();

     sl_add_to_head(tl,
		 build_l_list(make_l_list_from_c_list_type(soak, TT_OP_INSTANCE))); 
     add_fact(make_fact_from_expr(build_expr_pfr_terms(soak_pred, tl)) ,oprs);

/*     oprs->posted_meta_fact = TRUE; We can do this here while we post the empty soak meta_fact */
}

Op_Instance_List reshuffle_randomly_soak_list(Op_Instance_List l)
{
     Op_Instance *opi;
     Op_Instance_List res;
     
     if (sl_slist_length(l) == 1) return l;

     res = sl_make_slist();    
     sl_loop_through_slist(l,  opi, Op_Instance *) 
	  if (RANDOM() & 16) 
	       sl_add_to_head(res, opi);
	  else
	       sl_add_to_tail(res, opi);

     FREE_SLIST(l);

     return res;
}

void reset_oprs_kernel(Oprs *oprs)
{
     Intention_Graph *ig = oprs->intention_graph;
     Intention *in;
     Op_Instance *opi;
     Intention_List tmp_il;

     shift_facts_goals(oprs);
     shift_facts_goals(oprs); /* this will flush all the new facts and goals */

     tmp_il = COPY_SLIST(ig->list_intentions); 

     sl_loop_through_slist(tmp_il, in, Intention *)
	  delete_intention_from_ig(in,ig);

     FREE_SLIST(tmp_il);	/* Free it after. */

     SAFE_SL_LOOP_THROUGH_SLIST(previous_soak, opi, Op_Instance *)
		    free_op_instance(opi);

     SAFE_SL_FLUSH_SLIST(previous_soak);
     return;
}


void set_oprs_run_mode(Oprs_Run_Type mode)
{
     switch(mode) {
     case RUN:
	  oprs_run_mode = RUN;
	  break;
     case STEP_NEXT:
	  oprs_run_mode = STEP_NEXT;
	  break;
     case STEP_HALT:
	  oprs_run_mode = HALT;
	  break;
     case HALT:
	  oprs_run_mode = HALT;
	  break;
     case STEP:
	  oprs_run_mode = STEP;
	  break;
     default:
	  fprintf(stderr, LG_STR("set_oprs_run_mode: unknown run_mode.\n",
				 "set_oprs_run_mode: unknown run_mode.\n"));
     }
#ifdef GRAPHIX
     xset_oprs_run_mode(mode);
#endif
}

void report_run_status(void)
{
     printf(LG_STR("The kernel is in ",
		   "The kernel is in "));
     switch(oprs_run_mode) {
     case RUN:
	  printf(LG_STR("RUN",
			"RUN"));
	  break;
     case STEP_NEXT:
	  printf(LG_STR("STEP_NEXT",
			"STEP_NEXT"));
	  break;
     case STEP_HALT:
     case STEP:
	  printf(LG_STR("STEP",
			"STEP"));
	  break;
     case HALT:
	  printf(LG_STR("HALT",
			"HALT"));
	  break;
     default:
	  fprintf(stderr, LG_STR("report_run_status: unknown run_mode.\n",
				 "report_run_status: unknown run_mode.\n"));
     }

     printf(LG_STR(" mode.\n",
		   " mode.\n"));
}
