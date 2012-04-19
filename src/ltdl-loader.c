/*                               -*- Mode: C -*-
 * ltdl-loader.c -- takes care of loading external code using ltdl.
 *
 * Copyright (c) 2012 Francois Felix Ingrand, LAAS/CNRS.
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

/* This code takes care of loading evaluable functions, predicates or actions. It is heavily inspired by
 * examples from the autobook. */

#include <ltdl.h>
#include <stdio.h>
#include <string.h>

#include "opaque-pub.h"
#include "constant-pub.h"
#include "ltdl-loader_f.h"

/* Save and return a copy of the dlerror() error  message,
   since the next API call may overwrite the original. */
static char *dlerrordup (char *errormsg);

PBoolean
load_function_from_dl(char *filename, char *entry_point_name)
{
     char *errormsg = NULL;
     lt_dlhandle module = NULL;
     entrypoint *user_function = NULL;
     int errors = 0;
     static int inited =0;
     static lt_dladvise advise;

     /* Initialise libltdl. */

     if (! inited) {
	  // LTDL_SET_PRELOADED_SYMBOLS();
	  errors = lt_dlinit ();
	  inited = 1;
	  
	  if (lt_dladvise_init (&advise) || lt_dladvise_ext (&advise) || lt_dladvise_global (&advise)) {
	       errormsg = dlerrordup (errormsg);
	       fprintf(stderr, "load_function_from_dl: %s: problem with lt_dladvise.\n",errormsg);
	       return FALSE;
	  }
     }
      
     /* Load the module. */
     if (!errors)
	  module = lt_dlopenadvise (filename, advise);

     /* Find the entry point. */
     if (!module) {
	  errormsg = dlerrordup (errormsg);
	  fprintf(stderr, "load_function_from_dl: %s Could not find module \"%s\".\n", errormsg, filename);
	  return FALSE;
     } else {
	  user_function = (entrypoint *) lt_dlsym (module, entry_point_name);

	  if (!user_function) {	/* I am not expecting NULL symbol... even if it is technically possible. */
	       errormsg = dlerrordup (errormsg);
	       fprintf(stderr, "load_function_from_dl: %s Could not find symbol %s in module %s.\n", errormsg, entry_point_name, filename);
	       return FALSE;
	  }
	  errormsg = dlerrordup (errormsg);
	  if (errormsg != NULL) {
	       errors = lt_dlclose (module);
	       module = NULL;
	  }
     }

     /* Call the entry point function. */
     (*user_function)();
     return TRUE;
}

/* Be careful to save a copy of the error message,
   since the  next API call may overwrite the original. */
static char *
dlerrordup (char *errormsg)
{
     char *error = (char *) lt_dlerror ();
     if (error && !errormsg)
	  errormsg = strdup (error);
     return errormsg;
}
