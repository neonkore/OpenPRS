static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * oprs-util.c -- 
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

#include <stdio.h>
#include <stdlib.h>

#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "oprs-type-pub.h"
#include "lang.h"
#include "oprs-util_f.h"

PBoolean get_int_from_env(PString env, int *val)
{
#ifndef VXWORKS
     char *sval;

     sval = getenv(env);
     
     if (sval) {
	  if (!sscanf (sval,"%d",val)) {
	       fprintf(stderr, LG_STR("Unable to parse an integer from the environment variabe %s: %s",
				      "Unable to parse an integer from the environment variabe %s: %s"), env, sval);
	       return FALSE;
	  }
	  return TRUE;
     } else 
#endif
	  return FALSE;
}

#ifdef VXWORKS
#include "taskLib.h"
#include "sysLib.h"

void sleep(int seconde)
{
     taskDelay(seconde * sysClkRateGet());
}

int getpid()
{
     return taskIdSelf();
}

#endif

/* This is a test */
