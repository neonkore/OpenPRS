/*                               -*- Mode: C -*- 
 * command-bboard.c -- 
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
#include "constant.h"
#include "demo.h"
#include "demo_f.h"

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawnB.h>
#include <Xm/Label.h>

/* For Attachment */
Widget commandBBoardFrame;

Widget switch_valve_openDButton, switch_valve_closeDButton;
Widget emergency_stopDButton, emergency_pix_label;
Widget filling_switch_goDButton, filling_switch_stopDButton;
Widget queue_switch_goDButton, queue_switch_stopDButton;

extern Pixmap disarm_stop_pixmap;

void go_light_filling(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_light(demo->light_filling, GO);
}

void stop_light_filling(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_light(demo->light_filling, STOP);

}

void go_light_queue(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_light(demo->light_queue, GO);

}

void stop_light_queue(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_light(demo->light_queue, STOP);

}

void emergency_activate(Widget w, XtPointer client_data, XtPointer call_data)
{
     emergency_stop();
}

void switch_opened(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_valve(OPENING);
}

void switch_closed(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_valve(CLOSING);
}

void make_command_bboard(Widget parent)
{
     Widget  commandBBoard, commandBBoardsize;
     Widget switch_valve_bb, switch_valve_label;
     Widget switch_valve_Menu;
     Widget emergency_bb, emergency_Menu;
     Widget filling_switch_bb, filling_switch_label, filling_switch_Menu;
     Widget queue_switch_bb, queue_switch_label, queue_switch_Menu;
     
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     commandBBoardFrame =  XmCreateFrame(parent, "commandBBoardFrame", args, n);
     XtManageChild(commandBBoardFrame);

     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     commandBBoard = XmCreateBulletinBoard(commandBBoardFrame, "commandBBoard", args, n);
     XtManageChild(commandBBoard);

     n = 0;
/* empty label for fixing the size */
     commandBBoardsize = XmCreateLabel(commandBBoard, "commandBBoardsize", args, n);
     XtManageChild(commandBBoardsize);

/*
 * Switch_valve
 */
     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.talkback_bg); n++;
     switch_valve_bb = XmCreateBulletinBoard(commandBBoard, "switch_valve_bb", args, n);
     XtManageChild(switch_valve_bb);

     n = 1;
     switch_valve_label = XmCreateLabel(switch_valve_bb, "switch_valve_label", args, n);
     XtManageChild(switch_valve_label);

     n = 1;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     switch_valve_Menu = XmCreateWorkArea(switch_valve_bb, "switch_valve_Menu", args, n);
     XtManageChild(switch_valve_Menu);

     switch_valve_openDButton = XmCreateDrawnButton(switch_valve_Menu, "switch_valve_openDButton", args, n);
     XtAddCallback(switch_valve_openDButton, XmNactivateCallback, switch_opened, 0);
     XtManageChild(switch_valve_openDButton);

     switch_valve_closeDButton = XmCreateDrawnButton(switch_valve_Menu, "switch_valve_closeDButton", args, n);
     XtAddCallback(switch_valve_closeDButton, XmNactivateCallback, switch_closed, 0);
     XtManageChild(switch_valve_closeDButton);

/*
 * Emergency
 */
     n = 1;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     emergency_bb = XmCreateBulletinBoard(commandBBoard, "emergency_bb", args, n);
     XtManageChild(emergency_bb);

     n = 1;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     emergency_Menu = XmCreateWorkArea(emergency_bb,"emergency_Menu",args,n);
     XtManageChild(emergency_Menu);

     n = 1;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     emergency_stopDButton = XmCreateDrawnButton(emergency_Menu,"emergency_stopDButton", args, n);
     XtAddCallback(emergency_stopDButton, XmNactivateCallback, emergency_activate, 0);
     XtManageChild(emergency_stopDButton);

     n = 1;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, disarm_stop_pixmap); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     emergency_pix_label = XmCreateLabel(emergency_bb, "emergency_pix_label", args, n);
     XtManageChild(emergency_pix_label);

/*
 * Filling_switch
 */
     n = 1;
     filling_switch_bb = XmCreateBulletinBoard(commandBBoard, "filling_switch_bb", args, n);
     XtManageChild(filling_switch_bb);
     filling_switch_label = XmCreateLabel(filling_switch_bb, "filling_switch_label", args, n);
     XtManageChild(filling_switch_label);

     n = 1;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     filling_switch_Menu = XmCreateWorkArea(filling_switch_bb, "filling_switch_Menu", args, n);
     XtManageChild(filling_switch_Menu);

     filling_switch_goDButton = XmCreateDrawnButton(filling_switch_Menu, "filling_switch_goDButton", args, n);
     XtAddCallback(filling_switch_goDButton, XmNactivateCallback, go_light_filling, 0);
     XtManageChild(filling_switch_goDButton);

     filling_switch_stopDButton = XmCreateDrawnButton(filling_switch_Menu, "filling_switch_stopDButton", args, n);
     XtAddCallback(filling_switch_stopDButton, XmNactivateCallback, stop_light_filling, 0);
     XtManageChild(filling_switch_stopDButton);

/*
 * Queue_switch
 */
     n = 1;
     queue_switch_bb = XmCreateBulletinBoard(commandBBoard, "queue_switch_bb", args, n);
     XtManageChild(queue_switch_bb);
     queue_switch_label = XmCreateLabel(queue_switch_bb, "queue_switch_label", args, n);
     XtManageChild(queue_switch_label);

     n = 1;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     queue_switch_Menu = XmCreateWorkArea(queue_switch_bb, "queue_switch_Menu", args, n);
     XtManageChild(queue_switch_Menu);

     queue_switch_goDButton = XmCreateDrawnButton(queue_switch_Menu, "queue_switch_goDButton", args, n);
     XtAddCallback(queue_switch_goDButton, XmNactivateCallback, go_light_queue, 0);
     XtManageChild(queue_switch_goDButton);

     queue_switch_stopDButton = XmCreateDrawnButton(queue_switch_Menu, "queue_switch_stopDButton", args, n);
     XtAddCallback(queue_switch_stopDButton, XmNactivateCallback, stop_light_queue, 0);
     XtManageChild(queue_switch_stopDButton);
}


