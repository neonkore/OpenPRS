/*                               -*- Mode: C -*- 
 * util.c -- 
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

#include "demo.h"
#include "demo_f.h"
#include "bitmaps.h"

Pixmap valve_pixmap, valve_open_pixmap;
Pixmap light_pixmap, red_light_on_pixmap, red_light_off_pixmap, green_light_on_pixmap, green_light_off_pixmap;
Pixmap bp_pixmap;
Pixmap waiting_truck_pixmap, ready_truck_pixmap, leaving_truck_pixmap;
Pixmap waiting_truck_filled_pixmap, ready_truck_filled_pixmap, leaving_truck_filled_pixmap;
Pixmap filling_no_truck_pixmap;
Pixmap tank_pixmap[11];
Pixmap filled_tank_pixmap[12];
 
Pixmap disarm_stop_pixmap, armed_stop_pixmap;

extern Widget messageTextWindow, warningTextWindow, oprsTextWindow;
extern Widget valve_pix_label, filling_pix_label, filling_truck_pix_label, tank_pix_label;

void demo_error (char *s)
{
     printf(LG_STR("Error : %s \n",
		   "Error : %s \n"),s);
}

void demo_warning (char *s)
{
     XBell(XtDisplay( warningTextWindow), 100);

     appendTextWindow ( warningTextWindow, s);
}

void demo_message (char *s)
{
     appendTextWindow ( messageTextWindow, s);
}

void oprs_message (char *s)
{
     appendTextWindow (oprsTextWindow, s);
}


void set_button(Widget w)
{
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.global_fg); n++;
     XtSetArg(args[n], XmNforeground, app_data.global_bg); n++;
     XtSetValues(w,args,n);
}

void unset_button(Widget w)
{
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.global_bg); n++;
     XtSetArg(args[n], XmNforeground, app_data.global_fg); n++;
     XtSetValues(w,args,n);
}

void set_barber_button(Widget w)
{
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.barber_fg); n++;
     XtSetArg(args[n], XmNforeground, app_data.barber_bg); n++;
     XtSetValues(w,args,n);
}

void unset_barber_button(Widget w)
{
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.barber_bg); n++;
     XtSetArg(args[n], XmNforeground, app_data.barber_fg); n++;
     XtSetValues(w,args,n);
}

void init_colors (Widget w)
{
}

void init_pixmaps(Widget w)
{
     light_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     light_bits, light_width, light_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     red_light_on_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     light_on_bits, light_on_width, light_on_height,
					     app_data.red_light_on_color, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     red_light_off_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     light_off_bits, light_off_width, light_off_height,
					     app_data.red_light_off_color, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     green_light_on_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     light_on_bits, light_on_width, light_on_height,
					     app_data.green_light_on_color, app_data.global_bg,
							 DefaultDepthOfScreen(XtScreen(w)));

     green_light_off_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     light_off_bits, light_off_width, light_off_height,
					     app_data.green_light_off_color, app_data.global_bg,
							  DefaultDepthOfScreen(XtScreen(w)));

     bp_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     bp_bits, bp_width, bp_height,
					     app_data.barber_fg, app_data.barber_bg, DefaultDepthOfScreen(XtScreen(w)));

     disarm_stop_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     disarm_stop_bits, disarm_stop_width, disarm_stop_height,
					     app_data.emergency_color, app_data.talkback_bg, DefaultDepthOfScreen(XtScreen(w)));
     armed_stop_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     armed_stop_bits, armed_stop_width, armed_stop_height,
					     app_data.emergency_color, app_data.talkback_bg, DefaultDepthOfScreen(XtScreen(w)));

     waiting_truck_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     truck_bits, truck_width, truck_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     waiting_truck_filled_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     truck_filled_bits, truck_filled_width, truck_filled_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     ready_truck_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     ready_truck_bits, ready_truck_width, ready_truck_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     ready_truck_filled_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     ready_truck_filled_bits, ready_truck_filled_width, ready_truck_filled_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     leaving_truck_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     leaving_truck_bits, leaving_truck_width, leaving_truck_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     leaving_truck_filled_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     leaving_truck_filled_bits, leaving_truck_filled_width, leaving_truck_filled_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     valve_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     valve_bits, valve_width, valve_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     valve_open_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     valve_open_bits, valve_open_width, valve_open_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filling_no_truck_pixmap = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filling_no_truck_bits, filling_no_truck_width, filling_no_truck_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[0] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank0_bits, tank0_width, tank0_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[1] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank1_bits, tank1_width, tank1_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[2] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank2_bits, tank2_width, tank2_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[3] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank3_bits, tank3_width, tank3_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[4] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank4_bits, tank4_width, tank4_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[5] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank5_bits, tank5_width, tank5_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[6] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank6_bits, tank6_width, tank6_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[7] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank7_bits, tank7_width, tank7_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[8] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank8_bits, tank8_width, tank8_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[9] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank9_bits, tank9_width, tank9_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     tank_pixmap[10] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     tank10_bits, tank10_width, tank10_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[0] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank0_bits, filled_tank0_width, filled_tank0_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[1] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank1_bits, filled_tank1_width, filled_tank1_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[2] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank2_bits, filled_tank2_width, filled_tank2_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[3] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank3_bits, filled_tank3_width, filled_tank3_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[4] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank4_bits, filled_tank4_width, filled_tank4_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[5] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank5_bits, filled_tank5_width, filled_tank5_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[6] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank6_bits, filled_tank6_width, filled_tank6_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[7] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank7_bits, filled_tank7_width, filled_tank7_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[8] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank8_bits, filled_tank8_width, filled_tank8_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[9] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank9_bits, filled_tank9_width, filled_tank9_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));

     filled_tank_pixmap[10] = XCreatePixmapFromBitmapData(XtDisplay(w),
					     RootWindowOfScreen(XtScreen(w)),
					     filled_tank10_bits, filled_tank10_width, filled_tank10_height,
					     app_data.global_fg, app_data.global_bg, DefaultDepthOfScreen(XtScreen(w)));


}

extern Widget emergency_stopDButton, emergency_pix_label;

void set_emergency_button()
{
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNforeground, app_data.global_bg); n++;
     XtSetValues( emergency_stopDButton ,args,n);

     n =0;
     XtSetArg(args[n], XmNlabelPixmap, armed_stop_pixmap); n++;
     XtSetValues( emergency_pix_label ,args,n);

}

void unset_emergency_button()
{
     Arg args[7];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, app_data.global_bg); n++;
     XtSetArg(args[n], XmNforeground, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNtopShadowColor, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNbottomShadowColor, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNselectColor, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNarmColor, app_data.emergency_color); n++;
     XtSetArg(args[n], XmNborderColor, app_data.emergency_color); n++;
     XtSetValues( emergency_stopDButton,args,n);

     n =0;
     XtSetArg(args[n], XmNlabelPixmap, disarm_stop_pixmap); n++;
     XtSetValues( emergency_pix_label ,args,n);
}

void update_light_buttons (Light *light)
{
     Arg args[2];
     Cardinal n;

     if (light->status == GO){
	  /* Switch */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, app_data.green_light_on_color); n++;
	  XtSetArg(args[n], XmNforeground, app_data.global_fg); n++;
	  XtSetValues(light->go_switch,args,n);
	  n = 0;
	  XtSetArg(args[n], XmNbackground, app_data.global_bg); n++;
	  XtSetArg(args[n], XmNforeground, app_data.red_light_on_color); n++;
	  XtSetValues(light->stop_switch,args,n);
	  /* Light */
	  n =0;
	  XtSetArg(args[n], XmNlabelPixmap, green_light_on_pixmap); n++;
	  XtSetValues(light->go_light,args,n);
	  n =0;
	  XtSetArg(args[n], XmNlabelPixmap, red_light_off_pixmap); n++;
	  XtSetValues(light->stop_light,args,n);

     } else if (light->status == STOP){
	  /* Switch */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, app_data.global_bg); n++;
	  XtSetArg(args[n], XmNforeground, app_data.green_light_on_color); n++;
	  XtSetValues(light->go_switch,args,n);
	  n = 0;
	  XtSetArg(args[n], XmNbackground, app_data.red_light_on_color); n++;
	  XtSetArg(args[n], XmNforeground, app_data.global_fg); n++;
	  XtSetValues(light->stop_switch,args,n);
	  /* Light */
	  n =0;
	  XtSetArg(args[n], XmNlabelPixmap, green_light_off_pixmap); n++;
	  XtSetValues(light->go_light,args,n);
	  n =0;
	  XtSetArg(args[n], XmNlabelPixmap, red_light_on_pixmap); n++;
	  XtSetValues(light->stop_light,args,n);

     } else {
	  demo_error (LG_STR(" update_light_buttons : unknown status ",
			     " update_light_buttons : unknown status "));
     }
	  
}

void set_waiting_truck_pix_label()
{
     Arg args[2];
     Cardinal n;

     n =0;
     if (demo->v_status == V_OPEN)
	  XtSetArg(args[n], XmNlabelPixmap, waiting_truck_filled_pixmap);
     else
	  XtSetArg(args[n], XmNlabelPixmap, waiting_truck_pixmap);
     n++;
     XtSetValues(filling_truck_pix_label, args,n);
     XtUnmanageChild(tank_pix_label);
}

void set_ready_truck_pix_label()
{
     Arg args[2];
     Cardinal n;

     n =0;
     if (demo->v_status == V_OPEN)
	  XtSetArg(args[n], XmNlabelPixmap, ready_truck_filled_pixmap);
     else
	  XtSetArg(args[n], XmNlabelPixmap, ready_truck_pixmap); 
     n++;
     XtSetValues(filling_truck_pix_label, args,n);
     set_tank_pix_label(0);
     XtManageChild(tank_pix_label);
}

void set_leaving_truck_pix_label()
{
     Arg args[2];
     Cardinal n;

     n =0;
     if (demo->v_status == V_OPEN)
	  XtSetArg(args[n], XmNlabelPixmap, leaving_truck_filled_pixmap);
     else
	  XtSetArg(args[n], XmNlabelPixmap, leaving_truck_pixmap);
     n++;
     XtSetValues(filling_truck_pix_label, args,n);
/*
     XtUnmanageChild(tank_pix_label);
*/
}

void set_valve_open_pix_label()
{
     Arg args[2];
     Cardinal n;

     n =0;
     XtSetArg(args[n], XmNlabelPixmap, valve_open_pixmap); n++;
     XtSetValues(valve_pix_label, args,n);
     XtManageChild(filling_pix_label);
     if (demo->tank->filling_truck != NULL) {
	  n = 0;
	  if (demo->tank->filling_truck->status == READY_TO_FILL)
	       XtSetArg(args[n], XmNlabelPixmap, ready_truck_filled_pixmap);
	  else if (demo->tank->filling_truck->status == READY_TO_GO)
	       XtSetArg(args[n], XmNlabelPixmap, leaving_truck_filled_pixmap);
	  else
	       XtSetArg(args[n], XmNlabelPixmap, waiting_truck_filled_pixmap);
	  n++;
	  XtSetValues(filling_truck_pix_label, args,n);
	  set_tank_pix_label (demo->tank->value);

     }
}

void set_valve_close_pix_label()
{
     Arg args[2];
     Cardinal n;

     n =0;
     XtSetArg(args[n], XmNlabelPixmap, valve_pixmap); n++;
     XtSetValues(valve_pix_label, args,n);
     XtUnmanageChild(filling_pix_label);
     if (demo->tank->filling_truck != NULL) {
	  n = 0;
	  if (demo->tank->filling_truck->status == READY_TO_FILL)
	       XtSetArg(args[n], XmNlabelPixmap, ready_truck_pixmap);
	  else if (demo->tank->filling_truck->status == READY_TO_GO)
	       XtSetArg(args[n], XmNlabelPixmap, leaving_truck_pixmap);
	  else
	       XtSetArg(args[n], XmNlabelPixmap, waiting_truck_pixmap);
	  n++;
	  XtSetValues(filling_truck_pix_label, args,n);
	  set_tank_pix_label (demo->tank->value);
     }

}

void set_tank_pix_label (int value)
{
     Arg args[2];
     Cardinal n;

     if (value == -1 ) value = 0;
     if ((value < 0) || (value > 10)) {
	  demo_error (LG_STR("unknown value in set_tank_pix_label",
			     "unknown value in set_tank_pix_label"));
	  return;
     }
	  
     n =0;
     if (demo->v_status == V_OPEN)
	  XtSetArg(args[n], XmNlabelPixmap, filled_tank_pixmap[value]);
     else
	  XtSetArg(args[n], XmNlabelPixmap, tank_pixmap[value]);
     n++;
     XtSetValues(tank_pix_label, args,n);
}


void set_over_filled_tank_pix_label()
{

}
