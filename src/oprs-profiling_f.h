/*                               -*- Mode: C -*- 
 * oprs-profiling_f.h -- 
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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


#include "oprs-profiling.h"

void time_stamp(PDate *pd);
void time_start(PDate *pd);
void time_stop(PDate *pd,int funct_id);
void time_report(PDate *pd,int funct_id);
void time_add(PDate *result,PDate *pd);
void time_note(int index, int funct_id);

/* #ifdef VXWORKS */
/* int gettimeofday(struct timeval *tp, struct timezone *tzp); */
/* #endif */

#define TIME_START(date) time_start(date)

#define TIME_STOP(date,code) time_stop(date,code)

#define TIME_REPORT(date,code) time_report(date,code)

#define TIME_ADD(result,date) time_add(result,date)

#define INIT_PROFILE_TIMER() init_profile_timer() 

#define GET_PROFILE_TIMER(tp) get_profile_timer(tp)

#define ADD_PROFILE_TIMER(old,result) add_profile_timer(old,result)

#define TIME_STAMP(date) time_stamp(date)

void reset_profiled_data_db(void);
void report_profiled_data(void);
void reset_profiled_data(void);
