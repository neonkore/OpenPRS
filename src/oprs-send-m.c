/*                               -*- Mode: C -*- 
 * oprs-send-m.c -- 
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



#include "constant.h"
#include "oprs-type.h"
#include "oprs-sprint.h"

#include "oprs-sprint_f.h"
#include "oprs-send-m_f.h"
#include "mp-pub.h"
#include "user-trace_f.h"

void oprs_send_message_string(PString message, Symbol rec)
{

     if (debug_trace[SEND_MESSAGE]) {
 	  printf(LG_STR("%s: sending the message: %s to %s.\n",
			"%s: envoie du message: %s à %s.\n"), mp_name, message, rec);
     }

     if (user_trace[USER_TRACE] && user_trace[UT_SEND_MESSAGE]) {
	  Symbol recs[1];
	  
	  recs[0] = rec;
	  user_trace_send_message(message, 1, recs);
     }

     send_message_string(message,rec);
}

void oprs_broadcast_message_string(PString message)
{

     if (debug_trace[SEND_MESSAGE]) {
 	  printf(LG_STR("%s: broadcasting the message: %s.\n",
			"%s: diffusion du message: %s.\n"), mp_name, message);

     }
     if (user_trace[USER_TRACE] && user_trace[UT_SEND_MESSAGE])
	  user_trace_send_message(message, 0, NULL);
     
     broadcast_message_string(message);
}


void oprs_multicast_message_string(PString message, unsigned int nb_recs, Symbol *recs)
{
     if (nb_recs) {
	  if (debug_trace[SEND_MESSAGE]) {
	       int i;

	       printf(LG_STR("%s: multicasting the message: %s to",
			     "%s: envoie multiple du message: %s à"), mp_name, message);

	       for (i=0; i!= nb_recs; i++)
		    printf(" %s", recs[i]);

	       printf(".\n");
	  }
	  if (user_trace[USER_TRACE] && user_trace[UT_SEND_MESSAGE])
	       user_trace_send_message(message, nb_recs, recs);

	  multicast_message_string(message, nb_recs, recs);
     } else {
 	  printf(LG_STR("%s: multicasting the message: %s to nobody.\n",
			"%s: envoie multiple du message: %s à personne.\n"), mp_name, message);
     }
}

void send_message_expr(Expression *f, PString rec)
{
     Sprinter *message = make_sprinter(BUFSIZ);
     PBoolean save_pvv =  print_var_value;

     print_var_value = TRUE;
     sprint_expr(message, f);

     print_var_value = save_pvv;

     oprs_send_message_string(SPRINTER_STRING(message),rec);
     free_sprinter(message);
}

void broadcast_message_expr(Expression *f)
{
     Sprinter *message = make_sprinter(BUFSIZ);
     PBoolean save_pvv =  print_var_value;

     print_var_value = TRUE;
     sprint_expr(message, f);

     print_var_value = save_pvv;

     oprs_broadcast_message_string(SPRINTER_STRING(message));
     free_sprinter(message);
}
