/*                               -*- Mode: C -*- 
 * lisp-list.c -- 
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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

#include <stdlib.h>

#include "lisp-list.h"
#include "oprs-type.h"

#include "intention_f.h"
#include "op-instance_f.h"
#include "fact-goal_f.h"
#include "oprs-type_f.h"
#include "lisp-list_f.h"

const L_List l_nil = (L_List)0;

/* 
 * Most of these functions could be written as macros...
 * 
 */


L_List cons(Term *term, L_List cdr)
{
     L_List res = MAKE_OBJECT(struct l_list);
    
     res->car = term;
     res->cdr = cdr;
     res->size = L_SIZE(cdr) + 1;

     return res;
}

Term *l_car(L_List l)
{
     return CAR(l);
}


Term *l_nth(L_List l, int i)
{
     for(; i>1; i--)
	  l=CDR(l);
     return CAR(l);
}

L_List l_cdr(L_List l)
{
     return CDR(l);
}

int l_length(L_List l)
{
     return L_SIZE(l);
}

L_List make_l_list_from_c_list_type(Slist *list, Term_Type type)
{
     L_List res = l_nil;
     Term *elt;
     void *node;
     Slist *l;
     
     if (list) {
	  l = COPY_SLIST(list); 

	  while ((node = sl_get_from_tail(l)) != NULL) {
	       switch (type) {
	       case TT_INTENTION: 	 
		    elt = build_intention_term(dup_intention(node));
		    break;
	       case TT_OP_INSTANCE: 	 
		    elt = build_opi_term(dup_op_instance(node));
		    break;
	       case TT_GOAL: 	 
		    elt = build_goal_term(dup_goal(node));
		    break;
	       case TT_FACT: 	 
		    elt = build_fact_term(dup_fact(node));
		    break;
	       default:
		    fprintf(stderr,LG_STR("WARNING: make_l_list_from_c_list_type: unknown type.\n",
					  "ATTENTION: make_l_list_from_c_list_type: type inconnu.\n"));
		    elt = build_nil();
	       }
	       res = cons(elt,res);
	  }
	  FREE_SLIST(l);
     }    
     return res;
}

L_List make_l_list_from_c_list(Slist *list)
{
     L_List res = l_nil;
     Term *elt;
     Slist *l = COPY_SLIST(list); 

     while ((elt = sl_get_from_tail(l)) != NULL)
	  res = cons(dup_term(elt),res); /* We need to make a dup of each element. */
     FREE_SLIST(l);
     
     return res;
}

L_List transform_c_list_to_l_list(Slist *l)
/* 
 *  transform_c_list_to_l_list_l_rec - will destroy the c list to 
 *                                     built the lisp one... 
 */
{
     L_List res = l_nil;
     Term *elt;

     while ((elt = sl_get_from_tail(l)) != NULL)
	  res = cons(elt,res);

     FREE_SLIST(l);
     
     return res;
}

Term *transform_term_c_list_to_term_l_list(Term *res)
{
     if (res->type == C_LIST) {
	  res->type = LISP_LIST;
	  res->u.l_list =  transform_c_list_to_l_list(res->u.c_list);
     }
     return res;
}

Slist *make_c_list_from_l_list (L_List l)
{
     Slist *res = sl_make_slist();

     while (l != l_nil) {
	  sl_add_to_tail(res,CAR(l));
	  l = CDR(l);
     }
     return res;	  
}

Slist *make_c_list_from_term_l_list (L_List l)
{
     Slist *res = sl_make_slist();

     while (l != l_nil) {
	  sl_add_to_tail(res, dup_term(CAR(l)));
	  l = CDR(l);
     }
     return res;	  
}

PBoolean expr_sort_term(Term *e1, Term *e2)
{
     return (compare_term(e1,e2) <= 0);
}

L_List l_sort(L_List l)
{
     Slist *c_l = make_c_list_from_term_l_list(l);

     sl_sort_slist_func(c_l,expr_sort_term);

     return transform_c_list_to_l_list(c_l);
}

Term *select_randomly_l_list(L_List l)
{
     int len = L_LENGTH(l);
     long rand_value;

     if (len == 0) fprintf(stderr,LG_STR("WARNING: select_randomly_c_list: empty list.\n",
					 "ATTENTION: select_randomly_c_list: liste vide.\n"));
     if (len == 1) return CAR(l);
     rand_value = RANDOM();
     return l_nth(l, (rand_value % len) + 1);
}

Term *l_last(L_List l)
{
     Term *car = CAR(l);
     while (l != l_nil) {
	  car = CAR(l);
	  l = CDR(l);
     }  
     return dup_term(car);
}

void mapcar(L_List l, PFV funct) /* Not a real mapcar, because it takes one elt only and does not return anything... */
{
     if (l == l_nil) return;
     (funct)(CAR(l));
}

L_List l_reverse(L_List l)
{
     L_List res = l_nil;

     while (l != l_nil) {
	  Term *car = CAR(l);

	  res = cons(dup_term(car), res);

	  l = CDR(l);
     }
     
     return res;	  
}

L_List dup_l_list(L_List l)
{
     if (l == l_nil) return l;

     DUP(l);

     return l;
}


void free_l_list(L_List l)
{
     if (l == l_nil) return;
     
     if (LAST_REF(l)) {
	  free_l_list(CDR(l));
	  free_term(CAR(l));
     }
     FREE(l);
}

L_List copy_l_list (L_List list)
{
     L_List tmp, previous, new_list = l_nil;
     
     tmp = l_nil;		/* Just to avoid gcc warning. */
     previous = l_nil;		/* Just to avoid gcc warning. */
     
     if (list != l_nil) {
	  while (list != l_nil) {
	       tmp = MAKE_OBJECT(struct l_list);
	       
	       tmp->car = copy_term (list->car);
	       tmp->size = L_SIZE(list); 
	       
	       if(new_list == l_nil)
		    new_list = tmp;
	       else
		    previous->cdr = tmp;

	       list = CDR(list);
	       previous = tmp;
	  }
	  tmp->size = 1; /* The last one */
	  tmp->cdr = l_nil;
     }
     return new_list;
}

L_List l_cons(Term *car, L_List cdr)
{
     L_List res = MAKE_OBJECT(struct l_list);

     res->car = dup_term (car);
     res->cdr = cdr;
     res->size = L_SIZE(cdr) + 1;

     return res;
}

L_List l_add_to_tail(L_List list, Term *car)
{
     L_List save_list = list, previous = l_nil;
     L_List res = MAKE_OBJECT(struct l_list);

     res->car = dup_term(car);
     res->size = 1;
     res->cdr = l_nil;

     while(list != l_nil) {
	  previous = list;
	  list->size = L_SIZE(list) + 1;
	  list = CDR(list);
     }
     if (previous == l_nil) 
	  return res;
     
     previous->cdr = res;
     return save_list;
}

Term *make_l_car_from_term(Term *t)
/* Return a LISP_CAR object which is the contains the Term. */
{     
     return dup_term(t);
}

Term *get_term_from_l_car(Term *l)
{
     return l;
}
