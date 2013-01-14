static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * slistPack.c -- 
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

#include "config.h"
/* include "../opaque-pub.h" enzo */

#ifdef VXWORKS
#include "vxWorks.h"
#include "taskLib.h"
#include "taskVarLib.h"
#endif

#include "slistPack.h"
#include "slistPack_f.h"

#include "stdio.h"
#include "macro-pub.h"
#include "macro.h"
#include "constant-pub.h"

/* DEBUG_COMPACTION cannot reliably work under VxWorks. */
#ifdef VXWORKS
#undef DEBUG_COMPACTION
#endif

#undef DEBUG_COMPACTION

#define SWAP_NODE(s1,s2) \
do {\
     void *_tmp = s1->node;\
     s1->node = s2->node;\
     s2->node = _tmp;\
} while (0)


#define SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) \
	for(snode = slist->first.dy; snode; snode = snode->next)

#ifdef DEBUG_COMPACTION
int _decompiled_slist = 0;
int _decompiled_locked_slist = 0;
int _decompiled_si_slist = 0;
int _decompiled_db_slist = 0;
int _decompiled_st_slist = 0;
int _decompiled_em_slist = 0;
int _compiled_slist = 0;
int _compiled_em_slist = 0;
int _compiled_si_slist = 0;
int _compiled_db_slist = 0;
int _compiled_st_slist = 0;
#endif

#if defined(USE_MULTI_THREAD)
int _slist_compaction_enabled = FALSE;
#else
int _slist_compaction_enabled = TRUE;
#endif

Slist *_dynamic_slist_list = NULL;

void free_dynamic_slist_list(void)
{
     if (_dynamic_slist_list) {
	  if (! sl_slist_empty (_dynamic_slist_list))
	       fprintf(stderr,"slistPack: %d list have not been freed.\n",
		       sl_slist_length(_dynamic_slist_list));
	  FREE_OBJECT(_dynamic_slist_list);
     }
}

Slist *sl_make_slist(void)
{
     Slist *res = MAKE_OBJECT(Slist);

#ifdef CHECK_LOOP_REENTRANCE
     res->loop_used = FALSE;
#endif

     if (!_dynamic_slist_list) {
#ifdef VXWORKS
	  if (taskVarAdd(0,(int *)&_dynamic_slist_list) != OK) {
	       perror("sl_make_slist (_dynamic_slist_list): taskVarAdd");
	       taskSuspend(0);
	  }
	  if (taskVarAdd(0,&_slist_compaction_enabled) != OK) {
	       perror("sl_make_slist (_slist_compaction_enabled): taskVarAdd");
	       taskSuspend(0);
	  }
#endif
	  _dynamic_slist_list = MAKE_OBJECT(Slist);
#ifdef CHECK_LOOP_REENTRANCE
	  _dynamic_slist_list->loop_used = FALSE;
#endif


	  _dynamic_slist_list->type = SLT_DYNAMIC;
	  _dynamic_slist_list->first.dy =
	       _dynamic_slist_list->last.dy =
	       _dynamic_slist_list->current.dy = NULLSnode;
	  _dynamic_slist_list->length = 0;
	  _dynamic_slist_list->dynamic = 0;
     }
     
     res->type = SLT_DYNAMIC;
     
     res->first.dy = res->last.dy = res->current.dy = NULLSnode;
     res->length = 0;
     res->dynamic = 1;
     
     if (_slist_compaction_enabled) {
	  sl_add_to_head(_dynamic_slist_list,res);
     }
     return res;
}

static INLINE Snode *sl_make_snode(void *node)
{
     Snode *res = MAKE_OBJECT(Snode);
     
//     if (!node) {
//	  fprintf(stderr,"slistPack: sl_make_snode: you should not put NULL pointers in slist.\n");
//     }
     
     res->next = NULLSnode;
     res->node = node;

     return res;
}

//static INLINE 
void sl_free_snode(Snode *snode)
{
     FREE_OBJECT(snode);
     return;
}

void *sl_add_to_head(Slist *slist, void *node)
{
     Snode *snode = sl_make_snode(node);
     
     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     snode->next = slist->first.dy;
     slist->first.dy = snode;
     if (!slist->last.dy) slist->last.dy = snode;
     slist->length++;
     if (slist->dynamic) slist->dynamic++; /* if 0 than let it */

     return node;
}

void *sl_add_in_order(Slist *slist, void *node, int (*comp)(void*, void*))
/* enzo: 
 * We assume 'slist' nodes are correctly ordered, with respect to 'comp'.
 * Than, we insert 'node' at the right place, by using 'comp'.
 */
{
     Snode *snode = sl_make_snode(node);
     Snode *snode_tmp, *snode_previous;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     if (! slist->length) {
	 slist->first.dy = slist->last.dy = snode;
     } else {
	 snode_previous = slist->first.dy;
	 snode_tmp = slist->first.dy;
	 while ((snode_tmp != slist->last.dy) &&
		((*comp)(snode_tmp->node, node))) {
	     snode_previous = snode_tmp;
	     snode_tmp = snode_tmp->next;
	 }
	 if ((snode_tmp == slist->last.dy) &&
	     ((*comp)(snode_tmp->node, node))) {
	     snode_tmp->next = snode;
	     slist->last.dy = snode;
	 } else {
	     snode_previous->next = snode;
	     snode->next = snode_tmp;
	 }
     }
     slist->length++;
     if (slist->dynamic)
	 slist->dynamic++; /* if 0 than let it */

     return node;
}

void *sl_add_to_tail(Slist *slist, void *node)
{
     Snode *snode = sl_make_snode(node);

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     if (slist->last.dy)		/* Non Empty */
	  slist->last.dy->next = snode;
     else
	  slist->first.dy = snode;
	  
     slist->last.dy = snode;
     slist->length++;
     if (slist->dynamic) slist->dynamic++; /* if 0 than let it */

     return node;
}

Slist *_sl_copy_slist_func(Slist *slist, SL_PFPV func)
{
     Slist *res;
     Snode *snode;

     if (slist->type == SLT_DYNAMIC) {
	  res = sl_make_slist();

	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       sl_add_to_tail(res, (*func)(snode->node));
	  }
     } else {
	  res = MAKE_OBJECT(Slist);

#ifdef CHECK_LOOP_REENTRANCE
	  res->loop_used = FALSE;
#endif

	  res->current.sd = NULL;
	  res->type = slist->type;
	  res->length = slist->length;
	  res->dynamic = slist->dynamic;

	  switch (slist->type) {
	  case SLT_STATIC: {
	       int i, length = slist->length;

	       res->first.st  = MALLOC(length*sizeof(void *));
	       res->last.st = res->first.st + length - 1;

	       for(i=length-1; i >=0 ; i--)
		    *(res->first.st + i) = (*func)(*(slist->first.st + i));

	       break;
	  }
	  case SLT_DOUBLE:
	       res->first.sd = (*func)(slist->first.sd);
	       res->last.sd = (*func)(slist->last.sd);
	       res->current.sd = NULL;
	       break;
	  case SLT_SINGLE:
	       res->first.sd = (*func)(slist->first.sd);
	       res->last.sd = NULL;
	       res->current.sd = NULL;
	       break;
	  case SLT_EMPTY: 
	       *res = *slist;
	       break;
	  case SLT_DYNAMIC:	/* Just to please gcc */
	       break;
	  }
     }
     
     return res;
}

Slist *_sl_copy_slist_func1(Slist *slist, void *node1, SL_PFPV func)
{
     Slist *res;
     Snode *snode;

     if (slist->type == SLT_DYNAMIC) {
	  res = sl_make_slist();

	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       sl_add_to_tail(res, (*func)(snode->node, node1));
	  }
     } else {
	  res = MAKE_OBJECT(Slist);


#ifdef CHECK_LOOP_REENTRANCE
	  res->loop_used = FALSE;
#endif
	  res->current.sd = NULL;
	  res->type = slist->type;
	  res->length = slist->length;
	  res->dynamic = slist->dynamic;

	  switch (slist->type) {
	  case SLT_STATIC: {
	       int i, length = slist->length;

	       res->first.st  = MALLOC(length*sizeof(void *));
	       res->last.st = res->first.st + length - 1;

	       for(i=length-1; i >=0 ; i--)
		    *(res->first.st + i) = (*func)(*(slist->first.st + i), node1);

	       break;
	  }
	  case SLT_DOUBLE:
	       res->first.sd = (*func)(slist->first.sd, node1);
	       res->last.sd = (*func)(slist->last.sd, node1);
	       res->current.sd = NULL;
	       break;
	  case SLT_SINGLE:
	       res->first.sd = (*func)(slist->first.sd, node1);
	       res->last.sd = NULL;
	       res->current.sd = NULL;
	       break;
	  case SLT_EMPTY: 
	       *res = *slist;
	       break;
	  case SLT_DYNAMIC:	/* Just to please gcc */
	       break;
	  }
     }
     
     return res;
}

Slist *_sl_copy_slist_func2(Slist *slist, void *node1, void *node2, SL_PFPV func)
{
     Slist *res;
     Snode *snode;

     if (slist->type == SLT_DYNAMIC) {
	  res = sl_make_slist();

	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       sl_add_to_tail(res, (*func)(snode->node, node1, node2));
	  }
     } else {
	  res = MAKE_OBJECT(Slist);


#ifdef CHECK_LOOP_REENTRANCE
	  res->loop_used = FALSE;
#endif
	  res->current.sd = NULL;
	  res->type = slist->type;
	  res->length = slist->length;
	  res->dynamic = slist->dynamic;

	  switch (slist->type) {
	  case SLT_STATIC: {
	       int i, length = slist->length;

	       res->first.st  = MALLOC(length*sizeof(void *));
	       res->last.st = res->first.st + length - 1;

	       for(i=length-1; i >=0 ; i--)
		    *(res->first.st + i) = (*func)(*(slist->first.st + i), node1, node2);

	       break;
	  }
	  case SLT_DOUBLE:
	       res->first.sd = (*func)(slist->first.sd, node1, node2);
	       res->last.sd = (*func)(slist->last.sd, node1, node2);
	       res->current.sd = NULL;
	       break;
	  case SLT_SINGLE:
	       res->first.sd = (*func)(slist->first.sd, node1, node2);
	       res->last.sd = NULL;
	       res->current.sd = NULL;
	       break;
	  case SLT_EMPTY: 
	       *res = *slist;
	       break;
	  case SLT_DYNAMIC:	/* Just to please gcc */
	       break;
	  }
     }
     
     return res;
}

Slist *sl_copy_slist(Slist *slist)
{
     Snode *snode;
     Slist *res;

     if (slist->type == SLT_DYNAMIC) {
	  res = sl_make_slist();

	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       sl_add_to_tail(res, snode->node);
	  }
     } else {
	  res = MAKE_OBJECT(Slist);

	  *res = *slist;

#ifdef CHECK_LOOP_REENTRANCE
	  res->loop_used = FALSE;
#endif
	  res->current.sd = NULL;

	  switch (slist->type) {
	  case SLT_STATIC: {
	       int length = slist->length;
	       res->first.st  = MALLOC(length*sizeof(void *));

	       BCOPY(slist->first.st, res->first.st ,length*sizeof(void *));
	       res->last.st = res->first.st + length - 1;
	       break;
	  }
	  case SLT_EMPTY: 
	  case SLT_SINGLE:
	  case SLT_DOUBLE:
	  case SLT_DYNAMIC:	/* Just to please gcc */
	       break;
	  }
     }
     
     return res;
}

void sl_free_slist(Slist *slist)
{
     Snode *tmp, *snode;

     switch (slist->type) {
     case SLT_DYNAMIC:
	  snode = slist->first.dy; 
     
	  while (snode) {
	       tmp = snode->next;
	       sl_free_snode(snode);
	       snode = tmp;
	  }
	  if (_slist_compaction_enabled) {
	       sl_delete_slist_node(_dynamic_slist_list,slist);
	  }
	  break;
     case SLT_STATIC:
#ifdef DEBUG_COMPACTION
	  _compiled_slist--;
	  _compiled_st_slist--;
#endif
	  FREE(slist->first.st);	/* Free the array. */
	  break;
     case SLT_DOUBLE:
#ifdef DEBUG_COMPACTION
	  _compiled_slist--;
	  _compiled_db_slist--;
#endif
	  break;
     case SLT_SINGLE:
#ifdef DEBUG_COMPACTION
	  _compiled_slist--;
	  _compiled_si_slist--;
#endif
	  break;
     case SLT_EMPTY:
#ifdef DEBUG_COMPACTION
	  _compiled_slist--;
	  _compiled_em_slist--;
#endif
	  break;
     }
     FREE_OBJECT(slist);
     return;
}

void sl_flush_slist(Slist *slist)
{
     Snode *tmp, *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);
     
     snode = slist->first.dy; 
     
     while (snode) {
	  tmp = snode->next;
	  sl_free_snode(snode);
	  snode = tmp;
     }

     slist->first.dy = slist->last.dy = slist->current.dy = NULLSnode;
     slist->length = 0;

     return;
}

void **sl_add_to_tail_ret_ref(Slist *slist, void *node)	/* return the @ of the pointer to the node */
{
     Snode *snode = sl_make_snode(node);

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     if (slist->last.dy)		/* Non Empty */
	  slist->last.dy->next = snode;
     else
	  slist->first.dy = snode;
	  
     slist->last.dy = snode;
     slist->length++;
     if (slist->dynamic) slist->dynamic++;

     return &(snode->node);
}

void *sl_insert_slist_pos(Slist *slist, void *node, int pos)
{
     Snode *snode, *res;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     if (pos <= 1 || ! (slist->length))
	  return sl_add_to_head(slist, node);

     if (pos >= (signed)slist->length)
	  return sl_add_to_tail(slist, node);

     pos--;
     res = sl_make_snode(node);

     SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	  if (!--pos) {
	       res->next = snode->next;
	       snode->next = res;
	       slist->length++;
	       return node;
	  }
     }
     
     return NULL;
}

void *sl_delete_slist_node(Slist *slist, void *node)
{
/* _sl_delete_slist_func  and  sl_delete_slist_node are almost the same... so if you find
 * a bug here, most likely, the same is in the other.
 */
     Snode *prev, *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,TRUE);

     snode = slist->first.dy; 
     prev = NULLSnode;

     while (snode) {
	  if (snode->node == node) {
	       if (snode == slist->first.dy) 
		    slist->first.dy = snode->next;
	       else
		    prev->next = snode->next;
	       if (snode == slist->last.dy) 
		    slist->last.dy = prev;
	       break;
	  }
	  prev = snode;
	  snode = snode->next;
     }
     
     if (snode) {
	  if (slist->current.dy == snode) slist->current.dy = NULLSnode;
	  sl_free_snode(snode);
	  slist->length--;
	  return node;
     } else 
	  return NULL;
}

void *_sl_delete_slist_func(Slist *slist, void *node, SL_PFC func)
{
/* _sl_delete_dy_slist_func  and  sl_delete_dy_slist_node are almost the same... so if you find
 * a bug here, most likely, the same is in the other.
 */
     Snode *prev, *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,TRUE);

     snode = slist->first.dy; 
     prev = NULLSnode;

     while (snode) {
	  if  ((*func)(node, snode->node)) {
	       if (snode == slist->first.dy) 
		    slist->first.dy = snode->next;
	       else
		    prev->next = snode->next;
	       if (snode == slist->last.dy) 
		    slist->last.dy = prev;
	       break;
	  }
	  prev = snode;
	  snode = snode->next;
     }
     
     if (snode) {
	  void *res;

	  res = snode->node;
	  if (slist->current.dy == snode) slist->current.dy = NULLSnode;
	  sl_free_snode(snode);
	  slist->length--;
	  return res;
     } else 
	  return NULL;
}

void *sl_replace_slist_node(Slist *slist, void *oldnode, void *newnode)
{
     Snode *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	  if (snode->node == oldnode) {
	       break;
	  }
     }
     
     if (snode) {
	  snode->node = newnode;
	  return newnode;
     } else 
	  return sl_add_to_tail(slist, newnode);
}

void *sl_replace_slist_func(Slist *slist, SL_PFPV func)
{
     Snode *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,FALSE);

     SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	  snode->node = (*func)(snode->node);
     }
     
     return slist;
}

int sl_slist_empty(Slist *slist)
{
     return (slist->length == 0);
}

int sl_in_slist(Slist *slist, void *node)
{
     Snode *snode;
     unsigned int i = 1;
     int res = 0;

     switch (slist->type) {
     case SLT_DYNAMIC:
	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       if (snode->node == node) {
		    res = i;
		    break;
	       } else i++;
	  }
	  break;
     case SLT_STATIC:
	  for(i=1; i <= slist->length; i++)
	       if (*(slist->first.st + i - 1) == node) {
		    res = i;
		    break;
	       }
	  break;
     case SLT_DOUBLE:
	  if (slist->last.sd == node) res = 2;
     case SLT_SINGLE:
	  if (slist->first.sd == node) res = 1;
	  break;
     case SLT_EMPTY:
	  break;
     }
     
     return res;
}

int sl_slist_length(Slist *slist)
{
     return slist->length;
}

void *sl_get_from_head(Slist *slist)
{
     void *res;
     Snode *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,TRUE);

     if ((snode = slist->first.dy)) {
	  res = snode->node;
	  slist->first.dy = snode->next;
	  if (slist->last.dy == snode)
	       slist->last.dy = NULLSnode;
	  if (slist->current.dy == snode) slist->current.dy = NULLSnode;
	  sl_free_snode(snode);
	  slist->length--;
     } else
	  res = NULL;

     return res;
}

void *sl_get_from_tail(Slist *slist) /* Very expensive... avoid it as much as possible... please. */
{
     void *res;
     Snode *snode;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,TRUE);

     if (slist->length == 0)
	  return NULL;

     snode = slist->first.dy;
     if (slist->length == 1) {
	  slist->first.dy = slist->last.dy = slist->current.dy = NULLSnode;
	  slist->length = 0;
	  res = snode->node;
	  sl_free_snode(snode);
     } else {
	  while (snode) {
	       if (snode->next == slist->last.dy) {
		    break;
	       }
	       snode = snode->next;
	  }

	  if (snode) {		/* The before last item */
	       Snode *tmp;

	       slist->last.dy = snode;
	       slist->length--;
	       tmp = snode;
	       snode = snode->next; /* grab the last snode. */
	       tmp->next = NULLSnode; /* unlink the last snode... may fix MW's bug! */
	       if (slist->current.dy == snode) slist->current.dy = NULLSnode;
	       res =  snode->node;
	       sl_free_snode(snode);
	  } else {		/* snode is NULLSnode */
	       fprintf(stderr,"slistPack: sl_get_from_tail: list problem.\n");
	       res = NULL;
	  }
     }
     return res;
}

void *sl_get_slist_head(Slist *slist)
{
     switch (slist->type) {
     case SLT_DYNAMIC:
	  return (slist->first.dy ? slist->first.dy->node : NULL);
     case SLT_STATIC:
	  return  *(slist->first.st);
     case SLT_DOUBLE:
     case SLT_SINGLE:
	  return slist->first.sd;
     case SLT_EMPTY:
	  return NULL ;
     }
     return NULL ;
}

void *sl_get_slist_tail(Slist *slist)
{
     switch (slist->type) {
     case SLT_DYNAMIC:
	  return (slist->last.dy ? slist->last.dy->node : NULL);
     case SLT_STATIC:
	  return  *(slist->last.st);
     case SLT_DOUBLE:
	  return slist->last.sd;
     case SLT_SINGLE:
	  return slist->first.sd;
     case SLT_EMPTY:
	  return NULL ;
     }
     return NULL ;
}

void *sl_get_slist_pos(Slist *slist, int pos)
{
     Snode *snode;

     switch (slist->type) {
     case SLT_DYNAMIC:
	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) {
	       if (!--pos) return snode->node;
	  }
	  break;
     case SLT_STATIC:
	  if (pos > 0 || (unsigned)pos < slist->length)
	       return *(((void **)(slist->first.st)) + pos - 1);
	  break;
     case SLT_DOUBLE:
	  if (pos == 2) return slist->last.sd;
     case SLT_SINGLE:
	  if (pos == 1) return slist->first.sd;
	  break;
     case SLT_EMPTY:
	  break;
     }
     
     return NULL;
}

void *sl_get_slist_next(Slist *slist, void *node) /* Do not use it to delete node... */
{
     if (!slist->first.dy) return NULL;
 
     switch (slist->type) {
     case SLT_DYNAMIC:
	  if (! node) {
	       slist->current.dy = slist->first.dy;
	       return slist->first.dy->node;
	  } if (slist->current.dy && slist->current.dy->node == node) {
	       slist->current.dy = slist->current.dy->next;
	       if (slist->current.dy)
		    return slist->current.dy->node;
	       else
		    return NULL;
	  } else {
	       Snode *snode;
	  
	       snode = slist->first.dy; 

	       while (snode) {
		    if (snode->node == node) {
			 slist->current.dy = snode->next;
			 if (slist->current.dy)
			      return slist->current.dy->node;
			 else
			      return NULL;
		    }
		    snode = snode->next;
	       }
	  }
	  
	  return NULL;
     case SLT_STATIC:
	  if (! node) {
	       slist->current.st = slist->first.st;
	       return *(slist->first.st);
	  } if (slist->current.st && *(slist->current.st) == node) {
	       slist->current.st = slist->current.st + 1;
	       if (slist->current.st <= slist->last.st)
		    return *(slist->current.st);
	       else
		    return NULL;
	  } else {
	       void **nnode;
	  
	       for(nnode = slist->first.st; nnode <= slist->last.st; nnode++)
		    if (*nnode == node) {
			 slist->current.st = nnode;
			 if (nnode == slist->last.st)
			   return NULL;
			 else
			   return *(slist->current.st + 1);
		    }
	  }
	  
	  return NULL;
     case SLT_DOUBLE:
	  if (!node)
	       return slist->first.sd;
	  else if (node ==  slist->first.sd)
	       return slist->last.sd;
	  else
	       return NULL;
     case SLT_SINGLE:
	  if (!node)
	       return slist->first.sd;
	  else 
	       return NULL;
     case SLT_EMPTY:
	  break;
     }
     return NULL;
}

Slist *sl_concat_slist(Slist *sl1, Slist *sl2)
{

     sl_check_loop_corruption(sl1);
     sl_check_loop_corruption(sl2);

     if (sl2->type) decompile_slist(sl1,FALSE);
     if (sl1->type) decompile_slist(sl2,FALSE);

    if (sl1 == sl2) return sl1;		/* ignore this call	*/

     if (sl1->last.dy) {		/* Non empty */
	  if (sl2->last.dy) {	/* Non empty */
	       sl1->last.dy->next = sl2->first.dy;
	       sl1->last.dy = sl2->last.dy; 
	       sl1->length += sl2->length;
	  }
     } else {			/* sl1 empty */
	  *sl1 = *sl2;
     }

     if (_slist_compaction_enabled) {
	  sl_delete_slist_node(_dynamic_slist_list,sl2);
     }
     FREE_OBJECT(sl2);
     
     return sl1;
}

Slist *sl_list_difference(Slist *l1, Slist *l2)
{
     void * node;
     Slist *res = sl_make_slist();

     sl_loop_through_slist(l1, node, void *) {
	  if (! (sl_in_slist(l2,node)))
	       sl_add_to_tail(res,node);
     }
     return res;
}

void *_sl_search_slist(Slist *slist, void *node, SL_PFC func)
{
     Snode *snode;
     void **nnode;
     
/*      if (slist->length >150 ) */
/* 	  fprintf(stderr,"Search in a long list\n"); */

     switch (slist->type) {
     case SLT_DYNAMIC:
	  SL_LOOP_THROUGH_DY_SLIST_SNODE(slist,snode) 
	       if ((*func)(node, snode->node)) return (snode->node);
	  break;
     case SLT_STATIC:
	  for(nnode = (void **)slist->first.st; nnode <= slist->last.st; nnode++)
	       if ((*func)(node, *nnode)) return (*nnode);
	  break;
     case SLT_DOUBLE:
	  if ((*func)(node, slist->first.sd)) return slist->first.sd;
	  if ((*func)(node, slist->last.sd)) return slist->last.sd;
	  break;
     case SLT_SINGLE:
	  if ((*func)(node, slist->first.sd)) return slist->first.sd;
	  break;
     case SLT_EMPTY:
	  break;
     }
     return NULL;
}


/* This version should be more efficient... as it swap the node in place. */

Slist *_sl_sort_slist_func(Slist *slist, SL_PFC	func)
{
     Snode *ptr1;
     Snode *ptr2;

     sl_check_loop_corruption(slist);

     if (slist->type) decompile_slist(slist,TRUE);

     if (slist->length <= 1) return slist;

     SL_LOOP_THROUGH_DY_SLIST_SNODE(slist, ptr1) {
	  for(ptr2 = ptr1->next; ptr2; ptr2 = ptr2->next)
	       if (!((*func)(ptr1->node, ptr2->node)))
		    SWAP_NODE(ptr1,ptr2);

     }
     
     return slist;
}

Slist_Type sl_slist_type(Slist *slist)
{
     return slist->type;
}

void compile_slist(Slist *slist)
{
     if (slist->type != SLT_DYNAMIC) return;
     switch (slist->length) {
     case 0: 
	  slist->current.sd = 0;
#ifdef DEBUG_COMPACTION
	  _compiled_em_slist++;
#endif
	  slist->type = SLT_EMPTY;
	  break;
     case 1: {
	  Snode *singleton;
	  
	  singleton = slist->first.dy;
	  slist->current.sd = (slist->current.dy == slist->first.dy ? singleton->node : 0);
	  slist->first.sd = singleton->node;
	  slist->last.sd = NULL;
	  slist->type = SLT_SINGLE;
	  sl_free_snode(singleton);
#ifdef DEBUG_COMPACTION
	  _compiled_si_slist++;
#endif
	  break;
     }
     case 2: {
	  Snode *first = slist->first.dy;
	  Snode *second = slist->last.dy;

	  slist->current.sd = (slist->current.dy == slist->first.dy ? first->node:
			       (slist->current.dy == slist->last.dy ? second->node : 0));
	  slist->first.sd = first->node;
	  slist->last.sd = second->node;
	  slist->type = SLT_DOUBLE;
	  sl_free_snode(first);
	  sl_free_snode(second);
#ifdef DEBUG_COMPACTION
	  _compiled_db_slist++;
#endif
	  break;
     }
     default: {
	  Snode *snode, *tmp;
	  void **array = MALLOC(slist->length*sizeof(void *));
	  int i = 0, found = 0;

	  snode = slist->first.dy; 

	  while (snode) {
	       if (! found && slist->current.dy == snode) {
		    slist->current.st = array + i;
		    found = 1;
	       }
	       tmp = snode->next;
	       *(array + i ) = snode->node;
	       i++;
	       sl_free_snode(snode);
	       snode = tmp;
	  }	   

	  if (! found) slist->current.st = 0;

	  slist->first.st = array;
	  slist->last.st = array + i - 1;
	  slist->type = SLT_STATIC;
#ifdef DEBUG_COMPACTION
	  _compiled_st_slist++;
#endif
	  break;
     }
     }
}

Slist *decompile_slist(Slist *slist, int lock_it)
{
     switch (slist->type) {
     case SLT_DYNAMIC:
	  return slist;
     case SLT_SINGLE: {
	  Snode *snode = sl_make_snode(slist->first.sd);

	  snode->next = NULLSnode;
	  slist->current.dy = (slist->current.sd == slist->first.sd ? snode : 0);
	  slist->first.dy = slist->last.dy = snode;
#ifdef DEBUG_COMPACTION
	  _decompiled_si_slist++;
	  _compiled_si_slist--;
#endif
	  break;
     }
     case SLT_DOUBLE: {
	  Snode *snode1 = sl_make_snode(slist->first.sd);
	  Snode *snode2 = sl_make_snode(slist->last.sd);

	  slist->current.dy = (slist->current.sd == slist->first.sd ? snode1:
			       (slist->current.sd == slist->last.sd ? snode2 : 0));
	  snode2->next = NULLSnode;
	  slist->last.dy = snode1->next = snode2;
	  slist->first.dy = snode1;
#ifdef DEBUG_COMPACTION
	  _decompiled_db_slist++;
	  _compiled_db_slist--;
#endif
	  break;
     }
     case SLT_STATIC: {
	  int i, found = 0;
	  void **array = slist->first.st;
	  Snode *prev = NULLSnode;

	  for (i = slist->length;i > 0; i--) {
	       Snode *snode = sl_make_snode(*(array + i - 1));
		    
	       if (!found && (array + i - 1 == slist->current.st)) {
		    slist->current.dy = snode;
		    found = 1;
	       }

	       if (!prev) slist->last.dy = snode;
	       snode->next = prev;
	       prev = snode;
	  }
	  slist->first.dy = prev;
	  if (! found) slist->current.dy = NULLSnode;
	  FREE(array);	/* Free the array. */

#ifdef DEBUG_COMPACTION
	  _decompiled_st_slist++;
	  _compiled_st_slist--;
#endif
	  break;
     }
     case SLT_EMPTY:
#ifdef DEBUG_COMPACTION
	  _decompiled_em_slist++;
	  _compiled_em_slist--;
#endif
	  break;
     }
     slist->type = SLT_DYNAMIC;
     if (lock_it) {
#ifdef DEBUG_COMPACTION
	  _decompiled_locked_slist++;
#endif
	  slist->dynamic = 0;		/* this is to tell... no way jose... we are not going to 
					   make the same mistake again...*/
     } else { 
	  slist->dynamic = slist->length + 1;
	  if (_slist_compaction_enabled) {
	       sl_add_to_head(_dynamic_slist_list,slist);
	  }
     }
#ifdef DEBUG_COMPACTION
     _decompiled_slist++;
#endif
     return slist;
}

void compile_dynamic_slist(void)
{
     Slist *slist;

     if (_slist_compaction_enabled && _dynamic_slist_list) {
	  sl_loop_through_slist(_dynamic_slist_list, slist, Slist *) {
	       if ((unsigned)(slist->length + 1) == slist->dynamic) {
		    compile_slist(slist);
#ifdef DEBUG_COMPACTION
		    _compiled_slist++;
#endif
	       }
	  }
	  sl_flush_slist(_dynamic_slist_list);
#ifdef DEBUG_COMPACTION
	  report_slist_compaction();
#endif
     }
}

void enable_slist_compaction(void)
{
#if !defined(USE_MULTI_THREAD)
     _slist_compaction_enabled = TRUE;
#endif
}

int disable_slist_compaction(void)
{
     int old = _slist_compaction_enabled;

     if (old) compile_dynamic_slist();
     _slist_compaction_enabled = FALSE;
     if (_dynamic_slist_list) sl_flush_slist(_dynamic_slist_list);
     return old;
}

#ifdef DEBUG_COMPACTION
report_slist_compaction()
{
     fprintf(stderr,"slistPack: compacted %d lists (%d empty, %d statics, %d doubles, %d singles).\n", 
	     _compiled_slist, _compiled_em_slist, _compiled_st_slist, _compiled_db_slist,
	     _compiled_si_slist);
     fprintf(stderr,"slistPack: decompacted %d lists (%d locked) (%d statics, %d doubles, %d singles, %d empty).\n", 
	     _decompiled_slist, _decompiled_locked_slist, _decompiled_st_slist, _decompiled_db_slist, _decompiled_si_slist, _decompiled_em_slist);
}
#endif


#define CHECK_FREQUENCY 10
#define MAX_SLIST_SIZE 40
#define DISABLE_AFTER_X_CALL 20

void check_and_sometimes_compact_list(void)
{
     if (_slist_compaction_enabled) {
	  static int j=DISABLE_AFTER_X_CALL;
	  static int i=CHECK_FREQUENCY;

	  if (!--i) {
	       if (_dynamic_slist_list->length > MAX_SLIST_SIZE) {
		    compile_dynamic_slist();
		    j=DISABLE_AFTER_X_CALL;
	       }
	       i=CHECK_FREQUENCY;
	       if (!--j)
		    disable_slist_compaction(); /* it is not changing anymore */
	  }
     }
}
