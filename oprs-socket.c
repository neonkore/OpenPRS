
/*                               -*- Mode: C -*- 
 * oprs-socket.c -- 
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
#include "sockLib.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#ifdef WIN95
#include "winsock.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#endif

#include <time.h>
#include <stdlib.h>

#include "macro.h"
#include "lang.h"
#include "constant.h"
#include "version.h"

void no_linger_reuseaddr(int socket)
{
#ifdef SOLARIS
     struct linger ling;

     ling.l_onoff = 0;
     ling.l_linger = 0;

     if (setsockopt(socket, SOL_SOCKET,SO_LINGER, &ling, sizeof(ling)) < 0)
	  perror("mp server: setsockopt");
#else
/* Under Solaris, the REUSEADDR allows more than one bind... */
     int val = TRUE;

     if (setsockopt(socket, SOL_SOCKET,SO_REUSEADDR, (char *)&val , sizeof(val)) < 0)
	  perror("mp server: setsockopt");
#endif
}

void socket_cleanup_and_exit(int status)
{
#ifdef WIN95
     int WSAErr;

     WSAErr = WSACleanup();

     if (WSAErr != 0) {
	  fprintf(stderr, LG_STR("Error in WSACleanup.\n",
				 "Erreur dans la fonctiom système WSACleanup.\n"));
     }
#endif
     exit(status);
}

void socket_startup(void)
{
#ifdef WIN95
     WORD wVersionRequested;
     WSADATA wsad;
     int WSAErr;

     wVersionRequested = MAKEWORD(1,1);
     WSAErr = WSAStartup(wVersionRequested,&wsad);

     if (WSAErr != 0) {
	  fprintf(stderr, LG_STR("Error in WSAStartup.\n",
				 "Erreur dans la fonctiom système WSAStartup.\n"));
	  exit(1);
     }
#endif
}
