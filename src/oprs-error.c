/*                               -*- Mode: C -*- 
 * oprs-error.c -- 
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



#include "oprs-type.h"
#include "oprs-error.h"
#include "oprs-error_f.h"

void oprs_perror(const char *s, Oprs_Error error)
{
     if (s && *s)
	  fprintf(stderr,"%s (%s): %s: %s\n", oprs_prompt, LG_STR("ERROR", "ERREUR"), s, oprs_strerror(error));
     else
	  fprintf(stderr,"%s (%s): %s\n", oprs_prompt, LG_STR("ERROR", "ERREUR"), oprs_strerror(error));
}

char *oprs_strerror(Oprs_Error error)
{
     switch (error) {
     case PE_UNEXPECTED_DUMP_TYPE:
	  return LG_STR("Unexpected dump type.",
			"Type de dump inattendu.");
     case PE_UNEXPECTED_TEMPORAL_OPER:
	  return LG_STR("Unexpected temporal operator.",
			"Opérateur temporal inattendu.");

     case PE_UNEXPECTED_LOGICAL_OPER:
	  return LG_STR("Unexpected logical operator.",
			"Opérateur logique inattendu.");

     case PE_UNEXPECTED_TERM_TYPE:
	  return LG_STR("Unexpected term type.",
			"Type de term inattendu.");

     case PE_UNEXPECTED_INSTRUCTION_TYPE:
	  return LG_STR("Unexpected instruction type.",
			"Type d'instruction inattendu.");

     case PE_UNEXPECTED_CONDITION_TYPE:
	  return LG_STR("Unexpected condition type.",
			"Type de condition inattendu.");

     case PE_EXPECTED_INTEGER_TERM_TYPE:
	  return LG_STR("Expecting an INTEGER.",
			"Attendait un INTEGER.");
     case PE_EXPECTED_LONG_LONG_TERM_TYPE:
	  return LG_STR("Expecting a LONG_LONG.",
			"Attendait un LONG_LONG.");
     case PE_EXPECTED_INT_ARRAY_TERM_TYPE:
	  return LG_STR("Expecting an INT_ARRAY.",
			"Attendait un INT_ARRAY.");
     case PE_EXPECTED_FLOAT_TERM_TYPE:
	  return LG_STR("Expecting an TT_FLOAT.",
			"Attendait un TT_FLOAT.");
     case PE_EXPECTED_FLOAT_ARRAY_TERM_TYPE:
	  return LG_STR("Expecting an FLOAT_ARRAY.",
			"Attendait un FLOAT_ARRAY.");
     case PE_EXPECTED_ATOM_TERM_TYPE:
	  return LG_STR("Expecting an TT_ATOM.",
			"Attendait un TT_ATOM.");
     case PE_EXPECTED_LISP_LIST_TERM_TYPE:
	  return LG_STR("Expecting an LISP_LIST.",
			"Attendait un LISP_LIST.");
     case PE_EXPECTED_VARIABLE_TERM_TYPE:
	  return LG_STR("Expecting an VARIABLE.",
			"Attendait un VARIABLE.");
     case PE_EXPECTED_LENV_TERM_TYPE:
	  return LG_STR("Expecting an VARIABLE_LIST.",
			"Attendait un VARIABLE_LIST.");
     case PE_EXPECTED_GTEXPRESSION_TERM_TYPE:
	  return LG_STR("Expecting an GTEXPRESSION.",
			"Attendait un GTEXPRESSION.");
     case PE_EXPECTED_GEXPRESSION_TERM_TYPE:
	  return LG_STR("Expecting an GEXPRESSION.",
			"Attendait un GEXPRESSION.");
     case PE_EXPECTED_INTENTION_TERM_TYPE:
	  return LG_STR("Expecting an INTENTION.",
			"Attendait un INTENTION.");
     case PE_EXPECTED_OP_INSTANCE_TERM_TYPE:
	  return LG_STR("Expecting an OP_INSTANCE.",
			"Attendait un OP_INSTANCE.");
     case PE_EXPECTED_FACT_TERM_TYPE:
	  return LG_STR("Expecting an FACT.",
			"Attendait un FACT.");
     case PE_EXPECTED_GOAL_TERM_TYPE:
	  return LG_STR("Expecting an GOAL.",
			"Attendait un GOAL.");
     case PE_EXPECTED_U_POINTER_TERM_TYPE:
	  return LG_STR("Expecting an U_POINTER.",
			"Attendait un U_POINTER.");
     case PE_EXPECTED_C_LIST_TERM_TYPE:
	  return LG_STR("Expecting an C_LIST.",
			"Attendait un C_LIST.");
     case PE_EXPECTED_EXPRESSION_TERM_TYPE:
	  return LG_STR("Expecting a EXPRESSION.",
			"Attendait un EXPRESSION.");
     case PE_EXPECTED_STRING_TERM_TYPE:
	  return LG_STR("Expecting an STRING.",
			"Attendait un STRING.");
     case PE_OUT_OF_BOUND_ARRAY_ACCES:
	  return LG_STR("Out of bound array index.",
			"Indice de tableau hors bornes.");
     case PE_DEAD_INTENTION:
	  return LG_STR("Dead intention.",
			"Intention morte.");
     case PE_CAR_FROM_EMPTY_LISP_LIST:
	  return LG_STR("Trying to get an element from an empty LISP_LIST.",
			"Tentative d'extraction d'élément d'une liste LISP_LIST vide.");

     case PE_UNREGISTERED_ERROR:
	  return "";

     default:
	  return LG_STR("Unknown OPRS Error.",
			"Erreur inconnue.");
     }
}
