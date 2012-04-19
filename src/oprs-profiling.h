/*                               -*- Mode: C -*- 
 * oprs-profiling.h -- 
 * 
 * $Id$
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


#ifndef INCLUDE_oprs_profiling
#define INCLUDE_oprs_profiling

#ifdef VXWORKS
#include "systime.h"
#elif !defined(WIN95)
#include <sys/time.h>
#endif
/*	For timeval definition. */
#ifdef WIN95
#include <winsock.h>
#endif

#include "oprs-type-pub.h"


extern PDate zero_date;

 /* 
  * do not changes these constants, OPRS objects have been compiled with them 
  *
  */
#define T_DEFAULT 0               /* */
#define T_RESET 1                 /* */
#define T_ALL 2                   /* */

#define T_SEND_MESSAGE 11         /* sending a message */
#define T_CONCLUDE 12             /* conclude a fact ( an expr ) in DB */
#define T_RETRIEVE 13             /* total time : 
				     to consult if a gexpr with a frame is True in the DB
				     and build the bindings.
				     It is more general than retrieve a simple fact.*/
#define T_RELEVANT 14             /* To find all the relevant OP for a new fact or goal */
#define T_APPLIC 15               /* to determine if a relevant op is applicable */
#define T_INTENTION_CREATE 16     /* to create an intention */
#define T_INTENTION_ACTIVE 17     /* total time an intention was active ( executed ) */
#define T_INTENTION_IN_GRAPH 18   /* between the creation and deletion of an intention */
#define T_HANDLE 19               /* If the temporal operator is one of test, wait or achieve:
				       time to check if a goal is already satisfied in DB.
				     It is just one level upper the T_RETRIEVE*/
/* Goals profiling */
#define T_GOAL_CR_REC 21          /* Time between Creation and Reception of a goal */
#define T_GOAL_CR_SOAK 22         /*                           Soak                */
#define T_GOAL_CR_REP 23          /*                           Reponse (success or failed) */
#define T_GOAL_REC_SOAK 24        /*              Reception and Soak                   */
#define T_GOAL_REC_REP 25         /*                            Reponse */
#define T_GOAL_SOAK_REP 26        /*              Soak and Reponse of a goal */
/* Fact profiling */
#define T_FACT_CR_REC 31          /* Time between Creation and Reception of a fact */
#define T_FACT_CR_SOAK 32         /*                           Soak                */
#define T_FACT_CR_REP 33          /*                           Reponse (success or failed) */
#define T_FACT_REC_SOAK 34        /*              Reception and Soak                   */
#define T_FACT_REC_REP 35         /*                            Reponse */
#define T_FACT_SOAK_REP 36        /*              Soak and Reponse of a fact */

#endif /* INCLUDE_oprs_profiling */
