static char rcsid [] = "$Id$";

/*                               -*- Mode: C -*- 
 * user-action.c -- contains user defined evaluable actions.
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

#include <stdio.h>

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"

#include "user-action.h"

int get_local_time();
Term *action_send_message(TermList terms);

Term *action_init_oprs_time_td(TermList terms)
{
     Term *t1,*res;

     res = MAKE_OBJECT(Term);
     t1 = (Term *)get_list_pos(terms, 1);
     if (t1->type != INTEGER)
	   fprintf(stderr,"Expecting an INTEGER in init_oprs_time_td\n");

     init_time(t1->u.intval);

     res->type = ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     return res;
}

Term *action_halt_oprs_time_td(TermList terms)
{
     Term *res;

     halt_time();

     res = MAKE_OBJECT(Term);
     res->type = ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     return res;
}

Term *action_run_oprs_time_td(TermList terms)
{
     Term *res;

     wake_up_time();

     res = MAKE_OBJECT(Term);
     res->type = ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     return res;
}

void declare_user_action(void)
{
     make_and_declare_action("INIT-OPRS-TIME",action_init_oprs_time_td, 1);
     make_and_declare_action("HALT-OPRS-TIME",action_halt_oprs_time_td, 0);
     make_and_declare_action("RUN-OPRS-TIME",action_run_oprs_time_td, 0);
     /* French version... */
     make_and_declare_action("INITIALISER-TEMPS",action_init_oprs_time_td, 1);
     make_and_declare_action("INTERROMPRE-TEMPS",action_halt_oprs_time_td, 0);
     make_and_declare_action("DEMARRER-TEMPS",action_run_oprs_time_td, 0);
     make_and_declare_action("ENVOYER-MESSAGE",action_send_message, 2);
     return;
}
