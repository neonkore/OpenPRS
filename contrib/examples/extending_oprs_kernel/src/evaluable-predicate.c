/*                               -*- Mode: C -*- 
 * evaluable-predicate.c -- contains user defined evaluable predicates.
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

#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "user-ev-predicate.h"
#include "ev-predicate_f-pub.h"

#include "pu-parse-tl_f.h"

#include "evaluate-predicate_f.h"

PBoolean is_an_int(TermList tl)
{
     int *intPtr;

     return PUGetOprsParameters(tl, 1,
				INTEGER, intPtr);
}


PBoolean my_connected(TermList tl)
{
     if ( 1 == 1) {		/* Put your fancy condition here. */
	  return TRUE;
     } else {
	  return FALSE;
     }

}

void declare_myprs_eval_pred(void)
{
     make_and_declare_eval_pred("IsAnInt", is_an_int, 1, TRUE);
     make_and_declare_eval_pred("MyConnected", my_connected, 0, TRUE);
     return;
}






