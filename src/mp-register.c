/*                               -*- Mode: C -*-
 * mp-register.c --
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

#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

#include "macro.h"
#include "constant.h"
#include "lang.h"

#include "mp.h"
#include "mp-register.h"
#include "mp-register_f.h"

/* These are the only two globals of this lib... They are taskVar'ed under VxWorks. */
int mp_socket = -1;
char *mp_name = NULL;

PBoolean start_mp_oprs(const char *mp_hostname, int mp_port);

int external_register_to_the_mp_host_prot(const char *name, const char *host_name, int port, Protocol_Type prot)
{
     Register_Type res;
     struct sockaddr_in socket_addr;
#ifdef VXWORKS
     int host_addr;
#else
     struct hostent *host_entry;            /* host entry */
#endif
     int len, trial = 0;

#ifdef VXWORKS
     if (mp_socket == -1) {	/* Have not been taskVarAdded yet. */
	  if (taskVarAdd(0,&mp_socket) != OK) {
	       perror("register_to_the_mp: taskVarAdd");
	       return(-1);
	  }
	  mp_socket = -2;	/* To indicate it is now taskVarAdded. */
	  if (taskVarAdd(0,(int *)&mp_name) != OK) {
	       perror("register_to_the_mp: taskVarAdd");
	       return(-1);
	  }
     }
#endif

     NEWSTR_STD(name, mp_name);

     do {
	  if (trial++) {	/* We will get here after the first attempt to connect. */
	       if (CLOSE_SOCK(mp_socket) < 0) /* Close the failed socket */
		    perror("unregister_oprs: close/closesocket");
	       if (trial == 2) {
		    start_mp_oprs(host_name, port);
		    
		    sleep(3);
	       } else if (trial <= 10) {
		    sleep(1);
	       } else {
		    perror(LG_STR("register_to_the_mp: 10 attempts failed: connect",
				  "register_to_the_mp: 10 tentatives ont échouées: connect"));
		    return(-1);
	       }
	  }

	  if ((mp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	       perror("server: socket");
	       return(-1);
	  }

	  /* You have to call gethostbyname... each time, as the start_mp_oprs may reset it somehow... */
#ifdef VXWORKS
	  if ((host_addr =  hostGetByName(host_name)) == NULL) {
	       perror("register_to_the_mp: gethostbyname");
	       return(-1);
	  }
#else
	  if ((host_entry = gethostbyname(host_name)) == NULL) {
	       perror("register_to_the_mp: gethostbyname");
	       return(-1);
	  }
#endif     
	  
	  
#ifdef VXWORKS
	  BZERO((char *)&socket_addr, sizeof(socket_addr));
	  BCOPY((char *)&host_addr, (char *)&socket_addr.sin_addr,
		sizeof(host_addr));
#else
	  BZERO(&socket_addr, sizeof(socket_addr));
	  BCOPY(host_entry->h_addr, &socket_addr.sin_addr, host_entry->h_length);
#endif

	  socket_addr.sin_family = AF_INET;
	  socket_addr.sin_port = htons((unsigned short)port);

	  len = sizeof(socket_addr);
	  
     } while (connect(mp_socket, (struct sockaddr *)&socket_addr, len) < 0);

#ifdef HAVE_FORK
	  fcntl(mp_socket, F_SETFD, 1);      /* this is to avoid that the ns will be dup when we fork */
#endif

     fprintf(stderr,LG_STR("Connected to the message passer on host: %s, on port: %d.\n",
			   "Connecté au message passer sur la machine: %s, sur le port: %d.\n"), host_name, port);

     write_int_to_socket(mp_socket,prot);

     _write_string_to_socket(mp_socket,name);
     
     if ((res =(Register_Type)read_int_from_socket(mp_socket)) != REGISTER_OK) {
	  fprintf(stderr, LG_STR("external_register_to_the_mp_host: unable to register to the mp.\n",
				 "external_register_to_the_mp_host: impossible de s'enregistrer auprès du message passer.\n"));
	  if (res == REGISTER_NAME_CONFLICT) 
	       fprintf(stderr, LG_STR("external_register_to_the_mp_host: name %s already used.\n",
				      "external_register_to_the_mp_host: le nom %s est déjà utilisé.\n"), name);
	  
	  return -1;
     }

     return mp_socket;
}

int external_register_to_the_mp_prot(const char *name, int port, Protocol_Type prot)
{
     char host_name[MAX_HOST_NAME];            /* name of the host */

     if (gethostname(host_name, MAX_HOST_NAME) != 0) {
	  perror("register_to_the_mp: gethostname");
	  return(-1);
     }
     
     return (external_register_to_the_mp_host_prot(name,host_name,port,prot));
}

void disconnect_from_the_mp(void)
{
     if (mp_socket > 0) {
	  write_int_to_socket(mp_socket, DISCONNECT_MT);
	  if (CLOSE_SOCK(mp_socket) < 0)
	       perror("unregister_oprs: close/closesocket");
	  mp_socket = -2;
     } else 
	  fprintf(stderr, LG_STR("disconnect_from_the_mp: not connected.\n",
				 "disconnect_from_the_mp: non connecté.\n"));
}

int write_size(int socket, char *buffer, int size)
{				
     int written_so_far = 0;

     while (written_so_far != size) {
	  int i;
	  PROTECT_SYSCALL_FROM_EINTR(i,WRITE_SOCK(socket, buffer + written_so_far, size - written_so_far));
	  if (i == -1) {
			 perror("write_size: write/send");
			 return -1;
	  } else if (!i) {
	       fprintf(stderr, LG_STR("write_size: could not write one byte.\n",
				      "write_size: n'a pu écrire un octet.\n"));
	       return 0;
	  } else {       
	       written_so_far += i;
	       if  (written_so_far != size) {
	       fprintf(stderr, LG_STR("write_size: warning, %d written so far out of %d, retrying.\n", 
				      "write_size: attention, %d ecrit sur %d a ecrire.\n"),
		       written_so_far, size);
	       }
	  }
     }
     return written_so_far;
}

int write_what_you_can(int socket, char *buffer, int size)
{				
     int i;

     PROTECT_SYSCALL_FROM_EINTR(i,WRITE_SOCK(socket, buffer, size));
     if (i == -1) {
	  perror("write_what_you_can: write/send");
	  return -1;
     } else if (!i) {
	  fprintf(stderr, LG_STR("write_what_you_can: could not write one byte.\n",
				 "write_what_you_can: n'a pu écrire un octet.\n"));
	  return 0;
     }
     return i;
}

void _write_string_to_socket(int socket, const char *message)
{
     int i, total_size, h_size_mess;
     char *buf, *tmp;
     uint32_t n_size_mess;                  /* for network */

     h_size_mess = strlen(message);
     n_size_mess = htonl(h_size_mess);     


     total_size = sizeof(n_size_mess) + h_size_mess;

     tmp = buf = (char *)malloc(total_size);

     BCOPY(&n_size_mess,tmp, sizeof(n_size_mess));
     tmp +=  sizeof(n_size_mess);
     BCOPY(message,tmp, h_size_mess);
     
     if ((i = write_size(socket,buf,total_size)) == -1) {
	  perror("write_string_to_socket: write_size");
     } else if (i != total_size) {
	  fprintf(stderr,LG_STR("ERROR: Did not write all the bytes in write_string_to_socket.\n",
				"ERREUR: N'ai pas écrit tous les octets dans la fonction write_string_to_socket.\n"));
     }

     free(buf);

}

/*  read_size will read size bytes from socket and put them in *buffer... Return -1 on error. */
int read_size(int socket, char *buffer, int size)
{				
     int read_so_far = 0;

     while (read_so_far != size) { /* if size == 0 exit right away... */
	  int i;
	  PROTECT_SYSCALL_FROM_EINTR(i,READ_SOCK(socket, buffer + read_so_far, size - read_so_far));
	  if (i == -1) {
			 perror("read_size: read/recv");
			 return -1;
	  } else if (!i) {
#ifdef OPRS_DEBUG
	       fprintf(stderr, LG_STR("read_size: read EOF.\n",
				      "read_size: lu FDF.\n"));
#endif
	       return 0;
	  } else	       
	       read_so_far += i;
     }
     return read_so_far;
}

PString read_string_from_socket(int socket, int *size)
{
     PString res;
     int i;
     uint32_t n_size_mess;                  /* for network */

     if ((i = read_size(socket, (char *)&n_size_mess, sizeof(n_size_mess))) == -1) {
	  perror("read_string_from_socket: read_size");
	  return NULL;
     } else if (!i) {
	  fprintf(stderr, LG_STR("read_string_from_socket: EOF in read_string_from_socket: (size).\n",
				 "read_string_from_socket: FDF dans la fonction read_string_from_socket (size).\n"));
	  return NULL;
     }

     *size = ntohl(n_size_mess);

     res = (PString) malloc(*size + 1);


     if ((i = read_size(socket, res, *size)) == -1) {
	  perror("read_string_from_socket: read_size");
	  free(res);
	  return NULL;
     } else if (!i && *size) {	/* nothing read while has something to read */
	  fprintf(stderr, LG_STR("read_string_from_socket: EOF in read_string_from_socket (message).\n",
				 "read_string_from_socket: FDF dans la fonction read_string_from_socket (message).\n"));
	  return NULL;
     } else {
	  res[*size] = '\0';

	  return res;
     }
}

void write_int_to_socket(int socket ,int h_int)
{
     int i, size;
     char *buf;
     uint32_t n_int;                  /* for network */

     n_int = htonl(h_int);     

     size = sizeof(n_int);
     buf = (char *)malloc(size);

     BCOPY(&n_int,buf, size);
     if ((i = write_size(socket,buf,size)) == -1) {
	  perror("write_int_to_socket: write_size");
     } else if (i != size) {
	  fprintf(stderr,LG_STR("ERROR: write_int_to_socket: did not write all the bytes.\n",
				"ERREUR: write_int_to_socket: n'a pas écrit tous les octets.\n"));
     }
     free(buf);
}

int read_int_from_socket(int socket)
{
     int res;
     int i, size;
     uint32_t n_int;                  /* for network */

     size = sizeof(n_int);
     if ((i = read_size(socket, (char *)&n_int, size)) == -1) {
	  perror("read_int_from_socket: read_size");
	  return 0;
     } else if (!i) {
#ifdef OPRS_DEBUG
	  fprintf(stderr, LG_STR("read_int_from_socket: read EOF.\n",
				 "read_int_from_socket: lu FDF.\n"));
#endif
	  return 0;
     } else {
	  res = ntohl(n_int);
	  
	  return res;
     }
}

#define INT_MAX_PRINTED_SIZE 20	/* 20 should be enough... */

PBoolean start_mp_oprs(const char *mp_hostname, int mp_port)
/* Will start a message passer on the specified host and the specified port.
 * Return TRUE if everything went fine... FALSE if any problem... 
 */
{
#if defined(VXWORKS) || defined(WIN95)
	 fprintf(stderr, LG_STR("Cannot start the message passer (%s:%d) for you under this OS.\n",
			    "Ne peut pas démarrer le message passer (%s:%d) sous cet OS.\n"), mp_hostname,mp_port);
     return FALSE;
#else
     struct hostent *host_entry, *mp_host_entry;            /* host entry */
     char *copy_name;
     char host_name[MAX_HOST_NAME];            /* name of the host */

     if (gethostname(host_name, MAX_HOST_NAME) != 0) {
	  perror("start_mp_oprs: gethostname");
	  return FALSE;
     }

     if ((host_entry = gethostbyname(host_name)) == NULL) {
	  perror("start_mp_oprs: gethostbyname");
	  return FALSE;
     }
     NEWSTR_STD(host_entry->h_name,copy_name);

     if ((mp_host_entry = gethostbyname(mp_hostname)) == NULL) {
	  perror("start_mp_oprs: gethostbyname");
	  return FALSE;
     }
     
     if (strcmp(mp_host_entry->h_name, copy_name) == 0) {			/* Same host... */
	  int pid;

	  free(copy_name);

	  fprintf(stderr, LG_STR("Starting message passer on local host: %s, listening on port: %d.\n",
				 "Lancement du message passer sur la machine locale: %s, écoute sur le port: %d.\n"),mp_hostname, mp_port);

	  fflush(stdout);	/* Lets flush, otherwise the child will print the same thing for a while. */

	  if ((pid = fork()) < 0) {
	       perror("start_mp_oprs: fork");
	       exit(1);
	  }
	  if (pid == 0) {	/* The child */
	       char *prog = MP_OPRS_PROGNAME;
	       char *args[4];

	       args[0] = prog;
	       args[1] = "-j";
	       args[2] = (char *) malloc(INT_MAX_PRINTED_SIZE);
	       sprintf (args[2],"%d", mp_port);
	       args[3] = NULL;
	       pid = getpid();
	       if (setpgid(0, getpid()) <0) { /* will disconnect it from the term... */
		    perror("start_mp_oprs: setpgid");
	       }	
	       execvp(prog, args); /* Lets start the message passer */
	       perror(prog);	/* If we get here, we have it bad. */
	       exit(1);

	  } else {		/* The father */

#ifdef OPRS_DEBUG
	       fprintf(stderr, LG_STR("Message passer has process number: %d\n",
				      "Le message passer a le numéro de processus: %d\n"), pid);
#endif
	       
	       return TRUE;
	  }
     } else {			/* Different host... */
	  char buffer[BUFSIZ];
	  int sys_ret;

	  free(copy_name);
       
	  fprintf(stderr, LG_STR("Starting message passer on remote host: %s, listening on port: %d.\n",
				 "Lance le message passer sur la machine distante: %s, écoute sur le port: %d.\n"),mp_hostname, mp_port);
	  sprintf(buffer,"ssh %s -n mp-oprs -j %d &", mp_hostname, mp_port);
	  sys_ret = system(buffer);
	       
	  return (sys_ret != -1); /* system returns -1 if an error occurs. */
     }
#endif
}

