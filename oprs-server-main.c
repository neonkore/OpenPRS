static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*-
 * oprs-main.c --
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

#if defined(WIN95)
#include "winsock.h"
#else
#include "netdb.h"
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAS_SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif
#include <stdlib.h>

#if defined(HAS_READLINE)
#include <readline/readline.h>
#include <readline/history.h>
#endif     

#ifdef USE_MULTI_THREAD
#include <pthread.h>


pthread_mutex_t new_client = PTHREAD_MUTEX_INITIALIZER;
pthread_t accept_thread;
#endif

#include "slistPack.h"
#include "slistPack_f.h"

#include "constant.h"
#include "macro.h"
#include "version.h"

#include "oprs-type.h"
#include "oprs-client.h"
#include "oprs-server.h"
#include "parser-funct.h"

#include "oprs-type_f.h"
#include "help_f.h"
#include "oprs-util_f.h"
#include "oprs-socket_f.h"
#include "parser-funct_f.h"
#include "mp-register_f.h"
#include "oprs-client_f.h"
#include "oprs-pred-func_f.h"

#include "mp-register_f.h"

#define SERVER_ARG_ERR_MESSAGE LG_STR(\
"Usage: oprs-server [-X] [-n] [-i server-port-number]\n\
\t[-m message-passer-hostname] [-j message-passer-port-number]\n\
\t[-l upper|lower|none ]\n",\
"Utilisation: oprs-server [-X] [-n] [-i numéro-port-server]\n\
\t[-m machine-message-passer] [-j numéro-port-message-passer]\n\
\t[-l upper|lower|none ]\n")

char *progname;
int mp_port = MP_PORT;
int server_port = SERVER_PORT;
char *mp_hostname;
char *server_hostname;
int oprs_server_socket;
PBoolean use_x_window = FALSE;
PBoolean use_thread = TRUE;
PBoolean i_have_the_stdin;
OprsClientList oprslist;

void server_init_arg(int argc,char **argv)
{
     char *id_case_option = NULL;

     int c, getoptflg = 0;
     int  snumber_flg = 0, mpnumber_flg = 0, mpname_flg = 0, lci_flg = 0;
     extern char *optarg;
     int maxlength = MAX_HOST_NAME * sizeof(char);

     while ((c = getopt(argc, argv, "Xi:j:m:l:nh")) != EOF)
	  switch (c)
	  {
	  case 'l':
	       lci_flg++;
	       id_case_option = optarg;
	       break;
	  case 'i':
	       snumber_flg++;
	       if (!sscanf (optarg,"%d",&server_port ))
		    getoptflg++;
	       break;
	  case 'j':
	       mpnumber_flg++;
	       if (!sscanf (optarg,"%d",&mp_port ))
		    getoptflg++;
	       break;
	  case 'X':
	       use_x_window = TRUE;
	       break;
	  case 'n':
	       use_thread = FALSE;
	       break;
	  case 'm':
	       mpname_flg++;
	       mp_hostname = optarg;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
 
	  }
       if (getoptflg) {
	    fprintf(stderr, SERVER_ARG_ERR_MESSAGE );
	    exit(1);
       }

     server_hostname = (char *)MALLOC (maxlength);
     if (gethostname(server_hostname, MAX_HOST_NAME) != 0) {
	  fprintf(stderr, LG_STR("oprs-server: ERROR: gethostname \n",
				 "oprs-server: ERREUR: gethostname \n"));
	  exit(1);
	  }

     if (!snumber_flg && !get_int_from_env("OPRS_SERVER_PORT", &server_port))
	  server_port = SERVER_PORT;

     if (mpname_flg){
	  if (gethostbyname(mp_hostname) == NULL){
	       fprintf(stderr, LG_STR("oprs-server: Invalid mp host name.\n",
				      "oprs-server: Nom de machine mp invalide.\n"));
	       exit (1);
	  }
     } else if (! (mp_hostname = getenv("OPRS_MP_HOST"))) {
	  mp_hostname = (char *)MALLOC (maxlength);
	  if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
	       fprintf(stderr, LG_STR("oprs-server: ERROR: gethostname.\n",
						"oprs-server: ERREUR: gethostname.\n"));
	       exit(1);
	  }
     }

     if (!mpnumber_flg && !get_int_from_env("OPRS_MP_PORT", &mp_port))
	 mp_port = MP_PORT;
     
     if ( lci_flg ) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("oprs-server: Invalid case option, check the arguments .\n",
				      "oprs-server: Option de capitalisation dans les arguments invalide.\n"));
	  }
     } else if ((id_case_option = getenv("OPRS_ID_CASE")) != NULL) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("oprs-server: Invalid case option, check the OPRS_ID_CASE environment variable.\n",
				      "oprs-server: Option de capitalisation dans la variable d'environement OPRS_ID_CASE invalide.\n"));
	  }
     }

}

#if defined(HAS_READLINE)

char *strip_white(char *string)
{
     char *s, *t;
  
     for (s = string; whitespace (*s); s++)
	  ;
    
     if (*s == 0)
	  return (s);

     t = s + strlen (s) - 1;
     while (t > s && whitespace (*t))
	  t--;
     *++t = '\0';

     return s;
}

char *strip_start_white(char *string)
{
     char *s;
  
     for (s = string; whitespace (*s); s++)
	  ;
    
     return (s);
}

char *concat_str1_str2(char *str1, char *str2)
{
     char *res = MALLOC_STD(sizeof(char)*(strlen(str1) + 1 + strlen(str2)));
     
     sprintf(res,"%s%s", str1, str2);
     return res;     
}

char *concat_str1_space_str2(char *str1, char *str2)
{
     char *res = MALLOC_STD(sizeof(char)*(strlen(str1) + 2 + strlen(str2)));
     
     sprintf(res,"%s %s", str1, str2);
     return res;     
}

char *concat_str1_str2_quote(char *str1, char *str2)
{
     char *res = MALLOC_STD(sizeof(char)*(strlen(str1) + 2 + strlen(str2)));
     
     sprintf(res,"%s%s\"", str1, str2);
     return res;     
}

void get_possible_completion_from_oprs_client(Oprs_Client *pcl)
{
     int size;

     char buf[40];
/*      sprintf(buf, "completion %d\n", pcl->gen_comp); */
     sprintf(buf, "completion\n");
     write_size(pcl->socket,buf,strlen(buf));
     
     size = read_int_from_socket(pcl->socket);
     if (size) {
	  int i;
	  int ignore;
	  
	  if (pcl->completion_size) {
	       for (i = 0; i < pcl->completion_size; i++) {
		    free(pcl->completion[i]);
	       }
	       free(pcl->completion);
	  }

	  pcl->completion_size = size;
/* 	  pcl->gen_comp = read_int_from_socket(pcl->socket); */
	  pcl->completion = (char **)malloc(size * sizeof(char *));

	  for (i = 0; i < size; i++) {
	       char *to_free;
	       to_free = read_string_from_socket(pcl->socket, &ignore);
	       pcl->completion[i] = concat_str1_space_str2(pcl->name, to_free);
	       free(to_free);
	  }
     } /* else nothing or nothing new... */
}

void rehash_named_oprs_client(PString oprs_name)
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else {
	  get_possible_completion_from_oprs_client(oprs_cl);
     }

}

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;
     
/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char * rl_gets (char *prompt)
{
     /* If the buffer has already been allocated, return the memory
	to the free pool. */
     if (line_read)
     {
	  free (line_read);
	  line_read = (char *)NULL;
     }
     
     /* Get a line from the user. */
     line_read = readline (prompt);
     
     /* If the line has any text in it, save it on the history. */
     if (line_read && *line_read)
	  add_history (line_read);
     
     return (line_read);
}

typedef enum {CC_NONE, CC_KERNEL, CC_NAME, CC_KERNEL_FACT, CC_FACT, CC_KERNEL_FACT_OR_GOAL, CC_STRING, CC_FILENAME} Completion_Cont;

typedef struct {
     char *name;		/* User printable name of the function. */
     Completion_Cont cont;	/* What is expected then... */
} Command;


Command commands[] = {
     {"accept", CC_NONE},
     {"add ", CC_KERNEL_FACT_OR_GOAL},
     {"connect ", CC_KERNEL},
     {"exit", CC_NONE},
     {"h", CC_NONE},
     {"help", CC_NONE},
     {"include \"", CC_FILENAME},
     {"kill ", CC_KERNEL},
     {"rehash ", CC_KERNEL},
     {"make ", CC_NAME},
     {"make_x ", CC_NAME},
     {"q", CC_NONE},
     {"quit", CC_NONE},
     {"reset kernel ", CC_KERNEL},
     {"reset parser ", CC_KERNEL},
     {"send ", CC_KERNEL_FACT},
     {"broadcast ", CC_FACT},
     {"show copyright", CC_NONE},
     {"show version", CC_NONE},
     {"transmit ", CC_KERNEL},
     {"transmit_all \"", CC_STRING},
     {NULL,CC_NONE},
};

char *oprs_kernel_name_completion(char *text, int state, Completion_Cont cont)
{
     static int list_index, len, i;
     Oprs_Client *oprs_cl;


     if (cont == CC_KERNEL) {
	  if (!state) {
	       list_index = 1;
	       len = strlen (text);
	  }
	  while ((oprs_cl = sl_get_slist_pos(oprslist, list_index))) {
	       list_index++;

	       if (strncasecmp (oprs_cl->name, text, len) == 0) {
		    char *res;

		    NEWSTR_STD(oprs_cl->name,res);
		    return res;
	       }
	  }
	  return NULL;
     } else {/* CC_KERNEL_FACT CC_KERNEL_FACT_OR_GOAL */
	  if (!state) {
	       list_index = 1;
	       i = 0;
	       len = strlen (text);
	  }
	  while ((oprs_cl = sl_get_slist_pos(oprslist, list_index))) {
	       int j;
	       for (j=i ; j < oprs_cl->completion_size ; j++) {
		    if (strncasecmp (oprs_cl->completion[j], text, len) == 0) {
			 char *res;
			 
			 NEWSTR_STD(oprs_cl->completion[j],res);
			 i = j + 1;
			 return res;
		    }
	       }
	       i = 0;
	       list_index++;
	  }
	  return NULL;
     }
     
}


char *oprs_server_completion(char *text_arg, int state)
{
     static int list_index, len;
     static PBoolean command_match;
     static char *name;
     char *text = strip_start_white(text_arg);
     static Command com;
     
     if (!state) {
	  list_index = 0;
	  len = strlen (text);
	  command_match = FALSE;
     }

     /* Return the next name which partially matches from the command list. */
     while (command_match || commands[list_index].name) {
	  char *res;
	  static int len_com;

	  if (!command_match) {
	       com = commands[list_index];
	       name = com.name;
	       len_com = strlen(name);
	       list_index++;
	  }

	  if (!command_match &&
	      (strncmp (name, text, len)) == 0 &&
	      len < len_com) {
		    NEWSTR_STD(name,res);
		    return res;
	  } else if (command_match ||
		     ((len >= len_com) &&
		     ((strncmp (name, text, len_com)) == 0))) {
	       char *to_free;
	       /* Command match completely and white space */
	       switch (com.cont)
	       {
	       case CC_NONE:
	       case CC_FACT:
	       case CC_NAME:
	       case CC_STRING:
		    if (len == len_com) {
			 NEWSTR_STD(name,res);
			 return res;
		    }
		    break;
	       case CC_FILENAME:
		    command_match = TRUE;
		    to_free = rl_filename_completion_function(text+len_com,state);
		    if (to_free) {
			 res = concat_str1_str2(name,to_free);
			 FREE_STD(to_free);
		    } else res = NULL;
		    return res;
	       case CC_KERNEL:
	       case CC_KERNEL_FACT:
	       case CC_KERNEL_FACT_OR_GOAL:
		    command_match = TRUE;
		    to_free = oprs_kernel_name_completion(text+len_com,state,com.cont);
		    if (to_free) {
			 res = concat_str1_str2(name,to_free);
			 FREE_STD(to_free);
		    } else res = NULL;
		    return res;
	       }
	  }
     }
     /* If no names matched, then return NULL. */
     return ((char *)NULL);
      
}

void initialize_rl(void)
{
     rl_basic_word_break_characters = "";
     rl_readline_name = "oprs";
     rl_completion_append_character ='\0';
     rl_completion_entry_function = (rl_compentry_func_t *)oprs_server_completion;
}
#endif

int oprs_server_yyparse(void)
{
     closed_file = FALSE;
     yy_begin_COMMAND();
     return oprs_yyparse();
}

int main(int argc, char **argv, char **envp)
{
     Oprs_Client *oprs_cl;
     Slist *oprslist2;
     PBoolean ignore;
#if defined(HAS_READLINE)
     char hist_filename[FILENAME_MAX];
     char *home;
#endif
     socket_startup();

     disable_slist_compaction();

     progname = argv[0];
     
     server_init_arg( argc, argv);

     yy_begin_COMMAND();		/* On demarre le lex parser dans cet etat */

     SRANDOM(getpid());		/* we should use the date or something else... */

     init_hash_size_id(0);
     init_hash_size_pred_func(0);


     make_id_hash();		/* Make the symbol hash table */
     init_id_hash();

     make_pred_func_hash();
     make_global_var_list();

     oprs_prompt = OPRS_SERVER_MP_NAME; /* Set the prompt */
     i_have_the_stdin = TRUE;

     setvbuf(stdin, NULL, _IONBF, 0);	/* In case we are connected to a file, we want to be unbuffered Otherwise we have
				 * trouble when we fork because the buffer of the child stdin is full of the father
				 * command... */

     external_register_to_the_mp_host_prot(OPRS_SERVER_MP_NAME,mp_hostname,mp_port,MESSAGES_PT);

#ifdef USE_MULTI_THREAD
    if (use_thread) {
	  if (pthread_create(&accept_thread,NULL,init_server_socket_and_accept,NULL)) {
	       perror("oprs-server_main: pthread_thr_create");
	       fprintf(stderr, LG_STR("ERROR: unable to start a new thread.\n",
				      "ERREUR: impossible de démarrer un nouveau thread.\n"));
	       exit(1);
	  }
    } else
#endif    
     init_server_socket();	/* Set up the socket for communication between the father and the childs. */

     oprslist = sl_make_slist();

     print_server_intro();	/* Welcome to the wonderful world of OPRS */

#ifdef USE_MULTI_THREAD
     if (use_thread)
	  printf(LG_STR("This oprs-server will auto accept new clients.\n",
		   "Ce oprs-server accepte les nouveaux clients automatiquement.\n"));
     else
#endif
     printf(LG_STR("This oprs-server does not auto accept new clients.\n",
		   "Ce oprs-server n'accepte pas les nouveaux clients automatiquement.\n"));

#if defined(HAS_READLINE)
     using_history ();
     
     if ((home = getenv("HOME")))
	  sprintf(hist_filename,"%s/.history-oprs",home);
     else
	  sprintf(hist_filename,"./.history-oprs");

     if (read_history(hist_filename) != 0) {
	  if (errno != ENOENT)
	       perror("oprs-server: read_history");
     }

     initialize_rl();

#endif

     while (!quit &&		/* I have not been ask to quit */
	    (feof(stdin) == 0)) { /* The stdin is still here... */
#if defined(HAS_READLINE)
	  char *command;

	  command = rl_gets("oprs-server> ");
	  if (command) {
	       if (*command)
		    yyparse_one_command_string(command);
	       /* free(command);  Do not free, it is done in rl_gets... */
	  } else
	       quit = TRUE;

#else
	  print_oprs_prompt();	/* Prompt and parse baby. */
	  oprs_server_yyparse();
#endif
     }

#if defined(HAS_READLINE)
     if (write_history(hist_filename) != 0) {
	  perror("oprs-server: write_history");
     }

     if (history_truncate_file(hist_filename,100) != 0) {
	  perror("oprs-server: history_truncate_file");
     }
#endif

#ifdef USE_MULTI_THREAD
     if (use_thread) {
#ifdef HAS_PTHREAD_KILL
	  pthread_kill(accept_thread,SIGKILL); /* This will kill the accepting thread. */
#endif
     }
#endif

     oprslist2 = COPY_SLIST(oprslist);

     sl_loop_through_slist(oprslist2, oprs_cl, Oprs_Client *)
	  kill_oprs_client(oprs_cl);

     FREE_SLIST(oprslist2);
/*     shutdown(oprs_server_socket,2); */

     
     CLOSE_SOCK(oprs_server_socket);
     CLOSE_SOCK(mp_socket);
     socket_cleanup_and_exit(0);
}

void user_trace_send_message(PString message, unsigned int nb_recs, PString *recs)
{
}


void print_server_intro(void)
{
     printf(LG_STR("\n\
\t\tOPRS Server (%s.%s.%s %s EN).\n\
\n\
\t%s\n\
\n\
OPRS Server comes with ABSOLUTELY NO WARRANTY.\n\
type \"help\", \"h\" or \"?\" for more information. \n\
type \"show copyright\" for copyright and warranty information.\n\
type \"show version\" for version information.\n\
\n\
a dot at the begining of the line followed by a carriage return resets the command parser.\n\
",
		   "\n\
\t\tOPRS Server (%s.%s.%s %s FR).\n\
\n\
\t%s\n\
\n\
OPRS Server est fourni sans garantie aucune.\n\
tapez \"help\", \"h\" or \"?\" pour plus d'information. \n\
tapez \"show copyright\" pour des informations de droits et de garantie.\n\
tapez \"show version\" pour des informations de version.\n\
\n\
Un point tapé en debut de ligne suivi d'un retour chariot reinitialise le parser de commande.\n\
"), major_version, minor_version, patchlevel, status, COPYRIGHT_STRING);
}


void print_server_help_from_parser()
{
     printf("\n\
The following commands are currently available under the oprs-server:\n\
\n\
\t accept\n\
\t\t Accept a oprs kernel\n\
\n\
\t add name goal|fact\n\
\t\t Send a goal or a fact to the oprs named \"name\". This is how\n\
\t\t you can post new fact or new goal in a oprs client.\n\
\n\
\t connect name\n\
\t\t Connect the standard input to the oprs agent named \"name\".\n\
\t\t This will put this oprs agent in \"command\" mode.\n\
\n\
\t help|h|?\n\
\t\t Print this help.\n\
\n\
\t include file_name\n\
\t\t Include and execute all the commands in file_name.\n\
\n\
\t kill name\n\
\t\t Kill the OPRS client named \"name\".\n\
\n\
\t make_x name \n\
\t\t Create a xoprs agent named \"name\" (in a separate Unix process).\n\
\n\
\t make name \n\
\t\t Create a oprs agent named \"name\" (in a separate Unix process).\n\
\n\
\t q|quit|exit|EOF\n\
\t\t quit the program\n\
\n\
\t reset parser name\n\
\t\t Send a \"reset parser\" to the OPRS client named \"name\".\n\
\n\
\t reset kernel name\n\
\t\t Send a \"reset kernel\" to the OPRS client named \"name\".\n\
\n\
\t send name message\n\
\t\t send the message to the oprs named \"name\".\n\
\n\
\t transmit name string\n\
\t\t Send a command to a OPRS client named \"name\".\n\
\t\t This is how you can send command\n\
\t\t to a OPRS client without connecting it.\n\
\n\
\t transmit_all string\n\
\t\t Send a command to a all OPRS client.\n\
\n\
%s\
\n\
%s", parser_warning, bug_report);
}
