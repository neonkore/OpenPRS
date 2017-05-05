/*                               -*- Mode: C -*- 
 * pu-parse-tl.c --- 
 * 
 * Filename: pu-parse-tl.c
 * Description: 
 * Author: Felix Ingrand <felix@laas.fr>
 *
 * Copyright (C) 1993-2012 LAAS/CNRS.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
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
 */
/*                               -*- Mode: C -*- 
 * pu-parse-tl.c -- 
 * 
 *
 *                         -- C N R S -- 
 *         Laboratoire d'Automatique et d'Analyse des Systemes 
 *                 7 Avenue du colonel Roche   
 *                     31 077 Toulouse Cedex
 * 
 */

#include <config.h>

/* include system */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <inttypes.h>

/* OPRS list */
#include "slistPack.h"
#include "slistPack_f.h"

#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "macro-pub.h"

#include "oprs-type_f-pub.h"
#include "lisp-list_f-pub.h"

#include "pu-parse-tl_f.h"

PBoolean PU_bind_short(short *shortPtr, Term *term)
{
     if (term->type == INTEGER) {
       if ((term->u.intval >= SHRT_MIN) && (term->u.intval <= SHRT_MAX)) {
	 *shortPtr = (short)term->u.intval;
	  return TRUE;
       } else {
	 fprintf(stderr,"PU_bind_short: int value too large to fit in a short.\n");
	 return FALSE;
       }
     } else {
       fprintf(stderr,"PU_bind_short: Bad parameter (expected an INTEGER).\n");
       return FALSE;
     }
}

PBoolean PU_bind_integer(int *intPtr, Term *term)
{
     if (term->type == INTEGER) {
	  *intPtr = term->u.intval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_integer: Bad parameter (expected an INTEGER).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_long_long(long long int *intPtr, Term *term)
{
     if (term->type == LONG_LONG) {
	  *intPtr = term->u.llintval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_long_long: Bad parameter (expected a LONG_LONG).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_string(char **strPtr, Term *paramTerm)
{
     if (paramTerm->type == STRING) {
	  *strPtr = paramTerm->u.string;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_string: Bad parameter (expected a STRING).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_atom(Symbol *atomPtr, Term *paramTerm)
{
     if (paramTerm->type == ATOM) {
	  *atomPtr = paramTerm->u.id;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_atom: Bad parameter (expected an ATOM).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_u_pointer(void **ptr, Term *paramTerm)
{
     if (paramTerm->type == U_POINTER) {
	  *ptr = paramTerm->u.u_pointer;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_u_pointer: Bad parameter (expected a U_POINTER).\n");
	  return FALSE;
     }		
}

PBoolean PU_bind_u_memory(U_Memory **u_mem, Term *paramTerm)
{
     if (paramTerm->type == U_MEMORY) {
	  *u_mem = paramTerm->u.u_memory;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_u_memory: Bad parameter (expected a U_POINTER).\n");
	  return FALSE;
     }		
}

PBoolean PU_bind_expr(Expression **tcPtr, Term *paramTerm)
{
     if (paramTerm->type == EXPRESSION) {
	  *tcPtr = paramTerm->u.expr;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_expr: Bad parameter (expected a EXPRESSION).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_l_list(L_List *lispList, Term *paramTerm)
{
     if (paramTerm->type == LISP_LIST) {
	  *lispList = paramTerm->u.l_list;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_expr: Bad parameter (expected a LISP_LIST).\n");
	  return FALSE;
     }
}

PBoolean PU_bind_double(double *doublePtr, Term *paramTerm)
{
     if (paramTerm->type == FLOAT) { /* What we call FLOAT in oprs are double */
	  *doublePtr = *paramTerm->u.doubleptr;
	  return TRUE;
     } else if (paramTerm->type == INTEGER) {
	  fprintf(stderr,"PU_bind_double: Warning: Casting parameter from INTEGER to FLOAT.\n");
	  *doublePtr = paramTerm->u.intval;
	  return TRUE;
     } else if (paramTerm->type == LONG_LONG) {
	  fprintf(stderr,"PU_bind_double: Warning: Casting parameter from LONG_LONG to FLOAT.\n");
	  *doublePtr = paramTerm->u.llintval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_double: Bad parameter (expected a FLOAT or an INTEGER).\n");
	  return FALSE;
     }
}


PBoolean PU_bind_float(float *floatPtr, Term *paramTerm)
{
     if (paramTerm->type == FLOAT) {
       double dd = *(paramTerm->u.doubleptr);
       if ((dd < FLT_MIN) || (dd > FLT_MAX)) {
	 fprintf(stderr,"PU_bind_float: Error: this double %f does not fit in a float;.\n", dd);
	 return FALSE;
       }
       *floatPtr = dd;
       return TRUE;
     } else if (paramTerm->type == INTEGER) {
	  fprintf(stderr,"PU_bind_float: Warning: Casting parameter from INTEGER to FLOAT.\n");
	  *floatPtr = paramTerm->u.intval;
	  return TRUE;
     } else if (paramTerm->type == LONG_LONG) {
	  fprintf(stderr,"PU_bind_float: Warning: Casting parameter from LONG_LONG to FLOAT.\n");
	  *floatPtr = paramTerm->u.llintval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_float: Bad parameter (expected a FLOAT or an INTEGER).\n");
	  return FALSE;
     }
}

/*------------------------------------------------------------
  * Fonction which checks the parameters and bind them to your pointers.
  * Ex: PUGetOprsParameters(termList, 6, // we expect 6 arguments
  *                                FLOAT, doublePtr, // they OpenPRS type, your pointer,
  *                                INTEGER, intPtr,
  *                                STRING, charPtrPtr,
  *                                U_POINTER, voidPtrPtr,
  *                                ATOM, charPtrPtr,
  *                                LISP_LIST, L_ListPtr)
  * Return TRUE if everything went fine!
  ------------------------------------------------------------*/
PBoolean PUGetOprsParameters(TermList paramList, int NbParametres, ...)
{
     va_list listArg;
     int paramCour;
     int result = TRUE;
  
     if (sl_slist_length(paramList) < NbParametres)	{ /* Less parameters than requested */
	  fprintf(stderr,"PUGetOprsParameters: ERROR: requesting %d parameters in a parameter list containing only %d element.\n",NbParametres, sl_slist_length(paramList));
	  return FALSE;
     }

     va_start(listArg, NbParametres);
  
     for(paramCour = 1; paramCour <= NbParametres; paramCour++) {
	  Term_Type type;
	  type = va_arg(listArg, Term_Type);
    
	  switch(type) {
	  case FLOAT:
	       result &= PU_bind_double(va_arg(listArg, double *),
				       (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case INTEGER:
	       result &= PU_bind_integer(va_arg(listArg, int *),
					 (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case LONG_LONG:
	       result &= PU_bind_long_long(va_arg(listArg, long long int *),
					   (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case STRING:
	       result &= PU_bind_string(va_arg(listArg, char **),
					(Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case EXPRESSION:
	       result &= PU_bind_expr(va_arg(listArg, Expression **),
					   (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case U_POINTER:
	       result &= PU_bind_u_pointer(va_arg(listArg, void **),
					   (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case U_MEMORY:
	       result &= PU_bind_u_memory(va_arg(listArg, U_Memory **),
					  (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case ATOM:
	       result &= PU_bind_atom(va_arg(listArg, Symbol *),
				      (Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  case LISP_LIST:
	       result &= PU_bind_l_list(va_arg(listArg, L_List *),
					(Term *)sl_get_slist_pos(paramList, paramCour));
	       break;
	  default:
	       fprintf(stderr,"PUGetOprsParameters: Unknown type in PUGetOprsParameters\n");
	       return(FALSE);
	       break;
	  }
     }
  
     va_end(listArg);
  
     return(result);
}

/*------------------------------------------------------------
  Function which grabs the element of an OpenPRS LISP_LIST, check the length and the type
  of the argument and bind them to your pointers.
  * Ex: PUGetLispListElem(lispList, 6, // a LISP_LIST with 6 elements
  *                     FLOAT, doublePtr, // first one a FLOAT, your pointers, 
  *                     INTEGER, intPtr, // second on an integer, etc...
  *                     STRING, charPtrPtr,
  *                     U_POINTER, voidPtrPtr,
  *                     ATOM, charPtrPtr,
  *                     LISP_LIST, L_ListPtr)
  * Return  TRUE if everything went fine.
  ------------------------------------------------------------*/
PBoolean PUGetLispListElem(L_List lispList, int NbTerm, ...)
{
     va_list listArg;
     int paramCour;
     int result = TRUE;
  
     /* on verifie la longueur...*/
  
     if (l_length(lispList) < NbTerm)	{ /* Less parameters than requested */
	  fprintf(stderr,"PUGetLispListElem: Error: requesting %d parameters in a LISP_LIST containing only %d element.\n",NbTerm, l_length(lispList));
	  return(FALSE);
     }

     va_start(listArg, NbTerm);
  
     for(paramCour = 1; paramCour <= NbTerm; paramCour++) {
	  Term_Type type;
    
	  type = va_arg(listArg, Term_Type);
    
	  switch(type) {
	  case FLOAT:
	       result &= PU_bind_double(va_arg(listArg, double *),l_car(lispList));
	       break;
	  case INTEGER:
	       result &= PU_bind_integer(va_arg(listArg, int *),l_car(lispList));
	       break;
	  case LONG_LONG:
	       result &= PU_bind_long_long(va_arg(listArg, long long int *),l_car(lispList));
	       break;
	  case STRING:
	       result &= PU_bind_string(va_arg(listArg, char **),l_car(lispList));
	       break;
	  case EXPRESSION:
	       result &= PU_bind_expr(va_arg(listArg, Expression **),l_car(lispList));
	       break;
	  case U_POINTER:
	       result &= PU_bind_u_pointer(va_arg(listArg, void **),l_car(lispList));
	       break;
	  case U_MEMORY:
	       result &= PU_bind_u_memory(va_arg(listArg, U_Memory **),l_car(lispList));
	       break;
	  case ATOM:
	       result &= PU_bind_atom(va_arg(listArg, Symbol *),l_car(lispList));
	       break;
	  case LISP_LIST:
	       result &= PU_bind_l_list(va_arg(listArg, L_List *),l_car(lispList));
	       break;
	  default:
	       fprintf(stderr,"Unknown type in PUGetLispListElem\n");
	       return(FALSE);
	       break;
	  }
	  lispList = l_cdr(lispList);

     }
  
     va_end(listArg);
  
     return(result);
}

int PUGetLispListCar(L_List *lispListPtr, Term_Type type, void *ptr)
/* Grab the car (first element) of an OpenPRS LISP_LIST. */
{
     Term *paramTerm;

     if (l_length(*lispListPtr) < 1) {
	  return -1;
     }

     paramTerm = l_car(*lispListPtr);
     *lispListPtr = l_cdr(*lispListPtr);


     switch(type) {
     case FLOAT:
	  if (! (PU_bind_double((double *)ptr, paramTerm)))
	       return -1;
	  break;
     case INTEGER:
	  if (! (PU_bind_integer((int *)ptr, paramTerm)))
	       return -1;
	  break;
     case LONG_LONG:
	  if (! (PU_bind_long_long((long long int *)ptr, paramTerm)))
	       return -1;
	  break;
     case STRING:
	  if (! (PU_bind_string((char **)ptr, paramTerm)))
	       return -1;
	  break;
     case EXPRESSION:
	  if (! (PU_bind_expr((Expression **)ptr, paramTerm)))
	       return -1;
	  break;
     case U_POINTER:
	  if (! (PU_bind_u_pointer((void **)ptr, paramTerm)))
	       return -1;
	  break;
     case U_MEMORY:
	  if (! (PU_bind_u_memory((U_Memory **)ptr, paramTerm)))
	       return -1;
	  break;
     case ATOM:
	  if (! (PU_bind_atom((Symbol *)ptr, paramTerm)))
	       return -1;
	  break;
     case LISP_LIST:
	  if (! (PU_bind_l_list((L_List *)ptr, paramTerm)))
	       return -1;
	  break;
     default:
	  fprintf(stderr,"Unknown type in PUGetLispListCar\n");
	  return(-1);
	  break;
     }
  
     return(l_length(*lispListPtr));
}

/*------------------------------------------------------------
  * Function which grabs the element of an OpenPRS EXPRESSION. Of course, the first one
  * must be an ATOM.
  * Ex: PUGetOprsTermCompArgs(tc, 6, 
  *                                ATOM, charPtrPtr,
  *                                FLOAT, doublePtr,
  *                                INTEGER, intPtr,
  *                                STRING, charPtrPtr,
  *                                U_POINTER, voidPtrPtr,
  *                                LISP_LIST, L_ListPtr)
  * Return TRUE if all OK.
  ------------------------------------------------------------*/
PBoolean PUGetOprsTermCompArgs(Expression *tc, int NbParametres, ...)
{
     va_list listArg;
     int paramCour;
     int result = TRUE;
     TermList  paramList = tc->terms;
  
     if (sl_slist_length(paramList) + 1 < NbParametres)	{ /* Less parameters than requested */
	  fprintf(stderr,"PUGetOprsTermCompArgs: ERROR: requesting %d parameters in a EXPRESSION containing only %d element.\n",NbParametres, sl_slist_length(paramList) + 1);
	  return FALSE;
     }

     va_start(listArg, NbParametres);
  
     for(paramCour = 1; paramCour <= NbParametres; paramCour++) {
	  Term_Type type;
	  type = va_arg(listArg, Term_Type);

	  if (paramCour == 1) 
	       if (type == ATOM) { /* first argument must be an ATOM. */
		    Symbol *atom = va_arg(listArg, Symbol *);
		    *atom =  pred_func_rec_symbol(tc->pfr);
	       } else {
		    fprintf(stderr,"PUGetOprsTermCompArgs: Bad parameter %d (expected an ATOM)\n",  paramCour);
		    result = FALSE;
	       }
	  else 
	       switch(type) {
	       case FLOAT:
		    result &= PU_bind_double(va_arg(listArg, double *),
					    (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case INTEGER:
		    result &= PU_bind_integer(va_arg(listArg, int *),
					      (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case LONG_LONG:
		    result &= PU_bind_long_long(va_arg(listArg, long long int *),
					      (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case STRING:
		    result &= PU_bind_string(va_arg(listArg, char **),
					     (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case EXPRESSION:
		    result &= PU_bind_expr(va_arg(listArg, Expression **),
						(Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case U_POINTER:
		    result &= PU_bind_u_pointer(va_arg(listArg, void **),
						(Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case U_MEMORY:
		    result &= PU_bind_u_memory(va_arg(listArg, U_Memory **),
					       (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case ATOM:
		    result &= PU_bind_atom(va_arg(listArg, Symbol *),
					   (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       case LISP_LIST:
		    result &= PU_bind_l_list(va_arg(listArg, L_List *),
					     (Term *)sl_get_slist_pos(paramList, paramCour - 1));
		    break;
	       default:
		    fprintf(stderr,"PUGetOprsTermCompArgs: Unknown type in switch.\n");
		    return(FALSE);
		    break;
	       }
     }
  
     va_end(listArg);
  
     return(result);
}

PBoolean PUGetOprsTermCompSpecArg(Expression *tc, int rank, Term_Type type, void *ptr)
/* Grab a specific argument from an EXPRESSION, rank=0 is the first element.  */
{
     if (rank == 0) 
	  if (type == ATOM) {
	       *(Symbol *)ptr = pred_func_rec_symbol(tc->pfr);
	  } else {
	       fprintf(stderr,"PUGetOprsTermCompSpecArg: Bad first parameter (expected an ATOM)\n");
	       return FALSE;
	  }
     else {
	  TermList  paramList = tc->terms;
	  Term *paramTerm;
	  
	  if (rank > sl_slist_length(paramList))
	       return FALSE;	/* beyond the length of the list. */

	  paramTerm = (Term *)sl_get_slist_pos(paramList, rank);


	  if (! paramTerm) {
	       fprintf(stderr,"PUGetOprsTermCompSpecArg: Unable to get %d argument in TERM COMP.\n",
		       rank);
	       return FALSE;
	  }
	  switch(type) {
	  case FLOAT:
	       if ( ! PU_bind_double((double *)ptr, paramTerm))
		    return FALSE;
	       break;
	  case INTEGER:
	       if ( ! PU_bind_integer((int *)ptr, paramTerm))
		    return FALSE;
	       break;
	  case LONG_LONG:
	       if ( ! PU_bind_long_long((long long int *)ptr, paramTerm))
		    return FALSE;
	       break;
	  case STRING:
	       if ( ! PU_bind_string((char **)ptr, paramTerm))
		    return FALSE;
	       break;
	  case EXPRESSION:
	       if ( ! PU_bind_expr((Expression **)ptr, paramTerm))
		    return FALSE;
	       break;
	  case U_POINTER:
	       if ( ! PU_bind_u_pointer((void **)ptr, paramTerm))
		    return FALSE;
	       break;
	  case U_MEMORY:
	       if ( ! PU_bind_u_memory((U_Memory **)ptr, paramTerm))
		    return FALSE;
	       break;
	  case ATOM:
	       if ( ! PU_bind_atom((Symbol *)ptr, paramTerm))
		    return FALSE;
	       break;
	  case LISP_LIST:
	       if ( ! PU_bind_l_list((L_List *)ptr, paramTerm))
		    return FALSE;
	       break;
	  default:
	       fprintf(stderr,"Unknown type in PUGetOprsTermCompSpecArg\n");
	       return FALSE;
	       break;
	  }
     } 
     return(TRUE);
}

PBoolean PUGetOprsParametersSpecArg(TermList paramList, int rank, Term_Type type, void *ptr)
/* Grab a specific argument from a TermList (parameter to evaluable functions/predicates
   actions. rank=1 is the first element. */
{
     Term *paramTerm;
	  
     if (rank > sl_slist_length(paramList))
	  return FALSE;

     paramTerm = (Term *)sl_get_slist_pos(paramList, rank);


     if (! paramTerm) {
	  fprintf(stderr,"PUGetOprsTermCompSpecArg: Unable to get %d argument in TERM COMP.\n",
		  rank);
	  return FALSE;
     }
     switch(type) {
     case FLOAT:
	  if ( ! PU_bind_double((double *)ptr, paramTerm))
	       return FALSE;
	  break;
     case INTEGER:
	  if ( ! PU_bind_integer((int *)ptr, paramTerm))
	       return FALSE;
	  break;
     case LONG_LONG:
	  if ( ! PU_bind_long_long((long long int *)ptr, paramTerm))
	       return FALSE;
	  break;
     case STRING:
	  if ( ! PU_bind_string((char **)ptr, paramTerm))
	       return FALSE;
	  break;
     case EXPRESSION:
	  if ( ! PU_bind_expr((Expression **)ptr, paramTerm))
	       return FALSE;
	  break;
     case U_POINTER:
	  if ( ! PU_bind_u_pointer((void **)ptr, paramTerm))
	       return FALSE;
	  break;
     case U_MEMORY:
	  if ( ! PU_bind_u_memory((U_Memory **)ptr, paramTerm))
	       return FALSE;
	  break;
     case ATOM:
	  if ( ! PU_bind_atom((Symbol *)ptr, paramTerm))
	       return FALSE;
	  break;
     case LISP_LIST:
	  if ( ! PU_bind_l_list((L_List *)ptr, paramTerm))
	       return FALSE;
	  break;
     default:
	  fprintf(stderr,"Unknown type in PUGetOprsParameterSpecArg\n");
	  return FALSE;
	  break;
     }
     return(TRUE);
}

/* These functions get a VarArg termList: example, you have a goal C
 * struct with 3 fields, order an int, x and y double.

   paramList = (VarArg (order 20)(goal.x 3.4)(y 4.5)(frame "base_id")), 
   find_them_all ensure that all the arguments are provided,
   This is what we expect as ... args. N-uple of this form
   INTEGER, "goal.order", &(goal.order),
   FLOAT, "goal.x", &(goal.x),
   FLOAT, "goal.y", &(goal.y)

   which is usually produced by the call to this macro defined in pu-parse-tl_f.h
   set_parameter_or_default_value(goal.order, 20, INTEGER),
   set_parameter_or_default_value(goal.x, 0.0, FLOAT),
   set_parameter_or_default_value(goal.y, 0.0, FLOAT)

   the second argument of each n-uple in the macro is the default value which has
   already been bound by the macro call.

   Note that the argument in the VarArg can be minimal (e.g. order and
   y) as there are no other possible solutions. This is usefull to
   access intenal field in deep structures without having to give the whole path.
 */
PBoolean PUGetOprsVarArgParameters(TermList paramList, PBoolean find_them_all, int nb, ...)
{
     va_list listArg;
     int paramCour;
     int result = TRUE;
     Term *t;
     TermList tl;
     static Symbol vararg_sym = NULL;

     if (!vararg_sym) vararg_sym = declare_atom("VarArg");


     if (sl_slist_empty(paramList)) { /* nothing... bad idea, OpenPRS probably already complain. */
       fprintf(stderr,"PUGetOprsVarArgParameters: Warning: should not be called without any parameter, defaulting all the variables.\n");
       return TRUE;
     }

     if (sl_slist_length(paramList) != 1 ) { /* one EXPRESSION */
       fprintf(stderr,"PUGetOprsVarArgParameters: ERROR: expecting one Expression such as (VarArg ...)\n");
       return FALSE;
     }
     
     t = (Term *)sl_get_slist_head(paramList);
     
     if (t->type != EXPRESSION) {
       fprintf(stderr,"PUGetOprsVarArgParameters: ERROR: expecting an EXPRESSION\n");
       return FALSE;
     }

     if (pred_func_rec_symbol(t->u.expr->pfr) != vararg_sym)  { /* check the name too ;-) */
       fprintf(stderr,"PUGetOprsVarArgParameters: ERROR: expecting VarArg, not: %s\n", pred_func_rec_symbol(t->u.expr->pfr));
       return FALSE;
     }

     tl = t->u.expr->terms; /* The list of argument of the vararg */

     sl_loop_through_slist(tl, t, Term *) {
       PBoolean found;
       Symbol argName;
       Term * argTerm;
       
       if (t->type != EXPRESSION) { 
	 fprintf(stderr,"PUGetOprsVarArgParameters: ERROR: expecting an Expression\n");
	 return FALSE;
       }

       argName = pred_func_rec_symbol(t->u.expr->pfr);
       argTerm = (Term *)sl_get_slist_head(t->u.expr->terms);
       
       found = FALSE;
       va_start(listArg, nb);
  
       for(paramCour = 0; paramCour <nb; paramCour++) {
	  Term_Type type;
	  char *fieldName, *ptr;
	  char *last = NULL;
	  

	  type = va_arg(listArg, Term_Type);
    	  fieldName = va_arg(listArg, char *);
 
	  ptr = fieldName;
	  while((ptr = strstr(ptr, argName))) last = ptr++; /* find the last occurence of argName in fieldName. */
	  
	  if (last && (strlen(last) == strlen(argName)) && /* The last AND it terminates the string */
	      ((last == fieldName) ||			   /* argName is the whole field I am not sure it is a great idea, 
							      no particular reasons why we should know the structure name. */
	       (last[-1] == '.'))) {			   /* or the char before is a period . (better) */
	    
	    found = TRUE;

	    switch(type) {
	    case FLOAT:
	      result &= PU_bind_double(va_arg(listArg, double *),
				      argTerm);
	      break;
	    case INTEGER:
	      result &= PU_bind_integer(va_arg(listArg, int *),
					argTerm);
	      break;
	    case LONG_LONG:
	      result &= PU_bind_long_long(va_arg(listArg, long long int *),
					  argTerm);
	      break;
	    case STRING:
	      result &= PU_bind_string(va_arg(listArg, char **),
				       argTerm);
	      break;
	    case EXPRESSION:
	      result &= PU_bind_expr(va_arg(listArg, Expression **),
				     argTerm);
	      break;
	    case U_POINTER:
	      result &= PU_bind_u_pointer(va_arg(listArg, void **),
					  argTerm);
	      break;
	    case U_MEMORY:
	      result &= PU_bind_u_memory(va_arg(listArg, U_Memory **),
					 argTerm);
	      break;
	    case ATOM:
	      result &= PU_bind_atom(va_arg(listArg, Symbol *),
				     argTerm);
	      break;
	    case LISP_LIST:
	      result &= PU_bind_l_list(va_arg(listArg, L_List *),
				       argTerm);
	      break;
	    default:
	      fprintf(stderr,"PUGetOprsVarArgParameters: Unknown type in PUGetOprsVarArgParameters\n");
	      return(FALSE);
	      break;
	    } 
	    
	    if (! result) return (FALSE);
	    else break;	/* we have assigned the structure field... move to the next arg.
			   Note that this is also why we assign the first field in the list.*/
	  } else {		/* this is not the right argument, still we need to skip the pointer  */
	    va_arg(listArg, void *); /* this should do... we are skipping a pointer... no? */
	  }
       }
       va_end(listArg);		/* Will end this turn and then we can go back to the beginning of the list. */

       if (! found)
	 if (find_them_all) {
	   fprintf(stderr,"PUGetOprsVarArgParameters: Error: could not find \"%s\" in the argument list.\n", argName);
	   return FALSE;
	 }
     }
     
     return(result);
}

typedef PBoolean (* encode_genom_function)(Term *, void *);


/* Following the logic of the previous function and pushing it further
 * to recursively call encoding function to fill nested structure.
 * This function and associated macros is heavily used in the
 * GenoM3/OpenPRS template. Check the resulting code to see how it is
 * used. */
PBoolean PUGetOprsVarArgG3Parameters(Expression *expr, PBoolean find_them_all, int nb, ...)
{
     /* This function get something like this as argument:
	(tc, TRUE,  3, 
	"speedRef",TRUE,&(in->speedRef), encode_genom_demo_speed,
	"bar.pos", TRUE, &(in->bar.pos), encode_genom_int,
	"bar.err", FALSE, &(in->bar.err), encode_genom_double,
	); 
	The n-uple above can be obtained using the set_val_addr_func macro. Such as
	(tc, TRUE,  3, 
	set_val_addr_func(in, speedRef,TRUE, encode_genom_demo_speed),
	set_val_addr_func(in, bar.pos ,TRUE, encode_genom_demo_int),
	set_val_addr_func(in, bar.err ,FALSE, encode_genom_demo_double),
	); 
	You get the idea... 
*/
     va_list listArg;
     int paramCour;
     int result = TRUE;
     Term *t;
     TermList tl;
     static Symbol vararg_sym = NULL;
     Slist *filled = sl_make_slist();

     tl = expr->terms; /* The list of argument of the vararg */

     sl_loop_through_slist(tl, t, Term *) {
	  PBoolean found;
	  Symbol argName;
	  Term * argTerm;
       
	  if (t->type != EXPRESSION) { 
	       fprintf(stderr,"PUGetOprsVarArgG3Parameters: ERROR: expecting an Expression\n");
	       return FALSE;
	  }

	  argName = pred_func_rec_symbol(t->u.expr->pfr);
	  argTerm = (Term *)sl_get_slist_head(t->u.expr->terms);
       
	  found = FALSE;
	  va_start(listArg, nb);
  
	  for(paramCour = 0; paramCour <nb; paramCour++) {
	       Term_Type type;
	       char *fieldName, *ptr;
	       char *last = NULL;
	       int mandatory;
	       void *addr;
	       encode_genom_function funct;
	       PBoolean funct_call_ok;

	       /* Get the four fields */
	       fieldName = va_arg(listArg, char *);
	       mandatory = va_arg(listArg, int);
	       addr = va_arg(listArg, void *);
	       funct = va_arg(listArg, encode_genom_function);

	       ptr = fieldName;
	       while((ptr = strstr(ptr, argName))) last = ptr++; /* find the last occurence of argName in fieldName. */
	  
	       if (last && (strlen(last) == strlen(argName)) && /* The last AND it terminates the string */
		   ((last == fieldName) || /* argName is the whole field I am not sure it is a great idea, 
					      no particular reasons why we should know the structure name. */
		    (last[-1] == '.'))) { /* or the char before is a period . (better) */
	    
		    found = TRUE;
		    add_to_tail(filled, (void *)(intptr_t)(paramCour+1)); /* we remember the ith param was filled. */
	    
		    funct_call_ok = (funct)(argTerm,addr);

		    if (! funct_call_ok) return FALSE; /* the parsing did not go well. Just stop here. */

		    break; 		/* we found it, move to the next arg */
	       }
	  }
	  va_end(listArg); /* Will end this turn and then we can go back to the beginning of the list. */

	  if (!found) 
	       if (find_them_all) {
		    fprintf(stderr,"PUGetOprsVarArgG3Parameters: Error: could not find \"%s\" in the argument list.\n", argName);
		    return (FALSE);
	       }
     }

     /* check the filled list. */
     va_start(listArg, nb);
     
     for(paramCour = 0; paramCour <nb; paramCour++) {
	  char *fieldName;;
	  int mandatory;
	  void *addr;
	  encode_genom_function funct;
	  
	  /* Get the four fields */
	  fieldName = va_arg(listArg, char *);
	  mandatory = va_arg(listArg, int);
	  addr = va_arg(listArg, void *);
	  funct = va_arg(listArg, encode_genom_function);
	  if (mandatory && ! sl_in_slist(filled, (void *)(intptr_t)(paramCour+1))) {
	       fprintf(stderr,"PUGetOprsVarArgG3Parameters: Error: Param %s is mandatory but was not provided.\n", 
		       fieldName);
	       return (FALSE);
	  }
     }

     va_end(listArg);		/* Will end this turn and then we can go back to the beginning of the list. */
     sl_free_slist(filled);

     return(TRUE);
}
