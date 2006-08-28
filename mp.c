static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*-
 * mp-oprs.c -- Message passer.
 *
 * Copyright (c) 1991-2003 Francois Felix Ingrand.
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
#include "ioLib.h"
#include "selectLib.h"
#include "sockLib.h"
#include "inetLib.h"
#include <stdlib.h>
#include <time.h>
#elif defined(WIN95)
#include "winsock.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include "slistPack.h"
#include "slistPack_f.h"

#include "macro.h"
#include "lang.h"
#include "constant.h"

#include "mp.h"
#include "mp-register.h"

#include "mp-pub.h"

#include "mp-register_f.h"
#include "send-message_f.h"
#include "oprs-socket_f.h"
#include "oprs-util_f.h"

				
#define MP_EXIT_TIME_OUT 5*3600	/* number of seconds before exiting. */

#define MP_OPRS_ARG_ERR_MESSAGE LG_STR(\
"Usage: mp-oprs [-h] [-v] [-x] [-l log_file] [-j] message-passer-port-number\n",\
"Utilisation: mp-oprs [-h] [-v] [-x] [-l fichier_log] [-j] numero-port-message-passer\n")

void free_buffered_message(Buff_Message *buff);
void get_and_buffer_message(Mp_Oprs_Client *mpc_sender);
void send_buffered_message(Mp_Oprs_Client *mpc_recipient);

PBoolean mp_verbose = FALSE; 
PBoolean mp_exclude = FALSE;	/* Exclude older client... */

static PBoolean mp_log = FALSE; 
static char *mp_log_filename = NULL;
static FILE *mp_log_file;

static char *mp_hostname = NULL;
static int mp_port;
static uid_t mp_uid;

static int mp_oprs_socket;
static Slist *mp_sender_list;
static Slist *mp_recipient_list;
static fd_set mp_readfds_save;
static fd_set mp_writefds_save;
static int mp_max_fds;

void no_linger(int socket)
{
     struct linger ling;

     ling.l_onoff = 0;
     ling.l_linger = 0;

     if (setsockopt(socket, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling)) < 0)
	  perror("mp-oprs: no_linger: setsockopt");
     if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, 0, 0) < 0)
	  perror("mp-oprs: no_linger: setsockopt");
}

void init_mp_socket()
/*
 * init_mp_socket - This function is called once. It will create the socket on which
 *                      we will be listening for the client to register themselves.
 *                      Return void.
 */
{
	 int port = mp_port;
	 struct sockaddr_in mp_oprs_socket_addr;
	 int len;

	 if ((mp_oprs_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	  perror("mp-oprs: init_mp_socket: socket");
	  socket_cleanup_and_exit(1);
	 }

	 fprintf(stderr,
		 LG_STR("mp-oprs (%s:%d): Message passer running on %s, port: %d.\n",
			"mp-oprs (%s:%d): Message passer s'exécute sur %s, port: %d.\n"), mp_hostname, port, mp_hostname, port );
	 BZERO(&mp_oprs_socket_addr, sizeof(mp_oprs_socket_addr));
	 /* INADDR_ANY would accept any interface on this host... */
	 mp_oprs_socket_addr.sin_addr.s_addr = INADDR_ANY;

	 mp_oprs_socket_addr.sin_family = AF_INET;
	 mp_oprs_socket_addr.sin_port = htons((unsigned short)port);

	 len = sizeof(mp_oprs_socket_addr);

	 no_linger_reuseaddr(mp_oprs_socket);

	 if (bind(mp_oprs_socket, (struct sockaddr *)&mp_oprs_socket_addr, len) < 0) {
	  perror("mp-oprs: init_mp_socket: bind");
	  socket_cleanup_and_exit(1);
	 }
	 if (listen(mp_oprs_socket, 5) < 0) {
	  perror("mp-oprs: init_mp_socket: listen");
	  socket_cleanup_and_exit(1);
	 }
}

PBoolean mp_client_dead(Mp_Oprs_Client *mp_oprs_cl)
{
     int nfound;
     struct timeval time_out;
     fd_set writefds;
     
     FD_ZERO(&writefds);
     FD_SET(mp_oprs_cl->socket, &writefds);

     time_out.tv_sec = 0;
     time_out.tv_usec = 0;
     
     PROTECT_SYSCALL_FROM_EINTR(nfound, select(mp_oprs_cl->socket + 1, NULL,
						     &writefds, NULL, &time_out));
     if (nfound == -1)
	  return TRUE;	/* An error occured... the socket must be dead... */
     else
	  return FALSE;
}

void unregister_oprs(Mp_Oprs_Client *mp_oprs_cl)
{
     char *mess;
     PBoolean name = TRUE;

     fprintf(stderr, LG_STR("mp-oprs (%s:%d): Disconnecting the client: %s from the message passer.\n",
			    "mp-oprs (%s:%d): Déconnecte le client: %s du message passer.\n"), mp_hostname, mp_port, mp_oprs_cl->name);

     if (CLOSE_SOCK(mp_oprs_cl->socket) < 0)
	  perror("unregister_oprs: close/closesocket");

     FD_CLR(mp_oprs_cl->socket, &mp_readfds_save);
     FD_CLR(mp_oprs_cl->socket, &mp_writefds_save);
     sl_delete_slist_node(mp_recipient_list, mp_oprs_cl);
     sl_delete_slist_node(mp_sender_list, mp_oprs_cl);
     sl_loop_through_slist(mp_oprs_cl->mess_list, mess, char *) {
	  if (!name)		/* the sender name (do not free it), the message */
	       free(mess);
	  name = !name;
     }
     sl_free_slist(mp_oprs_cl->mess_list);
     free(mp_oprs_cl->name);
     free(mp_oprs_cl);
}

void exit_mp(void)
{
	  Mp_Oprs_Client *mpc;

	  sl_loop_through_slist(mp_sender_list, mpc, Mp_Oprs_Client *)
	  unregister_oprs(mpc);
	  sl_free_slist(mp_recipient_list);
	  sl_free_slist(mp_sender_list);

	  free(mp_hostname);
	  if (mp_log) {
	       time_t tt =  time(NULL);

	       fprintf(mp_log_file, LG_STR("mp-oprs (%s:%d): End logging: %s",
					   "mp-oprs (%s:%d): Fin de logue: %s"),
		       mp_hostname, mp_port, (tt != -1 ? ctime(&tt) : "Unknown\n"));
	       
	       fclose(mp_log_file);
	  }
	  shutdown(mp_oprs_socket,2);
#ifdef VXWORKS     
	  free_dynamic_slist_list();
#endif
	  socket_cleanup_and_exit(0);
}

PBoolean equal_mp_oprs_name(PString name, Mp_Oprs_Client *pc)
{
	 return (strcmp(name, pc->name) == 0);
}

void register_oprs(int socket)
{
     char *name;
     Mp_Oprs_Client *mp_oprs_cl;
     Protocol_Type prot;
     int size;
     

     prot = read_int_from_socket(socket);

     name = read_string_from_socket(socket, &size);

     if (!name) {
       fprintf(stderr, LG_STR("mp-oprs (%s:%d): client name == NULL (prot = %d)!\n",
			      "mp-oprs (%s:%d): client name == NULL (prot = %d)!\n"),
		  mp_hostname, mp_port, prot);
       return;
     }

     if ((mp_oprs_cl = sl_search_slist(mp_sender_list, name, equal_mp_oprs_name)) != NULL) {
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): Already has a client named: %s.\n",
				 "mp-oprs (%s:%d): A déjà un client sous le nom de: %s.\n"),
		  mp_hostname, mp_port, name);
	  if (mp_exclude) {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): exclude older client option [-x] is on.\n",
				      "mp-oprs (%s:%d): l'option d'exclusion du plus vieux client [-x] est active.\n"),
		       mp_hostname, mp_port);
	       unregister_oprs(mp_oprs_cl);
	  } else if (mp_client_dead(mp_oprs_cl)){
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): %s seems to be dead.\n",
				      "mp-oprs (%s:%d): %s semble mort.\n"),
		       mp_hostname, mp_port, name);
	       unregister_oprs(mp_oprs_cl);
	  } else {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): Denying registration to %s.\n",
				      "mp-oprs (%s:%d): Refus d'enregistrement à %s.\n"),
		       mp_hostname, mp_port, name);

	       write_int_to_socket(socket, REGISTER_NAME_CONFLICT);
	       shutdown(socket,2);
	       return;
	  }
     }

     write_int_to_socket(socket, REGISTER_OK);

     if (strcmp(name, KILL_MP_NAME) == 0){
	  int kuid;

	  free(name);
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): Kill request, checking identity.\n",
				 "mp-oprs (%s:%d): Requète de destruction, vérification identité.\n"), mp_hostname, mp_port);
	  if ((kuid = read_int_from_socket(socket)) && (mp_uid != kuid)) {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): Denying kill-mp, you are not the user who started this message passer.\n",
				      "mp-oprs (%s:%d): Refus requète de kill-mp, vous n'êtes pas l'utilisateur qui a démarré ce message passer.\n"), mp_hostname, mp_port);
	       write_int_to_socket(socket, REGISTER_NAME_CONFLICT); /* Just to say... you are not allowed. */
	       shutdown(socket,2);
	       return;
	  } else {
	       write_int_to_socket(socket, REGISTER_OK); /* Just to say OK. */
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): Shuting down the message passer socket.\n",
				      "mp-oprs (%s:%d): Déconnexion du socket et arrêt total du message passer.\n"), mp_hostname, mp_port);
	       exit_mp();
	  }
     }
     
     mp_oprs_cl = MAKE_OBJECT_STD(Mp_Oprs_Client);

     mp_oprs_cl->name = name;
     mp_oprs_cl->socket = socket;
     mp_oprs_cl->prot = prot;
     mp_oprs_cl->mess_list = sl_make_slist();
     sl_add_to_head(mp_sender_list, mp_oprs_cl);

     if ((strcmp(mp_oprs_cl->name, OPRS_SERVER_MP_NAME) != 0) && 
	 (strcmp(mp_oprs_cl->name, OP_EDITOR_MP_NAME) != 0)) {
	  sl_add_to_head(mp_recipient_list, mp_oprs_cl);
     }
     
     fprintf(stderr, LG_STR("mp-oprs (%s:%d): Registering the client: %s with protocol: %s.\n",
			    "mp-oprs (%s:%d): Enregistrement du client: %s avec le protocole: %s.\n"), mp_hostname, mp_port, mp_oprs_cl->name, 
	     (mp_oprs_cl->prot == MESSAGES_PT ? "MESSAGES_PT" : "STRINGS_PT"));

     FD_SET(socket, &mp_readfds_save);
     if ((socket + 1) > mp_max_fds)
	  mp_max_fds = socket + 1;
}

#ifndef VXWORKS
void init_arg(int argc,char **argv)
{

     int c, getoptflg = 0;
     int mpnumber_flg = 0;
	 int mp_log_flg = 0;
     char *mp_log_file_arg = NULL;

     extern int optind;
     extern char *optarg;

     while ((c = getopt(argc, argv, "l:j:xvh")) != EOF)
	  switch (c)
	  {
	  case 'l':
	       mp_log_flg++;
	       mp_log_file_arg = optarg;
	       break;
	  case 'j':
	       mpnumber_flg++;
	       if (!sscanf (optarg,"%d",&mp_port ))
		    getoptflg++;
	       break;
	  case 'v':
	       mp_verbose = TRUE;
	       break;
	  case 'x':
	       mp_exclude = TRUE;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
 
	  }
       if (getoptflg) {
	    fprintf(stderr, MP_OPRS_ARG_ERR_MESSAGE);
	    socket_cleanup_and_exit(1);
       }

     if (!mpnumber_flg && !get_int_from_env("OPRS_MP_PORT", &mp_port)) {
	  mp_port = MP_PORT;
	  if ( argc >= (optind + 1))
	       if (!sscanf (argv[optind],"%d",&mp_port))
		    fprintf(stderr, LG_STR("mp_oprs: Unparsable mp_port argument.\n",
					   "mp_oprs: Impossible de parser l'argument mp_port.\n"));
     }

	 if (mp_log_flg){
	  NEWSTR(mp_log_file_arg,mp_log_filename);
	  }
}
#endif


#ifdef VXWORKS
int mp_oprs_main(int mp_port_arg, int verbose_arg, char *mp_log_filename_arg, int exclude_arg)
#else
int main(int argc, char **argv, char **envp)
#endif
{
     fd_set readfds;
     fd_set writefds; 
     int nfound, ns;
     socklen_t fromlen;
     PBoolean ignore;

     struct sockaddr_in from_addr;

     Slist *tmp_list;
     Mp_Oprs_Client *mp_oprs_cl;
     struct timeval time_out;

#ifndef VXWORKS
     disable_slist_compaction();
#endif

#if SETBUF_UNDEFINED
     setbuffer(stdout,NULL,0);
#else
     setbuf(stdout,NULL);
#endif

     socket_startup();

     mp_hostname = (char *)malloc(MAX_HOST_NAME * sizeof(char));
     if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
	  fprintf(stderr, LG_STR("Error in gethostname.\n",
				 "Erreur dans la fonctiom système gethostname.\n"));
	  socket_cleanup_and_exit(1);
     }

#ifdef VXWORKS
     mp_port = mp_port_arg;
     mp_verbose = verbose_arg;
     mp_exclude = exclude_arg;
     if (mp_log_filename_arg && strcmp(mp_log_filename_arg,""))	/* not NULL not empty string. */
	  mp_log_filename = mp_log_filename_arg;
     else
	  mp_log_filename = NULL;
#else
     init_arg(argc,argv);
#endif

#ifdef HAS_GETUID
     mp_uid = getuid();
#else
     mp_uid = 0;
#endif

     init_mp_socket();

     mp_recipient_list = sl_make_slist();
     mp_sender_list = sl_make_slist();

     if (mp_log_filename) {
	  if ((mp_log_file = (FILE *) fopen(mp_log_filename, "w")) == NULL)
	       fprintf(stderr, "mp-oprs (%s:%d): register_oprs: fopen(%s, w): NULL.\n",
		       mp_hostname, mp_port, mp_log_filename);
	  else {
	       time_t tt =  time(NULL);
	       mp_log = TRUE;
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): Logging output in file '%s'.\n",
				      "mp-oprs (%s:%d): Logue les traces/sorties dans le fichier '%s'.\n"),
		       mp_hostname, mp_port, mp_log_filename);
	       fprintf(mp_log_file, LG_STR("mp-oprs (%s:%d): Start logging: %s",
					   "mp-oprs (%s:%d): Debut de logue: %s"),
		       mp_hostname, mp_port, (tt != -1 ? ctime(&tt) : "Unknown\n"));
	  }
     }

     FD_ZERO(&mp_readfds_save);
     FD_ZERO(&mp_writefds_save);
     FD_SET(mp_oprs_socket, &mp_readfds_save);
     fromlen = sizeof(from_addr);
     mp_max_fds = mp_oprs_socket + 1;

     while (TRUE) {

	  readfds = mp_readfds_save;
	  writefds = mp_writefds_save;
	  time_out.tv_sec = MP_EXIT_TIME_OUT;
	  time_out.tv_usec = 0;

	  PROTECT_SYSCALL_FROM_EINTR(nfound, select(mp_max_fds, &readfds, &writefds, NULL,
							 &time_out));
	  if (nfound == -1) {
	      perror("mp-oprs: main:select");
	      socket_cleanup_and_exit(1);
	 } else if (nfound == 0)	/* Timed out */
	       if (sl_slist_empty(mp_sender_list)) { /* Nobody connected... */
		    fprintf(stderr, LG_STR("mp-oprs (%s:%d): Nobody registered for more than %d seconds,\n\
mp-oprs (%s:%d): exit.\n",
					   "mp-oprs (%s:%d): Personne d'enregistré depuis plus de %d secondes,\n\
mp-oprs (%s:%d): quit.\n"),
			    mp_hostname, mp_port, MP_EXIT_TIME_OUT, mp_hostname, mp_port);
		    exit_mp();
	       } else
		    continue;
	  else {
	       if (FD_ISSET(mp_oprs_socket, &readfds)) {
		    if ((ns = accept(mp_oprs_socket,  (struct sockaddr *)&from_addr, &fromlen)) < 0) {
			 perror("mp-oprs: accept");
			 socket_cleanup_and_exit(1);
		    } else
			 register_oprs(ns);
	       }
	       
	       tmp_list = COPY_SLIST(mp_sender_list);	/* I copy it because I will have a loop_through in it */
	       sl_loop_through_slist(tmp_list, mp_oprs_cl, Mp_Oprs_Client *) {
		    if (FD_ISSET(mp_oprs_cl->socket, &readfds)) {
			 get_and_buffer_message(mp_oprs_cl);
		    }
		    if (FD_ISSET(mp_oprs_cl->socket, &writefds)) {
			 send_buffered_message(mp_oprs_cl);
		    }
	       }
	       FREE_SLIST(tmp_list);
	  }
     }
}


void send_strings_message_string_socket(int socket, PString rec, PString message )
{
     int h_size_name,i,total_size, h_size_mess;
     u_long n_size_name, n_size_mess;                  /* for network */

     char *buf, *tmp;

     h_size_name = strlen(rec);
     h_size_mess = strlen(message);
     n_size_name = htonl(h_size_name);     
     n_size_mess = htonl(h_size_mess);     

     total_size = sizeof(n_size_name) + h_size_name + sizeof(n_size_mess) + h_size_mess;

     tmp = buf = (char *)malloc(total_size);

     BCOPY((char *)&n_size_name,tmp, sizeof(n_size_name));
     tmp +=  sizeof(n_size_name);
     BCOPY((char *)rec,tmp, h_size_name);
     tmp +=  h_size_name;

     BCOPY((char *)&n_size_mess,tmp, sizeof(n_size_mess));
     tmp +=  sizeof(n_size_mess);
     BCOPY(message,tmp, h_size_mess);

     if ((i = write_size(socket,buf,total_size)) == -1) {
	  perror("send_strings_message_string: write/send");
     }
     
     if (i != total_size) {
	  fprintf(stderr,LG_STR("mp-oprs (%s:%d): Did not write all the bytes in send_strings_message_string.\n",
				"mp-oprs (%s:%d): N'ai pas pu écrire tous les octets dans la fonction send_strings_message_string.\n"), mp_hostname, mp_port);
     }
     free(buf);
}

void send_buffered_message(Mp_Oprs_Client *mpc_recipient)
{
     Buff_Message *buff;

     if (sl_slist_empty(mpc_recipient->mess_list)) {
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): empty messages buffered %s.\n",
				 "mp-oprs (%s:%d): buffer de messages vide %s.\n"), mp_hostname, mp_port, mpc_recipient->name);
	  FD_CLR(mpc_recipient->socket, &mp_writefds_save);
     } else {
	  Slist *mess_copy = sl_copy_slist(mpc_recipient->mess_list);

	  sl_loop_through_slist(mess_copy, buff, Buff_Message *) {
	       fd_set writefds;
	       struct timeval tv;
	       int ready, wwyc;

	       tv.tv_sec = 0;
	       tv.tv_usec = 0;

	       FD_ZERO(&writefds);
	       FD_SET(mpc_recipient->socket, &writefds);
	       PROTECT_SYSCALL_FROM_EINTR(ready, select(mpc_recipient->socket + 1, NULL,
							&writefds, NULL, &tv));

	       if (ready == -1) {
		    perror("get_and_send_message: select");
		    break;
	       }

	       if ( !ready) break;

	       wwyc = write_what_you_can(mpc_recipient->socket,
					 buff->message + buff->written_so_far,
					 buff->size - buff->written_so_far);
	       
	       if (!wwyc) {
		    fprintf(stderr, LG_STR("mp-oprs (%s:%d): EOF in send_buffered_message to %s.\n",
					   "mp-oprs (%s:%d): FDF dans la fonction send_buffered_message) a %s.\n"), mp_hostname, mp_port, mpc_recipient->name);
		    unregister_oprs(mpc_recipient);
		    return;
	       }

	       buff->written_so_far += wwyc;

	       if (buff->written_so_far == buff->size) 
		    free_buffered_message(sl_get_from_head(mpc_recipient->mess_list));
	       else 
		    break;	     
	  }

	  sl_free_slist(mess_copy);
	  if (sl_slist_empty(mpc_recipient->mess_list))
	       FD_CLR(mpc_recipient->socket, &mp_writefds_save);
     }
}

void add_buffered_message(Mp_Oprs_Client *rec, Mp_Oprs_Client *sender, PString sender_name, PString message)
{
     Buff_Message *buff = MAKE_OBJECT_STD(Buff_Message);
     char *buf;
     int size;
     char  error[100];

     buff->sender = sender;
     buff->sender_name = sender_name;
     buff->written_so_far = 0;

     FD_SET(rec->socket, &mp_writefds_save);
     if ((rec->socket + 1) > mp_max_fds)
	  mp_max_fds = rec->socket + 1;

     if (message[0] == '\0') {
	  if (sender) { /* reporting a disconnected or unknown client */ 
	       sprintf(error,"(disconnected %s)\n", sender_name);
	       message = error;
	  } else {
	       sprintf(error, "(unknown %s)\n", sender_name);
	       message = error;
	  }
	  sender_name = "mp-oprs";
     }

     if (rec->prot == MESSAGES_PT) {
	  size = strlen(sender_name) + strlen(message) + 40;
	  buf = (char *)malloc(size);

	  size = sprintf(buf, "receive %s %s\n", sender_name, message);
	  
     } else {
	  int h_size_name, h_size_mess;
	  u_long n_size_name, n_size_mess;                  /* for network */
	  char *tmp;

	  h_size_name = strlen(sender_name);
	  h_size_mess = strlen(message);
	  n_size_name = htonl(h_size_name);     
	  n_size_mess = htonl(h_size_mess);     

	  size = sizeof(n_size_name) + h_size_name + sizeof(n_size_mess) + h_size_mess;

	  buf = (char *)malloc(size);
	  
	  tmp = buf;
	  BCOPY((char *)&n_size_name,tmp, sizeof(n_size_name));
	  tmp +=  sizeof(n_size_name);
	  BCOPY(sender_name,tmp, h_size_name);
	  tmp +=  h_size_name;

	  BCOPY((char *)&n_size_mess,tmp, sizeof(n_size_mess));
	  tmp +=  sizeof(n_size_mess);
	  BCOPY(message,tmp, h_size_mess);
     }
     buff->message = buf;
     buff->size = size;

     sl_add_to_tail(rec->mess_list,buff);
}

void free_buffered_message(Buff_Message *buff)
{
     free(buff->message);
     free(buff);
}

void get_and_buffer_message(Mp_Oprs_Client *mpc_sender)
{
     char *recipient = NULL;
     char *message;
     Message_Type mess_type;

     Mp_Oprs_Client *mpc_recipient;
     int nb_rec = 0;
     int s;
     int size_rec, size_mes;

     s = mpc_sender->socket;

     if ((mess_type = read_int_from_socket(s)) == 0) {
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): EOF in get_and_send_message (message_type) from %s.\n",
				 "mp-oprs (%s:%d): FDF dans la fonction get_and_send_message (message_type) du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
	  unregister_oprs(mpc_sender);
	  return;
     }

     if (mess_type == DISCONNECT_MT) {
	  unregister_oprs(mpc_sender);
	  return;
     }	  

     if ((mess_type != MESSAGE_MT) && (mess_type != BROADCAST_MT)
	  && (mess_type != MULTICAST_MT)) {
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): Unknown message type in get_and_send_message from %s.\n",
				 "mp-oprs (%s:%d): Message de type inconnu dans la fonction get_and_send_message du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
	  unregister_oprs(mpc_sender);
	  return;
     }

     if (mess_type == MESSAGE_MT) {
	  if ((recipient = read_string_from_socket(s, &size_rec)) == NULL) {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): EOF in get_and_send_message (recipient) from %s.\n",
				      "mp-oprs (%s:%d): FDF dans la fonction get_and_send_message (recipient) du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
	       unregister_oprs(mpc_sender);
	       return;
	  }
     }

     if (mess_type == MULTICAST_MT) {
	  if ((nb_rec = read_int_from_socket(s)) == 0) {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): EOF in get_and_send_message (multicast_nb) from %s.\n",
				      "mp-oprs (%s:%d): FDF dans la fonction get_and_send_message (multicast_nb) du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
	       unregister_oprs(mpc_sender);
	       return;
	  }
     }

     if ((message = read_string_from_socket(s, &size_mes)) == NULL) {
	  fprintf(stderr, LG_STR("mp-oprs (%s:%d): EOF in get_and_send_message, (message) from %s.\n",
				 "mp-oprs (%s:%d): FDF dans la fonction get_and_send_message, (message) du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
	  unregister_oprs(mpc_sender);
	  return;
	  }

     if (mess_type == MULTICAST_MT) {
	  int i;

	  if (mp_verbose) {
	       printf(LG_STR("mp-oprs (%s:%d): %s multicast to",
			     "mp-oprs (%s:%d): %s multicast à"), mp_hostname, mp_port,
		      mpc_sender->name);
	  }

	  if (mp_log) {
	       fprintf(mp_log_file, LG_STR("%s multicast to",
					   "%s multicast à"),
		       mpc_sender->name);
	  }

	  for (i=nb_rec; i>0 ; i--) {
	       if ((recipient = read_string_from_socket(s, &size_rec)) == NULL) {
		    fprintf(stderr, LG_STR("\nmp-oprs (%s:%d): EOF in get_and_send_message (recipient multicast) from %s.\n",
					   "\nmp-oprs (%s:%d): FDF dans la fonction get_and_send_message (recipient multicast) du client %s.\n"), mp_hostname, mp_port, mpc_sender->name);
		    unregister_oprs(mpc_sender);
		    return;
	       }

	       if ((mpc_recipient = (Mp_Oprs_Client *) sl_search_slist(mp_recipient_list, recipient, equal_mp_oprs_name)) == NULL) {
		    fprintf(stderr, LG_STR("\nmp-oprs (%s:%d): %s to %s ***WARNING: no multicast recipient***: %s.\n",
					   "\nmp-oprs (%s:%d): %s à %s ***ATTENTION: pas de destinataire multicast***: %s.\n"), mp_hostname, mp_port, mpc_sender->name, recipient, message);
		    add_buffered_message(mpc_sender, NULL, recipient, "");
	       } else  {
		    add_buffered_message(mpc_recipient, mpc_sender,mpc_sender->name, message);
		    if (mp_verbose) {
			 printf(" %s", mpc_recipient->name);
			 fflush(stdout);
		    }
		    
		    if (mp_log) {
			 fprintf(mp_log_file, " %s", mpc_recipient->name);
			 fflush(mp_log_file);
		    }
	       }
	       free(recipient);	       
	  }
	  if (mp_verbose) {
	       printf(": %s.\n", message);
	  }

	  if (mp_log) {
	       fprintf(mp_log_file, ": %s.\n", message);
	  }

     } else if (mess_type == MESSAGE_MT) {
	  if ((mpc_recipient = (Mp_Oprs_Client *) sl_search_slist(mp_recipient_list,
								     recipient,
								     equal_mp_oprs_name)) == NULL) {
	       fprintf(stderr, LG_STR("mp-oprs (%s:%d): %s to %s ***WARNING: no recipient***: %s.\n",
				      "mp-oprs (%s:%d): %s à %s ***ATTENTION: pas de destinataire***: %s.\n"),
		       mp_hostname, mp_port, mpc_sender->name, recipient, message);
	       add_buffered_message(mpc_sender, NULL, recipient, "");
	  } else  {
	       add_buffered_message(mpc_recipient, mpc_sender,mpc_sender->name, message);
	       if (mp_verbose) {
		    printf(LG_STR("mp-oprs (%s:%d): %s to %s: %s.\n",
				  "mp-oprs (%s:%d): %s à %s: %s.\n"), mp_hostname, mp_port,
			   mpc_sender->name, mpc_recipient->name, message);
		    fflush(stdout);
	       }

	       if (mp_log) {
		    fprintf(mp_log_file, LG_STR("%s to %s: %s.\n",
						"%s à %s: %s.\n"),
			    mpc_sender->name, mpc_recipient->name, message);
		    fflush(mp_log_file);
	       }
	  }
     } else {			/* BROADCAST_MT */
	  sl_loop_through_slist(mp_recipient_list, mpc_recipient, Mp_Oprs_Client *)
	       if (mpc_recipient != mpc_sender)
		    add_buffered_message(mpc_recipient, mpc_sender, mpc_sender->name, message);
	  if (mp_verbose) {
	       printf(LG_STR("mp-oprs (%s:%d): %s broadcast: %s.\n",
			     "mp-oprs (%s:%d): %s diffuse: %s.\n"), mp_hostname, mp_port, mpc_sender->name, message);
	       fflush(stdout);
	  }
	  if (mp_log) {
	       fprintf(mp_log_file, LG_STR("%s broadcast: %s.\n",
					   "%s diffuse: %s.\n"), mpc_sender->name, message);
	       fflush(mp_log_file);
	  }
     }

     if (mess_type == MESSAGE_MT) free(recipient);
     free(message);
}
