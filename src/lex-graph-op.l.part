/*                               -*- Mode: C -*- 
 * lex-graph-op.l -- 
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
 * This piece of lex grammar describe token used in a graph OP.
 */

/* OP file version. */
<OPF_VERSION>1				{return OPF_VERSION1;}
<OPF_VERSION>2				{return OPF_VERSION2;}

<FIELD_TYPE>INVOCATION			{return FT_INVOCATION_TK;}
<FIELD_TYPE>CONTEXT			{return FT_CONTEXT_TK;}
<FIELD_TYPE>CALL			{return FT_CALL_TK;}
<FIELD_TYPE>SETTING			{return FT_SETTING_TK;}
<FIELD_TYPE>PROPERTIES			{return FT_PROPERTIES_TK;}
<FIELD_TYPE>DOCUMENTATION		{return FT_DOCUMENTATION_TK;}
<FIELD_TYPE>EFFECTS			{return FT_EFFECTS_TK;}
<FIELD_TYPE>ACTION			{return FT_ACTION_TK;}

<NODE_TYPE>PROCESS			{return NT_PROCESS_TK;}
<NODE_TYPE>START			{return NT_START_TK;}
<NODE_TYPE>END				{return NT_END_TK;}
<NODE_TYPE>ELSE				{return NT_ELSE_TK;}
<NODE_TYPE>THEN				{return NT_THEN_TK;}
<NODE_TYPE>IF				{return NT_IF_TK;}

<EDGE_TYPE>GOAL			{return ET_GOAL_TK;}
<EDGE_TYPE>ELSE			{return ET_ELSE_TK;}
<EDGE_TYPE>THEN			{return ET_THEN_TK;}
<EDGE_TYPE>IF			{return ET_IF_TK;}

<ACTION_TYPE>\*=\*			{return SPEC_ACT_TK;}
