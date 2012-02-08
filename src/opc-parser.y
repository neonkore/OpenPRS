%{
static const char* const rcsid = "$Id$";



#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#ifndef WIN95
#include <sys/file.h>
#endif
#endif



#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "shashPack.h"
#include "shashPack_f.h"

#include "oprs-type.h"
#include "op-structure.h"
#include "opc.h"
#include "op-compiler.h"
#include "help_f.h"
#include "oprs-print.h"
#include "oprs-sprint.h"
#include "oprs_f.h"
#include "parser-funct.h"
#include "op-default.h"
#include "database.h"
#include "oprs-pred-func.h"
#include "relevant-op.h"
#include "parser-funct_f.h"
#include "oprs-main.h"
#include "top-lev.h"
#include "oprs-profiling.h"

#include "xoprs-main.h"

#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "op-structure_f.h"
#include "top-structure_f.h"
#include "database_f.h"
#include "op-compiler_f.h"
#include "fact-goal_f.h"
#include "unification_f.h"
#include "relevant-op_f.h"
#include "soak_f.h"
#include "other_proto.h"
#include "oprs-send-m_f.h"
#include "oprs-init_f.h"
#include "int-graph_f.h"
#include "conditions_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-array_f.h"
#include "top-lev_f.h"
#include "oprs-profiling_f.h"

char *node_name;
PString type_string;
Node_Type node_type;
int node_x, node_y, visible, text_dx, text_dy, string_width, fill_lines;
Slist *text, *graphic, *step_list;
Slist *edge_location;
long int read_check_sum;


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
    Add_Del_List	add_del_list;
    VarList 		var_list;
    Property		*prop;
    Envar       	*var;
    Term 		*term;
    Expression 		*expr;
    Pred_Func_Rec	*pred_func_rec;
    Action_Field	*action_field;
    Double_Node		*dn;
    NodeList 		nl;
    EdgeList 		el;
    Node_Type 		nt;
    Slist *		list;
}

%token <real> 		REAL_TK
%token <pointer>	POINTER_TK
%token <integer> 	INTEGER_TK
%token <long_long> 	LONG_LONG_TK
%token <string> 	QSTRING_TK SYMBOL_TK COMMENT_TK LOGICAL_VAR_TK PROGRAM_VAR_TK

%token <cmd>		DECLARE_TK ID_TK SET_TK ACTION_TK
%token <cmd>		EMPTY_TK INCLUDE_TK LOAD_TK
%token <cmd>		DB_TK OP_TK OPF_TK ON_TK OFF_TK  TYPE_SEP_TK
%token <cmd>		FF_TK BE_TK CWP_TK EVALUABLE_TK PREDICATE_TK FUNCTION_TK REQUIRE_TK SYM_TK
%token <cmd>		OPRS_DATA_PATH_TK OP_PREDICATE_TK DUMP_TK 

%token <cmd>		OPF_VERSION1 OPF_VERSION2

%token <cmd>		FT_INVOCATION_TK FT_CONTEXT_TK FT_CALL_TK FT_ACTION_TK 
%token <cmd>		FT_SETTING_TK FT_PROPERTIES_TK FT_DOCUMENTATION_TK FT_EFFECTS_TK 

%token <cmd>		NT_PROCESS_TK NT_START_TK NT_END_TK NT_ELSE_TK 
%token <cmd>		NT_THEN_TK NT_IF_TK 

%token <cmd>		ET_GOAL_TK ET_ELSE_TK ET_THEN_TK ET_IF_TK 

%token <cmd>		SPEC_ACT_TK RESET_DOT_TK
%token <cmd>		OP_LISP_TK CP_LISP_TK OP_ARRAY_TK CP_ARRAY_TK OPENP_TK CLOSEP_TK EOF_TK
%token <cmd>		TFT_INVOCATION_TK TFT_CONTEXT_TK TFT_SETTING_TK TFT_BODY_TK
%token <cmd>		TFT_PROPERTIES_TK TFT_DOCUMENTATION_TK TFT_EFFECTS_TK TFT_ACTION_TK 
%token <cmd>	        TFT_CALL_TK

%token <cmd>		WHILE_TK DO_TK PAR_TK DEFOP_TK IF_TK ELSE_TK GOTO_TK BREAK_TK ELSEIF_TK LABEL_TK

%type <integer> 	pos_x pos_y pp_width
%type <bool> 		debug_flag boolean visible pp_fill split join
%type <string> 		op_name node_name edge_name prop_name documentation file_name write_file_name read_file_name symbol
%type <pred_func_rec>	pred_func
%type <list_term> 	term_list
%type <var> 		variable
%type <term> 		term 
%type <expr>		expr call simple_action edge_expr invocation setting
%type <action_field>	action action_expr
%type <list_expr> 	list_expr context list_par_expr effects list_effects

%type <dn> 		body inst list_inst while_inst do_inst par_inst top_expr if_inst comment goto_inst
%type <dn> 		label_inst break_inst if_part_inst
%type <prop_list>       properties properties_list
%type <prop> 		property
%type <var_list> 	var_list
%type <list>		edge_knots list_knots body_list

%%
one_command_return: 				
	command 			{YYACCEPT;} 
	;

command:
	EOF_TK 					{;} 
	| LOAD_TK OPF_TK			{yy_begin_0();}
		read_file_name 			{if (!be_quiet) printf(LG_STR("%sLoading/compiling OP File \"%s\".\n",
									      "%sChargement/compilation fichier OP \"%s\".\n"),parser_indent, $4);
						 if (open_file_for_yyparse($4)<0) {
						      printf(LG_STR("%sAbort loading/compiling OP File.\n",
								    "%sAbandon chargement/compilation fichier OP.\n"),parser_indent);
						      FREE($4);
						      YYABORT;
						 }
						 current_graph_file_name = $4;} 
		op_graph 			{close_file_for_yyparse(); FREE($4);
						 if (!be_quiet) printf(LG_STR("%sLoading/compiling OP File done.\n",
									      "%sChargement/compilation fichier OP terminé.\n"),parser_indent);}
	| DUMP_TK OP_TK				{yy_begin_0();} 
		write_file_name			{if (!be_quiet) printf(LG_STR("Dumping OP db in \"%s\".\n",
							       "Dump de base de OP dans \"%s\".\n"),$4);
		                                 dump_op_database_from_parser(relevant_op, $4);
						 if (!be_quiet) printf(LG_STR("\nDumping OP db done.\n",
							       "\nDump de base de OP terminé.\n"));
						 FREE($4);}
	| LOAD_TK DUMP_TK OP_TK			{yy_begin_0();} 
		read_file_name			{if (!be_quiet) printf(LG_STR("%sLoading compiled OPs from \"%s\".\n",
							       "%sChargement OPs compilés de \"%s\".\n"),parser_indent,$5);
		                                 load_op_database_from_parser(relevant_op, $5);
						 if (!be_quiet) printf(LG_STR("\n%sLoading compiled OPs done.\n",
							       "\n%sChargement OPs compilés terminé.\n"),parser_indent);
						 FREE($5);}
	| EMPTY_TK OP_TK DB_TK			{clear_op_database(relevant_op, compile_graphix); 
						 if (!be_quiet) printf(LG_STR("OP Database cleared.\n",
									      "Base de OP vidée.\n"));}
	| INCLUDE_TK   				{yy_begin_0();} 
		read_file_name 			{if (open_file_for_yyparse($3)<0) YYABORT; FREE($3);} 
	| REQUIRE_TK   				{yy_begin_0();} 
		read_file_name 			{if (! (loaded_include_file($3))) {
		     					if (open_file_for_yyparse($3)<0) YYABORT;
							provide_include_file($3);
						} else  FREE($3);} 
 	| DECLARE_TK ID_TK			{yy_begin_0();}
		SYMBOL_TK 			
 	| DECLARE_TK PREDICATE_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{check_pfr = TRUE;}
 	| DECLARE_TK FUNCTION_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func 			{check_pfr = TRUE;}
 	| DECLARE_TK OP_PREDICATE_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{declare_predicate_op($4); check_pfr = TRUE;}
 	| DECLARE_TK CWP_TK 			{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{declare_cwp($4); check_pfr = TRUE;}
 	| DECLARE_TK FF_TK  			{yy_begin_0(); check_pfr = FALSE;}
		pred_func INTEGER_TK 		{declare_ff($4,$5); check_pfr = TRUE;}
 	| DECLARE_TK BE_TK  			{yy_begin_0(); check_pfr = FALSE;}
		pred_func 			{declare_be($4); check_pfr = TRUE;}
 	| DECLARE_TK EVALUABLE_TK PREDICATE_TK  {yy_begin_0(); check_pfr = FALSE;}
		SYMBOL_TK INTEGER_TK    	{make_and_declare_eval_pred($5,NULL,$6,TRUE);
		                                 check_pfr = TRUE;}
 	| DECLARE_TK EVALUABLE_TK FUNCTION_TK   {yy_begin_0(); check_pfr = FALSE;}
		SYMBOL_TK INTEGER_TK      	{make_and_declare_eval_funct($5,NULL,$6);
		                                 check_pfr = TRUE;}
 	| DECLARE_TK ACTION_TK                  {yy_begin_0(); check_pfr = FALSE;}
		SYMBOL_TK INTEGER_TK       	{make_and_declare_action($4,NULL,$5);
		                                 check_pfr = TRUE;}
	| SET_TK OPRS_DATA_PATH_TK QSTRING_TK	{oprs_data_path = $3;}

	| SET_TK SYM_TK debug_flag		{compiler_option[CHECK_SYMBOL]=$3;}
	| SET_TK FUNCTION_TK debug_flag		{compiler_option[CHECK_PFR]=$3;}
	| SET_TK PREDICATE_TK debug_flag	{compiler_option[CHECK_PFR]=$3;}

        ;

debug_flag:
	 ON_TK 					{$$ = TRUE;}
	|OFF_TK					{$$ = FALSE;}
	   ;





op_graph:					{yy_begin_OPF_VERSION();}
	new_op_graph 				
	| error					{warning(LG_STR("Either this is not a OP file, or this OP file version is obsolete.\n\
\tConvert it using the OP-Editor (load the file in the op-editor and then save it)\n\
\tor issue the \"op-editor -c <file-to-convert>*\" command.",
								"Ou bien ce fichier n'es pas un fichier de OP, ou cette version de format de OP est obsolète.\n\
\tSi c'est le case, convertisses le en utilisant le OP-Editor (chargez le puis sauvez le)\n\
\tou bien tapez la commande \"op-editor -c <fichier-a-convertir>*\"."));}

	;





new_op_graph: | 
        OPENP_TK OPF_VERSION1 			{yy_begin_0();}
	nop_list CLOSEP_TK			      
        | OPENP_TK OPF_VERSION2 			{yy_begin_0();}
	  OPENP_TK					
           symbol_list				
          CLOSEP_TK nop_list CLOSEP_TK
	| OPENP_TK 				{yy_begin_0();} 
	     top_list CLOSEP_TK
	;

symbol_list: 	
        
	| symbol_list symbol			 
	;

nop_list: 	
        
	| nop_list nop
	| nop_list top
	;

nop: 	
	OPENP_TK op_name 				{init_make_op($2, compile_graphix);} 
	OPENP_TK nlist_fields CLOSEP_TK
	OPENP_TK nlist_nodes CLOSEP_TK
	OPENP_TK nlist_edges CLOSEP_TK CLOSEP_TK		{finish_loading_op(current_op,  (compile_graphix?global_draw_data:NULL));
						 add_op_to_relevant_op(current_op,relevant_op);
						 if (!be_quiet) printf("    %s%s\n",parser_indent,$2);
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
                {build_and_add_node(current_op, $3, NT_ELSE, $4, $5, $6, $7, (compile_graphix?global_draw_data:NULL));}
	| NT_THEN_TK {yy_begin_0();} node_name join split pos_x pos_y
                {build_and_add_node(current_op, $3, NT_THEN, $4, $5, $6, $7, (compile_graphix?global_draw_data:NULL));}
	| NT_IF_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_IF, $4, $5, $6, $7,  (compile_graphix?global_draw_data:NULL));}
	| NT_PROCESS_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_PROCESS, $4, $5, $6, $7,  (compile_graphix?global_draw_data:NULL));}
	| NT_END_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_END, $4, $5, $6, $7,  (compile_graphix?global_draw_data:NULL));}
	| NT_START_TK {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_START, $4, $5, $6, $7,  (compile_graphix?global_draw_data:NULL));}
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
		{build_invocation(current_op, $3, $4, $5, $6, $7, $8, $9, (compile_graphix?global_draw_data:NULL));}
	FT_CALL_TK {yy_begin_0();} node_name call pos_x pos_y visible pp_width pp_fill
		{build_call(current_op, $3, $4, $5, $6, $7, $8, $9, (compile_graphix?global_draw_data:NULL));}
	| FT_CONTEXT_TK {yy_begin_0();} node_name context  pos_x pos_y visible pp_width pp_fill
		{build_context(current_op, $3, $4, $5, $6, $7, $8, $9,  (compile_graphix?global_draw_data:NULL));}
	| FT_SETTING_TK {yy_begin_0();} node_name setting  pos_x pos_y visible pp_width pp_fill
		{build_setting(current_op, $3, $4, $5, $6, $7, $8, $9,  (compile_graphix?global_draw_data:NULL));}
	| FT_PROPERTIES_TK {yy_begin_0();} node_name properties  pos_x pos_y visible pp_width pp_fill
		{build_properties(current_op, $3, $4, $5, $6, $7, $8, $9,  (compile_graphix?global_draw_data:NULL));}
	| FT_DOCUMENTATION_TK {yy_begin_0();} node_name documentation pos_x pos_y visible pp_width pp_fill
		{if (compile_graphix)                 
		     build_documentation(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);
		     else
		     FREE($4);}
	| FT_EFFECTS_TK {yy_begin_0();} node_name effects pos_x pos_y visible pp_width pp_fill
		{build_effects(current_op, $3, $4, $5, $6, $7, $8, $9,  (compile_graphix?global_draw_data:NULL));}
	| FT_ACTION_TK {yy_begin_0();} node_name action  pos_x pos_y visible pp_width pp_fill
		{build_action(current_op, $3, $4, $5, $6, $7, $8, $9,  (compile_graphix?global_draw_data:NULL));}
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

invocation:                     {check_symbol = TRUE;}
            expr		{$$ = $2;
			        check_symbol = FALSE;}
	    ;

call: 				{$$ = NULL;}
        | expr			{$$ = $1;}
	;

context: 			{$$ = NULL;}
        | expr			{$$ = sl_make_slist();sl_add_to_tail($$,$1);}
        | list_par_expr		{$$ = $1;}
	;

setting: 				{$$ = NULL;}
	|                       {check_symbol = TRUE;}
          expr			{$$ = $2;
			         check_symbol = FALSE;}
	;

properties: 			{$$ = NULL;}
	| OPENP_TK                   {check_symbol = TRUE;}
           properties_list CLOSEP_TK  {check_symbol = FALSE; $$ = $3;}
	;

properties_list:			{$$ = sl_make_slist();}
 	| properties_list property	{if ($2) sl_add_to_tail($1,$2); $$=$1;}
	;

documentation:			{$$ = NULL;}
	| QSTRING_TK		{$$ = $1;}
	;



effects:                       {check_symbol = TRUE;}
          list_effects		{$$ = $2;
			         check_symbol = FALSE;}
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
	OPENP_TK  {yy_begin_0();} pred_func 				
		term_list CLOSEP_TK		 	 {$$=build_simple_action_cached($3,$4);}
;

nlist_edges: 
	
	| nlist_edges nedge  			
	;

nedge: 	
	OPENP_TK {yy_begin_EDGE_TYPE();} nedge_typed CLOSEP_TK
	| error RESET_DOT_TK 			{warning(LG_STR("Parsing error, badly formed edge",
								"Erreur de parsing, arc mal formé"));}
	;



edge_expr:                    {check_symbol = TRUE;}
         expr 		{$$ = $2;
			         check_symbol = FALSE;}
	;

edge_knots:
	OPENP_TK list_knots CLOSEP_TK			{$$ = $2;}
	;

list_knots:  			{$$ = sl_make_slist();}
	| list_knots OPENP_TK INTEGER_TK INTEGER_TK CLOSEP_TK	{sl_add_to_tail($1,(void *)$3); sl_add_to_tail($1,(void *)$4); $$ = $1;}
	;

op_name:
	SYMBOL_TK				{$$=$1;}
	;

edge_name: 
	SYMBOL_TK				{$$=$1;}
	;

node_name: 
	SYMBOL_TK				{$$=$1;}
	;


nedge_typed:
	ET_GOAL_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_GOAL, $6, $7, $8, $9, $10, $11, (compile_graphix?global_draw_data:NULL));}
	| ET_IF_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_IF, $6, $7, $8, $9, $10, $11, (compile_graphix?global_draw_data:NULL));}
	| ET_ELSE_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_ELSE, (compile_graphix?global_draw_data:NULL));}
	| ET_THEN_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_THEN, (compile_graphix?global_draw_data:NULL));}
	;

top_list: 
	
	| top_list top
	|  error RESET_DOT_TK 			{warning(LG_STR("Parsing error, expecting a Text OP list",
								"Erreur de parsing, attendait une liste de OP textuel"));}
	;

top: OPENP_TK DEFOP_TK op_name 			{init_make_top($3, compile_graphix);} 
	       fields_list
	CLOSEP_TK					{finish_loading_top(current_op, (compile_graphix?global_draw_data:NULL));
						 add_op_to_relevant_op(current_op,relevant_op);
						 if (!be_quiet) printf("    %s%s\n",parser_indent,$3);
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




variable:
	LOGICAL_VAR_TK 				{$$=build_var_in_frame($1,LOGICAL_VARIABLE);}
	| LOGICAL_VAR_TK  TYPE_SEP_TK SYMBOL_TK	{$$=build_var_in_frame($1,LOGICAL_VARIABLE);
						 set_var_type_by_name($3, $$);}
	| PROGRAM_VAR_TK			{$$=build_var_in_frame($1,PROGRAM_VARIABLE);}
	| PROGRAM_VAR_TK  TYPE_SEP_TK SYMBOL_TK	{$$=build_var_in_frame($1,PROGRAM_VARIABLE);
						 set_var_type_by_name($3, $$);}
	;

symbol:  SYMBOL_TK				{$$=$1;}
	| SYMBOL_TK TYPE_SEP_TK SYMBOL_TK 	{set_type_by_name($3,$1);$$=$1;}
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





read_file_name: QSTRING_TK			{$$=canonicalize_read_file_name($1);}
	;

write_file_name: QSTRING_TK			{$$=$1;}
	;


%%

Slist *included_file_name;

PBoolean loaded_include_file(char *filename)
{
	 return (included_file_name ?
	     (sl_search_slist(included_file_name, filename, equal_string) != NULL) :
	     FALSE);
}

void provide_include_file(char *filename)
{



     sl_add_to_tail(included_file_name,filename);
}
