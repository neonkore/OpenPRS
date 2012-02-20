/*                               -*- Mode: C -*- 
 * user-external.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
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

#include "config.h"

#include "opaque-pub.h"
#include "user-external.h"
#include "user-external_f.h"

#include "stdio.h"
#include "time.h"

/* 
 * This file will contain the functions provided to the user and 
 * called from the kernel. For example, if the user want to get 
 * statistics on FACTS or GOALS, or database operation, we could put 
 * in the kernel code a function call pointing to a function defined 
 * here. By default this function would do nothing, but if the user 
 * wish it would perform some operations.
 */


/* This is the pointer to the predicate which sort the list of the root of the intention graph.
 * If it is NULL, then no predicates are applied
 */

#define ONE_MILLION 1000000
#define MAX_SAVE 1001

long send_sec[MAX_SAVE],  send_usec[MAX_SAVE],  receive_sec[MAX_SAVE],  receive_usec[MAX_SAVE];



int nb_send_message = 0;
int nb_conclude = 0;
int nb_retrieve = 0;
int nb_relevant = 0;
int nb_applic = 0;
int nb_int_create = 0;
int nb_int_active = 0;
int nb_int_in_graph = 0;
int nb_handle = 0;
int nb_goal = 0;

int nb_test = 0;

long total_sec_send_message =0, total_usec_send_message = 0;
long min_sec_send_message = -1, min_usec_send_message = -1;
long max_sec_send_message = -1, max_usec_send_message = -1;

long total_sec_conclude =0, total_usec_conclude = 0;
long min_sec_conclude = -1, min_usec_conclude = -1;
long max_sec_conclude = -1, max_usec_conclude = -1;

long total_sec_retrieve =0, total_usec_retrieve = 0;
long min_sec_retrieve = -1, min_usec_retrieve = -1;
long max_sec_retrieve = -1, max_usec_retrieve = -1;

long total_sec_relevant =0, total_usec_relevant = 0;
long min_sec_relevant = -1, min_usec_relevant = -1;
long max_sec_relevant = -1, max_usec_relevant = -1;

long total_sec_applic =0, total_usec_applic = 0;
long min_sec_applic = -1, min_usec_applic = -1;
long max_sec_applic = -1, max_usec_applic = -1;

long total_sec_int_create =0, total_usec_int_create = 0;
long min_sec_int_create = -1, min_usec_int_create = -1;
long max_sec_int_create = -1, max_usec_int_create = -1;

long total_sec_int_active = 0, total_usec_int_active = 0;
long min_sec_int_active = -1, min_usec_int_active = -1;
long max_sec_int_active = -1, max_usec_int_active = -1;

long total_sec_int_in_graph =0, total_usec_int_in_graph = 0;
long min_sec_int_in_graph = -1, min_usec_int_in_graph = -1;
long max_sec_int_in_graph = -1, max_usec_int_in_graph = -1;

long total_sec_handle =0, total_usec_handle = 0;
long min_sec_handle = -1, min_usec_handle = -1;
long max_sec_handle = -1, max_usec_handle = -1;

long total_sec_goal =0, total_usec_goal = 0;
long min_sec_goal = -1, min_usec_goal = -1;
long max_sec_goal = -1, max_usec_goal = -1;

long total_sec_test = 0, total_usec_test = 0;
long min_sec_test = -1, min_usec_test = -1;
long max_sec_test = -1, max_usec_test = -1;

void send_message_average ( long sec, long usec);
void conclude_average ( long sec, long usec);
void retrieve_average ( long sec, long usec);
void relevant_average ( long sec, long usec);
void applic_average ( long sec, long usec);
void int_create_average ( long sec, long usec);
void int_active_average ( long sec, long usec);
void int_in_graph_average ( long sec, long usec);
void handle_average ( long sec, long usec);
void goal_average ( long sec, long usec);

void test_average ( long sec, long usec);

void init_send_save(void);
void init_receive_save(void);
void send_save (int index, long sec, long usec);
void receive_save (int index, long sec, long usec);

void user_time_reset_all ();
void user_time_print_all ();


/* The following flag controls collecting timing data for all probes.
  Haven't added flags to control collecting specific probe data   */
int collect_timing_data_flag = 0;  /* 0=don't collect timing data, 1=do collect timming data */
int wall_clock_flag = 0;  /* 1=save into start time,  0=save into end time  */

int send_message_flag = 0;
int conclude_flag = 0;
int retrieve_flag = 0;
int relevant_flag = 0;
int applic_flag = 0;
int int_create_flag = 0;
int int_active_flag = 0;
int int_in_graph_flag = 0;
int handle_flag = 0;
int goal_flag = 0;

int send_flag = 0;
int receive_flag = 0;

int test_flag = 0;

struct tm *s_systime; /* ptr to system time structure  */
time_t t;

int shr=0, smin=0, ssec=0, ehr=0, emin=0, esec=0;


static char fname[101] = {NULL}; /* File name must be <= 100 characters  */
FILE *pfname;  /*  Ptr to fname stream  */
static char data_id[100] = {NULL}; /* User specified DATA id  */



/************ START OF FUNCTION PROTOTYPES/DEFINITIONS ******************/

PBoolean intention_list_sort_by_priority(Intention *i1, Intention *i2);

extern PFB intention_list_sort_predicate;

void get_fname ();

void get_data_id ();

void write_data_to_file ();

void append_data_to_file ();

void init_data_id();

void write_it();

PBoolean my_intention_list_sort_example(Intention *i1, Intention *i2)
{
     return (intention_priority(i1) > intention_priority(i2));
}

PBoolean intention_list_sort_by_priority_then_time(Intention *i1, Intention *i2)
{
     if (intention_priority(i1) != intention_priority(i2))
	  return (intention_priority(i1) > intention_priority(i2));
     else
	  return (intention_time(i1) < intention_time(i2));

}

void start_kernel_user_hook(char *name)
{
     intention_scheduler = &intention_scheduler_time_sharing;
     /* The next statement is useless, as intention sheduler as priotity over intention sorting. */
     intention_list_sort_predicate = &intention_list_sort_by_priority;
     main_loop_pool_sec = 0L;
     main_loop_pool_usec = 1000L;
}


void end_kernel_user_hook()
{
     printf(LG_STR("Bye, bye...\n",
		   "Bye, bye...\n"));
}

void user_call_from_parser (int code)
{
     switch (code){
     case 0:    /* Stop collecting timing data  */
          collect_timing_data_flag = 0; 
	  printf(LG_STR("Timing is turned off.\n",
			"Timing is turned off.\n"));

	  t = time(NULL);
	  s_systime = localtime(&t);
	  printf("End Time:  %.2d:%.2d:%.2d\n", ehr, emin, esec);
	  printf("Date: %.2d/%.2d/%.2d", s_systime->tm_mon+1, /* We assume we will issue Stop command on the same*/
		 s_systime->tm_mday, s_systime->tm_year);     /* day we actually stop collecting data.  */

	  break;
	case 1:   /* Start collecting timing data  */
	  collect_timing_data_flag = 1;
	  printf("Timing is turned on.\n");
	  user_time_reset_all();

	  t = time(NULL);
	  s_systime = localtime(&t);
	  printf("Date: %.2d/%.2d/%.2d", s_systime->tm_mon+1,  /* We assume we will actually start collecting timing */
		 s_systime->tm_mday, s_systime->tm_year);      /* data the same day we issue the Start command   */
	  wall_clock_flag = 1;

	  user_time_reset_all();
	  printf("\nTiming data is reinitialized.\n");

	  break;
	case 2:   /*  Reinitialize timing data  */
	  user_time_reset_all();
	  wall_clock_flag = 1;	  
	  printf("Timing data is reinitialized.\n");
	  break;
	case 3:    /* Print timing data to standard output */
	  user_time_print_all();
	  break;
	case 4:  /*  Print timing data to standard output and append it to 
		     new or existing file  */
	  
	  user_time_print_all();
	  append_data_to_file();
	  break;
	case 5:  /* Print timing data to standard output and write data
		    to new or existing file (overwriting current file data)  */

	  user_time_print_all();
	  write_data_to_file();
	  break;
     case 10:
	  /* */
	  send_message_flag = 0;
	  conclude_flag = 0;
	  retrieve_flag = 0;
	  relevant_flag = 0;
	  applic_flag = 0;
	  int_create_flag = 0;
	  int_active_flag = 0;
	  int_in_graph_flag = 0;
	  handle_flag = 0;
	  goal_flag = 0;

	  send_flag = 0;
	  receive_flag = 0;
	  
	  test_flag = 0;
	  break;
     case 11:
	  test_flag = 1;
	  break;
     case 12:
	  goal_flag = 1;
	  break;
     case 13:
	  int_active_flag = 1;
	  int_in_graph_flag = 1;
	  break;
     case 14:
	  conclude_flag = 1;
	  break;
     case 15:
	  send_flag = 1;
	  init_send_save( );
	  receive_flag = 1;
	  init_receive_save( );
	  break;
     default:
	  printf(" Unknown constant, ( check the user-external.[ch] files ) \n");
	}
}

void user_time_note (int funct_id, int index, long sec, long usec)
{
     switch (funct_id){
     case T_DEFAULT:
	  break;
     case T_SEND:
	  if (collect_timing_data_flag &&  send_flag) send_save(index, sec, usec);
	  break;
     case T_RECEIVE:
	  if (collect_timing_data_flag &&  receive_flag) receive_save(index, sec, usec);
	  break;
     default:
	  printf(" Unknown time note: %d \n", funct_id);
     }
}

void user_time (int funct_id, long sec, long usec)
{
     switch (funct_id){
     case T_DEFAULT:
	  break;
     case T_SEND_MESSAGE:
	  if (collect_timing_data_flag &&  send_message_flag) send_message_average ( sec, usec);
	  break;
     case T_CONCLUDE:
	  if (collect_timing_data_flag &&  conclude_flag) conclude_average ( sec, usec);
	  break;
     case T_RETRIEVE:
	  if (collect_timing_data_flag &&  retrieve_flag) retrieve_average ( sec, usec);
	  break;
     case T_RELEVANT:
	  if (collect_timing_data_flag &&  relevant_flag) relevant_average ( sec, usec);
	  break;
     case T_APPLIC:
	  if (collect_timing_data_flag &&  applic_flag) applic_average ( sec, usec);
	  break;
     case T_INTENTION_CREATE:
/*
	  if (collect_timing_data_flag &&  int_create_flag) int_create_average ( sec, usec);
*/
	  break;
     case T_INTENTION_ACTIVE:
	  if (collect_timing_data_flag &&  int_active_flag) int_active_average ( sec, usec);
	  break;
     case T_INTENTION_IN_GRAPH:
	  if (collect_timing_data_flag &&  int_in_graph_flag) int_in_graph_average ( sec, usec);
	  break;
     case T_HANDLE:
	  if (collect_timing_data_flag &&  handle_flag) handle_average ( sec, usec);
	  break;
     case T_GOAL_CR_REC:   
	  break;
     case T_GOAL_CR_SOAK:  
	  break;
     case T_GOAL_CR_REP:  
	  if (collect_timing_data_flag &&  goal_flag) goal_average ( sec, usec);
	  break;
     case T_GOAL_REC_SOAK: 
	  break;
     case T_GOAL_REC_REP:
	  break;
     case T_GOAL_SOAK_REP:
	  break;
     case T_FACT_CR_REC:   
	  break;
     case T_FACT_CR_SOAK:  
	  break;
     case T_FACT_CR_REP:     
	  break;
     case T_FACT_REC_SOAK:    
	  break;
     case T_FACT_REC_REP:     
	  break;
     case T_FACT_SOAK_REP:
	  break;
     case T_TEST:
	  if (collect_timing_data_flag &&  test_flag) test_average ( sec, usec);
	  break;
/**	  
  if (collect_timing_data_flag &&  test_flag) test_average ( sec, usec);
 */
     default:
	  printf(" Unknown time constant: %d \n", funct_id);
     }
}


void send_message_average ( long sec, long usec)
{
     float total_time, average_time;

     nb_send_message++;
     total_sec_send_message += sec;
     total_usec_send_message += usec;
     if (total_usec_send_message >= ONE_MILLION) {
	   total_sec_send_message += 1;
	   total_usec_send_message -= ONE_MILLION;
      }
/*
     total_time =  total_sec_send_message + ((float)  total_usec_send_message ) /ONE_MILLION;
     average_time = total_time/ nb_send_message;
     printf( "send-message %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_send_message, total_time, average_time);
*/

     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_send_message < 0)  {
       min_sec_send_message = sec;
       min_usec_send_message = usec;
       max_sec_send_message = sec;
       max_usec_send_message = usec;
     }
     else if (sec < min_sec_send_message) {
       min_sec_send_message = sec;
       min_usec_send_message = usec;
     }
     else if ((sec == min_sec_send_message) && (usec < min_usec_send_message)) 
       min_usec_send_message = usec;
     else if (sec > max_sec_send_message) {
       max_sec_send_message = sec;
       max_usec_send_message = usec;
     }
     else if ((sec == max_sec_send_message) && (usec > max_usec_send_message))
       max_usec_send_message = usec;
       
}

void conclude_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_conclude++;
     total_sec_conclude += sec;
     total_usec_conclude += usec;
     if (total_usec_conclude >= ONE_MILLION) {
	   total_sec_conclude += 1;
	   total_usec_conclude -= ONE_MILLION;
      }
/*
     total_time =  total_sec_conclude + ((float)  total_usec_conclude ) /ONE_MILLION;
     average_time = total_time/ nb_conclude;
     printf( "conclude %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_conclude, total_time, average_time);
*/

     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};

     if (min_sec_conclude < 0)  {
       min_sec_conclude = sec;
       min_usec_conclude = usec;
       max_sec_conclude = sec;
       max_usec_conclude = usec;
     }
     else if (sec < min_sec_conclude) {
       min_sec_conclude = sec;
       min_usec_conclude = usec;
     }
     else if ((sec == min_sec_conclude) && (usec < min_usec_conclude)) 
       min_usec_conclude = usec;
     else if (sec > max_sec_conclude) {
       max_sec_conclude = sec;
       max_usec_conclude = usec;
     }
     else if ((sec == max_sec_conclude) && (usec > max_usec_conclude))
       max_usec_conclude = usec;
       
}

void retrieve_average ( long sec, long usec)
{
     float total_time, average_time;

     nb_retrieve++;
     total_sec_retrieve += sec;
     total_usec_retrieve += usec;
     if (total_usec_retrieve >= ONE_MILLION) {
	   total_sec_retrieve += 1;
	   total_usec_retrieve -= ONE_MILLION;
      }
/*
     total_time =  total_sec_retrieve + ((float)  total_usec_retrieve ) /ONE_MILLION;
     average_time = total_time/ nb_retrieve;
     printf( "retrieve %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_retrieve, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_retrieve < 0)  {
       min_sec_retrieve = sec;
       min_usec_retrieve = usec;
       max_sec_retrieve = sec;
       max_usec_retrieve = usec;
     }
     else if (sec < min_sec_retrieve) {
       min_sec_retrieve = sec;
       min_usec_retrieve = usec;
     }
     else if ((sec == min_sec_retrieve) && (usec < min_usec_retrieve)) 
       min_usec_retrieve = usec;
     else if (sec > max_sec_retrieve) {
       max_sec_retrieve = sec;
       max_usec_retrieve = usec;
     }
     else if ((sec == max_sec_retrieve) && (usec > max_usec_retrieve))
       max_usec_retrieve = usec;



}

void relevant_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_relevant++;
     total_sec_relevant += sec;
     total_usec_relevant += usec;
     if (total_usec_relevant >= ONE_MILLION) {
	   total_sec_relevant += 1;
	   total_usec_relevant -= ONE_MILLION;
      }
/*
     total_time =  total_sec_relevant + ((float)  total_usec_relevant ) /ONE_MILLION;
     average_time = total_time/ nb_relevant;
     printf( "relevant %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_relevant, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};

     if (min_sec_relevant < 0)  {
       min_sec_relevant = sec;
       min_usec_relevant = usec;
       max_sec_relevant = sec;
       max_usec_relevant = usec;
     }
     else if (sec < min_sec_relevant) {
       min_sec_relevant = sec;
       min_usec_relevant = usec;
     }
     else if ((sec == min_sec_relevant) && (usec < min_usec_relevant)) 
       min_usec_relevant = usec;
     else if (sec > max_sec_relevant) {
       max_sec_relevant = sec;
       max_usec_relevant = usec;
     }
     else if ((sec == max_sec_relevant) && (usec > max_usec_relevant))
       max_usec_relevant = usec;

}

void applic_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_applic++;
     total_sec_applic += sec;
     total_usec_applic += usec;
     if (total_usec_applic >= ONE_MILLION) {
	   total_sec_applic += 1;
	   total_usec_applic -= ONE_MILLION;
      }
/*
     total_time =  total_sec_applic + ((float)  total_usec_applic ) /ONE_MILLION;
     average_time = total_time/ nb_applic;
     printf( "applic %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_applic, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};



     if (min_sec_applic < 0)  {
       min_sec_applic = sec;
       min_usec_applic = usec;
       max_sec_applic = sec;
       max_usec_applic = usec;
     }
     else if (sec < min_sec_applic) {
       min_sec_applic = sec;
       min_usec_applic = usec;
     }
     else if ((sec == min_sec_applic) && (usec < min_usec_applic)) 
       min_usec_applic = usec;
     else if (sec > max_sec_applic) {
       max_sec_applic = sec;
       max_usec_applic = usec;
     }
     else if ((sec == max_sec_applic) && (usec > max_usec_applic))
       max_usec_applic = usec;

}

void int_create_average ( long sec, long usec)
{
     float total_time, average_time;

     nb_int_create++;
     total_sec_int_create += sec;
     total_usec_int_create += usec;
     if (total_usec_int_create >= ONE_MILLION) {
	   total_sec_int_create += 1;
	   total_usec_int_create -= ONE_MILLION;
      }
/*
     total_time =  total_sec_int_create + ((float)  total_usec_int_create ) /ONE_MILLION;
     average_time = total_time/ nb_int_create;
     printf( "int_create %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_int_create, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_int_create < 0)  {
       min_sec_int_create = sec;
       min_usec_int_create = usec;
       max_sec_int_create = sec;
       max_usec_int_create = usec;
     }
     else if (sec < min_sec_int_create) {
       min_sec_int_create = sec;
       min_usec_int_create = usec;
     }
     else if ((sec == min_sec_int_create) && (usec < min_usec_int_create)) 
       min_usec_int_create = usec;
     else if (sec > max_sec_int_create) {
       max_sec_int_create = sec;
       max_usec_int_create = usec;
     }
     else if ((sec == max_sec_int_create) && (usec > max_usec_int_create))
       max_usec_int_create = usec;

}

void int_active_average ( long sec, long usec)
{
     float total_time, average_time;

     nb_int_active++;
     total_sec_int_active += sec;
     total_usec_int_active += usec;
     if (total_usec_int_active >= ONE_MILLION) {
	   total_sec_int_active += 1;
	   total_usec_int_active -= ONE_MILLION;
      }
/*

     total_time =  total_sec_int_active + ((float)  total_usec_int_active ) /ONE_MILLION;
     average_time = total_time/ nb_int_active;
     printf( "int_active %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_int_active, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_int_active < 0)  {
       min_sec_int_active = sec;
       min_usec_int_active = usec;
       max_sec_int_active = sec;
       max_usec_int_active = usec;
     }
     else if (sec < min_sec_int_active) {
       min_sec_int_active = sec;
       min_usec_int_active = usec;
     }
     else if ((sec == min_sec_int_active) && (usec < min_usec_int_active)) 
       min_usec_int_active = usec;
     else if (sec > max_sec_int_active) {
       max_sec_int_active = sec;
       max_usec_int_active = usec;
     }
     else if ((sec == max_sec_int_active) && (usec > max_usec_int_active))
       max_usec_int_active = usec;

}

void int_in_graph_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_int_in_graph++;
     total_sec_int_in_graph += sec;
     total_usec_int_in_graph += usec;
     if (total_usec_int_in_graph >= ONE_MILLION) {
	   total_sec_int_in_graph += 1;
	   total_usec_int_in_graph -= ONE_MILLION;
      }
/*

     total_time =  total_sec_int_in_graph + ((float)  total_usec_int_in_graph ) /ONE_MILLION;
     average_time = total_time/ nb_int_in_graph;
     printf( "int_in_graph %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_int_in_graph, total_time, average_time);
*/ 

     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_int_in_graph < 0)  {
       min_sec_int_in_graph = sec;
       min_usec_int_in_graph = usec;
       max_sec_int_in_graph = sec;
       max_usec_int_in_graph = usec;
     }
     else if (sec < min_sec_int_in_graph) {
       min_sec_int_in_graph = sec;
       min_usec_int_in_graph = usec;
     }
     else if ((sec == min_sec_int_in_graph) && (usec < min_usec_int_in_graph)) 
       min_usec_int_in_graph = usec;
     else if (sec > max_sec_int_in_graph) {
       max_sec_int_in_graph = sec;
       max_usec_int_in_graph = usec;
     }
     else if ((sec == max_sec_int_in_graph) && (usec > max_usec_int_in_graph))
       max_usec_int_in_graph = usec;

}

void handle_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_handle++;
     total_sec_handle += sec;
     total_usec_handle += usec;
     if (total_usec_handle >= ONE_MILLION) {
	   total_sec_handle += 1;
	   total_usec_handle -= ONE_MILLION;
      }
/*
     total_time =  total_sec_handle + ((float)  total_usec_handle ) /ONE_MILLION;
     average_time = total_time/ nb_handle;
     printf( "handle %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_handle, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_handle < 0)  {
       min_sec_handle = sec;
       min_usec_handle = usec;
       max_sec_handle = sec;
       max_usec_handle = usec;
     }
     else if (sec < min_sec_handle) {
       min_sec_handle = sec;
       min_usec_handle = usec;
     }
     else if ((sec == min_sec_handle) && (usec < min_usec_handle)) 
       min_usec_handle = usec;
     else if (sec > max_sec_handle) {
       max_sec_handle = sec;
       max_usec_handle = usec;
     }
     else if ((sec == max_sec_handle) && (usec > max_usec_handle))
       max_usec_handle = usec;

}


void goal_average ( long sec, long usec)
{
     float total_time, average_time;


     nb_goal++;
     total_sec_goal += sec;
     total_usec_goal += usec;
     if (total_usec_goal >= ONE_MILLION) {
	   total_sec_goal += 1;
	   total_usec_goal -= ONE_MILLION;
      }
/*
     total_time =  total_sec_goal + ((float)  total_usec_goal ) /ONE_MILLION;
     average_time = total_time/ nb_goal;
     printf( "goal %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_goal, total_time, average_time);
*/


     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};


     if (min_sec_goal < 0)  {
       min_sec_goal = sec;
       min_usec_goal = usec;
       max_sec_goal = sec;
       max_usec_goal = usec;
     }
     else if (sec < min_sec_goal) {
       min_sec_goal = sec;
       min_usec_goal = usec;
     }
     else if ((sec == min_sec_goal) && (usec < min_usec_goal)) 
       min_usec_goal = usec;
     else if (sec > max_sec_goal) {
       max_sec_goal = sec;
       max_usec_goal = usec;
     }
     else if ((sec == max_sec_goal) && (usec > max_usec_goal))
       max_usec_goal = usec;

}


void test_average ( long sec, long usec)
{
     float total_time, average_time;

     nb_test++;
     total_sec_test += sec;
     total_usec_test += usec;
     if (total_usec_test >= ONE_MILLION) {
	   total_sec_test += 1;
	   total_usec_test -= ONE_MILLION;
      }
/*
     total_time =  total_sec_test + ((float)  total_usec_test ) /ONE_MILLION;
     average_time = total_time/ nb_test;
     printf( "test %lds%6ldus, nb: %d total_time: %f average: %f\n", sec, usec,
	    nb_test, total_time, average_time);
*/

     if (wall_clock_flag) {

	  t = time(NULL);
	  s_systime = localtime(&t);
	  shr = s_systime->tm_hour;
	  smin = s_systime->tm_min;
	  ssec = s_systime->tm_sec; 
	  ehr = shr;
	  emin = smin;
	  esec = sec;
  	  wall_clock_flag = 0;
	}
     else {
       
       	  t = time(NULL);
	  s_systime = localtime(&t);
	  ehr = s_systime->tm_hour;
	  emin = s_systime->tm_min;
	  esec = s_systime->tm_sec; 

	};

     if (min_sec_test < 0)  {
       min_sec_test = sec;
       min_usec_test = usec;
       max_sec_test = sec;
       max_usec_test = usec;
     }
     else if (sec < min_sec_test) {
       min_sec_test = sec;
       min_usec_test = usec;
     }
     else if ((sec == min_sec_test) && (usec < min_usec_test)) 
       min_usec_test = usec;
     else if (sec > max_sec_test) {
       max_sec_test = sec;
       max_usec_test = usec;
     }
     else if ((sec == max_sec_test) && (usec > max_usec_test))
       max_usec_test = usec;


}



void user_time_reset_all ()
{
     nb_send_message = 0; total_sec_send_message =0; total_usec_send_message = 0;
     nb_conclude = 0;     total_sec_conclude =0;     total_usec_conclude = 0;
     nb_retrieve = 0;     total_sec_retrieve =0;     total_usec_retrieve = 0;
     nb_relevant = 0;     total_sec_relevant =0;     total_usec_relevant = 0;
     nb_applic = 0;       total_sec_applic =0;       total_usec_applic = 0;
     nb_int_create = 0;   total_sec_int_create =0;   total_usec_int_create = 0;
     nb_int_active = 0;   total_sec_int_active =0;   total_usec_int_active = 0;
     nb_int_in_graph = 0; total_sec_int_in_graph =0; total_usec_int_in_graph = 0;
     nb_handle = 0;       total_sec_handle =0;       total_usec_handle = 0;
     nb_goal = 0;       total_sec_goal =0;       total_usec_goal = 0;
     nb_test = 0;       total_sec_test =0;       total_usec_test = 0;

     min_sec_send_message =-1; min_usec_send_message = -1;
     min_sec_conclude =-1;     min_usec_conclude = -1;
     min_sec_retrieve =-1;     min_usec_retrieve = -1;
     min_sec_relevant =-1;     min_usec_relevant = -1;
     min_sec_applic =-1;       min_usec_applic = -1;
     min_sec_int_create =-1;   min_usec_int_create = -1;
     min_sec_int_active =-1;   min_usec_int_active = -1;
     min_sec_int_in_graph =-1; min_usec_int_in_graph = -1;
     min_sec_handle =-1;       min_usec_handle = -1;
     min_sec_goal =-1;       min_usec_goal = -1;
     min_sec_test =-1;       min_usec_test = -1;

     max_sec_send_message =-1; max_usec_send_message = -1;
     max_sec_conclude =-1;     max_usec_conclude = -1;
     max_sec_retrieve =-1;     max_usec_retrieve = -1;
     max_sec_relevant =-1;     max_usec_relevant = -1;
     max_sec_applic =-1;       max_usec_applic = -1;
     max_sec_int_create =-1;   max_usec_int_create = -1;
     max_sec_int_active =-1;   max_usec_int_active = -1;
     max_sec_int_in_graph =-1; max_usec_int_in_graph = -1;
     max_sec_handle =-1;       max_usec_handle = -1;
     max_sec_goal =-1;       max_usec_goal = -1;
     max_sec_test =-1;       max_usec_test = -1;

     
}

void user_time_print_all ()
{
     float total_time, average_time, min, max;

printf ("\n\n");  /* Ensure we start on new line */


  printf("\nStart-Time  %.2d:%.2d:%.2d ", shr, smin, ssec);

  printf("\nEnd-Time  %.2d:%.2d:%.2d ", ehr, emin, esec);

  printf("\nDate %.2d-%.2d-%.2d\n", s_systime->tm_mon+1,
	  s_systime->tm_mday, s_systime->tm_year);



/* send_message */
     if(send_message_flag) {
	  total_time =  total_sec_send_message + ((float)  total_usec_send_message ) /ONE_MILLION;
	  average_time = total_time/ nb_send_message;
	  min = min_sec_send_message + ((float) min_usec_send_message) /ONE_MILLION;
	  max = max_sec_send_message + ((float) max_usec_send_message /ONE_MILLION);
	  printf( "send_message - \tnb: %d total_time: %f average: %f min: %f max: %f\n", 
	    nb_send_message, total_time, average_time, min, max);
     }

/* conclude */
     if(conclude_flag) {
	  total_time =  total_sec_conclude + ((float)  total_usec_conclude ) /ONE_MILLION;
	  average_time = total_time/ nb_conclude;
	  min = min_sec_conclude + ((float) min_usec_conclude) /ONE_MILLION;
	  max = max_sec_conclude + ((float) max_usec_conclude) /ONE_MILLION;
	  printf( "conclude - \tnb: %d total_time: %f average: %f min: %f max: %f\n",   nb_conclude, total_time, average_time, min, max);
     }

/* retrieve */
     if(retrieve_flag) {
	  total_time =  total_sec_retrieve + ((float)  total_usec_retrieve ) /ONE_MILLION;
	  average_time = total_time/ nb_retrieve;
	  min = min_sec_retrieve + ((float) min_usec_retrieve) /ONE_MILLION;
	  max = max_sec_retrieve + ((float) max_usec_retrieve) /ONE_MILLION;
	  printf( "retrieve - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_retrieve, total_time, average_time, min, max);
     }

/* relevant */
     if(relevant_flag) {
	  total_time =  total_sec_relevant + ((float)  total_usec_relevant ) /ONE_MILLION;
	  average_time = total_time/ nb_relevant;
	  min = min_sec_relevant + ((float) min_usec_relevant) /ONE_MILLION;
	  max = max_sec_relevant + ((float) max_usec_relevant) /ONE_MILLION;
	  printf( "relevant - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_relevant, total_time, average_time, min, max);
     }

/* applic */
     if(applic_flag) {
	  total_time =  total_sec_applic + ((float)  total_usec_applic ) /ONE_MILLION;
	  average_time = total_time/ nb_applic;
	  min = min_sec_applic + ((float) min_usec_applic) /ONE_MILLION;
	  max = max_sec_applic + ((float) max_usec_applic) /ONE_MILLION;
	  printf( "applic - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_applic, total_time, average_time, min, max);
     }

/* int_create */
     if(int_create_flag) {
	  total_time =  total_sec_int_create + ((float)  total_usec_int_create ) /ONE_MILLION;
	  average_time = total_time/ nb_int_create;
	  min = min_sec_int_create + ((float) min_usec_int_create) /ONE_MILLION;
	  max = max_sec_int_create + ((float) max_usec_int_create) /ONE_MILLION;
	  printf( "int_create - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_int_create, total_time, average_time, min, max);
     }

/* int_active */
     if(int_active_flag) {
	  total_time =  total_sec_int_active + ((float)  total_usec_int_active ) /ONE_MILLION;
	  average_time = total_time/ nb_int_active;
	  min = min_sec_int_active + ((float) min_usec_int_active) /ONE_MILLION;
	  max = max_sec_int_active + ((float) max_usec_int_active) /ONE_MILLION;
	  printf( "int_active - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_int_active, total_time, average_time, min, max);
     }

/* int_in_graph */
     if(int_in_graph_flag) {
	  total_time =  total_sec_int_in_graph + ((float)  total_usec_int_in_graph ) /ONE_MILLION;
	  average_time = total_time/ nb_int_in_graph;
	  min = min_sec_int_in_graph + ((float) min_usec_int_in_graph) /ONE_MILLION;
	  max = max_sec_int_in_graph + ((float) max_usec_int_in_graph) /ONE_MILLION;
	  printf( "int_in_graph - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_int_in_graph, total_time, average_time, min, max);
     }

/* handle */
     if(handle_flag) {
	  total_time =  total_sec_handle + ((float)  total_usec_handle ) /ONE_MILLION;
	  average_time = total_time/ nb_handle;
	  min = min_sec_handle + ((float) min_usec_handle) /ONE_MILLION;
	  max = max_sec_handle + ((float) max_usec_handle) /ONE_MILLION;
	  printf( "handle - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_handle, total_time, average_time, min, max);
     }


/* goal */
     if(goal_flag) {
	  total_time =  total_sec_goal + ((float)  total_usec_goal ) /ONE_MILLION;
	  average_time = total_time/ nb_goal;
	  min = min_sec_goal + ((float) min_usec_goal) /ONE_MILLION;
	  max = max_sec_goal + ((float) max_usec_goal) /ONE_MILLION;
	  printf( "goal - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_goal, total_time, average_time, min, max);
     }


/* test */
     if(test_flag) {
	  total_time =  total_sec_test + ((float)  total_usec_test ) /ONE_MILLION;
	  average_time = total_time/ nb_test;
	  min = min_sec_test + ((float) min_usec_test) /ONE_MILLION;
	  max = max_sec_test + ((float) max_usec_test) /ONE_MILLION;
	  printf( "test - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_test, total_time, average_time, min, max);
     }

     if (send_flag && receive_flag) {
	  int i;
	  long sec, usec;

	  nb_test = 0;
	  total_sec_test = 0;
	  total_usec_test = 0;
	  min_sec_test = -1;
	  min_usec_test = -1;
	  max_sec_test = -1;
	  max_usec_test = -1;
	  nb_test = 0;
	  for (i = 0; i < MAX_SAVE; i++) 
	       if ((send_sec[i] != 0) && (receive_sec[i] != 0 )) {
		    sec = receive_sec[i] - send_sec[i];
		    usec = receive_usec[i] - send_usec[i];
		    if (usec < 0L){
			 usec += ONE_MILLION;
			 sec -= 1;
		    }
		    nb_test++;
		    total_sec_test += sec;
		    total_usec_test += usec;
		    if (total_usec_test >= ONE_MILLION) {
			 total_sec_test += 1;
			 total_usec_test -= ONE_MILLION;
		    }
		    if (min_sec_test < 0)  {
			 min_sec_test = sec;
			 min_usec_test = usec;
			 max_sec_test = sec;
			 max_usec_test = usec;
		    }
		    else if (sec < min_sec_test) {
			 min_sec_test = sec;
			 min_usec_test = usec;
		    }
		    else if ((sec == min_sec_test) && (usec < min_usec_test)) 
			 min_usec_test = usec;
		    else if (sec > max_sec_test) {
			 max_sec_test = sec;
			 max_usec_test = usec;
		    }
		    else if ((sec == max_sec_test) && (usec > max_usec_test))
			 max_usec_test = usec;
	       }
	  
	  total_time =  total_sec_test + ((float)  total_usec_test ) /ONE_MILLION;
	  average_time = total_time/ nb_test;
	  min = min_sec_test + ((float) min_usec_test) /ONE_MILLION;
	  max = max_sec_test + ((float) max_usec_test) /ONE_MILLION;
	  printf( "test - \tnb: %d total_time: %f average: %f min: %f max: %f\n", nb_test, total_time, average_time, min, max);
     }
}


void append_data_to_file ()
{
     float total_time, average_time, min, max;
     char ans[4] = {NULL};


  if (fname[0] == NULL) {
    get_fname();
    printf("\nOpening file: %s", &fname);
    if((pfname = fopen((char *) &fname, "a+")) == NULL) {
      printf("Error opening file %s,\n verify what was typed in and retry.", &fname);
      return;
    };
  }
  else {
    printf("\nAppend data to the file named: %s ?\n Type Y/y or N/n then hit return: ", &fname);
    gets(ans);

    while (! ((ans[0]== 'N') || (ans[0]== 'n') || (ans[0]== 'Y') || (ans[0]== 'y'))) {
      printf("\nYou did not give a Y/y or N/n answer to the last question, try again");
      printf("\nAppend data to the file named: %s ?\n Type Y/y or N/n then hit return: ", &fname);
      gets(ans);
    };

    
    if ((ans[0]== 'N') || (ans[0]== 'n')) {
      fname[0] = NULL;
      write_data_to_file();
      return;
    }
    else if ((ans[0]== 'Y') || (ans[0]== 'y')) {
      printf("\nOpening file: %s", &fname);
      if((pfname = fopen((char *) &fname, "a+")) == NULL) {
	printf("\nError opening file %s, \nverify what you typed in and retry append operation.", &fname);
	return;
    };
    }
  };
 
     get_data_id();

     write_it();

     printf("\n\nFinished appending data to file: %s", &fname);

}



void write_data_to_file ()
{
     float total_time, average_time, min, max;
     char ans[2] ={NULL};


  if (fname[0] == NULL) {
    get_fname();
    printf("\nOpening file: %s", &fname);
    if((pfname = fopen((char *) &fname, "w")) == NULL) {
      printf("Error opening file %s,\n verify what was typed in and retry.", &fname);
      return;
    };
  }
  else {
    printf("\nWrite data to the file named: %s ?\n Type Y/y or N/n then hit return:", &fname);
    gets(ans);

    while (! ((ans[0] == 'N') || (ans[0] == 'n') || (ans[0] == 'Y') || (ans[0] == 'y'))) {
      printf("\nYou did not give a Y./y or N/n answer to the last question, try again");
      printf("\nWrite data to the file named: %s ?\n Type Y/y or N/n then hit return: ", &fname);
      gets(ans);
    };

    
    if ((ans[0] == 'N') || (ans[0] == 'n')) {
      fname[0] = NULL;
      write_data_to_file();
      return;
    }
    else if ((ans[0] == 'Y') || (ans[0] == 'y')) {
      printf("\nOpening file: %s", &fname);
      if((pfname = fopen((char *) &fname, "w")) == NULL) {
	printf("\nError opening file %s,\n verify what you typed in and retry.", &fname);
	return;
    };
    }
  };
 
     get_data_id();

     write_it();

     printf("\n\nFinished writing data to file: %s", &fname);


}



void get_data_id ()
{

  char ans[4] = {NULL};


  printf("\n\nEnter in text you wish to identify with this data\n(e.g., BENCHMARK 1), NOTE: do not inclose text in \"...\" :\n");
  gets(data_id);
  printf("\nYou specified  %s, is that what you want? Type Y/y or N/n: ", &data_id);
  gets(ans);

  while (! ((ans[0] == 'Y') || (ans[0] == 'y'))) {
    while (! ((ans[0] == 'Y') || (ans[0] == 'y') || (ans[0] == 'N') || (ans[0] == 'n'))) {
    printf("\n\nYou did not give a Y/y or N/n answer to the last question, try again.");
    printf("\nYou specified  %s, is that what you want? Type Y/y or N/n: ", &data_id);
    gets(ans);
  };

    if ((ans[0] == 'N') || (ans[0] == 'n')) {
      printf("\nEnter in text you wish to identify with this data\n(e.g., BENCHMARK 1): NOTE: do not inclose text in \"...\" :\n");
      gets(data_id);
      printf("\n You specified  %s , is that what you want? Type Y/y or N/n: ", &data_id);
      gets(ans);
    };
  };
}



void get_fname ()
{

  printf("\n\n Enter in EXPANDED pathname of file to append data\n 
(e.g., /tmp_mnt/home/file),  NOTE: do not inclose name in \"...\": .\n");
  gets(fname);
}




void write_it()
{

     float total_time, average_time, min, max;


     fprintf(pfname,"("); /*  Opening left ( so Lisp can readin data as s-expression */

     fprintf(pfname, "(Data-ID  %s )", data_id); 

     fprintf(pfname, "\n(Start-Time  %.2d %.2d %.2d) ", shr, smin, ssec);

     fprintf(pfname, "\n(End-Time  %.2d %.2d %.2d) ", ehr, emin, esec);

     fprintf(pfname, "\n(Date %.2d-%.2d-%.2d)\n", s_systime->tm_mon+1,
	     s_systime->tm_mday, s_systime->tm_year);


/* send_message */
     if (send_message_flag) {
	  total_time =  total_sec_send_message + ((float)  total_usec_send_message ) /ONE_MILLION;
	  average_time = total_time/ nb_send_message;
	  min = min_sec_send_message + ((float) min_usec_send_message) /ONE_MILLION;
	  max = max_sec_send_message + ((float) max_usec_send_message /ONE_MILLION);
	  fprintf(pfname, "(send_message \tnb %d total_time %f average %f\n\tmin %f max %f)\n", 
		  nb_send_message, total_time, average_time, min, max);
     }

/* conclude */
     if (conclude_flag) {
	  total_time =  total_sec_conclude + ((float)  total_usec_conclude ) /ONE_MILLION;
	  average_time = total_time/ nb_conclude;
	  min = min_sec_conclude + ((float) min_usec_conclude) /ONE_MILLION;
	  max = max_sec_conclude + ((float) max_usec_conclude) /ONE_MILLION;
	  fprintf(pfname, "(conclude \tnb %d total_time %f average %f\n\tmin %f max %f)\n",   nb_conclude, total_time, average_time, min, max);
     }

/* retrieve */
     if (retrieve_flag) {
	  total_time =  total_sec_retrieve + ((float)  total_usec_retrieve ) /ONE_MILLION;
	  average_time = total_time/ nb_retrieve;
	  min = min_sec_retrieve + ((float) min_usec_retrieve) /ONE_MILLION;
	  max = max_sec_retrieve + ((float) max_usec_retrieve) /ONE_MILLION;
	  fprintf(pfname, "(retrieve \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_retrieve, total_time, average_time, min, max);
     }

/* relevant */
     if (relevant_flag) {
	  total_time =  total_sec_relevant + ((float)  total_usec_relevant ) /ONE_MILLION;
	  average_time = total_time/ nb_relevant;
	  min = min_sec_relevant + ((float) min_usec_relevant) /ONE_MILLION;
	  max = max_sec_relevant + ((float) max_usec_relevant) /ONE_MILLION;
	  fprintf(pfname, "(relevant \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_relevant, total_time, average_time, min, max);
     }

/* applic */
     if (applic_flag) {
	  total_time =  total_sec_applic + ((float)  total_usec_applic ) /ONE_MILLION;
	  average_time = total_time/ nb_applic;
	  min = min_sec_applic + ((float) min_usec_applic) /ONE_MILLION;
	  max = max_sec_applic + ((float) max_usec_applic) /ONE_MILLION;
	  fprintf(pfname, "(applic \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_applic, total_time, average_time, min, max);
     }

/* int_create */
     if (int_create_flag) {
	  total_time =  total_sec_int_create + ((float)  total_usec_int_create ) /ONE_MILLION;
	  average_time = total_time/ nb_int_create;
	  min = min_sec_int_create + ((float) min_usec_int_create) /ONE_MILLION;
	  max = max_sec_int_create + ((float) max_usec_int_create) /ONE_MILLION;
	  fprintf(pfname, "(int_create \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_int_create, total_time, average_time, min, max);
     }

/* int_active */
     if (int_active_flag) {
	  total_time =  total_sec_int_active + ((float)  total_usec_int_active ) /ONE_MILLION;
	  average_time = total_time/ nb_int_active;
	  min = min_sec_int_active + ((float) min_usec_int_active) /ONE_MILLION;
	  max = max_sec_int_active + ((float) max_usec_int_active) /ONE_MILLION;
	  fprintf(pfname, "(int_active \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_int_active, total_time, average_time, min, max);
     }

/* int_in_graph */
     if (int_in_graph_flag) {
	  total_time =  total_sec_int_in_graph + ((float)  total_usec_int_in_graph ) /ONE_MILLION;
	  average_time = total_time/ nb_int_in_graph;
	  min = min_sec_int_in_graph + ((float) min_usec_int_in_graph) /ONE_MILLION;
	  max = max_sec_int_in_graph + ((float) max_usec_int_in_graph) /ONE_MILLION;
	  fprintf(pfname, "(int_in_graph \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_int_in_graph, total_time, average_time, min, max);
     }

/* handle */
     if (handle_flag) {
	  total_time =  total_sec_handle + ((float)  total_usec_handle ) /ONE_MILLION;
	  average_time = total_time/ nb_handle;
	  min = min_sec_handle + ((float) min_usec_handle) /ONE_MILLION;
	  max = max_sec_handle + ((float) max_usec_handle) /ONE_MILLION;
	  fprintf(pfname, "(handle \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_handle, total_time, average_time, min, max);
     }

/* goal */
     if (goal_flag) {
	  total_time =  total_sec_goal + ((float)  total_usec_goal ) /ONE_MILLION;
	  average_time = total_time/ nb_goal;
	  min = min_sec_goal + ((float) min_usec_goal) /ONE_MILLION;
	  max = max_sec_goal + ((float) max_usec_goal) /ONE_MILLION;
	  fprintf(pfname, "(goal \tnb %d total_time %f average %f\n\tmin %f max %f)\n", nb_goal, total_time, average_time, min, max);
     }


/* test */
     if (test_flag) {
	  total_time =  total_sec_test + ((float)  total_usec_test ) /ONE_MILLION;
	  average_time = total_time/ nb_test;
	  min = min_sec_test + ((float) min_usec_test) /ONE_MILLION;
	  max = max_sec_test + ((float) max_usec_test) /ONE_MILLION;
	  fprintf(pfname, "(test \tnb %d total_time %f average %f\n\tmin %f max %f))\n", nb_test, total_time, average_time, min, max);
     }



     fprintf(pfname, ")\n\n\n");  /* Closes off s-expression  */

     fclose(pfname); /* Close file  */
     printf("\nFile %s closed.\n", &fname);


}

void init_send_save(void)
{
     int i;

     for (i = 0; i < MAX_SAVE; i++)
	  send_sec[i] = send_usec[i] = 0;
}

void init_receive_save(void)
{
     int i;

     for (i = 0; i < MAX_SAVE; i++)
	  receive_sec[i] = receive_usec[i] = 0;
}

void send_save (int index, long sec, long usec)
{
     if (index < MAX_SAVE) {
	  send_sec[index] = sec;
	  send_usec[index] = usec;
     }
}

void receive_save (int index, long sec, long usec)
{
     if (index < MAX_SAVE) {
	  receive_sec[index] = sec;
	  receive_usec[index] = usec;
     }
}
