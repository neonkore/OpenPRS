static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-error.c -- 
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

#include "oprs-type.h"
#include "oprs-rerror.h"
#include "oprs-sprint.h"
#include "parser-funct.h"

#include "oprs-sprint_f.h"
#include "oprs-rerror_f.h"

char *current_eval_funct = NULL;
char *current_eval_pred = NULL;
char *current_ac = NULL;

Error_Type current_error_type = ET_NONE;

Error_Ctxt current_error_ctxt = EC_NONE;

jmp_buf global_error_jmp;
jmp_buf check_stdin_jmp;
jmp_buf find_soak_fact_jmp;
jmp_buf find_soak_goal_jmp;
jmp_buf execute_intention_jmp;
jmp_buf goal_posting_jmp;
jmp_buf consult_database_jmp;
jmp_buf execute_action_jmp;

void report_fatal_external_error(char *error_message)
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp, strlen(progname) + strlen(oprs_prompt?oprs_prompt:"") + 2,
			       sprintf(f,"%s:%s%s",progname, (oprs_prompt?oprs_prompt:""), (oprs_prompt?":":"")));
     
     switch (current_error_type) {
     case EVAL_FUNCTION:
	  SPRINT(sp,64 + strlen(current_eval_funct),
				    sprintf(f,LG_STR("Fatal Error in evaluable function: %s",
						     "Erreur fatale dans la fonction évaluable: %s"), current_eval_funct));
	  break;
     case EVAL_PREDICATE:
	  SPRINT(sp,64 + strlen(current_eval_pred),
				    sprintf(f,LG_STR("Fatal Error in evaluable predicate: %s",
						     "Erreur fatale dans le prédicat évaluable: %s"), current_eval_pred));
	  break;
     case ACTION:
	  SPRINT(sp,64 + strlen(current_ac),
				    sprintf(f,LG_STR("Fatal Error in action: %s",
						     "Erreur fatale dans l'action: %s"), current_ac));
	  break;
     case ET_NONE:
     default:
	  SPRINT(sp,32,sprintf(f,LG_STR("Fatal Error of unknown type: ",
					"Erreur fatale de type inconnu: ")));
     }

     SPRINT(sp, 4 + strlen(error_message), sprintf(f,", %s\n", error_message));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));

     propagate_error(current_error_ctxt);
}

void propagate_error(Error_Ctxt error_ctxt)
{
     switch (error_ctxt) {
     case  EC_GOAL_POSTING:	/* We can dump the current intention if any. */
	  longjmp(goal_posting_jmp,-1);
	  break;
     case EC_CONSULT_DATABASE:
	  longjmp(consult_database_jmp,-1);
	  break;
     case EC_EXECUTE_ACTION:		/* We can dump the current intention for sure... */
	  longjmp(execute_action_jmp,-1);
	  break;
     case EC_EXECUTE_INTENTION:
	  longjmp(execute_intention_jmp,-1);
	  break;
     case EC_CHECK_STDIN:
	  longjmp(check_stdin_jmp,-1);
	  break;
     case EC_FIND_SOAK_GOAL:
	  longjmp(find_soak_goal_jmp,-1);
	  break;
     case EC_FIND_SOAK_FACT:
	  longjmp(find_soak_fact_jmp,-1);
	  break;
     case EC_NONE:
     default: {
	       static Sprinter *sp = NULL;

	       if (! sp)
		    sp = make_sprinter(0);
	       else
		    reset_sprinter(sp);

	       SPRINT(sp,64,sprintf(f,LG_STR("Propagating error in an unknown context.\n",
					     "Propagation d'erreur dans un contexte inconnu.\n")));
	       fprintf(stderr, "%s", SPRINTER_STRING(sp));
	       printf("%s", SPRINTER_STRING(sp));
	       exit(1);
	  }
     }
}

void report_recoverable_external_error(char *error_message)
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp, strlen(progname) + strlen(oprs_prompt?oprs_prompt:"") + 2,
			       sprintf(f,"%s:%s%s",progname, (oprs_prompt?oprs_prompt:""), (oprs_prompt?":":"")));
     
     switch (current_error_type) {
     case EVAL_FUNCTION:
	  SPRINT(sp,64 + strlen(current_eval_funct),
				    sprintf(f,LG_STR("Recoverable Error in evaluable function: %s",
						     "Erreur rattrapable dans la fonction évaluable: %s"), current_eval_funct));
	  break;
     case EVAL_PREDICATE:
	  SPRINT(sp,64 + strlen(current_eval_pred),
				    sprintf(f,LG_STR("Recoverable Error in evaluable predicate: %s",
						     "Erreur rattrapable dans le prédicat évaluable: %s"), current_eval_pred));
	  break;
     case ACTION:
	  SPRINT(sp,64 + strlen(current_ac),
				    sprintf(f,LG_STR("Recoverable Error in action: %s",
						     "Erreur rattrapable dans l'action: %s"), current_ac));
	  break;
     case ET_NONE:
     default:
	  SPRINT(sp,32,sprintf(f,LG_STR("Recoverable Error of unknown type: ",
					"Erreur rattrapable de type inconnu: ")));
     }

     SPRINT(sp, 4 + strlen(error_message), sprintf(f,", %s\n", error_message));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}
