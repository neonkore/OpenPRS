%{

/*                               -*- Mode: C -*- 
 * oprs-server-parser.l -- Lex parser
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

#include "config.h"

#include <stdlib.h>


#include "macro.h"
#include "constant.h"
#include "opaque.h"
#include "oprs-type.h"
#include "parser-funct.h"

#include "oprs-type_f.h"
/* #include "oprs-array_f.h" */
/* #include "help_f.h" */
/* #include "oprs-client_f.h" */
/* #include "oprs-send-m_f.h" */
/* #include "send-message_f.h" */
#include "parser-funct_f.h"


#include "oprs-server-parser.tab.h"

include "lex-simple-def.l"

include "lex-parser-def.l"

%}

include "lex-macro.l"

id {untypedid}

     /* Pour montrer qu'on demarre dans cet etat, on l'initialise dans oprs-parser.y
	Cet etat est necessaire de toutes facons pour accepter des id avec des pname de commandes...*/
%s COMMAND

%%
{ws}		;
<COMMAND>h	{return HELP;}
<COMMAND>help	{return HELP;}
<COMMAND>\?	{return HELP;}

<COMMAND>show		{return SHOW; }
<COMMAND>version	{return VERSION; }
<COMMAND>copyright	{return COPYRIGHT; }
<COMMAND>add		{return ADD_TK; }
<COMMAND>send	 	{return SEND; }
<COMMAND>broadcast 	{return BROADCAST; }
<COMMAND>make		{return MAKE_OPRS; }
<COMMAND>make_x		{return MAKE_X_OPRS; }
<COMMAND>accept		{return ACCEPT_OPRS; }
<COMMAND>connect	{return CONNECT; }
<COMMAND>kill		{return KILL_OPRS; }
<COMMAND>rehash		{return REHASH_TK; }
<COMMAND>reset		{return RESET_TK; }
<COMMAND>parser		{return PARSER_TK; }
<COMMAND>kernel		{return KERNEL_TK; }
<COMMAND>transmit	{return TRANSMIT; }
<COMMAND>transmit_all	{return TRANSMIT_ALL; }
<COMMAND>quit		{return QUIT; }
<COMMAND>q		{return QUIT; }
<COMMAND>exit		{return QUIT; }
<COMMAND>include	{return INCLUDE; }

include "lex-exp.l"

%%
#include "parser-funct.h"
#include "yy-funct.c"

void yy_begin_0()
{
     BEGIN 0;
}

void yy_begin_COMMAND()
{
     BEGIN COMMAND;
}
