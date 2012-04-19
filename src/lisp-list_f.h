/*                               -*- Mode: C -*- 
 * lisp-list_f.h -- 
 * 
 * $Id$
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

#include "lisp-list_f-pub.h"

L_List cons(Term *car, L_List cdr);
L_List make_l_list_from_c_list(Slist *l);
L_List make_l_list_from_c_list_type(Slist *list, Term_Type type);
Term *select_randomly_l_list(L_List l);
void mapcar(L_List l, PFV funct);
L_List l_list_union(L_List l1, L_List l2);
L_List l_list_intersection(L_List l1, L_List l2);
L_List l_list_difference(L_List l1, L_List l2);
L_List l_list_difference_order(L_List l1, L_List l2);
L_List l_list_append(L_List l1, L_List l2);
L_List dup_l_list(L_List l);
void free_l_list(L_List l);
L_List l_reverse(L_List l);
Slist *make_c_list_from_l_list (L_List l);
PBoolean memq(Term *elt, L_List l);
L_List l_sort(L_List l);
L_List transform_c_list_to_l_list(Slist *l);

#define L_SIZE(l) ((l) == l_nil ? 0 : l->size)
#define CAR(l) ((l) == l_nil ? (Term *)l_nil : (l)->car)
#define CAR_DUP(l) ((l) == l_nil ? (Term *)l_nil : dup_term((l)->car))
#define CDR(l) ((l) == l_nil ? l_nil : (l)->cdr)
#define CDR_DUP(l) ((l) == l_nil ? l_nil : dup_l_list((l)->cdr))
#define L_LENGTH(l) (L_SIZE(l))

PBoolean equal_l_car(Term *car1, Term *car2);
PBoolean equal_l_list(L_List l1, L_List l2);
L_List l_delete(Term *car, L_List l1);

L_List l_add_to_tail(L_List list, Term *car);
Term *l_last(L_List l);
