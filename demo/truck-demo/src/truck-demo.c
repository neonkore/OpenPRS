/*                               -*- Mode: C -*- 
 * truck-demo.c -- 
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

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawnB.h>
#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>

#include "constant.h"
#include "demo.h"
#include "demo_f.h"

#include "td-icon.bit"

void init_demo (void);
Demo_Boolean simulator (void);

Demo *demo;

#define XtNtalkbackBackground "talkbackBackground"
#define XtCTalkbackBackground "TalkbackBackground"

#define XtNbarberBackground "barberBackground"
#define XtCBarberBackground "BarberBackground"
#define XtNbarberForeground "barberForeground"
#define XtCBarberForeground "BarberForeground"

#define XtNred_light_onColor "red_light_onColor"
#define XtCRed_light_onColor "Red_light_onColor"
#define XtNred_light_offColor "red_light_offColor"
#define XtCRed_light_offColor "Red_light_offColor"

#define XtNgreen_light_onColor "green_light_onColor"
#define XtCGreen_light_onColor "Green_light_onColor"
#define XtNgreen_light_offColor "green_light_offColor"
#define XtCGreen_light_offColor "Green_light_offColor"

#define XtNemergencyColor "emergencyColor"
#define XtCEmergencyColor "EmergencyColor"

AppData app_data;

XtAppContext app_context;

static XtResource resources[] = {
     {
	  XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, global_fg), XtRString, XtDefaultForeground
     },
     {
	  XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, global_bg), XtRString, XtDefaultBackground
     },
     {
	  XtNtalkbackBackground, XtCTalkbackBackground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, talkback_bg), XtRString, XtDefaultBackground
     },
     {
	  XtNbarberForeground, XtCBarberForeground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, barber_fg), XtRString, XtDefaultForeground
     },
     {
	  XtNbarberBackground, XtCBarberBackground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, barber_bg), XtRString, XtDefaultBackground
     },
     {
	  XtNred_light_onColor, XtCRed_light_onColor, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, red_light_on_color), XtRString, "red"
     },
     {
	  XtNred_light_offColor, XtCRed_light_offColor, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, red_light_off_color), XtRString, "red"
     },
     {
	  XtNgreen_light_onColor, XtCGreen_light_onColor, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, green_light_on_color), XtRString, "green4"
     },
     {
	  XtNgreen_light_offColor, XtCGreen_light_offColor, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, green_light_off_color), XtRString, "green4"
     },
     {
	  XtNemergencyColor, XtCEmergencyColor, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(AppData, emergency_color), XtRString, "red"
     },

     {"language", "Language", XmRString, sizeof(String),
      XtOffsetOf(AppData, language), XmRImmediate, NULL}
};

/* this is to allow our parsing of these options. */
static XrmOptionDescRec options[] = {
   {"-m",	"",	XrmoptionSkipArg,	NULL},
   {"-j",	"",	XrmoptionSkipArg,	NULL},
   {"-n",	"",	XrmoptionSkipArg,	NULL},
   {"-t",	"",	XrmoptionSkipNArgs,	0},
   {"-L",	"",	XrmoptionSkipArg,	NULL},
   {"-h",	"",	XrmoptionSkipNArgs,	0}
};

String fallback_resources[] = {
#include "Truck-demo.ad.h"
NULL
};

/* For Attachment */
extern Widget clockBBoardFrame, commandBBoardFrame, controlBBoardFrame, fillingBBoardFrame, errorBBoardFrame;
extern Widget messageWindowFrame, oprsWindowFrame, warningWindowFrame;

extern Widget modeRunDButton, modeHaltDButton;

Widget demo_quitQuestion, demo_resetQuestion;
Widget aloneDButton, mp_connectDButton, oprs_connectDButton;


/*
 * quit button callback function
 */
void ReallyQuit(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtCloseDisplay(XtDisplay(w));
     exit(0); 
}

void Quit(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(demo_quitQuestion);
}

/*
 * reset button callback function
 */
void ReallyReset(Widget w, XtPointer client_data, XtPointer call_data)
{
     clear_all_textWindows();
     init_demo_time(0);
     init_valve ();
     init_filling ();
     reset_default_options_menus ();
}

void Reset(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(demo_resetQuestion);
}

void Clear(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     clear_all_textWindows();
}

void Alone(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     if (demo->oprs_slave) {
	  leave_oprs ();
	  demo->oprs_slave = FALSE;
	  unset_button (oprs_connectDButton);
	  set_button (aloneDButton);
	  if (demo->oprs_time) change_time_server ();
	  demo_message(LG_STR("you are now alone \n",
			      "you are now alone \n"));

     } else {
	  demo_warning(LG_STR("you are already alone",
			      "you are already alone"));
	  return;
      }

}

void Mp_connect(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     if (demo->connected) {
	  demo_warning(LG_STR("you are already connected to the Message Passer",
			      "you are already connected to the Message Passer"));
	  
     } else {
	  connect_to_mp ();
	  if (demo->connected) 
	       set_button (mp_connectDButton);
     } 
}

void Oprs_connect(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     if (demo->oprs_slave) {
	  demo_warning(LG_STR("you are already slave of OPRS",
			      "you are already slave of OPRS"));

     } else {
	  connect_to_oprs ();
     }
}

void accepted_by_oprs()
{
     demo->oprs_slave = TRUE;
     set_button (oprs_connectDButton);
     unset_button (aloneDButton);
     if (demo->oprs_time) {
	  change_time_server ();
	  if (demo->status == HALT) {
	       sleep(1);
	       halt_oprs_time ();
	  }
     }
     demo_message(LG_STR("you are now slave of OPRS \n",
			 "you are now slave of OPRS \n"));
     sleep(1);
     run_oprs_load ();

}

void refused_by_oprs(char *other)
{
     char message[BUF_SIZE];
     char *str =LG_STR("There is already another client named :",
		       "There is already another client named :");

     sprintf (message,"%s %s\n",str, other);
     demo_message (LG_STR("you have been refused by OPRS\n",
			  "you have been refused by OPRS\n"));
     demo_message (message);

}

int main(int argc, char **argv)
{
#ifdef X11R5     
     extern void _XEditResCheckMessages();
#endif

     Widget      topLevel, topForm, commandsMenu, commandsMenuFrame;
     Widget      quitDButton, resetDButton, clearDButton;

     Cardinal n;
     Arg args[MAXARGS];
     Pixmap icon_pixmap;
     char *language_str;

     init_demo ();
     
     topLevel = XtVaAppInitialize(
				  &app_context,	/* Application context */
				  "Truck-demo",	/* application class name */
				   options, XtNumber(options),  /* command line option list */
				  (Cardinal *) & argc, argv,	/* command line args */
				  fallback_resources,	/* for missing app-defaults file */
				  NULL);	/* terminate varargs list */



#ifdef X11R5     
     XtAddEventHandler(topLevel, (EventMask)0, True, _XEditResCheckMessages, NULL);
#endif

     XtGetApplicationResources(topLevel, &app_data, resources, XtNumber(resources), NULL, 0);

     language_str = app_data.language;
     select_language(language_str, TRUE);

     icon_pixmap = XCreateBitmapFromData(XtDisplay(topLevel),
					 RootWindowOfScreen(XtScreen(topLevel)),
					 td_icon_bits,
					 td_icon_width, td_icon_height);

     XtVaSetValues(topLevel,
		   XmNiconPixmap, icon_pixmap,
		   XmNtitle, LG_STR("Truck Loading Simulator",
				    "Simulateur Chargement Camion"),
		   XmNiconName, LG_STR("Truck Sim",
				       "Sim Camion"),
		   NULL);

     demo_init_arg(argc, argv);
    
     topForm = XmCreateForm(topLevel, "topForm", NULL, 0);
     XtManageChild(topForm);

     init_colors(topForm);
     init_pixmaps(topForm);

     n = 0;
     commandsMenuFrame = XmCreateFrame(topForm,"commandsMenuFrame",args,n);
     XtManageChild(commandsMenuFrame);

     n = 0;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     commandsMenu = XmCreateWorkArea(commandsMenuFrame, "commandsMenu", args, n);
     XtManageChild(commandsMenu);

     n = 0;
     quitDButton = XmCreateDrawnButton(commandsMenu,"quitDButton", args, n);
     XtAddCallback(quitDButton, XmNactivateCallback, Quit, 0);
     XtManageChild(quitDButton);

     resetDButton = XmCreateDrawnButton(commandsMenu,"resetDButton", args, n);
     XtAddCallback(resetDButton, XmNactivateCallback, Reset, 0);
     XtManageChild(resetDButton);

     clearDButton = XmCreateDrawnButton(commandsMenu,"clearDButton", args, n);
     XtAddCallback(clearDButton, XmNactivateCallback, Clear, 0);
     XtManageChild(clearDButton);

     aloneDButton = XmCreateDrawnButton(commandsMenu,"aloneDButton", args, n);
     XtAddCallback(aloneDButton, XmNactivateCallback, Alone, 0);
     XtManageChild(aloneDButton);

     mp_connectDButton = XmCreateDrawnButton(commandsMenu,"mp_connectDButton", args, n);
     XtAddCallback(mp_connectDButton, XmNactivateCallback, Mp_connect, 0);
     XtManageChild(mp_connectDButton);

     oprs_connectDButton = XmCreateDrawnButton(commandsMenu,"oprs_connectDButton", args, n);
     XtAddCallback(oprs_connectDButton, XmNactivateCallback, Oprs_connect, 0);
     XtManageChild(oprs_connectDButton);

     demo_quitQuestion = XmCreateQuestionDialog(topLevel, "demo_quitQuestion", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(demo_quitQuestion, XmDIALOG_HELP_BUTTON));
     XtAddCallback(demo_quitQuestion, XmNokCallback, ReallyQuit, NULL);
    
     demo_resetQuestion = XmCreateQuestionDialog(topLevel, "demo_resetQuestion", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(demo_resetQuestion, XmDIALOG_HELP_BUTTON));
     XtAddCallback(demo_resetQuestion, XmNokCallback, ReallyReset, NULL);

     make_text_windows(topForm);

     make_clock_bboard(topForm);

     make_command_bboard(topForm);

     make_control_bboard(topForm);

     make_filling_bboard(topForm);

     make_error_bboard(topForm);

/* Attach Widget */

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetValues(messageWindowFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, messageWindowFrame); n++;
     XtSetValues(warningWindowFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, warningWindowFrame); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(oprsWindowFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, messageWindowFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(commandsMenuFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, commandsMenuFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetValues(clockBBoardFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, commandsMenuFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, clockBBoardFrame); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, errorBBoardFrame); n++;
     XtSetValues(controlBBoardFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, controlBBoardFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, clockBBoardFrame); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, errorBBoardFrame); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, fillingBBoardFrame); n++;
     XtSetValues(commandBBoardFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, commandsMenuFrame); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(errorBBoardFrame , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, clockBBoardFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, errorBBoardFrame); n++;
     XtSetValues(fillingBBoardFrame , args, n);

     XtRealizeWidget(topLevel);
    
     if (demo->oprs_slave) {
	  set_button (oprs_connectDButton);
	  unset_button (aloneDButton);
     } else {
	  unset_button (oprs_connectDButton);
	  set_button (aloneDButton);
     }

     XtAppAddWorkProc(app_context, (XtWorkProc)simulator, NULL);
     
     init_demo_time (0);
     init_valve ();
     init_filling ();

     XtAppMainLoop(app_context);

     
     return ;
}

void init_demo ()
{
     Valve *valve;
     Tank *tank;
     Light *light_filling, *light_queue;
     Truck *truck;
     int i;
     int nb_truck;

     demo = (Demo *) malloc (sizeof (Demo));
     demo->status = HALT;
     demo->oprs_slave = FALSE;
     demo->oprs_time = TRUE;
     
     nb_truck = 5;
     if (nb_truck > MAX_TRUCK) {
	  demo_error (LG_STR("nb_truck is too big",
			     "nb_truck is too big"));
	  nb_truck = MAX_TRUCK;
     }
     demo->nb_truck = nb_truck;

     valve = (Valve *) malloc (sizeof (Valve));
     demo->valve = valve;
     tank = (Tank *) malloc (sizeof (Tank));
     demo->tank = tank;
     demo->sensor1 = (Sensor *) malloc (sizeof (Sensor));
     demo->sensor2 = (Sensor *) malloc (sizeof (Sensor));
     demo->light_filling = light_filling = (Light *) malloc (sizeof (Light));
     demo->light_queue = light_queue = (Light *) malloc (sizeof (Light));
     demo->tr_sensor = (Tr_sensor *) malloc (sizeof (Tr_sensor));

     for (i=0; i < demo->nb_truck ; i++) {
	  truck = (Truck *) malloc (sizeof (Truck));
	  truck->number = i;
	  truck->light_filling = light_filling;
	  truck->light_queue = light_queue;
	  demo->truck[i] = truck;
     }
     truck = (Truck *) malloc (sizeof (Truck));
     truck->number = -1;
     truck->light_filling = light_filling;
     truck->light_queue = light_queue;
     demo->mysterious_truck = truck;

     demo->sensor1->valve = valve; 
     demo->sensor2->valve = valve; 

     demo->sensor1->id = 1; 
     demo->sensor2->id = 2; 
}

void re_register_simulator(XtPointer ignore1, XtIntervalId ignore2)
{
/*
     XtWorkProcId  workProc_id = XtAppAddWorkProc(app_context, simulator, NULL);
     printf ("%d \n", workProc_id);
*/

     XtAppAddWorkProc(app_context, (XtWorkProc)simulator, NULL);
}

Demo_Boolean simulator ()
{
     Demo_Time time;
     int i;
     
     if ((! demo->oprs_slave ) || (! demo->oprs_time )){
	  update_clock();
     }
     if (demo->status == RUN) {
	  time = get_demo_time();
	  check_valve(time);
	  check_sensor(demo->sensor1,time);
	  check_sensor(demo->sensor2,time);
	  check_tank(time);
	  for (i = 0; i < demo->nb_truck; i++)
	       check_truck(demo->truck[i],time);

	  check_truck(demo->mysterious_truck,time);
     }

     XtAppAddTimeOut(app_context,100, (XtTimerCallbackProc)re_register_simulator, NULL); /* This guy will re register us in 100 ms. */
     return (True); /* This is a work proc : we  deregister it */
}


#if  defined(X11R5) && defined (SUN_BSD)

/* This is because of a bug in X11R5 Sun libraries.... do not ask.... */
dlsym()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

dlopen()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

dlclose()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

#endif
