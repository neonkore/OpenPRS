/*                               -*- Mode: C -*- 
 * oprs_f.h -- declaration des fonctions externes de oprs.c
 * 
 * $Id$
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

#ifdef  __cplusplus
extern "C"  {
#endif

void add_fact(Fact *, Oprs *);
void send_command_to_parser(PString s);
void send_message_term(Expression *exp, PString rec);
void multicast_message_term(Expression *exp, unsigned int nb_recs, PString *recs);
void broadcast_message_term(Expression *exp);

void add_external_fact(char *predicat, TermList param_list);

/* All objects are duplicated except TERM_COMP and LISP_LIST which must come
   from make_external_term_comp or make_external_term_list. */
TermList make_external_term_list(int nb_arg, ...);
Expression *make_external_expr(char *function, TermList param_list);

Expression *make_external_expression(char *function, TermList param_list);
L_List make_external_lisp_list(TermList param_list);

char *kernel_name(void);

#ifdef __cplusplus
}
#endif
