/*                               -*- Mode: C -*- 
/* pu-parse-tl.c --- 
 * 
 * Filename: pu-parse-tl.c
 * Description: 
 * Author: Felix Ingrand <felix@laas.fr>
 *
 * Copyright (C) 1993-2011 LAAS/CNRS.
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
 * $Id$
 * 
 */

#ifndef __CEXTRACT__

#include <config.h>

#ifdef VXWORKS
#include "vxWorks.h"
#endif

/* include system */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* pour OPRS */
#include "slistPack.h"
#include "slistPack_f.h"

#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "macro-pub.h"

#include "oprs-type_f-pub.h"
#include "lisp-list_f-pub.h"

#include "pu-parse-tl_f.h"

#endif

static PBoolean PU_bind_integer(int *intPtr, Term *term)
{
     if (term->type == INTEGER) {
	  *intPtr = term->u.intval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_integer: Bad parameter (expected an INTEGER).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_long_long(long long int *intPtr, Term *term)
{
     if (term->type == LONG_LONG) {
	  *intPtr = term->u.llintval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_long_long: Bad parameter (expected a LONG_LONG).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_string(char **strPtr, Term *paramTerm)
{
     if (paramTerm->type == STRING) {
	  *strPtr = paramTerm->u.string;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_string: Bad parameter (expected a STRING).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_atom(Symbol *atomPtr, Term *paramTerm)
{
     if (paramTerm->type == ATOM) {
	  *atomPtr = paramTerm->u.id;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_atom: Bad parameter (expected an ATOM).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_u_pointer(void **ptr, Term *paramTerm)
{
     if (paramTerm->type == U_POINTER) {
	  *ptr = paramTerm->u.u_pointer;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_u_pointer: Bad parameter (expected a U_POINTER).\n");
	  return FALSE;
     }		
}

static PBoolean PU_bind_u_memory(U_Memory **u_mem, Term *paramTerm)
{
     if (paramTerm->type == U_MEMORY) {
	  *u_mem = paramTerm->u.u_memory;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_u_memory: Bad parameter (expected a U_POINTER).\n");
	  return FALSE;
     }		
}

static PBoolean PU_bind_expr(Expression **tcPtr, Term *paramTerm)
{
     if (paramTerm->type == EXPRESSION) {
	  *tcPtr = paramTerm->u.expr;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_expr: Bad parameter (expected a EXPRESSION).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_l_list(L_List *lispList, Term *paramTerm)
{
     if (paramTerm->type == LISP_LIST) {
	  *lispList = paramTerm->u.l_list;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_expr: Bad parameter (expected a LISP_LIST).\n");
	  return FALSE;
     }
}

static PBoolean PU_bind_float(double *doublePtr, Term *paramTerm)
{
     if (paramTerm->type == FLOAT) {
	  *doublePtr = *paramTerm->u.doubleptr;
	  return TRUE;
     } else if (paramTerm->type == INTEGER) {
	  fprintf(stderr,"PU_bind_float: Warning: Casting parameter from INTEGER to FLOAT.\n");
	  *doublePtr = paramTerm->u.intval;
	  return TRUE;
     } else if (paramTerm->type == LONG_LONG) {
	  fprintf(stderr,"PU_bind_float: Warning: Casting parameter from LONG_LONG to FLOAT.\n");
	  *doublePtr = paramTerm->u.llintval;
	  return TRUE;
     } else {
	  fprintf(stderr,"PU_bind_float: Bad parameter (expected a FLOAT or an INTEGER).\n");
	  return FALSE;
     }
}

/*------------------------------------------------------------
  * Fonction qui verifie le type des parametre et qui les affectent
  * Ex: PUGetOprsParameters(termList, 6, 
  *                                FLOAT, doublePtr,
  *                                INTEGER, intPtr,
  *                                STRING, charPtrPtr,
  *                                U_POINTER, voidPtrPtr,
  *                                ATOM, charPtrPtr,
  *                                LISP_LIST, L_ListPtr)
  * Retourne TRUE si tout c'est bien passe !
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
	       result &= PU_bind_float(va_arg(listArg, double *),
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
  Fonction qui decompose une LISP_LIST en ces differents elements
  en verifiant leur type...
  * Ex: PUGetLispListElem(lispList, 6, 
  *                     FLOAT, doublePtr,
  *                     INTEGER, intPtr,
  *                     STRING, charPtrPtr,
  *                     U_POINTER, voidPtrPtr,
  *                     ATOM, charPtrPtr,
  *                     LISP_LIST, L_ListPtr)
  * Retourne TRUE si tout c'est bien passe !
  * (Dans le cas des STRING les "" sont enleves et il est a
  * la charge de l'utilisateur de desallouer la chaine...
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
	       result &= PU_bind_float(va_arg(listArg, double *),l_car(lispList));
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
{
     Term *paramTerm;

     if (l_length(*lispListPtr) < 1) {
	  return -1;
     }

     paramTerm = l_car(*lispListPtr);
     *lispListPtr = l_cdr(*lispListPtr);


     switch(type) {
     case FLOAT:
	  if (! (PU_bind_float((double *)ptr, paramTerm)))
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
  * Fonction qui ramasse les elements d'un EXPRESSION
  * Ex: PUGetOprsTermCompArgs(tc, 6, 
  *                                FLOAT, doublePtr,
  *                                INTEGER, intPtr,
  *                                STRING, charPtrPtr,
  *                                U_POINTER, voidPtrPtr,
  *                                ATOM, charPtrPtr,
  *                                LISP_LIST, L_ListPtr)
  * Retourne TRUE si tout c'est bien passe !
  * Fortement inspire de la fonction PUGetOprsParamaters...(FFI 22/3/94)
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
	       if (type == ATOM) {
		    char **atom = va_arg(listArg, char**);
		    *atom =  pred_func_rec_symbol(tc->pfr);
	       } else {
		    fprintf(stderr,"PUGetOprsTermCompArgs: Bad parameter %d (expected an ATOM)\n",  paramCour);
		    result = FALSE;
	       }
	  else 
	       switch(type) {
	       case FLOAT:
		    result &= PU_bind_float(va_arg(listArg, double *),
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
	       return FALSE;

	  paramTerm = (Term *)sl_get_slist_pos(paramList, rank);


	  if (! paramTerm) {
	       fprintf(stderr,"PUGetOprsTermCompSpecArg: Unable to get %d argument in TERM COMP.\n",
		       rank);
	       return FALSE;
	  }
	  switch(type) {
	  case FLOAT:
	       if ( ! PU_bind_float((double *)ptr, paramTerm))
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
	  if ( ! PU_bind_float((double *)ptr, paramTerm))
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

/* We get an argument similar:
   (VarArg (order 20)), This was passed from OpenPRS
   10 <the result of the initialization>, "goal.order", int *, INTEGER
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
       char *argName;
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
	      result &= PU_bind_float(va_arg(listArg, double *),
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
	   return (FALSE);
	 }
     }
     
     return(result);
}
