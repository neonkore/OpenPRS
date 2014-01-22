/*                               -*- Mode: C -*- 
 * action.c -- contains user defined actions.
 * 
 * Copyright (c) 2013 LAAS/CNRS
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

#include "oprs-type_f-pub.h"
#include "fact-goal_f-pub.h"
#include "oprs_f-pub.h"

#include "pu-parse-tl_f.h"
#include "pu-mk-term_f.h"
#include "pu-genom_f.h"

#include "action_f-pub.h"

#include "action_f.h"


Term *init_action(TermList terms)
{
     int init;
     if (!PUGetOprsParameters(terms, 1,
			      INTEGER, &init
	      )) {
	  fprintf(stderr,"This was printed from the action init_action which did not get an int. Failure.\n");	
	  return build_nil();
     }
     fprintf(stderr,"This was printed from the action init_action, passing %d. Success.\n", init);	
     return(build_t());
}

Term *end_action(TermList terms)
{
     /* do something. */
     fprintf(stderr,"This was printed from the action end_action.\n");	
     return(build_t());
}

Term *my_action(TermList terms)
{
     double v,w;

     if (PUGetOprsParameters(terms, 2,
			     FLOAT, &v,
			     FLOAT, &w)) {
	  fprintf(stderr,"This was printed from the action my_action, passing %f and %f. Success.\n", v, w);	
	  return(build_t());
     } else
	  fprintf(stderr,"This was printed from the action my_action, which did not get the 2 FLOAT. Failure.\n");	
	  return(build_nil());
}

void declare_myoprs_action(void)
{
  make_and_declare_action("MyInit", init_action, 1);
  make_and_declare_action("MyEnd", end_action, 0);
  make_and_declare_action("MyAction", my_action, 2);
  return;
}
