static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * shashPack.c -- 
 * 
 * Copyright (c) 1991-2005 Francois Felix Ingrand.
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
#endif

#include "shashPack.h"
#include "shashPack_f.h"

#include "slistPack_f.h"

#include "macro-pub.h"
#include "constant-pub.h"

Shash *_sh_make_hashtable(int size, SL_PFI hashfunc, SL_PFC matchfunc)
{
     int i;
     Shash *res;

     if (size < 0) return NULLShash;

     res =  MAKE_OBJECT(Shash);
     res->lists = OPRS_MALLOC(size * sizeof(Slist *));

     res->size = size;
     res->hashfunc = hashfunc;
     res->matchfunc = matchfunc;
     res->number = 0;

     for (i = size-1; i>= 0; i--) res->lists[i] = NULLSlist;

     return res;
}

void sh_flush_hashtable(Shash *table)
{
     int i;

     for (i = table->size-1; i >= 0; i--)
	  if (table->lists[i]) sl_flush_slist(table->lists[i]);
}

void sh_free_hashtable(Shash *table)
{
     int i;

     for (i = table->size-1; i >= 0; i--)
	  if (table->lists[i]) sl_free_slist(table->lists[i]);
     OPRS_FREE(table->lists);
     FREE_OBJECT(table);
}

void *sh_add_to_hashtable(Shash * table, void * node, void * ident)
{
     int index;
     Slist *list;

     index = (* table->hashfunc)(ident);
    
     if (!(list = table->lists[index]))
	  list = table->lists[index] = sl_make_slist();

     table->number++;
     return sl_add_to_tail(list, node);
}


void *sh_get_from_hashtable(Shash *table, void *ident)
{
     int index;
     void *ptr;
     Slist *list;

     index = (* table -> hashfunc)(ident);

     if ((list = table->lists[index])) {
	  sl_loop_through_slist(list, ptr, void *) 
	       if ((* table->matchfunc)(ident, ptr)) {
		    sl_deprotect_loop(list);	  
		    return ptr;
	       }
     }
     return NULL;
}

void *sh_delete_from_hashtable(Shash *table, void *ident)
{
     int index;
     void *ptr;
     Slist *list;

     index = (* table -> hashfunc)(ident);

     if ((list = table->lists[index])) {
	  sl_loop_through_slist(list, ptr, void*) 
	       if ((* table->matchfunc)(ident, ptr)) {
		    table->number--;
		    sl_deprotect_loop(list);
		    return sl_delete_slist_node(list, ptr);
	       }
     }
     return NULL;
}

int _sh_for_all_hashtable(Shash *table, void *node, SL_PFI func)
{
     int i, sum = 0;
     void *temp;
     Slist *list;

    for (i = table->size; --i >= 0;)
	 if ((list = table->lists[i])) {
	    sl_loop_through_slist(list, temp, void *)
		 sum += (*func)(node, temp);
	 }
     return sum;
}

int _sh_for_all_2hashtable(Shash *table, void *node, void *node2, SL_PFI func)
{
     int i, sum = 0;
     void *temp;
     Slist *list;

    for (i = table->size; --i >= 0;)
	 if ((list = table->lists[i])) {
	    sl_loop_through_slist(list, temp, void *)
		 sum += (*func)(node, node2, temp);
	 }
     return sum;
}

int _sh_for_all_3hashtable(Shash *table, void *node, void *node2, void *node3, SL_PFI func)
{
     int i, sum = 0;
     void *temp;
     Slist *list;

    for (i = table->size; --i >= 0;)
	 if ((list = table->lists[i])) {
	    sl_loop_through_slist(list, temp, void *)
		 sum += (*func)(node, node2, node3, temp);
	 }
     return sum;
}
