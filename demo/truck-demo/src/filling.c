/*                               -*- Mode: C -*- 
 * filling.c -- 
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


#define sensor_free_string LG_STR("   No Truck   ",\
				  " Pas de camion ")

#define sensor_busy_string LG_STR("Truck in Place",\
				  "Camion en Place")

extern Widget queue_label;

extern Widget  queued_truck_pix_label, filling_truck_pix_label, filling_pix_label, tank_pix_label;
extern  Widget filling_light_stop_pix_label, filling_light_go_pix_label;
extern  Widget queue_light_stop_pix_label, queue_light_go_pix_label;


extern Widget filling_switch_goDButton, filling_switch_stopDButton;
extern Widget queue_switch_goDButton, queue_switch_stopDButton;
extern Widget truck_sensor_busyDButton, truck_sensor_freeDButton;
extern Widget sensor_truckDButton;
extern Widget  filling_control_fillingDButton, filling_control_emptyDButton, filling_control_fullDButton;
extern Widget fillingSlider;

Demo_Boolean  add_truck_in_place (Truck *truck);
void remove_truck_from_place(void);
void  add_truck_in_queue (Truck *truck);
void remove_truck_from_queue (Truck *truck);

void init_tank_filling (void);
void end_tank_filling (void);

static int nb_truck_queued = 0;
static Truck *queue[MAX_TRUCK];

void init_filling()
{
     char string[STRSIZE];
     XmString xmstr;
     Truck *truck;
     int i;

     demo->tank->mode = NOT_READY;
     demo->tank->value = -1;
     demo->tank->min_tank = MIN_TANK;
     demo->tank->full_tank = FULL_TANK;
     demo->tank->max_tank = MAX_TANK;
/* the tank sensor is always active */
     demo->tank->wake_up = TANK_CHECKING;
     demo->tank->checking = TANK_CHECKING;

     demo->tank->filling_delay = -1;
     demo->tank->filling_response = FILLING_DELAY;
     demo->tank->filling_truck = NULL;

     nb_truck_queued = 0;
     sprintf (string,"%d",  nb_truck_queued);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( queue_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

     xmstr = XmStringCreateSimple(sensor_free_string);
     XtVaSetValues( sensor_truckDButton, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

     for (i= 0; i< demo->nb_truck; i++) {
	  truck = demo->truck[i];
	  truck->status = ABSENT;
	  truck->response = TRUCK_RESPONSE;
	  truck->queue = TRUCK_QUEUE;
	  truck->flow = TRUCK_FLOW;
	  truck->wake_up = i * TRUCK_FLOW;
	  truck->first_in_queue = FALSE;
     }
     truck = demo->mysterious_truck;
     truck->response = TRUCK_RESPONSE;
     truck->status = ABSENT;
     truck->wake_up = -1;

     demo->light_filling->status = STOP;
     demo->light_queue->status = STOP;

     demo->light_filling->go_switch = filling_switch_goDButton;
     demo->light_filling->stop_switch = filling_switch_stopDButton;
     demo->light_queue->go_switch = queue_switch_goDButton;
     demo->light_queue->stop_switch = queue_switch_stopDButton;

     demo->light_filling->go_light = filling_light_go_pix_label;
     demo->light_filling->stop_light = filling_light_stop_pix_label;
     demo->light_queue->go_light = queue_light_go_pix_label;
     demo->light_queue->stop_light = queue_light_stop_pix_label;

     update_light_buttons (demo->light_queue);
     update_light_buttons (demo->light_filling);

     demo->tr_sensor->status = FREE;

     demo->tr_sensor->busy_light = truck_sensor_busyDButton;
     demo->tr_sensor->free_light = truck_sensor_freeDButton;

     unset_button(truck_sensor_busyDButton);
     set_button(truck_sensor_freeDButton);

     unset_button ( filling_control_emptyDButton);
     unset_barber_button ( filling_control_fillingDButton);
     unset_button ( filling_control_fullDButton);
	  
     XtUnmanageChild(fillingSlider);
     XtUnmanageChild(filling_truck_pix_label);
     XtUnmanageChild(tank_pix_label);
     XtUnmanageChild(queued_truck_pix_label);
     set_waiting_truck_pix_label();
}

void change_light(Light *light, Light_Status status)
{
     if ((status == GO) || (status == STOP ) ) {
	  light->status = status;
	  update_light_buttons (light);
	  if (demo->oprs_slave)
	       send_oprs_light_move (light);
     } else {
	  demo_error (LG_STR("change_light : unknown status ",
			     "change_light : unknown status "));
     }
	  
}

Demo_Boolean  add_truck_in_place (Truck *truck)
{
     XmString xmstr;

     if (demo->tr_sensor->status == BUSY) {
	  demo_warning (LG_STR("A truck is already in place\n",
			       "Un camion est déjà en place\n"));
	  return(FALSE);
     }
     xmstr = XmStringCreateSimple(sensor_busy_string);
     XtVaSetValues( sensor_truckDButton, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
	
     demo->tr_sensor->status = BUSY;
     set_button(truck_sensor_busyDButton);
     unset_button(truck_sensor_freeDButton);

     if (demo->oprs_slave)
	  send_oprs_sensor_busy();

     demo_message (LG_STR("A truck is in place\n",
			  "Un camion est en place\n"));

     set_waiting_truck_pix_label();

     XtManageChild(filling_truck_pix_label);

     demo->tank->filling_truck = truck;
     return (TRUE);

}

void busy_truck_sensor ()
{
     Demo_Time time;
     Demo_Boolean res;
     Truck *truck = demo->mysterious_truck;

     res = add_truck_in_place (truck);
      if (res == FALSE) {
	   demo_error (LG_STR("sensor_truck_change  : change failed ",
			      "sensor_truck_change  : change failed "));
	   return;
      }
     time = get_demo_time();
     truck->status = IN_PLACE;
     truck->wake_up = time + truck->response;

}

void remove_truck_from_place ()
{
     XmString xmstr;

     if (demo->tr_sensor->status == FREE) {
	  demo_error (LG_STR("The place is already free\n",
			     "La place est déjà libre\n"));
	  return;
     }
     xmstr = XmStringCreateSimple(sensor_free_string);
     XtVaSetValues( sensor_truckDButton, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
     
     demo->tr_sensor->status = FREE;
     unset_button(truck_sensor_busyDButton);
     set_button(truck_sensor_freeDButton);
     
     if (demo->oprs_slave)
	  send_oprs_sensor_free();


     demo_message (LG_STR("The truck leaves the filling place\n",
			  "Le camion quitte la station\n"));
     XtUnmanageChild(filling_truck_pix_label);
     XtUnmanageChild(tank_pix_label);
     set_waiting_truck_pix_label();

     demo->tank->filling_truck = NULL;
     return;
}

void free_truck_sensor ()
{
     Demo_Time time;
     Truck *truck;

     truck = demo->tank->filling_truck;

     if ((truck->status == IN_PLACE) || (truck->status == READY_TO_GO)) {
	  /* the filling is not initialised */
	  remove_truck_from_place();
     } else if  (truck->status == READY_TO_FILL) {
	  /* the filling is initialised */
	  end_tank_filling();
	  remove_truck_from_place();
     } else
	  demo_error (LG_STR("We can not free if the truck is not here\n",
			     "We can not free if the truck is not here\n"));

     truck->status = ABSENT;
     if (truck->number == -1) 
	  truck->wake_up =-1;
     else {
	  time = get_demo_time();
	  truck->wake_up = time + truck->flow;
     }


}

void add_truck_in_queue (Truck *truck)
{
     char string[STRSIZE];
     XmString xmstr;
     char message[BUF_SIZE];
     char *str1 =LG_STR("add truck:",
			"ajoute camion:");
     char *str2 =LG_STR("in queue. range :",
			"dans la queue. rang :");

     queue [nb_truck_queued] = truck;

     if (nb_truck_queued == 0) {
	  truck->first_in_queue = TRUE;
	  XtManageChild(queued_truck_pix_label);
     } else {
	  truck->first_in_queue = FALSE;
          truck->wake_up += truck->queue;
     }
     nb_truck_queued++;
     
     sprintf (string,"%d",  nb_truck_queued);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( queue_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);

     sprintf (message,"%s %d %s %d\n",str1, truck->number, str2, nb_truck_queued);
     demo_message(message);
     
}

void remove_truck_from_queue (Truck *truck)
{
     char string[STRSIZE];
     XmString xmstr;

     int i;

     if (nb_truck_queued <1) {
	  demo_error( LG_STR("No Truck in the queue",
			     "Pas de camion dans la queue"));
	  return;
     }
     if (queue [0] != truck) {
	  demo_error( LG_STR("Not the first truck which leaves the queue ",
			     "Not the first truck which leaves the queue "));
	  return;
     }

     for (i = 1 ; i < nb_truck_queued; i++)
	  queue[i-1]= queue [i];
     nb_truck_queued--;
     if (nb_truck_queued > 0) {
	  queue[0]->first_in_queue = TRUE;
	  queue[0]->wake_up += truck->queue; /* for let time to stop the light */
	  XtManageChild(queued_truck_pix_label);
     }
     sprintf (string,"%d",  nb_truck_queued);
     xmstr = XmStringCreateSimple(string);
     XtVaSetValues( queue_label, XmNlabelString, xmstr, NULL);
     XmStringFree(xmstr);
}

void check_truck(Truck *truck, Demo_Time time)
{
     if (truck->wake_up == -1)
	  return;
     if (time >=  truck->wake_up) {      /* something to do */
	  if (truck->status == ABSENT) { 
	       if (truck->number == -1) {
		    demo_error (LG_STR("The mysterious truck cannot been add in queue\n",
				       "The mysterious truck cannot been add in queue\n"));
		    truck->wake_up = -1;
		    return;
	       }
	       add_truck_in_queue (truck);
	       truck->wake_up = time + truck->response;
	       truck->status = IN_QUEUE;

	  } else if (truck->status == IN_QUEUE) {
	       if (truck->first_in_queue) {
		    XtManageChild(queued_truck_pix_label);

		    if( (truck->light_queue->status == GO) &&
			 add_truck_in_place (truck) ) {
			 
			 XtUnmanageChild(queued_truck_pix_label);
			 remove_truck_from_queue (truck);
			 truck->status = IN_PLACE;
			 truck->wake_up = time + truck->response;
		    } else
			 truck->wake_up = time + truck->response;
	       } else {
		    truck->wake_up = time + truck->queue;
	       }
	  
	  } else if (truck->status == IN_PLACE) {
	       truck->wake_up = time + truck->response;
	       truck->status = READY_TO_FILL;
	       init_tank_filling();

	       set_ready_truck_pix_label();

	  } else if (truck->status == READY_TO_FILL) {
	       if (truck->light_filling->status == GO) {
		    truck->wake_up = time + truck->response;
		    truck->status = READY_TO_GO;
		    end_tank_filling();

		    set_leaving_truck_pix_label();

		} else
		    truck->wake_up = time + truck->response;
	  
	  } else if (truck->status == READY_TO_GO) {
	       free_truck_sensor ();

	  } else
	       demo_error (LG_STR("check_truck: Unknown truck mode ",
				  "check_truck: Unknown truck mode "));

     }
}

void init_tank_filling ()
{
     Demo_Time time;
     Tank *tank = demo->tank;

     time = get_demo_time();

     if (demo->tank->filling_delay != -1)
	  demo_warning(LG_STR("The filling begins before ...\n",
			      "The filling begins before ...\n"));

     if (demo->tank->mode != NOT_READY)
	  demo_error (LG_STR("init_tank_filling",
			     "init_tank_filling"));

     demo->tank->value = -1;
     demo->tank->mode = READY;

     if (demo->oprs_slave)
	  send_oprs_tank_empty();
     demo_message (LG_STR("The tank is ready\n",
			  "The tank is ready\n"));
     
     demo->tank->status = NONE;
     change_filling(demo->tank->min_tank);

     XtManageChild(fillingSlider);
     XtVaSetValues( fillingSlider, XmNvalue, 0, NULL);
	  
     set_ready_truck_pix_label();
}

void end_tank_filling ()
{
     demo->tank->mode = NOT_READY;
     if (demo->tank->filling_delay != -1)
	  demo_warning(LG_STR("The filling has not been stopped\n",
			      "The filling has not been stopped\n"));

     demo_message (LG_STR("The tank is ready to go\n",
			  "The tank is ready to go\n"));

     demo->tank->status = NONE;
     demo->tank->value = -1;

     unset_button ( filling_control_emptyDButton);
     unset_barber_button ( filling_control_fillingDButton);
     unset_button ( filling_control_fullDButton);
	  
     XtUnmanageChild(fillingSlider);
}

void go_filling_tank()
{
     Demo_Time time;

     time = get_demo_time();

     if (demo->tank->mode == READY) {
	  demo->tank->filling_delay = time + demo->tank->filling_response;

     } else if (demo->tank->mode == NOT_READY) {
	  demo_warning (LG_STR("The tank is not ready to fill\n",
			       "The tank is not ready to fill\n"));
	  demo->tank->filling_delay = time + demo->tank->filling_response;

     } else
	  demo_error (LG_STR("go_filling_tank: unknown mode",
			     "go_filling_tank: unknown mode"));

}

void suspend_filling_tank ()
{
     if (demo->tank->mode == READY) {
	  demo->tank->filling_delay = -1;

     } else if (demo->tank->mode == NOT_READY) {
	  demo_warning (LG_STR("The tank is not ready to fill, then to supend\n",
			       "The tank is not ready to fill, then to supend\n"));
	  demo->tank->filling_delay = -1;

     } else
	  demo_error (LG_STR("suspend_filling_tank: unknown mode",
			     "suspend_filling_tank: unknown mode"));

}

void check_tank (Demo_Time time)
{
     if (demo->tank->wake_up == -1)
	  return;

     if (time >=  demo->tank->wake_up) {      /* something to do */
	  Demo_Time filling_delay;
	  filling_delay = demo->tank->filling_delay;

	  if (demo->tank->mode == READY) {
	       if ((filling_delay != -1) && (time >= filling_delay)) {
		    /* the vanne is open and it is time to add fuel */
		     
		    change_filling (demo->tank->value +1);
		    demo->tank->filling_delay += demo->tank->filling_response;
	       }

	  } else if (demo->tank->mode == NOT_READY) {
	       if (filling_delay != -1) {
		    demo_warning (LG_STR("the tank is not ready, but filled\n",
					 "the tank is not ready, but filled\n"));

	       }
	  } else
	       demo_error (LG_STR("check_tank: unknown mode",
				  "check_tank: unknown mode"));
	  
	  demo->tank->wake_up = time + demo->tank->checking; /* Next Check */
     }
}


void change_filling (int new_value)
{
     Tank *tank = demo->tank;
     int tank_fill = demo->tank->value;
     int min_tank = demo->tank->min_tank;
     int full_tank = demo->tank->full_tank;
     int max_tank = demo->tank->max_tank;

     if (tank_fill == new_value) /* nothing to do */
	  return;

     if ((new_value > max_tank) || (new_value < min_tank)){
	  demo_error(LG_STR("change_filling: value out of range",
			    "change_filling: value out of range"));
	  return;
     }

     if ((new_value < full_tank) && (new_value > min_tank)) {
	  if (tank->status != HALF) {
	       unset_button ( filling_control_emptyDButton);
	       set_barber_button ( filling_control_fillingDButton);
	       unset_button ( filling_control_fullDButton);
	       tank->status = HALF;

	  }
	  tank->value = new_value;
	  set_tank_pix_label (new_value);

     } else if (new_value == min_tank) {
	  if (tank->status != EMPTY) {
	       set_button ( filling_control_emptyDButton);
	       unset_barber_button ( filling_control_fillingDButton);
	       unset_button ( filling_control_fullDButton);
	       tank->status = EMPTY;
	  }
	  tank->value = new_value;
	  set_tank_pix_label (new_value);

     } else if (new_value == full_tank) {
	  if (tank->status != FULL) {
	       unset_button ( filling_control_emptyDButton);
	       unset_barber_button ( filling_control_fillingDButton);
	       set_button ( filling_control_fullDButton);
	       tank->status = FULL;
	  
	       if (demo->oprs_slave)
		    send_oprs_tank_full();
	       demo_message(LG_STR("The tank is full\n",
				   "The tank is full\n"));
	  }
	  tank->value = new_value;
	  set_tank_pix_label (new_value);

     } else if ((new_value > full_tank) && (new_value < max_tank)) {
	  if (tank->status != OVER_FULL) {
	       unset_button ( filling_control_emptyDButton);
	       unset_barber_button ( filling_control_fillingDButton);
	       set_button ( filling_control_fullDButton);
	       tank->status = OVER_FULL;
	  }
	  demo_warning (LG_STR("The tank is full\n",
			       "The tank is full\n"));
	  tank->value = new_value;
          set_tank_pix_label (new_value);

     } else if (new_value == max_tank) {
	  if (tank->status != OVER_FULL) {
	       unset_button ( filling_control_emptyDButton);
	       unset_barber_button ( filling_control_fillingDButton);
	       set_button ( filling_control_fullDButton);
	       tank->status = OVER_FULL;
	  }
	  demo_warning (LG_STR("The tank is overfull\n",
			       "The tank is overfull\n"));
	  set_over_filled_tank_pix_label ();

     } else
	  demo_error (LG_STR("invalid value in change_filling\n",
			     "invalid value in change_filling\n"));
		

     XtVaSetValues( fillingSlider, XmNvalue, new_value, NULL);
}
