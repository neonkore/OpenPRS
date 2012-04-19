/*                               -*- Mode: C -*- 
 * oprs-init_f.h -- 
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

Oprs *oprs_init_main(char *name, PBoolean use_x_window, char *mp_hostn, int mp_port, char *server_hostn, int server_port);
void report_profiling(void);
char *oprs_init_arg(int argc,char **argv, char **mp_hostname, int *mp_port, char **server_hostname, int *server_port);
void oprs_init_env(void);
PBoolean parse_one_or_more(void);
void wrap_up(void);
void dump_kernel_from_parser(Oprs *oprs, PString file_name);
void load_kernel_from_parser(Oprs *oprs, PString file_name);
void clear_op_database(Relevant_Op *rk, PBoolean graphic);
void report_meta_option_array(void);
void set_user_trace(User_Trace ut, PBoolean val);
void run_initial_commands(void);
