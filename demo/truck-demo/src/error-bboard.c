/*                               -*- Mode: C -*- 
 * error-bboard.c -- 
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
#include "demo-time.h"

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>
#include <Xm/DrawnB.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Xm/LabelG.h>

#include <Xm/PushBG.h>	
#include <Xm/CascadeB.h>

/* For Attachment */
Widget errorBBoardFrame;

Demo_Boolean change_truck_sensor (Truck_Sensor_Status status);

Widget sensor_truckDButton, fillingSlider;

#ifdef HP_UX
Widget valve_option_Value,  sensor1_option_Value,  sensor2_option_Value;
Widget full_option_Value,  delay_option_Value;
#else
Widget valve_option_Menu,  sensor1_option_Menu,  sensor2_option_Menu,  full_option_Menu,  delay_option_Menu;
Widget valve_option_Default,  sensor1_option_Default,  sensor2_option_Default;
Widget full_option_Default,  delay_option_Default;
#endif /* HP_UX */

void FillingSliderMoved(Widget w, XtPointer client_data, XmScaleCallbackStruct *call_data)
{
     change_filling (call_data->value);

}

void sensor_truck_change (Widget w, XtPointer client_data, XtPointer call_data)
{
     if (demo->tr_sensor->status == FREE) {
	  busy_truck_sensor();
     } else if (demo->tr_sensor->status == BUSY) {
	  free_truck_sensor();
     } else {
	  demo_error (LG_STR("sensor_truck_change: unknown status.",
			     "sensor_truck_change: état inconnu. "));
     }
}

void valve_option_cb( Widget menu_item, int item_no, XmAnyCallbackStruct *cbs)
{
     switch (item_no){
     case 0:
	  demo->valve->moving_response =0;
	  break;
     case 1:
	  demo->valve->moving_response =2;
	  break;
     case 2:
	  demo->valve->moving_response =3;
	  break;
     case 3:
	  demo->valve->moving_response =6;
	  break;
     case 4:
	  demo->valve->moving_response = BLOCKED_VALUE;
	  break;
     default:
	  demo_error (LG_STR("valve_option_cb: unknown value.",
			     "valve_option_cb: vanne inconnue."));

     }
#ifdef HP_UX

     if (item_no < 4) {
	  char string[STRSIZE];
	  XmString xmstr;
     
	  sprintf (string,"%7d  ", demo->valve->moving_response);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( valve_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
	  
     } else {
	  XmString xmstr;

	  xmstr = XmStringCreateSimple(LG_STR("blocked  ",
					      "bloquée  "));
	  XtVaSetValues( valve_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
     }
#endif /* HP_UX */
}

void change_sensor_checking_time(Sensor *sensor, int item_no)
{
     Demo_Time time = get_demo_time();

     switch (item_no){
     case 0:
	  sensor->checking =0;
	  break;
     case 1:
	  sensor->checking =1;
	  break;
     case 2:
	  sensor->checking =2;
	  break;
     case 3:
	  sensor->checking =6;
	  break;
     case 4:
	  sensor->checking =8;
	  break;
     case 5:
	  sensor->checking = BLOCKED_VALUE;
	  break;
     default:
	  demo_error (LG_STR("sensor1_option_cb: unknown value.",
			     "sensor1_option_cb: valeur inconnue."));

     }
     if (item_no == 5)
	  sensor->wake_up = -1;
     else
	  sensor->wake_up = time + sensor->checking;
}

void sensor1_option_cb( Widget menu_item, int item_no, XmAnyCallbackStruct *cbs)
{
     change_sensor_checking_time(demo->sensor1, item_no);

#ifdef HP_UX

     if (item_no < 5) {
	  char string[STRSIZE];
	  XmString xmstr;
     
	  sprintf (string,"%7d  ", demo->sensor1->checking);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( sensor1_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
	  
     } else {
	  XmString xmstr;

	  xmstr = XmStringCreateSimple(LG_STR("blocked  ",
					      "bloqué   "));
	  XtVaSetValues( sensor1_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
     }
#endif /* HP_UX */
}

void sensor2_option_cb( Widget menu_item, int item_no, XmAnyCallbackStruct *cbs)
{
     change_sensor_checking_time(demo->sensor2, item_no);
#ifdef HP_UX

     if (item_no < 5) {
	  char string[STRSIZE];
	  XmString xmstr;
     
	  sprintf (string,"%7d  ", demo->sensor2->checking);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( sensor2_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
	  
     } else {
	  XmString xmstr;

	  xmstr = XmStringCreateSimple(LG_STR("blocked  ",
					      "bloqué   "));
	  XtVaSetValues( sensor2_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
     }
#endif /* HP_UX */
}

void full_option_cb( Widget menu_item, int item_no, XmAnyCallbackStruct *cbs)
{
     switch (item_no){
     case 0:
	  demo->tank->full_tank =2;
	  break;
     case 1:
	  demo->tank->full_tank =4;
	  break;
     case 2:
	  demo->tank->full_tank =6;
	  break;
     case 3:
	  demo->tank->full_tank =8;
	  break;
     case 4:
	  demo->tank->full_tank =9;
	  break;
     case 5:
	  demo->tank->full_tank = 10;
	  break;
     default:
	  demo_error (LG_STR("full_option_cb : unknown value",
			     "full_option_cb : unknown value"));

     }
#ifdef HP_UX

     if (item_no < 6) {
	  char string[STRSIZE];
	  XmString xmstr;
     
	  sprintf (string,"%7d  ", demo->tank->full_tank);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( full_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
	  
     }
#endif /* HP_UX */
}

void delay_option_cb( Widget menu_item, int item_no, XmAnyCallbackStruct *cbs)
{
     switch (item_no){
     case 0:
	  demo->tank->filling_response =1;
	  break;
     case 1:
	  demo->tank->filling_response =3;
	  break;
     case 2:
	  demo->tank->filling_response =5;
	  break;
     case 3:
	  demo->tank->filling_response =10;
	  break;
     case 4:
	  demo->tank->filling_response =15;
	  break;
     case 5:
	  demo->tank->filling_response = 20;
	  break;
     default:
	  demo_error (LG_STR("delay_option_cb : unknown value",
			     "delay_option_cb : unknown value"));

     }
#ifdef HP_UX

     if (item_no < 6) {
	  char string[STRSIZE];
	  XmString xmstr;
     
	  sprintf (string,"%7d  ", demo->tank->filling_response);
	  xmstr = XmStringCreateSimple(string);
	  XtVaSetValues( delay_option_Value, XmNlabelString, xmstr, NULL);
	  XmStringFree(xmstr);
     }
#endif /* HP_UX */
}

void make_error_bboard(Widget parent)
{
     Widget  errorBBoard, errorBBoardsize, errorBBoardlabel1;
     Widget  sensor_truck_MenuFrame, sensor_truck_Menu;
     Widget valve_option_form, valve_option_label;
     Widget sensor1_option_form, sensor1_option_label;
     Widget sensor2_option_form, sensor2_option_label;
     Widget filling_option_form, full_option_label, delay_option_label;

     Widget valve_option_frame;
     Widget options_Menu;

#ifdef HP_UX    
     Widget valve_option_Menu,  sensor1_option_Menu,  sensor2_option_Menu;
     Widget full_option_Menu,  delay_option_Menu;
     Widget valve_submenu, valve_button, valve_0, valve_1, valve_2, valve_3, valve_4;
     Widget sensor1_submenu, sensor1_button, sensor1_0, sensor1_1, sensor1_2, sensor1_3, sensor1_4, sensor1_5;
     Widget sensor2_submenu, sensor2_button, sensor2_0, sensor2_1, sensor2_2, sensor2_3, sensor2_4, sensor2_5;
     Widget full_submenu, full_button, full_0, full_1, full_2, full_3, full_4, full_5;
     Widget delay_submenu, delay_button, delay_0, delay_1, delay_2, delay_3, delay_4, delay_5;

     XmString val_default;
#endif

     int i;
     Cardinal n;
     Arg args[MAXARGS];

     XmString opt_label, val0, val1, val2, val3, val4, val5;

     n = 0;
     errorBBoardFrame =  XmCreateFrame(parent, "errorBBoardFrame", args, n);
     XtManageChild(errorBBoardFrame);

     n = 0;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     errorBBoard = XmCreateBulletinBoard(errorBBoardFrame, "errorBBoard", args, n);
     XtManageChild(errorBBoard);

     n = 0;
/* empty label for fixing the size */
     errorBBoardsize = XmCreateLabel(errorBBoard, "errorBBoardsize", args, n);
     XtManageChild(errorBBoardsize);

     errorBBoardlabel1 = XmCreateLabel(errorBBoard, "errorBBoardlabel1", args, n);
     XtManageChild(errorBBoardlabel1);

/* 
 * Options Menu
 */

/* valve */
     n = 0;
     valve_option_form = XmCreateForm(errorBBoard, "valve_option_form", args, n);
     XtManageChild(valve_option_form);

     n = 0;
     valve_option_label = XmCreateLabel(valve_option_form, "valve_option_label", args, n);
     XtManageChild(valve_option_label);

     opt_label = XmStringCreateSimple(LG_STR("Time :",
					     "Temps :"));
     val0 = XmStringCreateSimple("0");
     val1 = XmStringCreateSimple("2");
     val2 = XmStringCreateSimple("3");
     val3 = XmStringCreateSimple("6");
     val4 = XmStringCreateSimple(LG_STR("blocked",
					"bloquée"));



#ifdef HP_UX 
     val_default = XmStringCreateSimple("      3  ");

     n = 0;
     valve_option_Menu =  XmCreateMenuBar(valve_option_form, "valve_option_Menu", args, n);
     XtManageChild(valve_option_Menu);

     valve_submenu = XmCreatePulldownMenu(valve_option_Menu, "valve_submenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, opt_label); n++;
     XtSetArg(args[n], XmNsubMenuId, valve_submenu); n++;
     valve_button = XmCreateCascadeButton(valve_option_Menu, "valve_button", args, n);
     XtManageChild(valve_button);

     n =0;
     XtSetArg(args[n], XmNlabelString, val0); n++;
     valve_0 =  XmCreatePushButtonGadget(valve_submenu, "valve_0", args, n);
     XtAddCallback(valve_0, XmNactivateCallback,  (XtCallbackProc)valve_option_cb, (XtPointer) 0);
     XtManageChild(valve_0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val1); n++;
     valve_1 =  XmCreatePushButtonGadget(valve_submenu, "valve_1", args, n);
     XtAddCallback(valve_1, XmNactivateCallback,  (XtCallbackProc)valve_option_cb, (XtPointer) 1);
     XtManageChild(valve_1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val2); n++;
     valve_2 =  XmCreatePushButtonGadget(valve_submenu, "valve_2", args, n);
     XtAddCallback(valve_2, XmNactivateCallback,  (XtCallbackProc)valve_option_cb, (XtPointer) 2);
     XtManageChild(valve_2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val3); n++;
     valve_3 =  XmCreatePushButtonGadget(valve_submenu, "valve_3", args, n);
     XtAddCallback(valve_3, XmNactivateCallback,  (XtCallbackProc)valve_option_cb, (XtPointer) 3);
     XtManageChild(valve_3);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val4); n++;
     valve_4 =  XmCreatePushButtonGadget(valve_submenu, "valve_4", args, n);
     XtAddCallback(valve_4, XmNactivateCallback,  (XtCallbackProc)valve_option_cb, (XtPointer) 4);
     XtManageChild(valve_4);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, valve_option_Menu); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, valve_option_label); n++;
     XtSetArg(args[n], XmNlabelString, val_default); n++;
     valve_option_Value = XmCreateLabel(valve_option_form, "valve_option_Value", args, n);
     XtManageChild(valve_option_Value);

#else /* HP_UX */

     valve_option_Menu = XmVaCreateSimpleOptionMenu
	  (valve_option_form, "valve_option_Menu",
	   opt_label, (KeySym)NULL, 2, (XtCallbackProc)valve_option_cb,

	   XmVaPUSHBUTTON, val0, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val1, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val2, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val3, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val4, NULL, NULL, NULL,
	   NULL);

     XtManageChild(valve_option_Menu);

#endif /* HP_UX */

     XmStringFree(opt_label);
     XmStringFree(val0);
     XmStringFree(val1);
     XmStringFree(val2);
     XmStringFree(val3);
     XmStringFree(val4);

/* sensor1 */
     n = 0;
     sensor1_option_form = XmCreateForm(errorBBoard, "sensor1_option_form", args, n);
     XtManageChild(sensor1_option_form);

     n = 0;
     sensor1_option_label = XmCreateLabel(sensor1_option_form, "sensor1_option_label", args, n);
     XtManageChild(sensor1_option_label);

     opt_label = XmStringCreateSimple(LG_STR("Time :",
					     "Temps :"));
     val0 = XmStringCreateSimple("0");
     val1 = XmStringCreateSimple("1");
     val2 = XmStringCreateSimple("2");
     val3 = XmStringCreateSimple("6");
     val4 = XmStringCreateSimple("8");
     val5 = XmStringCreateSimple(LG_STR("blocked",
					"bloqué"));

#ifdef HP_UX
     val_default = XmStringCreateSimple("      1  ");

     n = 0;
     sensor1_option_Menu =  XmCreateMenuBar(sensor1_option_form, "sensor1_option_Menu", args, n);
     XtManageChild(sensor1_option_Menu);

     sensor1_submenu = XmCreatePulldownMenu(sensor1_option_Menu, "sensor1_submenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, opt_label); n++;
     XtSetArg(args[n], XmNsubMenuId, sensor1_submenu); n++;
     sensor1_button = XmCreateCascadeButton(sensor1_option_Menu, "sensor1_button", args, n);
     XtManageChild(sensor1_button);

     n =0;
     XtSetArg(args[n], XmNlabelString, val0); n++;
     sensor1_0 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_0", args, n);
     XtAddCallback(sensor1_0, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 0);
     XtManageChild(sensor1_0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val1); n++;
     sensor1_1 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_1", args, n);
     XtAddCallback(sensor1_1, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 1);
     XtManageChild(sensor1_1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val2); n++;
     sensor1_2 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_2", args, n);
     XtAddCallback(sensor1_2, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 2);
     XtManageChild(sensor1_2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val3); n++;
     sensor1_3 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_3", args, n);
     XtAddCallback(sensor1_3, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 3);
     XtManageChild(sensor1_3);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val4); n++;
     sensor1_4 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_4", args, n);
     XtAddCallback(sensor1_4, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 4);
     XtManageChild(sensor1_4);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val5); n++;
     sensor1_5 =  XmCreatePushButtonGadget(sensor1_submenu, "sensor1_5", args, n);
     XtAddCallback(sensor1_5, XmNactivateCallback,  (XtCallbackProc)sensor1_option_cb, (XtPointer) 5);
     XtManageChild(sensor1_5);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, sensor1_option_Menu); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, sensor1_option_label); n++;
     XtSetArg(args[n], XmNlabelString, val_default); n++;
     sensor1_option_Value = XmCreateLabel(sensor1_option_form, "sensor1_option_Value", args, n);
     XtManageChild(sensor1_option_Value);

#else /* HP_UX */
     sensor1_option_Menu = XmVaCreateSimpleOptionMenu
	  (sensor1_option_form, "sensor1_option_Menu",
	   opt_label, (KeySym)NULL, 1, (XtCallbackProc)sensor1_option_cb,
	   XmVaPUSHBUTTON, val0, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val1, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val2, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val3, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val4, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val5, NULL, NULL, NULL,
	   NULL);

     XtManageChild(sensor1_option_Menu);

#endif /* HP_UX */

     XmStringFree(opt_label);
     XmStringFree(val0);
     XmStringFree(val1);
     XmStringFree(val2);
     XmStringFree(val3);
     XmStringFree(val4);
     XmStringFree(val5);

/* sensor2 */
     n = 0;
     sensor2_option_form = XmCreateForm(errorBBoard, "sensor2_option_form", args, n);
     XtManageChild(sensor2_option_form);

     n = 0;
     sensor2_option_label = XmCreateLabel(sensor2_option_form, "sensor2_option_label", args, n);
     XtManageChild(sensor2_option_label);

     opt_label = XmStringCreateSimple(LG_STR("Time :",
					     "Temps :"));
     val0 = XmStringCreateSimple("0");
     val1 = XmStringCreateSimple("1");
     val2 = XmStringCreateSimple("2");
     val3 = XmStringCreateSimple("6");
     val4 = XmStringCreateSimple("8");
     val5 = XmStringCreateSimple(LG_STR("blocked",
					"bloqué"));

#ifdef HP_UX
     val_default = XmStringCreateSimple("      2  ");

     n = 0;
     sensor2_option_Menu =  XmCreateMenuBar(sensor2_option_form, "sensor2_option_Menu", args, n);
     XtManageChild(sensor2_option_Menu);

     sensor2_submenu = XmCreatePulldownMenu(sensor2_option_Menu, "sensor2_submenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, opt_label); n++;
     XtSetArg(args[n], XmNsubMenuId, sensor2_submenu); n++;
     sensor2_button = XmCreateCascadeButton(sensor2_option_Menu, "sensor2_button", args, n);
     XtManageChild(sensor2_button);

     n =0;
     XtSetArg(args[n], XmNlabelString, val0); n++;
     sensor2_0 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_0", args, n);
     XtAddCallback(sensor2_0, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 0);
     XtManageChild(sensor2_0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val1); n++;
     sensor2_1 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_1", args, n);
     XtAddCallback(sensor2_1, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 1);
     XtManageChild(sensor2_1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val2); n++;
     sensor2_2 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_2", args, n);
     XtAddCallback(sensor2_2, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 2);
     XtManageChild(sensor2_2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val3); n++;
     sensor2_3 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_3", args, n);
     XtAddCallback(sensor2_3, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 3);
     XtManageChild(sensor2_3);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val4); n++;
     sensor2_4 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_4", args, n);
     XtAddCallback(sensor2_4, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 4);
     XtManageChild(sensor2_4);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val5); n++;
     sensor2_5 =  XmCreatePushButtonGadget(sensor2_submenu, "sensor2_5", args, n);
     XtAddCallback(sensor2_5, XmNactivateCallback,  (XtCallbackProc)sensor2_option_cb, (XtPointer) 5);
     XtManageChild(sensor2_5);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, sensor2_option_Menu); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, sensor2_option_label); n++;
     XtSetArg(args[n], XmNlabelString, val_default); n++;
     sensor2_option_Value = XmCreateLabel(sensor2_option_form, "sensor2_option_Value", args, n);
     XtManageChild(sensor2_option_Value);

#else /* HP_UX */
     sensor2_option_Menu = XmVaCreateSimpleOptionMenu
	  (sensor2_option_form, "sensor2_option_Menu",
	   opt_label, (KeySym)NULL, 2, (XtCallbackProc)sensor2_option_cb,
	   XmVaPUSHBUTTON, val0, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val1, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val2, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val3, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val4, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val5, NULL, NULL, NULL,
	   NULL);

     XtManageChild(sensor2_option_Menu);

#endif /* HP_UX */

     XmStringFree(opt_label);
     XmStringFree(val0);
     XmStringFree(val1);
     XmStringFree(val2);
     XmStringFree(val3);
     XmStringFree(val4);
     XmStringFree(val5);

/* filling */
     n = 0;
     filling_option_form = XmCreateForm(errorBBoard, "filling_option_form", args, n);
     XtManageChild(filling_option_form);

     n = 0;
     full_option_label = XmCreateLabel(filling_option_form, "full_option_label", args, n);
     XtManageChild(full_option_label);

     opt_label = XmStringCreateSimple(LG_STR("Value :",
					     "Valeur :"));
     val0 = XmStringCreateSimple("2");
     val1 = XmStringCreateSimple("4");
     val2 = XmStringCreateSimple("6");
     val3 = XmStringCreateSimple("8");
     val4 = XmStringCreateSimple("9");
     val5 = XmStringCreateSimple("10");

#ifdef HP_UX
     val_default = XmStringCreateSimple("      8  ");

     n = 0;
     full_option_Menu =  XmCreateMenuBar(filling_option_form, "full_option_Menu", args, n);
     XtManageChild(full_option_Menu);

     full_submenu = XmCreatePulldownMenu(full_option_Menu, "full_submenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, opt_label); n++;
     XtSetArg(args[n], XmNsubMenuId, full_submenu); n++;
     full_button = XmCreateCascadeButton(full_option_Menu, "full_button", args, n);
     XtManageChild(full_button);

     n =0;
     XtSetArg(args[n], XmNlabelString, val0); n++;
     full_0 =  XmCreatePushButtonGadget(full_submenu, "full_0", args, n);
     XtAddCallback(full_0, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 0);
     XtManageChild(full_0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val1); n++;
     full_1 =  XmCreatePushButtonGadget(full_submenu, "full_1", args, n);
     XtAddCallback(full_1, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 1);
     XtManageChild(full_1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val2); n++;
     full_2 =  XmCreatePushButtonGadget(full_submenu, "full_2", args, n);
     XtAddCallback(full_2, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 2);
     XtManageChild(full_2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val3); n++;
     full_3 =  XmCreatePushButtonGadget(full_submenu, "full_3", args, n);
     XtAddCallback(full_3, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 3);
     XtManageChild(full_3);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val4); n++;
     full_4 =  XmCreatePushButtonGadget(full_submenu, "full_4", args, n);
     XtAddCallback(full_4, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 4);
     XtManageChild(full_4);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val5); n++;
     full_5 =  XmCreatePushButtonGadget(full_submenu, "full_5", args, n);
     XtAddCallback(full_5, XmNactivateCallback,  (XtCallbackProc)full_option_cb, (XtPointer) 5);
     XtManageChild(full_5);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, full_option_Menu); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, full_option_label); n++;
     XtSetArg(args[n], XmNlabelString, val_default); n++;
     full_option_Value = XmCreateLabel(filling_option_form, "full_option_Value", args, n);
     XtManageChild(full_option_Value);

#else /* HP_UX */
     full_option_Menu = XmVaCreateSimpleOptionMenu
	  (filling_option_form, "full_option_Menu",
	   opt_label, (KeySym)NULL, 3, (XtCallbackProc)full_option_cb,
	   XmVaPUSHBUTTON, val0, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val1, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val2, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val3, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val4, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val5, NULL, NULL, NULL,
	   NULL);

     XtManageChild(full_option_Menu);

#endif /* HP_UX */

     XmStringFree(opt_label);
     XmStringFree(val0);
     XmStringFree(val1);
     XmStringFree(val2);
     XmStringFree(val3);
     XmStringFree(val4);
     XmStringFree(val5);

     n = 0;
     delay_option_label = XmCreateLabel(filling_option_form, "delay_option_label", args, n);
     XtManageChild(delay_option_label);

     opt_label = XmStringCreateSimple(LG_STR("Time :",
					     "Temps :"));
     val0 = XmStringCreateSimple("1");
     val1 = XmStringCreateSimple("3");
     val2 = XmStringCreateSimple("5");
     val3 = XmStringCreateSimple("10");
     val4 = XmStringCreateSimple("15");
     val5 = XmStringCreateSimple("20");

#ifdef HP_UX
     val_default = XmStringCreateSimple("     10  ");

     n = 0;
     delay_option_Menu =  XmCreateMenuBar(filling_option_form, "delay_option_Menu", args, n);
     XtManageChild(delay_option_Menu);

     delay_submenu = XmCreatePulldownMenu(delay_option_Menu, "delay_submenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, opt_label); n++;
     XtSetArg(args[n], XmNsubMenuId, delay_submenu); n++;
     delay_button = XmCreateCascadeButton(delay_option_Menu, "delay_button", args, n);
     XtManageChild(delay_button);

     n =0;
     XtSetArg(args[n], XmNlabelString, val0); n++;
     delay_0 =  XmCreatePushButtonGadget(delay_submenu, "delay_0", args, n);
     XtAddCallback(delay_0, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 0);
     XtManageChild(delay_0);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val1); n++;
     delay_1 =  XmCreatePushButtonGadget(delay_submenu, "delay_1", args, n);
     XtAddCallback(delay_1, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 1);
     XtManageChild(delay_1);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val2); n++;
     delay_2 =  XmCreatePushButtonGadget(delay_submenu, "delay_2", args, n);
     XtAddCallback(delay_2, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 2);
     XtManageChild(delay_2);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val3); n++;
     delay_3 =  XmCreatePushButtonGadget(delay_submenu, "delay_3", args, n);
     XtAddCallback(delay_3, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 3);
     XtManageChild(delay_3);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val4); n++;
     delay_4 =  XmCreatePushButtonGadget(delay_submenu, "delay_4", args, n);
     XtAddCallback(delay_4, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 4);
     XtManageChild(delay_4);

     n = 0;
     XtSetArg(args[n], XmNlabelString, val5); n++;
     delay_5 =  XmCreatePushButtonGadget(delay_submenu, "delay_5", args, n);
     XtAddCallback(delay_5, XmNactivateCallback,  (XtCallbackProc)delay_option_cb, (XtPointer) 5);
     XtManageChild(delay_5);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, delay_option_Menu); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, delay_option_label); n++;
     XtSetArg(args[n], XmNlabelString, val_default); n++;
     delay_option_Value = XmCreateLabel(filling_option_form, "delay_option_Value", args, n);
     XtManageChild(delay_option_Value);

#else /* HP_UX */
     delay_option_Menu = XmVaCreateSimpleOptionMenu
	  (filling_option_form, "delay_option_Menu",
	   opt_label, (KeySym)NULL, 3, (XtCallbackProc)delay_option_cb,
	   XmVaPUSHBUTTON, val0, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val1, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val2, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val3, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val4, NULL, NULL, NULL,
	   XmVaPUSHBUTTON, val5, NULL, NULL, NULL,
	   NULL);

     XtManageChild(delay_option_Menu);

#endif /* HP_UX */

     XmStringFree(opt_label);
     XmStringFree(val0);
     XmStringFree(val1);
     XmStringFree(val2);
     XmStringFree(val3);
     XmStringFree(val4);
     XmStringFree(val5);

#ifndef HP_UX

/* save the default options */

     n =0;
     XtSetArg(args[n], XmNmenuHistory, &valve_option_Default); n++;
     XtGetValues(valve_option_Menu, args, n);
     n =0;
     XtSetArg(args[n], XmNmenuHistory, &sensor1_option_Default); n++;
     XtGetValues(sensor1_option_Menu, args, n);
     n =0;
     XtSetArg(args[n], XmNmenuHistory, &sensor2_option_Default); n++;
     XtGetValues(sensor2_option_Menu, args, n);
     n =0;
     XtSetArg(args[n], XmNmenuHistory, &full_option_Default); n++;
     XtGetValues(full_option_Menu, args, n);
     n =0;
     XtSetArg(args[n], XmNmenuHistory, &delay_option_Default); n++;
     XtGetValues(delay_option_Menu, args, n);
     
#endif /* HP_UX */

/*
 * Sensor_truck
 */
     n = 0;
     sensor_truck_MenuFrame = XmCreateFrame(errorBBoard,"sensor_truck_MenuFrame",args,n);
     XtManageChild(sensor_truck_MenuFrame);

     n = 0;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     sensor_truck_Menu = XmCreateWorkArea(sensor_truck_MenuFrame,"sensor_truck_Menu",args,n);
     XtManageChild(sensor_truck_Menu);

     n = 0;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     sensor_truckDButton = XmCreateDrawnButton(sensor_truck_Menu,"sensor_truckDButton", args, n);
     XtAddCallback(sensor_truckDButton, XmNactivateCallback, sensor_truck_change, 0);
     XtManageChild(sensor_truckDButton);

/*
 * Filling Slider
 */
     n = 0;
     XtSetArg(args[n], XmNminimum, MIN_TANK); n++;
     XtSetArg(args[n], XmNmaximum, MAX_TANK); n++;
     XtSetArg(args[n], XmNshowValue, True); n++;
     fillingSlider = XmCreateScale(errorBBoard,"fillingSlider",args,n);
     XtAddCallback(fillingSlider, XmNvalueChangedCallback, (XtCallbackProc)FillingSliderMoved, NULL);
     XtAddCallback(fillingSlider, XmNdragCallback, (XtCallbackProc)FillingSliderMoved, NULL);

     for ( i = MIN_TANK; i< MAX_TANK; i++)
	  XtVaCreateManagedWidget("-", xmLabelGadgetClass, fillingSlider, NULL);
}

void reset_default_options_menus ()
{ 
     Arg args[1];
     Cardinal n;

#ifdef HP_UX
     char string[STRSIZE];
     XmString xmstr;
     
     sprintf (string,"%7d  ", demo->valve->moving_response);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( valve_option_Value, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
	  
     sprintf (string,"%7d  ", demo->sensor1->checking);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( sensor1_option_Value, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
	  
     sprintf (string,"%7d  ", demo->sensor2->checking);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( sensor2_option_Value, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
	  
     sprintf (string,"%7d  ", demo->tank->full_tank);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( full_option_Value, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
	  
     sprintf (string,"%7d  ", demo->tank->filling_response);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( delay_option_Value, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

#else
     n = 0;
     XtSetArg(args[n], XmNmenuHistory, valve_option_Default); n++;
     XtSetValues(valve_option_Menu, args, n);
     n = 0;
     XtSetArg(args[n], XmNmenuHistory, sensor1_option_Default); n++;
     XtSetValues(sensor1_option_Menu, args, n);
     n = 0;
     XtSetArg(args[n], XmNmenuHistory, sensor2_option_Default); n++;
     XtSetValues(sensor2_option_Menu, args, n);
     n = 0;
     XtSetArg(args[n], XmNmenuHistory, full_option_Default); n++;
     XtSetValues(full_option_Menu, args, n);
     n = 0;
     XtSetArg(args[n], XmNmenuHistory, delay_option_Default); n++;
     XtSetValues(delay_option_Menu, args, n);
#endif /* HP_UX */
}
