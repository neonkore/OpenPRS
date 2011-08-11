/*                               -*- Mode: C -*- 
 * user-ev-function.c -- contains user defined evaluable functions.
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


#include "config.h"

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "oprs-type-pub.h"
#include "user-ev-function.h"

#include "oprs-type_f-pub.h"
#include "ev-function_f-pub.h"
#include "lisp-list_f-pub.h"
#include "oprs-rerror_f-pub.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-rerror_f.h"
#include "oprs-error_f.h"
Term *toto_eval_func(TermList terms)
{
     Term *t1,*res;

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     res->type = INTEGER;
     res->u.intval = 3;
     
     return res;
}

Term *test_u_mem_eval_func(TermList terms)
{
     return build_memory("1234567890",11);
}

Term *example1_l_list_eval_func(TermList terms)
{
     int num;
     Term *t1, *t2, *t, *res;
     Term *lcar;
     L_List list1, new_list = l_nil;
     PBoolean error = FALSE;

     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != LISP_LIST) {
	  fprintf(stderr,"Expecting a LISP_LIST as 1st argument in example1_l_list_eval_func\n");
	  error = TRUE;
     }
     if (t2->type != INTEGER) {
	  fprintf(stderr,"Expecting a INTEGER as 2nd argument in example1_l_list_eval_func\n");
	  error = TRUE;
     }
     if (error) {
	  res->type = LISP_LIST;
	  res->u.l_list = l_nil;
          return res;
     }
     list1 = t1->u.l_list;
     num = t2->u.intval;

     while (list1 != l_nil) {
	  lcar = l_car(list1);
	  t = get_term_from_l_car(lcar);
	  if (t->type != LISP_LIST) {
	       fprintf(stderr,"Expecting a LISP_LIST in the term\n");
	       error = TRUE;
	       break;
	  }

	  if (num > l_length(t->u.l_list)){
	       fprintf(stderr,"Not that number of element in the list \n");
	       error = TRUE;
	       break;
	  }
	  lcar = l_nth (t->u.l_list, num);
	  new_list = l_add_to_tail(new_list, lcar);

	  list1 = l_cdr(list1);
     }
     if (error) {
	  res->type = LISP_LIST;
	  res->u.l_list = l_nil;
          return res;
     }
     res->type = LISP_LIST;
     res->u.l_list = new_list;
     
     return res;
}

/*
 This function does the same that the CONS evaluable function. 
It builds a new list which is the "cons" of the first argument which is 
an undefined term, and the second one which is a LISP-LIST term.
Note the copy_l_list, which must be done if we want to use a list given in argument,
 and the FREE of the Term *which is built here, by make_l_car_from_term, the l_cons will 
dup it so we need to free it. In the previous example we build the L_List with already 
existing Term *, so we do NOT free it.
*/

Term *example2_l_list_eval_func(TermList terms)
{
     Term *t1, *t2, *res;
     Term *lcar;
     L_List list, new_list = l_nil;

     res = MAKE_OBJECT(Term);
     res->type = LISP_LIST;
     res->u.l_list = new_list;
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type != LISP_LIST) {
	  fprintf(stderr,"Expecting a LISP_LIST as 2nd argument in example2_l_list_eval_func\n");
	  return res;
     }

     list = t2->u.l_list;

/* Build a l_car from the term */
     lcar = make_l_car_from_term (t1);

/* We want to make a new list using one given in argument we NEED to copy it.*/
     new_list = copy_l_list (list);

     new_list = l_cons(lcar, new_list);
     
     /* The l_cons dup the car, it was created here (make_l_car_from_term),
	so we need to free it. */
     OPRS_FREE(lcar);

     res->u.l_list = new_list;
     
     return res;
}

/*
  This function builds a lisp-list of lisp-list term,
   all the objects we create except the returned term (res) must been free,
   these objects are created by MAKE_OBJECT, or  make_l_car_from_term.
*/
Term *example3_l_list_eval_func(TermList terms)
{
     int i, j;

     Term *t1, *t2, *res;
     Term *lcar;
     L_List new_list1, new_list2;

     res = MAKE_OBJECT(Term);
     new_list1 = l_nil;

     for (i = 1; i <= 3; i++) {

	  new_list2 = l_nil;
	  for (j = 1; j <= 4; j++) {
	       t2 = MAKE_OBJECT(Term);
	       t2->type = INTEGER;
	       t2->u.intval = i * 10 + j;

	       lcar = make_l_car_from_term (t2);
	       /* The  make_l_car_from_term dup the term, so we need to free it */
	       OPRS_FREE(t2);
     
	       new_list2 = l_cons(lcar, new_list2);
	       /* The l_cons dup the car, it was created here (make_l_car_from_term),
		  so we need to free it. */
	       OPRS_FREE(lcar);
	  }
	  t1 = MAKE_OBJECT(Term);
	  t1->type = LISP_LIST;
	  t1->u.l_list = new_list2;
	  lcar = make_l_car_from_term (t1);
	  new_list1 = l_add_to_tail(new_list1, lcar);
	  
	  OPRS_FREE(t1); 
	  OPRS_FREE(lcar);
     }
     res->type = LISP_LIST;
     res->u.l_list = new_list1;
     
     return res;
}



void declare_user_eval_funct()
{
     make_and_declare_eval_funct("TEST_UMEM",test_u_mem_eval_func, 0);
     make_and_declare_eval_funct("DO-NOT-CALL-ME-TOTO",toto_eval_func, 1);
     make_and_declare_eval_funct("EX-LIST-1",example1_l_list_eval_func, 2);
     make_and_declare_eval_funct("EX-LIST-2",example2_l_list_eval_func, 2);
     make_and_declare_eval_funct("EX-LIST-3",example3_l_list_eval_func, 0);

#ifdef TRUCK_DEMO
     make_and_declare_eval_funct("INIT-OPRS-TIME",action_init_oprs_time, 1);
     make_and_declare_eval_funct("TIME",time_eval_funct, 0);
#endif
     return;
}
