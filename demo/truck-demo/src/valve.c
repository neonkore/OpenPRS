/*                               -*- Mode: C -*- 
 * valve.c -- 
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
#include "demo-time.h"

extern Widget sensor1_movingDButton, sensor1_closeDButton, sensor1_openDButton;
extern Widget sensor2_movingDButton, sensor2_closeDButton, sensor2_openDButton;
extern Widget switch_valve_openDButton, switch_valve_closeDButton;

void open_valve (void);
void close_valve (void);

void init_valve()
{
     unset_emergency_button ();

     demo->v_status = V_CLOSE;
     set_valve_close_pix_label();

     demo->valve->status = CLOSE;
     demo->valve->wake_up = -1;
     demo->valve->moving_response = VALVE_RESPONSE;
/* The sensors are always active */
     demo->sensor1->wake_up = SENSOR1_CHECKING;
     demo->sensor2->wake_up = SENSOR2_CHECKING;
     demo->sensor1->checking = SENSOR1_CHECKING;
     demo->sensor2->checking = SENSOR2_CHECKING;

     unset_button (switch_valve_openDButton);
     set_button (switch_valve_closeDButton);

     unset_button (sensor1_openDButton);
     unset_barber_button (sensor1_movingDButton);
     set_button (sensor1_closeDButton);
     demo->sensor1->status = S_CLOSE;

     unset_button (sensor2_openDButton);
     unset_barber_button (sensor2_movingDButton);
     set_button (sensor2_closeDButton);
     demo->sensor2->status = S_CLOSE;
     
     demo->sensor1->open_light = sensor1_openDButton;
     demo->sensor1->moving_light = sensor1_movingDButton;
     demo->sensor1->close_light = sensor1_closeDButton;
     demo->sensor2->open_light = sensor2_openDButton;
     demo->sensor2->moving_light = sensor2_movingDButton;
     demo->sensor2->close_light = sensor2_closeDButton;

}

void change_valve(Valve_Mode mode)
{
     if (demo->valve->moving_response == BLOCKED_VALUE) {
	  demo_warning (LG_STR("The vanne is blocked ",
			       "The vanne is blocked "));
	  return;
     }
     if (mode == OPENING) {
	  set_button (switch_valve_openDButton);
	  unset_button (switch_valve_closeDButton);

	  switch (demo->valve->status) {
	  case OPEN: 
	       demo_warning (LG_STR("The valve is already open",
				    "The valve is already open"));
	       break;
	  case  MOVING_OPEN:
	       demo_warning (LG_STR("keep_cool: The valve is opening",
				    "keep_cool: The valve is opening"));
	       break;
	  case MOVING_CLOSE:
	  case CLOSE:
	       open_valve();
	       break;
	  default:
	       demo_error(LG_STR("change_valve: Unknown status",
				 "change_valve: Unknown status"));
	       break;
	  }
     } else if (mode == CLOSING) {
	  unset_button (switch_valve_openDButton);
	  set_button (switch_valve_closeDButton);

     	  switch (demo->valve->status) {
	  case OPEN:
	  case  MOVING_OPEN:
	       close_valve();
	       break;
	  case MOVING_CLOSE:
	       demo_warning (LG_STR("keep_cool: The valve is closing",
				    "keep_cool: The valve is closing"));
	       break;
	  case CLOSE:
	       demo_warning (LG_STR("The valve is already closed",
				    "The valve is already closed"));
	       break;
	  default:
	       demo_error(LG_STR("change_valve: Unknown status",
				 "change_valve: Unknown status"));
	       break;
	  }
     } else {
	  demo_error(LG_STR("change_valve: mode",
			    "change_valve: mode"));
	  return;
    }

}

void open_valve ()
{
     Demo_Time time;

     time = get_demo_time();

     demo->valve->status = MOVING_OPEN;
     demo->valve->wake_up = time + demo->valve->moving_response;
}

void close_valve ()
{
     Demo_Time time;

     time = get_demo_time();

     demo->valve->status = MOVING_CLOSE;
     demo->valve->wake_up = time + demo->valve->moving_response;
}

void check_valve(Demo_Time time)
{
     if (demo->valve->wake_up == -1)
	  return;

     if (time >=  demo->valve->wake_up) {      /* something to do */
	  if (demo->valve->status == MOVING_CLOSE) {
	       demo->valve->status = CLOSE;
	       demo->v_status = V_CLOSE;
	       demo_message (LG_STR("The valve is closed\n",
				    "The valve is closed\n"));
	       suspend_filling_tank();

	       set_valve_close_pix_label();
	  } else if  (demo->valve->status == MOVING_OPEN) {
	       demo->valve->status = OPEN;
	       demo->v_status = V_OPEN;
	       demo_message (LG_STR("The valve is open\n",
				    "The valve is open\n"));
	       go_filling_tank();

	       set_valve_open_pix_label();
	  } else
	       demo_error (LG_STR("Wake up but not moving",
				  "Wake up but not moving"));
     demo->valve->wake_up = -1;
     }
}

void check_sensor(Sensor *sensor, Demo_Time time)
{
     if (sensor->wake_up == -1) /* this sensor has been inhibited */
	  return;
     if (time >=  sensor->wake_up) {      /* something to do */
	  if (sensor->valve->status == OPEN) { 
	       if (sensor->status != S_OPEN) { /* up to date */
		    set_button(sensor->open_light);
		    unset_barber_button(sensor->moving_light);
		    unset_button(sensor->close_light);
		    sensor->status = S_OPEN;

		    if (demo->oprs_slave)
			 send_oprs_sensor_open(sensor->id);
	       }
	  } else if (sensor->valve->status == CLOSE) {
	       if (sensor->status != S_CLOSE) { /* up to date */
		    unset_button(sensor->open_light);
		    unset_barber_button(sensor->moving_light);
		    set_button(sensor->close_light);
		    sensor->status = S_CLOSE;

		    if (demo->oprs_slave)
			 send_oprs_sensor_close(sensor->id);
	       }
	  } else if ((sensor->valve->status == MOVING_OPEN)
		     || (sensor->valve->status == MOVING_CLOSE)) { 
	       if (sensor->status != S_MOVING) { /* up to date */
		    unset_button(sensor->open_light);
		    set_barber_button(sensor->moving_light);
		    unset_button(sensor->close_light);
		    sensor->status = S_MOVING;

		    if (demo->oprs_slave)
			 send_oprs_sensor_bp(sensor->id);
	       }
	  } else
	       demo_error (LG_STR("check_sensor: Unknown valve mode ",
				  "check_sensor: Unknown valve mode "));

	  sensor->wake_up = time + sensor->checking;
     }
}

void emergency_stop()
{
     set_emergency_button ();
     demo_message (LG_STR(" EMERGENCY STOP \n",
			  " EMERGENCY STOP \n"));
     change_run_mode (HALT);

     if (demo->v_status == V_OPEN) {
	  suspend_filling_tank();
	  demo->v_status = V_CLOSE;
	  set_valve_close_pix_label();
     }
}
