/*                               -*- Mode: C -*- 
 * slistPackTest.c -- 
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

#include "stdio.h"
#include "slistPack.h"
#include "slistPack_f.h"

int int_equal(int i1, int i2)
{
     return i1 == i2;
}

print_them(Slist *l1)
{
     int i;

     printf("liste ");
     sl_loop_through_slist(l1,i,int)
	  printf("%d ", i);
     printf("\n");
}

main()
{
     Slist *l1, *l2, *l3;
     int i;

     l1 = sl_make_slist();
     l2 = sl_make_slist();

     print_them(l1);
     print_them(l2);

     for(i = 0; i<100; i++) {
	  sl_add_to_head(l1,i);
	  sl_add_to_tail(l2,i);
     }
     print_them(l1);
     print_them(l2);

     l1 = sl_concat_slist(l1,l2);
     l3 = sl_copy_slist(l1);

     print_them(l1);
     print_them(l3);

     sl_flush_slist(l1);

     print_them(l1);

     sl_free_slist(l1);
     sl_free_slist(l3);

     l1 = sl_make_slist();
     l2 = sl_make_slist();

     for(i = 1; i<100; i++) {
	  sl_add_to_head(l1,i);
     }

     sl_concat_slist(l2,l1);
     print_them(l2);

     l1 = sl_make_slist();

     sl_concat_slist(l2,l1);

     print_them(l2);

     l1 = sl_make_slist();

     for(i = 1; i<100; i += 2) {
	  sl_delete_slist_node(l2,i);
	  sl_delete_slist_node(l1,i);
     }
     print_them(l1);
     print_them(l2);

     if (sl_in_slist(l2, 50))
	  print_them(l1);

     if (sl_search_slist(l2, 50, int_equal))
	  print_them(l2);

     if (sl_search_slist(l2, 150, int_equal))
	  print_them(l2);
     
     printf("head %d\n", sl_get_slist_head(l2));
     
     printf("head %d\n", sl_get_slist_tail(l2));
     
     printf("get from head");
     while (i = sl_get_from_head(l2))
	  printf("%d ", i);
     printf("\n");

     for(i = 1; i<100; i++) {
	  sl_add_to_tail(l1,i);
     }
     
     printf("get from tail");
     while (i = sl_get_from_tail(l1))
	  printf("%d ", i);
     printf("\n");

     print_them(l1);
     print_them(l2);
     
     for(i = 1; i<100; i++) {
	  sl_add_to_head(l1,i);
     }
 
     i = 0;
     while (i = sl_get_slist_next(l1,i))
	  printf("%d ", i);
     printf("\n");

     i= 100;
     while (i = sl_get_slist_next(l1,i))
	  printf("%d ", i);
     printf("\n");

     i= 50;
     while (i = sl_get_slist_next(l1,i))
	  printf("%d ", i);
     printf("\n");

     print_them(l1);
     print_them(l2);

     sl_free_slist(l1);
     sl_free_slist(l2);

}
