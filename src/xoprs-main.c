/*                               -*- Mode: C -*- 
 * xoprs-main.c -- Top level file for the X/Motif interface for one OPRS.
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#include <string.h>

/* 
 * Standard Toolkit include files
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/DrawnB.h>
#include <Xm/PanedW.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>	      
#include <Xm/RowColumn.h>

#include <Xm/Separator.h>

#include "xhelp.h"
#include "xhelp_f.h"

#include "constant.h"
#include "macro.h"
#include "version.h"

#include "xoprs-textwin.h"
#include "oprs-type.h"
#include "oprs.h"
#include "parser-funct.h"
#include "oprs-main.h"
#include "int-graph.h"
#include "top-lev.h"
#include "op-default.h"
#include "ope-graphic.h"
#include "xoprs-intention.h"
#include "xoprs-textwin_f.h"

#include "mp-register.h"
#include "xoprs-main.h"
#include "xoprs-main_f.h"
#include "ope-graphic_f.h"
#include "xoprs-intention_f.h"
#include "top-lev_f.h"
#include "xoprs-menu_f.h"
#include "oprs-type_f.h"
#include "oprs-init_f.h"

PBoolean install_user_trace = FALSE;

Boolean xoprs_top_level_loop(XtPointer oprs);
Boolean wait_other_events(XtPointer oprs);
void reset_oprs_kernel(Oprs *oprs);
void update_active_idle(XtPointer oprs, XtIntervalId *ignore);
void UpdateMessageWindow(char *string);
void ClearMessageWindow(void);

void set_button(Widget w);
void unset_button(Widget w); 

Widget x_oprs_top_level_widget;

Widget topLevel, messageWindow,  oprsIdleDButton, oprsStoppedDButton, oprsActiveDButton;

extern PBoolean use_dialog_error;

PBoolean main_loop_registered = FALSE;

PBoolean other_events_registered = FALSE;

XtWorkProcId other_events_wp;

void register_main_loop_from_other_events(Oprs *oprs)
{
     if (! main_loop_registered) {
	  XtAppAddWorkProc(app_context,&xoprs_top_level_loop,oprs);
	  main_loop_registered = TRUE;
     }
}

void register_main_loop(Oprs *oprs)
{
     register_main_loop_from_other_events(oprs);
     if (other_events_registered) {
	  XtRemoveWorkProc(other_events_wp);
	  other_events_registered = FALSE;
     }
}

void deregister_main_loop(Oprs *oprs)
{
     main_loop_registered = FALSE;
     other_events_wp = XtAppAddWorkProc(app_context,&wait_other_events,oprs);
     other_events_registered = TRUE;
     XtAppAddTimeOut(app_context, (main_loop_pool_sec * 1000) +  (main_loop_pool_usec / 1000),
		     (XtTimerCallbackProc)register_main_loop, oprs); /* This guy will re register us. */
}

Boolean wait_other_events(XtPointer oprs_par)
{
     fd_set readfds;
     int nfound, max_fds;
     struct timeval pool_tv;
     Oprs *oprs = (Oprs *)oprs_par;

     set_readfds(&readfds, &max_fds, FALSE);
     pool_tv.tv_sec = 0;
     pool_tv.tv_usec = 1000;	/* we check the 2 sockets for new facts or goals. 1 mili second timeout. 
				   Because we are called as an XtWorkProc and we do not want to block the rest of the application. */
     if ((nfound = select(max_fds, &readfds, NULL, NULL, &pool_tv)) < 0)	
	  if (errno != EINTR) {
	       perror("wait_other_events: select NULL");
	  }
     if (nfound > 0 ){
	  register_main_loop_from_other_events(oprs);
	  other_events_registered = FALSE;
	  return TRUE;
     } else return FALSE;
}


void set_oprs_active_mode(PBoolean mode)
{
     if (mode) {
	  unset_button(oprsIdleDButton);
	  set_button(oprsActiveDButton);
     } else {
	  set_button(oprsIdleDButton);
	  unset_button(oprsActiveDButton);
     }
}

void xset_oprs_run_mode(Oprs_Run_Type mode)
{
     switch(mode) {
     case RUN:
	  unset_button(oprsStoppedDButton);
	  register_main_loop(current_oprs);
	  break;
     case STEP_NEXT:
	  unset_button(oprsStoppedDButton);
	  register_main_loop(current_oprs);
	  break;
     case STEP_HALT:
	  set_button(oprsStoppedDButton);
	  break;
     case HALT:
	  set_button(oprsStoppedDButton);
	  break;
     case STEP:
	  unset_button(oprsStoppedDButton);
	  break;
     default:
	  fprintf(stderr, LG_STR("xset_oprs_run_mode: unknown run_mode.\n",
				 "xset_oprs_run_mode: unknown run_mode.\n"));
     }
}

void OprsResetButton(Widget w, XtPointer oprs, XtPointer call_data)
{ 
/*     reset_oprs_kernel((Oprs *)oprs); */
     resetQuestionManage();
}

void OprsHaltButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(HALT);
}

void OprsStepButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(STEP);
}

void OprsStepNextButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(STEP_NEXT);
}

void OprsRunButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(RUN);
}

XtAppContext app_context;

#include "xoprs-icon.bit"

Pixel globalBack, globalFore;

struct _resrcs {
     XmFontList fontList;		  /* font to use for bitmap labels */
     int work_width, work_height;	  /* default bitmap size */
     Boolean  printWidgetTree;
     Boolean  printEnglishOperator;
     char *log_file;
     char *language;
     int  ip_x;
     int  ip_y;
     int  ip_width;
     int  ctxt_x;
     int  ctxt_y;
     int  ctxt_width;
     int  set_x;
     int  set_y;
     int  set_width;
     int  eff_x;
     int  eff_y;
     int  eff_width;
     int  prop_x;
     int  prop_y;
     int  prop_width;
     int  doc_x;
     int  doc_y;
     int  doc_width;
     int  act_x;
     int  act_y;
     int  act_width;
     int  bd_x;
     int  bd_y;
     int  bd_width;
     int  edge_width;
}       Resrcs;

static XtResource resources[] = {
     {"workWidth", "WorkWidth", XmRInt, sizeof(int),
     XtOffsetOf(struct _resrcs, work_width), XmRImmediate, (char *) WORK_WIDTH},
     {"workHeight", "WorkHeight", XmRInt, sizeof(int),
     XtOffsetOf(struct _resrcs, work_height), XmRImmediate, (char *) WORK_HEIGHT},
     {"logFile", "LogFile", XmRString, sizeof(String),
     XtOffsetOf(struct _resrcs, log_file), XmRImmediate, NULL},
     {"language", "Language", XmRString, sizeof(String),
     XtOffsetOf(struct _resrcs, language), XmRImmediate, NULL},

     {"ipX", "IpX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_x), XmRImmediate, (char *)IP_X},
     {"ipY", "IpY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_y), XmRImmediate, (char *)IP_Y},
     {"ipWidth", "IpWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_width), XmRImmediate, (char *)IP_WIDTH},
     {"ctxtX", "CtxtX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_x), XmRImmediate, (char *)CTXT_X},
     {"ctxtY", "CtxtY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_y), XmRImmediate, (char *)CTXT_Y},
     {"ctxtWidth", "CtxtWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_width), XmRImmediate, (char *)CTXT_WIDTH},
     {"setX", "SetX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_x), XmRImmediate, (char *)SET_X},
     {"setY", "SetY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_y), XmRImmediate, (char *)SET_Y},
     {"setWidth", "SetWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_width), XmRImmediate, (char *)SET_WIDTH},
     {"effX", "EffX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_x), XmRImmediate, (char *)EFF_X},
     {"effY", "EffY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_y), XmRImmediate, (char *)EFF_Y},
     {"effWidth", "EffWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_width), XmRImmediate, (char *)EFF_WIDTH},
     {"propX", "PropX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_x), XmRImmediate, (char *)PROP_X},
     {"propY", "PropY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_y), XmRImmediate, (char *)PROP_Y},
     {"propWidth", "PropWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_width), XmRImmediate, (char *)PROP_WIDTH},
     {"docX", "DocX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_x), XmRImmediate, (char *)DOC_X},
     {"docY", "DocY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_y), XmRImmediate, (char *)DOC_Y},
     {"docWidth", "DocWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_width), XmRImmediate, (char *)DOC_WIDTH},
     {"actX", "ActX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_x), XmRImmediate, (char *)ACT_X},
     {"actY", "ActY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_y), XmRImmediate, (char *)ACT_Y},
     {"actWidth", "ActWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_width), XmRImmediate, (char *)ACT_WIDTH},
     {"bdX", "BdX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_x), XmRImmediate, (char *)BD_X},
     {"bdY", "BdY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_y), XmRImmediate, (char *)BD_Y},
     {"bdWidth", "BdWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_width), XmRImmediate, (char *)BD_WIDTH},
     {"edgeWidth", "EdgeWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, edge_width), XmRImmediate, (char *)EDGE_WIDTH},

     {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     XtOffsetOf(struct _resrcs, fontList), XmRImmediate, NULL},
     {"printWidgetTree", "PrintWidgetTree", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printWidgetTree), XmRImmediate, (XtPointer)FALSE},
     {"printEnglishOperator", "PrintEnglishOperator", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printEnglishOperator), XmRImmediate, (XtPointer)FALSE},
};

/* this is to allow our parsing of these options. */
static XrmOptionDescRec options[] = {
   {"-s",	"",	XrmoptionSkipArg,	NULL},
   {"-i",	"",	XrmoptionSkipArg,	NULL},
   {"-m",	"",	XrmoptionSkipArg,	NULL},
   {"-j",	"",	XrmoptionSkipArg,	NULL},
   {"-n",	"",	XrmoptionSkipArg,	NULL},
   {"-I",	"",	XrmoptionSkipArg,	NULL},
   {"-D",	"",	XrmoptionSkipArg,	NULL},
   {"-P",	"",	XrmoptionSkipArg,	NULL},
   {"-E",	"",	XrmoptionSkipArg,	NULL},
   {"-F",	"",	XrmoptionSkipArg,	NULL},
   {"-l",	"",	XrmoptionSkipArg,	NULL},
   {"-L",	"",	XrmoptionSkipArg,	NULL},
   {"-h",	"",	XrmoptionSkipNArgs,	0},
   {"-q",	"",	XrmoptionSkipNArgs,	0},
   {"-t",	"",	XrmoptionSkipNArgs,	0},
   {"-a",	"",	XrmoptionSkipNArgs,	0},
   {"-A",	"",	XrmoptionSkipNArgs,	0},
   {"-p",	"",	XrmoptionSkipNArgs,	0},
   {"-c",	"",	XrmoptionSkipArg,	NULL},
   {"-x",	"",	XrmoptionSkipArg,	NULL},
   {"-d",	"",	XrmoptionSkipArg,	NULL},
   {"-pwt",	"*printWidgetTree",	XrmoptionNoArg,	"True"},
   {"-log",	"*logFile",	XrmoptionSepArg,	NULL},
   {"-peo",	"*printEnglishOperator",	XrmoptionNoArg,	"True"},
     };

String fallback_resources[] = {
#include "XOprs.ad.h"
NULL
};

Widget textWindow;
int gtk = 0;

#ifdef C_PLUS_PLUS_RELOCATABLE
int oprs_main(int argc,char **argv, char ** envp)
#else
int main(int argc, char **argv, char **envp)
#endif
{
     char *server_hostn;
     int server_port;

     char *mp_hostn;
     int mp_port;

#ifdef XEDITRESCHECKMESSAGES_AVAILABLE
     extern void _XEditResCheckMessages();
#endif
     Pixmap icon_pixmap;
     Cardinal n;
     Arg args[MAXARGS];
     char *name;
     char title[LINSIZ];
     char welcome[LINSIZ];
     Draw_Data dd;
     Int_Draw_Data idd;
     Oprs *oprs;
     char *log_file;
     char *language_str;

     Widget topForm, oprsDrawWinFrame, oprsDrawWin, oprsIntDrawWinFrame, oprsIntDrawWin;
     Widget oprsMenuFrame, oprsMenu, oprsRunButton, oprsHaltButton, oprsResetButton, oprsStepButton, oprsStepNextButton;
     Widget menuBar;

     progname = argv[0];

     use_dialog_error = TRUE;

     global_draw_data = &dd;
     global_int_draw_data = &idd;

     topLevel =  XtVaAppInitialize(
				   &app_context, /* Application context */
				   "XOprs", /* application class name */
				   options, XtNumber(options),  /* command line option list */
				   &argc, argv, /* command line args */
				   fallback_resources,	/* for missing
							 * app-defaults file */
				   NULL); /* terminate varargs list */

#ifdef XEDITRESCHECKMESSAGES_AVAILABLE
     XtAddEventHandler(topLevel, (EventMask)0, True, _XEditResCheckMessages, NULL);
#endif

     x_oprs_top_level_widget = topLevel; /*  This is the external name . */

     XtGetApplicationResources(topLevel, &Resrcs, resources, XtNumber(resources), NULL, 0);

     print_english_operator = Resrcs.printEnglishOperator;
     log_file = Resrcs.log_file;
     language_str = Resrcs.language;
     ip_x = Resrcs.ip_x;
     ip_y = Resrcs.ip_y;
     ip_width = Resrcs.ip_width;
     ctxt_x = Resrcs.ctxt_x;
     ctxt_y = Resrcs.ctxt_y;
     ctxt_width = Resrcs.ctxt_width;
     set_x = Resrcs.set_x;
     set_y = Resrcs.set_y;
     set_width = Resrcs.set_width;
     eff_x = Resrcs.eff_x;
     eff_y = Resrcs.eff_y;
     eff_width = Resrcs.eff_width;
     prop_x = Resrcs.prop_x;
     prop_y = Resrcs.prop_y;
     prop_width = Resrcs.prop_width;
     doc_x = Resrcs.doc_x;
     doc_y = Resrcs.doc_y;
     doc_width = Resrcs.doc_width;
     act_x = Resrcs.act_x;
     act_y = Resrcs.act_y;
     act_width = Resrcs.act_width;
     bd_x = Resrcs.bd_x;
     bd_y = Resrcs.bd_y;
     bd_width = Resrcs.bd_width;
     edge_width = Resrcs.edge_width;

     select_language(language_str, TRUE);

     oprs_init_env();
     name = oprs_init_arg(argc, argv, &mp_hostn, &mp_port, &server_hostn, &server_port);

#if SETBUF_UNDEFINED
     setbuffer(stdout,NULL,0);
#else
     setbuf(stdout,NULL);
#endif

     oprs = oprs_init_main(name, TRUE, mp_hostn, mp_port, server_hostn, server_port);

     icon_pixmap = XCreateBitmapFromData(XtDisplay(topLevel),
					 RootWindowOfScreen(XtScreen(topLevel)),
					 xp_icon_bits,
					 xp_icon_width, xp_icon_height );

     sprintf(title,"XOprs %s --- %s",  package_version, name);

     XtVaSetValues(topLevel, 
		   XmNiconPixmap, icon_pixmap,
		   XmNtitle, title,
		   XmNiconName,name,
		   XmNborderWidth, 1,
		   NULL);

     create_help_info(topLevel);
    
     n = 0;
     XtSetArg(args[n], XmNfractionBase, 5); n++;
     topForm = XmCreateForm(topLevel, "topForm", args,n);
     XtManageChild(topForm);
    
     menuBar = xp_create_menu_bar(topForm, &dd, &idd);

     n = 0;
     oprsMenuFrame = XmCreateFrame(topForm,"oprsMenuFrame",args,n);
     XtManageChild(oprsMenuFrame);

     n = 0;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET)); n++;
     messageWindow = XtCreateManagedWidget("messageWindow", xmLabelWidgetClass, topForm, args, n);

     n = 0;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     oprsMenu = XmCreateWorkArea(oprsMenuFrame,"oprsMenu",args,n);
     XtManageChild(oprsMenu);


     n = 0;
     XtSetArg(args[n], XmNtraversalOn, False); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
     oprsActiveDButton = 
#ifdef LESSTIF
	  XmCreatePushButton
#else
	  XmCreateDrawnButton
#endif
	  (oprsMenu,"oprsActiveDButton", args, n);
     XtManageChild(oprsActiveDButton);

     oprsIdleDButton = 
#ifdef LESSTIF
	  XmCreatePushButton
#else
	  XmCreateDrawnButton
#endif
	  (oprsMenu,"oprsIdleDButton", args, n);
     XtManageChild(oprsIdleDButton);

     oprsStoppedDButton = 
#ifdef LESSTIF
	  XmCreatePushButton
#else
	  XmCreateDrawnButton
#endif
	  (oprsMenu,"oprsStoppedDButton", args, n);
     XtManageChild(oprsStoppedDButton);


     XtManageChild(XmCreateSeparator(oprsMenu,"oprsMenuSeparator",NULL,0));
     XtManageChild(XmCreateSeparator(oprsMenu,"oprsMenuSeparator",NULL,0));

     oprsRunButton = XmCreatePushButtonGadget(oprsMenu,"oprsRunButton", NULL, 0);
     XtAddCallback(oprsRunButton, XmNarmCallback, OprsRunButton, &dd);
     XtManageChild(oprsRunButton);

     oprsStepButton = XmCreatePushButtonGadget(oprsMenu,"oprsStepButton", NULL, 0);
     XtAddCallback(oprsStepButton, XmNarmCallback, OprsStepButton, &dd);
     XtManageChild(oprsStepButton);

     oprsStepNextButton = XmCreatePushButtonGadget(oprsMenu,"oprsStepNextButton", NULL, 0);
     XtAddCallback(oprsStepNextButton, XmNarmCallback, OprsStepNextButton, &dd);
     XtManageChild(oprsStepNextButton);

     oprsHaltButton = XmCreatePushButtonGadget(oprsMenu,"oprsHaltButton", NULL, 0);
     XtAddCallback(oprsHaltButton, XmNarmCallback, OprsHaltButton, &dd);
     XtManageChild(oprsHaltButton);

     XtManageChild(XmCreateSeparator(oprsMenu,"oprsMenuSeparator",NULL,0));

     oprsResetButton = XmCreatePushButtonGadget(oprsMenu,"oprsResetButton", NULL, 0);
     XtAddCallback(oprsResetButton, XmNactivateCallback, OprsResetButton, oprs);
     XtManageChild(oprsResetButton);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget,oprsMenuFrame); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, messageWindow); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

     oprsDrawWinFrame = XmCreateFrame(topForm, "oprsDrawWinFrame", args, n);
     XtManageChild(oprsDrawWinFrame);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, menuBar); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget,oprsDrawWinFrame); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget,oprsMenuFrame); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 3); n++;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, True); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
#ifndef LESSTIF
     XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); n++;
     XtSetArg(args[n], XmNscrollLeftSide, True); n++;
#endif
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;

     textWindow = XmCreateScrolledText (topForm, "textWindow", args, n);
     XtAddCallback (textWindow, XmNmodifyVerifyCallback, (XtCallbackProc)VerifySizeTextWindow, NULL);
     XtManageChild (textWindow);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, menuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, XtParent(textWindow)); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget,oprsDrawWinFrame); n++;

     oprsIntDrawWinFrame = XmCreateFrame(topForm, "oprsIntDrawWinFrame", args, n);
     XtManageChild(oprsIntDrawWinFrame);

     oprsIntDrawWin = XmCreateScrolledWindow(oprsIntDrawWinFrame, "oprsIntDrawWin", NULL, 0);
     XtManageChild(oprsIntDrawWin);

     idd.canvas = XmCreateDrawingArea(oprsIntDrawWin, "oprsIntCanvas", NULL, 0);
     XtManageChild(idd.canvas);

     XtSetArg(args[0], XmNheight, &idd.canvas_height);
     XtSetArg(args[1], XmNwidth, &idd.canvas_width);
     XtGetValues(idd.canvas, args, 2);

     idd.copy_area_index_queue = make_queue();
     idd.work_height = MAX(IDD_WORK_HEIGHT, idd.canvas_height);
     idd.work_width = MAX(IDD_WORK_WIDTH, idd.canvas_width);
     idd.top = 0;
     idd.left = 0;
     idd.ig = NULL;
     idd.expose_region = NULL;
     idd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT);
     idd.fontlist = Resrcs.fontList;
     idd.reposition_all = TRUE;

     XtAddCallback(idd.canvas, XmNexposeCallback, (XtCallbackProc)idd_handle_exposures, &idd);
     XtAddCallback(idd.canvas, XmNresizeCallback, (XtCallbackProc)idd_resize, &idd);
     XtAddEventHandler(idd.canvas, 0, TRUE, (XtEventHandler)idd_handle_g_exposures, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonPressMask, FALSE, (XtEventHandler)idd_canvas_mouse_press, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonMotionMask, FALSE, (XtEventHandler)idd_canvas_mouse_motion, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonReleaseMask, FALSE, (XtEventHandler)idd_canvas_mouse_release, (XtPointer)&idd);

     idd_create_scrollbars(oprsIntDrawWin, &idd);

     n = 0;
     XtSetArg(args[n], XmNverticalScrollBar, idd.vscrollbar); n++;
     XtSetArg(args[n], XmNhorizontalScrollBar, idd.hscrollbar); n++;
     XtSetArg(args[n], XmNworkWindow, idd.canvas); n++;
     XtSetValues(oprsIntDrawWin, args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, menuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

     XtSetValues(oprsMenuFrame, args, n);

     oprsDrawWin = XmCreateScrolledWindow(oprsDrawWinFrame, "oprsDrawWin", NULL, 0);
     XtManageChild(oprsDrawWin);

     dd.canvas = XmCreateDrawingArea(oprsDrawWin, "oprsCanvas", NULL, 0);
     XtManageChild(dd.canvas);

     XtSetArg(args[0], XmNheight, &dd.canvas_height);
     XtSetArg(args[1], XmNwidth, &dd.canvas_width);
     XtGetValues(dd.canvas, args, 2);

     dd.copy_area_index_queue = make_queue();
     dd.mode = MOVING_CANVAS;
     dd.og_selected_on_press = NULL;
     dd.work_height = MAX3(WORK_HEIGHT, Resrcs.work_height, (int)dd.canvas_height);
     dd.work_width = MAX3(WORK_WIDTH, Resrcs.work_width, (int)dd.canvas_width);
     dd.top = 0;
     dd.left = 0;
     dd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT);
     dd.fontlist = Resrcs.fontList;
     dd.expose_region = NULL;
     dd.just_compiling = FALSE;
     dd.op = NULL;

     XtAddCallback(dd.canvas, XmNexposeCallback, (XtCallbackProc)handle_exposures, &dd);
     XtAddCallback(dd.canvas, XmNresizeCallback, (XtCallbackProc)resize, &dd);
     XtAddEventHandler(dd.canvas, 0, TRUE, (XtEventHandler)handle_g_exposures, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonPressMask, FALSE, (XtEventHandler)oprs_canvas_mouse_press, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonMotionMask, FALSE, (XtEventHandler)oprs_canvas_mouse_motion, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonReleaseMask, FALSE, (XtEventHandler)oprs_canvas_mouse_release, (XtPointer)&dd);

     create_scrollbars(oprsDrawWin, &dd);

     n = 0;
     XtSetArg(args[n], XmNverticalScrollBar, dd.vscrollbar); n++;
     XtSetArg(args[n], XmNhorizontalScrollBar, dd.hscrollbar); n++;
     XtSetArg(args[n], XmNworkWindow, dd.canvas); n++;
     XtSetValues(oprsDrawWin, args, n);

     xpcreate_dialogs(topLevel,&dd);
     xp_create_showdb_dialog(topLevel);
     xp_create_showig_dialog(topLevel);
     xp_create_show_cond_dialog(topLevel);
     xp_create_show_intention_dialog(topLevel);
     xp_create_filesel(topLevel);
     xp_create_rop_dialog(topLevel, &dd);
     xp_information_create(topLevel);
     create_syntax_error_dialog(topLevel);

     XtRealizeWidget(topLevel);

     dd.window = XtWindow(dd.canvas);

     n = 0;
     XtSetArg(args[n], XmNforeground, &globalFore); n++;
     XtSetArg(args[n], XmNbackground, &globalBack); n++;
     XtGetValues(oprsActiveDButton, args, n);

     create_gc(&dd);

     idd_create_gc(&idd);

     sprintf(welcome, LG_STR("\n\t\tThis is X-OpenPRS (%s).\n\t%s\n\n",
			     "\n\t\tThis is X-OpenPRS (%s).\n\t%s\n\n"),
	     package_version, COPYRIGHT_STRING);

     AppendTextWindow(textWindow,welcome,FALSE);
     AppendTextWindow(textWindow, "X-OpenPRS comes with ABSOLUTELY NO WARRANTY.\n\n",FALSE);

     sprintf(title, LG_STR("Welcome to X-OpenPRS %s %s",
			   "Welcome to X-OpenPRS %s %s"), 
	     package_version, COPYRIGHT_STRING);
     UpdateMessageWindow(title);

     call_oprs_cat(log_file,textWindow);

     idd.ig = oprs->intention_graph;
     register_main_loop(oprs);

     update_active_idle((XtPointer)oprs,(XtIntervalId)NULL);

     start_kernel_hook(name);

     run_initial_commands();

     sprintf(title, "X-OPRS %s", package_version);
     UpdateMessageWindow(title);

     XtAppMainLoop(app_context);
     
     return(1);
}

void ReallyQuit(Widget w, XtPointer client_data, XtPointer call_data)
{

     wrap_up();

     if (Resrcs.printWidgetTree) {
	  print_top_widget(topLevel);
     }

     XtCloseDisplay(XtDisplay(w));
     exit(0); 
}

void update_active_idle(XtPointer oprs_par, XtIntervalId *ignore)
{
 
     Oprs *oprs = (Oprs *)oprs_par;

     set_oprs_active_mode(! (sl_slist_empty(oprs->new_facts) &&
			    sl_slist_empty(oprs->new_goals) && /* We could be more specific... here */
			    sl_slist_empty(oprs->intention_graph->current_intentions)));
     XtAppAddTimeOut(app_context,100,update_active_idle,oprs);
}

void set_button(Widget w)
{   
     Pixel back;
     Arg args[3];
     Cardinal n; 
     XmString xmstring;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtSetArg(args[n],  XmNlabelString, &xmstring); n++;
     XtGetValues(w,args,n);
     
     if (back != globalFore) {	/* not inversed */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, globalFore); n++;
	  XtSetArg(args[n], XmNforeground, globalBack); n++;
	  XtSetArg(args[n], XmNlabelString, xmstring); n++;
	  XtSetValues(w,args,n);
     }
}

void unset_button(Widget w)
{
     Pixel back;
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtGetValues(w,args,n);
     
     if (back != globalBack) {	/* not inversed */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, globalBack); n++;
	  XtSetArg(args[n], XmNforeground, globalFore); n++;
	  XtSetValues(w,args,n);
     }
}

void toggle_button(Widget w)
{
     Pixel back,fore;
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtSetArg(args[n], XmNforeground, &fore); n++;
     XtGetValues(w,args,n);

     n = 0;
     XtSetArg(args[n], XmNbackground, fore); n++;
     XtSetArg(args[n], XmNforeground, back); n++;
     XtSetValues(w,args,n);
     
}

void ClearMessageWindow(void)
{
     Arg args[1];
     XmString xmres;

     XtSetArg(args[0], XmNlabelString, xmres = XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET));
     XtSetValues(messageWindow, args, 1);
     XmStringFree(xmres);
}

void UpdateMessageWindow(char *string)
{
     Arg args[1];
     XmString xmres;
     XmString x1,x2;

     xmres = XmStringCreate(" ", XmSTRING_DEFAULT_CHARSET);
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET));
     XmStringFree(x1);
     XmStringFree(x2);

     XtSetArg(args[0], XmNlabelString, xmres);
     XtSetValues(messageWindow, args, 1);
     XmStringFree(xmres);
}

#ifdef DLSYMOPENCLOSE_UNDEFINED
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
