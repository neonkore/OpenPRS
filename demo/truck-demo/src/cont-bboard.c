/*                               -*- Mode: C -*- 
 * cont-bboard.c -- 
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

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawnB.h>
#include <Xm/Label.h>

/* For Attachment */
Widget controlBBoardFrame;


Widget  sensor1_movingDButton, sensor1_closeDButton, sensor1_openDButton;
Widget  sensor2_movingDButton, sensor2_closeDButton, sensor2_openDButton;
Widget  truck_sensor_busyDButton, truck_sensor_freeDButton;
Widget  filling_control_fillingDButton, filling_control_emptyDButton, filling_control_fullDButton;

extern Pixmap bp_pixmap;

void make_control_bboard(Widget parent)
{
     Widget  controlBBoard, controlBBoardlabel1, controlBBoardlabel2, controlBBoardsize;

     Widget  sensor1_label,  sensor1_valveMenuFrame, sensor1_valveMenu;
     Widget  sensor2_label,  sensor2_valveMenuFrame, sensor2_valveMenu;
     Widget  filling_control_label,  filling_controlMenuFrame, filling_controlMenu;
     Widget  truck_sensor_label, truck_sensorMenuFrame, truck_sensor_Menu;

     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     controlBBoardFrame =  XmCreateFrame(parent, "controlBBoardFrame", args, n);
     XtManageChild(controlBBoardFrame);

     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     controlBBoard = XmCreateBulletinBoard(controlBBoardFrame, "controlBBoard", args, n);
     XtManageChild(controlBBoard);

     n = 0;
     controlBBoardlabel1 = XmCreateLabel(controlBBoard, "controlBBoardlabel1", args, n);
     XtManageChild(controlBBoardlabel1);
     controlBBoardlabel2 = XmCreateLabel(controlBBoard, "controlBBoardlabel2", args, n);
     XtManageChild(controlBBoardlabel2);
/* empty label for fixing the size */
     controlBBoardsize = XmCreateLabel(controlBBoard, "controlBBoardsize", args, n);
     XtManageChild(controlBBoardsize);

     sensor1_label = XmCreateLabel(controlBBoard, "sensor1_label", args, n);
     XtManageChild(sensor1_label);
     sensor2_label = XmCreateLabel(controlBBoard, "sensor2_label", args, n);
     XtManageChild(sensor2_label);
     filling_control_label = XmCreateLabel(controlBBoard, "filling_control_label", args, n);
     XtManageChild(filling_control_label);
     truck_sensor_label = XmCreateLabel(controlBBoard, "truck_sensor_label", args, n);
     XtManageChild(truck_sensor_label);

/*
 * Sensor 1
 */
     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.talkback_bg); n++;
     sensor1_valveMenuFrame = XmCreateFrame(controlBBoard,"sensor1_valveMenuFrame",args,n);
     XtManageChild(sensor1_valveMenuFrame);

     n = 1;
     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     sensor1_valveMenu = XmCreateWorkArea(sensor1_valveMenuFrame,"sensor1_valveMenu",args,n);
     XtManageChild(sensor1_valveMenu);

     n = 1;
     XtSetArg(args[n], XmNtraversalOn, False); n++;
     sensor1_openDButton = XmCreateDrawnButton(sensor1_valveMenu,"sensor1_openDButton", args, n);
     XtManageChild(sensor1_openDButton);

     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, bp_pixmap); n++;
     sensor1_movingDButton = XmCreateDrawnButton(sensor1_valveMenu,"sensor1_movingDButton", args, n);
     XtManageChild(sensor1_movingDButton);

     n = 2;
     sensor1_closeDButton = XmCreateDrawnButton(sensor1_valveMenu,"sensor1_closeDButton", args, n);
     XtManageChild(sensor1_closeDButton);

/*
 * Sensor 2
 */
     n = 1;
     sensor2_valveMenuFrame = XmCreateFrame(controlBBoard,"sensor2_valveMenuFrame",args,n);
     XtManageChild(sensor2_valveMenuFrame);

     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     sensor2_valveMenu = XmCreateWorkArea(sensor2_valveMenuFrame,"sensor2_valveMenu",args,n);
     XtManageChild(sensor2_valveMenu);

     n = 1;
     XtSetArg(args[n], XmNtraversalOn, False); n++;
     sensor2_openDButton = XmCreateDrawnButton(sensor2_valveMenu,"sensor2_openDButton", args, n);
     XtManageChild(sensor2_openDButton);

     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, bp_pixmap); n++;
     sensor2_movingDButton = XmCreateDrawnButton(sensor2_valveMenu,"sensor2_movingDButton", args, n);
     XtManageChild(sensor2_movingDButton);

     n =2;
     sensor2_closeDButton = XmCreateDrawnButton(sensor2_valveMenu,"sensor2_closeDButton", args, n);
     XtManageChild(sensor2_closeDButton);

/*
 * Filling_control Talkback
 */
     n = 1;
     filling_controlMenuFrame = XmCreateFrame(controlBBoard, "filling_controlMenuFrame", args, n);
     XtManageChild(filling_controlMenuFrame);

     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
     filling_controlMenu = XmCreateWorkArea(filling_controlMenuFrame,"filling_controlMenu",args,n);
     XtManageChild(filling_controlMenu);

     n = 1;
     XtSetArg(args[n], XmNtraversalOn, False); n++;
     filling_control_fullDButton = XmCreateDrawnButton(filling_controlMenu,"filling_control_fullDButton", args, n);
     XtManageChild(filling_control_fullDButton);

     XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
     XtSetArg(args[n], XmNlabelPixmap, bp_pixmap); n++;
     filling_control_fillingDButton = XmCreateDrawnButton(filling_controlMenu,"filling_control_fillingDButton", args, n);
     XtManageChild(filling_control_fillingDButton);

     n = 2;
     filling_control_emptyDButton = XmCreateDrawnButton(filling_controlMenu,"filling_control_emptyDButton", args, n);
     XtManageChild(filling_control_emptyDButton);

/*
 * Truck in place
 */
     n = 1;
     truck_sensorMenuFrame = XmCreateFrame(controlBBoard, "truck_sensorMenuFrame", args, n);
     XtManageChild(truck_sensorMenuFrame);

     XtSetArg(args[n], XmNisAligned, True); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     truck_sensor_Menu = XmCreateWorkArea(truck_sensorMenuFrame, "truck_sensor_Menu", args, n);
     XtManageChild(truck_sensor_Menu);

     n = 1;
     XtSetArg(args[n], XmNtraversalOn, False); n++;
     truck_sensor_busyDButton = XmCreateDrawnButton(truck_sensor_Menu, "truck_sensor_busyDButton", args, n);
     XtManageChild(truck_sensor_busyDButton);

     truck_sensor_freeDButton = XmCreateDrawnButton(truck_sensor_Menu, "truck_sensor_freeDButton", args, n);
     XtManageChild(truck_sensor_freeDButton);

}
