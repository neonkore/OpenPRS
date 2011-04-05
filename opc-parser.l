
%{
/*                               -*- Mode: C -*- 
 * opc-parser.l -- 
 * 
 * $Id$
 *
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
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
#include <stdlib.h>
#include <sys/file.h>


#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "oprs-type.h"

#ifndef NO_GRAPHIX
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif

#include "op-structure.h"
#include "parser-funct.h"

#include "oprs-type_f.h"
#include "parser-funct_f.h"

#include "opc-parser.tab.h"

include "lex-simple-def.l"

include "lex-parser-def.l"

%}

include "lex-macro.l"

id {typedid}

     /* Pour montrer qu'on demarre dans cet etat, on l'initialise dans oprs-parser.y
	Cet etat est necessaire de toutes facons pour accepter des id avec des pname de commandes...*/
%s COMMAND OPF_VERSION NODE_TYPE FIELD_TYPE EDGE_TYPE ACTION_TYPE COLLECT_COMMENT
%e 2000
%o 12000
%a 10000
%p 8000
%k 800
%n 1000

%%
{ws}		;

<COMMAND>declare	{return DECLARE_TK;}
<COMMAND>empty		{return EMPTY_TK;}
<COMMAND>include	{return INCLUDE_TK;}
<COMMAND>require	{return REQUIRE_TK;}
<COMMAND>load		{return LOAD_TK;}
<COMMAND>set		{return SET_TK;}
<COMMAND>dump		{return DUMP_TK;}

<COMMAND>db		{return DB_TK;}
<COMMAND>id		{return ID_TK;}
<COMMAND>op		|
<COMMAND>ka		{return OP_TK;}
<COMMAND>opf		|
<COMMAND>kaf		{return OPF_TK;}

<COMMAND>oprs_data_path	{return OPRS_DATA_PATH_TK;}

<COMMAND>on		{return ON_TK;}
<COMMAND>off		{return OFF_TK;}

<COMMAND>op_predicate	{return OP_PREDICATE_TK;}
<COMMAND>predicate	{return PREDICATE_TK;}
<COMMAND>function	{return FUNCTION_TK;}
<COMMAND>action		{return ACTION_TK;}
<COMMAND>symbol		{return SYM_TK;}
<COMMAND>evaluable	{return EVALUABLE_TK;}
<COMMAND>ff		{return FF_TK;}
<COMMAND>be		{return BE_TK;}
<COMMAND>cwp		{return CWP_TK;}

include "lex-graph-op.l"

include "lex-text-op.l"

include "lex-meta-exp.l"

: 		{return TYPE_SEP_TK;}

include "lex-exp.l"

%%
#include "yy-funct.c"

void yy_begin_0()
{
     BEGIN 0;
}

void yy_begin_COMMAND()
{
     BEGIN COMMAND;
}

void yy_begin_OPF_VERSION(void)
{
     BEGIN OPF_VERSION;
}

void yy_begin_NODE_TYPE(void)
{
     BEGIN NODE_TYPE;
}

void yy_begin_FIELD_TYPE(void)
{
     BEGIN FIELD_TYPE;
}

void yy_begin_ACTION_TYPE(void)
{
     BEGIN ACTION_TYPE;
}

void yy_begin_EDGE_TYPE(void)
{
     BEGIN EDGE_TYPE;
}

void yy_begin_COLLECT_COMMENT(void)
{
     BEGIN COLLECT_COMMENT;
}

