/*                               -*- Mode: C -*- 
 * user-ev-predicate.c -- contains user defined evaluable predicate.
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

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "user-ev-predicate.h"

#include "ev-predicate_f-pub.h"

#ifdef TRUCK_DEMO
int get_local_time();

PBoolean elapsed_time_ep(TermList tl)
{
     Term *t1, *t2;
     int time;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     t2 = (Term *)sl_get_slist_pos(tl, 2);
     if ((t1->type != INTEGER) || (t2->type != INTEGER))
	  fprintf(stderr,LG_STR("Expecting two integer in elapsed_time_ep\n",
				"Expecting two integer in elapsed_time_ep\n"));

     time = get_local_time();

     printf ("elapsed time : %d %d %d  \n",t1->u.intval,t2->u.intval, time);

     if ((t1->u.intval + t2->u.intval) <= time ) return TRUE;
     else return FALSE;
}
#endif

PBoolean foo_bar_ep(TermList tl)
{
     Term *t;

     t = (Term *)sl_get_slist_head(tl);

     if ((t->type == LISP_LIST)  && (t->u.l_list == l_nil)) return TRUE;
     else return FALSE;
}

void declare_user_eval_pred(void)
{
     make_and_declare_eval_pred("FOO_BAR",(PFB)foo_bar_ep, 1, FALSE);
#ifdef TRUCK_DEMO
     make_and_declare_eval_pred("ELAPSED-TIME",elapsed_time_ep, 2, FALSE);
#endif
     return;
}


