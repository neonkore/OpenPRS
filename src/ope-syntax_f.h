/*                               -*- Mode: C -*-
 * ope-syntax_f.h --
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

void report_syntax_error(PString message);
PBoolean check_name(PString * name);
PBoolean check_ip(PString * ip);
PBoolean check_ctxt(PString * ctxt);
PBoolean check_set(PString * set);
PBoolean check_eff(PString * eff);
PBoolean check_prop(PString * prop);
PBoolean check_ach(PString * prop);
PBoolean check_doc(PString * prop);
PBoolean check_act(PString * act);
PBoolean check_edge(PString * edge);
void report_user_error(PString message);

/*
PBoolean yyparse_string(PString s);
PBoolean yyparse_string_result(PString s, PString *result);
PBoolean yyparse_gexpr(PString *result);
PBoolean yyparse_term(PString *result);
PBoolean yyparse_prop_list(PString *result);
PBoolean yyparse_ach(PString *result);
PBoolean yyparse_doc(PString *result);
PBoolean yyparse_gmexpr_or_nil(PString *result);
PBoolean yyparse_id(PString *result);
PBoolean yyparse_gmexpr(PString *result);
PBoolean yyparse_gtexpr(PString *result);
PBoolean yyparse_lgtexpr_or_nil(PString *result);
*/
