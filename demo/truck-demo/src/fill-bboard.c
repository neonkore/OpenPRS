/*                               -*- Mode: C -*- 
 * fill-bboard.c -- 
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

extern Pixmap light_pixmap, red_light_on_pixmap, red_light_off_pixmap, green_light_on_pixmap, green_light_off_pixmap;

extern Pixmap waiting_truck_pixmap, valve_pixmap, filling_no_truck_pixmap, tank_pixmap[];

/* For Attachment */
Widget fillingBBoardFrame;

Widget queue_label;
Widget queued_truck_pix_label, filling_truck_pix_label, valve_pix_label, filling_pix_label, tank_pix_label;
Widget  queue_light_stop_pix_label, queue_light_go_pix_label;
Widget  filling_light_stop_pix_label, filling_light_go_pix_label;

void make_filling_bboard(Widget parent)
{
     Widget  fillingBBoard, fillingBBoardlabel;
     Widget  filling_label, queue_title_label;
     Widget  queue_light_pix_label;
     Widget  filling_light_pix_label;
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     fillingBBoardFrame =  XmCreateFrame(parent, "fillingBBoardFrame", args, n);
     XtManageChild(fillingBBoardFrame);

     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     fillingBBoard = XmCreateBulletinBoard(fillingBBoardFrame, "fillingBBoard", args, n);
     XtManageChild(fillingBBoard);

     n = 0;
     fillingBBoardlabel = XmCreateLabel(fillingBBoard, "fillingBBoardlabel", args, n);
     XtManageChild(fillingBBoardlabel);

/*
 * queue
 */
     n = 0;
     queue_title_label = XmCreateLabel(fillingBBoard, "queue_title_label", args, n);
     XtManageChild(queue_title_label);
     queue_label = XmCreateLabel(fillingBBoard, "queue_label", args, n);
     XtManageChild(queue_label);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, tank_pixmap[0]); n++;
     tank_pix_label = XmCreateLabel(fillingBBoard, "tank_pix_label", args, n);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, waiting_truck_pixmap); n++;
     queued_truck_pix_label = XmCreateLabel(fillingBBoard, "queued_truck_pix_label", args, n);
     filling_truck_pix_label = XmCreateLabel(fillingBBoard, "filling_truck_pix_label", args, n);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, valve_pixmap); n++;
     valve_pix_label = XmCreateLabel(fillingBBoard, "valve_pix_label", args, n);
     XtManageChild(valve_pix_label);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, filling_no_truck_pixmap); n++;
     filling_pix_label = XmCreateLabel(fillingBBoard, "filling_pix_label", args, n);

/* lights */
     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, red_light_on_pixmap); n++;
     queue_light_stop_pix_label = XmCreateLabel(fillingBBoard, "queue_light_stop_pix_label", args, n);
     XtManageChild(queue_light_stop_pix_label);
     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, green_light_off_pixmap); n++;
     queue_light_go_pix_label = XmCreateLabel(fillingBBoard, "queue_light_go_pix_label", args, n);
     XtManageChild(queue_light_go_pix_label);
     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, light_pixmap); n++;
     queue_light_pix_label = XmCreateLabel(fillingBBoard, "queue_light_pix_label", args, n);
     XtManageChild(queue_light_pix_label);

     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, red_light_on_pixmap); n++;
     filling_light_stop_pix_label = XmCreateLabel(fillingBBoard, "filling_light_stop_pix_label", args, n);
     XtManageChild(filling_light_stop_pix_label);
     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, green_light_off_pixmap); n++;
     filling_light_go_pix_label = XmCreateLabel(fillingBBoard, "filling_light_go_pix_label", args, n);
     XtManageChild(filling_light_go_pix_label);
     n = 0;
     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, light_pixmap); n++;
     filling_light_pix_label = XmCreateLabel(fillingBBoard, "filling_light_pix_label", args, n);
     XtManageChild(filling_light_pix_label);

}

