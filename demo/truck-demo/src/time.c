/*                               -*- Mode: C -*- 
 * time.c -- 
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

#ifdef VX_WORKS
#include "systime.h"
#else
#include <math.h>
#include <sys/time.h>
#endif

#include "local-time_f.h"
#include "demo.h"

double pow(double, double);

#define ONE_MILLION 1000000

static long local_time, last_stamped_local_time;
static struct timeval last_stamped_real_time;

static float speed;
static int speed_indice, min_indice, max_indice;

static int halted;

void init_time(int initial_time)
{
     struct timeval real_time;
     struct timezone tzp;

     if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");
     
     local_time = last_stamped_local_time= initial_time;
     last_stamped_real_time = real_time;
	 
     speed = 1.0;
     speed_indice = 0;
     max_indice = 0;
     min_indice = 0;
     halted = 0;
}

void init_speed(int min_i, int max_i)
{
     if (min_i >0) {
	  perror (LG_STR("minimal speed indice value must be negative or null",
			 "minimal speed indice value must be negative or null"));
	  min_indice = 0;
     } else
	  min_indice = min_i;

     if (max_i <0) {
	  perror (LG_STR("maximal speed indice value must be positive or null",
			 "maximal speed indice value must be positive or null"));
	  max_indice = 0;
     } else
	  max_indice = max_i;
}

int get_local_time ()
{
     long sec, usec;
     struct timeval real_time;
     struct timezone tzp;

     if (halted)
	  return(local_time);

     if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");

     /* Elapsed real time since last stamp */
	
     sec = real_time.tv_sec - last_stamped_real_time.tv_sec;
     usec = real_time.tv_usec - last_stamped_real_time.tv_usec;
     if (usec < 0L){
	  usec += ONE_MILLION;
	  sec -= 1;
     }
     sec *= speed;
     usec *= speed;
	  
     sec += usec/ONE_MILLION;
     
     local_time = last_stamped_local_time + sec;
     
     return((int) local_time);
}

void halt_time ()      /* stamp then halt */
{
     stamp_time();
     halted = 1;
}

void wake_up_time ()
{
     struct timeval real_time;
     struct timezone tzp;

     if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");

     last_stamped_real_time = real_time;
     halted = 0;
}

void stamp_time ()
{
     long sec, usec;
     struct timeval real_time;
     struct timezone tzp;

     if (halted) /* the time has already been stamped : do nothing */
	  return;

     if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");

     /* Elapsed real time since last stamp */
	
     sec = real_time.tv_sec - last_stamped_real_time.tv_sec;
     usec = real_time.tv_usec - last_stamped_real_time.tv_usec;
     if (usec < 0L){
	  usec += ONE_MILLION;
	  sec -= 1;
     }
     sec *= speed;
     usec *= speed;
	  
     sec += usec/ONE_MILLION;
     
     last_stamped_real_time = real_time;
     last_stamped_local_time += sec;
}

void set_speed(int new_value)
{
     if (speed_indice == new_value) /* nothing to do */
	  return;
     if ((new_value > max_indice) || (new_value < min_indice)){
	  perror(LG_STR("set_speed: value out of range",
			"set_speed: value out of range"));
	  return;
     }

/* stamp the time and then change the speed */

     stamp_time();

     speed_indice = new_value;
     speed = pow ((double) 2 , (double) speed_indice);
}

