/*                               -*- Mode: C -*- 
 * demo-time.c -- 
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
#include <math.h>

#include "constant.h"
#include "demo.h"

#define MAX_INDICE 7
#define MIN_INDICE -8

double pow(double, double);

static Demo_Time demo_time, last_printed_time;
static float demo_speed;
static int demo_speed_indice;

extern Widget modeRunDButton, modeHaltDButton;
extern Widget master_label, timer_label, speed_label;

#include "demo_f.h"
#include "local-time_f.h"

void update_clock()
{
     char string[STRSIZE];
     XmString xmstr;
     Demo_Time time;

     time = get_demo_time();
     if (time != last_printed_time) {
	  last_printed_time = time;
	  sprintf (string,"%6d", time);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( timer_label, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
     }
}

void init_demo_time(int initial_time)
{
     char string[STRSIZE];
     XmString xmstr;

     if (demo->oprs_slave && demo->oprs_time) { /* tell OPRS to send us time */
	  ask_oprs_time (initial_time);
	  xmstr = XmStringCreateSimple("OPRS");

     } else { 
	  init_time(initial_time);
	  xmstr = XmStringCreateSimple("LOCAL");
     }
     XtVaSetValues( master_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

     demo_time = initial_time;

     sprintf (string,"%6d", demo_time);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( timer_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
     last_printed_time = demo_time;

     demo_speed = 1.0;
     demo_speed_indice = 0;
     init_speed (MIN_INDICE, MAX_INDICE);

     sprintf (string,"%.4f", demo_speed);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( speed_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

     if (demo->status == HALT) {
	  unset_button (modeRunDButton);
	  set_button (modeHaltDButton);
     } else {
	  set_button (modeRunDButton);
	  unset_button (modeHaltDButton);
     }
}

void change_time_server ()
{
     init_demo_time (demo_time);
}

Demo_Time get_demo_time ()
{
     if (demo->oprs_slave && demo->oprs_time) /* The Time has been sent by oprs */
	  return(demo_time);

     if (demo->status == HALT)
	  return(demo_time);

     demo_time = get_local_time ();
     return(demo_time);
}

void change_demo_speed(Speed_Mode mode)
{
     char string[STRSIZE];
     XmString xmstr;

     if (demo->oprs_slave) {
	  demo_warning(LG_STR("change_demo_speed: Not implemented in connected mode.\n",
			      "change_demo_speed: Pas implémenté en mode connecré.\n"));
	  return;
     }
     if (mode == FASTER){
	  if (demo_speed_indice >= MAX_INDICE){
	       demo_warning(LG_STR("change_demo_speed: Maximal Speed.\n",
				   "change_demo_speed: Vitesse Maximale.\n"));
	       return;
	  } else
	       demo_speed_indice++;
     } else if (mode == SLOWER) {
	  if (demo_speed_indice <= MIN_INDICE) {
	       demo_warning(LG_STR("change_demo_speed: Minimal Speed.\n",
				   "change_demo_speed: Vitesse Minimale.\n"));
	       return;
	  } else
	       demo_speed_indice--;
     } else {
	  demo_error(LG_STR("change_demo_speed: Unknown mode.",
			    "change_demo_speed: Mode inconnu."));
	  return;
     }
     set_speed( demo_speed_indice);

     demo_speed = pow ( 2 , demo_speed_indice);

     sprintf (string,"%.4f", demo_speed);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( speed_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
}

void change_run_mode(Demo_Run_Mode mode)
{
     if (demo->status == mode)
	  return;

     if (mode == HALT) {      
	  if (demo->oprs_slave && demo->oprs_time) {
	       halt_oprs_time();
	  } else
	       halt_time();
	  demo->status = mode;
	  unset_button (modeRunDButton);
	  set_button (modeHaltDButton);
     } else if (mode == RUN){
	  if (demo->oprs_slave && demo->oprs_time) {
	       run_oprs_time();
	  } else
	       wake_up_time();
	  demo->status = mode;
	  unset_button (modeHaltDButton);
	  set_button (modeRunDButton);
     } else {
	  demo_error(LG_STR("set_run_mode: Unknown mode.",
			    "set_run_mode: Mode inconnu."));
	  return;
     }
}

void set_time_from_oprs(int oprs_time)
{
     if (demo->oprs_time) {
	  demo_time = oprs_time;
	  update_clock();
     }
     return;
}
