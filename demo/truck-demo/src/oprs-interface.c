/*                               -*- Mode: C -*- 
 * oprs-interface.c -- 
 * 
 * $Id$
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
#include <ctype.h>

#include "constant.h"
#include "demo.h"
#include "interface.h"
#include "message.h"

#include "opaque-pub.h"
#include "mp-pub.h"

extern XtAppContext app_context;
extern Widget      aloneDButton, mp_connectDButton, oprs_connectDButton;

int mp_port;

void demo_init_arg(int argc,char **argv)
{
     int c, getoptflg = 0;
     int mpname_flg = 0, mpnumber_flg = 0, demoname_flg = 0,  lang_flg =0;
     char *lang_str = NULL;

     struct hostent *check_hostname;
     extern int optind;
     extern char *optarg;
     int maxlength = MAX_HOST_NAME * sizeof(char);

     while ((c = getopt(argc, argv, "m:j:n:th")) != EOF) {
	  switch (c)
	  {
	  case 'm':
	       mpname_flg++;
	       mp_host_name = optarg;
	       break;
	  case 'j':
	       mpnumber_flg++;
	       if (!sscanf (optarg,"%d",&mp_port ))
		    getoptflg++;
	       break;
	  case 'n':
	       demoname_flg++;
	       demo_name = optarg;
	       break;
	  case 't':
	       demo->oprs_time = FALSE;
	       break;
	  case 'L':
	       lang_flg++;
	       lang_str = optarg;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
 
	  }
     }
     if (getoptflg) {
	  fprintf(stderr, DEMO_ARG_MESSAGE );
	  exit(1);
     }

     if (lang_flg) {
	  select_language(lang_str, FALSE);
     }

     if (mpname_flg){
	  if ((check_hostname = gethostbyname (mp_host_name)) == NULL){
	       fprintf(stderr, LG_STR("Invalid mp host name \n",
				      "Invalid mp host name \n"));
	       exit (1);
	  }
     } else {
	  mp_host_name = (char *)malloc (maxlength);
	  if (gethostname(mp_host_name, MAX_HOST_NAME) != 0) {
	       fprintf(stderr, LG_STR("Error in gethostname \n",
				      "Error in gethostname \n"));
	       exit(1);
	  }
     }
     if (!mpnumber_flg)
	  mp_port = MP_PORT;
     
     if ( !demoname_flg ){
	  demo_name = default_demo_name;
     } else {
	  int i, length = strlen (demo_name);

	  for (i = 0; i< length ; i++){
	       demo_name[i] = toupper (demo_name[i]);
	  } 
     }
}

void send_message_to_oprs (char *message)
{
     char trace_message[BUF_SIZE];

     if (!demo->connected) {
	  demo_error (LG_STR("send_message: You are not connected ",
			     "send_message: You are not connected "));
          return;
     }
     send_message_string(message, OPRS_NAME);

     sprintf (trace_message,LG_STR("Send: %s\n",
				   "Send: %s\n"), message);
     oprs_message(trace_message);
     
}

void get_oprs_message (XtPointer client_data,int *fid,XtInputId id)
{
     char trace_message[BUF_SIZE];
     int length;
     char *sender;
     char *message;

     sender = read_string_from_socket(*fid, &length);
     message = read_string_from_socket(*fid, &length);
 
     if ( decode_message (message, sender) != 0){
	  sprintf (trace_message,LG_STR("Received %s from %s \n",
					"Received %s from %s \n"), message, sender);
	  demo_error (trace_message);
     }

     free(sender);
     free(message);
}

void connect_to_mp ()
{
     if ((mp_socket = external_register_to_the_mp_host_prot(demo_name,mp_host_name,mp_port,STRINGS_PT)) == -1) {
	  demo->connected = FALSE;
	  demo_warning (LG_STR("Unable to register to the Message Passer",
			       "Unable to register to the Message Passer"));
     } else {
	  demo_message (LG_STR("You are connected to the Message Passer\n",
			       "You are connected to the Message Passer\n"));
	  demo->connected = TRUE;
#ifdef X11R5
	  XtAppAddInput (app_context,
			 mp_socket,
			 (XtPointer)XtInputReadMask,
			 (XtInputCallbackProc)get_oprs_message,                 /* the read function */
			 NULL);
#else
	  XtAppAddInput (app_context,
			 mp_socket,
			 XtInputReadMask,
			 get_oprs_message,                 /* the read function */
			 NULL);
#endif
     }
}
