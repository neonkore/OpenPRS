/*                               -*- Mode: C -*- 
 * ev-function.c -- Deal with evaluable functions in OPRS.
 * 
 * Copyright (c) 1991-2014 Francois Felix Ingrand.
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
#include <shashPack.h>
#include <shashPack_f.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN95
#include <dos.h>
#else
#ifndef VXWORKS
#include <sys/time.h>
#endif
#endif

#ifdef VXWORKS
#include <systime.h>
#include <timers.h>
#include "oprs-type.h"
#include "oprs-profiling.h"
#include "systime.h"
#endif

#include "constant.h"
#include "opaque.h"
#include "macro.h"

#include "ev-function.h"
#include "oprs-type.h"
#include "oprs.h"
#include "op-instance.h"
#include "intention.h"
#include "database.h"
#include "fact-goal.h"
#include "int-graph.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-sprint.h"
#include "type.h"

#include "database_f.h"
#include "lisp-list_f.h"
#include "unification_f.h"
#include "soak_f.h"
#include "intend_f.h"
#include "oprs-type_f.h"
#include "oprs_f.h"
#include "op-instance_f.h"
#include "intention_f.h"
#include "int-graph_f.h"
#include "ev-function_f.h"
#include "action_f-pub.h"
#include "fact-goal_f.h"
#include "oprs-array_f.h"
#include "oprs-pred-func_f.h"
#include "oprs-profiling_f.h"
#include "oprs-rerror_f.h"
#include "oprs-error_f.h"
#include "oprs-sprint_f.h"
#include "type_f.h"

void *make_xoprs_ut_window(void);

Term *ef_sin (TermList terms)
/* This is the standard sin */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(sin((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(sin((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(sin(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_asin (TermList terms)
/* This is the standard sin */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(asin((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(asin((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(asin(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_cos (TermList terms)
/* This is the standard cos */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(cos((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(cos((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(cos(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_acos (TermList terms)
/* This is the standard cos */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(acos((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(acos((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(acos(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_tan (TermList terms)
/* This is the standard tan */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(tan((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(tan((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(tan(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_atan (TermList terms)
/* This is the standard tan */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_FLOAT;
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->u.doubleptr = make_doubleptr(atan((double)t->u.intval));
	  break;
     case LONG_LONG:
	  res->u.doubleptr = make_doubleptr(atan((double)t->u.llintval));
	  break;
     case TT_FLOAT:
	  res->u.doubleptr = make_doubleptr(atan(*t->u.doubleptr));
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_abs (TermList terms)
/* This is the standard abs */
{
     Term *t, *res;
     
     res = MAKE_OBJECT(Term);
     t = (Term *)sl_get_slist_head(terms);
     switch (t->type) {
     case INTEGER:
	  res->type = INTEGER;
	  if (t->u.intval < 0 ) res->u.intval = - t->u.intval;
	  else res->u.intval = t->u.intval;
	  break;
     case LONG_LONG:
	  res->type = LONG_LONG;
	  if (t->u.llintval < 0 ) res->u.llintval = - t->u.llintval;
	  else res->u.llintval = t->u.llintval;
	  break;
     case TT_FLOAT:
	  res->type = TT_FLOAT;
	  if (*t->u.doubleptr < 0.0 ) res->u.doubleptr = make_doubleptr(- *t->u.doubleptr);
	  else res->u.doubleptr = make_doubleptr(*t->u.doubleptr);
	  break;
     default: 	 
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	  break;
     }
     return res;
}

Term *ef_mod (TermList terms)
/* This is the standard mod */
{
     Term *t1, *t2, *res;

     t1 = (Term *)sl_get_slist_next(terms, NULL);
     t2 = (Term *)sl_get_slist_next(terms, t1);

     res = NULL;

     switch (t1->type) {
     case INTEGER:
	  switch (t2->type) {
	  case INTEGER:
	       res = build_integer(((int)t1->u.intval) % ((int)t2->u.intval));
	       break;
	  case LONG_LONG:
	       res = build_long_long(((int)t1->u.intval) % ((int)t2->u.llintval));
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     case LONG_LONG:
	  switch (t2->type) {
	  case INTEGER:
	       res = build_long_long(((int)t1->u.llintval) % ((int)t2->u.intval));
	       break;
	  case LONG_LONG:
	       res = build_long_long(((int)t1->u.llintval) % ((int)t2->u.llintval));
	       break;
	  default:
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	       break;
	  }
	  break;
     default:
	  report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE));
	  break;
     }

     return res;
}

Term *ef_rand (TermList terms)
{
     Term *t1, *res;
     
     res = MAKE_OBJECT(Term);
     res->type = INTEGER;

     t1 = (Term *)sl_get_slist_pos(terms,1);
     if (t1->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     }

     
     res->u.intval = (t1->u.intval ? RANDOM() % t1->u.intval : 0);

     return res;
}

Term *ef_plus (TermList terms)
/* This is the standard + */
{
     Term *t, *res = NULL;
     Term_Type tres = INTEGER;
     int i = 0;
     long long int lli = 0ll;
     double d = 0.0;
     
     sl_loop_through_slist(terms,t, Term *){
	  if (tres == INTEGER )
	       switch (t->type) {
	       case INTEGER:
		    i += t->u.intval;
		    break;
	       case LONG_LONG:
		    tres = LONG_LONG;
		    lli = i + t->u.llintval;
		    break;
	       case TT_FLOAT:
		    tres = TT_FLOAT;
		    d = i + *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else if (tres == LONG_LONG )
	       switch (t->type) {
	       case INTEGER:
		    lli += t->u.intval;
		    break;
	       case LONG_LONG:
		    lli += t->u.llintval;
		    break;
	       case TT_FLOAT:
		    tres = TT_FLOAT;
		    d = lli + *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else if (tres == TT_FLOAT )
	       switch (t->type) {
	       case INTEGER:
		    d += t->u.intval;
		    break;
	       case LONG_LONG:
		    d += t->u.llintval;
		    break;
	       case TT_FLOAT:
		    d += *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 

     }
     if (tres == TT_FLOAT) {
	  res = build_float(d);
     } else if (tres == LONG_LONG) {
	  res = build_long_long(lli);
     } else {
	  res = build_integer(i);
     }

     return res;
}

Term *ef_moins (TermList terms)
/* The standard - */
{
     Term *t, *res;
     Term_Type tres = INTEGER;
     int i = 0;
     long long lli = 0ll;
     double d = 0.0;
     PBoolean first = TRUE;
     
     res = MAKE_OBJECT(Term);
     if (sl_slist_length(terms) == 1) {
	  t = (Term *)sl_get_slist_head(terms);
	  switch (t->type) {
	  case INTEGER:
	       i = -(t->u.intval);
	       break;
	  case LONG_LONG:
	       tres = LONG_LONG;
	       lli = -(t->u.llintval);
	       break;
	  case TT_FLOAT:
	       tres = TT_FLOAT;
	       d = -(*t->u.doubleptr);
	       break;
	  default: 	 
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	       break;
	  }
     } else {
	  sl_loop_through_slist(terms,t, Term *){
	       if (first) {
		    first = FALSE;
		    switch (t->type) {
		    case INTEGER:
			 i = t->u.intval;
			 break;
		    case LONG_LONG:
			 tres = LONG_LONG;
			 lli = t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       } else if (tres == INTEGER )
		    switch (t->type) {
		    case INTEGER:
			 i -= t->u.intval;
			 break;
		    case LONG_LONG:
			 tres = LONG_LONG;
			 lli = i - t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = i - *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       else if (tres == LONG_LONG )
		    switch (t->type) {
		    case INTEGER:
			 lli -= t->u.intval;
			 break;
		    case LONG_LONG:
			 lli -= t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = i - *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       else 
		    switch (t->type) {
		    case INTEGER:
			 d -= t->u.intval;
			 break;
		    case LONG_LONG:
			 d -= t->u.llintval;
			 break;
		    case TT_FLOAT:
			 d -= *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	  }
     }
     if (tres == TT_FLOAT) {
	  res->type = TT_FLOAT;
	  res->u.doubleptr = make_doubleptr(d);
     } else if (tres == LONG_LONG) {
	  res->type = LONG_LONG;
	  res->u.llintval = lli;
     } else {
	  res->type = INTEGER;
	  res->u.intval = i;
     }
     return res;
}

Term *ef_mult (TermList terms)
/* Standard * */
{
     Term *t, *res = NULL;
     Term_Type tres = INTEGER;
     int i = 1.0;
     long long int lli = 1ll;
     double d = 1.0;
     
     sl_loop_through_slist(terms,t, Term *){
	  if (tres == INTEGER )
	       switch (t->type) {
	       case INTEGER:
		    i *= t->u.intval;
		    break;
	       case LONG_LONG:
		    tres = LONG_LONG;
		    lli = i * t->u.llintval;
		    break;
	       case TT_FLOAT:
		    tres = TT_FLOAT;
		    d = i * *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else if (tres == LONG_LONG )
	       switch (t->type) {
	       case INTEGER:
		    lli *= t->u.intval;
		    break;
	       case LONG_LONG:
		    lli *= t->u.llintval;
		    break;
	       case TT_FLOAT:
		    tres = TT_FLOAT;
		    d = lli * *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else if (tres == TT_FLOAT )
	       switch (t->type) {
	       case INTEGER:
		    d *= t->u.intval;
		    break;
	       case LONG_LONG:
		    d *= t->u.llintval;
		    break;
	       case TT_FLOAT:
		    d *= *t->u.doubleptr;
		    break;
	       default: 	 
		    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
		    break;
	       }
	  else
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 

     }
     if (tres == TT_FLOAT) {
	  res = build_float(d);
     } else if (tres == LONG_LONG) {
	  res = build_long_long(lli);
     } else {
	  res = build_integer(i);
     }

     return res;
}


Term *ef_div (TermList terms)
/* Standard / */
{
     Term *t, *res;
     Term_Type tres = INTEGER;
     int i = 1;
     long long lli = 1ll;
     double d = 1.0;
     PBoolean first = TRUE;
     
     res = MAKE_OBJECT(Term);
     if (sl_slist_length(terms) == 1) {
	  t = (Term *)sl_get_slist_head(terms);
	  switch (t->type) {
	  case INTEGER:
	       tres = TT_FLOAT;
	       d = 1 / (t->u.intval);
	       break;
	  case LONG_LONG:
	       tres = TT_FLOAT;
	       d = 1 / (t->u.llintval);
	       break;
	  case TT_FLOAT:
	       tres = TT_FLOAT;
	       d = 1 / (*t->u.doubleptr);
	       break;
	  default: 	 
	       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
	       break;
	  }
     } else {
	  sl_loop_through_slist(terms,t, Term *){
	       if (first) {
		    first = FALSE;
		    switch (t->type) {
		    case INTEGER:
			 i = t->u.intval;
			 break;
		    case LONG_LONG:
			 tres = LONG_LONG;
			 lli = t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       } else if (tres == INTEGER )
		    switch (t->type) {
		    case INTEGER:
			 i /= t->u.intval;
			 break;
		    case LONG_LONG:
			 tres = LONG_LONG;
			 lli = i / t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = i / *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       else if (tres == LONG_LONG )
		    switch (t->type) {
		    case INTEGER:
			 lli /= t->u.intval;
			 break;
		    case LONG_LONG:
			 lli /= t->u.llintval;
			 break;
		    case TT_FLOAT:
			 tres = TT_FLOAT;
			 d = i - *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	       else 
		    switch (t->type) {
		    case INTEGER:
			 d /= t->u.intval;
			 break;
		    case LONG_LONG:
			 d /= t->u.llintval;
			 break;
		    case TT_FLOAT:
			 d /= *t->u.doubleptr;
			 break;
		    default: 	 
			 report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
			 break;
		    }
	  }
     }
     if (tres == TT_FLOAT) {
	  res->type = TT_FLOAT;
	  res->u.doubleptr = make_doubleptr(d);
     } else if (tres == LONG_LONG) {
	  res->type = LONG_LONG;
	  res->u.llintval = lli;
     } else {
	  res->type = INTEGER;
	  res->u.intval = i;
     }
     return res;
}

Term *string_cat_ef(TermList terms)
/* Concatenate 2 strings. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
     if (t2->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     res->type = STRING;
     res->u.string = (char *)MALLOC((strlen(t1->u.string) +
				     strlen(t2->u.string) + 1) * sizeof(char));
     
     sprintf(res->u.string,"%s%s",t1->u.string,t2->u.string);
     return res;
}

Term *term_string_cat_ef(TermList terms)
/* Concatenate any terms. */
{
     Term *t, *res;
     Sprinter *sp = make_sprinter(0);
     PBoolean save_pb = print_backslash;

     print_backslash = FALSE;

     sl_loop_through_slist(terms,t, Term *){
	  sprint_term(sp,t);
     }
     
     print_backslash = save_pb;

     res = build_string(SPRINTER_STRING(sp));

     free_sprinter(sp);
     
     return res;
}

Term *val_ef(TermList terms)
{
     Term *res = (Term *)sl_get_slist_head(terms); /* There is not need to check the argument.. 
						  we know it is present (because of the arg checking. */
     
     return find_binding_ef(res);	/* find its value in case it is not a final value.. */
}

/* Now we define some useful lisp like functions. */

Term *l_length_ef(TermList terms)
/* Return an integer, the length of a LISP_LIST object. */
{
     Term *t,*res;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = L_LENGTH(t->u.l_list);

     return res;
}

Term *l_last_ef(TermList terms)
/* Return a LISP_CAR Object which last element of the LISP_LIST. */
{
     Term *t;
     Term *res;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res = l_last(t->u.l_list);
     if (! res) report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     return res;
}

Term *l_car_ef(TermList terms)
/* Return an LISP_CAR Object which is the car of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     if (! (t = CAR_DUP(t->u.l_list)))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *term_car_ef(TermList terms)
/* Return an Term Object which is the node of the car of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     WARN_OBSOLETE(LG_STR("Function TERM-CAR not needed anymore.\n\
\tUse CAR instead.",
			  "Fonction TERM-CAR n'est plus nécessaire.\n\
\tUtilisez CAR."));
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if (! l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return dup_term(l_car);
}


Term *l_cdr_ef(TermList terms)
/* Return a LISP_LIST object which is the CDR element of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(CDR_DUP(t->u.l_list));
}

Term *l_caar_ef(TermList terms)
/* Return a LISP_CAR Object which is the caar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if (!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     if (l_car->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return CAR_DUP(l_car->u.l_list);
}

Term *l_cadr_ef(TermList terms)
/* Return a LISP_CAR Object which is the cadr of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (! (t = (CAR_DUP(CDR(t->u.l_list)))))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_cdar_ef(TermList terms)
/* Return a LISP_CAR Object which is the cdar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if (!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(CDR_DUP(t->u.l_list));
}

Term *l_cddr_ef(TermList terms)
/* Return a LISP_LIST object which is the CDDR element of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(dup_l_list(CDR(CDR(t->u.l_list))));
}

Term *l_caaar_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (! (t = CAR_DUP(t->u.l_list)))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_caadr_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(CDR(t->u.l_list));
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE)); 
     if (! (t = CAR_DUP(t->u.l_list)))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_cadar_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (! (t = CAR_DUP(CDR(t->u.l_list))))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_caddr_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (! (t = CAR_DUP(CDR(CDR(t->u.l_list)))))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_cdaar_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(CDR_DUP(t->u.l_list));
}

Term *l_cdadr_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(CDR(t->u.l_list));
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(CDR_DUP(t->u.l_list));
}

Term *l_cddar_ef(TermList terms)
/* Return a LISP_CAR Object which is the caaar of the LISP_LIST. */
{
     Term *t;
     Term *l_car;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     l_car = CAR(t->u.l_list);
     if(!l_car)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t = l_car;
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(CDR_DUP(CDR(t->u.l_list)));
}

Term *l_cdddr_ef(TermList terms)
/* Return a LISP_LIST object which is the CDDDR element of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     return build_l_list(dup_l_list(CDR(CDR(CDR(t->u.l_list)))));
}

Term *l_second_ef(TermList terms)
/* Return a LISP_CAR Object which is the second element of the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (! (t = CAR_DUP(CDR(t->u.l_list))))
	 report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     return t;
}

Term *l_nth_ef(TermList terms)
/* Return the LISP_CAR object which is the n^{th} element of the
   LISP_LIST, numbering from 0.
*/
{
  Term
    *n_term = (Term *)sl_get_slist_pos(terms,1),
    *list_term = (Term *)sl_get_slist_pos(terms,2);
  int n;
  L_List list;

  if (n_term->type != INTEGER)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
  n = n_term->u.intval;

  if (list_term->type != LISP_LIST)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
  list = list_term->u.l_list;

  while (n>0) {
    if (! (list = CDR(list)))
         report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
    --n;
  }

  return CAR_DUP(list);
}

Term *lisp_car_to_term_ef(TermList terms)
/* Return a Term object which is the node of a  LISP_CAR. */
{ 
     WARN_OBSOLETE(LG_STR("Function LISP-CAR-TO-TERM not needed anymore.\n\
\tRemove its call from your code.",
			  "Fonction LISP-CAR-TO-TERM n'est plus nécessaire.\n\
\tRetirez les appels de votre code."));

     return dup_term((Term *)sl_get_slist_pos(terms,1));

}

Term *l_cons_term_ef(TermList terms)
{
     Term *t1, *t2, *res;
     
     WARN_OBSOLETE(LG_STR("Function CONS-TERM not needed anymore.\n\
\tReplace it with CONS in  your code.",
			  "Fonction CONS-TERM n'est plus nécessaire.\n\
\tRemplacer par un appel a CONS dans votre code."));

     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     if (t2->type != LISP_LIST) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = cons(dup_term(t1), dup_l_list(t2->u.l_list));

      return res;
}

Term *term_to_lisp_car_ef(TermList terms)
/* Return a LISP_CAR object which is the contains the Term. */
{
     Term *t1;

     WARN_OBSOLETE(LG_STR("Function TERM-TO-LISP-CAR not needed anymore.\n\
\tRemove its call from your code.",
			  "Fonction TERM-TO-LISP-CAR n'est plus nécessaire.\n\
\tRetirez les appels de votre code."));
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     return dup_term(t1);

}

Term *l_cons_ef(TermList terms)
/* Return a LISP_LIST object which is the CONS of first and element of the LISP_LIST. */
{
     Term *t1, *t2, *res;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     if (t2->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = cons(dup_term(t1), dup_l_list(t2->u.l_list));

     return res;
}

Term *l_cons_tail_ef(TermList terms)
/* Return a LISP_LIST object which add the car at the end of the LISP_LIST. */
{
     Term *t1, *t2, *res;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     if(!t1)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     t2 = (Term *)sl_get_slist_pos(terms,2);
     if (t2->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = l_add_to_tail(copy_l_list(t2->u.l_list),t1);

     return res;
}

Term *l_reverse_ef(TermList terms)
/* Return a LISP_LIST object which is the REVERSE of the LISP_LIST. */
{
     Term *t,*res;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = l_reverse(t->u.l_list);

     return res;
}

Term *l_sort_ef(TermList terms)
/* Return a LISP_LIST object which is the REVERSE of the LISP_LIST. */
{
     Term *t,*res;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;

     res->u.l_list = l_sort(t->u.l_list);

     return res;
}

Term *l_select_randomly_ef(TermList terms)
/* Return a L_CAR object which has been randomly choosen in the LISP_LIST. */
{
     Term *t;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));

     return dup_term(select_randomly_l_list(t->u.l_list));
     
}

Term *apply_subst_in_expr_ef(TermList terms)
/* Return a Term Expressopm object which has been created by the subsitution of the first var by the second val in the third. */
{
     Term *var, *value, *expr_term, *res;
     Envar *envar;
     Frame *fr;
     Expression *expr;
     PBoolean bool_tmp;
     List_Envar lenv = sl_make_slist();
     
     var = (Term *)sl_get_slist_pos(terms,1);
     if (var->type != VARIABLE)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));

     value = (Term *)sl_get_slist_pos(terms,2);
     if (!value)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     expr_term= (Term *)sl_get_slist_pos(terms,3);
     if (expr_term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     expr = expr_term->u.expr;

     envar = copy_envar(var->u.var);
     sl_add_to_head(lenv,envar);
     fr = make_frame(empty_env);
     bind_var(envar,value,fr);
     /* May be we could free the frame here.. */
     bool_tmp = check_all_vars_in_subst_lenv;
     check_all_vars_in_subst_lenv = FALSE;
     res = build_term_expr(subst_lenv_in_expr(expr,lenv,QET_NONE));
     check_all_vars_in_subst_lenv = bool_tmp;

     return res;
}

Term *apply_subst_in_goal_ef(TermList terms)
/* Return a L_CAR Goal object which has been created by the subsitution of the first var by the second val in the third. */
{
     Term *var, *value, *expr_term, *res;
     Goal *goal;
     Envar *envar;
     Frame *fr;
     PBoolean bool_tmp;
     Expression *expr;
     List_Envar lenv = sl_make_slist();
     
     var = (Term *)sl_get_slist_pos(terms,1);
     if (var->type != VARIABLE)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));

     value = (Term *)sl_get_slist_pos(terms,2);
     if (!value)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));

     expr_term= (Term *)sl_get_slist_pos(terms,3);
     if (expr_term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     expr = expr_term->u.expr;

     envar = copy_envar(var->u.var);
     sl_add_to_head(lenv,envar);
     fr = make_frame(empty_env);
     bind_var(envar,value,fr);

     bool_tmp = check_all_vars_in_subst_lenv;
     check_all_vars_in_subst_lenv = FALSE;
     goal = make_goal_from_expr(subst_lenv_in_expr(expr,lenv,QET_NONE), fr);
     check_all_vars_in_subst_lenv = bool_tmp;

     free_frame(fr);     /* it was duplicated in make_goal_from_expr */
     res = build_goal_term(goal);

     return res;
}

Term *build_goal_ef(TermList terms)
/* Return a L_CAR Goal object which has been created from the expr. */
{
     Term *expr_term, *res;
     Goal *goal;
     Frame *fr;
     Expression *expr;
     
     expr_term= (Term *)sl_get_slist_pos(terms,1);
     if (expr_term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     expr = dup_expr(expr_term->u.expr);


     fr = make_frame(empty_env);
     goal = make_goal_from_expr(expr, fr);

     free_frame(fr);     /* it was duplicated in make_goal_from_expr */
     res = build_goal_term(goal); /* There was a dup_goal here which
				     does not seem to be justified. */
     return res;
}


Term *transform_pos_expr_to_expr(Expression *expr)
{
     return build_term_expr(expr);
}

Term *goal_statement_predicate_ef(TermList terms)
{
     Term *term, *res;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     res = MAKE_OBJECT(Term);

     res->type = TT_ATOM;
     res->u.id = EXPR_EXPR1(term->u.expr)->pfr->name;

     return res;
}

Term *goal_statement_arg_ef(TermList terms)
{
     Term *term, *pos;
     TermList tl;

     term = (Term *)sl_get_slist_pos(terms,1);
     pos = (Term *)sl_get_slist_pos(terms,2);
     if (pos->type != INTEGER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tl = EXPR_TERMS(EXPR_EXPR1(term->u.expr));

     if (list_length(tl) < pos->u.intval) {
	  report_recoverable_external_error("Expr has less argument than required.");
	  return build_nil();
     }

     return dup_term((Term *)sl_get_slist_pos(tl,pos->u.intval));
}

Term *copy_term_keeping_variable_unique_ef(TermList terms)
{
     Term *res;

     copy_var_uniquely = sl_make_slist();
     
     res = copy_term((Term *)sl_get_slist_pos(terms,1));
     
     FREE_SLIST(copy_var_uniquely);
     copy_var_uniquely = NULL;

     return res;
}

Term *copy_term_ef(TermList terms)
{
     return copy_term((Term *)sl_get_slist_pos(terms,1));
}

Term *goal_statement_arg_cp_ef(TermList terms)
{
     Term *term = goal_statement_arg_ef(terms);
     Term *res =  copy_term(term);
     
     free_term(term);

     return res;
}

Term *goal_statement_ef(TermList terms)
{
     Term *term, *res;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != TT_GOAL)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_GOAL_TERM_TYPE));

     res = MAKE_OBJECT(Term);

     res->type = EXPRESSION;
     res->u.expr = dup_expr(term->u.goal->statement);

     return res;
}

Term *goal_statement_cp_ef(TermList terms)
{
     Term *tmp = goal_statement_ef(terms);
     Term *res = copy_term(tmp);

     free_term(tmp);

     return res;
}

Term *fact_statement_ef(TermList terms)
{
     Term *term;
     Expression *expr = NULL;
     
     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != TT_FACT)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FACT_TERM_TYPE));

     return build_term_expr(dup_expr(expr));
}

Term *fact_statement_cp_ef(TermList terms)
{
     Term *tmp = fact_statement_ef(terms);
     Term *res = copy_term(tmp);

     free_term(tmp);

     return res;
}

Term *fact_statement_predicate_ef(TermList terms)
{
     Term *term, *res;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     res = MAKE_OBJECT(Term);

     res->type = TT_ATOM;
     res->u.id = term->u.expr->pfr->name;

     return res;
}


Term *fact_statement_arg_ef(TermList terms)
{
     Term *term, *pos;
     TermList tl;

     term = (Term *)sl_get_slist_pos(terms,1);
     pos = (Term *)sl_get_slist_pos(terms,2);
     if (pos->type != INTEGER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tl = term->u.expr->terms;

     if (list_length(tl) < pos->u.intval) {
	  report_recoverable_external_error("EXPR has less argument than required.");
	  return build_nil();
     }

     return dup_term((Term *)sl_get_slist_pos(tl,pos->u.intval));
}

Term *fact_statement_arg_cp_ef(TermList terms)
{
     Term *term = fact_statement_arg_ef(terms);
     Term *res =  copy_term(term);
     
     free_term(term);

     return res;
}

Term *tc_function_ef(TermList terms)
{
     Term *term, *res;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     res = MAKE_OBJECT(Term);

     res->type = TT_ATOM;
     res->u.id = term->u.expr->pfr->name;

     return res;
}

Term *make_atom_ef(TermList terms)
{
     Term *t;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != STRING) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     return build_id(make_atom(t->u.string));
}


Term *tc_arg_ef(TermList terms)
{
     Term *term, *pos;
     TermList tl;

     term = (Term *)sl_get_slist_pos(terms,1);
     pos = (Term *)sl_get_slist_pos(terms,2);
     if (pos->type != INTEGER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     if (term->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tl = term->u.expr->terms;

     if (list_length(tl) < pos->u.intval) {
	  report_recoverable_external_error("EXPRESSION has less argument than required.");
	  return build_nil();
     }

     return dup_term((Term *)sl_get_slist_pos(tl,pos->u.intval));
}

Term *tc_arg_cp_ef(TermList terms)
{
     Term *term = tc_arg_ef(terms);
     Term *res =  copy_term(term);
     
     free_term(term);

     return res;
}


Term *n_all_ef(TermList terms)
/* N-ALL is used in OPs such as in (! (.. (n-all ($x $y) (foo $y $x)) .. )) which will return
 * the LISP_LIST of LISP_LIST containing instance of $x $y which satisfy (foo $y $x). 
 */
{
     Term *t1, *t2, *res;
     Expression *expr;
     ExprFrameList efl;
     ExprFrame *ef;
     VarList var_list;	
     TermList term_list = sl_make_slist();

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     var_list = make_c_list_from_l_list(t1->u.l_list);
     if ((t2->type != EXPRESSION) && (t2->type != EXPRESSION)) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE)); 
     
     expr = dup_expr(t2->u.expr);
     
     efl = consult(expr, empty_env, current_oprs->database, FALSE);
     
     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  Term *t_var;
	  Term *t_res =MAKE_OBJECT(Term);

	  TermList term_list2 = sl_make_slist();

	  reinstall_frame(ef->frame);
	  
	  sl_loop_through_slist(var_list, t_var, Term *) {
	       Term *t_res2;

	       if (t_var->type != VARIABLE)
		    report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));

	       t_res2 = copy_term(t_var->u.var->value);
	       sl_add_to_tail(term_list2, t_res2);
	  }
	  
	  desinstall_frame(ef->frame, empty_env);

	  t_res->type = LISP_LIST;
	  t_res->u.l_list = transform_c_list_to_l_list(term_list2);

	  sl_add_to_head(term_list, t_res);
	  
	  free_frame(ef->frame);
	  FREE(ef);
     }
     
     FREE_SLIST(var_list);
     FREE_SLIST(efl);
     free_expr(expr);

     res->type = LISP_LIST;
     res->u.l_list = transform_c_list_to_l_list(term_list);
 
     return res;
}

Term *n_all_list_ef(TermList terms)
/* N-ALL-LIST is used in OPs such as in (! (.. (n-all-list ($x $y) (foo $y $x)) .. )) which will return
 * the LISP_LIST of $x LISP_LIST and $y LISP_LIST which satisfy (foo $y $x). 
 */
{
     Term *t1, *t2, *res;
     Expression *expr;
     ExprFrameList efl;
     VarList var_list;	
     ExprFrame *ef;
     Term *t_var;
     TermList term_list = sl_make_slist();

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     var_list = make_c_list_from_l_list(t1->u.l_list);
     if ((t2->type != EXPRESSION) && (t2->type != EXPRESSION)) 
	     report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE)); 
     
     expr = dup_expr(t2->u.expr);
     
     efl = consult(expr, empty_env, current_oprs->database, FALSE);
     
     sl_loop_through_slist(var_list, t_var, Term *) {
	  
	  Term *t_res =MAKE_OBJECT(Term);

	  TermList term_list2 = sl_make_slist();

	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       Term *t_res2;
	       
	       reinstall_frame(ef->frame);
	       
	       if (t_var->type != VARIABLE)
		    report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));

	       t_res2 = copy_term(t_var->u.var->value);
	       sl_add_to_tail(term_list2, t_res2);

	       desinstall_frame(ef->frame, empty_env);
	  }
	  

	  t_res->type = LISP_LIST;
	  t_res->u.l_list = transform_c_list_to_l_list(term_list2);

	  sl_add_to_tail(term_list, t_res);
	  
     }

     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  
	  free_frame(ef->frame);
	  FREE(ef);

     }
     
     FREE_SLIST(var_list);
     FREE_SLIST(efl);
     free_expr(expr);

     res->type = LISP_LIST;
     res->u.l_list = transform_c_list_to_l_list(term_list);
 
     return res;
}

Term *all_ef(TermList terms)
/* ALL returns a LISP_LIST of unique instance of a variable in an expr checked in the DB */
{
     Term *t1, *t2,*res;
     Expression *expr;
     ExprFrameList efl;
     ExprFrame *ef;
     TermList term_list = sl_make_slist();

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != VARIABLE)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));
     if ((t2->type != EXPRESSION) && (t2->type != EXPRESSION))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE)); 

     expr = dup_expr(t2->u.expr);
     
     efl = consult(expr, empty_env, current_oprs->database, FALSE);
     
     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  Term *t_res, *t_res2;

	  reinstall_frame(ef->frame);

	  t_res2 = find_binding_ef(t1); /* This will return a dup.. we want a real copy. */
	  t_res = copy_term(t_res2);
	  free_term(t_res2);

	  desinstall_frame(ef->frame, empty_env);

	  if (sl_search_slist(term_list, t_res, equal_term) == NULL)
	       sl_add_to_head(term_list, t_res);
	  
	  free_frame(ef->frame);
	  FREE(ef);
     }

     FREE_SLIST(efl);
     free_expr(expr);

     res->type = LISP_LIST;
     res->u.l_list = transform_c_list_to_l_list(term_list);

     return res;
}

Term *mention_ef(TermList terms)
/* ALL returns a LISP_LIST of unique instance of expr mentioning TERM in the DB */
{
     Term *t1,*res;
     ExprList el;
     ExprList resl = sl_make_slist();
     Expression *expr;

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     
     /* This will return a list of expr copy... */
     el = collect_expr_mentioning_term_2(current_oprs->database,t1);
     
     sl_loop_through_slist(el, expr, Expression *) {
	  sl_add_to_head(resl,build_term_expr(expr));
     }
     
     FREE_SLIST(el);

     res->type = LISP_LIST;
     res->u.l_list = transform_c_list_to_l_list(resl);

     return res;
}

Term *all_pos_ef(TermList terms)
/* ALL returns a LISP_LIST of all the pos elt in an expr checked in the DB */
{
     Term *t1, *t2,*res;
     Expression *expr = NULL;
     ExprFrameList efl;
     ExprFrame *ef;
     TermList term_list = sl_make_slist();

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != INTEGER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));

     if (t2->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE)); 
     else
	  expr = dup_expr(t2->u.expr);
     
     efl = consult(expr, empty_env, current_oprs->database, TRUE);
     
     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  sl_add_to_head(term_list, copy_term((Term *)sl_get_slist_pos(ef->expr->terms,
							       t1->u.intval)));
	  
	  free_frame(ef->frame);
	  free_expr(ef->expr);
	  FREE(ef);
     }

     FREE_SLIST(efl);
     free_expr(expr);

     res->type = LISP_LIST;
     res->u.l_list = transform_c_list_to_l_list(term_list);

     return res;
}

Term *ff_val_ef(TermList terms)
/* ff_val_ef returns a Term* which is the term in ff expr checked in the DB */
{
     Term *t1, *t2,*res;
     Expression *expr;
     ExprFrameList efl;
     ExprFrame *ef;
     PBoolean first;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != VARIABLE) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_VARIABLE_TERM_TYPE));
     }
     
     if (t2->type != EXPRESSION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE)); 
	  return res;
     } else 
	  expr = dup_expr(t2->u.expr);
     
     efl = consult(expr, empty_env, current_oprs->database, FALSE);
     
     if (PFR_FF(expr->pfr) == -1) {
	  report_fatal_external_error(LG_STR("ff_val_ef called with a predicate %s which has not been declared as a functional fact.",
					     "ff_val_ef appelé avec le prédicat %s qui n'a pas été déclaré comme fait fonctionel."));
	  return res;
     }

     if (sl_slist_empty(efl)) {
	  return res;
     }
     
     if (sl_slist_length(efl) > 1) {
	  report_fatal_external_error(LG_STR("ff_val_ef found more than one value for a functional fact.",
					     "ff_val_ef: trouvé plus d'une valeur pour un fait fonctionel.")); 
	  
     }

     first = TRUE;

     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  Term *t_res2;

	  if (first) {
	       reinstall_frame(ef->frame);

	       FREE(res);

	       t_res2 = find_binding_ef(t1); /* This will return a dup.. we want a real copy. */
	       res = copy_term(t_res2);
	       free_term(t_res2);
	       
	       desinstall_frame(ef->frame, empty_env);
	  }

	  free_frame(ef->frame);
	  FREE(ef);

	  first = FALSE;
     }

     FREE_SLIST(efl);
     free_expr(expr);
     
     return res;
}

Term *make_int_array_ef(TermList terms)
{
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
	  return make_int_array(0);
     }
     
     return make_int_array(t1->u.intval);
}

Term *make_float_array_ef(TermList terms)
{
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
	  return make_float_array(0);
     }
     
     return make_float_array(t1->u.intval);
}

Term *get_int_array_ef(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);
     Term *t2 = (Term *)sl_get_slist_pos(terms, 2);

     res->type = INTEGER;
     res->u.intval = 0;

     if (t1->type != INT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INT_ARRAY_TERM_TYPE));
     } else if (t2->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     } else {
	  int i = t2->u.intval;

	  if ((i >= 0) && (i < (int)t1->u.int_array->size)) {
	       res->u.intval = t1->u.int_array->array[i];
	  } else
	       report_fatal_external_error(oprs_strerror(PE_OUT_OF_BOUND_ARRAY_ACCES));
     }
     return res;
}

Term *get_float_array_ef(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);
     Term *t2 = (Term *)sl_get_slist_pos(terms, 2);

     res->type = TT_FLOAT;

     if (t1->type != FLOAT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_ARRAY_TERM_TYPE));
     } else if (t2->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     } else  {
	  int i = t2->u.intval;

	  if ((i >= 0) && (i < (int)t1->u.float_array->size)) {
	       res->u.doubleptr = make_doubleptr(t1->u.float_array->array[i]);
	  } else
	       report_fatal_external_error(oprs_strerror(PE_OUT_OF_BOUND_ARRAY_ACCES));
     }
     return res;
}

Term *get_int_array_size_ef(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);

     res->type = INTEGER;
     res->u.intval = 0;

     if (t1->type != INT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INT_ARRAY_TERM_TYPE));
     } else  {
	  res->u.intval = t1->u.int_array->size;
     }
     return res;
}

Term *get_float_array_size_ef(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);

     res->type = INTEGER;
     res->u.intval = 0;

     if (t1->type != FLOAT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_ARRAY_TERM_TYPE));
     } else  {
	  res->u.intval = t1->u.float_array->size;
     }
     return res;
}

Term *type_of_ef(TermList terms)
{
     Term *t1;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     }
     return  build_id(type_of(t1->u.id));

}

Term *elts_of_type_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     res->type = LISP_LIST;
     res->u.l_list =  l_nil;
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     }
     res->u.l_list = transform_c_list_to_l_list(sl_copy_slist_func(elts_of_type(t1->u.id), build_id));

     return res;
} 

Term *surtypes_of_type_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     res->type = LISP_LIST;
     res->u.l_list =  l_nil;
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     }
     res->u.l_list = transform_c_list_to_l_list(sl_copy_slist_func(surtypes_of_type(t1->u.id), build_id_from_type));

     return res;
}

Term *make_l_list_ef(TermList terms)
/* This will return the LISP_LIST composed of the terms list */
{
     Term *res;

     res = MAKE_OBJECT(Term);

     res->type = LISP_LIST;
     res->u.l_list = make_l_list_from_c_list(terms);	/* This will dup all the elt. */

     return res;
}

Term *list_delete_ef(TermList terms)
/* This will return a LISP_LIST which is the difference of the two given as parameter. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = l_nil;
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if(!t1) {
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
	  return res;
     }
     if (t2->type != LISP_LIST) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
	  return res;
     }

     res->u.l_list = l_delete(t1, t2->u.l_list);
     return res;
}

Term *list_append_ef(TermList terms)
/* This will return a new LISP_LIST with the 2 LISP_LIST appended (the second one is not copied). */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != LISP_LIST) || (t2->type != LISP_LIST))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = l_list_append(t1->u.l_list, t2->u.l_list);
     return res;
}

Term *list_difference_ef(TermList terms)
/* This will return a LISP_LIST which is the difference of the two given as parameter. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != LISP_LIST) || (t2->type != LISP_LIST))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = l_list_difference(t1->u.l_list, t2->u.l_list);
     return res;
}

Term *list_intersection_ef(TermList terms)
/* This will return a LISP_LIST which is the intersection of the two given as parameter. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != LISP_LIST) || (t2->type != LISP_LIST))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = l_list_intersection(t1->u.l_list, t2->u.l_list);
     return res;
}

Term *list_union_ef(TermList terms)
/* This will return a LISP_LIST which is the union of the two given as parameter. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != LISP_LIST) || (t2->type != LISP_LIST))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = l_list_union(t1->u.l_list, t2->u.l_list);
     return res;
}

Term *list_difference_order_ef(TermList terms)
/* This will return a LISP_LIST which is the difference of the two given as parameter. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if ((t1->type != LISP_LIST) || (t2->type != LISP_LIST))
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = l_list_difference_order(t1->u.l_list, t2->u.l_list);
     return res;
}

Term *preferred_of_ef(TermList terms)
{
     Term *t1,*res;
     static Symbol prop = NULL;

     if (!prop) DECLARE_TEXT_ID("PREFERRED",prop);

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = satisfy_prop_l_list_opi(t1->u.l_list,prop);
     
     return res;
}

Term *safety_handlers_of_ef(TermList terms)
{
     Term *t1,*res;
     static Symbol prop = NULL;

     if (!prop) DECLARE_TEXT_ID("SAFETY-HANDLER",prop);

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = satisfy_prop_l_list_opi(t1->u.l_list,prop);
     
     return res;
}

Term *fact_invoked_ops_of_ef(TermList terms)
{
     Term *t1,*res;

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = fact_invoked_ops_l_list_opi(t1->u.l_list);
     
     return res;
}

Term *op_instance_goal_ef(TermList terms)
/* Return the goal for which a op-instance in a LISP_CAR is working on. */
{
     Term *t1,*res;
     Op_Instance *opi;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if(!t1)
	  report_fatal_external_error(oprs_strerror(PE_CAR_FROM_EMPTY_LISP_LIST));
     opi = (Op_Instance *)(t1->u.opi);
     if (opi->goal) {
	  res = build_goal_term(dup_goal(opi->goal));
     } else {
	  report_fatal_external_error(LG_STR("Trying to get a goal from fact op_instance in op_instance_goal_ef.",
					     "Tentative d'obtenir un but d'une OP instance invoquée par fait dans la fonction op_instance_goal_ef."));
	  res = MAKE_OBJECT(Term);
	  res->type = TT_ATOM;
	  res->u.id = lisp_t_sym;
     }
     
     return res;
}

Term *current_intention_ef(TermList terms)
{
     Term *t_res =MAKE_OBJECT(Term);
     Intention *ci = current_intention;

     t_res->type = TT_INTENTION;
     t_res->u.in = dup_intention(ci);
     
     return t_res;
}

Term *list_all_intentions_ef(TermList terms)
{
     Term *t_res =MAKE_OBJECT(Term);
     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));

     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(il, TT_INTENTION);
     
     return t_res;
}

Term *list_other_intentions_ef(TermList terms)
{
     Term *t_res = MAKE_OBJECT(Term);
     Intention_List il, il_res;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci;

     ci = current_intention;
     il = ig->list_intentions;

     il_res = COPY_SLIST(il); 

     sl_delete_slist_node(il_res,ci);

     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(il_res, TT_INTENTION);
     FREE_SLIST(il_res);

     return t_res;
}

Term *list_sleeping_intentions_ef(TermList terms)
{
     Term *t_res =MAKE_OBJECT(Term);
     Intention_Graph *ig = current_oprs->intention_graph;

     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(ig->sleeping, TT_INTENTION);
     
     return t_res;
}

Term *list_root_intentions_ef(TermList terms)
{
     Term *t_res =MAKE_OBJECT(Term);
     Intention_Graph *ig = current_oprs->intention_graph;

     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(ig->list_first, TT_INTENTION);
     
     return t_res;
}

Term *number_of_intentions_ef(TermList terms)
{
     Term *res;

     res = MAKE_OBJECT(Term);
     
     res->type = INTEGER;
     res->u.intval = sl_slist_length(current_oprs->intention_graph->list_intentions); 

     return res;
}

Term *find_intentions_id_ef(TermList tl)
{
     Term *t1,*t_res =MAKE_OBJECT(Term);
     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));
     Intention_List il_res = sl_make_slist();
     Intention *in;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     if (t1->type != TT_ATOM)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     
   
     sl_loop_through_slist(il, in, Intention *) {
	  if (in->id == t1->u.id)
	       sl_add_to_tail(il_res,in);
     }
     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(il_res,TT_INTENTION);
     
     FREE_SLIST(il_res);

     return t_res;
}

Term *find_intention_id_ef(TermList tl)
{
     Term *t1,*t_res = MAKE_OBJECT(Term);
     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));
     Intention *in;

     t1 = (Term *)sl_get_slist_pos(tl, 1);
     if (t1->type != TT_ATOM)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     
     t_res->type = TT_ATOM;
     t_res->u.id = nil_sym;
     
     sl_loop_through_slist(il, in, Intention *) {
	  if (in->id == t1->u.id) {
	       t_res->type = TT_INTENTION;
	       t_res->u.in = dup_intention(in);
	       break;
	  }
     }
     
     return t_res;
}

Term *get_intention_priority_ef(TermList terms)
{
     Term *res, *t1;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *in;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if(!t1 || t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	  return res;
     }

     in = t1->u.in;

     if (valid_intention(ig, in)) {
	  res->type = INTEGER;
	  res->u.intval = intention_priority(in);
     } else {
	  report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
     }
     return res;
}

Term *current_intention_id_ef(TermList terms)
{
     Term *res;
     Intention *ci = current_intention;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = intention_id(ci);
     return res;
}

Term *get_intention_id_ef(TermList terms)
{
     Term *res, *t1;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *in;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if(!t1 || t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	  return res;
     } 
     in = t1->u.in;

     if (valid_intention(ig, in)) 
	  res->u.id = intention_id(in);
     else 
	  report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));

     return res;
}

Term *get_intention_time_ef(TermList terms)
{
     Term *res, *t1;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *in;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if(!t1 || t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	  return res;
     } 
     in = t1->u.in;

     if (valid_intention(ig, in)) {
	  res->type = INTEGER;
	  res->u.intval = (int) intention_time(in);
     } else {
	  report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
     }
     return res;
}


Term *make_window_ef(TermList terms)
{
     return build_pointer(make_xoprs_ut_window());
}

Term *gensym_ef(TermList terms)
{
     Term *res;
     static int i = 0;
     char name[64];

     res = MAKE_OBJECT(Term);
     
     res->type = TT_ATOM;
#ifdef VXWORKS
     take_or_create_sem(&id_hash_sem);
#endif     
     do {
	  sprintf(name, "G%d", i++);
     } while (get_and_check_from_hashtable(id_hash,name) != NULL);
#ifdef VXWORKS
     give_sem(id_hash_sem);
#endif     
     DECLARE_ID(name,res->u.id);

     return res;
}

Term *kernel_name_ef(TermList terms)
{
     Term *res;

     res = MAKE_OBJECT(Term);
     
     res->type = TT_ATOM;
     DECLARE_ID(current_oprs->name,res->u.id);

     return res;
}

Term *get_the_decision_procedures_of_ef(TermList terms)
{
/*
     Term *t_res =MAKE_OBJECT(Term);

     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));
     Intention_List il_res = sl_make_slist();
     Op_Instance *opi;
     Intention *in;

     sl_loop_through_slist(il, in, Intention *) {
	  if ((opi =intention_bottom_op_instance(in)) &&
	      property_p(opi,decision_procedure_sym))
	      sl_add_to_tail(il_res,in);
     }
     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(il_res, TT_INTENTION);

     FREE_SLIST(il_res);

     return t_res;
 */
     Term *t1, *res =MAKE_OBJECT(Term);

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     res->type = LISP_LIST;
     res->u.l_list = satisfy_prop_l_list_opi(t1->u.l_list, decision_procedure_sym);
     
     return res;


}

Term *get_intended_decision_procedures_ef(TermList tl)
{
     Term *t_res =MAKE_OBJECT(Term);
     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));
     Intention_List il_res = sl_make_slist();
     Op_Instance *opi;
     Intention *in;

     sl_loop_through_slist(il, in, Intention *) {
	  if ((opi =intention_bottom_op_instance(in)) &&
	      property_p(opi,decision_procedure_sym) &&
	      opi->intended)
	       sl_add_to_tail(il_res,in);
     }
     t_res->type = LISP_LIST;
     t_res->u.l_list = make_l_list_from_c_list_type(il_res, TT_INTENTION);

     FREE_SLIST(il_res);

     return t_res;
}

#ifndef HAS_TIMES

Term *uclock_t_ef(TermList terms)
{
     static int called = FALSE;
     Term *res;

     if (! called) {
	  fprintf(stderr, LG_STR("WARNING: for your information, uclock_t_ef returns 0 under this OS.\n",
				 "ATTENTION: pour votre information, uclock_t_ef return 0 sous cet OS.\n"));
	  called = TRUE;
     }

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = 0;

     return res;
}

Term *sclock_t_ef(TermList terms)
{
     static PBoolean called = FALSE;
     Term *res;

     if (! called) {
	  fprintf(stderr, LG_STR("WARNING: for your information, sclock_t_ef returns 0 under this OS.\n",
				 "ATTENTION: pour votre information, sclock_t_ef return 0 sous cet OS.\n"));
	  called = TRUE;
     }

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = 0;

     return res;
}

Term *usclock_t_ef(TermList terms)
{
     static PBoolean called = FALSE;
     Term *res;

     if (! called) {
	  fprintf(stderr, LG_STR("WARNING: for your information, usclock_t_ef returns 0 under this OS.\n",
				 "ATTENTION: pour votre information, usclock_t_ef return 0 sous cet OS.\n"));
	  called = TRUE;
     }

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = 0;

     return res;
}

#else

#include <sys/times.h>
#include <limits.h>

Term *uclock_t_ef(TermList terms)
{
     Term *res;
     struct tms buffer;

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = -1;
 
     if (times(&buffer) == -1) 
	  perror("uclock_t_ef: times");
     else
	  res->u.intval = buffer.tms_utime;

     return res;
}

Term *sclock_t_ef(TermList terms)
{
     Term *res;
     struct tms buffer;

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = -1;

     if (times(&buffer) == -1) 
	  perror("sclock_t_ef: times");
     else
	  res->u.intval = buffer.tms_stime;

     return res;
}

Term *usclock_t_ef(TermList terms)
{
     Term *res;
     struct tms buffer;

     res = MAKE_OBJECT(Term);
     res->type = INTEGER;
     res->u.intval = -1;

     if (times(&buffer) == -1) 
	  perror("usclock_t_ef: times");
     else
	  res->u.intval = buffer.tms_stime + buffer.tms_utime;

     return res;
}
#endif

#ifdef VXWORKS
unsigned long get_mtime ()
{
     unsigned long msec;
     struct timespec tp;

     if (clock_gettime(CLOCK_REALTIME, &tp) != 0)
	  perror("clock_gettime");

     msec = (tp.tv_nsec / 1000000) + (tp.tv_sec * 1000);
     return(msec);
}
#elif defined(WIN95)
unsigned long get_mtime ()
{
	  unsigned long msec;
	  struct time tm;

	  gettime(&tm);

	  msec = (tm.ti_hund * 10) + ((tm.ti_sec + (tm.ti_min  + (tm.ti_hour * 60))* 60) * 1000);
	  
	  return(msec);
}
#else
unsigned long get_mtime ()
{
	  unsigned long msec;
	  struct timeval real_time;
	  struct timezone tzp;

	  if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");

	  msec = (real_time.tv_usec / 1000) + (real_time.tv_sec * 1000);
	  
	  return(msec);
}
#endif

Term *mtime_ef(TermList terms)
{
	  Term *res;

	  res = MAKE_OBJECT(Term);

	  res->type = INTEGER;
	  res->u.intval = (unsigned int)get_mtime();

	  return res;
}

#ifdef VXWORKS
long get_time ()
{
	  long sec;
	  struct timespec tp;

	  if (clock_gettime(CLOCK_REALTIME, &tp) != 0)
	  perror("clock_gettime");

	  sec = tp.tv_sec;
	  
	  return(sec);
}
#elif defined(WIN95)
unsigned long get_time ()
{
	  int sec;
	  struct time tm;

	  gettime(&tm);

	  sec = tm.ti_sec + ((tm.ti_min  + (tm.ti_hour * 60))* 60);
	  
	  return(sec);
}
#else
long get_time ()
{
     long sec;
     struct timeval real_time;
     struct timezone tzp;

     if (gettimeofday(&real_time,&tzp) != 0)
	  perror("gettimeofday");

	
     sec = real_time.tv_sec;
     
     return(sec);
}
#endif

Term *time_ef(TermList terms)
{
     Term *res;

     res = MAKE_OBJECT(Term);

     res->type = INTEGER;
     res->u.intval = (int)get_time();

     return res;
}

Term *float_to_int_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     res->type = INTEGER;

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type -= INTEGER) {	/* to make life easier... */
	  res->u.intval = t1->u.intval;
	  return res;
     }

     if (t1->type != TT_FLOAT) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_TERM_TYPE));
     }

     res->u.intval = (int)*t1->u.doubleptr;

     return res;
}

Term *int_to_float_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     res->type = TT_FLOAT;

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type == TT_FLOAT) {
	  res->u.doubleptr = make_doubleptr(*t1->u.doubleptr);
	  return res;
     }
     if (t1->type == LONG_LONG) {
	  res->u.doubleptr = make_doubleptr((double)t1->u.llintval);
	  return res;
     }

     if (t1->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     }

     res->u.doubleptr = make_doubleptr((double)t1->u.intval);

     return res;
}

Term *int_to_llint_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     }

     res->type = LONG_LONG;
     res->u.llintval = (long long int)t1->u.intval;

     return res;
}

Term *float_to_llint_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type != TT_FLOAT) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_TERM_TYPE));
     }

     res->type = LONG_LONG;
     res->u.llintval = (long long int)*t1->u.doubleptr;

     return res;
}

Term *llint_to_float_ef(TermList terms)
{
     Term *res, *t1;

     res = MAKE_OBJECT(Term);

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     if (t1->type != LONG_LONG) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LONG_LONG_TERM_TYPE));
     }

     res->type = TT_FLOAT;
     res->u.doubleptr = make_doubleptr((double)t1->u.llintval);

     return res;
}

Term *evaluate_term_function(Eval_Funct *ef, Symbol ef_name, TermList tl)
/* This is the main function to evaluate a composed term with an evaluable function */
{
     Term *res;
#ifdef OPRS_PROFILING     
     PDate tp;
#endif

     current_error_type = EVAL_FUNCTION;
     current_eval_funct = ef_name;

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EF])
	  GET_PROFILE_TIMER(&tp);
#endif

     res = (Term *)(* (ef->function))(tl);

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_EF]) {
	  ADD_PROFILE_TIMER(&tp,ef->time_active);
	  ef->nb_call++;
     }
#endif
     
     current_error_type = ET_NONE;

     return res;
}

Term *build_expr_from_atom_llisp_ef(TermList terms)
{
     Term *t1, *t2;
     TermList tl;
     Term *lcar;
     L_List list1;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     }
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type != LISP_LIST) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     }
     list1 = t2->u.l_list;

     tl = sl_make_slist();
     while (list1 != l_nil) {
	  lcar = l_car(list1);
	  tl = build_term_list(tl, (Term *)dup_term(get_term_from_l_car(lcar)));
	  list1 = l_cdr(list1);
     }
     
     return build_term_expr(build_expr_pfr_terms(find_or_create_pred_func(t1->u.id), tl));
}


Term * build_term_llisp_from_expr_ef(TermList terms)
{
     Term *t1;
     L_List ll;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != EXPRESSION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     }

     ll = make_l_list_from_c_list(t1->u.expr->terms);
     ll = cons(build_id(t1->u.expr->pfr->name),ll);
     return build_l_list(ll);
}

Term *build_execution_call_for_ipp_plan(TermList terms)
{
    Term *t, *res;
    char *plan;

     res = MAKE_OBJECT(Term);
     
     res->type = STRING;

    t = (Term *)sl_get_slist_pos(terms, 1);
    if (t->type != STRING) {
	fprintf(stderr, LG_STR("ERROR: IPP plan should be of type STRING.\n",
				 "ERREUR: Le plan retourne par IPP devrait etre de type STRING.\n"));
	  return build_nil();
    }

    if ((plan = strchr(t->u.string, '!'))) {
/* 	return build_gtexpr_from_texpr(build_texpr(ACHIEVE, */
/* 						   (build_gexpr_from_expr */
/* 						    (build_expr_cached_pred(pred, */
/* 									    (TermList) sl_make_slist(), */
/* 									    TRUE))))); */
	plan++; plan++; /* skip '!' and blank */
	res->u.string = (char *) MALLOC (50 * sizeof(char));
	strncpy(res->u.string, plan, 50); /* 50 chars should be enough */
	plan = strchr(res->u.string, ')');
	plan++;
	*plan = '\0';
	return res;
    } else {
	fprintf(stderr, LG_STR("ERROR: Could not find invocation part in IPP plan.\n",
				 "ERREUR: Pas de partie d'invocation trouvee dans le plan retourne par IPP.\n"));
	return build_nil();
    }
	
    
}

Term *make_goal_from_expr_ef(TermList terms)
{
     Term *t1;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != EXPRESSION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     }
     return build_goal_term(make_goal_from_expr(dup_expr(t1->u.expr),
						empty_env));
}

Term *find_applicable_ops_for_goal_ef(TermList terms)
{
     Term *t1, *res;
     Op_Instance_List opl;
     Op_Instance *opi;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_GOAL)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_GOAL_TERM_TYPE));
     
     opl = find_soak_goal(t1->u.goal, current_oprs, FALSE);

     res = build_l_list(make_l_list_from_c_list_type(opl, TT_OP_INSTANCE));
     

     SAFE_SL_LOOP_THROUGH_SLIST(opl, opi, Op_Instance *)
	  free_op_instance(opi);
     SAFE_SL_FREE_SLIST(opl);

     return res;
}
Term *arctan(TermList terms)
{
  double valt1, valt2;
  Term *t1,*t2;
  
  t1 = (Term *)get_list_pos(terms, 1);
  t2 = (Term *)get_list_pos(terms, 2);
  
  switch (t1->type) {
  case INTEGER:
       valt1 = t1->u.intval;
       break;
  case LONG_LONG:
       valt1 = t1->u.llintval;
       break;
  case TT_FLOAT:
       valt1 = *t1->u.doubleptr;
       break;
  default: 	 
       report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
       return NULL;
  }
  
  switch (t2->type) {
  case INTEGER:
       valt2 = t2->u.intval;
       break;
  case LONG_LONG:
       valt2 = t2->u.llintval;
       break;
  case TT_FLOAT:
       valt2 = *t2->u.doubleptr;
       break;
  default: 	 
    report_fatal_external_error(oprs_strerror(PE_UNEXPECTED_TERM_TYPE)); 
    return NULL;
  }
  
  return build_float(atan2(valt1, valt2)); 
}

Term *sprintf_ef(TermList terms)
/* Print a list term. */
{
  //Term *t;
  PString fmt_str, fmt_str2;
  Term *term;
  //Expression *tc;
  PBoolean save_pb;
  static Sprinter *sp = NULL;

  if (sp) reset_sprinter(sp);
  else sp = make_sprinter(0);

  term = find_binding((Term *)sl_get_slist_next(terms, NULL));
  if (term->type != STRING)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
  fmt_str = term->u.string;

  for (fmt_str2 = fmt_str; *fmt_str2 ; fmt_str2++) {
    if (*fmt_str2 != '%') {
      SPRINT(sp, 1,sprintf(f,"%c",*fmt_str2));
    } else {
      switch (*++fmt_str2) {
      case 'g':
      case 'd':
      case 'f':
      case 's':
        if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
          report_fatal_external_error
            (LG_STR("Directive and no term left to print in sprintf.",
                    "Des directives subsistent mais plus de termes à imprimer dans la fonction sprintf."));
        save_pb = print_backslash;
        print_backslash = FALSE;
        //print_unsigned = (*fmt_str2 == 'u') ? TRUE : FALSE;
        sprint_term(sp,find_binding(term));
        print_backslash = save_pb;
        break;
      case 't':
        if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
          report_fatal_external_error
            (LG_STR("Directive and no term left to print in sprintf.",
                    "Des directives subsistent mais plus de termes à imprimer dans la fonction sprintf."));
        save_pb = print_backslash;
        print_backslash = TRUE;
        sprint_term(sp,find_binding(term));
        print_backslash = save_pb;
        break;
      case '%':
        SPRINT(sp, 1,sprintf(f,"%c",'%'));
        break;
      default:
        fprintf(stderr,
                LG_STR("Unknown directive %%%c in sprintf.\n",
                       "Directive %%%c inconnue dans la fonction sprintf.\n"),
                *fmt_str2);
        break;
      }
    }
  }

  if ((term = (Term *)sl_get_slist_next(terms, term)) != 0)
    report_recoverable_external_error
      (LG_STR("term(s) left to print in sprintf.",
              "Des term(s) restent à imprimer dans la fonction sprintf."));

  return build_string(SPRINTER_STRING(sp));
}


void declare_ev_funct(void)
/* Create the evaluable function hash table. (which is in the oprs structure. */
{
     make_and_declare_eval_funct("+",ef_plus, -1);
     make_and_declare_eval_funct("-",ef_moins, -1);
     make_and_declare_eval_funct("*",ef_mult, -1);
     make_and_declare_eval_funct("/",ef_div, -1);
     make_and_declare_eval_funct("ABS",ef_abs, 1);
     make_and_declare_eval_funct("SIN",ef_sin, 1);
     make_and_declare_eval_funct("ASIN",ef_asin, 1);
     make_and_declare_eval_funct("COS",ef_cos, 1);
     make_and_declare_eval_funct("ACOS",ef_acos, 1);
     make_and_declare_eval_funct("TAN",ef_tan, 1);
     make_and_declare_eval_funct("ATAN",ef_atan, 1);
     make_and_declare_eval_funct("MOD",ef_mod, 2);
     make_and_declare_eval_funct("RAND",ef_rand, 1);
     
     make_and_declare_eval_funct("ARCTAN",arctan, 2);



     make_and_declare_eval_funct("INT-TO-FLOAT",int_to_float_ef, 1);
     make_and_declare_eval_funct("FLOAT-TO-INT",float_to_int_ef, 1);

     make_and_declare_eval_funct("LLINT-TO-FLOAT",llint_to_float_ef, 1);
     make_and_declare_eval_funct("FLOAT-TO-LLINT",float_to_llint_ef, 1);

     make_and_declare_eval_funct("INT-TO-LLINT",int_to_llint_ef, 1);

     make_and_declare_eval_funct("STRING-CAT",string_cat_ef, 2);

     make_and_declare_eval_funct("TERM-STRING-CAT",term_string_cat_ef, -1);

     make_and_declare_eval_funct("VAL", val_ef, 1);

     make_and_declare_eval_funct("DELETE-FROM-LIST",list_delete_ef, 2);
     make_and_declare_eval_funct("LIST-DIFFERENCE",list_difference_ef, 2);
     make_and_declare_eval_funct("LIST-INTERSECTION",list_intersection_ef, 2);
     make_and_declare_eval_funct("LIST-UNION",list_union_ef, 2);
     make_and_declare_eval_funct("LIST-APPEND",list_append_ef, 2);
     make_and_declare_eval_funct("LIST-DIFFERENCE-ORDER",
					list_difference_order_ef, 2);
     make_and_declare_eval_funct("FACT-INVOKED-OPS-OF",fact_invoked_ops_of_ef, 1);
     make_and_declare_eval_funct("OP-INSTANCE-GOAL",op_instance_goal_ef, 1);
     make_and_declare_eval_funct("SAFETY-HANDLERS-OF",safety_handlers_of_ef, 1);
     make_and_declare_eval_funct("PREFERRED-OF",preferred_of_ef, 1);

     make_and_declare_eval_funct("LENGTH",l_length_ef, 1);
     make_and_declare_eval_funct("SELECT-RANDOMLY",l_select_randomly_ef, 1);
     make_and_declare_eval_funct("CONS-TERM",l_cons_term_ef, 2);
     make_and_declare_eval_funct("CONS",l_cons_ef, 2);
     make_and_declare_eval_funct("CONS-TAIL",l_cons_tail_ef, 2);
     make_and_declare_eval_funct("FIRST",l_car_ef, 1);


     make_and_declare_eval_funct("MAKE-INT-ARRAY",
			     make_int_array_ef, 1);
     make_and_declare_eval_funct("MAKE-FLOAT-ARRAY",
			     make_float_array_ef, 1);
     make_and_declare_eval_funct("GET-INT-ARRAY",
			     get_int_array_ef, 2);
     make_and_declare_eval_funct("GET-FLOAT-ARRAY",
			     get_float_array_ef, 2);
     make_and_declare_eval_funct("GET-INT-ARRAY-SIZE",
			     get_int_array_size_ef, 1);
     make_and_declare_eval_funct("GET-FLOAT-ARRAY-SIZE",
			     get_float_array_size_ef, 1);

     make_and_declare_eval_funct("MAKE-WINDOW",
				 make_window_ef, 0);

     make_and_declare_eval_funct("CAR",l_car_ef, 1);
     make_and_declare_eval_funct("CDR",l_cdr_ef, 1);
     make_and_declare_eval_funct("CAAR",l_caar_ef, 1);
     make_and_declare_eval_funct("CADR",l_cadr_ef, 1);
     make_and_declare_eval_funct("CDAR",l_cdar_ef, 1);
     make_and_declare_eval_funct("CDDR",l_cddr_ef, 1);
     make_and_declare_eval_funct("CAAAR",l_caaar_ef, 1);
     make_and_declare_eval_funct("CAADR",l_caadr_ef, 1);
     make_and_declare_eval_funct("CADAR",l_cadar_ef, 1);
     make_and_declare_eval_funct("CADDR",l_caddr_ef, 1);
     make_and_declare_eval_funct("CDAAR",l_cdaar_ef, 1);
     make_and_declare_eval_funct("CDADR",l_cdadr_ef, 1);
     make_and_declare_eval_funct("CDDAR",l_cddar_ef, 1);
     make_and_declare_eval_funct("CDDDR",l_cdddr_ef, 1);
     make_and_declare_eval_funct("SECOND",l_second_ef, 1);
     make_and_declare_eval_funct("NTH",l_nth_ef, 2);
     make_and_declare_eval_funct("REVERSE",l_reverse_ef, 1);
     make_and_declare_eval_funct("SORT-ALPHA",l_sort_ef, 1);
     make_and_declare_eval_funct("LAST",l_last_ef, 1);

     make_and_declare_eval_funct("ELTS-OF-TYPE", elts_of_type_ef, 1);
     make_and_declare_eval_funct("SURTYPES-OF-TYPE", surtypes_of_type_ef, 1);
     make_and_declare_eval_funct("TYPE-OF", type_of_ef, 1);

     make_and_declare_eval_funct("MAKE-ATOM",make_atom_ef, 1);
     make_and_declare_eval_funct("GENSYM",gensym_ef, 0);
     make_and_declare_eval_funct("FF-VAL",ff_val_ef, 2);
     make_and_declare_eval_funct("MENTION",mention_ef, 1);
     make_and_declare_eval_funct("ALL",all_ef, 2);
     make_and_declare_eval_funct("ALL-POS",all_pos_ef, 2);
     make_and_declare_eval_funct("N-ALL",n_all_ef, 2);
     make_and_declare_eval_funct("N-ALL-LIST",n_all_list_ef, 2);
     make_and_declare_eval_funct("L-LIST",make_l_list_ef, -1);

     make_and_declare_eval_funct("TERM-CAR",term_car_ef, 1);
     make_and_declare_eval_funct("LISP-CAR-TO-TERM",lisp_car_to_term_ef, 1);
     make_and_declare_eval_funct("TERM-TO-LISP-CAR",term_to_lisp_car_ef, 1);

     make_and_declare_eval_funct("APPLY-SUBST-IN-EXPR", apply_subst_in_expr_ef, 3);
     make_and_declare_eval_funct("APPLY-SUBST-IN-GOAL", apply_subst_in_goal_ef, 3);
     make_and_declare_eval_funct("BUILD-GOAL", build_goal_ef, 1);

     make_and_declare_eval_funct("GET-CURRENT-INTENTION",
					current_intention_ef, 0);
     make_and_declare_eval_funct("GET-ALL-INTENTIONS",
					list_all_intentions_ef, 0);
     make_and_declare_eval_funct("GET-OTHER-INTENTIONS",
					list_other_intentions_ef, 0);
     make_and_declare_eval_funct("GET-SLEEPING-INTENTIONS",
					list_sleeping_intentions_ef, 0);
     make_and_declare_eval_funct("GET-ROOT-INTENTIONS",
					list_root_intentions_ef, 0);

     make_and_declare_eval_funct("NUMBER-OF-INTENTIONS",number_of_intentions_ef, 0);
     make_and_declare_eval_funct("GET-THE-DECISION-PROCEDURES-OF",
					get_the_decision_procedures_of_ef, 1); 
     make_and_declare_eval_funct("GET-INTENDED-DECISION-PROCEDURES",
					get_intended_decision_procedures_ef, 0); 

     make_and_declare_eval_funct("FIND-INTENTIONS-ID",
					find_intentions_id_ef, 1); 
     make_and_declare_eval_funct("FIND-INTENTION-ID",
					find_intention_id_ef, 1); 

     make_and_declare_eval_funct("GET-INTENTION-PRIORITY",
					get_intention_priority_ef, 1);
     make_and_declare_eval_funct("GET-CURRENT-INTENTION-ID",
					current_intention_id_ef, 0);
     make_and_declare_eval_funct("GET-INTENTION-ID",
					get_intention_id_ef, 1);
     make_and_declare_eval_funct("GET-INTENTION-TIME",
					get_intention_time_ef, 1);

     make_and_declare_eval_funct("USER-SYS-CLOCK-TICK",usclock_t_ef, 0);
     make_and_declare_eval_funct("USER-CLOCK-TICK",uclock_t_ef, 0);


     make_and_declare_eval_funct("GOAL-STATEMENT",goal_statement_ef, 1);
     make_and_declare_eval_funct("FACT-STATEMENT",fact_statement_ef, 1);
     make_and_declare_eval_funct("GSTATEMENT-PREDICAT",goal_statement_predicate_ef, 1);
     make_and_declare_eval_funct("FSTATEMENT-PREDICAT",fact_statement_predicate_ef, 1);
     make_and_declare_eval_funct("GSTATEMENT-ARG",goal_statement_arg_ef, 2);
     make_and_declare_eval_funct("FSTATEMENT-ARG",fact_statement_arg_ef, 2);

     make_and_declare_eval_funct("GOAL-STATEMENT-COPY",goal_statement_cp_ef, 1);
     make_and_declare_eval_funct("FACT-STATEMENT-COPY",fact_statement_cp_ef, 1);
     make_and_declare_eval_funct("GSTATEMENT-ARG-COPY",goal_statement_arg_cp_ef, 2);
     make_and_declare_eval_funct("FSTATEMENT-ARG-COPY",fact_statement_arg_cp_ef, 2);
     make_and_declare_eval_funct("COPY-TERM", copy_term_ef, 1);
     make_and_declare_eval_funct("COPY-TERM-LITERAL-VAR", copy_term_keeping_variable_unique_ef, 1);

     make_and_declare_eval_funct("TERM-COMP-FUNCTION", tc_function_ef, 1);
     make_and_declare_eval_funct("TERM-COMP-ARG-COPY", tc_arg_cp_ef, 2);
     make_and_declare_eval_funct("TERM-COMP-ARG", tc_arg_ef, 2);

     make_and_declare_eval_funct("MTIME",mtime_ef, 0);
     make_and_declare_eval_funct("TIME",time_ef, 0);
     make_and_declare_eval_funct("PROPERTY-OF",property_of_ef, 2);


     make_and_declare_eval_funct("KERNEL-NAME",kernel_name_ef, 0);

     make_and_declare_eval_funct("BUILD-TERM-COMP-FROM-ATOM-LLIST", build_expr_from_atom_llisp_ef, 2);
     make_and_declare_eval_funct("BUILD-LLISP-FROM-TERM-COMP", build_term_llisp_from_expr_ef, 1);

     make_and_declare_eval_funct("BUILD-EXECUTION-CALL-FOR-IPP-PLAN", build_execution_call_for_ipp_plan, 1);

     make_and_declare_eval_funct("MAKE-GOAL-FROM-EXPR", make_goal_from_expr_ef, 1);
     make_and_declare_eval_funct("FIND-APPLICABLE-OPS-FOR-GOAL", find_applicable_ops_for_goal_ef, 1);
     make_and_declare_action("TEST-FOR-LTDL", find_applicable_ops_for_goal_ef, 1);

     make_and_declare_eval_funct("SPRINTF", sprintf_ef, -1);

     /* declare_user_eval_funct(); */
     /* declare_shary_user_eval_funct(); /\* why is this here???? (FFI) *\/ */

}
