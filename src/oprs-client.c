/*                               -*- Mode: C -*- 
 * oprs-client.c -- 
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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
#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>

#ifdef WIN95
#include <winsock.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#ifdef HAS_SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif

#include <errno.h>
#include <fcntl.h>

#include "slistPack.h"
#include "slistPack_f.h"
#include "shashPack.h"
#include "shashPack_f.h"

#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "oprs-server.h"
#include "oprs-sprint.h"
#include "oprs-client.h"

#include "oprs-client_f.h"
#include "oprs-sprint_f.h"
#include "mp-register_f.h"
#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#ifndef WIN95
#include "socket_f.h"
#endif

#include "oprs-socket_f.h"


Oprs_Client *make_oprs_client(PString name);
Oprs_Client *accept_oprs_client();
void kill_oprs_client(Oprs_Client *oprs_cl);

void init_server_socket()
/* 
 * init_server_socket - This function is called once. It will create the socket on which 
 *                      we will be listening for the client we will create.
 *                      Return void.
 */
{
     int port = server_port;
     struct sockaddr_in oprs_socket_addr;
     int len;

     if ((oprs_server_socket = socket(
					AF_INET,
				     SOCK_STREAM, 0)) < 0) {
	  perror("oprs-server: socket");
	  exit(1);
     }

     fprintf(stderr,LG_STR("oprs-server: oprs-server running on %s, port: %d.\n",
			   "oprs-server: oprs-server s'exécute sur %s, port: %d.\n"), server_hostname, port );

     BZERO(&oprs_socket_addr, sizeof(oprs_socket_addr));
     oprs_socket_addr.sin_addr.s_addr = INADDR_ANY;

     oprs_socket_addr.sin_family = AF_INET;
     oprs_socket_addr.sin_port = htons(port);

     len = sizeof(oprs_socket_addr);
     
     no_linger_reuseaddr(oprs_server_socket);
#ifdef HAVE_FORK
     if (fcntl(oprs_server_socket, F_SETFD, 1) == -1)
	  perror("oprs-server: init_server_socket: fcntl:");      /* this is to avoid that the ns will be dup when we fork */
#endif

     if (bind(oprs_server_socket, (struct sockaddr *)&oprs_socket_addr, len) < 0) {
	  perror("oprs-server: init_server_socket: bind");
	  exit(1);
     }

     if (listen(oprs_server_socket, 30) < 0) {
	  perror("oprs-server: init_server_socket: listen");
	  exit(1);
     }
}

Oprs_Client *make_x_oprs_client(PString name)
{
     PBoolean save_use_x = use_x_window; 
     Oprs_Client *child;

     use_x_window = TRUE; 
     
     child = make_oprs_client(name);

     use_x_window = save_use_x; 
     return(child);
}

Oprs_Client *make_oprs_client(PString name)
/* 
 * make_oprs_client - Create a oprs client with the name "name". Note the fork and the 
 *                   various code executed by the father or the child. 
 *                   Return a pointer to Oprs_Client.
 */
{
#ifdef HAVE_FORK
	  int pid;

     fflush(stdout);		/* Lets flush, otherwise the child will print the same thing for a while. */
     /* Note that fflush(NULL) breaks... (it is supose to flush all FILE *). */
     if ((pid = fork()) < 0) {
	  perror("oprs-server: fork");
	  exit(1);
     }

     if (pid == 0) {		/* The child */

	  char *args[13];
	  char *prog =  (use_x_window ? X_OPRS_PROGNAME: OPRS_PROGNAME);
	  int fd;

	  if ((fd = open("/dev/tty", O_RDWR)) > 0) { /* No controlling tty... we are realy lost now... */
#ifdef HAS_IOCTL_TIOCNOTTY
	       ioctl(fd, TIOCNOTTY, 0);
#endif
	       close(fd);
	  }

	  pid = getpid();	/* Get out of the caller process group (go in bg)... */
	  if (setpgid(0, getpid()) <0) {
	       perror("oprs-server: setpgid");
	  }
	  args[2] = (char *) MALLOC(MAX_PRINTED_INT_SIZE);
	  args[4] = (char *) MALLOC(MAX_PRINTED_INT_SIZE);
	  sprintf (args[2],"%d", server_port);
	  sprintf (args[4],"%d", mp_port);

	  args[0] = prog;
	  args[1] = "-i";
	  args[3] = "-j";
	  args[5] = "-n";
	  args[6] = name;
	  args[7] = "-q";
	  /* We need to set this option ( because of environment variables ) */
	  args[8] = "-l";
	  if (lower_case_id) 
	       args[9] = "lower";
	  else if (no_case_id) 
	       args[9] = "none";
	  else 
	       args[9] = "upper";
	  args[10] = "-m";
	  args[11] = mp_hostname;
	  
	  args[12] = NULL;

	  execvp(prog,args);	/* Lets start the client */
	  perror(prog);		/* If we get here, we have it bad. */
	  exit(1);
	  return(NULL);			/* This is just to please the compiler and avoid a warning... */

     } else {
#ifdef USE_MULTI_THREAD	  
     if (use_thread)
	  return (NULL); 
     else
#endif
	  return( accept_oprs_client());
	  }     
#else /* HAVE_FORK */
	  printf(LG_STR("oprs-server: No fork available under this system.",
			"oprs-server: Pas de fork possible sur ce systeme."));
	  return NULL;
#endif
}

Oprs_Client *accept_oprs_client()
/*
 * accept_oprs_client - create a oprs client with the name "name".
 *                   This "child" could have been executed :
 *                     - by a fork ( command "MAKE" )
 *                     - by the user ( command "ACCEPT")
 *                   Return a pointer to Oprs_Client.
 */
{

     int pid, ns;
     socklen_t fromlen;
     Oprs_Client *oprs_cl;
     int size_message;
     char *message;
     PBoolean flag_x;
     struct sockaddr_in from_addr;

     ns = -1;
     fromlen = sizeof(from_addr);
     while (ns == -1 &&
	    ((ns = accept(oprs_server_socket,  (struct sockaddr *)&from_addr, &fromlen)) == -1)) {
	  if (errno != EINTR) {
	       perror("oprs-server: accept");
	  }
     }
     /* we want the name of the child ( for the "accept" command) */

     _write_string_to_socket(ns,ASK_NAME_MESSAGE); /* to wake up the waiting child */

     if(!( message =  read_string_from_socket(ns,&size_message))) {
	  fprintf(stderr,LG_STR("oprs-server: Could not get the client name.\n",
				"oprs-server: N'a pu lire le nom de ce client.\n"));
	  return NULL;
     }

     oprs_cl = MAKE_OBJECT(Oprs_Client);


     DECLARE_ID(message,oprs_cl->name);
     free(message);		/* message is std malloced. */

     pid = read_int_from_socket(ns);
     flag_x = (PBoolean) read_int_from_socket(ns);

     oprs_cl->pid = pid;
     oprs_cl->use_x = flag_x;

#if defined(HAVE_LIBREADLINE)
     oprs_cl->completion_size = 0;
     oprs_cl->completion = NULL;
#endif

     oprs_cl->socket = ns;
#ifdef HAVE_FORK
	  fcntl(ns, F_SETFD, 1);      /* this is to avoid that the ns will be dup when we fork */
#endif
#ifdef USE_MULTI_THREAD
     if (use_thread) 
	  if (pthread_mutex_lock(&new_client) < 0)
	       perror("oprs-server: accept_oprs_client: mutex_lock");
#endif
     sl_add_to_head(oprslist,oprs_cl);
#ifdef USE_MULTI_THREAD
     if (use_thread) 
	  if (pthread_mutex_unlock(&new_client) < 0)
	       perror("oprs-server: accept_oprs_client: mutex_unlock");
#endif
     return oprs_cl;

}

void info_oprs_client(Oprs_Client *oprs_cl)
{
	  printf(LG_STR("oprs-server: The new client is named: %s ",
			"oprs-server: Le nouveau client est: %s "),oprs_cl->name);
	  printf(LG_STR("its pid is: %d ",
			"son numéro de processus est: %d"),oprs_cl->pid);
     if(oprs_cl->use_x)
	  printf(LG_STR(" and it is running under X.\n",
			" et il a une interface X.\n"));
     else
	  printf(LG_STR(".\n",
			".\n"));

     return;
}

#ifdef USE_MULTI_THREAD
void *init_server_socket_and_accept(void *ignore)
{
     Oprs_Client *oprs_cl;

     init_server_socket();
     while (1) {
	  oprs_cl = accept_oprs_client();
	  if (oprs_cl) {
	       printf(LG_STR("oprs-server: auto accepting a new client (%s).\n",
			     "oprs-server: acceptation automatique d'un nouveau client (%s).\n"), oprs_cl->name);
	       info_oprs_client(oprs_cl);
	  } else
	       printf(LG_STR("oprs-server: failed to auto accept a new client.\n",
			     "oprs-server: echec d'acceptation automatique d'un nouveau client.\n"));
     }
}
#endif

PBoolean equal_oprs_name(PString name, Oprs_Client *pc)
/* 
 * equal_oprs_name - Return true if "pc" point on a oprs which has "name" has name. False otherwise. 
 */
{
     return (name == pc->name);     
}

PBoolean client_responding(Oprs_Client * pc)
{
     int ready;
     fd_set writefds;
     fd_set readfds;
     struct timeval tv;

     tv.tv_sec = 0;
     tv.tv_usec = 0;

     FD_ZERO(&writefds);
     FD_ZERO(&readfds);
     FD_SET(pc->socket, &writefds);
     FD_SET(pc->socket, &readfds);
     ready = select(pc->socket + 1, &readfds, &writefds, NULL, &tv);
     
     if (ready < 0) {
	  if (errno != EINTR) {
	       perror("oprs-server: get_and_send_message: select");
	       return FALSE;
	  } else return client_responding(pc);
     }

     if (ready == 0)
	  return FALSE;
     else if (FD_ISSET(pc->socket, &readfds)) {
	  /* We are not supposed to read anything in this socket... must be dead. */
	  fprintf(stderr,LG_STR("oprs-server: EOF from %s.\n",
				"oprs-server: FDF de %s.\n"), pc->name);
	  
	  return FALSE;
     } else 
	  return TRUE;
}

void add_expr_to_client(Expression *f, PString oprs_name)
{
     Oprs_Client *oprs_cl;
    static Sprinter *sp = NULL;

     if (! sp) sp = make_sprinter(0);
     
     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else {
	  PBoolean save_pvv =  print_var_name; /* In case there are some variables... */
	  	  
	  if (! client_responding(oprs_cl)) {
	       fprintf(stderr,LG_STR("oprs-server: OPRS client %s not responding, killing it.\n",
				     "oprs-server: le OPRS client %s ne répond plus, je le zape.\n"), oprs_name);
	       kill_oprs_client(oprs_cl);
	       return;
	  }

	  print_var_name = TRUE;

	  reset_sprinter(sp);

	  SPRINT(sp, 4, sprintf(f,"add "));
	  sprint_expr(sp,f);
	  SPRINT(sp, 1, sprintf(f,"\n"));

	  print_var_name = save_pvv;
	  
	  write_size(oprs_cl->socket,SPRINTER_STRING(sp), SPRINTER_SIZE(sp));
     }
}

void transmit_to_oprs_client(Oprs_Client *oprs_cl, PString command)
{
     static Sprinter *sp = NULL;

     if (! sp) sp = make_sprinter(0);

     if (! client_responding(oprs_cl)) {
	  fprintf(stderr,LG_STR("oprs-server: OPRS client %s not responding, killing it.\n",
				"oprs-server: le OPRS client %s ne répond plus, je le zape.\n"), oprs_cl->name);
	  kill_oprs_client(oprs_cl);
     } else {

	  printf(LG_STR("oprs-server: sending the command '%s' to client %s.\n",
			"oprs-server: envoie la command '%s' au client %s.\n"), command, oprs_cl->name);

	  reset_sprinter(sp);

	  SPRINT(sp, strlen(command) + 1, sprintf(f,"%s\n", command));
	  
	  write_size(oprs_cl->socket,SPRINTER_STRING(sp), SPRINTER_SIZE(sp));
     }
}

void transmit_to_all_client(PString command)
{
     Oprs_Client *oprs_cl;
     Slist *oprslist2 = COPY_SLIST(oprslist);

     sl_loop_through_slist(oprslist2, oprs_cl,Oprs_Client *) {
	  transmit_to_oprs_client(oprs_cl, command);
     }

     FREE_SLIST(oprslist2);
}

void transmit_to_client(PString oprs_name, PString command)
/* 
 * transmit_to_client - This function transmit the string command to the oprs named 
 *                      oprs_name. If the oprs_name does not exist, an error is reported.
 *                      Return void.
 */
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else
	  transmit_to_oprs_client(oprs_cl, command);
	  
}

void kill_oprs_client(Oprs_Client *pcl)
{
     if (CLOSE_SOCK(pcl->socket) == -1)
	  perror("kill_oprs_client: close");
     sl_delete_slist_node(oprslist,pcl);

#if defined(HAVE_LIBREADLINE)
     if (pcl->completion_size) {
	  int i;
	  for (i = 0; i < pcl->completion_size; i++) {
	       free(pcl->completion[i]);
	  }
	  FREE_STD(pcl->completion);
     }
#endif
     FREE(pcl);
}
	  
void kill_named_oprs_client(PString oprs_name)
/* 
 * kill_named_oprs_client - This function kill a oprs client named oprs_name. 
 *                   If the client does not exist, an error is reported.
 *                   Return void.
 */                   
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else {
	  kill_oprs_client(oprs_cl);
     }

}

void write_size_or_disconnect_client( Oprs_Client *oprs_cl,char *message, int size)
{
     if (write_size(oprs_cl->socket,message, size) != size ) {
	  fprintf(stderr,LG_STR("oprs-server: Could not write all the message %s to the OPRS client %s, killing it.\n",
				"oprs-server: N'ai pu écrire tout le message %s au OPRS client %s, je le zape.\n"), message, oprs_cl->name);
	  kill_oprs_client(oprs_cl);
     }
}
  
void reset_parser_named_oprs_client(PString oprs_name)
/* 
 * reset_parser_named_oprs_client - This function send a reset-parser to a oprs client named oprs_name. 
 *                   If the client does not exist, an error is reported.
 *                   Return void.
 */                   
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else
	  write_size_or_disconnect_client(oprs_cl,")\n.\n", 4);
}

#define RESET_STRING "reset kernel\n"

void reset_kernel_named_oprs_client(PString oprs_name)
/* 
 * reset_kernel_named_oprs_client - This function send a reset-kernel to a oprs client named oprs_name. 
 *                   If the client does not exist, an error is reported.
 *                   Return void.
 */                   
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else {
	  write_size_or_disconnect_client(oprs_cl,RESET_STRING, strlen(RESET_STRING));

     }
}

#define TAKE_STDIN_STRING "take_stdin\n"

void pass_the_stdin(PString oprs_name)
/* 
 * pass_the_stdin - will pass the stdin to a oprs client. This function can olny be called 
 *                  from the oprs server. After sending a "special" command to the client 
 *                  to tell it that it can grab the stdin, it then wait any character on 
 *                  the socket associated to the client. Note that we (the server) will 
 *                  not read on stdin meanwhile (because read block). However, we expect the client 
 *                  to execute "sometime" a leave_the_stdin so we can be connected to it 
 *                  again, which is done by doing nothing (because we did not really 
 *                  disconnect from it.)
 *                  Return void.
 *
 *                  We first verify if the client is running under X, because in such a case
 *                  it is a nonsense to give it the stdin.
 */
{
     Oprs_Client *oprs_cl;

     if ((oprs_cl = (Oprs_Client *)sl_search_slist(oprslist,oprs_name,equal_oprs_name)) == NULL)
	  fprintf(stderr,LG_STR("oprs-server: Unknown OPRS client %s.\n",
				"oprs-server: OPRS client %s inconnu.\n"), oprs_name);
     else if (oprs_cl->use_x)
	  fprintf(stderr,LG_STR("oprs-server: This OPRS client %s is running under X,\n\tit doesn't need to be connected !\n",
				"oprs-server: Le OPRS client %s tourne sous une interface X,\n\til n'a pas besoin d'être connecté!\n"), oprs_name);
     else {
	  write_size_or_disconnect_client(oprs_cl,TAKE_STDIN_STRING, strlen(TAKE_STDIN_STRING));
     }
}
