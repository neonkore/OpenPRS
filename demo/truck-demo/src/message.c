/*                               -*- Mode: C -*- 
 * message.c -- 
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
/*********************************************************************
 *
 * Functions for decoding messages from C_Oprs to  truck demo
 *
**********************************************************************/

#include "constant.h"
#include "demo.h"
#include "message.h"

#include "demo_f.h"

int decode_message (char *message, char *sender)
{
     char trace_message[BUF_SIZE];
     char tmp_str[BUF_SIZE], *pt_tmp_str;
     char *str1, *str2;
     int int_value;
     
        pt_tmp_str = tmp_str;

     if ( strcmp (sender, OPRS_NAME ) != 0 ) {
	  demo_error (LG_STR("decode_message : message from unknown sender ",
			     "decode_message : message from unknown sender "));
	  return(1);
     }
     
     str1 = message;

     str2 = parse_open (str1);
     if (str2 == str1) 	  return(1);	
     str1 = str2;

     str2 = parse_word(str1, &pt_tmp_str);
     if (str2 == str1) 	  return(1);	
     str1 = str2;

     if ( strcmp (tmp_str, SHUTDOWN_COMMAND ) == 0 ) { /* shutdown by oprs */
	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  
	  emergency_stop();
     
	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);

     } else if ( strcmp (tmp_str, TIME_COMMAND ) == 0 ) { /* get oprs-time */
	  str2 = parse_space (str1);
	  if (str2 == str1) return(1);	
	  str1 = str2;

	  str2 = parse_int(str1, &int_value);
	  if (str2 == str1) return(1);
	  str1 = str2;

	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  set_time_from_oprs(int_value);
	       
     } else if ( strcmp (tmp_str, ACCEPT_COMMAND ) == 0 ) { /* accept by oprs */
	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);

	  accepted_by_oprs();

     } else if ( strcmp (tmp_str, REFUSED_COMMAND ) == 0 ) { /* refused by oprs */

	  str2 = parse_space_word(str1, &pt_tmp_str);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  refused_by_oprs(tmp_str);

	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);
     
     } else if ( strcmp (tmp_str, LIGHT_COMMAND ) == 0 ) {
	  Light_Status status;
	  Light *light;
	  Demo_Boolean error = FALSE;

	  str2 = parse_space_word(str1, &pt_tmp_str);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  if ( strcmp (tmp_str, LIGHT_NAME_Q ) == 0 )
	       light = demo->light_queue;
	  else	if ( strcmp (tmp_str, LIGHT_NAME_F ) == 0 )
	       light = demo->light_filling;
	  else {
	       error = TRUE;
	       demo_error (LG_STR("Unknown light name",
				  "Unknown light name"));
	  }
	  str2 = parse_space_word(str1, &pt_tmp_str);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  if ( strcmp (tmp_str, LIGHT_ON ) == 0 )
	       status = GO;
	  else	if ( strcmp (tmp_str, LIGHT_OFF ) == 0 )
	       status = STOP;
	  else {
	       error = TRUE;
	       demo_error (LG_STR("Unknown light status",
				  "Unknown light status"));
	  }
	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  if (! error)
	       change_light (light, status);

	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);

     } else if ( strcmp (tmp_str, VALVE_COMMAND ) == 0 ) {
	  Valve_Mode mode;
	  Demo_Boolean error = FALSE;

	  str2 = parse_space_word(str1, &pt_tmp_str);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  if ( strcmp (tmp_str, VALVE_OPEN ) == 0 )
	       mode = OPENING;
	  else	if ( strcmp (tmp_str, VALVE_CLOSE ) == 0 )
	       mode = CLOSING;
	  else {
	       error = TRUE;
	       demo_error (LG_STR("Unknown valve mode",
				  "Unknown valve mode"));
	  }
	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  if (! error)
	       change_valve (mode);
	  
	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);

     } else if ( strcmp (tmp_str, WARNING_COMMAND ) == 0 ) {

	  str2 = parse_space_between_par (str1, &pt_tmp_str);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  str2 = parse_close (str1);
	  if (str2 == str1)  return(1);	
	  str1 = str2;

	  demo_warning(tmp_str);

	  sprintf (trace_message,LG_STR("Get: %s\n",
					"Get: %s\n"), message);
	  oprs_message(trace_message);

     } else
     {
	  demo_error(LG_STR("decode_message : unkown message type ",
			    "decode_message : unkown message type "));
	  return(1);	
     }
     return(0);	
}

char *parse_word (char *str, char **res)
{
     char *tmp = str;
     int i;

     for ( i = 0; (*str != SPACE_CHAR) &&  (*str != CLOSE_CHAR) && (*str != NULL_CHAR); str++, i++)
	  /* null loop statement */;

     if (i == 0) {
	  demo_error (LG_STR("decode_message : invalid format : expecting a word",
			     "decode_message : invalid format : expecting a word"));
	  return (str);
     }
     BCOPY (tmp, *res, i);
     (*res)[i] = NULL_CHAR;
     return (str);
}

char *parse_int (char *str, int *val)
{
     char *tmp = str;

     for ( ; (*str != SPACE_CHAR) &&  (*str != CLOSE_CHAR) && (*str != NULL_CHAR); str++)
	  /* null loop statement */;
     if (!sscanf   (tmp, "%d",val)) {
	  return (tmp);
	  demo_error (LG_STR("decode_message : invalid format : expecting an integer",
			     "decode_message : invalid format : expecting an integer"));
     }
     return (str);
}

char *parse_open (char *str)
{
     if (*str != OPEN_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : open parenthesis ",
			     "decode_message : invalid format : open parenthesis "));
	  return (str);
     } else
	  return (++str);
}

char *parse_close (char *str)
{
     if (*str != CLOSE_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : close parenthesis ",
			     "decode_message : invalid format : close parenthesis "));
	  return (str);
     } else
	  return (++str);
}

char *parse_space (char *str)
{
     if (*str != SPACE_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : space ",
			     "decode_message : invalid format : space "));
	  return (str);
     } else
	  return (++str);
}

char *parse_space_word (char *str, char **res)
{
     char *tmp;
     int i;

     if (*str != SPACE_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : space ",
			     "decode_message : invalid format : space "));
	  return (str);
     } 
     str++;
     tmp = str;

     for ( i = 0; (*str != SPACE_CHAR) &&  (*str != CLOSE_CHAR) && (*str != NULL_CHAR); str++, i++)
	  /* null loop statement */;

     if (i == 0) {
	  demo_error (LG_STR("decode_message : invalid format : expecting a word",
			     "decode_message : invalid format : expecting a word"));
	  return (--str);
     }
     BCOPY (tmp, *res, i);
     (*res)[i] = NULL_CHAR;
     return (str);
}

char *parse_space_between_par (char *str, char **res)
{
     char *tmp, *tmp2 = str;
     int i;

     if (*str != SPACE_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : space ",
			     "decode_message : invalid format : space "));
	  return (tmp2);
     }
     str++;

     if (*str != OPEN_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : open parenthesis ",
			     "decode_message : invalid format : open parenthesis "));
	  return (tmp2);
     }

     tmp = ++str;
     for ( i = 0; (*str != CLOSE_CHAR) && (*str != NULL_CHAR); str++, i++)
	  /* null loop statement */;

     if (i == 0) {
	  demo_error (LG_STR("decode_message : invalid format ",
			     "decode_message : invalid format "));
	  return (tmp2);
     }
     BCOPY (tmp, *res, i);
     (*res)[i++] = END_CHAR;
     (*res)[i] = NULL_CHAR;

      if (*str != CLOSE_CHAR) {
	  demo_error (LG_STR("decode_message : invalid format : close parenthesis ",
			     "decode_message : invalid format : close parenthesis "));
	  return (tmp2);
     } else
	  return (++str);
}



void connect_to_oprs()
{
     if (!demo->connected) {
	   demo_warning (LG_STR("You must first connect to Message Passer\n",
				"You must first connect to Message Passer\n"));
	   return;
      } else {
	   char message[BUF_SIZE];

	   sprintf(message,"(%s %s)",CONNECT_COMMAND, demo_name);
	   send_message_to_oprs (message);
      }
}

void leave_oprs()
{
     if (!demo->connected) {
	   demo_error (LG_STR("We can not leave if we are not connected\n",
			      "We can not leave if we are not connected\n"));
	   return;
      } else {
	   char message[BUF_SIZE];
	   sprintf(message,"(%s %s)", STOP_TIME_COMMAND, demo_name);
	   send_message_to_oprs (message);
	   sprintf(message,"(%s %s)",DISCONNECT_COMMAND, demo_name);
	   send_message_to_oprs (message);
      }
}

void halt_oprs_time ()
{
	   send_message_to_oprs(HALT_TIME_MESSAGE);

}

void run_oprs_time ()
{
	   send_message_to_oprs(RUN_TIME_MESSAGE);

}

void ask_oprs_time (int initial_time)
{
     char message[BUF_SIZE];

     sprintf(message,"(%s %s %i)",INIT_TIME_COMMAND, demo_name, initial_time);
     send_message_to_oprs (message);
}

void run_oprs_load ()
{
     if (!demo->oprs_slave) {
	   demo_error (LG_STR("You are not slave of OPRS",
			      "You are not slave of OPRS"));
	   return;
      } else {
	   send_message_to_oprs(RUN_MESSAGE);
      }
}

void  send_oprs_sensor_busy()
{
     send_message_to_oprs(SENSOR_BUSY_MESSAGE);
}

void send_oprs_sensor_free()
{
     send_message_to_oprs(SENSOR_FREE_MESSAGE);
}

void send_oprs_tank_empty()
{
     send_message_to_oprs(TRUCK_READY_MESSAGE);
}


void send_oprs_tank_full()
{
     send_message_to_oprs(TRUCK_FULL_MESSAGE);
}

void send_oprs_sensor_open(int sensor_id)
{
     if (sensor_id == 1)
	  send_message_to_oprs(SENSOR1_OPEN_MESSAGE);
     else if (sensor_id == 2)
	  send_message_to_oprs(SENSOR2_OPEN_MESSAGE);
     else 
	  demo_error (LG_STR("Unknown sensor_id",
			     "Unknown sensor_id"));
}

void send_oprs_sensor_bp(int sensor_id)
{
     if (sensor_id == 1)
	  send_message_to_oprs(SENSOR1_BP_MESSAGE);
     else if (sensor_id == 2)
	  send_message_to_oprs(SENSOR2_BP_MESSAGE);
     else 
	  demo_error (LG_STR("Unknown sensor_id",
			     "Unknown sensor_id"));
}

void send_oprs_sensor_close(int sensor_id)
{
     if (sensor_id == 1)
	  send_message_to_oprs(SENSOR1_CLOSE_MESSAGE);
     else if (sensor_id == 2)
	  send_message_to_oprs(SENSOR2_CLOSE_MESSAGE);
     else 
	  demo_error (LG_STR("Unknown sensor_id",
			     "Unknown sensor_id"));
}

void send_oprs_light_move(Light *light)
{
     if (light->status == GO) {
	  if (light->name == LIGHT_Q) {
	       send_message_to_oprs(LIGHT1_GO_MESSAGE);
	  } else if (light->name == LIGHT_F) {
	       send_message_to_oprs(LIGHT2_GO_MESSAGE);
	  } else 
		     demo_error (LG_STR("Unknown light_name",
					"Unknown light_name"));
	 
     } else if (light->status == STOP) {
	  if (light->name == LIGHT_Q) {
	       send_message_to_oprs(LIGHT1_STOP_MESSAGE);
	  } else if (light->name == LIGHT_F) {
	       send_message_to_oprs(LIGHT2_STOP_MESSAGE);
	  } else 
		     demo_error (LG_STR("Unknown light_name",
					"Unknown light_name"));
	 
	 
     } else 
	  demo_error (LG_STR("Unknown light_status",
			     "Unknown light_status"));
}
