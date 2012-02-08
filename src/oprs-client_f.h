/*                               -*- Mode: C -*- 
 * oprs-client_f.h -- 
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

Oprs_Client *accept_oprs_client();
Oprs_Client *make_oprs_client(PString name);
Oprs_Client *make_x_oprs_client(PString name);
void *init_server_socket_and_accept(void *);
void add_expr_to_client(Expression *f, PString oprs_name);
void broadcast_message_term(Expression *exp);
void get_message(Fact *f, Oprs *p, PString sender);
void info_oprs_client(Oprs_Client *oprs_cl);
void init_client_socket(char *name, PBoolean use_x);
void init_server_socket(void);
void kill_named_oprs_client(PString oprs_name);
void kill_oprs_client(Oprs_Client *oprs_cl);
void leave_the_stdin(void);
void pass_the_stdin(PString oprs_name);
void reset_kernel_named_oprs_client(PString oprs_name);
void reset_parser_named_oprs_client(PString oprs_name);
void send_message(Fact *f, PString rec);
void take_the_stdin(void);
void transmit_to_all_client(PString command);
void transmit_to_client(PString oprs_name, PString command);
PBoolean equal_oprs_name(PString name, Oprs_Client *pc);
