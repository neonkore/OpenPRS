/*                               -*- Mode: C -*- 
 * yacc-text-op.y -- 
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

fields_list: invocation_field other_fields_list
;

other_fields_list: field 
	   | other_fields_list field
;

field: body_field
     | action_field
     | call_field
     | context_field
     | setting_field
     | properties_field
     | documentation_field
     | effects_field
;

invocation_field: TFT_INVOCATION_TK invocation
		{
#ifndef GRAPHIX
		build_invocation(current_op, "INVOCATION", $2, 0, 0, TRUE, 0, TRUE, NULL);
#else
		build_invocation(current_op, "INVOCATION", $2, ip_x, ip_y, TRUE, ip_width, TRUE, global_draw_data);
#endif
				  }
;

call_field: TFT_CALL_TK call
		{
#ifndef GRAPHIX
		build_call(current_op, "CALL", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_call(current_op, "CALL", $2, call_x, call_y, TRUE, call_width, TRUE,global_draw_data);
#endif
			       }
;

context_field: TFT_CONTEXT_TK context
		{
#ifndef GRAPHIX
		build_context(current_op, "CONTEXT", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_context(current_op, "CONTEXT", $2, ctxt_x, ctxt_y, TRUE, ctxt_width, TRUE,global_draw_data);
#endif
			       }
;

setting_field: TFT_SETTING_TK setting
		{
#ifndef GRAPHIX
		build_setting(current_op, "SETTING", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_setting(current_op, "SETTING", $2, set_x, set_y, TRUE, set_width, TRUE,global_draw_data);
#endif
			       }
;

properties_field: TFT_PROPERTIES_TK properties
		{
#ifndef GRAPHIX
		build_properties(current_op, "PROPERTIES", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_properties(current_op, "PROPERTIES", $2, prop_x, prop_y, TRUE, prop_width, TRUE,global_draw_data);
#endif
			       }
;

documentation_field: TFT_DOCUMENTATION_TK documentation
		{
#ifndef GRAPHIX
                FREE($2);
#else
		build_documentation(current_op, "DOCUMENTATION", $2, doc_x, doc_y, TRUE, doc_width, TRUE,global_draw_data);
#endif
			       }
;

effects_field: TFT_EFFECTS_TK effects
		{
#ifndef GRAPHIX
		build_effects(current_op, "EFFECTS", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_effects(current_op, "EFFECTS", $2, eff_x, eff_y, TRUE, eff_width, TRUE,global_draw_data);
#endif
			       }
;

action_field: TFT_ACTION_TK action
{
#ifndef GRAPHIX
		build_action(current_op, "ACTION", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_action(current_op, "ACTION", $2, act_x, act_y, TRUE, act_width, TRUE,global_draw_data);
#endif
			       }
;

body_field: TFT_BODY_TK {yy_begin_COLLECT_COMMENT(); check_symbol = TRUE;} body
		{Body *to_free;
		 yy_begin_0(); check_symbol = FALSE;
		 if ($3) {
		      current_op->start_point = DN_HEAD($3);
		} else {
		     current_op->start_point = make_simple_node();
		}
		 
#ifndef GRAPHIX
		build_body(current_op, "BODY", to_free = ($3?DN_BODY($3):make_body(NULL)), 0, 0, TRUE, 0, TRUE, NULL);
#else
		build_body(current_op, "BODY", to_free = ($3?DN_BODY($3):make_body(NULL)), bd_x, bd_y, TRUE, bd_width, TRUE, global_draw_data);
#endif
		free_double_node($3);
#ifdef OPRS_KERNEL 
#ifdef GRAPHIX
		clean_inst_sl_in_slist_og_inst(current_op->list_og_inst);
		DUP_SLIST(current_op->list_og_inst);
#endif
		free_body(to_free);
		current_op->body = NULL;		
#endif
		 if (really_build_node) current_op->start_point->type = NT_START;}
;

body: OPENP_TK list_inst CLOSEP_TK			{if ($2) {
     						DN_BODY($2)=make_body(DN_LIST_INST($2));
					   		}
						 $$ = $2;}
;

list_inst:	/* vide */		{$$ = NULL;}
	 | list_inst inst		{$$ = add_inst_to_list_inst($1,$2);}
;

inst: top_expr				{$$=$1;}
    | if_inst					{$$=$1;}
    | while_inst				{$$=$1;}
    | do_inst					{$$=$1;}
    | par_inst					{$$=$1;}
    | comment					{$$=$1;}
    | goto_inst					{$$=$1;}
    | label_inst				{$$=$1;}
    | break_inst				{$$=$1;}
;

top_expr: expr				{$$=build_inst($1);}
;

comment: COMMENT_TK				{$$=build_comment($1);}
;

goto_inst: GOTO_TK SYMBOL_TK 			{$$=build_goto_inst($2);}
;

break_inst: BREAK_TK	 			{$$=build_break_inst();}
;

label_inst: LABEL_TK SYMBOL_TK			{$$=build_label_inst($2);}
;

if_part_inst: expr list_inst 			{$$ = build_if($1,$2,NULL,FALSE);}
	| expr list_inst ELSE_TK list_inst  	{$$ = build_if($1,$2,$4,FALSE);}
	| expr list_inst ELSEIF_TK if_part_inst  	{$$ = build_if($1,$2,$4,TRUE);}
;

if_inst: OPENP_TK IF_TK if_part_inst CLOSEP_TK 			{$$ = $3;}
;

while_inst: OPENP_TK WHILE_TK expr list_inst CLOSEP_TK 	{$$ = build_while($3,$4); parse_break_list($$);}
;

do_inst: OPENP_TK DO_TK list_inst WHILE_TK expr CLOSEP_TK	{$$ = build_do_until($3,$5); parse_break_list($$);}
;

par_inst: OPENP_TK PAR_TK body_list CLOSEP_TK		{$$ = build_par_branch($3);}
;

body_list:			/* vide? */ 	{$$=sl_make_slist();}
	 | body_list body			{sl_add_to_tail($1,$2); $$ = $1;}
;
