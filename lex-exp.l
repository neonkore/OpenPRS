/*                               -*- Mode: C -*- 
 * lex-exp.l -- 
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

/* 
 * This piece of lex grammar describe the token used for all the
 * epxressions in the various yacc grammars.
 */

^\.\n		{ parser_line_number[parser_index]++;
                  return RESET_DOT_TK;} /* This is the reset token...  when the parser is lost. */

/* Basic terms. */
{real}		|
{real}{exp}    	{ sscanf(oprs_yytext,"%lf",&oprs_yylval.real);
		  return REAL_TK;}
{pointer}       { sscanf(oprs_yytext+2,"%x", (unsigned int *)&oprs_yylval.pointer); /* the +2 is to get rid of the 0x... */
		  return POINTER_TK;}
{integer}       { sscanf(oprs_yytext,"%d", &oprs_yylval.integer);
		  return INTEGER_TK;}
{comment}	;
{qstring}	{ NEWQSTR(oprs_yytext,oprs_yylval.string);
		  return QSTRING_TK;}

\|\|		{ oprs_yylval.string = or_sym;
		  return SYMBOL_TK;}
&		{ oprs_yylval.string = and_sym;
		  return SYMBOL_TK;}
~		{ oprs_yylval.string = not_sym;
		  return SYMBOL_TK;}

/* Variables. */
{lvar}		{ DECLARE_VAR(oprs_yytext,oprs_yylval.string);
		  return LOGICAL_VAR_TK;}
{pvar} |
{gpvar}		{ DECLARE_VAR(oprs_yytext,oprs_yylval.string);
		  return PROGRAM_VAR_TK;}

/* Temporal operators. */
\+>		{  oprs_yylval.string = cond_add_sym;
		  return SYMBOL_TK;}
->		{  oprs_yylval.string = cond_del_sym;
		  return SYMBOL_TK;}
=>	|
/* ASSERT and CONCLUDE are both allowed */
[Aa][Ss][Ss][Ee][Rr][Tt]	|
[Cc][Oo][Nn][Cc][Ll][Uu][Dd][Ee]	{  oprs_yylval.string = temp_conclude_sym;
		  return SYMBOL_TK;}
~>	|
[Rr][Ee][Tt][Rr][Aa][Cc][Tt] 		{  oprs_yylval.string = retract_sym;
		  return SYMBOL_TK;}
!	|
[Aa][Cc][Hh][Ii][Ee][Vv][Ee] 		{  oprs_yylval.string = achieve_sym;
		  return SYMBOL_TK;}
\?	|
[Tt][Ee][Ss][Tt]	 		{  oprs_yylval.string = test_sym;
		  return SYMBOL_TK;}
#	|
[Pp][Rr][Ee][Ss][Ee][Rr][Vv][Ee]	{  oprs_yylval.string = preserve_sym;
		  return SYMBOL_TK;}
\^	|
[Ww][Aa][Ii][Tt]	 		{  oprs_yylval.string = twait_sym;
		  return SYMBOL_TK;}
\%	|
[Mm][Aa][Ii][Nn][Tt][Aa][Ii][Nn]	{  oprs_yylval.string = maintain_sym;
		  return SYMBOL_TK;}
\<>	|
[Cc][Aa][Ll][Ll]			{  oprs_yylval.string = call_sym;
		  return SYMBOL_TK;}

/* Paranthesis and brackets. */
{oplisp}        { return OP_LISP_TK;}
{cplisp}        { return CP_LISP_TK;}
{opar}          { return OP_ARRAY_TK;}
{cpar}          { return CP_ARRAY_TK;}

/* Particular symbol and atom. */
t	|
T		{ oprs_yylval.string = lisp_t_sym;
		  return SYMBOL_TK;}
[nN][iI][lL]	{ oprs_yylval.string = nil_sym;
		  return SYMBOL_TK;}

{id} |
{idbar}		{ DECLARE_ID(oprs_yytext,oprs_yylval.string);
		  return SYMBOL_TK;}

{op}        	{ return OPENP_TK;}
{cp}        	{ return CLOSEP_TK;}

{nl}	    	{parser_line_number[parser_index]++;}

.		{warning("unexpected character.");}
