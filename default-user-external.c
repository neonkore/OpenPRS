static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * default-user-external.c -- 
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
#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include "opaque-pub.h"
#include "user-external.h"

#include "intention_f-pub.h"
#include "user-external_f.h"

PBoolean my_intention_list_sort_example(Intention *i1, Intention *i2)
{
     return (intention_priority(i1) > intention_priority(i2));
}

void start_kernel_user_hook()
{
     intention_scheduler = &intention_scheduler_time_sharing; 
     /* intention_scheduler = NULL; */
     /* The next statement is useless, if the intention sheduler is not NULL
	(as it has priotity over intention sorting.) */
     intention_list_sort_predicate = &intention_list_sort_by_priority;
     main_loop_pool_sec = 0L;
     main_loop_pool_usec = 10000L;
}


void end_kernel_user_hook()
{
     printf("Bye, bye...\n");
}

void user_call_from_parser (int code)
{
}
