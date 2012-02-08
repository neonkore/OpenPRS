/*                               -*- Mode: C -*- 
 * lex-macro.l -- 
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2005 Francois Felix Ingrand.
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
 * Some lex macro used in the rest of the grammars.
 * 
 */

ws		[ \t]+
real 		[-\+]?[0-9]+\.[0-9]*
exp		[eE][-\+]?[0-9]+
integer 	[-\+]?[0-9]+[l]?
long_long 	[-\+]?[0-9]+ll
pointer 	0x[0123456789abcdefABCDEF]+
comment		;.*$
qstring 	\"([^"]|\\["])*\" 
untypedid	([^ \t\n~0123456789"&\(\)\$@;\|\.[\]][^ \t\n"\(\)\]]*)
typedid 	([^ \t\n~0123456789:"&\(\)\$@;\|\.[\]][^ \t\n:"\(\)\]]*)
/* numberid	([^ \t\n"&\(\)\$@;\|\.\]][^ \t\n"\(\)\]]*) */
idbar   	(\|[^|]+\|)
nl		\n
or		V
and		&
op		\(
cp		\)
opar		\[
cpar		\]
oplisp		\(\.
cplisp		\.\)
lvar		\${id}
pvar		@{id}
gpvar		@@{id}
