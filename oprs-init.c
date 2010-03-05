static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-init.c -- 
 * 
 * Copyright (c) 1991-2010 Francois Felix Ingrand.
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

#if defined(WINSOCK)
#include <winsock.h>
#elif defined(VXWORKS)
#include "ioLib.h"
#include <hostLib.h>
#include <sys/ioctl.h>
#else
#include <fcntl.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN95
#include <unistd.h>
#endif

#include "constant.h"
#include "version.h"
#include "macro.h"

#include "oprs-main.h"

#ifndef NO_GRAPHIX
#include "xoprs-main.h"
#endif

#include "oprs-type.h"
#include "database.h"
#include "relevant-op.h"
#include "op-structure.h"
#include "parser-funct.h"
#include "oprs.h"
#include "oprs-profiling.h"
#include "soak.h"
#include "conditions.h"
#include "oprs-dump.h"
#include "intention.h"
#include "top-lev.h"
#include "oprs-error.h"
#include "oprs-sprint.h"
#include "type.h"

#include "action_f.h"
#include "database_f.h"
#include "ev-function_f.h"
#include "ev-predicate_f.h"
#include "conditions_f.h"
#include "help_f.h"
#include "mp-register_f.h"
#include "parser-funct_f.h"
#include "oprs-dump_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-init_f.h"
#include "oprs-profiling_f.h"
#include "oprs-socket_f.h"
#include "oprs-type_f.h"
#include "oprs-util_f.h"
#include "oprs_f.h"
#include "relevant-op_f.h"
#include "soak_f.h"
#include "top-lev_f.h"
#include "user-external_f.h"
#include "oprs-error_f.h"
#include "oprs-sprint_f.h"
#include "type_f.h"

#ifndef NO_GRAPHIX
#include "xoprs-intention.h"
#include "xoprs-intention_f.h"
#endif

#define OPRS_HELP_MESSAGE LG_STR(\
"Go to the oprs-server, and enter the 'accept' command.\n",\
"Tapez la commande 'accept' dans le oprs-server.\n")

#define OPRS_ARG_ERR_MESSAGE LG_STR(\
"Usage: %s [-s server-hostname] [-i server-port-number] [-a] [-A]\n\
\t[-m message-passer-hostname] [-j message-passer-port-number]\n\
\t[-x include-filename]* [-c command]* [-l upper|lower|none ] [-L en|fr]\n\
\t[-I size-ID-hash-table] [-P size-pred-hash-table]\n\
\t[-D size-database-hash-table]\n\
\t[-d oprs-data-path] [-n] client-name\n",\
"Utilisation: %s [-s server-machine] [-i server-numero-port] [-a] [-A]\n\
\t[-m message-passer-machine] [-j message-passer-numero-port]\n\
\t[-x include-fichier]* [-c commande]* [-l upper|lower|none ] [-L en|fr]\n\
\t[-I taille-table-ID] [-P taille-table-predicat]\n\
\t[-D taille-table-database]\n\
\t[-d oprs-data-path] [-n] nom-client\n")

PBoolean register_to_server = TRUE;
PBoolean register_to_mp = TRUE;

PBoolean dumped_code = FALSE;

extern char *server_hostname;
extern int server_port;

Op_Instance_List previous_soak;

#ifdef CONNECT_TO_IXTET
char *ixtet_name = NULL;	/* The name of the IxTeT reco module. */
PDate ixtet_start_date;	/* The zero */
#endif

void wrap_up(void)
{
#ifdef HAS_TIMER
     kill_condition_timer();
#endif
     end_kernel_user_hook();
     free_oprs(current_oprs);
     current_oprs = NULL;

     free_pred_func_hash();
     free_global_var_list();
     free_id_hash();
     free_type_hash();

#ifndef VXWORKS
     close(0);
#endif
     if (register_to_server) 
	  if (CLOSE_SOCK(ps_socket) < 0)
	       perror("wrap_up: ps_socket: close/closesocket");
     if (register_to_mp) 
	  disconnect_from_the_mp();

     FREE_SLIST(empty_list);
     FREE_OBJECT(goal_satisfied_in_db_op_ptr);
     FREE_OBJECT(goal_for_intention_op_ptr);
     FREE_OBJECT(goal_waiting_op_ptr);

     free_dynamic_slist_list();
#ifdef OPRS_DEBUG
     pmstats();
#endif
}

Slist *list_buffered_command = NULL;

void save_command_in_buffer (PString command)
{
     PString buffered_command;
     int total_length;

     if (list_buffered_command == NULL) {
	  list_buffered_command = sl_make_slist();
     }
	
     total_length =  strlen(command) + 1;
     buffered_command = (char *) MALLOC ( total_length);
     sprintf (buffered_command, "%s", command);
     sl_add_to_tail(list_buffered_command, buffered_command);
}

extern PBoolean use_dialog_error;
extern PBoolean flushing_xt_events;


int oprs_yyparse_caller(void)
{
     closed_file = FALSE;
     yy_begin_COMMAND();
     return oprs_yyparse();
}

PBoolean parse_one_or_more(void) /* return TRUE if the stop_checking_the_stdin alarm has been raised */
{
     if (parse_source == PS_SOCKET) stop_parsing_from_socket = FALSE;

     oprs_yyparse_caller();		/* call once */

     if (quit ||  (feof(stdin) != 0)) {
	  wrap_up();
	  socket_cleanup_and_exit(1);
     }     
     
     while (((parse_source == PS_SOCKET) &&
	     !stop_parsing_from_socket && /* The socket is emtpy */
	     !stop_checking_the_stdin) || /* enough commands have come now...  */
	    i_have_the_stdin || (parser_index > 0)) {
	  if (quit || (feof(stdin) && (parser_index == 0))) {
	       wrap_up();
	       socket_cleanup_and_exit(1);
	  }     
	  print_oprs_prompt();
	  oprs_yyparse_caller();
     }

     if ( use_dialog_error && (parser_message != NULL))
	  report_syntax_error("");

     if (list_buffered_command  != NULL) { /* A command has been saved in buffer, parse it */
	  if (flushing_xt_events)
	       fprintf(stderr,LG_STR("Calling parse_one_or_more while processing Xt events.\n",
				     "Appel de parse_one_or_more alors que l'on exécute des evénements Xt.\n"));
	  else {
	       Slist *tmp_list = list_buffered_command;
	       PString buffered_command;
	       
	       list_buffered_command = NULL; /* to do not loop in recursive call */
	       sl_loop_through_slist(tmp_list, buffered_command, PString) {
		    send_command_to_parser(buffered_command);
		    FREE(buffered_command);
	       }
	       FREE_SLIST(tmp_list);
	  }
     }
     
     return stop_checking_the_stdin;
}

void send_command_to_parser(PString s)
{

     if (flushing_xt_events) { /* We are already parsing a command : save this one in buffer. */
	  save_command_in_buffer(s);
     } else  {
	  Parse_Source_Type string_saved_parse_source;
	  int string_saved_line_number;
     
	  string_saved_parse_source = parse_source;
	  string_saved_line_number = string_line_number;
	  parse_source = PS_STRING;  
	  string_line_number = 1;
	  oprs_yy_scan_string(s);	   
 	  yy_begin_COMMAND();		/* On demarre le lex parser dans cet etat */
	  safe_parse_one_or_more();
	  yy_delete_buffer_no_arg();
	  parse_source = string_saved_parse_source;
	  string_line_number = string_saved_line_number;
     }
}

void report_meta_option_array(void)
{
     PBoolean one_is_on = FALSE;

     fprintf(stdout, LG_STR("The following meta-fact are posted: \n",
			    "Les méta faits suivant seront postés: \n"));

     if (meta_option[META_LEVEL] && meta_option[SOAK_MF]) {
	  fprintf(stdout, LG_STR(" SOAK",
				 " SOAK"));
	  one_is_on = TRUE;
     }
     if (meta_option[META_LEVEL] && meta_option[FACT_INV]){
	  fprintf(stdout, LG_STR(" FACT-INVOKED-OPS",
				 " FACT-INVOKED-OPS"));
	  one_is_on = TRUE;
     }
     if (meta_option[META_LEVEL] && meta_option[GOAL_INV]){
	  fprintf(stdout, LG_STR(" GOAL-INVOKED-OPS",
				 " GOAL-INVOKED-OPS"));
	  one_is_on = TRUE;
     }
     if (meta_option[META_LEVEL] && meta_option[APP_OPS_FACT]){
	  fprintf(stdout, LG_STR(" APPLICABLE-OPS-FACT",
				 " APPLICABLE-OPS-FACT"));
	  one_is_on = TRUE;
     }
     if (meta_option[META_LEVEL] && meta_option[APP_OPS_GOAL]){
	  fprintf(stdout, LG_STR(" APPLICABLE-OPS-GOAL",
				 " APPLICABLE-OPS-GOAL"));
	  one_is_on = TRUE;
     }
     if (one_is_on)
	  fprintf(stdout, "\n");
     else
	  fprintf(stdout, "none.\n");
}

void initialize_option_array()
{
#ifdef OPRS_PROFILING
     profiling_option[PROFILING] = TRUE;
     profiling_option[PROF_DB] = TRUE;
     profiling_option[PROF_ACTION_OP] = TRUE;
     profiling_option[PROF_OP] = TRUE;
     profiling_option[PROF_EP] = TRUE;
     profiling_option[PROF_EF] = TRUE;
     profiling_option[PROF_AC] = TRUE;
#endif

     meta_option[META_LEVEL] = TRUE;
     meta_option[SOAK_MF] = TRUE;
     meta_option[FACT_INV] = TRUE;
     meta_option[GOAL_INV] = TRUE;
     meta_option[APP_OPS_FACT] = FALSE;
     meta_option[APP_OPS_GOAL] = FALSE;

     compiler_option[CHECK_ACTION] = TRUE;
     compiler_option[CHECK_PFR] = TRUE;
     compiler_option[CHECK_SYMBOL] = TRUE;

     run_option[EVAL_ON_POST] = TRUE;
     run_option[PAR_POST] = TRUE;
     run_option[PAR_INTEND] = TRUE;
     run_option[PAR_EXEC] = TRUE;

     user_trace[USER_TRACE] = FALSE;
     user_trace[UT_POST_GOAL] = FALSE;
     user_trace[UT_POST_FACT] = FALSE;
     user_trace[UT_RECEIVE_MESSAGE] = FALSE;
     user_trace[UT_SEND_MESSAGE] = FALSE;
/*      user_trace[UT_DATABASE] = FALSE;
     user_trace[UT_INTENTION_FAILURE] = FALSE; */
     user_trace[UT_SUC_FAIL] = FALSE;
     user_trace[UT_INTEND] = FALSE;
     user_trace[UT_RELEVANT_OP] = FALSE;
     user_trace[UT_SOAK] = FALSE;
     user_trace[UT_OP] = FALSE;
     user_trace[UT_THREADING] = FALSE;


     report_meta_option_array();
}

void initialize_debug_trace_array()
{
     debug_trace[INTENTION_FAILURE] = TRUE;
     debug_trace[INT_FAIL_BIND] = FALSE;
     debug_trace[SUC_FAIL] = FALSE;
     debug_trace[DATABASE] = FALSE;
     debug_trace[DATABASE_KEY] = FALSE;
     debug_trace[DATABASE_FRAME] = FALSE;
     debug_trace[CONCLUDE_FROM_PARSER] = TRUE;
     debug_trace[POST_GOAL] = FALSE;
     debug_trace[POST_FACT] = FALSE;
     debug_trace[RECEIVE_MESSAGE] = TRUE;
     debug_trace[SEND_MESSAGE] = FALSE;
     debug_trace[INTEND] = FALSE;
     debug_trace[OP_COMPILER] = FALSE;
     debug_trace[RELEVANT_OP] = FALSE;
     debug_trace[SOAK] = FALSE;
     debug_trace[GRAPHIC_OP] = TRUE;
     debug_trace[TEXT_OP] = TRUE;
     debug_trace[GRAPHIC_INTEND] = TRUE;
     debug_trace[THREADING] = FALSE;
}

void initialize_cst()
{
     empty_list = sl_make_slist();	/* create the empty list (constant please...) */

     goal_satisfied_in_db_op_ptr = MAKE_OBJECT(Op_Structure);
     goal_for_intention_op_ptr = MAKE_OBJECT(Op_Structure);
     goal_waiting_op_ptr = MAKE_OBJECT(Op_Structure);

     goal_satisfied_in_db_op_ptr->name = "Goal satisfied in DB";
     goal_satisfied_in_db_op_ptr->start_point = NULL;
     goal_satisfied_in_db_op_ptr->invocation = NULL;
     goal_satisfied_in_db_op_ptr->call = NULL;
     goal_satisfied_in_db_op_ptr->context = NULL;
     goal_satisfied_in_db_op_ptr->setting = NULL;
     goal_satisfied_in_db_op_ptr->effects = NULL;
     goal_satisfied_in_db_op_ptr->node_list = empty_list;
     goal_satisfied_in_db_op_ptr->action = NULL;
     goal_satisfied_in_db_op_ptr->list_var = empty_list;
     goal_satisfied_in_db_op_ptr->properties = empty_list;
#ifdef OPRS_PROFILING
     goal_satisfied_in_db_op_ptr->time_active = zero_date;
     goal_satisfied_in_db_op_ptr->nb_relevant = 0;
     goal_satisfied_in_db_op_ptr->nb_applicable = 0;
     goal_satisfied_in_db_op_ptr->nb_intended = 0;
     goal_satisfied_in_db_op_ptr->nb_succes = 0;
     goal_satisfied_in_db_op_ptr->nb_failure = 0;
#endif
     goal_satisfied_in_db_op_ptr->text_traced = FALSE;
#ifndef NO_GRAPHIX
     goal_satisfied_in_db_op_ptr->documentation = NULL;
     goal_satisfied_in_db_op_ptr->graphic_traced = FALSE;
#endif
     goal_satisfied_in_db_op_ptr->step_traced = FALSE;

     goal_waiting_op_ptr->name = "Goal waiting OP";
     goal_waiting_op_ptr->start_point = NULL;
     goal_waiting_op_ptr->invocation = NULL;
     goal_waiting_op_ptr->call = NULL;
     goal_waiting_op_ptr->context = NULL;
     goal_waiting_op_ptr->setting = NULL;
     goal_waiting_op_ptr->effects = NULL;
     goal_waiting_op_ptr->node_list = empty_list;
     goal_waiting_op_ptr->action = NULL;
     goal_waiting_op_ptr->list_var = empty_list;
     goal_waiting_op_ptr->properties = empty_list;
#ifdef OPRS_PROFILING
     goal_waiting_op_ptr->time_active = zero_date;
     goal_waiting_op_ptr->nb_relevant = 0;
     goal_waiting_op_ptr->nb_applicable = 0;
     goal_waiting_op_ptr->nb_intended = 0;
     goal_waiting_op_ptr->nb_succes = 0;
     goal_waiting_op_ptr->nb_failure = 0;
#endif
     goal_waiting_op_ptr->text_traced = FALSE;
     goal_waiting_op_ptr->step_traced = FALSE;
#ifndef NO_GRAPHIX
     goal_waiting_op_ptr->documentation = NULL;
     goal_waiting_op_ptr->graphic_traced = FALSE;
#endif

     goal_for_intention_op_ptr->name = "Goal Intention";
     goal_for_intention_op_ptr->start_point = NULL;
     goal_for_intention_op_ptr->invocation = NULL;
     goal_for_intention_op_ptr->call = NULL;
     goal_for_intention_op_ptr->context = NULL;
     goal_for_intention_op_ptr->setting = NULL;
     goal_for_intention_op_ptr->effects = NULL;
     goal_for_intention_op_ptr->node_list = empty_list;
     goal_for_intention_op_ptr->action = NULL;
     goal_for_intention_op_ptr->list_var = empty_list;
     goal_for_intention_op_ptr->properties = empty_list;
     goal_for_intention_op_ptr->text_traced = FALSE;
#ifndef NO_GRAPHIX
     goal_for_intention_op_ptr->documentation = NULL;
     goal_for_intention_op_ptr->graphic_traced = FALSE;
#endif
#ifdef OPRS_PROFILING
     goal_for_intention_op_ptr->time_active = zero_date;
     goal_for_intention_op_ptr->nb_relevant = 0;
     goal_for_intention_op_ptr->nb_applicable = 0;
     goal_for_intention_op_ptr->nb_intended = 0;
     goal_for_intention_op_ptr->nb_succes = 0;
     goal_for_intention_op_ptr->nb_failure = 0;
#endif
     goal_for_intention_op_ptr->step_traced = FALSE;
}

void oprs_init_env(void)
{
     oprs_data_path = getenv("OPRS_DATA_PATH");
}
Slist *initial_commands = NULL;

void run_initial_commands(void) 
{
     if (initial_commands) {
	  char *command = (char *) sl_get_from_head(initial_commands);
	  while (command != NULL) {
	       send_command_to_parser(command); 
	       FREE (command);
	       command = (char *) sl_get_from_head(initial_commands);
	  }
	  FREE_SLIST(initial_commands);
     }
}

void add_initial_command(char *command_arg)
{
     char * command= (char *)MALLOC(strlen(command_arg) + 1 + 1);

     sprintf(command,"%s\n",command_arg);
     SAFE_SL_ADD_TO_TAIL(initial_commands, command);
}

void add_initial_include_command(char *include_filename)
{
     Sprinter *sp = make_sprinter(0);
     SPRINT(sp, 16 + strlen(include_filename),
	    sprintf(f,"include \"%s\"\n", include_filename)); 
     add_initial_command(SPRINTER_STRING(sp));

     free_sprinter(sp);
}

#ifndef VXWORKS
char *oprs_init_arg(int argc,char **argv, char **mp_hostn, int *mp_p, char **server_hostn, int *server_p)
{
     char *oprs_name = NULL, *lang_str = NULL;	/* Just to avoid GCC Warning */
     char *data_path = NULL;
     char *id_case_option = NULL;
     char *mp_hostn_g, *mp_hostname = NULL;
     int mp_port;
     char *server_hostn_g, *server_hostname = NULL;
     int server_port;


     int c, getoptflg = 0;
     int sname_flg = 0, snumber_flg = 0, mpname_flg = 0, mpnumber_flg = 0, oprsname_flg = 0;
     int quiet_flg = 0, dname_flg = 0, alone_flg = 0, alone_mp_flg = 0, english_flg = 0, lci_flg = 0;
     int database_flg = 0, pred_func_flg = 0, id_flg = 0, lang_flg =0;
     int database_hash_size, pred_func_hash_size, id_hash_size;

     extern int optind;
     extern char *optarg;
     int maxlength = MAX_HOST_NAME * sizeof(char);

     while ((c = getopt(argc, argv, 
#ifdef CONNECT_TO_IXTET
			"c:d:s:x:X:i:m:j:n:D:P:I:l:L:hpqaA"
#else
			"c:d:s:x:i:m:j:n:D:P:I:l:L:hpqaA"
#endif
	  )) != EOF)
	  switch (c)
	  {
	  case 'x': 
	       add_initial_include_command(optarg);
	       break;
	  case 'c': 
	       add_initial_command(optarg);
	       break;
	  case 'd':
	       dname_flg++;
	       data_path = optarg;
	       break;
#ifdef CONNECT_TO_IXTET
	  case 'X':
	       NEWSTR(optarg,ixtet_name);
	       break;
#endif
	  case 's':
	       sname_flg++;
	       NEWSTR(optarg,server_hostname);
	       break;
	  case 'i':
	       snumber_flg++;
	       if (!sscanf (optarg,"%d",&server_port ))
		    getoptflg++;
	       break;
	  case 'm':
	       mpname_flg++;
	       NEWSTR(optarg, mp_hostname);
	       break;
	  case 'j':
	       mpnumber_flg++;
	       if (!sscanf (optarg,"%d",&mp_port ))
		    getoptflg++;
	       break;
	  case 'n':
	       oprsname_flg++;
	       oprs_name = optarg;
	       break;
	  case 'a':
	       alone_flg++;
	       break;
	  case 'A':
	       alone_mp_flg++;
	       break;
	  case 'p':
	       english_flg++;
	       break;
	  case 'l':
	       lci_flg++;
	       id_case_option = optarg;
	       break;
	  case 'L':
	       lang_flg++;
	       lang_str = optarg;
	       break;
	  case 'q':
	       quiet_flg++;
	       break;
	  case 'D':
	       database_flg++;
	       if (!sscanf (optarg,"%d",&database_hash_size ))
		    getoptflg++;
	       break;
	  case 'P':
	       pred_func_flg++;
	       if (!sscanf (optarg,"%d",&pred_func_hash_size ))
		    getoptflg++;
	       break;
	  case 'I':
	       id_flg++;
	       if (!sscanf (optarg,"%d",&id_hash_size ))
		    getoptflg++;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
 
	  }
     if (getoptflg) {
	  fprintf(stderr, OPRS_ARG_ERR_MESSAGE, argv[0]);
	  socket_cleanup_and_exit(1);
     }

     if ( !oprsname_flg ){
	  if ( argc < (optind + 1)) {
	       fprintf(stderr, LG_STR("ERROR: We need the name of the oprs client as argument.\n",
				      "ERREUR: Le nom du oprs client est un argument obligatoire.\n"));
	       socket_cleanup_and_exit (1);
	  } else
	       oprs_name = argv[optind];
     }

     if (lang_flg) {
	  select_language(lang_str, FALSE);
     }

     if (sname_flg){
	  if (gethostbyname(server_hostname) == NULL){
	       fprintf(stderr, "%s%s", "oprs_init_arg: gethostbyname: ", 
		       LG_STR("Invalid server host name.\n",
			      "Nom de serveur invalide.\n"));
	       socket_cleanup_and_exit (1);
	  }
     } else if (! (server_hostn_g = getenv("OPRS_SERVER_HOST"))) {
	  server_hostname = (char *)MALLOC (maxlength);
	  if (gethostname(server_hostname, MAX_HOST_NAME) != 0) {
	       perror("oprs_init_arg: gethostname");
	       socket_cleanup_and_exit(1);
	  }
     } else {
	  if (gethostbyname(server_hostn_g) == NULL){
	       fprintf(stderr, "%s%s", "oprs_init_arg: gethostbyname: ", 
		       LG_STR("Invalid server host name, check the OPRS_SERVER_HOST environment variable.\n",
			      "Nom de serveur invalide, vérifier la variable d'environement OPRS_SERVER_HOST.\n"));
	       server_hostname = (char *)MALLOC (maxlength);
	       if (gethostname(server_hostname, MAX_HOST_NAME) != 0) {
		    perror("oprs_init_arg: gethostname");
		    socket_cleanup_and_exit(1);
	       }
	  } else
	       NEWSTR(server_hostn_g,server_hostname);
     }
     if (!snumber_flg && !get_int_from_env("OPRS_SERVER_PORT", &server_port))
	  server_port = SERVER_PORT;

     if (mpname_flg){
	  if (gethostbyname(mp_hostname) == NULL){
	       fprintf(stderr, "%s%s", "oprs_init_arg: gethostbyname: ", 
		       LG_STR("Invalid server host name.\n",
			      "Nom de serveur invalide.\n"));
	       socket_cleanup_and_exit (1);
	  }
     } else if (! (mp_hostn_g = getenv("OPRS_MP_HOST"))) {
	  mp_hostname = (char *)MALLOC (maxlength);
	  if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
	       perror("oprs_init_arg: gethostname");
	       socket_cleanup_and_exit(1);
	  }
     } else {
	  if (gethostbyname (mp_hostn_g) == NULL){
	       fprintf(stderr, "%s%s", "oprs_init_arg: gethostbyname: ", 
		       LG_STR("Invalid server host name, check the OPRS_MP_HOST environment variable.\n",
			      "Nom de serveur invalide, vérifier la variable d'environement OPRS_MP_HOST.\n"));
	       mp_hostname = (char *)MALLOC (maxlength);
	       if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
		    perror("oprs_init_arg: gethostname");
		    socket_cleanup_and_exit(1);
	       }
	  }
	  else
	       NEWSTR(mp_hostn_g,mp_hostname);

     }
     if (!mpnumber_flg && !get_int_from_env("OPRS_MP_PORT", &mp_port))
	  mp_port = MP_PORT;
     
     /* Set this option before normalizing the name */
     if ( lci_flg ) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the arguments.\n",
				      "Option de minuscule/majuscule invalide, vérifier les arguments.\n"));
	  }
     } else if ((id_case_option = getenv("OPRS_ID_CASE")) != NULL) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the OPRS_ID_CASE environment variable.\n",
				      "Option de minuscule/majuscule invalide, vérifier la variable d'environement OPRS_ID_CASE.\n"));
	  }
     } 

     if( oprs_name[0] == '|'){  /* The name is an "idbar" */
	  int i, length = strlen (oprs_name) -1;
	  for (i =1 ; i<length ; i++) { /* we check if there is no bar inside the name*/
	       if (oprs_name[i] == '|'){
		    fprintf(stderr, LG_STR("Error in the oprs-name.\n",
					   "Erreur dans le nom du oprs.\n"));
		    socket_cleanup_and_exit(1);
	       }
	  }
	  if (oprs_name[length] != '|'){
	       fprintf(stderr, LG_STR("Error in the oprs-name.\n",
				      "Erreur dans le nom du oprs.\n"));
	       socket_cleanup_and_exit(1);
	  }
     } else
	  NORMALIZE_STRING_IN_PLACE(oprs_name);

     if (dname_flg) {
	  oprs_data_path = data_path;
     }

     if (database_flg)
	  init_hash_size_database(database_hash_size);
     else
	  init_hash_size_database(0);

     if (pred_func_flg)
	  init_hash_size_pred_func(pred_func_hash_size);
     else
	  init_hash_size_pred_func(0);

     if (id_flg)
	  init_hash_size_id(id_hash_size);
     else
	  init_hash_size_id(0);

     if ( alone_flg ) {
	  fprintf(stderr, LG_STR("This kernel will not register to the OPRS-Server.\n",
				 "Ce noyau ne s'enregistre pas au OPRS-Server.\n"));
	  register_to_server = FALSE;
     }

     if ( alone_mp_flg ) {
	  fprintf(stderr, LG_STR("This kernel will not register to the message passer.\n",
				 "Ce noyau ne s'enregistre pas au message passer.\n"));
	  register_to_mp = FALSE;
     }

     if ( english_flg ) {
	  fprintf(stderr, LG_STR("This kernel will print english temporal operators.\n",
				 "Ce noyau imprime les opétateurs temporel en toutes lettres.\n"));
	  print_english_operator = TRUE;
     }

     if ( !quiet_flg && !alone_flg)
	  fprintf(stderr, OPRS_HELP_MESSAGE);

     init_hash_size_type(0);

     *mp_hostn = mp_hostname;
     *mp_p = mp_port;
     *server_hostn = server_hostname;
     *server_p = server_port;

     return(oprs_name);
}
#else
void init_hash_tables()
{
     init_hash_size_database(0);

     init_hash_size_pred_func(0);

     init_hash_size_id(0);

     init_hash_size_type(0);
}
#endif

Oprs *oprs_init_main(char *name, PBoolean use_x_window, char *mp_hostn, int mp_port, char *server_hostn, int server_port)
{
     yy_begin_COMMAND();			/* On demarre le lex parser dans cet etat */

     SRANDOM(getpid());		/* we should use the date or something else... */

     initialize_cst();
     initialize_debug_trace_array();
     initialize_option_array();
     make_id_hash();		/* Make the symbol hash table */
     init_id_hash();

     make_pred_func_hash();
     declare_predicat();
     declare_be_pred();
     declare_eval_pred();

#ifdef OPRS_PROFILING
     init_profile_timer();
#endif

     declare_ev_funct();
     declare_action();

     make_type_hash();		/* Make the type hash table */

     make_global_var_list();

     current_oprs = make_oprs(name);

     /* In case we are connected to a file, we want to be unbuffered
      * Otherwise we have trouble when we fork because the buffer of 
      * the child stdin is full of the father command...
      */
     setbuf(stdin,NULL);	

     i_have_the_stdin= FALSE;

     if (register_to_server) {
#ifdef VXWORKS	  
	  int on = 1;
#endif
	  init_client_socket(name, use_x_window, server_hostn, server_port);
#ifdef VXWORKS	  
	  ioctl(ps_socket,FIONBIO,&on); /* make it non blocking... */
#else
	  fcntl(ps_socket,F_SETFL,O_NONBLOCK);
#endif
     }
     if (register_to_mp) {
#ifdef VXWORKS	  
	  int on = 1;
#endif
	  if (external_register_to_the_mp_host_prot(name, mp_hostn, mp_port, MESSAGES_PT) == -1)
	       exit(1);
	  create_yy_mp();
#ifdef VXWORKS	  
	  ioctl(mp_socket,FIONBIO,&on); /* make it non blocking... */
#else
	  fcntl(mp_socket,F_SETFL,O_NONBLOCK);
#endif

     }

#ifdef CONNECT_TO_IXTET
      if (ixtet_name)
	  time_stamp(&ixtet_start_date);
#endif

     oprs_prompt = name;
     previous_soak = NULL;

#ifdef HAS_TIMER
     install_condition_timer_handler_set_arm();
#endif
     
     print_intro();		/* Welcome to the wonderful world of OPRS */

     return current_oprs;
}

void load_kernel_from_parser(Oprs *oprs, PString file_name)
{
     if (start_load_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: load_kernel_from_parser: failed start_load_session: %s\n",
				 "ERREUR: load_kernel_from_parser: echec start_load_session: %s\n"), file_name);
     } else {
	  Slist *oprs_new_facts;
	  Slist *oprs_locked_messages;
	  Slist *oprs_facts;
	  Slist *oprs_new_goals;
	  Slist *oprs_goals;
	  Slist *oprs_conditions_list;
	  PBoolean graphix;
	  ExprList exprs_to_load;
	  Op_List ops;
	  Expression *expr;
	  Op_Structure *op;
	  Intention_Graph *new_ig;
	  Relevant_Condition *rc;
	  Thread_Intention_Block *new_current_tib;
	  Intention *new_current_intention;

	  Type *ltype, *type;
	  TypeList oprs_type_list;
	  SymList elts;
	  Symbol s;

	  type_conversion = sl_make_slist();

	  reset_oprs_kernel(oprs);

	  oprs_type_list = load_list_type();

	  if (load_char() != DPT_LIST_EXPR)
	       oprs_perror("load_kernel_from_parser:DPT_LIST_EXPR", PE_UNEXPECTED_DUMP_TYPE);
	  exprs_to_load = load_list_expr();

	  if (load_char() != DPT_LIST_OP)
	       oprs_perror("load_kernel_from_parser:DPT_LIST_OP", PE_UNEXPECTED_DUMP_TYPE);

	  ops = load_list_op();

	  if (load_char() != DPT_IG)
 	       oprs_perror("load_kernel_from_parser:DPT_IG", PE_UNEXPECTED_DUMP_TYPE);

	  new_ig = load_ig();

	  oprs_new_facts = load_list_reloc_elt(DPT_FACT);
	  oprs_locked_messages = load_list_reloc_elt(DPT_FACT);
	  oprs_facts = load_list_reloc_elt(DPT_FACT);
	  oprs_new_goals = load_list_reloc_elt(DPT_GOAL);
	  oprs_goals = load_list_reloc_elt(DPT_GOAL);
	  oprs_conditions_list = load_list_reloc_elt(DPT_COND);

	  load_list_reloc_elt(DPT_ENVAR); /* this is enough to reload them... */

	  oprs->posted_meta_fact = load_boolean(); /* discutable... */
	  
	  LOAD_ADDR_AND_REF_LOC(DPT_TIB, oprs->critical_section);
	  LOAD_ADDR_AND_REF_LOC_ADDR(DPT_TIB, (void **)&new_current_tib);
	  LOAD_ADDR_AND_REF_LOC_ADDR(DPT_INTENTION, (void **)&new_current_intention);

	  graphix = load_boolean();

	  load_all_objects();

	  end_load_session();

	  sl_loop_through_slist(oprs_type_list, ltype, Type *) {
	       if (!(type = sl_search_slist(current_oprs->types, ltype->name, eq_stype_type))) {
		    sl_add_to_head(current_oprs->types, ltype); 
		    sl_loop_through_slist(ltype->elts, s, Symbol)
			 set_type_from_reload(ltype,s);
	       }
	  }

	  FREE_SLIST(oprs_type_list);

	  while ((ltype = sl_get_from_head(type_conversion))) {
	       elts = sl_get_from_head(type_conversion);
	       sl_loop_through_slist(elts, s,  Symbol) {
		    set_type(ltype,s);
	       }
	  }
	  
	  FREE_SLIST(type_conversion);

	  sl_loop_through_slist(oprs_conditions_list, rc, Relevant_Condition *) {
	       add_condition_to_expr_internal(rc);
	  }

	  oprs->new_facts = sl_concat_slist(oprs_new_facts,oprs->new_facts);
	  oprs->locked_messages = sl_concat_slist(oprs_locked_messages,
						 oprs->locked_messages);
	  oprs->facts = sl_concat_slist(oprs_facts,
				       oprs->facts);
	  oprs->new_goals = sl_concat_slist(oprs_new_goals,
					   oprs->new_goals);
	  oprs->goals = sl_concat_slist(oprs_goals,
				       oprs->goals);
	  oprs->conditions_list = sl_concat_slist(oprs_conditions_list,
						 oprs->conditions_list);

	  sl_loop_through_slist(exprs_to_load, expr,  Expression *) {
	       conclude(expr,current_oprs->database);
	       free_expr(expr);
	  }

	  FREE_SLIST(exprs_to_load);

	  sl_loop_through_slist(ops, op, Op_Structure *) {
	       add_op_to_relevant_op_internal(op,current_oprs->relevant_op);
	  }
	  FREE_SLIST(ops);

	  oprs->intention_graph = new_ig;
	  current_tib = new_current_tib ;
	  current_intention = new_current_intention;
     
#ifndef NO_GRAPHIX
	  global_int_draw_data->ig = new_ig;

	  if (graphix) {
	       if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
		    global_int_draw_data->reposition_all = TRUE;
		    draw_intention_graph(global_int_draw_data);
	       }
	  } else {		/* Has loaded a text kernel... need some traces... */
	       if (debug_trace[GRAPHIC_INTEND] && global_int_draw_data) {
		    rebuilt_intention_graph_graphic(new_ig);
	       }
	  }
#endif

     }

}

void dump_kernel_from_parser(Oprs *oprs, PString file_name)
{
     if (start_dump_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: dump_kernel_from_parser: failed start_dump_session: %s\n",
				 "ERREUR: dump_kernel_from_parser: echec start_dump_session: %s\n"), file_name);
     } else {
	  ExprList exprs_to_dump;
	  
	  
	  dump_list_type(oprs->types);
	  dump_char(DPT_LIST_EXPR);
	  dump_list_expr(exprs_to_dump = get_all_expr_from_db(oprs->database));
	  dump_char(DPT_LIST_OP);
	  dump_list_op(oprs->relevant_op->op_list);
	  dump_char(DPT_IG);
	  dump_ig(oprs->intention_graph);
	  DUMP_LIST_RELOC_ELT(oprs->new_facts, Fact *, DPT_FACT);
	  DUMP_LIST_RELOC_ELT(oprs->locked_messages, Fact *, DPT_FACT);
	  DUMP_LIST_RELOC_ELT(oprs->facts, Fact *, DPT_FACT);
	  DUMP_LIST_RELOC_ELT(oprs->new_goals, Goal *, DPT_GOAL);
	  DUMP_LIST_RELOC_ELT(oprs->goals, Goal *, DPT_GOAL);
	  DUMP_LIST_RELOC_ELT(oprs->conditions_list, Relevant_Condition *, DPT_COND);

	  DUMP_LIST_RELOC_ELT(global_var_list, Envar *, DPT_ENVAR);

	  dump_boolean(oprs->posted_meta_fact);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB, oprs->critical_section);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB, current_tib);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION, current_intention);
     
#ifdef NO_GRAPHIX
	  dump_boolean(0);		/* no graphic information */
#else
	  dump_boolean(1);		/* Will save graphic information */
#endif
	  dump_object_list();

	  end_dump_session();
	  FREE_SLIST(exprs_to_dump);
     }

}

void set_user_trace(User_Trace ut, PBoolean val)
{
     if (install_user_trace) {
	  user_trace[ut]=val;
     } else {
	  fprintf(stderr, LG_STR("WARNING: set_user_trace: this kernel has no trace user hook.\n\
\tNo need to change these traces.\n",
				 "ATTENTION: set_user_trace: ce noyau n'a pas de fonctions de trace utilisateurs.\n\
\tCa ne sert a rien de changer ces traces.\n"));
     }
}

void sprint_expr_in_gmexpr(Expression *expr)
{
     static Sprinter *sp = NULL;
     
     if (expr == NULL)  
	  return;
     if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
	  Term *term;
	  sl_loop_through_slist(expr->terms, term, Term *) 
	       sprint_expr_in_gmexpr(TERM_EXPR(term));
     } else {
	  if (! sp)
	       sp = make_sprinter(0);
	  else
	       reset_sprinter(sp);

	  sprint_expr(sp, expr);
	  _write_string_to_socket(ps_socket,SPRINTER_STRING(sp));
     }
}


int count_expr_in_gmexpr(Expression *expr)
{
     int i = 0;
     
     if (expr == NULL)  
	  return 0;
     if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
	  Term *term;
	  sl_loop_through_slist(expr->terms, term, Term *) 
	       i += count_expr_in_gmexpr(TERM_EXPR(term));
     } else {
	  i = 1;
     }
     return i;
}


void send_completion_to_server(Relevant_Op *rk)
{
     Op_Structure *op;
     PBoolean previous_pvn = print_var_name;
     int num = 0;
     
     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  num += count_expr_in_gmexpr(op->invocation);
     }

     write_int_to_socket(ps_socket,num);

     print_var_name = TRUE;

     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  sprint_expr_in_gmexpr(op->invocation);
     }

     print_var_name = previous_pvn;
}



void print_help_from_parser(void)
{
     printf("\n\
The following commands are currently available:\n\
\n\
\t add goal|fact\n\
\t\t Add a goal or a fact in the kernel.\n\
\n\
\t conclude expr\n\
\t\t Conclude the expression in the database.\n\
\n\
\t consult gexpr\n\
\t\t Consult the gexpression in the database.\n\
\n\
\t consult relevant op goal|fact\n\
\t\t Return the relevant op(s) for the goal or fact.\n\
\n\
\t consult applicable op goal|fact\n\
\t\t Return the applicable op(s) for the goal or fact.\n\
\n\
\t declare be predicat\n\
\t\t declare the predicat as basic event predicat.\n\
\n\
\t declare cwp predicat\n\
\t\t declare the predicat as closed world predicat (for the current\n\
\t\t OPRS).\n\
\n\
\t declare ff predicate integer\n\
\t\t declare the predicate as functional fact for the number or\n\
\t\t args integer.\n\
\n\
\t declare op predicate predicate\n\
\t\t declare the predicate as op predicate.\n\
\n\
\t declare id\n\
\t\t declare the symbol id, therefore, the OP compiler will not\n\
\t\t complain if it encounters this symbol.\n\
\n\
\t delete expr \n\
\t\t Delete the expression from the fact database.\n\
\n\
\t delete op op-name \n\
\t\t Delete the op op-name from the op database.\n\
\n\
\t delete opf file-name \n\
\t\t Delete from the op database all the ops from op file file-name.\n\
\n\
\t disconnect\n\
\t\t Instruct the connected oprs to leave the stdin and give it back\n\
\t\t to the OPRS Server. The oprs client will return in \"run\" mode.\n\
\n\
\t echo gexpr|gtexpr|gmexpr\n\
\t\t Echo the expression to the screen.\n\
\n\
\t empty fact db\n\
\t\t Clear the database.\n\
\n\
\t empty op db\n\
\t\t Clear the OP database.\n\
\n\
\t help|h|?\n\
\t\t Print this help.\n\
\n\
\t include file_name\n\
\t\t Include and execute all the commands in file_name.\n\
\n\
\t list action\n\
\t\t list all the actions.\n\
\n\
\t list all\n\
\t\t list a bunch of information.\n\
\n\
\t list be\n\
\t\t list the predicate declares as basic event.\n\
\n\
\t list cwp\n\
\t\t list the closed world predicate.\n\
\n\
\t list evaluable function\n\
\t\t list the evaluable functions.\n\
\n\
\t list evaluable predicate\n\
\t\t list the evaluable predicates.\n\
\n\
\t list ff\n\
\t\t list the functional fact predicates.\n\
\n\
\t list function\n\
\t\t list the function.\n\
\n\
\t list op_predicate\n\
\t\t list predicate declare as op_predicate.\n\
\n\
\t list opfs\n\
\t\t list the loaded op files.\n\
\n\
\t list op\n\
\t\t list all the ops loaded.\n\
\n\
\t list predicate\n\
\t\t list all the predicate.\n\
\n\
\t load db file_name\n\
\t\t Load all the facts contained in file_name.\n\
\n\
\t load opf op_graph|file_name\n\
\t\t Compile a list of OPs either from\n\
\t\t stdin or from the file file_name.\n\
\n\
\t q|quit|exit|EOF\n\
\t\t quit the program\n\
\n\
\t reset kernel\n\
\t\t reset the kernel.\n\
\n\
\t save db file_name\n\
\t\t save the database in file_name.\n\
\n\
\t send name message\n\
\t\t send the message to the oprs named \"name\".\n\
\n\
\t set action on|off\n\
\t\t set the compiler flag action on|off.\n\
\n\
\t set eval post on|off\n\
\t\t Turn on or off the current-quote mechanism.\n\
\n\
\t set meta fact op on|off\n\
\t\t set meta fact op on|off.\n\
\n\
\t set meta fact on|off\n\
\t\t set meta fact on|off.\n\
\n\
\t set meta goal op on|off\n\
\t\t set meta goal op on|off.\n\
\n\
\t set meta goal on|off\n\
\t\t set meta goal on|off.\n\
\n\
\t set meta on|off\n\
\t\t Turn on or off the meta_level mechanism. (this will greatly\n\
\t\t increase the performance of the system).\n\
\n\
\t set parallel intend on|off\n\
\t\t set parallel intending on|off.\n\
\n\
\t set parallel intention on|off\n\
\t\t set parallel intention execution on|off.\n\
\n\
\t set parallel post on|off\n\
\t\t set parallel posting on|off.\n\
\n\
\t set oprs_data_path string\n\
\t\t set the OPRS_DATA_PATH.\n\
\n\
\t set soak on|off\n\
\t\t set soak on|off.\n\
\n\
\t set time_stamping on|off\n\
\t\t set time stamping on|off.\n\
\n\
\t show copyright\n\
\t\t Print the copyright notice\n\
\n\
\t show db\n\
\t\t show the database contents.\n\
\n\
\t show intention\n\
\t\t show the intention graph.\n\
\n\
\t show op op_name\n\
\t\t display the op named op_name.\n\
\n\
\t show oprs_data_path\n\
\t\t show OPRS_DATA_PATH value.\n\
\n\
\t show version\n\
\t\t show version.\n\
\n\
\t stat db\n\
\t\t Print Statistics on the database hashtables.\n\
\n\
\t stat id\n\
\t\t Print Statistics on the ID hashtable.\n\
\n\
\t stat all\n\
\t\t Print Statistics on all the hashtables.\n\
\n\
\t trace all on|off\n\
\t\t turn on or off a bunch of trace.\n\
\n\
\t trace applicable op on|off\n\
\t\t trace applicable op on|off.\n\
\n\
\t trace db frame on|off\n\
\t\t trace the frame in database operation.\n\
\n\
\t trace db on|off\n\
\t\t trace database operations.\n\
\n\
\t trace fact on|off\n\
\t\t trace fact posting.\n\
\n\
\t trace goal on|off\n\
\t\t trace goal posting.\n\
\n\
\t trace graphic on|off\n\
\t\t set the graphic trace for op on|off.\n\
\n\
\t trace graphic op op_name on|off\n\
\t\t set the graphic trace for op_name.\n\
\n\
\t trace graphic opf file_name on|off\n\
\t\t set the graphic trace for all the op in opf file_name.\n\
\n\
\t trace intend on|off\n\
\t\t trace intending on|off.\n\
\n\
\t trace load op on|off\n\
\t\t trace op conpilationon|off.\n\
\n\
\t trace receive on|off\n\
\t\t trace message reception on|off.\n\
\n\
\t trace relevant op on|off\n\
\t\t trace relevant op lookup on|off.\n\
\n\
\t trace send on|off\n\
\t\t trace message sending on|off.\n\
\n\
\t trace suc_fail on|off\n\
\t\t trace op success failure on|off.\n\
\n\
\t trace text on|off\n\
\t\t set op text trace on|off.\n\
\n\
\t trace text op op_name\n\
\t\t trace text op op_name.\n\
\n\
\t trace text opf file_name\n\
\t\t trace text opf file_name.\n\
\n\
\t trace thread on|off\n\
\t\t trace threading (split/join) on|off.\n\
\n\
\t undeclare be predicat\n\
\t\t undeclare the predicat as basic event predicat.\n\
\n\
\t unify expr expr \n\
\t\t unify two expressions.\n\
\n\
%s\
\n\
%s", parser_warning, bug_report);
}

void print_intro()
{
     printf(LG_STR("\n\
\t\tOPRS (%s EN).\n\
\n\
\t%s\n\
\n\
Version     : %s\n\
Compiled on : %s\n\
Date        : %s\n\
\n\
 ALL RIGHTS RESERVED\n\
\n\
OPRS comes with ABSOLUTELY NO WARRANTY.\n\
",
		   "\n\
\t\tOPRS (%s FR).\n\
\n\
\t%s\n\
\n\
Version     : %s\n\
Compiled on : %s\n\
Date        : %s\n\
\n\
 ALL RIGHTS RESERVED\n\
\n\
OPRS est fourni sans garantie aucune.\n\
"), package_version, COPYRIGHT_STRING, package_version, host, date);
}
