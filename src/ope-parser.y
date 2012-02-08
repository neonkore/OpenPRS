%{
static const char* const rcsid = "$Id$";


#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


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

int node_x, node_y, visible, text_dx, text_dy, string_width = 0, fill_lines;
Slist *edge_location;
extern PBoolean check_symbol;
PBoolean out_edge;


%}

%union {
    double    		real;        
    int    		integer;    
    long long int	long_long;    
    void    		*pointer;    
    char    		*string;    
    int    		cmd;        
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
		list_par_expr	 		{
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



op_graph:					{char message[BUFSIZ];
						 yy_begin_OPF_VERSION();
						 sprintf(message,"Loading  OP file: %s. Please Wait.",
							 current_graph_file_name);
						 if (! no_window) TimeoutCursors(True, message);}
	new_op_graph 				{yy_begin_0();
						 add_op_file_name(current_graph_file_name, relevant_op);
						 if (! no_window) TimeoutCursors(False, NULL);}
	| error RESET_DOT_TK				{warning("Parsing error, expecting a OP graph");}
	;




new_op_graph: | 
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
        
	| symbol_list SYMBOL_TK			 {if (sl_search_slist(current_opfile->list_sym, $2, equal_string) == NULL)
							sl_add_to_tail(current_opfile->list_sym, $2);}
	;

nop_list: 	
        
| nop_list ntop    			{if (!no_window) process_xt_events();}
	;

ntop: nop
      |top
;

nop: 	
	OPENP_TK op_name 				{init_make_op($2, ! no_window);} 
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

list_knots:  			{$$ = sl_make_slist();}
	| list_knots OPENP_TK INTEGER_TK INTEGER_TK CLOSEP_TK	{sl_add_to_tail($1, (void *)$3); sl_add_to_tail($1,(void *)$4); $$ = $1;}
	;

top_list: 	
	
| top_list top				{if (!no_window) process_xt_events();}
	| error RESET_DOT_TK				{warning(LG_STR("Parsing error, expecting a Text OP list",
								"Erreur de parsing, attendait une liste de OP Textuel"));}
	;

top: OPENP_TK DEFOP_TK op_name 			{init_make_top($3,
#ifdef NO_GRAPHIX
							      FALSE
#else
							      ! no_window
#endif
							      );
					         current_op->graphic = FALSE;} 
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
						 if (!no_window) process_xt_events();
						 if (!be_quiet) printf("%s\n",$3);
						 disable_variable_parsing();}
;



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
#ifdef NO_GRAPHIX
		build_invocation(current_op, "INVOCATION", $2, 0, 0, TRUE, 0, TRUE, NULL);
#else
		build_invocation(current_op, "INVOCATION", $2, ip_x, ip_y, TRUE, ip_width, TRUE, global_draw_data);
#endif
				  }
;

call_field: TFT_CALL_TK call
		{
#ifdef NO_GRAPHIX
		build_call(current_op, "CALL", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_call(current_op, "CALL", $2, call_x, call_y, TRUE, call_width, TRUE,global_draw_data);
#endif
			       }
;

context_field: TFT_CONTEXT_TK context
		{
#ifdef NO_GRAPHIX
		build_context(current_op, "CONTEXT", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_context(current_op, "CONTEXT", $2, ctxt_x, ctxt_y, TRUE, ctxt_width, TRUE,global_draw_data);
#endif
			       }
;

setting_field: TFT_SETTING_TK setting
		{
#ifdef NO_GRAPHIX
		build_setting(current_op, "SETTING", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_setting(current_op, "SETTING", $2, set_x, set_y, TRUE, set_width, TRUE,global_draw_data);
#endif
			       }
;

properties_field: TFT_PROPERTIES_TK properties
		{
#ifdef NO_GRAPHIX
		build_properties(current_op, "PROPERTIES", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_properties(current_op, "PROPERTIES", $2, prop_x, prop_y, TRUE, prop_width, TRUE,global_draw_data);
#endif
			       }
;

documentation_field: TFT_DOCUMENTATION_TK documentation
		{
#ifdef NO_GRAPHIX
                FREE($2);
#else
		build_documentation(current_op, "DOCUMENTATION", $2, doc_x, doc_y, TRUE, doc_width, TRUE,global_draw_data);
#endif
			       }
;

effects_field: TFT_EFFECTS_TK effects
		{
#ifdef NO_GRAPHIX
		build_effects(current_op, "EFFECTS", $2, 0, 0, TRUE, 0, TRUE,NULL);
#else
		build_effects(current_op, "EFFECTS", $2, eff_x, eff_y, TRUE, eff_width, TRUE,global_draw_data);
#endif
			       }
;

action_field: TFT_ACTION_TK action
{
#ifdef NO_GRAPHIX
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
		 
#ifdef NO_GRAPHIX
		build_body(current_op, "BODY", to_free = ($3?DN_BODY($3):make_body(NULL)), 0, 0, TRUE, 0, TRUE, NULL);
#else
		build_body(current_op, "BODY", to_free = ($3?DN_BODY($3):make_body(NULL)), bd_x, bd_y, TRUE, bd_width, TRUE, global_draw_data);
#endif
		free_double_node($3);
#ifdef OPRS_KERNEL 
#ifndef NO_GRAPHIX
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

list_inst:			{$$ = NULL;}
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

body_list:			 	{$$=sl_make_slist();}
	 | body_list body			{sl_add_to_tail($1,$2); $$ = $1;}
;


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



list_effects:	 					{$$=NULL;}
	| list_par_expr 		{$$=$1;}
	;

list_par_expr:
	OPENP_TK list_expr CLOSEP_TK 		{$$=$2;}
	| OPENP_TK CLOSEP_TK 			{$$=sl_make_slist();}
	;

list_expr:
	expr 					{$$=sl_make_slist(); sl_add_to_tail($$,$1);}
	| list_expr expr 			{sl_add_to_tail($1,$2);$$=$1;}
	;

expr:
        OPENP_TK pred_func term_list CLOSEP_TK 	{$$=build_expr_pfr_terms($2,$3); 
	                                         if (! $$) {warning(LG_STR("badly formed expression",
									   "expression mal formée")); YYABORT;}}
	|  OPENP_TK pred_func error CLOSEP_TK		{warning(LG_STR("badly formed expression",
								"expression mal formée")); YYABORT;}
	;

pred_func: variable				{$$ = create_var_pred_func($1);}
	| SYMBOL_TK				{$$ = find_or_create_pred_func($1);}
	;
        
term_list:
	 				{$$=(TermList)sl_make_slist();}
	| term_list term 			{$$=build_term_list($1,$2);}
	;

file_name: QSTRING_TK				{$$=canonicalize_file_name($1);}
	;

term:
	INTEGER_TK 				{$$=build_integer($1);}
	| LONG_LONG_TK  			{$$=build_long_long($1);}
	| POINTER_TK  				{$$=build_pointer($1);}
	| REAL_TK  				{$$=build_float($1);}
	| QSTRING_TK				{$$=build_qstring($1);}
	| symbol 				{$$=build_id($1);}
	| variable  				{$$=build_term_from_var($1);}
	| expr	 				{$$=build_term_expr($1);}
	| OP_LISP_TK term_list CP_LISP_TK 	{$$=build_term_l_list_from_c_list($2);}
	| OP_ARRAY_TK term_list CP_ARRAY_TK	{$$=build_term_array_from_c_list($2);}
	| OP_ARRAY_TK error CP_ARRAY_TK		{warning(LG_STR("badly formed ARRAY term",
								"ARRAY term mal formée")); YYABORT;}
	| OP_ARRAY_TK error CLOSEP_TK		{warning(LG_STR("badly formed composed term, bad matching [ )",
								"composed term mal formée, mauvaise correspondance [ )")); YYABORT;}
	| OP_ARRAY_TK error CP_LISP_TK		{warning(LG_STR("badly formed composed term, bad matching [ .)",
								"composed term mal formée, mauvaise correspondance [ .)")); YYABORT;}
	| OPENP_TK error CLOSEP_TK			{warning(LG_STR("badly formed composed term",
								"composed term mal formée")); YYABORT;}
	| OPENP_TK error CP_LISP_TK		{warning(LG_STR("badly formed composed term, bad matching ( .)",
								"composed term mal formée, mauvaise correspondance ( .)")); YYABORT;}
	| OPENP_TK error CP_ARRAY_TK		{warning(LG_STR("badly formed composed term, bad matching ( ]",
								"composed term mal formée, mauvaise correspondance ( ]")); YYABORT;}
	| OP_LISP_TK error CP_LISP_TK	 	{warning(LG_STR("badly formed LISP_LIST term",
								"LISP_LIST term mal formée")); YYABORT;}
	| OP_LISP_TK error CLOSEP_TK	 	{warning(LG_STR("badly formed composed term, bad matching (. )",
								"composed term mal formée, mauvaise correspondance (. )")); YYABORT;}
	| OP_LISP_TK error CP_ARRAY_TK	 	{warning(LG_STR("badly formed composed term, bad matching (. ]",
								"composed term mal formée, mauvaise correspondance (. ]")); YYABORT;}
	;

var_list:
	variable 				{$$=(VarList)sl_make_slist(); sl_add_to_head($$,$1);}
	| var_list variable			{sl_add_to_tail($1,$2); $$=$1;}
	;




property:
	OPENP_TK prop_name term CLOSEP_TK 		{$$=build_property($2,$3);}

	| OPENP_TK prop_name error CLOSEP_TK		{warning(LG_STR("expecting a term after a property name",
								"attendait un term aprés un nom de propriété")); YYABORT;}
	| OPENP_TK error CLOSEP_TK	 		{warning(LG_STR("expecting a property name",
								"attendait un mnom de propriété")); YYABORT;}
	;

prop_name:
	SYMBOL_TK				{$$=$1;}
	;


%%
