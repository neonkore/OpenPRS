/*                               -*- Mode: C -*- 
 * mp-pub.h -- Public functions to register to, send to and receive from the MP.
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

#ifdef  __cplusplus
extern "C"  {
#endif


#ifndef INCLUDE_mp_pub
#define INCLUDE_mp_pub


extern int mp_socket;

extern char *mp_name;

typedef enum {MESSAGES_PT, STRINGS_PT} Protocol_Type;

int external_register_to_the_mp_host_prot(const char *name, const char *host_name, int port, Protocol_Type prot);
int external_register_to_the_mp_prot(const char *name, int port, Protocol_Type prot);

PString read_string_from_socket(int s,int *size);
void send_message_string_socket(int socket, Symbol rec, PString message );
void send_message_string(const PString message, Symbol rec);
void broadcast_message_string(const PString message);
void multicast_message_string(PString message, unsigned int nb_rec, Symbol *rec);
#endif

#ifdef __cplusplus
}
#endif
