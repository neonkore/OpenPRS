/*
 * 
 * $Id$
 *
 * Copyright (c) 1991-2001 Francois Felix Ingrand.
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

opt: '(' DEFOP_TK op_name 
	       fields_list
	')'
;

fields_list: invocation_field other_fields_list
;

other_fields_list: /* vide */
	   | field other_fields_list
;

field: action_body_field
     | context_field
     | call_field
     | setting_field
     | properties_field
     | documentation_field
     | effects_field
;

invocation_field: FT_INVOCATION_TK invocation
;

context_field: FT_CONTEXT_TK context
;

call_field: FT_CALL_TK call
;

setting_field: FT_SETTING_TK setting
;

properties_field: FT_PROPERTIES_TK properties
;

documentation_field: FT_DOCUMENTATION_TK documentation
;

effects_field: FT_EFFECTS_TK effects
;

action_body_field: FT_ACTION_TK action
		| body
;

body: '(' list_inst ')'
;

list_inst:			/* vide */
	 | list_inst inst
;

inst: goal
    | if_inst
    | while_inst
    | do_inst
    | par_inst
;

if_inst: '(' IF goal then_else_body ')'
;

then_else_body: body
	      | body body
;

while_inst: '(' WHILE goal list_inst ')'
;

do_inst: '(' DO list_inst WHILE goal ')'
;

par_inst: '(' // body_list ')'
;

body_list:			/* vide? */
	 | body_list body
;

