/*                               -*- Mode: C -*- 
 * demo_f.h -- 
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

/* functions from util.c */

void demo_error (char *s);
void demo_warning (char *s);
void demo_message (char *s);
void oprs_message (char *s);
void set_button(Widget w);
void unset_button(Widget w);
void set_barber_button(Widget w);
void unset_barber_button(Widget w);
void init_colors (Widget w);
void init_pixmaps(Widget w);

void set_emergency_button(void);
void unset_emergency_button(void);
void update_light_buttons (Light *light);
void set_waiting_truck_pix_label(void);
void set_ready_truck_pix_label(void);
void set_leaving_truck_pix_label(void);
void set_valve_open_pix_label(void);
void set_valve_close_pix_label(void);
void set_tank_pix_label (int value);
void set_over_filled_tank_pix_label(void);

/* message.c */
void connect_to_oprs(void);
void leave_oprs(void);
void halt_oprs_time(void);
void run_oprs_time(void);
void ask_oprs_time (int initial_time);
void set_time_from_oprs(int oprs_time);
void run_oprs_load (void);
void  send_oprs_sensor_busy(void);
void send_oprs_sensor_free(void);
void send_oprs_tank_empty(void);
void send_oprs_tank_full(void);
void send_oprs_sensor_open(int sensor_id);
void send_oprs_sensor_bp(int sensor_id);
void send_oprs_sensor_close(int sensor_id);
void send_oprs_light_move(Light *light);

/* oprs-interface.c */
void send_message_to_oprs (char *message);
void demo_init_arg(int argc,char **argv);
void connect_to_mp(void);

/* filling.c */
void init_filling(void);
void change_light(Light *light, Light_Status status);
void busy_truck_sensor (void);
void free_truck_sensor (void);
void check_truck(Truck *truck, Demo_Time time);
void go_filling_tank(void);
void suspend_filling_tank(void);
void check_tank (Demo_Time time);
void change_filling (int new_value);

/* valve.c */
void init_valve(void);
void change_valve(Valve_Mode mode);
void check_valve(Demo_Time time);
void check_sensor(Sensor *sensor, Demo_Time time);
void emergency_stop(void);

/* test-windows.c */
void make_text_windows(Widget parent);
void appendTextWindow(Widget textWindow, char *s);
void clear_all_textWindows(void);

/* demo-time.c */
void update_clock(void);
void init_demo_time(int initial_time);
Demo_Time get_demo_time (void);
void change_time_server (void);
void change_demo_speed(Speed_Mode mode);
void change_run_mode(Demo_Run_Mode mode);
void set_time_from_oprs(int oprs_time);

/* truck-demo.c */
void accepted_by_oprs(void);
void refused_by_oprs(char *other);

/* error-bboard.c */
void make_error_bboard(Widget parent);
void reset_default_options_menus (void);

void make_clock_bboard(Widget parent);
void make_command_bboard(Widget parent);
void make_filling_bboard(Widget parent);
void make_control_bboard(Widget parent);









