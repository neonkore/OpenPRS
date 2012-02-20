
/*                               -*- Mode: C -*- 
 * oprs-profiling.c -- 
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


#include <stdio.h>

#ifdef WIN95
#include <winsock.h>
#else
#ifdef VXWORKS
#include <systime.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif
#endif

#include "macro.h"

#include "oprs-profiling.h"
#include "oprs-type.h"

#include "oprs-profiling_f.h"


#define ONE_MILLION 1000000

PDate zero_date = {0L, 0L};	/* Begining of the time... */


void time_stamp(PDate *pd)
{
     struct timezone tzp;
	  
     if (GETTIMEOFDAY(pd,&tzp) != 0)
	  perror("time_stamp:gettimeofday");
}

#ifdef VXWORKS 
#include "time.h"

int oprs_gettimeofday(struct timeval *tp)
{
     struct timespec tsp;
     int error;

     if ((error = clock_gettime(CLOCK_REALTIME, &tsp)) != 0) {
	  perror("oprs_gettimeofday:clock_gettime");
	  return error;
     }

     tp->tv_usec = tsp.tv_nsec / 1000;
     tp->tv_sec = tsp.tv_sec;

    return(0);
}

#endif

#ifdef OPRS_PROFILING

/*
 * Itimers for profiling section.
 *
 */

void init_profile_timer(void)
{
     struct itimerval end_of_time;

     end_of_time.it_value.tv_sec = 99999999;	/* According to the Solaris Man page, this is the
						   highest acceptable value. */
     end_of_time.it_value.tv_usec = 0L;

     end_of_time.it_interval.tv_sec = 0L;
     end_of_time.it_interval.tv_usec = 0L;

     if (setitimer(ITIMER_PROF, &end_of_time, NULL) == -1)
	  perror("init_profile_timer:setitimer");
}

void get_profile_timer(struct timeval *tp)
{
     struct itimerval new;

     if (getitimer(ITIMER_PROF, &new) == -1)
	  perror("get_profile_timer:getitimer");

     tp->tv_sec = new.it_value.tv_sec;
     tp->tv_usec = new.it_value.tv_usec;
}


void add_profile_timer(struct timeval *old, struct timeval *result)
{
     struct itimerval new;

     if (getitimer(ITIMER_PROF, &new) == -1)
	  perror("add_profile_timer:getitimer");

     result->tv_sec += old->tv_sec - new.it_value.tv_sec;
     result->tv_usec += old->tv_usec - new.it_value.tv_usec;

     if (result->tv_usec < 0L){
	  result->tv_sec -= 1;
	  result->tv_usec += ONE_MILLION;
     } else if (result->tv_usec >= ONE_MILLION){
	  result->tv_sec += 1;
	  result->tv_usec -= ONE_MILLION;
     }
}

void time_start(PDate *pd)
{
     struct timezone tzp;

     if (GETTIMEOFDAY(pd,&tzp) != 0)
	  perror("gettimeofday");
}

void user_time(int i , long i2 , long i3)
{
}

void time_stop(PDate *pd,int funct_id)
{
     struct timezone tzp;
     PDate pd2;
     long sec, usec;

     if (GETTIMEOFDAY(&pd2,&tzp) != 0)
	  perror("gettimeofday");
    
     sec = pd2.tv_sec - pd->tv_sec;
     usec = pd2.tv_usec - pd->tv_usec;
     if (usec < 0L){
	  usec += ONE_MILLION;
	  sec -= 1;

     }
     user_time (funct_id, sec, usec);
}

void time_add(PDate *result, PDate *pd)
{
     struct timezone tzp;
     PDate pd2;

     if (GETTIMEOFDAY(&pd2,&tzp) != 0)
	  perror("gettimeofday");
    
     result->tv_sec += pd2.tv_sec - pd->tv_sec;
     result->tv_usec += pd2.tv_usec - pd->tv_usec;
     if (result->tv_usec < 0L){
	  result->tv_sec -= 1;
	  result->tv_usec += ONE_MILLION;
     } else if (result->tv_usec >= ONE_MILLION){
	  result->tv_sec += 1;
	  result->tv_usec -= ONE_MILLION;
     }
}

void time_report(PDate *pd,int funct_id)
{
     long sec, usec;

     sec = pd->tv_sec;
     usec = pd->tv_usec;
   
     user_time (funct_id, sec, usec);
}

void time_note(int index, int funct_id)
{
     struct timezone tzp;
     PDate pd;

     if (GETTIMEOFDAY(&pd,&tzp) != 0)
	  perror("gettimeofday");
#ifdef OPRS_DEBUG
     printf ("time : %ld.%6ld \n", pd.tv_sec, pd.tv_usec);
#endif /* OPRS_DEBUG */

}
#endif

#ifdef OPRS_PROFILING
void reset_profiled_data_db(void)
{
}

void reset_profiled_data_action_op(void)
{
}

void reset_profiled_data_op(void)
{
}

void reset_profiled_data_ep(void)
{
}

void reset_profiled_data_ef(void)
{
}

void reset_profiled_data_ac(void)
{
}


void report_profiled_data_db(void)
{
}

void report_profiled_data_action_op(void)
{
}

void report_profiled_data_op(void)
{
}

void report_profiled_data_ep(void)
{
}

void report_profiled_data_ef(void)
{
}

void report_profiled_data_ac(void)
{
}
#endif

void report_profiled_data(void)
{
#ifdef OPRS_PROFILING
     if (profiling_option[PROFILING]) {
	  if (profiling_option[PROF_DB]) {
	       report_profiled_data_db();
	  }
	  if (profiling_option[PROF_ACTION_OP]) {
	       report_profiled_data_action_op();
	  }
	  if (profiling_option[PROF_OP]) {
	       report_profiled_data_op();
	  }
	  if (profiling_option[PROF_EP]) {
	       report_profiled_data_ep();
	  }
	  if (profiling_option[PROF_EF]) {
	       report_profiled_data_ef();
	  }
	  if (profiling_option[PROF_AC]) {
	       report_profiled_data_ac();
	  }
     } else 
	  fprintf(stderr,"Profiling is disabled, no data to report.");
#else
     fprintf(stderr,"Profiling code not present in this kernel.\n\
Use a OPRS_PROFILE kernel or relocatable.");
#endif
}

void reset_profiled_data(void)
{
#ifdef OPRS_PROFILING
     reset_profiled_data_db();
     reset_profiled_data_action_op();
     reset_profiled_data_op();
     reset_profiled_data_ep();
     reset_profiled_data_ef();
     reset_profiled_data_ac();
#else
     fprintf(stderr,"Profiling code not present in this kernel.\n\
Use a OPRS_PROFILE kernel or relocatable.");
#endif
}

