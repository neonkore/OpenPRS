/*                               -*- Mode: C -*- 
/* tcl.c --- 
 * 
 * Filename: tcl.c
 * Description: 
 * Author: Felix Ingrand <felix@laas.fr>
 * Created: Fri Feb 25 15:43:16 2011 (+0100)
 *
 * Copyright (c) 2011 LAAS/CNRS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "tcl.h"
#include "tcl_f.h"

int tcl_fd;
FILE *tcl_FILE = NULL;

void exec_tcl_command (char *command)
{
  if (tcl_FILE) {
    fputs(command, tcl_FILE);
    fflush(tcl_FILE);
  } else
    fprintf(stderr,"tcl interpreter not started, cannot execute command.\n");
}


void start_tcl_interpreter()
{
     int pid, sp[2];

     if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) < 0) {
	  perror(" start_tcl_inerpreter: socketpair");
	  exit(1);
     }
     fcntl(sp[1], F_SETFD, 1);      /* this is to avoid that the ns will be dup when we fork */
     
     if ((pid = fork()) < 0) {
	  perror("fork");
	  exit(1);
     }

     if (pid) {			/* The father */
	  close(sp[0]);		/* Close one side */
	  fcntl(sp[1], F_SETFL, FNDELAY); /* I am not sure this is required... */
	  tcl_fd = sp[1];
	  tcl_FILE = fdopen(tcl_fd, "w");
     } else {			/* The child */
	  char *argv[2];

	  close(sp[1]);
	  fcntl(sp[0], F_SETFL, FNDELAY);
	  /* dup2(sp[0],1); */
	  dup2(sp[0],0);	/* this plug stdin on the socket */

	  argv[0] = "tclsh";
	  argv[1] = NULL;

	  execvp(argv[0], argv);
	  perror("xoprs: cannot exec tclsh");
	  exit(2);
     }
}
