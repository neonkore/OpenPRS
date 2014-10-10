/*                               -*- Mode: C -*- 
 * macro.h -- Macros used in OPRS.
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

#ifndef INCLUDE_macro
#define INCLUDE_macro

#include <string.h>
#include <ctype.h>

#include "macro-pub.h"

/* **************************************************************
 * Ces macros sont utilisees a differents endroits dans OPRS
 * ************************************************************** */

#ifndef ABS
#define ABS(a)       (((a) >= 0) ? (a) : -(a))
#endif
#ifndef	MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef	MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif
#define MIN3(a,b,c)     (((a) < (b)) ? (MIN(a,c)) : (MIN(b,c)))
#define SIGN(a)     (((a) < 0) ? (-1) : (1))

#ifdef HAVE_LRAND48
#define RANDOM()	lrand48()
#define SRANDOM(a)	srand48(a)
#elif defined(HAVE_RAND)
#define RANDOM()	rand()
#define SRANDOM(a)	srand(a)
#else
#define RANDOM()	random()
#define SRANDOM(a)	srandom(a)
#endif

#ifndef HAS_BZERO
#define BZERO(a,b)	memset(a,0,b)
#else
#define BZERO(a,b)	bzero((void *)(a),b)
#endif

#ifndef HAS_BCOPY
#define BCOPY(a,b,c)	memcpy(b,a,c)
#else
#define BCOPY(a,b,c)	bcopy((void *)(a),(void *)(b),c)
#endif

#ifndef HAS_INDEX
#define INDEX(a,b)	strchr(a,b)
#else
#define INDEX(a,b)	index(a,b)
#endif

/* Macro d'allocation. */

#define MALLOC(x) (malloc1(x))
#define REALLOC(x,y) (realloc1(x,y))
#define FREE(ptr) (free1((void *)ptr))

void *calloc1(size_t  elemNum, size_t elemSize);
void *dup_alloc1(void *mem );
int alloc_refcount1(void *mem);
void *realloc1(void *ptr, size_t size);

/* Macro to allocate and create a string equal to from pointer by to */
#define NEWSTR(from, to) do {size_t len = strlen(from);\
				   to = (char *)MALLOC(len+1);\
				   BCOPY(from, (char *)to, len+1);	\
				   } while (0)


#define NEWSTR_STD(from, to) do {size_t len = strlen(from);\
				   to = (char *)malloc(len+1);\
				   BCOPY(from, to, len+1);\
				   } while (0)

/* Macro to allocate and create a string equal to from (which is double quoted...) pointed by to (without the quote). */
#define NEWQSTR(from, to) do {size_t len = strlen(from) - 2;\
				   to = (char *)MALLOC(len+1);\
				   BCOPY(from + 1, to, len);\
				   to[len] = '\0';\
				   parser_line_number[parser_index] += debackslashify(to);\
				   } while (0)

/* will normalize an ID according to the various flags... */
#define NORMALIZE_STRING_IN_PLACE(from) \
     if (! no_case_id ) \
		do {size_t i,len = strlen(from); \
			 if (lower_case_id) \
			      for(i = 0; i <= len; i++) \
			        	from[i] = (isupper(from[i]) ? tolower(from[i]) : from[i]); \
			 else \
			      for(i = 0; i <= len; i++) \
			        	from[i] = (islower(from[i]) ? toupper(from[i]) : from[i]); \
				   } while (0)

/* Will return if an id is an id bar */
#define ID_BAR_P(id) ((id)[0] == '|')

/* declare an id, will check for id bar or not and the normalize the string... */

#ifdef VXWORKS
/* this is to try to catch a pb with VxWorks... */
#define DECLARE_ID(from, to) \
	  do {if  (! ID_BAR_P(from)) \
			NORMALIZE_STRING_IN_PLACE(from); \
	      take_or_create_sem(&id_hash_sem); \
	      if ((to = (PString)get_and_check_from_hashtable(id_hash,from)) == NULL) { \
		 NEWSTR(from,to); \
		 sh_add_to_hashtable(id_hash,to,to); } \
	            give_sem(id_hash_sem); \
	   } while (0)
#else
#define DECLARE_ID(from, to) \
	  do {if  (! ID_BAR_P(from)) NORMALIZE_STRING_IN_PLACE(from); \
	      if ((to = (Symbol)get_and_check_from_hashtable(id_hash,from)) == NULL) { \
		 NEWSTR(from,to); \
		 sh_add_to_hashtable(id_hash,to,to); } \
	   } while (0)
#endif

/* same as above, but assume that the from cannot be changed, therefore the copy... */
#define DECLARE_TEXT_ID(from, to) \
	  do {char *tmp; \
	      NEWSTR(from,tmp); \
	      DECLARE_ID(tmp,to) ; \
	      FREE(tmp); \
	   } while (0)

#define DECLARE_VAR(from, to) \
     do {NORMALIZE_STRING_IN_PLACE(from); \
		if ((to = (PString)sh_get_from_hashtable(id_hash,from)) == NULL) { \
		      NEWSTR(from,to); \
		      sh_add_to_hashtable(id_hash,to,to); }\
		   } while (0)


#define REF(ptr) (alloc_refcount1(ptr))
#define DUP(ptr) (dup_alloc1(ptr))

#define LAST_REF(ptr) (REF(ptr) == 1)

#define FREE_NODE(ptr) (free_node(ptr))

#define REF_NODE(ptr) (node_refcount(ptr))

#define DUP_NODE(ptr) (dup_node(ptr))

#define LAST_REF_NODE(ptr) (REF_NODE(ptr) == 1)

#define TIME_STAMP(date) time_stamp(date)

#define REF_LIST(ptr) (node_refcount(ptr))

#define DUP_LIST(ptr) (dup_node(ptr))

#define LAST_REF_LIST(ptr) (REF_LIST(ptr) == 1)

/* #define FREE_LIST(list) do {free_list((list),NOT_NODES); (list) = NULL;} while (0) */

#define FREE_LIST(list) do {if (REF_NODE(list) == 1) \
			       {free_list((list),NOT_NODES);} \
                            else FREE_NODE(list);} \
                        while (0)

#define FLUSH_LIST(list) flush_list((list),NOT_NODES)

#define COPY_LIST(list) copy_list((list),NOT_NODES)

/* Beware, the list arguments are *always* evaluated more than once... */

#define SAFE_SL_DUP_SLIST(l) if(l)DUP_SLIST(l)
#define SAFE_SL_COPY_SLIST(l) ((l)?COPY_SLIST(l):NULL)
#define SAFE_SL_SLIST_EMPTY(l) ((l)?sl_slist_empty(l):TRUE)
#define SAFE_SL_SLIST_LENGTH(l) ((l)?sl_slist_length(l):0)
#define SAFE_SL_ADD_TO_TAIL(l,node) ((l)?sl_add_to_tail(l,node):((l)=sl_make_slist(),sl_add_to_tail(l,node)))
#define SAFE_SL_ADD_TO_HEAD(l,node) ((l)?sl_add_to_head(l,node):((l)=sl_make_slist(),sl_add_to_head(l,node)))
#define SAFE_SL_DELETE_SLIST_NODE(l,node) ((l)?sl_delete_slist_node((l),(node)):NULL)
#define SAFE_SL_GET_FROM_HEAD(l) ((l)?sl_get_from_head(l):NULL)
#define SAFE_SL_GET_FROM_TAIL(l) ((l)?sl_get_from_tail(l):NULL)
#define SAFE_SL_GET_SLIST_NEXT(l,node) ((l)?sl_get_slist_next((l),(node)):NULL)
#define SAFE_SL_LOOP_THROUGH_SLIST(l,o,t) if(l)sl_loop_through_slist(l,o,t)
#define SAFE_SL_FLUSH_SLIST(l) if(l)FLUSH_SLIST(l)
#define SAFE_SL_FREE_SLIST(l) if(l)FREE_SLIST(l)
#define SAFE_SL_IN_SLIST(l,n) ((l)?sl_in_slist(l,n):FALSE)
#define SAFE_SL_CONCAT_SLIST(l1,l2) ((l1)?((l2)?sl_concat_slist(l1,l2):l1):((l2)?l1=l2:NULL))

#define REF_SLIST(ptr) (REF(ptr))

#define DUP_SLIST(ptr) (DUP(ptr))

#define LAST_REF_SLIST(ptr) (REF_SLIST(ptr) == 1)

#define FREE_SLIST(slist) do {if (LAST_REF_SLIST(slist)) \
			       {sl_free_slist(slist);} \
                            else FREE(slist);} \
                        while (0)

#define FLUSH_SLIST(slist) sl_flush_slist(slist)

#define COPY_SLIST(slist) sl_copy_slist(slist)

/* Winsock does not allow the use of read and write on sockets... which are not real file descriptor. */
#ifdef WINSOCK
#define WRITE_SOCK(s,buf,len) send(s,buf,len,0)
#define READ_SOCK(s,buf,len) recv(s,buf,len,0)
#define CLOSE_SOCK(s) closesocket(s)
#else
#define WRITE_SOCK(s,buf,len) write(s,buf,len)
#define READ_SOCK(s,buf,len) read(s,buf,len)
#define CLOSE_SOCK(s) close(s)
#endif

#ifdef WIN95
#define strcasecmp(a,b) stricmp(a,b)
#define euclen(x)	strlen(x)
#endif

#define TYPE(x) ((x)->type)

/* #define LOG_OPER(x) ((x)->log_oper) */
/* #define TEMP_OPER(x) ((x)->temp_oper) */

/* #define MEXPR_GEXPR(x) ((x)->gexpr) */
/* #define MEXPR_GTEXPR(x) ((x)->gtexpr) */
/* #define TEXPR_GEXPR(x) ((x)->gexpr) */

/* #define LTEXPR_GTEXOPRS(x) ((x)->gtexprs) */
/* #define LEXPR_EXOPRS(x) ((x)->exprs) */
/* #define LMEXPR_GMEXOPRS(x) ((x)->gmexprs) */

/* #define GTEXPR_TEXPR(x) ((x)->u.texpr) */
/* #define GTEXPR_LTEXPR(x) ((x)->u.ltexpr) */

/* #define GEXPR_EXPR(x) ((x)->u.expr) */
/* #define GEXPR_LEXPR(x) ((x)->u.lexpr) */

/* #define GMEXPR_MEXPR(x) ((x)->u.mexpr) */
/* #define GMEXPR_LMEXPR(x) ((x)->u.lmexpr) */

#define PROTECT_SYSCALL_FROM_EINTR(res,syscall)  while ((res = (syscall)) == -1 &&  (errno == EINTR)) {}

#define PROTECT_NULL_SYSCALL_FROM_EINTR(res,syscall)  while (!(res = (syscall)) &&  (errno == EINTR)) {}

#ifdef VXWORKS
#define GETTIMEOFDAY(tp,tzp) oprs_gettimeofday(tp)
#else
#ifdef GETTIMEOFDAY_HAS_NOT_TZP
#define GETTIMEOFDAY(tp,tzp) gettimeofday(tp)
#else
#define GETTIMEOFDAY(tp,tzp) gettimeofday(tp,tzp)
#endif
#endif
#endif /* INCLUDE_macro */
