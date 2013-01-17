
/*                               -*- Mode: C -*- 
 * send-message.c -- 
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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



#include <stdlib.h>
#include <netdb.h>

#include "oprs-type.h"
#include "mp-register.h"

#include "send-message_f.h"
#include "mp-register_f.h"

void send_message_string_socket(int socket, Symbol rec, PString message )
{
     uint32_t h_size_name,i,total_size, h_size_mess;
     uint32_t n_size_name, n_size_mess;                  /* for network */
     Message_Type mt = MESSAGE_MT;
     uint32_t n_mt;

     char *buf, *tmp;

     h_size_name = strlen(rec);
     h_size_mess = strlen(message);
     n_size_name = htonl(h_size_name);     
     n_size_mess = htonl(h_size_mess);     
     n_mt = htonl(mt);

     total_size = sizeof(n_mt) + sizeof(n_size_name) + h_size_name + sizeof(n_size_mess) + h_size_mess;

     tmp = buf = (char *)malloc(total_size);
     
     BCOPY((char *)&n_mt,tmp, sizeof(n_mt));
     tmp +=  sizeof(n_mt);

     BCOPY((char *)&n_size_name,tmp, sizeof(n_size_name));
     tmp +=  sizeof(n_size_name);
     BCOPY((char *)rec,tmp, h_size_name);
     tmp +=  h_size_name;

     BCOPY((char *)&n_size_mess,tmp, sizeof(n_size_mess));
     tmp +=  sizeof(n_size_mess);
     BCOPY(message,tmp, h_size_mess);


     if ((i = write_size(socket,buf,total_size)) == -1) {
	  perror("send_message_string: write/send");
     }
	  
     if (i != total_size) {
	  fprintf(stderr,LG_STR("Did not write all the bytes in send_message_string.\n",
				"Did not write all the bytes in send_message_string.\n"));
     }

     free(buf);

}

void send_message_string(PString message, Symbol rec)
{
     send_message_string_socket(mp_socket,rec,message);
}

void broadcast_message_string_socket(int socket, PString message )
{
     uint32_t i,total_size, h_size_mess;
     uint32_t n_size_mess;                  /* for network */
     Message_Type mt = BROADCAST_MT;
     uint32_t n_mt;

     char *buf, *tmp;

     h_size_mess = strlen(message);
     n_size_mess = htonl(h_size_mess);     
     n_mt = htonl(mt);

     total_size = sizeof(n_mt) + sizeof(n_size_mess) + h_size_mess;

     tmp = buf = (char *)malloc(total_size);
     
     BCOPY((char *)&n_mt,tmp, sizeof(n_mt));
     tmp +=  sizeof(n_mt);

     BCOPY((char *)&n_size_mess,tmp, sizeof(n_size_mess));
     tmp +=  sizeof(n_size_mess);
     BCOPY(message,tmp, h_size_mess);

     if ((i = write_size(socket,buf,total_size)) == -1) {
	  perror("broadcast_message_string: write/send");
     }
     
     if (i != total_size) {
	  fprintf(stderr,LG_STR("Did not write all the bytes in broadcast_message_string.\n",
				"Did not write all the bytes in broadcast_message_string.\n"));
     }

     free(buf);

}

void broadcast_message_string(PString message)
{
     broadcast_message_string_socket(mp_socket,message);
}

void multicast_message_string_socket(int socket, unsigned int nb_recs, Symbol *recs, PString message )
{
     uint32_t h_size_name,i,total_size, h_size_mess;
     uint32_t n_size_mess, n_nb_recs;                  /* for network */
     Message_Type mt = MULTICAST_MT;
     uint32_t n_mt;

     char *buf, *tmp;

     if (!nb_recs) return;

     h_size_name = 0;
     for (i=nb_recs; i>0; i--) {
	  h_size_name += strlen(recs[i-1]);
     }
     h_size_mess = strlen(message);
     n_nb_recs = htonl(nb_recs);
     n_size_mess = htonl(h_size_mess);
     n_mt = htonl(mt);

     total_size = sizeof(n_mt) + sizeof(n_nb_recs) + sizeof(uint32_t)*nb_recs + /* The number of int */
	  h_size_name + sizeof(n_size_mess) + h_size_mess;

     tmp = buf = (char *)malloc(total_size);

     BCOPY((char *)&n_mt,tmp, sizeof(n_mt));
     tmp +=  sizeof(n_mt);

     BCOPY((char *)&n_nb_recs,tmp, sizeof(n_nb_recs));
     tmp +=  sizeof(n_nb_recs);

     BCOPY((char *)&n_size_mess,tmp, sizeof(n_size_mess));
     tmp +=  sizeof(n_size_mess);
     BCOPY(message,tmp, h_size_mess);
     tmp +=  h_size_mess;

     for (i=0; i!=nb_recs; i++) {
          uint32_t h_rec_name = strlen(recs[i]);
	  uint32_t n_rec_name = htonl(h_rec_name);

	  BCOPY((char *)&n_rec_name,tmp, sizeof(n_rec_name));
	  tmp +=  sizeof(n_rec_name);
	  BCOPY((char *)recs[i],tmp, h_rec_name);
	  tmp +=  h_rec_name;
     }

     if ((i = write_size(socket,buf,total_size)) == -1) {
	  perror("multicast_message_string: write/send");
     }
	  
     if (i != total_size) {
	  fprintf(stderr,LG_STR("Did not write all the bytes in multicast_message_string.\n",
				"N'ai pu ecrire tous les octets dans multicast_message_string.\n"));
     }

     free(buf);
}

void multicast_message_string(PString message, unsigned int nb_recs, Symbol *recs)
{
     if (nb_recs)
	  multicast_message_string_socket(mp_socket, nb_recs, recs, message);
}
