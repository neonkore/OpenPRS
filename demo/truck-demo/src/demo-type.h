/*                               -*- Mode: C -*- 
 * demo-type.h -- 
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
#ifndef INCLUDE_demo_type
#define INCLUDE_demo_type

#define MAX_TRUCK 30

typedef struct {
     Pixel global_fg, global_bg;
     Pixel talkback_bg;
     Pixel barber_fg, barber_bg;
     Pixel red_light_on_color, red_light_off_color;
     Pixel green_light_on_color, green_light_off_color;
     Pixel emergency_color;
     char *language;
} AppData;

typedef int Demo_Time;
typedef char Demo_Boolean;

typedef enum {RUN, HALT} Demo_Run_Mode;
typedef enum {FASTER, SLOWER} Speed_Mode;

typedef enum {V_OPEN, V_CLOSE} V_Status;

typedef enum {OPEN, MOVING_OPEN, MOVING_CLOSE, CLOSE} Valve_Status;
typedef enum {OPENING, CLOSING, NOTHING} Valve_Mode;

typedef enum {OVER_FULL, FULL, HALF, EMPTY, NONE} Tank_Status;
typedef enum {READY, NOT_READY} Tank_Mode;

typedef enum {READY_TO_FILL, READY_TO_GO, IN_PLACE, IN_QUEUE, ABSENT} Truck_Status;
typedef enum {GO, STOP} Light_Status;
typedef enum {LIGHT_Q, LIGHT_F} Light_Name;
typedef enum {BUSY, FREE} Truck_Sensor_Status;

typedef enum {S_OPEN, S_MOVING, S_CLOSE} Sensor_Status;

typedef struct DEMO_DEF {
     Demo_Run_Mode status;
     V_Status v_status;
     Demo_Boolean oprs_slave;
     Demo_Boolean connected;   /* to OPRS */
     Demo_Boolean oprs_time;   /* If True, oprs will send us the time (when connected) */

     struct VALVE_DEF *valve;
     struct TANK_DEF *tank;
     struct SENSOR_DEF *sensor1, *sensor2;
     int nb_truck;
     struct TRUCK_DEF *truck[MAX_TRUCK];
     struct TRUCK_DEF *mysterious_truck;               /* for forcing filling */
     struct LIGHT_DEF *light_filling, *light_queue;
     struct TR_SENSOR_DEF *tr_sensor;

} Demo;

typedef struct VALVE_DEF {
     Valve_Status status;
     Valve_Mode mode;
     Demo_Time wake_up;
     Demo_Time moving_response;
} Valve;

typedef struct TANK_DEF {
     int value;
     int min_tank, full_tank, max_tank;
     Tank_Status status;
     Tank_Mode mode;
     Demo_Time wake_up;
     Demo_Time filling_response, checking;
     Demo_Time filling_delay;
     struct TRUCK_DEF *filling_truck;
} Tank;

typedef struct SENSOR_DEF {
     struct VALVE_DEF *valve;
     Sensor_Status status;
     int id;
     Demo_Time wake_up;
     Demo_Time checking;
     Widget open_light, moving_light, close_light;
} Sensor;

typedef struct TRUCK_DEF {
     int number;
     Truck_Status status;
     Demo_Time wake_up;
     Demo_Time response, flow, queue;
     Demo_Boolean first_in_queue;
     struct LIGHT_DEF *light_filling, *light_queue;
} Truck;

typedef struct LIGHT_DEF {
     Light_Status status;
     Light_Name name;
     Widget go_light, stop_light;
     Widget go_switch, stop_switch;
} Light;

typedef struct TR_SENSOR_DEF {
     Truck_Sensor_Status status;
     Widget busy_light, free_light;
} Tr_sensor;

#endif /* INCLUDE_demo_type */


