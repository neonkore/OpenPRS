
/* This code takes care of loading evaluable functions, predicates or actions */

#include <ltdl.h>
#include <stdio.h>
#include <string.h>

#include "opaque-pub.h"
#include "ltdl-loader_f.h"

/* Save and return a copy of the dlerror() error  message,
   since the next API call may overwrite the original. */
static char *dlerrordup (char *errormsg);

void
load_function_from_dl(char *filename, char *entry_point_name)
{
     char *errormsg = NULL;
     lt_dlhandle module = NULL;
     entrypoint *user_function = NULL;
     int errors = 0;
     static int inited =0;

     /* Initialise libltdl. */

     if (! inited) {
	  // LTDL_SET_PRELOADED_SYMBOLS();
	  errors = lt_dlinit ();
	  inited = 1;
     }
     if (!errors) {
	  errors = lt_dlsetsearchpath("."); /* not needed I think... */
     }
 
     /* Load the module. */
     if (!errors)
	  module = lt_dlopenext (filename);

     /* Find the entry point. */
     if (!module) {
	  errormsg = dlerrordup (errormsg);
	  fprintf(stderr, "ERROR: %s Could not find module %s.\n", errormsg, filename);
	  return;
     } else {
	  user_function = (entrypoint *) lt_dlsym (module, entry_point_name);

	  if (!user_function) {	/* I am not expecting NULL symbol... even if it is technically possible. */
	       errormsg = dlerrordup (errormsg);
	       fprintf(stderr, "ERROR: %s Could not find symbol %s in module %s.\n", errormsg, entry_point_name, filename);
	       return;
	  }
	  errormsg = dlerrordup (errormsg);
	  if (errormsg != NULL) {
	       errors = lt_dlclose (module);
	       module = NULL;
	  }
     }

     /* Call the entry point function. */
     (*user_function)();
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
