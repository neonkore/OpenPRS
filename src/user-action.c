static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * user-action.c -- 
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
#include "user-action.h"
#include "user-ev-function.h"

#include "oprs-type_f-pub.h"
#include "ev-function_f-pub.h"

#include "action_f-pub.h"
#include "lisp-list_f-pub.h"
#include "oprs_f-pub.h"

#include "oprs-error.h"
#include "oprs-error_f.h"

#include "oprs-rerror.h"
#include "oprs-rerror_f.h"
#include "oprs-rerror_f-pub.h"


Term *action_post_fact(TermList terms)
{
     Term *term, *res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != INTEGER) {
	  fprintf(stderr,"Expecting a INTEGER in action_post_fact.\n");
	  res->u.id = nil_sym;
     } else {
	  char command[BUFSIZ];
	  int j, i = term->u.intval;

	  for (j=1 ; j < i ; j++) {
	       sprintf(command, "add (test1 %d)\n", j);
	       send_command_to_parser(command);
	  }
	  res->u.id = lisp_t_sym;
     }
     return res;
}

Term *action_post_direct_fact(TermList terms)
{
     add_external_fact("foo", 
		       make_external_term_list(
			    2, 
			    TT_FLOAT, 4.0,
			    INTEGER, 5));

     add_external_fact(
	  "boo", 
	  make_external_term_list(
	       6, 
	       TT_FLOAT, 3.1415,
	       INTEGER, 5,
	       U_POINTER, 0x123456,
	       TT_ATOM, "foobar",
	       STRING, "This is a string in the fact",
	       LISP_LIST, make_external_lisp_list(
		    make_external_term_list(
			 2, 
			 TT_ATOM, "first",
			 TT_ATOM, "second")),
	       EXPRESSION, make_external_expr(
		    "bar",
		    make_external_term_list(
			 2,
			 INTEGER, -1,
			 TT_ATOM, "atom"))));

     return build_t();
}

Term *action_bar_foo(TermList terms)
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != TT_ATOM) || (t2->type != EXPRESSION)) fprintf(stderr,"Expecting an ID and a EXPRESSION in action_bar_foo\n");
     send_message_term(t2->u.expr,t1->u.id);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     return res;
}

#define declare_user_action user_action_LTX_declare_user_action

void declare_user_action(void)
{
     fprintf(stderr,"declare_user_action.\n");
     make_and_declare_eval_funct("FOO-BAR",action_bar_foo, 2);
     make_and_declare_action("FOO-BAR",action_bar_foo, 2);
#ifdef IGNORE
     make_and_declare_action("POST-FACT",action_post_fact, 1);
     make_and_declare_action("POST-FACT-DIRECT",action_post_direct_fact, 0);
#endif
     return; 
}
