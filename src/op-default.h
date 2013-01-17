/*                               -*- Mode: C -*- 
 * op-default.h -- 
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

#ifndef INCLUDE_op_default
#define INCLUDE_op_default

/* Default Start node position */
#define START_X 310
#define START_Y 10

/* Default Invocation part position. */
#define IP_X 10
#define IP_Y 50

/* Default Call part position. */
#define CALL_X 10
#define CALL_Y 150

/* Default Context part position. */
#define CTXT_X 10
#define CTXT_Y 250

/* Default Setting part position. */
#define SET_X 10
#define SET_Y 350

/* Default Effects part position. */
#define EFF_X 10
#define EFF_Y 450

/* Default Properties position. */
#define PROP_X 10
#define PROP_Y 550

/* Default Documentation position. */
#define DOC_X 10
#define DOC_Y 650

/* Default Action part position. */
#define ACT_X 310
#define ACT_Y 50

/* Default Body part position. */
#define BD_X 310
#define BD_Y 50

/* Default Width for pretty printing */
#define IP_WIDTH 80
#define CALL_WIDTH 60
#define CTXT_WIDTH 60
#define SET_WIDTH 60
#define ACH_WIDTH 60
#define EFF_WIDTH 60
#define PROP_WIDTH 60
#define DOC_WIDTH 60
#define ACT_WIDTH 60
#define BD_WIDTH 120
#define EDGE_WIDTH 40

extern int  ip_x;
extern int  ip_y;
extern int  ip_width;
extern int  ctxt_x;
extern int  ctxt_y;
extern int  ctxt_width;
extern int  call_x;
extern int  call_y;
extern int  call_width;
extern int  set_x;
extern int  set_y;
extern int  set_width;
extern int  eff_x;
extern int  eff_y;
extern int  eff_width;
extern int  prop_x;
extern int  prop_y;
extern int  prop_width;
extern int  doc_x;
extern int  doc_y;
extern int  doc_width;
extern int  act_x;
extern int  act_y;
extern int  act_width;
extern int  bd_x;
extern int  bd_y;
extern int  bd_width;
extern int  edge_width;

#endif
