%{
static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-parser.y -- yacc grammaire
 * 
 * Copyright (c) 1991-2004 Francois Felix Ingrand.
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
#include "oprs.h"
#include "help_f.h"
#include "oprs-print.h"
#include "oprs-sprint.h"
#include "oprs_f.h"
#include "op-default.h"
#include "database.h"
#include "oprs-pred-func.h"
#include "relevant-op.h"
#include "parser-funct.h"
#include "oprs-main.h"
#include "top-lev.h"
#include "oprs-profiling.h"

#ifndef NO_GRAPHIX
#include "xoprs-main.h"
#endif

#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "op-structure_f.h"
#include "top-structure_f.h"
#include "database_f.h"
#include "op-compiler_f.h"
#include "fact-goal_f.h"
#include "parser-funct_f.h"
#include "unification_f.h"
#include "relevant-op_f.h"
#include "soak_f.h"
#include "other_proto.h"
#include "oprs-send-m_f.h"
#include "oprs-init_f.h"
#include "int-graph_f.h"
#include "conditions_f.h"
#include "oprs-pred-func_f.h"
#include "type_f.h"
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
    double    		real;        /* real value */
    int    		integer;    /* integer value */
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
    NodeList 		nl;
    EdgeList 		el;
    Node_Type 		nt;
    Slist *		list;
}

%token <real> 		REAL_TK
%token <pointer>	POINTER_TK
%token <integer> 	INTEGER_TK
%token <string> 	QSTRING_TK SYMBOL_TK COMMENT_TK LOGICAL_VAR_TK PROGRAM_VAR_TK

%token <cmd>		HELP_TK ADD_TK CONCLUDE_TK CONSULT_TK DECLARE_TK DELETE_TK 
%token <cmd>		ECHO_TK EMPTY_TK INCLUDE_TK LIST_TK LOAD_TK RELOAD_TK RECEIVE_TK 
%token <cmd>		RESET_TK SEND_TK SET_TK SHOW_TK STAT_TK TRACE_TK UNIFY_TK DB_TK
%token <cmd>		FACT_TK GOAL_TK TSYM_TK INTENTION_TK OP_TK OPF_TK KERNEL_TK CONDITION_TK
%token <cmd>		COMPLETION_TK
%token <cmd>		META_TK EVAL_TK POST_TK PARALLEL_TK PROFILING_TK SILENT_TK FRAME_TK 
%token <cmd>		SOAK_TK THREADING_TK INTEND_TK SUC_FAIL_TK ALL_TK ON_TK OFF_TK 
%token <cmd>		VERSION_TK COPYRIGHT_TK MEMORY_TK FF_TK BE_TK CWP_TK RELEVANT_TK 
%token <cmd>		APPLICABLE_TK TAKE_STDIN_TK DISCONNECT_TK QUIT_TK TEXT_TK 
%token <cmd>		GRAPHIC_TK CALL_EXTERNAL_TK DUMMY_TK SAVE_TK UNDECLARE_TK TYPE_TK
%token <cmd>		EVALUABLE_TK AC_TK PREDICATE_TK FUNCTION_TK REQUIRE_TK SYM_TK
%token <cmd>		VARIABLE_TK OPRS_DATA_PATH_TK OP_PREDICATE_TK DUMP_TK MAKE_TK WORLD_TK
%token <cmd>		STEP_TK  NEXT_TK HALT_TK RUN_TK STATUS_TK FAILURE_TK USER_TK

%token <cmd>		OPF_VERSION1 OPF_VERSION2 TYPE_SEP_TK

%token <cmd>		FT_INVOCATION_TK FT_CONTEXT_TK FT_ACTION_TK FT_CALL_TK
%token <cmd>		FT_SETTING_TK FT_PROPERTIES_TK FT_DOCUMENTATION_TK FT_EFFECTS_TK 

%token <cmd>		NT_PROCESS_TK NT_START_TK NT_END_TK NT_ELSE_TK 
%token <cmd>		NT_THEN_TK NT_IF_TK 

%token <cmd>		ET_GOAL_TK ET_ELSE_TK ET_THEN_TK ET_IF_TK 

%token <cmd>		SPEC_ACT_TK RESET_DOT_TK
%token <cmd>		OR_TK TEMP_CONCLUDE_TK RETRACT_TK AND_TK
%token <cmd>		COND_ADD_TK COND_DEL_TK
%token <cmd>		OP_LISP_TK CP_LISP_TK OP_ARRAY_TK CP_ARRAY_TK OPENP_TK CLOSEP_TK
%token <cmd>		ACHIEVE_TK TEST_TK  PRESERVE_TK MAINTAIN_TK WAIT_TK CALL_TK EOF_TK
%token <cmd>		TFT_INVOCATION_TK TFT_CONTEXT_TK TFT_SETTING_TK TFT_BODY_TK TFT_CALL_TK
%token <cmd>		TFT_PROPERTIES_TK TFT_DOCUMENTATION_TK TFT_EFFECTS_TK TFT_ACTION_TK

%token <cmd>		WHILE_TK DO_TK PAR_TK DEFOP_TK IF_TK ELSE_TK GOTO_TK BREAK_TK ELSEIF_TK LABEL_TK 

%type <cmd>		reset_or_eof
%type <integer> 	pos_x pos_y pp_width
%type <bool> 		debug_flag boolean visible pp_fill split join
%type <string> 		op_name node_name edge_name prop_name documentation file_name write_file_name read_file_name symbol
%type <pred_func_rec>	pred_func
%type <list_term> 	term_list
%type <var> 		variable
%type <term> 		term 
%type <expr> 		simple_action
%type <action_field>	action action_expr
%type <expr> 		call expr invocation setting expr edge_expr
%type <list_expr> 	list_expr list_par_expr context effects list_effects
/* %type <add_del_list> 	*/
%type <dn> 		body inst list_inst while_inst do_inst par_inst top_expr if_inst comment goto_inst
%type <dn> 		label_inst break_inst if_part_inst
%type <prop_list>       properties properties_list
%type <prop> 		property
%type <var_list> 	var_list
%type <list>		edge_knots list_knots body_list symbol_list
%%
one_command_return: 				
	command 			{YYACCEPT;} /* We have to return explicitly to free the memory
						       and get out of here*/
	| EOF_TK			{YYACCEPT;}
	;

command:
/*	EOF_TK 					{}  Just to swallow EOF in include files...
	| */ UNIFY_TK  				{enable_variable_parsing();  yy_begin_0();}
		expr 
		expr 				{test_unify_expr($3,$4); disable_variable_parsing(); 
						 free_expr($3); free_expr($4);}
	| LOAD_TK DB_TK 			{yy_begin_0();}
		read_file_name 			{printf(LG_STR("%sLoading DB \"%s\".\n",
							       "%sChargement base de faits \"%s\".\n"),parser_indent, $4); 
						 if (open_file_for_yyparse($4)<0) {
						      printf(LG_STR("%sAbort Loading DB.\n",
								    "%sAbandon chargement base de faits.\n"),parser_indent);
						      FREE($4);
						      YYABORT; 
						 } 
						 FREE($4);}
		db_expr_list_par 		{close_file_for_yyparse(); recheck_all_conditions();
						 printf(LG_STR("%sLoading DB done.\n",
							       "%sChargement base de faits terminé.\n"),parser_indent);}
	| LOAD_TK OP_TK				{yy_begin_0(); current_graph_file_name = "stdin";}
	   	top_or_nop
	| LOAD_TK OPF_TK			{yy_begin_0();}
		read_file_name 			{char *dopf_filename;
		                                 if (dopf_filename = read_file_name_newer_dopf_p($4)) {
		                                 printf(LG_STR("%sLoading compiled OPs from \"%s\".\n",
							       "%sChargement OPs compilés de \"%s\".\n"),parser_indent,dopf_filename);
		                                 load_op_database_from_parser(current_oprs->relevant_op, dopf_filename);
						 printf(LG_STR("\n%sLoading compiled OPs done.\n",
							       "\n%sChargement OPs compilés terminé.\n"),parser_indent);
						 FREE(dopf_filename);
						 FREE($4);
						 YYACCEPT;}
                                                 if ( !dev_env ) {
						      printf(LG_STR("This command is only available on OPRS Development version.\n",
								    "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						      FREE($4);
						      YYABORT;
						 }
                                                 if (!be_quiet) printf(LG_STR("%sLoading/Compiling OP File \"%s\".\n",
									      "%sChargement/Compilation fichier OP \"%s\".\n"),parser_indent, $4);
						 if (open_file_for_yyparse($4)<0) {
						      printf(LG_STR("%sAbort loading/compiling OP File.\n",
								    "%sAbandon chargement/compilation fichier OP.\n"),parser_indent);
						      FREE($4);
						      YYABORT;
						 }
						 load_op_list = NULL; current_graph_file_name = $4;} 
		op_graph 			{close_file_for_yyparse(); FREE($4);
						 if (!be_quiet) printf(LG_STR("%sLoading/Compiling OP File done.\n",
									      "%sChargement/Compilation fichier OP terminé.\n"),parser_indent);}
	| LOAD_TK OP_TK 			{yy_begin_0();}
		symbol_list read_file_name	{ if ( !dev_env ) {
						      printf(LG_STR("This command is only available on OPRS Development version.\n",
								    "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						      FREE($5);
						      FREE_SLIST($4);
						      YYABORT;
						 }
                                                 if (!be_quiet) printf(LG_STR("%sLoading/Compiling some OPs from OP File \"%s\".\n",
									      "%sChargement/Compilation d'OPs du fichier OP \"%s\".\n"),parser_indent, $5);
						 if (open_file_for_yyparse($5)<0) {
						      printf(LG_STR("%sAbort loading/compiling  some OPs fromOP File.\n",
								    "%sAbandon chargement/compilation d'OPs du fichier OP.\n"),parser_indent);
						      FREE($5);
						      YYABORT;
						 }
						 load_op_list = $4; current_graph_file_name = $5;}
		op_graph 			{close_file_for_yyparse(); FREE($5); FREE_SLIST($4);
						 if (!be_quiet) printf(LG_STR("%sLoading/Compiling some OPs from OP File done.\n",
									      "%sChargement/Compilation d'OPs du fichier OP terminé.\n"),parser_indent);}
	| DELETE_TK OP_TK			{yy_begin_0();} 
		op_name  			{delete_op_name_from_rop($4,current_oprs->relevant_op,
#ifdef NO_GRAPHIX
									 FALSE
#else
									 (global_draw_data?TRUE:FALSE)
#endif
									 );}
	| DELETE_TK OPF_TK			{yy_begin_0();} 
		file_name	  		{delete_opf_from_rop($4,current_oprs->relevant_op,
#ifdef NO_GRAPHIX
								     FALSE
#else
								     (global_draw_data?TRUE:FALSE)
#endif
								     , TRUE);FREE($4);}
	| RELOAD_TK OPF_TK			{yy_begin_0();} 
		read_file_name	  		{char *dopf_filename;
		                                 text = sl_make_slist();
						 graphic = sl_make_slist();
						 step_list = sl_make_slist();

						 store_trace_status($4,current_oprs->relevant_op, &text, &graphic, &step_list);
						 delete_opf_from_rop($4,current_oprs->relevant_op,
#ifdef NO_GRAPHIX
								     FALSE
#else
								     (global_draw_data?TRUE:FALSE)
#endif
								     , TRUE);
                                                 if (dopf_filename = read_file_name_newer_dopf_p($4)) {
		                                    if (!be_quiet) printf(LG_STR("%sReloading compiled OPs from \"%s\".\n",
							                         "%sRechargement OPs compilés de \"%s\".\n"),
									  parser_indent,
									  dopf_filename);
		                                    load_op_database_from_parser(current_oprs->relevant_op, dopf_filename);
						    if (!be_quiet) printf(LG_STR("\n%sReloading compiled OPs done.\n",
						   	                         "\n%sRechargement OPs compilés terminé.\n"),parser_indent);
						    restore_trace_status($4,current_oprs->relevant_op, text, graphic, step_list);
						    FREE_SLIST(text);
						    FREE_SLIST(step_list);
						    FREE_SLIST(graphic);
						    FREE(dopf_filename);
						    FREE($4);
						    YYACCEPT;}
                                                 if ( !dev_env ) {
						    printf(LG_STR("This command is only available on OPRS Development version.\n",
							          "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						    FREE_SLIST(text);
						    FREE_SLIST(step_list);
						    FREE_SLIST(graphic);
						    FREE($4);
						    YYABORT;
						 }
						 if (!be_quiet) printf(LG_STR("%sReloading/recompiling OP File \"%s\".\n",
									      "%sRechargement/recompilation fichier OP \"%s\".\n"),parser_indent, $4);
						 if (open_file_for_yyparse($4)<0) {
						      printf(LG_STR("%sAbort reloading/recompiling OP File.\n",
								    "%sAbandon rechargement/recompilation fichier OP.\n"),parser_indent);
						      FREE($4);
						      YYABORT;
						 }
						 load_op_list = NULL; current_graph_file_name = $4;} 
		op_graph 			{close_file_for_yyparse();
						 if (!be_quiet) printf(LG_STR("%sReloading/recompiling OP file done.\n",
									      "%sRechargement/recompilation fichier OP terminé.\n"),parser_indent);
						 restore_trace_status($4,current_oprs->relevant_op, text, graphic, step_list);
						 FREE_SLIST(text);
						 FREE_SLIST(step_list);
						 FREE_SLIST(graphic);
						 FREE($4);}
	| SHOW_TK OP_TK				{yy_begin_0();} 
		 op_name  			{print_op_from_rop($4,current_oprs->relevant_op);}
	| LIST_TK OP_TK       			{list_ops(current_oprs->relevant_op);}
	| LIST_TK VARIABLE_TK OP_TK       	{list_ops_variables(current_oprs->relevant_op);}
	| LIST_TK OPF_TK      			{list_opfs(current_oprs->relevant_op);}
	| LIST_TK CWP_TK       			{list_cwp();}
	| LIST_TK FF_TK       			{list_ff();}
	| LIST_TK BE_TK       			{list_be();}
	| LIST_TK EVALUABLE_TK FUNCTION_TK 	{list_ef();}
	| LIST_TK OP_PREDICATE_TK		{list_op_predicate();}
	| LIST_TK EVALUABLE_TK PREDICATE_TK	{list_ep();}
	| LIST_TK FUNCTION_TK       		{list_pred_func();}
	| LIST_TK PREDICATE_TK       		{list_predicate();}
	| LIST_TK AC_TK       			{list_ac();}
	| LIST_TK ALL_TK       			{list_pred_func();list_cwp(); 
						 list_ff(); 
						 list_be(); 
						 list_op_predicate(); 
						 list_ep(); 
						 list_pred_func(); list_ef(); 
						 list_ac();}
	| TRACE_TK GRAPHIC_TK OP_TK  		{yy_begin_0();} 
		 op_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_graphic_op($5,current_oprs->relevant_op,$7);}
	| TRACE_TK GRAPHIC_TK OPF_TK  		{yy_begin_0();} 
		 file_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_graphic_opf($5,current_oprs->relevant_op,$7);FREE($5);}
	| TRACE_TK STEP_TK OP_TK  		{yy_begin_0();} 
		 op_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_step_op($5,current_oprs->relevant_op,$7);}
	| TRACE_TK STEP_TK OPF_TK  		{yy_begin_0();} 
		 file_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_step_opf($5,current_oprs->relevant_op,$7);FREE($5);}
	| TRACE_TK TEXT_TK OP_TK  		{yy_begin_0();} 
		 op_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_text_op($5,current_oprs->relevant_op,$7);}
	| TRACE_TK TEXT_TK OPF_TK  		{yy_begin_0();} 
		 file_name			{yy_begin_COMMAND();}
		 debug_flag			{trace_text_opf($5,current_oprs->relevant_op,$7);FREE($5);}
	| SHOW_TK VARIABLE_TK			{show_global_variable();}
	| SHOW_TK DB_TK				{print_database(current_oprs->database);}
	| SHOW_TK ALL_TK DB_TK				{print_database_key(current_oprs->database);}
	| SHOW_TK INTENTION_TK			{show_intentions(current_oprs->intention_graph);}
	| SHOW_TK CONDITION_TK			{show_conditions(current_oprs->conditions_list);}
	| SHOW_TK RUN_TK STATUS_TK		{report_run_status();}
	| STAT_TK DB_TK 			{stat_database(current_oprs->database);}
	| STAT_TK SYM_TK				{stat_id_hashtable();}
	| STAT_TK ALL_TK			{stat_id_hashtable();
						 stat_database(current_oprs->database);
						 stat_pred_func_hashtable();
						 stat_type_hashtable();}
	| SAVE_TK DB_TK				{yy_begin_0();} 
		write_file_name			{save_database(current_oprs->database, $4); FREE($4);}
	| DUMP_TK DB_TK				{yy_begin_0();} 
		write_file_name			{if ( !dev_env ) {
						      printf(LG_STR("This command is only available on OPRS Development version.\n",
								    "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						      FREE($4);
						      YYABORT;
						 }
                                                 printf(LG_STR("Dumping fact db in \"%s\".\n",
							       "Dump de base de faits dans \"%s\".\n"),$4);
		                                 dump_database_from_parser(current_oprs->database, $4);
						 printf(LG_STR("\nDumping DB done.\n",
							       "\nDump base de faits terminé.\n"));
						 FREE($4);}
	| DUMP_TK OP_TK				{yy_begin_0();} 
		write_file_name			{if ( !dev_env ) {
						      printf(LG_STR("This command is only available on OPRS Development version.\n",
								    "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						      FREE($4);
						      YYABORT;
						 }
                                                 printf(LG_STR("Dumping OP db in \"%s\".\n",
							       "Dump de base de OP dans \"%s\".\n"),$4);
		                                 dump_op_database_from_parser(current_oprs->relevant_op, $4);
						 printf(LG_STR("\nDumping OP db done.\n",
							       "\nDump de base de OP terminé.\n"));
						 FREE($4);}
	| DUMP_TK ALL_TK OPF_TK			{if ( !dev_env ) {
						      printf(LG_STR("This command is only available on OPRS Development version.\n",
								    "Cette commande n'est disponible que sur les versions de Développement de OPRS."));
						      YYABORT;
						 } else {
						      printf(LG_STR("Dumping all OP files.\n",
								    "Dump de tous les fichiers OPs\n"));
						      dump_all_opfs(current_oprs->relevant_op);
						      printf(LG_STR("Dumping all OP files done.\n",
								    "Dump de tous les fichiers OPs terminé.\n"));
						 }
                                                }
	| DUMP_TK KERNEL_TK			{yy_begin_0();} 
		write_file_name			{printf(LG_STR("Dumping kernel in \"%s\".\n",
							       "Dump du noyau dans \"%s\".\n"),$4);
		                                 dump_kernel_from_parser(current_oprs, $4);
						 printf(LG_STR("\nDumping kernel done.\n",
							       "\nDump noyau terminé.\n"));
						 FREE($4);}
	| LOAD_TK DUMP_TK DB_TK			{yy_begin_0();} 
		read_file_name			{printf(LG_STR("%sLoading fact db dump from \"%s\".\n",
							       "%sChargement dump base de faits de \"%s\".\n"),parser_indent,$5);
		                                 load_database_from_parser(current_oprs->database, $5);
						 printf(LG_STR("\n%sLoading db dump done.\n",
							       "\n%sChargement dump base de faits terminé.\n"),parser_indent,$5);
						 FREE($5);}
	| LOAD_TK DUMP_TK OP_TK			{yy_begin_0();} 
		read_file_name			{printf(LG_STR("%sLoading OP db dump from \"%s\".\n",
							       "%sChargement dump base de OP de \"%s\".\n"),parser_indent,$5);
		                                 load_op_database_from_parser(current_oprs->relevant_op, $5);
						 printf(LG_STR("\n%sLoading OP db dump done.\n",
							       "\n%sTerminé chargement dump base de OP.\n"),parser_indent);
						 FREE($5);}
	| LOAD_TK DUMP_TK KERNEL_TK		{yy_begin_0();} 
		read_file_name			{printf(LG_STR("%sLoading kenel dump from \"%s\".\n",
							       "%sChargement dump noyau OP de \"%s\".\n"),parser_indent,$5);
		                                 load_kernel_from_parser(current_oprs, $5);
						 printf(LG_STR("\n%sLoading kernel dump done.\n",
							       "\n%sChargement noyau terminé.\n"),parser_indent);
						 FREE($5);}
	| EMPTY_TK FACT_TK DB_TK		{clear_database(current_oprs->database);
						 if (!be_quiet) printf(LG_STR("Database cleared.\n",
									      "Base de faits vidée.\n"));}
	| EMPTY_TK OP_TK DB_TK			{clear_op_database(current_oprs->relevant_op,
#ifdef NO_GRAPHIX
								   FALSE
#else
								   TRUE
#endif
								   ); 
						 if (!be_quiet) printf(LG_STR("OP Database cleared.\n",
									      "Base de OP vidée.\n"));}
	| INCLUDE_TK   				{yy_begin_0();} 
		read_file_name 			{if (open_file_for_yyparse($3)<0) YYABORT; FREE($3);} 
	| REQUIRE_TK   				{yy_begin_0();} 
		read_file_name 			{if (! (loaded_include_file($3))) {
		     					if (open_file_for_yyparse($3)<0) YYABORT;
							provide_include_file($3);
						} else  FREE($3);} 
 	| DECLARE_TK TYPE_TK			{yy_begin_0();} 
	  SYMBOL_TK SYMBOL_TK			{if ($5== nil_sym) make_and_print_type($4, NULL); else make_and_print_type($4, $5);}
 	| SHOW_TK TYPE_TK SYMBOL_TK		{show_type($3);} 
 	| DECLARE_TK SYM_TK			{yy_begin_0();}
          dec_symbol_list
 	| DECLARE_TK TSYM_TK			{yy_begin_0();}
          dec_tsymbol_list
 	| DECLARE_TK PREDICATE_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{check_pfr = TRUE;}
 	| DECLARE_TK FUNCTION_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{check_pfr = TRUE;}
 	| DECLARE_TK OP_PREDICATE_TK		{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{declare_predicate_op($4); check_pfr = TRUE;}
 	| DECLARE_TK CWP_TK 			{yy_begin_0(); check_pfr = FALSE;}
		pred_func  			{declare_cwp($4); check_pfr = TRUE;}
 	| DECLARE_TK FF_TK  			{yy_begin_0(); check_pfr = FALSE;}
		pred_func INTEGER_TK 		{declare_ff($4,$5); check_pfr = TRUE;}
 	| DECLARE_TK BE_TK  			{yy_begin_0(); check_pfr = FALSE;}
		pred_func 			{declare_be($4); check_pfr = TRUE;}
 	| UNDECLARE_TK BE_TK  			{yy_begin_0();}
		pred_func 			{un_declare_be($4);}
 	| UNDECLARE_TK OP_PREDICATE_TK		{yy_begin_0();}
		pred_func 			{un_declare_predicate_op($4);}
	| STEP_TK  				{set_oprs_run_mode(STEP);}
	| NEXT_TK  				{set_oprs_run_mode(STEP_NEXT);}
	| HALT_TK  				{set_oprs_run_mode(HALT);}
	| RUN_TK  				{set_oprs_run_mode(RUN);}
	| DISCONNECT_TK				{leave_the_stdin();}
	| TAKE_STDIN_TK				{take_the_stdin();}
	| COMPLETION_TK				{send_completion_to_server(current_oprs->relevant_op);}
	| RESET_TK KERNEL_TK			{reset_oprs_kernel(current_oprs);}
	| CONCLUDE_TK  				{yy_begin_0(); check_pfr = FALSE; check_pfr = FALSE;} 
		 expr				{parser_conclude($3,current_oprs->database); free_expr($3);
		 				 check_pfr = TRUE; check_pfr = TRUE;}
	| CONCLUDE_TK error RESET_DOT_TK	{warning(LG_STR("Parsing error, expecting an expr after conclude",
								"Erreur de parsing, attendait une expr après la commande conclude"));}
	| DELETE_TK  				{yy_begin_0(); enable_variable_parsing();}
		expr				{delete_expr($3,current_oprs->database);
						 disable_variable_parsing(); free_expr($3);}
	| DELETE_TK error RESET_DOT_TK		{warning(LG_STR("Parsing error, expecting an expr after delete",
								"Erreur de parsing, attendait une expr après la commande delete"));}
	| CONSULT_TK  				{yy_begin_0(); enable_variable_parsing();
						 check_pfr = FALSE; check_pfr = FALSE;}
		expr				{parser_consult($3,current_oprs->database);
						 check_pfr = TRUE; check_pfr = TRUE;
					         disable_variable_parsing(); free_expr($3);}
	| CONSULT_TK error RESET_DOT_TK		{warning(LG_STR("Parsing error, expecting an expr after consult",
								"Erreur de parsing, attendait une expr après la commande consult"));}
	| consult_rop_frame expr		{parser_consult_expr_relevant_op($2,current_oprs->relevant_op);
					         disable_variable_parsing(); free_expr($2);}
	| consult_aop_frame expr 		{test_find_soak_expr($2,current_oprs);
					         disable_variable_parsing(); free_expr($2);}
	| RESET_TK PROFILING_TK			{reset_profiled_data();}
	| SHOW_TK MEMORY_TK			{pmstats();}
	| SHOW_TK PROFILING_TK			{report_profiled_data();}
	| SHOW_TK COPYRIGHT_TK			{print_copyright_notice();}
	| SHOW_TK VERSION_TK			{print_version();}
	| SHOW_TK OPRS_DATA_PATH_TK		{if (oprs_data_path && oprs_data_path[0])
	                                              printf(LG_STR("The current OPRS_DATA_PATH is: \"%s\"\n",
								    "Le OPRS_DATA_PATH est: \"%s\"\n"), oprs_data_path);
						 else printf(LG_STR("OPRS_DATA_PATH is not set.\n",
								    "OPRS_DATA_PATH n'a pas de valeur.\n"));}
	| SET_TK OPRS_DATA_PATH_TK QSTRING_TK	{oprs_data_path = $3;}

	| SET_TK META_TK debug_flag		{meta_option[META_LEVEL]=$3; report_meta_option_array();}
	| SET_TK SOAK_TK debug_flag		{meta_option[SOAK_MF]=$3; report_meta_option_array();}
	| SET_TK META_TK FACT_TK debug_flag	{meta_option[FACT_INV]=$4; report_meta_option_array();}
	| SET_TK META_TK GOAL_TK debug_flag	{meta_option[GOAL_INV]=$4; report_meta_option_array();}
	| SET_TK META_TK FACT_TK OP_TK debug_flag	{meta_option[APP_OPS_FACT]=$5; report_meta_option_array();}
	| SET_TK META_TK GOAL_TK OP_TK debug_flag	{meta_option[APP_OPS_GOAL]=$5; report_meta_option_array();}
	| SET_TK EVAL_TK POST_TK debug_flag	{run_option[EVAL_ON_POST]=$4;}
	| SET_TK PARALLEL_TK POST_TK debug_flag	{run_option[PAR_POST]=$4;}
	| SET_TK PARALLEL_TK INTEND_TK debug_flag	{run_option[PAR_INTEND]=$4;}
	| SET_TK PARALLEL_TK INTENTION_TK debug_flag	{run_option[PAR_EXEC]=$4;}
	| SET_TK SYM_TK debug_flag		{compiler_option[CHECK_SYMBOL]=$3;}
	| SET_TK AC_TK debug_flag		{compiler_option[CHECK_ACTION]=$3;}
	| SET_TK FUNCTION_TK debug_flag		{compiler_option[CHECK_PFR]=$3;}
	| SET_TK PREDICATE_TK debug_flag	{compiler_option[CHECK_PFR]=$3;}
	| SET_TK SILENT_TK debug_flag		{be_quiet = $3;}

	| TRACE_TK USER_TK TRACE_TK debug_flag		{set_user_trace(USER_TRACE,$4);}
	| TRACE_TK USER_TK OP_TK debug_flag	{set_user_trace(UT_OP,$4);}
	| TRACE_TK USER_TK INTEND_TK debug_flag		{set_user_trace(UT_INTEND,$4);}
	| TRACE_TK USER_TK SUC_FAIL_TK debug_flag	{set_user_trace(UT_SUC_FAIL,$4);}
	| TRACE_TK USER_TK SEND_TK debug_flag		{set_user_trace(UT_SEND_MESSAGE,$4);}
	| TRACE_TK USER_TK RECEIVE_TK debug_flag	{set_user_trace(UT_RECEIVE_MESSAGE,$4);}
	| TRACE_TK USER_TK FACT_TK debug_flag		{set_user_trace(UT_POST_FACT,$4);}
	| TRACE_TK USER_TK GOAL_TK debug_flag		{set_user_trace(UT_POST_GOAL,$4);}
	| TRACE_TK USER_TK RELEVANT_TK OP_TK debug_flag	{set_user_trace(UT_RELEVANT_OP,$5);}
	| TRACE_TK USER_TK APPLICABLE_TK OP_TK debug_flag	{set_user_trace(UT_SOAK,$5);}
	| TRACE_TK USER_TK THREADING_TK debug_flag	{set_user_trace(UT_THREADING,$4);}


	| TRACE_TK TEXT_TK debug_flag		{debug_trace[TEXT_OP]=$3;}
	| TRACE_TK GRAPHIC_TK debug_flag	{debug_trace[GRAPHIC_OP]=$3;}
	| TRACE_TK INTEND_TK debug_flag		{debug_trace[INTEND]=$3;}
	| TRACE_TK SUC_FAIL_TK debug_flag	{debug_trace[SUC_FAIL]=$3;}
	| TRACE_TK INTENTION_TK FAILURE_TK debug_flag	{debug_trace[INTENTION_FAILURE]=$3;}
	| TRACE_TK INTENTION_TK FAILURE_TK VARIABLE_TK debug_flag	{debug_trace[INTENTION_FAILURE]=$4;}
	| TRACE_TK SEND_TK debug_flag		{debug_trace[SEND_MESSAGE]=$3;}
	| TRACE_TK RECEIVE_TK debug_flag	{debug_trace[RECEIVE_MESSAGE]=$3;}
	| TRACE_TK FACT_TK debug_flag		{debug_trace[POST_FACT]=$3;}
	| TRACE_TK GOAL_TK debug_flag		{debug_trace[POST_GOAL]=$3;}
	| TRACE_TK DB_TK debug_flag		{debug_trace[DATABASE]=$3;}
	| TRACE_TK CONCLUDE_TK debug_flag	{debug_trace[CONCLUDE_FROM_PARSER]=$3;}
	| TRACE_TK DB_TK FRAME_TK debug_flag	{debug_trace[DATABASE_FRAME]=$4;}
	| TRACE_TK LOAD_TK OP_TK debug_flag	{debug_trace[OP_COMPILER]=$4;}
	| TRACE_TK RELEVANT_TK OP_TK debug_flag	{debug_trace[RELEVANT_OP]=$4;}
	| TRACE_TK APPLICABLE_TK OP_TK debug_flag	{debug_trace[SOAK]=$4;}
	| TRACE_TK THREADING_TK debug_flag	{debug_trace[THREADING]=$3;}
        | TRACE_TK ALL_TK debug_flag            {debug_trace[INTEND]=$3;  
						 debug_trace[SUC_FAIL]=$3;
						 debug_trace[SEND_MESSAGE]=$3;
						 debug_trace[RECEIVE_MESSAGE]=$3;
						 debug_trace[DATABASE]=$3;
						 debug_trace[POST_FACT]=$3;
						 debug_trace[POST_GOAL]=$3;
						 debug_trace[THREADING]=$3;
						 debug_trace[RELEVANT_OP]=$3;
						 debug_trace[THREADING]=$3;
						 debug_trace[SOAK]=$3;}
	| ECHO_TK 				{enable_variable_parsing();}
		echo_tout			{disable_variable_parsing();}
	| ECHO_TK error RESET_DOT_TK		{warning(LG_STR("Parsing error, expecting a expr, expr or expr after \"echo\"",
								"Erreur de parsing, attendait un expr, expr or expr après la commande \"echo\""));}
	| add_frame expr			{add_expr($2,current_oprs);
						 disable_variable_parsing();}
	| RECEIVE_TK 				{enable_variable_parsing(); yy_begin_0();}
		SYMBOL_TK expr 			{get_message($4,current_oprs,$3);}
	| SEND_TK 				{enable_variable_parsing(); yy_begin_0();}
		SYMBOL_TK expr			{send_message_expr($4,$3);
						 disable_variable_parsing();}
	| add_frame error RESET_DOT_TK		{warning(LG_STR("Parsing error, expecting a expr or a expr after add",
								"Erreur de parsing, attendait un expr or a expr après la commande add"));}
	| HELP_TK				{print_help_from_parser();}
        | CALL_EXTERNAL_TK INTEGER_TK           {
#ifdef OPRS_PROFILING
						 user_call_from_parser($2); 
#endif
						}
	| QUIT_TK				{quit = TRUE;}
	| RESET_DOT_TK				{}
	| error reset_or_eof			{warning(LG_STR("Parsing error, unknown command, reseting the parser",
								"Erreur de parsing, unknown command, reseting the parser"));}
        ;

consult_rop_frame:
	CONSULT_TK RELEVANT_TK OP_TK 		{yy_begin_0(); enable_variable_parsing();}
	;

consult_aop_frame:
	CONSULT_TK APPLICABLE_TK OP_TK		{yy_begin_0(); enable_variable_parsing();}
	;

debug_flag:
	 ON_TK 					{$$ = TRUE;}
	|OFF_TK					{$$ = FALSE;}
	   ;

dec_symbol_list:
	SYMBOL_TK {;}
	| OPENP_TK symbol_list CLOSEP_TK {FREE_SLIST($2);}
	;


dec_tsymbol_list:
	 SYMBOL_TK TYPE_SEP_TK SYMBOL_TK 	{set_type_by_name($3,$1);}
	| OPENP_TK tsymbol_list CLOSEP_TK {;}
	;

tsymbol_list: /*empty*/ {;}
	| tsymbol_list  SYMBOL_TK TYPE_SEP_TK SYMBOL_TK 	{set_type_by_name($4,$2);}
	;

symbol: SYMBOL_TK {$$=$1;}
	| SYMBOL_TK TYPE_SEP_TK SYMBOL_TK 	{set_type_by_name($3,$1);$$=$1;}
;

add_frame:
	ADD_TK 					{enable_variable_parsing(); yy_begin_0();}
	;

echo_tout:					{check_pfr = FALSE; check_pfr = FALSE;}
        expr					{print_expr($2); free_expr($2); printf("\n");
						 check_pfr = TRUE; check_pfr = TRUE;}
	;

/*DB section*/

db_expr_list_par:
	OPENP_TK db_expr_list CLOSEP_TK		{}
	| OPENP_TK error reset_or_eof 		{warning(LG_STR("Parsing error, expecting a list of expression",
									"Erreur de parsing, attendait une liste d'expression"));}
	   ;

reset_or_eof:  RESET_DOT_TK
	| EOF_TK
	;

db_expr_list:
	/*empty*/
	| db_expr_list expr 			{
#ifndef NO_GRAPHIX
						 static int i = 0;
#endif
						 conclude($2, current_oprs->database);
						 free_expr($2);
#ifndef NO_GRAPHIX
						 if (i++ == 35) {i = 0; process_xt_events();}
#endif
	                                      	}
	| error RESET_DOT_TK			{warning(LG_STR("Parsing error, expecting a liste of expression in the database",
								"Erreur de parsing, attendait une liste d'expression dans la base de faits"));}
	;

/**************/
/* OP section */
/**************/

op_graph:					{yy_begin_OPF_VERSION();}
	new_op_graph 				
	| error	EOF_TK				{warning(LG_STR("Either this is not a OP file, or this OP file version is obsolete.\n\
\tConvert it using the OP-Editor (load the file in the op-editor and then save it)\n\
\tor issue the \"op-editor -c <file-to-convert>*\" command.",
								"Ou bien ce fichier n'es pas un fichier de OP, ou cette version de format de OP est obsolète.\n\
\tSi c'est le case, convertisses le en utilisant le OP-Editor (chargez le puis sauvez le)\n\
\tou bien tapez la commande \"op-editor -c <fichier-a-convertir>*\"."));}

	;

/*****************************************************/
/* This is the new OP file grammar. Format Version 1 */
/*****************************************************/

new_op_graph: | /* empty  file... */
        OPENP_TK OPF_VERSION1 			{yy_begin_0();}
	nop_list CLOSEP_TK			      
        | OPENP_TK OPF_VERSION2 		{yy_begin_0();}
	  OPENP_TK				
	  symbol_list 				{FREE_SLIST($5);}
          CLOSEP_TK nop_list CLOSEP_TK
	| OPENP_TK 				{yy_begin_0();} 
	     top_list CLOSEP_TK
	;

symbol_list: 	     /* empty */ 	{$$ = sl_make_slist();}
	| symbol_list SYMBOL_TK		{sl_add_to_tail($1,$2); $$=$1;}	 
	;

nop_list: 	
        /* empty */
	| nop_list nop
	| nop_list top
	;


top_or_nop: top
	| nop
	;

nop: 	
	OPENP_TK op_name 				{init_make_op($2,
#ifdef NO_GRAPHIX
							      FALSE
#else
							      TRUE
#endif
							      );} /* Will initialize current_op. */
	OPENP_TK nlist_fields CLOSEP_TK
	OPENP_TK nlist_nodes CLOSEP_TK
	OPENP_TK nlist_edges CLOSEP_TK CLOSEP_TK		{finish_loading_op(current_op,
#ifdef NO_GRAPHIX
								    NULL
#else
								    global_draw_data
#endif
								    );
#ifndef NO_GRAPHIX
						 process_xt_events();
#endif
						 if (!load_op_list || sl_in_slist(load_op_list,$2)) {
                                                     add_op_to_relevant_op(current_op,current_oprs->relevant_op);
						     if (!be_quiet) printf("    %s%s\n",parser_indent,$2);
                                                 } else
						     free_op(current_op);
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
	NT_ELSE_TK 				{yy_begin_0();} 
	node_name join split pos_x pos_y	{build_and_add_node(current_op, $3, NT_ELSE, $4, $5, $6, $7,
#ifdef NO_GRAPHIX
								    NULL
#else
								    global_draw_data
#endif
	     );}
	| NT_THEN_TK  				{yy_begin_0();} 
	node_name join split pos_x pos_y	{build_and_add_node(current_op, $3, NT_THEN, $4, $5, $6, $7,
#ifdef NO_GRAPHIX
								    NULL
#else
								    global_draw_data
#endif
	     );}
	| NT_IF_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_IF, $4, $5, $6, $7, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
	| NT_PROCESS_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_PROCESS, $4, $5, $6, $7, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
	| NT_END_TK  {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_END, $4, $5, $6, $7, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
	| NT_START_TK {yy_begin_0();} node_name join split pos_x pos_y
		{build_and_add_node(current_op, $3, NT_START, $4, $5, $6, $7, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
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
		{build_invocation(current_op, $3, $4, $5, $6, $7, $8, $9,
#ifdef NO_GRAPHIX
				  NULL
#else
				  global_draw_data
#endif
				  );}
	| FT_CALL_TK {yy_begin_0();} node_name call  pos_x pos_y visible pp_width pp_fill
		{build_call(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
			       NULL
#else
			       global_draw_data
#endif
			       );}
	| FT_CONTEXT_TK {yy_begin_0();} node_name context  pos_x pos_y visible pp_width pp_fill
		{build_context(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
			       NULL
#else
			       global_draw_data
#endif
			       );}
	| FT_SETTING_TK {yy_begin_0();} node_name setting  pos_x pos_y visible pp_width pp_fill
		{build_setting(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
			       NULL
#else
			       global_draw_data
#endif
			       );}
	| FT_PROPERTIES_TK {yy_begin_0();} node_name properties  pos_x pos_y visible pp_width pp_fill
		{build_properties(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
				  NULL
#else
				  global_draw_data
#endif
				  );}
	| FT_DOCUMENTATION_TK {yy_begin_0();} node_name documentation pos_x pos_y visible pp_width pp_fill
		{
#ifndef NO_GRAPHIX
		     build_documentation(current_op, $3, $4, $5, $6, $7, $8, $9, global_draw_data);
#else
		     FREE($4);
#endif
		}
	| FT_EFFECTS_TK {yy_begin_0();} node_name effects pos_x pos_y visible pp_width pp_fill
		{build_effects(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
			       NULL
#else
			       global_draw_data
#endif
			       );}
	| FT_ACTION_TK {yy_begin_0();} node_name action  pos_x pos_y visible pp_width pp_fill
		{build_action(current_op, $3, $4, $5, $6, $7, $8, $9, 
#ifdef NO_GRAPHIX
			      NULL
#else
			      global_draw_data
#endif
			      );}
	;

include "yacc-graph-op.y"

nedge_typed:
	ET_GOAL_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_GOAL, $6, $7, $8, $9, $10, $11, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
	| ET_IF_TK {yy_begin_0();} node_name edge_name node_name edge_expr edge_knots pos_x pos_y pp_width pp_fill
		{build_and_add_edge(current_op, $3, $5, ET_IF, $6, $7, $8, $9, $10, $11, 
#ifdef NO_GRAPHIX
				    NULL
#else
				    global_draw_data
#endif
				    );}
	| ET_ELSE_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_ELSE, 
#ifdef NO_GRAPHIX
					      NULL
#else
					      global_draw_data
#endif
					      );}
	| ET_THEN_TK {yy_begin_0();} node_name edge_name node_name
		{build_and_add_then_else_edge(current_op, $3, $5, ET_THEN, 
#ifdef NO_GRAPHIX
					      NULL
#else
					      global_draw_data
#endif
					      );}
	;

top_list: 
	/* empty */
	| top_list top
	|  error RESET_DOT_TK 			{warning(LG_STR("Parsing error, expecting a Text OP list",
								"Erreur de parsing, attendait une liste de OP textuel"));}
	;

top: OPENP_TK DEFOP_TK op_name 			{init_make_top($3,
#ifdef NO_GRAPHIX
							      FALSE
#else
							      TRUE
#endif
							      );} /* Will initialize current_op. */
	       fields_list
	CLOSEP_TK					{finish_loading_top(current_op,
#ifdef NO_GRAPHIX
							      NULL
#else
							      global_draw_data
#endif
								    );
#ifndef NO_GRAPHIX
						 process_xt_events();
#endif
						 if (!load_op_list || sl_in_slist(load_op_list,$3)) {
                                                     add_op_to_relevant_op(current_op,current_oprs->relevant_op);
						     if (!be_quiet) printf("    %s%s\n",parser_indent,$3);
                                                 } else
						     free_op(current_op);
						 disable_variable_parsing();}
;

include "yacc-text-op.y"

variable:
	LOGICAL_VAR_TK 				{$$=build_var_in_frame($1,LOGICAL_VARIABLE);}
	| LOGICAL_VAR_TK  TYPE_SEP_TK SYMBOL_TK	{$$=build_var_in_frame($1,LOGICAL_VARIABLE);
						 set_var_type_by_name($3, $$);}
	| PROGRAM_VAR_TK			{$$=build_var_in_frame($1,PROGRAM_VARIABLE);}
	| PROGRAM_VAR_TK  TYPE_SEP_TK SYMBOL_TK	{$$=build_var_in_frame($1,PROGRAM_VARIABLE);
						 set_var_type_by_name($3, $$);}
	;

include "yacc-exp.y"
include "yacc-file.y"

%%

PBoolean loaded_include_file(char *filename)
{
	 return (current_oprs->included_file_name ?
	     (sl_search_slist(current_oprs->included_file_name, filename, equal_string) != NULL) :
	     FALSE);
}

void provide_include_file(char *filename)
{
  if (! current_oprs->included_file_name)  
    current_oprs->included_file_name = sl_make_slist();
  
  sl_add_to_tail(current_oprs->included_file_name,filename);
}
