/*                               -*- Mode: C -*- 
 * clock-bboard.c -- 
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

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>
#include <Xm/PushBG.h>
#include <Xm/DrawnB.h>
#include <Xm/Label.h>

#include "constant.h"
#include "demo.h"

#include "demo_f.h"

/* For Attachment */
Widget clockBBoardFrame;

Widget master_label, timer_label, speed_label, modeRunDButton, modeHaltDButton;

void SpeedFaster(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_demo_speed(FASTER);
}

void SpeedSlower(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_demo_speed(SLOWER);
}

void ModeRun(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_run_mode(RUN);
}

void ModeHalt(Widget w, XtPointer client_data, XtPointer call_data)
{
     change_run_mode(HALT);
}


void make_clock_bboard(Widget parent)
{
     Widget  clockBBoard, clockBBoardlabel, clockBBoardsize;
     Widget master_title_label, timer_title_label, speed_title_label;
     Widget speedMenu, speedFasterButton, speedSlowerButton;
     Widget modeMenu;

     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     clockBBoardFrame =  XmCreateFrame(parent, "clockBBoardFrame", args, n);
     XtManageChild(clockBBoardFrame);

     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     clockBBoard = XmCreateBulletinBoard(clockBBoardFrame, "clockBBoard", args, n);
     XtManageChild(clockBBoard);
     n = 0;
     clockBBoardlabel = XmCreateLabel(clockBBoard, "clockBBoardlabel", args, n);
     XtManageChild(clockBBoardlabel);
/* empty label for fixing the size */
     clockBBoardsize = XmCreateLabel(clockBBoard, "clockBBoardsize", args, n);
     XtManageChild(clockBBoardsize);

     n = 0;
     master_title_label = XmCreateLabel(clockBBoard, "master_title_label", args, n);
     XtManageChild(master_title_label);
     master_label = XmCreateLabel(clockBBoard, "master_label", args, n);
     XtManageChild(master_label);

     n = 0;
     timer_title_label = XmCreateLabel(clockBBoard, "timer_title_label", args, n);
     XtManageChild(timer_title_label);
     timer_label = XmCreateLabel(clockBBoard, "timer_label", args, n);
     XtManageChild(timer_label);

/*
 * Speed
 */
     speed_title_label = XmCreateLabel(clockBBoard, "speed_title_label", args, n);
     XtManageChild(speed_title_label);
     speed_label = XmCreateLabel(clockBBoard, "speed_label", args, n);
     XtManageChild(speed_label);

     n = 0;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     speedMenu = XmCreateWorkArea(clockBBoard,"speedMenu",args,n);
     XtManageChild(speedMenu);

     speedFasterButton = XmCreatePushButtonGadget(speedMenu,"speedFasterButton", args, n);
     XtAddCallback(speedFasterButton, XmNactivateCallback, SpeedFaster, 0);
     XtManageChild(speedFasterButton);

     speedSlowerButton = XmCreatePushButtonGadget(speedMenu,"speedSlowerButton", args, n);
     XtAddCallback(speedSlowerButton, XmNactivateCallback, SpeedSlower, 0);
     XtManageChild(speedSlowerButton);

/*
 * Mode
 */
     n = 0;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     modeMenu = XmCreateWorkArea(clockBBoard,"modeMenu",args,n);
     XtManageChild(modeMenu);

     n = 0;
     modeRunDButton = XmCreateDrawnButton(modeMenu,"modeRunDButton", args, n);
     XtAddCallback(modeRunDButton, XmNactivateCallback, ModeRun, 0);
     XtManageChild(modeRunDButton);

     modeHaltDButton = XmCreateDrawnButton(modeMenu,"modeHaltDButton", args, n);
     XtAddCallback(modeHaltDButton, XmNactivateCallback, ModeHalt, 0);
     XtManageChild(modeHaltDButton);
}
