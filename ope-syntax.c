static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*-
 * ope-syntax.c --
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

#include "config.h"

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "macro.h"

#include "ope-graphic.h"
#include "ope-global.h"
#include "oprs-type.h"
#include "parser-funct.h"
#include "ope-syntax_f.h"

extern PBoolean closed_file;   /* Flag used for parsing EOF in oprs */


void set_string_to_parse(char *s)
{
     parse_source = PS_STRING;
     string_line_number = 1;
     oprs_yy_scan_string(s);		/* could be replaced by yy_scan_buffer? */
}

PBoolean yyparse_string_result(PString s, PString *result)
{
     PBoolean save_pp, save_pv, save_fl, res;

     closed_file = FALSE;
     yy_begin_COMMAND();		/* On demarre le lex parser dans cet etat */

     save_pv = print_var_name;
     save_pp = pretty_print;
     save_fl = pretty_fill;

     pretty_print = TRUE;
     print_var_name = TRUE;
     pretty_fill = fill_lines;

     set_string_to_parse(s);
     if (pretty_width <= 0) pretty_width = 60;

     if (oprs_yyparse() == 0) {
	  *result = s_pretty_parsed;
	  res = TRUE;
     } else
	  res = FALSE;

     print_var_name = save_pv;
     pretty_print = save_pp;
     pretty_fill = save_fl;

     return res;
}

PBoolean yyparse_term(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_term %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

Action_Field *parsed_action = NULL;

PBoolean yyparse_action(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_action %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

Body *parsed_body = NULL;

PBoolean yyparse_body(PString * result)
{
     int length = strlen (*result);

     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_body %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_prop_list(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_pl %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_ach(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_t_or_nil %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_doc(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_string %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_gmexpr_or_nil(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_gmexpr_or_nil %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_call(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_expr %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_id(PString * result)
{
     PBoolean res_parsing;
     int length = strlen (*result);
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_id %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}


Expression *parsed_gmexpr = NULL;

PBoolean yyparse_gmexpr(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_gmexpr %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

Expression *parsed_gtexpr = NULL;

PBoolean yyparse_gtexpr(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     parsed_gtexpr = NULL;
     sprintf(ns, "echo_gtexpr %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

Expression *parsed_expr = NULL;

PBoolean yyparse_expr(PString * result)
{
     int length = strlen (*result);
     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_expr %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_effects(PString * result)
{
     int length = strlen (*result);

     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_eff %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean yyparse_ctxt(PString * result)
{
     int length = strlen (*result);

     PBoolean res_parsing;
     char *ns = (char *) MALLOC((length + 50) * sizeof(char));

     sprintf(ns, "echo_ctxt %s", *result);
     res_parsing = yyparse_string_result(ns, result);
     FREE(ns);
     return res_parsing;
}

PBoolean check_name(PString * name)
{
     if (!yyparse_id(name)) {
	  report_syntax_error(LG_STR("Illegal name!\nYou have to specify a symbol.",
				     "Illegal name!\nYou have to specify a symbol."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_node_name(PString * name)
{
     if (!yyparse_id(name)) {
	  report_syntax_error(LG_STR("Illegal node name!\nYou have to specify a symbol.",
				     "Illegal node name!\nYou have to specify a symbol."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_op_name(PString * name)
{
     if (!yyparse_id(name)) {
	  report_syntax_error(LG_STR("Illegal OP name!\nYou have to specify a symbol.",
				     "Illegal OP name!\nYou have to specify a symbol."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_ip(PString * ip)
{
     if (!yyparse_gmexpr(ip)) {
	  report_syntax_error(LG_STR("Invocation Part invalid!\nYou have to specify a Gmexpression.",
				     "Invocation Part invalid!\nYou have to specify a Gmexpression."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_ctxt(PString * ctxt)
{
     if (!yyparse_ctxt(ctxt)) {
	  report_syntax_error(LG_STR("Context Part invalid!\nYou have to specify a Gmexpression or nothing.",
				     "Context Part invalid!\nYou have to specify a Gmexpression or nothing."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_call(PString * ctxt)
{
     if (!yyparse_call(ctxt)) {
	  report_syntax_error(LG_STR("Call Part invalid!\nYou have to specify a Gmexpression or nothing.",
				     "Call Part invalid!\nYou have to specify a Gmexpression or nothing."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_set(PString * set)
{
     if (!yyparse_gmexpr_or_nil(set)) {
	  report_syntax_error(LG_STR("Setting Part invalid!\nYou have to specify a Gmexpression or nothing.",
				     "Setting Part invalid!\nYou have to specify a Gmexpression or nothing."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_eff(PString * eff)
{
     if (!yyparse_effects(eff)) {
	  report_syntax_error(LG_STR("Effect Part invalid!\nYou have to specify a list of Gtexpression or nothing.",
				     "Effect Part invalid!\nYou have to specify a list of Gtexpression or nothing."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_prop(PString * prop)
{
     if (!yyparse_prop_list(prop)) {
	  report_syntax_error(LG_STR("Property Part invalid!\nYou have to specify a list of paire or nothing.",
				     "Property Part invalid!\nYou have to specify a list of paire or nothing."));
	  return FALSE;
     } else
	  return TRUE;
}

/*
PBoolean check_ach(PString * prop)
{
     if (!yyparse_ach(prop)) {
	  report_syntax_error(LG_STR("Achiever invalid!\nYou have to specify T or NIL.",
				     "Achiever invalid!\nYou have to specify T or NIL."));
	  return FALSE;
     } else
	  return TRUE;
}
*/

PBoolean check_doc(PString * prop)
{
     if (!yyparse_doc(prop)) {
	  report_syntax_error(LG_STR("Documentation invalid!\nYou have to specify a quoted string.",
				     "Documentation invalid!\nYou have to specify a quoted string."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_act(PString * act)
{
     if (!yyparse_action(act)) {
	  report_syntax_error(LG_STR("Action Part invalid!\nYou have to specify an action.",
				     "Action Part invalid!\nYou have to specify an action."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_body(PString * body)
{
     if (!yyparse_body(body)) {
	  report_syntax_error(LG_STR("Body Part invalid!\nYou have to specify a body.",
				     "Body Part invalid!\nYou have to specify a body."));
	  return FALSE;
     } else
	  return TRUE;
}

PBoolean check_edge(PString * edge)
{
     if (!yyparse_gtexpr(edge)) {
	  report_syntax_error(LG_STR("Edge invalid!\nYou have to specify a gtexpression.",
				     "Edge invalid!\nYou have to specify a gtexpression."));
	  return FALSE;
     } else
	  return TRUE;
}

