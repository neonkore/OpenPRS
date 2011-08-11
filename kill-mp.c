/*                               -*- Mode: C -*- 
 * kill-mp.c -- 
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

#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#elif defined(WIN95)
#include "winsock.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>  
#include <netdb.h>
#endif

#include "constant.h"
#include "macro.h"
#include "lang.h"
#include "opaque.h"

#include "mp-pub.h"
#include "mp-register.h"

#include "oprs-util_f.h"
#include "oprs-socket_f.h"
#include "mp-register_f.h"

#define KILL_MP_ARG_ERR_MESSAGE LG_STR("Usage: kill-mp [-h] [-m message-passer-hostname] [-j message-passer-port-number]\n",\
"Utilisation: kill-mp [-h] [-m machine-message-passer] [-j port-message-passer]\n")

#ifdef VXWORKS
int kill_mp_main(int mp_port_arg)
#else
int main(int argc, char **argv, char **envp)
#endif
{
	  Register_Type res;
	  int mp_port;

#ifndef VXWORKS
	  int c, getoptflg = 0;
	  int mpnumber_flg = 0, mpname_flg = 0;
	  char *mp_hostname = NULL;
	  extern char *optarg;
	  extern int optind;
	  int maxlength = MAX_HOST_NAME * sizeof(char);

	socket_startup();

	  while ((c = getopt(argc, argv, "j:m:h")) != EOF)
	  switch (c)
	  {
	  case 'j':
			 mpnumber_flg++;
			 if (!sscanf (optarg,"%d",&mp_port ))
			 getoptflg++;
			 break;
	  case 'm':
			 mpname_flg++;
			 mp_hostname = optarg;
			 break;
	  case 'h':
	  case '?':
	  default:
			 getoptflg++;
 
	  }
	  if (getoptflg || ( argc > optind)) {
	  fprintf(stderr, KILL_MP_ARG_ERR_MESSAGE );
	  socket_cleanup_and_exit(1);
	  }

	  if (mpname_flg){
	  if (!gethostbyname (mp_hostname)){
			 fprintf(stderr, LG_STR("Invalid mp host name.\n",
						"nome de machine mp invalide.\n"));
			 socket_cleanup_and_exit (1);
	  }
	  } else if (! (mp_hostname = getenv("OPRS_MP_HOST"))) {
	  mp_hostname = (char *)malloc (maxlength);
	  if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
			 fprintf(stderr, LG_STR("Error in gethostname.\n",
						"Erreur dans gethostname.\n"));
			 socket_cleanup_and_exit(1);
	  }
	  }

	  if (!mpnumber_flg && !get_int_from_env("OPRS_MP_PORT", &mp_port))
	  mp_port = MP_PORT;

#else
	  char mp_hostname[MAX_HOST_NAME];
	  mp_port = mp_port_arg;

	  if (gethostname(mp_hostname, MAX_HOST_NAME) != 0) {
	  fprintf(stderr, LG_STR("Error in gethostname.\n",
				 "Erreur dans gethostname.\n"));
	  socket_cleanup_and_exit(1);
	  }
#endif

	  if (external_register_to_the_mp_host_prot(KILL_MP_NAME,mp_hostname,mp_port, MESSAGES_PT) < 0) {
	  fprintf(stderr, LG_STR("kill-mp (%s:%d): could not register.\n",
				 "kill-mp (%s:%d): ne peut s'enregistrer, .\n"), mp_hostname, mp_port);
	  } else {
	  write_int_to_socket(mp_socket,
#ifdef HAS_GETUID
				getuid()
#else
				0
#endif
			 );

	  if ((res =(Register_Type)read_int_from_socket(mp_socket)) == REGISTER_OK)
			 fprintf(stderr, LG_STR("kill-mp (%s:%d): killed the mp.\n",
						"kill-mp (%s:%d): a tué le mp.\n"), mp_hostname, mp_port);
	  else if (res == REGISTER_NAME_CONFLICT)
	       fprintf(stderr, LG_STR("kill-mp (%s:%d): not allowed to kill the mp.\n",
				      "kill-mp (%s:%d): non autorisé a tuer le le mp.\n"), mp_hostname, mp_port);
	  else 
	       fprintf(stderr, LG_STR("kill-mp (%s:%d): unexpected response from mp.\n",
				      "kill-mp (%s:%d): réponse inattendu du mp.\n"), mp_hostname, mp_port);
     }

	 socket_cleanup_and_exit(1);
}
