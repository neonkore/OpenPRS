/*                               -*- Mode: C -*- 
 * parser-funct_f.h -- 
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

int oprs_yyparse(void);
void mstats(FILE *fp, char *s);

void print_oprs_prompt(void);
int oprs_yyparse(void);
void warning(char *s);
PBoolean parse_one_or_more(void);
PString canonicalize_file_name(PString file_name);
PString canonicalize_read_file_name(PString file_name);
void report_syntax_error(PString message);
void report_parser_error(PString message);
void set_string_to_parse(char *s);
PBoolean loaded_include_file(char *filename);
void provide_include_file(char *filename);
void pmstats(void);
char *read_file_name_newer_dopf_p(char *filename);
char *compilable_opf(char *filename);

PBoolean check_exist_access(char *selected_file);
PBoolean check_read_access(char *selected_file);
PBoolean check_write_access(char *selected_file);

void oprs_yyerror(char *s);
void yy_begin_0(void);
void yy_begin_COMMAND(void);
void yy_begin_OPF_VERSION(void);
void yy_begin_NODE_TYPE(void);
void yy_begin_FIELD_TYPE(void);
void yy_begin_ACTION_TYPE(void);
void yy_begin_EDGE_TYPE(void);
void yy_begin_COLLECT_COMMENT(void);
