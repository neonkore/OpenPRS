static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-ll.c -- Lisp list functions required by the kernels
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
#include "lisp-list.h"
#include "oprs-type.h"
#include "oprs-print.h"

#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "unification_f.h"
#include "lisp-list_f.h"

PBoolean equal_l_list(L_List l1, L_List l2)
{
     if (L_LENGTH(l1) != L_LENGTH(l2))
	  return (FALSE);

     if (l1 == l_nil) return TRUE; /* They are the same length.. therefore both nil. */

     while (l1 != l_nil) {
	  if (equal_term(CAR(l1),CAR(l2))) {
	       l1 = CDR(l1);
	       l2 = CDR(l2);
	  } else return FALSE;
     }
     return TRUE;	  
}
  
PBoolean memq(Term *elt, L_List l)
{
     if (l == l_nil) return FALSE;
     else if (equal_term(elt, CAR(l))) return TRUE;
     else return(memq(elt, CDR(l)));
}

L_List l_list_difference(L_List l1, L_List l2)
{
     L_List res = l_nil;

     if (l2 == l_nil) return dup_l_list(l1);

     while (l1 != l_nil) {
	  Term *car = CAR(l1);

	  if (! (memq(car, l2))) res = cons(dup_term(car), res);
	  l1 = CDR(l1);
     }
     
     return res;	  
}

L_List l_list_intersection(L_List l1, L_List l2)
{
     L_List res = l_nil;

     if (l2 == l_nil || l1 == l_nil) return res;

     while (l1 != l_nil) {
	  Term *car = CAR(l1);
	  l1 = CDR(l1);
	  
	  if ((memq(car, l2)) && !(memq(car, l1))) res = cons(dup_term(car), res);
     }
     
     return res;	  
}

L_List l_list_union(L_List l1, L_List l2)
{
     L_List res = l_nil;

     while (l1 != l_nil) {
	  Term *car = CAR(l1);

	  if (! (memq(car,res))) res = cons(dup_term(car), res);
	  l1 = CDR(l1);
     }

     while (l2 != l_nil) {
	  Term *car = CAR(l2);

	  if (! (memq(car,res))) res = cons(dup_term(car), res);
	  l2 = CDR(l2);
     }
     
     return res;	  
}

L_List l_delete(Term *car_to_del, L_List l1)
{
     L_List res = l_nil;

     while (l1 != l_nil) {
	  Term *car = CAR(l1);

	  if (! (equal_term(car, car_to_del)))
	       res = cons(dup_term(car), res);
	  l1 = CDR(l1);
     }
     
     return res;	  
}

L_List l_list_difference_order(L_List l1, L_List l2)
{
     L_List res = l_nil;

     if (l2 == l_nil) return dup_l_list(l1);

     while (l1 != l_nil) {
	  Term *car = CAR(l1);

	  if (! (memq(car, l2))) {
	       res = l_add_to_tail(res, car);
	  }
	  l1 = CDR(l1);
     }
     
     return res;	  
}

L_List l_list_append(L_List l1, L_List l2)
{
     L_List tmp, res;

     if (l1 == l_nil) return dup_l_list(l2);

     res =  dup_l_list(l2);
     tmp =  l_reverse(l1);
	  
     while (tmp != l_nil) {
	  Term *car = CAR(tmp);

	  res = cons(dup_term(car), res);

	  tmp = CDR(tmp);
     }
     
     free_l_list(tmp);

     return res;	  
}

