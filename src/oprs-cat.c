static const char* const rcsid = "$Id$";
 
/*                               -*- Mode: C -*- 
 * oprs-cat.c -- 
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
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
 
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "macro.h"
#include "lang.h"
#include "constant.h"
#include "opaque.h"
 
typedef struct inf Inf;
 
void get_bytes(void);
void send_bytes(void);
 
struct inf {
     char *info;
     Inf *next;
     int size;
     int written;
};
 
Inf *read_info;
Inf *write_info;
PBoolean info;
 
PBoolean quit = FALSE;
PBoolean log_to_file = FALSE;
char *log_file;
int log_fd;
 
int main(int argc,char **argv)
{
     fd_set readfds, writefds, readfds_save, writefds_save;
     int nfound, read;
     struct timeval tv;
 
     if (setpgid(0, getpid()) <0) {
               perror("setpgid");
          }
 
     if (argc >=2 ) {
          log_file = argv[1];
 
          if ((log_fd = open(log_file,O_WRONLY | O_CREAT | O_TRUNC,0664)) == -1) {
               perror("oprs-cat: open");
          } else {
               fprintf(stderr,"oprs-cat: loging output in \"%s\".\n", log_file);
               log_to_file = TRUE;
          }
     }
 
     read_info =  write_info = MAKE_OBJECT(Inf);
 
     info = (read_info != write_info);
 
     FD_ZERO(&readfds_save);
     FD_ZERO(&writefds_save);
     FD_SET(0,&readfds_save);
     FD_SET(1,&writefds_save);
 
     while (TRUE) {
          
          readfds = readfds_save;
 
          writefds = writefds_save;
 
          if (!info && quit) {
               close(1);
               exit(0);
          }
 
          PROTECT_SYSCALL_FROM_EINTR(nfound, select(2, (!quit ? &readfds : NULL),
							  (info ? &writefds : NULL), NULL, NULL));
	  if (nfound == -1) perror("oprs-cat: select");
	  
          while ((nfound > 0) && info && (FD_ISSET(1,&writefds))) {
 
               send_bytes();
 
               tv.tv_sec = 0;
               tv.tv_usec = 0;
 
               writefds = writefds_save;
               
	       PROTECT_SYSCALL_FROM_EINTR(nfound, select(2, NULL, (info ? &writefds : NULL),
							 NULL, &tv))
		    if (nfound == -1) perror("oprs-cat: select");
	       
          }
 
          read = 0;
          
          while ((read < 128) && (nfound > 0) && !quit && FD_ISSET(0,&readfds)) {
 
               read++;
 
               get_bytes();
 
               tv.tv_sec = 0;
               tv.tv_usec = 0;
 
               readfds = readfds_save;
 
               PROTECT_SYSCALL_FROM_EINTR(nfound,select(2, (!quit ? &readfds : NULL), NULL,
							NULL, &tv));
	       if (nfound == -1)
		    perror("oprs-cat: select");
	       
          }
     }
}
 
 
void get_bytes()
{
     Inf *tmp;
     int res = -1;
     char buf[BUFSIZ];
     char *ptr;
 
     PROTECT_SYSCALL_FROM_EINTR(res,read(0,buf,BUFSIZ));
     if (res == -1) {
       if (errno == EIO) {
	 perror("oprs-cat: get_bytes");
	 exit(1);
       } else
	 exit(0);
     }

     if (res == 0) {
          /* fprintf(stderr,"Read: nothing"); */
          quit = TRUE;
     } else {
          /*      fprintf(stderr,"Read: %c",read_info->info); */
          /* fprintf(stderr,"Mallocing %d\n",res); */
          if (log_to_file)
               if (write(log_fd,buf,res) == -1)
                    perror("oprs-cat: log_file: write");
          ptr = (char *)MALLOC(res);
          BCOPY(buf,ptr,res);
          read_info->info = ptr;
          read_info->size = res;
          read_info->written = 0;
          tmp = MAKE_OBJECT(Inf);
          read_info->next = tmp;
          read_info = tmp;
          read_info->next = NULL;
     }
     
     info = (read_info != write_info);
}
 
void send_bytes()
{
     Inf *tmp = write_info;
     int  res;
 
     if (read_info == write_info) {
          fprintf(stderr,"oprs-cat: problem in the queue.\n");
     }
 
     if ((res = write(1,write_info->info + write_info->written,write_info->size)) == -1) {
          perror("oprs-cat: send_bytes");
          exit(1);
     } else if (res == write_info->size)  {
          write_info = write_info->next;
          FREE(tmp->info);
          FREE(tmp);
          info = (read_info != write_info);
     } else {
          write_info->written += res;
          write_info->size -= res;
     }
}
 
