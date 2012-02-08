static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-array.c -- 
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

#include "slistPack.h"
#include "slistPack_f.h"

#include "macro.h"
#include "constant.h"
#include "opaque.h"
#include "oprs-type.h"

#include "oprs-type_f.h"
#include "oprs-array_f.h"

Term *make_float_array(int size)
{
     Term *t = MAKE_OBJECT(Term);
     Float_array *fa = MAKE_OBJECT(Float_array);

     fa->size = (unsigned short)size;
     fa->array = (double *)MALLOC(sizeof(double) * size);
     BZERO(fa->array,sizeof(double) * size);

     t->type = FLOAT_ARRAY;
     t->u.float_array = fa;

     return t;
}

Term *make_float_array_from_array(int size, double *array)
{
     Term *t = MAKE_OBJECT(Term);
     Float_array *fa = MAKE_OBJECT(Float_array);

	 fa->size = (unsigned short)size;
     fa->array = array;

     t->type = FLOAT_ARRAY;
     t->u.float_array = fa;

     return t;
}

double *get_array_from_float_array(Term *t)
{
     if (t->type != FLOAT_ARRAY) {
	  fprintf(stderr,LG_STR("Error: Expecting a FLOAT_ARRAY in get_array_from_float_array.\n",
				"ERREUR: Attendait un FLOAT_ARRAY dans la fonction get_array_float_array.\n"));
	  return NULL;
     } else 
	  return t->u.float_array->array;
}

int get_float_array_size(Term *t)
{
     if (t->type != FLOAT_ARRAY) {
	  fprintf(stderr,LG_STR("ERROR: Expecting a FLOAT_ARRAY in get_float_array_size.\n",
				"ERREUR: Attendait un FLOAT_ARRAY dans la fonction get_float_array_size.\n"));
	  return 0;
     } else 
	  return t->u.float_array->size;
}

void free_float_array(Float_array *fa)
{
     FREE(fa->array);
     FREE(fa);
     return;
}

Float_array *copy_float_array(Float_array *fa)
{
     Float_array *res = MAKE_OBJECT(Float_array);

     res->size = fa->size;
     res->array = (double *)MALLOC(sizeof(double) * fa->size);
     BCOPY(fa->array,res->array,sizeof(double) * fa->size);
     
     return res;
}

PBoolean equal_float_array(Float_array *fa1, Float_array *fa2)
{
     if (fa1 == fa2) return TRUE;
     else if (fa1->size != fa2->size) return FALSE;
     else {
	  int i;
	  int size = fa1->size;
	  
	  for (i = 0; i < size; i++)
	       if (fa1->array[i] != fa2->array[i]) 
		    return FALSE;
     }
     return TRUE;
}


Term *make_int_array(int size)
{
     Term *t = MAKE_OBJECT(Term);
     Int_array *ia = MAKE_OBJECT(Int_array);

     ia->size = (unsigned short)size;
     ia->array = (int *)MALLOC(sizeof(int) * size);
     BZERO(ia->array,sizeof(int) * size);

     t->type = INT_ARRAY;
     t->u.int_array = ia;

     return t;
}

Term *make_int_array_from_array(int size, int *array)
{
     Term *t = MAKE_OBJECT(Term);
     Int_array *ia = MAKE_OBJECT(Int_array);

	 ia->size = (unsigned short)size;
     ia->array = array;

     t->type = INT_ARRAY;
     t->u.int_array = ia;

     return t;
}

int *get_array_from_int_array(Term *t)
{
     if (t->type != INT_ARRAY) {
	  fprintf(stderr,LG_STR("ERROR: Expecting a INT_ARRAY in get_array_from_int_array.\n",
				"ERREUR: Attendait un INT_ARRAY dans la fonction get_array_from_int_array.\n"));
	  return NULL;
     } else 
	  return t->u.int_array->array;
}

int get_int_array_size(Term *t)
{
     if (t->type != INT_ARRAY) {
	  fprintf(stderr,LG_STR("ERROR: Expecting a INT_ARRAY in get_int_array_size.\n",
				"ERREUR: Attendait un INT_ARRAY dans la fonction get_int_array_size.\n"));
	  return 0;
     } else 
	  return t->u.int_array->size;
}

void free_int_array(Int_array *ia)
{
     FREE(ia->array);
     FREE(ia);
     return;
}

Int_array *copy_int_array(Int_array *ia)
{
     Int_array *res = MAKE_OBJECT(Int_array);

     res->size = ia->size;
     res->array = (int *)MALLOC(sizeof(int) * ia->size);
     BCOPY(ia->array,res->array,sizeof(int) * ia->size);
     
     return res;
}

PBoolean equal_int_array(Int_array *ia1, Int_array *ia2)
{
     if (ia1 == ia2) return TRUE;
     else if (ia1->size != ia2->size) return FALSE;
     else {
	  int i;
	  int size = ia1->size;
	  
	  for (i = 0; i < size; i++)
	       if (ia1->array[i] != ia2->array[i]) 
		    return FALSE;
     }
     return TRUE;
}


Term *build_term_array_from_c_list(TermList tl)
{
     Term *res;
     int size;


     if ((size = sl_slist_length(tl)) == 0) {
	  fprintf(stderr,LG_STR("WARNING: Reading empty array.\n\t\
Array type will default to INTEGER.\n",
				"ATTENTION: Lecture d'un tableau vide.\n\t\
Le type du tableau est INTEGER.\n"));
	  return make_int_array(0);
     } else {
	  int i = 0;
	  Term *t;
	  Term *t1 = (Term *)sl_get_slist_pos(tl, 1);
	  
	  if (t1->type == INTEGER) {
	       int *array;

	       res = make_int_array(size);
	       array = res->u.int_array->array;

	       sl_loop_through_slist(tl, t, Term *) {
		    switch (t->type) {
		    case INTEGER:
			 array[i] = t->u.intval;
			 break;
			 case TT_FLOAT:
			 fprintf(stderr,LG_STR("WARNING: Casting a TT_FLOAT in INTEGER in build_term_array_from_c_list.\n",
					       "ATTENTION: Convertion d'un TT_FLOAT en INTEGER dans la fonction build_term_array_from_c_list.\n"));
			 array[i] = (int)*t->u.doubleptr;
			 break;
		    default:
			 fprintf(stderr,LG_STR("ERROR: Unsupported Term type in build_term_array_from_c_list.\n",
					       "ERREUR: Type de Term non admis dans la fonction build_term_array_from_c_list.\n"));
			 break;
		    }
		    i++;
		    free_term(t);
	       }
		   FREE_SLIST(tl);
	       
	       return res;
	  } else if (t1->type == TT_FLOAT) {
	       double *array;

	       res = make_float_array(size);
	       array = res->u.float_array->array;


	       sl_loop_through_slist(tl, t, Term *) {
		    switch (t->type) {
		    case TT_FLOAT:
			 array[i] = *t->u.doubleptr;
			 break;
		    case INTEGER:
			 fprintf(stderr,LG_STR("WARNING: Casting an INTEGER in TT_FLOAT in build_term_array_from_c_list.\n",
					       "ATTENTION: Convertion d'un INTEGER en TT_FLOAT dans la fonction build_term_array_from_c_list.\n"));
			 array[i] = (double)t->u.intval;
			 break;
		    default:
			 fprintf(stderr,LG_STR("ERROR: unknown Term Type in build_term_array_from_c_list.\n",
					       "ERREUR: Type de Term inconnu dans la fonction build_term_array_from_c_list.\n"));
			 break;
		    }
		    i++;
		    free_term(t);
	       }
		   FREE_SLIST(tl);
	       
	       return res;
	  } else {
	       fprintf(stderr,LG_STR("ERROR: Unable to determine array type from the first element,\n\t\
Defaulting to an empty integer array.\n",
				     "ERREUR: Incapable de déterminer le type du tableau à partir de son premier élément,\n\t\
Création d'un tableau vide d'INTEGER.\n"));
	       return make_int_array(0);
	  }	  
     }
}
