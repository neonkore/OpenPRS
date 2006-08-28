%{
static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * ope-parser.y -- yacc grammaire
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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/* #define	fileno(p)	((p)->_file) Required by bison.. */
#include "macro.h"

#include "slistPack.h"
#include "slistPack_f.h"

#include "oprs-type.h"
#include "op-structure.h"
#include "relevant-op.h"
#include "parser-funct.h"
#include "op-compiler.h"
#include "ope-graphic.h"
#include "ope-syntax.h"
#include "ope-global.h"
#include "oprs-print.h"
#include "oprs-type_f.h"
#include "oprs-array_f.h"
#include "relevant-op_f.h"
#include "top-structure_f.h"
#include "op-structure_f.h"
#include "oprs-print_f.h"
#include "oprs-pprint_f.h"
#include "oprs-pred-func_f.h"
#include "parser-funct_f.h"

char *node_name;
PString type_string;
/* Node_Type node_type; */
int node_x, node_y, visible, text_dx, text_dy, string_width = 0, fill_lines;
Slist *edge_location;
extern PBoolean check_symbol;
PBoolean out_edge;


%}

%union {
    double    		real;        /* real value */
    int    		integer;    /* integer value */
    long long int	long_long;    /* integer value */
    void    		*pointer;    /* pointer value */
    char    		*string;    /* string buffer */
    int    		cmd;        /* command value */
    PBoolean		bool;
    TermList 		list_term;
    ExprList 		list_expr;
    PropertyList 	prop_list;
    VarList 		var_list;
    Property		*prop;
    Add_Del_List	add_del_list;
    Envar       	*var;
    Term 		*term;
    Pred_Func_Rec	*pred_func_rec;
    Action_Field	*action_field;
    Expression 		*expr;
    Double_Node		*dn;
    Op_Structure 	*op;
    NodeList	 	nl;
    EdgeList 		el;
    Node 		*node;
    Edge 		*edge;
    Edge_Type 		et;
    Slist *		list;
}

%token <real> 		REAL_TK
%token <integer> 	INTEGER_TK 
%token <long_long> 	LONG_LONG_TK
%token <pointer>	POINTER_TK
%token <string> 	QSTRING_TK SYMBOL_TK LOGICAL_VAR_TK PROGRAM_VAR_TK COMMENT_TK 

%token <cmd> 		RESET_DOT_TK
%token <cmd>		ECHO_GEXPR ECHO_GMEXPR ECHO_GTEXPR ECHO_LGTEXPR ECHO_PL ECHO_TERM ECHO_GMEXPR_NIL ECHO_EXPR
%token <cmd> 		COMPILE_OP CONVERT_OP CONVERT_TEXT_OP
%token <cmd> 		COPY_OP APPEND_OP EXIT SPEC_ACT_TK ECHO_ACTION ECHO_BODY ECHO_CTXT
%token <cmd> 		ECHO_STRING ECHO_T_OR_NIL ECHO_EFF ECHO_ID
%token <cmd>            OP_LISP_TK CP_LISP_TK OP_ARRAY_TK CP_ARRAY_TK OPENP_TK CLOSEP_TK
%token <cmd>		OPF_VERSION1 OPF_VERSION2
%token <cmd>		FT_INVOCATION_TK FT_CONTEXT_TK FT_SETTING_TK FT_CALL_TK
%token <cmd>		FT_PROPERTIES_TK FT_DOCUMENTATION_TK FT_EFFECTS_TK FT_ACTION_TK 
%token <cmd>		ET_ELSE_TK ET_THEN_TK ET_IF_TK ET_GOAL_TK
%token <cmd>		NT_PROCESS_TK NT_START_TK NT_END_TK NT_ELSE_TK NT_THEN_TK NT_IF_TK 

%token <cmd>		TFT_INVOCATION_TK TFT_CONTEXT_TK TFT_SETTING_TK TFT_BODY_TK TFT_CALL_TK
%token <cmd>		TFT_PROPERTIES_TK TFT_DOCUMENTATION_TK TFT_EFFECTS_TK TFT_ACTION_TK

%token <cmd>		WHILE_TK DO_TK PAR_TK DEFOP_TK IF_TK ELSE_TK GOTO_TK BREAK_TK ELSEIF_TK LABEL_TK

%type <integer> 	pos_x pos_y pp_width
%type <bool> 		boolean visible pp_fill split join
%type <string> 		op_name node_name edge_name prop_name documentation file_name symbol t_or_nil
%type <expr> 		call invocation setting expr edge_expr
%type <list_expr> 	list_expr list_par_expr context effects list_effects
/* %type <add_del_list> 	*/
%type <dn> 		body inst list_inst while_inst do_inst par_inst top_expr if_inst comment goto_inst
%type <dn> 		label_inst break_inst if_part_inst
%type <list_term> 	term_list
%type <var> 		variable
%type <term> 		term 
%type <expr> 	simple_action
%type <pred_func_rec> 	pred_func
%type <action_field>	action action_expr
%type <prop_list> 	ne_properties ne_properties_list properties properties_list
%type <prop> 		property
%type <var_list> 	var_list
%type <list>		edge_knots list_knots body_list
%%

command:  
	ECHO_GEXPR 				{enable_variable_parsing(); yy_begin_0();} 
		expr 				{s_pretty_parsed = build_string_from_list_lines(pretty_print_expr(pretty_width, $3));}
	| ECHO_STRING  				{yy_begin_0();} 
		 QSTRING_TK 			{s_pretty_parsed = build_string_from_list_lines(pretty_print_backslash_string(pretty_width, $3));}
        | ECHO_STRING 				{NEWSTR("",s_pretty_parsed); yy_begin_0();}
        | ECHO_T_OR_NIL t_or_nil 		{NEWSTR($2, s_pretty_parsed); yy_begin_0();}
        | ECHO_ID SYMBOL_TK	 		{NEWSTR($2, s_pretty_parsed); yy_begin_0();}
        | ECHO_GTEXPR				{enable_variable_parsing(); yy_begin_0();}
		expr 				{s_pretty_parsed = build_string_from_list_lines(pretty_print_expr(pretty_width, $3));
					         parsed_gtexpr = $3;}
        | ECHO_GMEXPR 				{enable_variable_parsing(); yy_begin_0();}
		expr 				{s_pretty_parsed = build_string_from_list_lines(pretty_print_expr(pretty_width, $3));
					        parsed_gmexpr = $3;}
        | ECHO_EXPR 				{NEWSTR("",s_pretty_parsed); yy_begin_0();}
        | ECHO_EXPR 				{enable_variable_parsing(); yy_begin_0();}
		expr 				{s_pretty_parsed = build_string_from_list_lines(pretty_print_expr(pretty_width, $3));
					        parsed_expr = $3;}
        | ECHO_LGTEXPR 				{enable_variable_parsing(); yy_begin_0();}
		list_par_expr 			{s_pretty_parsed = build_string_from_list_lines(pretty_print_list_expr(pretty_width, $3));}
        | ECHO_PL 				{enable_variable_parsing(); yy_begin_0();}
		ne_properties 			{s_pretty_parsed = build_string_from_list_lines(pretty_print_list_property(pretty_width, $3));}
        | ECHO_PL  				{NEWSTR("",s_pretty_parsed); yy_begin_0();}
        | ECHO_TERM 				{enable_variable_parsing(); yy_begin_0();}
		term	 			{s_pretty_parsed = build_string_from_list_lines(pretty_print_term(pretty_width, $3));}
        | ECHO_ACTION 				{enable_variable_parsing(); yy_begin_0();}
		action	 			{s_pretty_parsed = build_string_from_list_lines(pretty_print_action_field(pretty_width, $3));
					         parsed_action = $3;}
        | ECHO_BODY 				{enable_variable_parsing(); yy_begin_COLLECT_COMMENT();}
		body	 			{Body *body;
	                                         yy_begin_0();
						 body = ($3 ? DN_BODY($3) : make_body(NULL));
						 s_pretty_parsed = build_string_from_list_lines(pretty_print_body(pretty_width, body));
					         parsed_body = body;}
	| ECHO_GMEXPR_NIL 			{enable_variable_parsing(); yy_begin_0();}
		expr 				{s_pretty_parsed = build_string_from_list_lines(pretty_print_expr(pretty_width, $3));}
        | ECHO_CTXT   	 			{enable_variable_parsing(); yy_begin_0();}
		list_par_expr	 		{/* sprint_list_gtexpr_par(s_parsed, BUFSIZ, $3); */
		                                 s_pretty_parsed = build_string_from_list_lines(pretty_print_exprlist(pretty_width, $3));}
        | ECHO_EFF   	 			{enable_variable_parsing(); yy_begin_0();}
                effects		 		{if ($3) s_pretty_parsed = build_string_from_list_lines(pretty_print_exprlist(pretty_width, $3));
						else NEWSTR("",s_pretty_parsed); yy_begin_0();}
	| ECHO_GMEXPR_NIL 			{NEWSTR("",s_pretty_parsed); yy_begin_0();}
        | ECHO_CTXT   	 			{NEWSTR("",s_pretty_parsed); yy_begin_0();}


	| CONVERT_OP 				{printf("Convert OP: "); yy_begin_0();}
		file_name file_name		{if (open_file_for_yyparse($3)<0) YYABORT;
						 current_graph_file_name = $3;
						 make_opfile($3,ACS_GRAPH);} 
		op_graph 			{close_file_for_yyparse(); printf("Loaded.\n");
						 if (rename($3, $4) < 0) perror("convert_op_file:rename");
						 else {write_opfile($3,current_opfile); printf("Written.\n");}
						 FREE($3);FREE($4);}
	| CONVERT_TEXT_OP 			{printf("Convert OP to text: "); yy_begin_0();}
		file_name file_name		{if (open_file_for_yyparse($3)<0) YYABORT;
						 current_graph_file_name = $3;
						 make_opfile($3,ACS_GRAPH);} 
		op_graph 			{close_file_for_yyparse(); printf("Loaded.\n");
						 write_opfile_tex($4,current_opfile); printf("Written.\n");
						 FREE($3);FREE($4);}
	| COMPILE_OP 				{printf("Loading OP: "); yy_begin_0();}
		file_name 			{if (open_file_for_yyparse($3)<0) YYABORT;
						 current_graph_file_name = $3;
						 make_opfile($3,ACS_GRAPH);} 
		op_graph 			{close_file_for_yyparse(); printf("Done.\n"); FREE($3);}
	| APPEND_OP 				{if (!be_quiet) printf("Append OP: "); yy_begin_0();}
		file_name 			{if (open_file_for_yyparse($3)<0) YYABORT;
						 current_graph_file_name = current_opfile->name;}
		op_graph 			{close_file_for_yyparse();  FREE($3);
						 if (!be_quiet) printf("Done.\n");}
        | COPY_OP                               {if (!be_quiet) printf("Duplicating OP: "); yy_begin_0();}
                file_name                         {if (open_file_for_yyparse($3)<0) YYABORT;
					          current_graph_file_name = current_opfile->name;} 
                ntop                              {close_file_for_yyparse();  FREE($3);
						 if (!be_quiet) printf("Done.\n");}
	|  error RESET_DOT_TK				{warning("Parsing error, unknown command, reseting the parser");}
	|  EXIT 				{printf("Quitting the OP-Editor.\n"); exit(0);}
	;

t_or_nil: 
	SYMBOL_TK 					{if ($1 != nil_sym && $1 != lisp_t_sym) YYERROR; $$ = $1;}
	;

/* OP section */

op_graph:					{char message[BUFSIZ];
						 yy_begin_OPF_VERSION();
						 sprintf(message,"Loading  OP file: %s. Please Wait.",
							 current_graph_file_name);
						 TimeoutCursors(True, message);}
	new_op_graph 				{yy_begin_0();
						 add_op_file_name(current_graph_file_name, relevant_op);
						 TimeoutCursors(False, NULL);}
	| error RESET_DOT_TK				{warning("Parsing error, expecting a OP graph");}
	;


/* This is the new OP file grammar. Format Version 1 */

new_op_graph: | /* empty  file... */
        OPENP_TK OPF_VERSION1 			{yy_begin_0();}
	nop_list CLOSEP_TK
        |OPENP_TK OPF_VERSION2 			{yy_begin_0();}
	OPENP_TK					{check_symbol = FALSE;}
         symbol_list				{check_symbol = TRUE;}
         CLOSEP_TK nop_list CLOSEP_TK
        | OPENP_TK 					{yy_begin_0();}	
	 top_list CLOSEP_TK
	;

symbol_list: 	
        /* empty */
	| symbol_list SYMBOL_TK			 {if (sl_search_slist(current_opfile->list_sym, $2, equal_string) == NULL)
							sl_add_to_tail(current_opfile->list_sym, $2);}
	;

nop_list: 	
        /* empty */
	| nop_list ntop    			{process_xt_events();}
	;

ntop: nop
      |top
;

nop: 	
	OPENP_TK op_name 				{init_make_op($2,TRUE);} /* Will initialize current_op. */
	OPENP_TK nlist_fields CLOSEP_TK
	OPENP_TK nlist_nodes CLOSEP_TK
	OPENP_TK nlist_edges CLOSEP_TK CLOSEP_TK			 {finish_loading_op(current_op, global_draw_data);
						  sl_add_to_tail(current_opfile->list_op,current_op);
						  if (current_opfile != buffer_opfile)
						       add_op_to_relevant_op_internal(current_op, relevant_op);
						  if (!be_quiet) printf("%s\n",$2);
						  disable_variable_parsing();}
	;

nlist_nodes: 
	/* empty */
	| nlist_nodes nnode  			
	;

nnode: 	
	OPENP_TK {yy_begin_NODE_TYPE();} nnode_typed CLOSEP_TK
	;

nnode_typed: 		
	NT_ELSE_TK {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_ELSE, $4, $5, $6, $7, global_draw_data);}
	| NT_THEN_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_THEN, $4, $5, $6, $7, global_draw_data);}
	| NT_IF_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_IF, $4, $5, $6, $7, global_draw_data);}
	| NT_PROCESS_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_PROCESS, $4, $5, $6, $7, global_draw_data);}
	| NT_END_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_END, $4, $5, $6, $7, global_draw_data);}
	| NT_START_TK {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_START, $4, $5, $6, $7, global_draw_data);}
     	;

join: boolean		{$$ = $1;}
	;

split: boolean		{$$ = $1;}
	;

nlist_fields: 
	/* empty (bien que ca ne soit pas possible...) */
	| nlist_fields nfield
	;

nfield: 	
	OPENP_TK {yy_begin_FIELD_TYPE();} nfield_typed CLOSEP_TK
	;

nfield_typed: 		
	FT_INVOCATION_TK {yy_begin_0();} node_name invocation pos_x pos_y visible pp_width pp_fill
		{build_invocation(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_CALL_TK {yy_begin_0();} node_name call  pos_x pos_y visible pp_width pp_fill
		{build_call(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_CONTEXT_TK {yy_begin_0();} node_name context  pos_x pos_y visible pp_width pp_fill
		{build_context(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_SETTING_TK {yy_begin_0();} node_name setting  pos_x pos_y visible pp_width pp_fill
		{build_setting(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_PROPERTIES_TK {yy_begin_0();} node_name properties  pos_x pos_y visible pp_width pp_fill
		{build_properties(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_DOCUMENTATION_TK {yy_begin_0();} node_name documentation pos_x pos_y visible pp_width pp_fill
		{build_documentation(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_EFFECTS_TK {yy_begin_0();} node_name effects pos_x pos_y visible pp_width pp_fill
		{build_effects(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	| FT_ACTION_TK {yy_begin_0();} node_name action  pos_x pos_y visible pp_width pp_fill
		{build_action(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);}
	;

invocation: expr	{$$ = $1;}
	 ;

call:		{$$ = NULL;}
        | expr	{$$ = $1;}
	 ;

pos_x: INTEGER_TK		{$$ = $1;}
	;

pos_y: INTEGER_TK		{$$ = $1;}
	;

visible: boolean	{$$ = $1;}
	;

pp_width: INTEGER_TK	{$$ = $1;}
	;

pp_fill: boolean	{$$ = $1;}
	;

boolean: INTEGER_TK	{$$ = ($1 ? TRUE : FALSE);}
	;

context: 			{$$ = NULL;}
        | expr			{$$ = sl_make_slist();sl_add_to_tail($$,$1);}
        | list_par_expr		{$$ = $1;}
	;

setting: 				{$$ = NULL;}
	| expr			{$$ = $1;}
	;

properties: 				{$$ = NULL;}
	| OPENP_TK properties_list CLOSEP_TK 	{$$= $2;}
	;

properties_list:			{$$ = sl_make_slist();}
 	| properties_list property	{if ($2) sl_add_to_tail($1,$2); $$=$1;}
	;

ne_properties: OPENP_TK ne_properties_list CLOSEP_TK	{$$=$2;}
	;

ne_properties_list: property			{$$=(PropertyList)sl_make_slist();
						 if ($1) sl_add_to_head($$,$1);  }
	| ne_properties_list property		{if ($2) sl_add_to_tail($1,$2);
						 $$=$1;}
	;

documentation:			{$$ = NULL;}
	| QSTRING_TK		{$$ = $1;}
	;

effects: list_effects	{$$ = $1;}
	;

action:                         {check_symbol = TRUE; yy_begin_ACTION_TYPE();}
            action_expr		{$$ = $2; check_symbol = FALSE;}
	;

action_expr:
	OPENP_TK SPEC_ACT_TK {yy_begin_0();} variable
		simple_action CLOSEP_TK		{$$ = build_action_field($5, TRUE, FALSE, NULL, $4);}
        | OPENP_TK SPEC_ACT_TK {yy_begin_0();} OPENP_TK var_list CLOSEP_TK
		simple_action CLOSEP_TK		{$$ = build_action_field($7, TRUE, TRUE, $5, NULL);}
	| simple_action				{$$ = build_action_field($1, FALSE, FALSE, NULL, NULL);}
;

simple_action:
	OPENP_TK					 {yy_begin_0();} 
	pred_func term_list CLOSEP_TK	 	 {$$=build_simple_action_cached($3,$4);}
;

nlist_edges: 
	/* empty */
	| nlist_edges nedge  			
	;

nedge: 	
	OPENP_TK {yy_begin_EDGE_TYPE();} nedge_typed CLOSEP_TK
	| error RESET_DOT_TK 			{warning("Parsing error, badly formed edge");}
	;


nedge_typed:
	ET_GOAL_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_GOAL, $6, $7, $8, $9, $10, $11, global_draw_data);}
	| ET_IF_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_IF, $6, $7, $8, $9, $10, $11, global_draw_data);}
	| ET_ELSE_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_ELSE, global_draw_data);}
	| ET_THEN_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_THEN, global_draw_data);}
	;

edge_expr: expr
	;

edge_knots:
	OPENP_TK list_knots CLOSEP_TK			{$$ = $2;}
	;

list_knots: /* empty */ 			{$$ = sl_make_slist();}
	| list_knots OPENP_TK INTEGER_TK INTEGER_TK CLOSEP_TK	{sl_add_to_tail($1, (void *)$3); sl_add_to_tail($1,(void *)$4); $$ = $1;}
	;

top_list: 	
	/* empty */
	| top_list top				{process_xt_events();}
	| error RESET_DOT_TK				{warning(LG_STR("Parsing error, expecting a Text OP list",
								"Erreur de parsing, attendait une liste de OP Textuel"));}
	;

top: OPENP_TK DEFOP_TK op_name 			{init_make_top($3,
#ifdef NO_GRAPHIX
							      FALSE
#else
							      TRUE
#endif
							      );
					         current_op->graphic = FALSE;} /* Will initialize current_op. */
	       fields_list
	CLOSEP_TK					{finish_loading_top(current_op,
#ifdef NO_GRAPHIX
							      NULL
#else
							      global_draw_data
#endif
								    );
						 sl_add_to_tail(current_opfile->list_op,current_op);
						 add_op_to_relevant_op_internal(current_op,relevant_op);
						 process_xt_events();
						 if (!be_quiet) printf("%s\n",$3);
						 disable_variable_parsing();}
;

include "yacc-text-op.y"

op_name:
	SYMBOL_TK				{$$=$1;}
	;

edge_name: 
	SYMBOL_TK					{$$ = $1;}
	;

node_name: 
	SYMBOL_TK					{$$ = $1;}
	;

variable:
	LOGICAL_VAR_TK 				{$$=build_var_in_frame($1,LOGICAL_VARIABLE);}
	| PROGRAM_VAR_TK			{$$=build_var_in_frame($1,PROGRAM_VARIABLE);}
	;


symbol:  SYMBOL_TK				{$$=$1;}
	;

include "yacc-exp.y"

%%
