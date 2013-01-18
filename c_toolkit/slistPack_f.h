/*                               -*- Mode: C -*- 
 * slistPack_f.h -- 
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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

/* #include "../opaque-pub.h" enzo */

Slist *sl_make_slist(void);
Slist *sl_copy_slist(Slist *slist);
void sl_free_slist(Slist *slist);

int sl_slist_empty(Slist *slist);
int sl_in_slist(Slist *slist, const void *node);
int sl_slist_length(Slist *slist);

const void *sl_get_slist_head(Slist *slist); /* Return only. */
const void *sl_get_slist_tail(Slist *slist); /* Return only. */
const void *sl_get_slist_next(Slist *slist, const void *node); 
const void *sl_get_slist_pos(Slist *slist, int pos); 

const void *_sl_search_slist(Slist *slist, const void *node, SL_PFC func);

Slist *_sl_sort_slist_func(Slist *slist, SL_PFC func);
Slist *_sl_copy_slist_func(Slist *slist, SL_PFPV func);
Slist *_sl_copy_slist_func1(Slist *slist, const void *node, SL_PFPV func);
Slist *_sl_copy_slist_func2(Slist *slist, const void *node, const void *node2, SL_PFPV func);

Slist *decompile_slist(Slist *slist, int lock_it);
void compile_slist(Slist *slist);
Slist_Type sl_slist_type(Slist *slist);

/* These define are just here to cast the func... */
#define sl_search_slist(slist,node,func) _sl_search_slist(slist,node,func)

#define sl_loop_through_dy_slist(slist,nnode,_type) \
for(slist->current = slist->first; \
    nnode = (_type)(slist->current ? slist->current->node : 0), slist->current; \
    slist->current = (slist->current ? slist->current->next : 0))

#define sl_loop_through_st_slist(slist,nnode,_type) \
for(slist->current = slist->first; \
    nnode = (_type)(slist->current ? *(slist->current) : 0), slist->current; \
    slist->current = (slist->current < slist->last ? slist->current + 1 : 0))

#define sl_loop_through_si_slist(slist,nnode,_type) \
for(slist->current = slist->first; \
    nnode = (_type)(slist->current); \
    slist->current = 0)

/* cette macro est fausse */
#define sl_loop_through_sb_slist(slist,nnode,_type) \
for(slist->current = slist->first; \
    nnode = (_type)(slist->current); \
    slist->current = (slist->current ? slist->last : 0))

/* this macro is a merging of the 4 macros above... */
/* The dynamic field is used in double list to cope with double list with twice the same element... */


#ifdef CHECK_LOOP_REENTRANCE
#define sl_deprotect_loop(slist) (slist)->loop_used=0
#define sl_check_loop_corruption(slist) if(slist->loop_used) abort()
#define sl_loop_through_slist(slist,nnode,_type) \
for( slist->loop_used = slist->loop_used ? (void *)(abort(),0) : (void *)1 , slist->current.st = slist->first.st , (slist->type==SLT_DOUBLE?slist->dynamic=0:0) ; \
    nnode = (_type)(slist->current.st ? \
		    (slist->type==SLT_STATIC? \
		     *(slist->current.st): \
		     (slist->type==SLT_DYNAMIC? \
		      slist->current.dy->node: \
		      slist->current.sd)): \
		    0), \
    slist->loop_used = slist->current.st, \
    slist->current.st; \
    (slist->type==SLT_SINGLE? \
     (slist->current.sd = 0): \
     (slist->type==SLT_STATIC? \
      (void *)(slist->current.st = (((slist->current.st) < (slist->last.st))? \
				    slist->current.st + 1 : 0)): \
      (slist->type==SLT_DYNAMIC? \
       (void *)(slist->current.dy = (slist->current.dy ? \
				     slist->current.dy->next : 0)): \
       (slist->type==SLT_DOUBLE? \
	(slist->current.sd = (!slist->dynamic ? \
			      slist->dynamic = 1, slist->last.sd : 0)): \
	(slist->current.sd = 0))))))
#else
#define sl_deprotect_loop(slist)
#define sl_check_loop_corruption(slist)
#define sl_loop_through_slist(slist,nnode,_type) \
for(slist->current.st = slist->first.st , (slist->type==SLT_DOUBLE?slist->dynamic=0:0) ; \
    nnode = (_type)(slist->current.st ? \
		    (slist->type==SLT_STATIC? \
		     *(slist->current.st): \
		     (slist->type==SLT_DYNAMIC? \
		      slist->current.dy->node: \
		      slist->current.sd)): \
		    0), \
    slist->current.st; \
    (slist->type==SLT_SINGLE? \
     (slist->current.sd = 0): \
     (slist->type==SLT_STATIC? \
      (void *)(slist->current.st = (((slist->current.st) < (slist->last.st))? \
				    slist->current.st + 1 : 0)): \
      (slist->type==SLT_DYNAMIC? \
       (void *)(slist->current.dy = (slist->current.dy ? \
				     slist->current.dy->next : 0)): \
       (slist->type==SLT_DOUBLE? \
	(slist->current.sd = (!slist->dynamic ? \
			      slist->dynamic = 1, slist->last.sd : 0)): \
	(slist->current.sd = 0))))))

#endif

#define sl_delete_slist_func(slist,node,func) _sl_delete_slist_func(slist,node,func)
#define sl_sort_slist_func(slist,func) _sl_sort_slist_func(slist,func)
#define sl_copy_slist_func(slist,func) _sl_copy_slist_func(slist,(SL_PFPV)(func))
#define sl_copy_slist_func1(slist,node1,func) _sl_copy_slist_func1(slist,node1,(SL_PFPV)(func))
#define sl_copy_slist_func2(slist,node1,node2,func) _sl_copy_slist_func2(slist,node1,node2,(SL_PFPV)(func))
Slist *sl_copy_slist(Slist *_slist);
const void *sl_replace_slist_node(Slist *_slist, const void *_oldnode, const void *_newnode);
Slist *sl_replace_slist_func(Slist *_slist, SL_PFPV _func);
const void *_sl_delete_slist_func(Slist *_slist, const void *_node, SL_PFC _func);
const void *sl_delete_slist_node(Slist *_slist, const void *_node);
const void *sl_add_to_head(Slist *_slist, const void *_snode);
const void **sl_add_to_tail_ret_ref(Slist *slist, const void *node);
const void *sl_add_to_tail(Slist *_slist, const void *_snode);
void sl_flush_slist(Slist *_slist);
const void *sl_get_from_head(Slist *_slist);
const void *sl_get_from_tail(Slist *_slist);
const void *sl_insert_slist_pos(Slist *_slist, const void *_snode, int _pos);
Slist *sl_concat_slist(Slist *_slist1, Slist *_slist2);
Slist *_sl_sort_slist_func(Slist *_slist, SL_PFC _func);
Slist *sl_list_difference(Slist *l1, Slist *l2);

void compile_dynamic_slist(void);
void enable_slist_compaction(void);
int disable_slist_compaction(void);
void check_and_sometimes_compact_list(void);
void free_dynamic_slist_list(void);

/* enzo */
const void *sl_add_in_order(Slist *, const void *, int (*) (const void*, const void*));


/*
 * QUEUE routines
 */

#define make_queue() sl_make_slist()
#define free_queue(queue) sl_free_slist(queue)
#define enqueue(queue, node) sl_add_to_tail(queue, node)
#define dequeue(queue) sl_get_from_head(queue)
#define head_of_queue(queue) sl_get_slist_head(queue)
#define length_of_queue(queue) sl_slist_length(queue)
#define queue_empty(queue) (length_of_queue(queue) == 0)

/*
 * For upward compatibility
 */

#define NOT_NODES 0

#define make_list() sl_make_slist()
#define copy_list(slist,ign) sl_copy_slist(slist)
#define free_list(slist,ignore) sl_free_slist(slist)
#define flush_list(slist) sl_flush_slist(slist)
#define add_to_head(slist, node) sl_add_to_head(slist, node)
#define add_to_tail(slist, node) sl_add_to_tail(slist, node)
#define delete_list_node(slist, node) sl_delete_slist_node(slist, node)
#define replace_list_node(slist, oldnode, newnode) sl_replace_slist_node(slist, oldnode, newnode)
#define slist_empty(slist) sl_slist_empty(slist)
#define in_list(slist, node) sl_in_slist(slist, node)
#define list_length(slist) sl_slist_length(slist)
#define get_from_head(slist) sl_get_from_head(slist)
#define get_from_tail(slist) sl_get_from_tail(slist)
#define get_list_head(slist) sl_get_slist_head(slist)
#define get_list_tail(slist) sl_get_slist_tail(slist)
#define get_list_next(slist, node)  sl_get_slist_next(slist, node) 
#define get_list_pos(slist, pos)  sl_get_slist_pos(slist, pos) 
#define search_list(slist, node, func) sl_search_slist(slist, node, func)
#define sort_list_func(slist, func) sl_sort_slist_func(slist, func)
#define append_list(sl1, sl2) sl_concat_slist(sl1, sl2)
#define loop_through_list(slist,nnode,type) sl_loop_through_slist(slist,nnode,type)

