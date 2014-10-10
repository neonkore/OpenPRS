/*                               -*- Mode: C -*- 
 * pu-mk-term.c -- 
 * 
 * Copyright (C) 1993-2012 LAAS/CNRS.
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
#include "oprs-type-pub.h"
#include "oprs-type_f-pub.h"
#include "oprs-pred-func_f.h"

#include "macro-pub.h"
#include "lisp-list_f-pub.h"

#include "pu-mk-term_f.h"

#endif


/*------------------------------------------------------------
  Fonctions utilitaires pour creer des Term * a renvoyer
  a OPRS
  ------------------------------------------------------------*/

Term * PUMakeTermInteger(int val)
{
     return(build_integer(val));
}

Term * PUMakeTermLongLong(long long int val)
{
     return(build_long_long(val));
}

Term* PUMakeTermFloat(double val)
{
     return(build_float(val));
}

Term * PUMakeTermAtom(const char *atom)
{
     return build_id(make_atom(atom));
}

Term* PUMakeTermUPointer(void *pointer)
{
     return(build_pointer(pointer));
}

Term* PUMakeTermUMemory(void *p, int size)
{
     return(build_memory(p, size));
}

Term * PUMakeTermTrue()
{
     return(build_t());
}

Term * PUMakeTermNil()
{
     return(build_nil());
}

Term *PUMakeTermString(const char *string)
{    
     return(build_string(string));
}

Term * PUMakeTermLispList(int NbTerm, ...)
{
     va_list listArg;
     int i;
     Term *result;
     L_List newList = l_nil;
  
     result = MAKE_OBJECT(Term);
     result->type = LISP_LIST;
  
     va_start(listArg, NbTerm);
     for (i = 0; i < NbTerm; i++) {
	  Term *monTerm;
    
	  monTerm = va_arg(listArg, Term *);
	  newList = l_add_to_tail(newList, monTerm);
     }
  
     result->u.l_list = newList;
     va_end(listArg);
     return(result);
}

Term * PUMakeTermComp(Pred_Func_Rec* funcName, int NbTerm, ...)
{
     va_list listArg;
     int i;
     TermList maList = sl_make_slist();

     va_start(listArg, NbTerm);
     for (i = 0; i < NbTerm; i++) {
	  Term *monTerm;

	  monTerm = va_arg(listArg, Term *);
	  sl_add_to_tail(maList, monTerm);
     }

     va_end(listArg);
     return(build_term_expr(build_expr_pfr_terms(funcName, maList)));
}

Term * PUMakeExternalTermComp(char* fName, int NbTerm, ...)
{
     va_list listArg;
     int i;
     Pred_Func_Rec *funcName = find_or_create_pred_func(declare_atom(fName));
     TermList maList = sl_make_slist();

     va_start(listArg, NbTerm);
     for (i = 0; i < NbTerm; i++) {
	  Term *monTerm;

	  monTerm = va_arg(listArg, Term *);
	  sl_add_to_tail(maList, monTerm);
     }

     va_end(listArg);
     return(build_term_expr(build_expr_pfr_terms(funcName, maList)));
}

TermList PUMakeTermList(int NbTerm, ...)
{
     va_list listArg;
     int i;
     TermList maList = sl_make_slist();

     va_start(listArg, NbTerm);
     for (i = 0; i < NbTerm; i++) {
	  Term *monTerm;

	  monTerm = va_arg(listArg, Term *);
	  sl_add_to_tail(maList, monTerm);
     }

     va_end(listArg);
     return(maList);
}

/* Term * PUMakeTermCList(int NbTerm, ...) */
/* { */
/*      va_list listArg; */
/*      int i; */
/*      Term *result; */
  
/*      va_start(listArg, NbTerm); */
  
/*      result = MAKE_OBJECT(Term); */
/*      result->type = C_LIST; */
/*      result->u.var_list = sl_make_slist(); */
  
/*      for (i = 0; i < NbTerm; i++)  */
/* 	  sl_add_to_tail(result->u.var_list, va_arg(listArg, Term *)); */
    
  
/*      va_end(listArg);     */
/*      return(result); */
/* } */
