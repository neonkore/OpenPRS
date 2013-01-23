/*
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

#include "config.h"

#if !defined(OPRS_MALLOC) && !defined(STD_MALLOC)
#define MIXED_MALLOC
#endif

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#include "semLib.h"
#include "taskLib.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "mallocPack_f.h"

#ifndef HAS_BCOPY
#define BCOPY(from, to, nbytes) memcpy(to, from, nbytes)
#endif
#ifndef HAS_BZERO
#define BZERO(a, b) memset(a, 0, b)
#endif

/* memalign does not seem to be declared in stdlib.h despite the man page. */
void *memalign(size_t boundary, size_t size);

#ifdef USE_MULTI_THREAD
#include <pthread.h>

pthread_mutex_t malloc_mtx = PTHREAD_MUTEX_INITIALIZER;

/* pthread_once_t malloc_mtx_init = PTHREAD_ONCE_INIT; */

/* void init_malloc_mtx(void) */
/* { */
/*      if (pthread_mutex_init(&malloc_mtx, NULL)) { */
/* 	  perror("init_malloc_mtx: pthread_mutex_init"); */
/*      } */
/* } */


void oprs_pthread_mutex_lock(pthread_mutex_t *mtx)
{
/*     if (pthread_once(&malloc_mtx_init, init_malloc_mtx)) 
       perror("oprs_pthread_mutex_lock: pthread_once"); */
     if (pthread_mutex_lock(&malloc_mtx)) {
	  perror("oprs_pthread_mutex_lock: pthread_mutex_lock");
     }
}

void oprs_pthread_mutex_unlock(pthread_mutex_t *mtx)
{
     if (pthread_mutex_unlock(&malloc_mtx)) {
	  perror("oprs_pthread_mutex_unlock: pthread_mutex_unlock");
     }
}
#endif

#ifdef VXWORKS
void take_or_create_sem(SEM_ID *sem)
{
     if (! *sem) {
	  if (taskLock() != OK) {
	       perror("take_or_create_sem:taskLock");
	       taskSuspend(0);
	  }
	  if (! *sem) {	/* Need to check again... after locking... */
	       if (!(*sem = semMCreate(SEM_Q_PRIORITY|
					    SEM_INVERSION_SAFE|
					    SEM_DELETE_SAFE))) {
		    perror("take_or_create_sem:semMCreate");
		    taskSuspend(0);
	       }
	  }
	  if (taskUnlock() != OK) {
	       perror("take_or_create_sem:taskUnlock");
	       taskSuspend(0);
	  }
     }

     if (semTake(*sem, WAIT_FOREVER) != OK) {
	  perror("take_or_create_sem:semTake");
     }
}

void give_sem(SEM_ID sem)
{
     if (semGive(sem) != OK) {
	  perror("give_sem:semGive");
     }
}
#endif

#ifdef STD_MALLOC

/* In STD_MALLOC, no problem of shareability of thelib. Standard malloc. */

#ifdef VXWORKS
#include "vxWorks.h"
#include "memLib.h"
#include "stdlib.h"
#else
#ifndef GNUWIN32
#include <stdlib.h>
#endif
#endif

/* Under VxWorks, no need to realign on double... int are OK, short do not work though... */
#ifdef VXWORKS
#define HEADER_TYPE int
#define HEADER_CONTENT int
#else
#define HEADER_TYPE double
#define HEADER_CONTENT int
#endif

/* BLK are chunks of memory while MEM are our mem (ie with our header). */

#define REF_BLK(mem) (*((HEADER_CONTENT *)(mem)))

#define REF_MEM(mem) (*((HEADER_CONTENT *)((HEADER_TYPE *)(mem) - 1)))

#define BLK_MEM(ptr) ((void *)((HEADER_TYPE *)(ptr) - 1))

#define MEM_BLK(ptr) ((void *)((HEADER_TYPE *)(ptr) + 1))


void *malloc1(unsigned nbytes)
{
     void *mem;
     
     if (nbytes == 0) return NULL;
     
     nbytes += sizeof(HEADER_TYPE);
     
     if ((mem = malloc(nbytes)) == 0) {
	  fprintf(stderr,"malloc1: unable to get memory.\n"); 
	  return NULL;
     } else {
	  REF_BLK(mem) = 1;
	  return (MEM_BLK(mem));
     }
}

void *realloc1(void *ptr, unsigned nbytes)
{
     void *mem;
     
     if (ptr == NULL) return malloc1(nbytes);
     
     nbytes += sizeof(HEADER_TYPE);
     
     if ((mem = realloc(BLK_MEM(ptr), nbytes)) == 0) { /* This will recopy the header too */
	  fprintf(stderr,"realloc1: unable to get memory.\n"); 
	  return NULL;
     } else {
	  return (MEM_BLK(mem));
     }
}

void *dup_alloc1(void *mem)
{
     if (mem)
          REF_MEM(mem)++;
     return mem;
}

int alloc_refcount1(void *mem)
{
     if (mem == NULL) return 0; /* ignore this call */
     
     return REF_MEM(mem); 
}

void free1(void *ptr)
{
     if (ptr == NULL) return; /* ignore this call */
     
     if (REF_MEM(ptr)-- == 1)
	  free(BLK_MEM(ptr));
#ifdef OPRS_DEBUG
     else if (REF_MEM(ptr) == -1)
	  fprintf(stderr,"free1: freeing an already freed object.\n"); 

#endif
}

void *calloc1(unsigned number, unsigned nbytes)
{ 
     void *p;
     unsigned int onb;
     
     onb = number * nbytes; /* number of bytes to be allocated */
     if ((p = malloc1(onb)) == NULL) return NULL;
     
     BZERO(p, onb);
     return p;
}

void pr_mstats(FILE *fp, char *s)
{
     fprintf(stderr,"Sorry, no memory stats while using the STD_MALLOC lib.\n"); 
}

void *copy_alloc(void *ap)
{
     fprintf(stderr,"No copy_alloc for 0x%x in this lib.\n", (unsigned int)ap);
     return NULL;
}

#else /* STD_MALLOC */

/* Either we use OPRS malloc or a mixed malloc */

#ifdef MIXED_MALLOC

#define _MALLOC malloc1
#define _FREE free1
#define _REALLOC realloc1
#define _CALLOC calloc1
#define _ALLOC_REFCOUNT alloc_refcount1
#define _DUP_ALLOC dup_alloc1

#else /* MIXED_MALLOC */

#define _OPRS_MALLOC
#define _MALLOC malloc
#define _FREE free
#define _REALLOC realloc
#define _CALLOC calloc
#define _DUP_ALLOC dup_alloc1
#define _ALLOC_REFCOUNT alloc_refcount1

#endif /* MIXED_MALLOC */

/* ************************************************************************
 *
 *
 * ************************************************************************

 C Programmers' Toolbox

 Purpose: Memory management routines. The standard Unix
 		free(), malloc(), calloc(), and realloc() are
			provided, as well copy_alloc() alloc_size(),
			and alloc_refcount()

	Author:		Thaddeus Julius Kowalski (1983,1984,1985)
			Robert A Zimmermann (1985 ...)

	Notes:		Compile with the DO_CHECK option set to n or greater:
			0 - Do no run time checking or error reporting
			1 - Report errors
			2 - Generate statistics
			3 - Border checking. Add a word before and after each
			 allocated chunk.

	History:	
	05 Dec 85	Complete re-write:
			 to allow more flexibility of allocation sizes
			 to allow refcounts
			 to better generate stats
	10 Dec 85	Expanded alloc_size in-line for speed
			Changed statistics, slightly
			Sorted the big-list in malloc()
	10 Jan 86	Added alloc_refcount

	28 Jul 86	Fixed realloc bug

	31 Jul 86	Added berkeley o/s testing

	15 Sep 86	Changed which_size to a short.
			malloc will now break if 32 gigabyte blocks are req'd

	04 Dec 86	Removed copymem in favor of bcopy

	25 Feb 87	Added bcopy macro on BELL machines

	17 Mar 87	Added proper defs for non-debug
			Added proper include stuff

	21 Apr 87	Added non-berkley error messages

	13 May 87	Folded in Roy Jewell apollo code

	01 Sep 87	Set calloc() to be compiled on AEGIS

	19 May 88	Added valloc and memalign()
	 

 *************************************************************************

This is a very fast storage allocator. It allocates blocks of a small 
number of different sizes, and keeps free lists of each size. Blocks that
don't exactly fit are passed up to the next larger size. Beyond the largest
size, it will allocate blocks of the requested size (rounded up to the
nearest 1k boundary) and do a first-fit reallocation from freed blocks.

In this implementation, the available sizes stored in the 'nodes' table.
This table contains the information needed to do allocation, and will
maintain statistics on their use. Additional sizes may be added by changing
the 'SIZES' constant, and adding the desired block size. Warning: be
sure to include the 4 byte overhead.

This is designed for use in a program that uses vast quantities of memory,
but bombs when it runs out. To make it a little better, it warns the
user when he starts to get near the end.


allocation routines:
OPRS_NODE	malloc(size)		Returns a pointer to a block of memory
				with SIZE bytes.

OPRS_NODE	copy_alloc(node)	Returns a pointer to a block of memory
				with with a copy of 'node'.

OPRS_NODE	realloc(node, new_size)
				Allocates a new block of memory, and
				copies as much data as possible into
				the new node. The old node is freed.
	
OPRS_NODE	dup_alloc(node)	Increments the reference count of
				the node.

OPRS_NODE	memalign(align, size)	Allocates size bytes on a the specified boundary.

OPRS_NODE	valloc(size)		Allocates size bytes aligned on a
				getpagesize() boundary.

deallocation routine:
int	free(node)		Decrements the refcount, and returns the
				node to the free-list if it was zero.

other routines:
int	alloc_size(node)	Returns number of bytes in node

int	alloc_refcount(node)	Returns the reference count


 ************************************************************************
 *		Set up defaults, error handlers, etc.			*
 ************************************************************************/

#ifdef DEBUG
#define DO_CHECK 3
#endif

#ifndef DO_CHECK /* Set default error checking */
#define DO_CHECK 2
#endif

#ifdef HAS_RLIMIT
#include <sys/time.h> /* needed by the storage requester */
#include <sys/resource.h>
#endif

#ifdef HAS_SIGSETMASK
#include <signal.h>
#endif

#include <unistd.h>

#define malloc_warn(str) \
write(2, str, strlen(str));

#ifdef VXWORKS
#define malloc_fatal(str) do { \
	  malloc_warn(str); \
	  malloc_warn("The task is being suspended for inspection."); \
	  taskSuspend(0); \
} while(0)
#else
#ifdef HAS_ABORT
#define malloc_fatal(str) do { \
	  malloc_warn(str); \
	  abort(); \
} while(0)
#else
#define malloc_fatal(str) do { \
	  malloc_warn(str); \
	  kill(0, SIGKILL); \
} while(0)
#endif
#endif

#if DO_CHECK > 0
static char *botch = "mallocPack: assertion failed: %s\n",
*free_err = "%s: attempt to access a (probably) freed node 0x%x\n",
*err_high_border = "%s: high border of node 0x%x is overwritten\n",
*err_low_border = "%s: low border of node 0x%x is overwritten\n";

static void malloc_error(char *str, char *func, char *ap)
{
     /* this is a function so that we can set a breakpoint in it */
     fprintf(stderr, str, func, ap);
}

#define ASSERT(p) if (!(p)) { \
			    malloc_error(botch, #p, NULL); \
		 	    abort(); \
			    }

#if DO_CHECK > 1
#define MAGIC 0x1b /* random value for tagging our nodes */
#define ASSERT_MAGIC(mp) ASSERT(HEAD(mp).our_block == MAGIC)

#define BAD_POINTER(func,mp,addr) \
		   if (HEAD(mp).our_block != MAGIC) { \
		   malloc_error(free_err, func, addr); \
		   return ; \
		   }

#else
#define ASSERT_MAGIC(mp)
#endif

#else
#define ASSERT(p)
#define ASSERT_MAGIC(mp)
#define BAD_POINTER(func,mp,addr)
#endif

#ifndef NULL /* pointer to nowhere */
#define NULL 0
#endif

/************************************************************************
 * Define the parameters for memory allocation 	*
 *----------------------------------------------------------------------*
 *	SIZES:	The number of valid memory sizes to be allocated.	*
 *		Each size will require an entry in the 'nodes' table,	*
 *		and they must be put in the table in ascending order.	*
 *----------------------------------------------------------------------*
 *	BLOCK_SIZE:	The size of the pages which we will request	*
 *		from the system. Each page, when received, will be	*
 *		subdivided into equal sized chunks of the required size.*
 *		The first will be returned to the user, the rest saved	*
 *		for future use.						*
 *		This must be a power of 2 (1024, 2048, etc.)		*
 *----------------------------------------------------------------------*
 *	BIGGEST_BLOCK:	The largest node that malloc may get by		*
 *		subdividing a BLOCK_SIZE byte page. Any request for	*
 *		nodes larger than this will be dealt with in multiples	*
 *		of BLOCK_SIZE chunks, so it should generally be the	*
 *		same as BLOCK_SIZE.					*
 ************************************************************************/

#ifdef VXWORKS
#define SIZES 11 /* number of block lists */
#else
#define SIZES 9 /* number of block lists */
#endif
#define LAST_LIST (SIZES-1) /* this is the last small one */
#define BIG_LIST LAST_LIST /* this is the over-sized one */

#define BLOCK_POWER 11 /* grab in 2^n byte chunks */

#ifdef VXWORKS
#define BLOCK_SIZE ((1<<BLOCK_POWER) -8)  /* -8 for VxWorks alloc header... */
#define BLOCK_SIZE_ALIGNED (1<<BLOCK_POWER) /* minimum for system request */
#define BIGGEST_BLOCK BLOCK_SIZE
#define BLOCK_BITS (BLOCK_SIZE_ALIGNED - 1) /* low bits below 1k */
#else
#define BLOCK_SIZE (1<<BLOCK_POWER) /* minimum for system request */
#define BLOCK_SIZE_ALIGNED (1<<BLOCK_POWER) /* minimum for system request */
#define BIGGEST_BLOCK BLOCK_SIZE /* maximum 'small' block */
#define BLOCK_BITS (BLOCK_SIZE_ALIGNED - 1) /* low bits below 1k */
#endif


/* We can cast to int, because we areonly interested in the lower bits. */

#define is_small(ptr) (((long)(ptr) & (long)BLOCK_BITS) != 0)


#define MAX_REFCOUNT ((unsigned short) ~0) /* beyond this, the block is un-freeable */

#if DO_CHECK > 2
#define BORDER_BITS 0xF123456F
#endif

/************************************************************************
 *		Define the data structures and types			*
 ************************************************************************/


typedef void * OPRS_NODE; /* rather than importing the header */

typedef struct {
#if DO_CHECK > 1
     unsigned int our_block; /* to tag blocks we created */
#elif (! defined (VXWORKS))
     unsigned int pad;		/* Only VxWorks can accept a header of size 4.*/
#endif
     unsigned short which_size; /* which free-list is it from? */
     unsigned short refcount; /* advanced memory support */
} header;



#if DO_CHECK > 2

/* We will put a 1 word border around all allocations so that any write
 * which exceeds the block will have a better chance of being detected
 */
typedef struct memory_struct {
     unsigned border; /* constant unless trashed */
     union {
	  header head; /* our information bytes */
	  struct memory_struct *next_node; /* the free list pointer */
	  double not_use; /* This is to prevent from unaligned acces */
     } data; /* next or current node */
} *memory;

#define HEADSIZE() (sizeof(struct memory_struct))
#define NODESIZE() (HEADSIZE() + sizeof(unsigned))
#define HEAD(p) (p)->data.head
#define NEXT(p) (p)->data.next_node
#define LOW_BORDER(p) (p)->border
#define HIGH_BORDER(p) ((int *)(p))[ptr->size/sizeof(unsigned) -1]

#define BAD_BORDER(func,mp,ap) \
 if (LOW_BORDER(mp) != BORDER_BITS) \
 malloc_error(err_low_border, func, ap); \
 if (HIGH_BORDER(mp) != BORDER_BITS) \
 malloc_error(err_high_border, func, ap);

#else

typedef union memory_struct {
     header head; /* our information bytes */
     union memory_struct *next_node; /* the free-list pointer */
#ifndef VXWORKS
     /* VxWorks does not mind the aligment on int.  */
     double not_use; /* This is to prevent from unaligned acces */
#endif
} *memory;

#define HEADSIZE() (sizeof(header))
#define NODESIZE() (HEADSIZE())
#define HEAD(p) (p)->head
#define NEXT(p) (p)->next_node

#endif

typedef struct long_memory_struct {
     header head; /* our information bytes */
     OPRS_NODE node; /* where the block is */
     struct long_memory_struct *next_node; /* next long block */
} *long_memory;


/************************************************************************
 * This is the array which will hold the list of free nodes *
 ************************************************************************/

static struct node_list {
     memory next_node;		/* free-list pointer */
     unsigned int num_in_use,	/* total nuber available */
	  num_free,		/* number currently available */
	  bytes_wasted,		/* total diff btw asked and given */
	  nmalloc,		/* times malloc was used on this size */
	  size;			/* NODESIZE() is included */
} nodes[SIZES] = {
#ifdef VXWORKS
     {NULL, 0, 0, 0, 0, 12}, 
#endif
     {NULL, 0, 0, 0, 0, 16}, 
#ifdef VXWORKS
     {NULL, 0, 0, 0, 0, 20}, 
#endif
     {NULL, 0, 0, 0, 0, 24}, 
     {NULL, 0, 0, 0, 0, 32},
     {NULL, 0, 0, 0, 0, 64},
     {NULL, 0, 0, 0, 0, 128},
     {NULL, 0, 0, 0, 0, 256},
     {NULL, 0, 0, 0, 0, 512},
     {NULL, 0, 0, 0, 0, BIGGEST_BLOCK},
     {NULL, 0, 0, 0, 0, 0} /* for big nodes */
};

#ifdef VXWORKS
#undef MALLOC_MOUCHARD
#endif

#ifdef MALLOC_MOUCHARD
#define MAX_MOUCHARD 64
int allocation[MAX_MOUCHARD];
#endif

#define long_nodes ((long_memory) nodes[ BIG_LIST ].next_node)


/************************************************************************
 * pr_mstats - print out statistics about malloc *
 ************************************************************************/

#define another_node(ptr) ptr->num_in_use++; ptr->num_free++;

static unsigned int allocated = 0,
adjust_count = 0,
adjust_size = 0;

void pr_mstats(FILE *fp, char *s)
{
#ifdef LONG_NODES
     register long_memory p;
#endif
     register int i;

#ifdef MIXED_MALLOC
     fprintf(fp, "Memory allocation statistics for %s (MIXED_MALLOC)\n", s);
#else
     fprintf(fp, "Memory allocation statistics for %s (OPRS_MALLOC)\n", s);
#endif

     for (i=0; i< LAST_LIST ; i++) {
	  if (i > 0 && nodes[i].size <= nodes[i-1].size)
	       fprintf(fp, "warning: list %d (size %d) smaller that list %d (size %d)\n",
			i, nodes[i].size, i-1, nodes[i-1].size);
	  if ((nodes[i].size % sizeof(OPRS_NODE)) != 0)
	       fprintf(fp, "warning: list %d (size %d) not pointer aligned\n",
			i, nodes[i].size);
     }

     fprintf(fp, "Sizes up to:\tin use:\tfree:\tmalloc'd:\tAvg Bytes wasted:");
     for (i=0; i < LAST_LIST; i++) {
	  fprintf(fp, "\n\t%d\t%d \t%d \t%d",
		   nodes[i].size, nodes[i].num_in_use - nodes[i].num_free,
		   nodes[i].num_free, nodes[i].nmalloc);
	  if (nodes[i].nmalloc)
	       fprintf(fp, " \t\t%d", nodes[i].bytes_wasted / nodes[i].nmalloc);
     }

     fprintf(fp, "\n<big nodes>\t%d \t%d \t%d\n",
	      nodes[BIG_LIST].num_in_use - nodes[BIG_LIST].num_free,
	      nodes[BIG_LIST].num_free, nodes[BIG_LIST].nmalloc);

#ifdef LONG_NODES
     for (p = long_nodes; p != NULL; p = p->next_node)
	  fprintf(fp, "\t0x%x, refcount(%d), size(%dK)\n",
		   p->node, p->head.refcount, p->head.which_size);
#endif
     fprintf(fp, "\nTotal Allocated = %dKbytes", allocated / BLOCK_SIZE);

     fprintf(fp, "\nRe-aligned sbrk point %d times for %d bytes",
	      adjust_count, adjust_size);

#ifdef MALLOC_MOUCHARD
     fprintf(fp, "Allocation histogram:\n");
     for (i=0; i+3 <  MAX_MOUCHARD; i=i+4) 
	  fprintf(fp,"%d: %d, %d: %d, %d: %d, %d: %d\n", 
		  i, allocation[i], i+1, allocation[i+1], 
		  i+2, allocation[i+2], i+3, allocation[i+3]);
#endif
     fprintf(fp, "\n");
}

#ifdef _OPRS_MALLOC
extern char *sbrk(int);

static OPRS_NODE morecore(register int size) /* number of bytes to get */
{ 
     register OPRS_NODE cp;
#ifdef HAS_RLIMIT
     struct rlimit rl;
#endif
#ifdef HAS_SIGSETMASK
     int old_mask;
#endif

     /* ensure that we start on BLOCK_SIZE boundaries */
#ifdef HAS_SIGSETMASK
     old_mask = sigsetmask(1 << (SIGALRM - 1));
#endif
     if ((((int) (cp = (OPRS_NODE) sbrk(0))) & BLOCK_BITS) != 0) {
	  adjust_count++;
	  adjust_size += BLOCK_SIZE_ALIGNED - (((int) cp) & BLOCK_BITS);

#ifdef SUN_BSD_BRK
	  /* on the sun, sbrk rounds up to the nearest word. go figure */
	  if (((int) cp) & (sizeof(int)-1)) {
	       if (brk((((int) cp) + sizeof(int)) & ~(sizeof(int)-1)) != 0)
		    malloc_fatal("mallocPack: could not set brk.\n")
			 else if ((((int) (cp = (OPRS_NODE) sbrk(0))) & BLOCK_BITS) != 0)
			      sbrk(BLOCK_SIZE_ALIGNED - (((int) cp) & BLOCK_BITS));
	  }
	  else
#endif
	  sbrk(BLOCK_SIZE_ALIGNED - (((int) cp) & BLOCK_BITS));
     }

     if ((int) (cp = (OPRS_NODE) sbrk(size)) == -1)  /* no more room! */

     {
#ifdef HAS_RLIMIT
	  getrlimit(RLIMIT_DATA, &rl);
	  if (rl.rlim_cur >= rl.rlim_max)
	       malloc_fatal("mallocPack: Ran out of memory.\n");
	  rl.rlim_cur = rl.rlim_max;
	  setrlimit(RLIMIT_DATA, &rl);
#if DO_CHECK
	  malloc_warn("Warning: Setting data resource limit to max.\n");
#endif
	  if ((int) (cp = (OPRS_NODE) sbrk(size)) == -1) {
#ifdef HAS_SIGSETMASK
	       sigsetmask(old_mask);
#endif
#if DO_CHECK
	       malloc_fatal("mallocPack: attempt at sbrk() failed.\n");
#else
	       return NULL;
#endif
	  }
#else
#ifdef HAS_SIGSETMASK
	  sigsetmask(old_mask);
#endif
#if DO_CHECK
	  malloc_fatal("mallocPack: attempt at sbrk() failed.\n");
#else
	  return NULL;
#endif
#endif
     }

     allocated += size; /* save the amount of memory we have allocated */
     ASSERT(!is_small(cp));

#ifdef HAS_SIGSETMASK
     sigsetmask(old_mask);
#endif
     return cp;
}
#else  /* _OPRS_MALLOC */

static OPRS_NODE morecore(int size) /* number of bytes to get */
{
     OPRS_NODE res;

     allocated += size; /* save the amount of memory we have allocated */
     /* This memory must be aligned on block boundary for macro such as "is small" to work.. */
     /* In this context, this is the standard system memalign/malloc... */
#ifdef HAVE_VALLOC
     res = (OPRS_NODE)valloc(size);
     if (is_small(res))
	  malloc_fatal("morecore: valloc returned badly aligned mem.\n");
#else
     res = (OPRS_NODE)memalign(BLOCK_SIZE_ALIGNED,size);
#endif
     if (!res)
	  malloc_fatal("morecore: cannot get more memory (block).\n");
     return res;
}


#ifdef VXWORKS
static SEM_ID malloc_sem = NULL;
#endif     
#endif /* !_OPRS_MALLOC */

OPRS_NODE _MALLOC(size_t nbytes)
{
     int nb, size;
     short index;
     long_memory lp, *lp2;

#ifdef VXWORKS
     take_or_create_sem(&malloc_sem);
#elif defined(USE_MULTI_THREAD)
     oprs_pthread_mutex_lock(&malloc_mtx);
#endif

     /* If the size needed exceeds the biggest standard sized block... */
     if ((nb = (nbytes + NODESIZE())) > BIGGEST_BLOCK) {

	  nb = nbytes;
	  size = (nb & BLOCK_BITS); /* align to 1k */
	  if (size > 0) nb += BLOCK_SIZE - size; /* for efficiency */
	  index = nb >> BLOCK_POWER; /* trash the low bits */

	  for (lp = long_nodes; lp != NULL; lp = lp->next_node)
	       if (lp->head.refcount == 0 && ((short)(lp->head.which_size) >= index))
		    break;
 
	  if (lp == NULL) {

	       another_node((&nodes[ BIG_LIST ])); /* count the nodes */
#if defined(USE_MULTI_THREAD)
	       oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	       lp = (long_memory) _MALLOC(sizeof(struct long_memory_struct));
#if defined(USE_MULTI_THREAD)
	       oprs_pthread_mutex_lock(&malloc_mtx);
#endif
	       lp->head.which_size = index;
	       if ((lp->node = morecore(nb)) == NULL) {
#ifdef VXWORKS
		    give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
		    return NULL;
	       }
	       /* insert pointer into list of long nodes */
	       for (lp2 = (long_memory *) &(nodes[BIG_LIST].next_node); ;
		    lp2 = &((*lp2)->next_node))
		    if (*lp2 == NULL || (short)((*lp2)->head.which_size) >= index) {
			 lp->next_node = *lp2;
			 (*lp2) = lp;
			 break;
		    }
	  }

	  nodes[ BIG_LIST ].num_free--;
	  nodes[ BIG_LIST ].nmalloc++;

	  lp->head.refcount = 1;
#ifdef VXWORKS
	  give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
	  oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	  return lp->node;
     } else {
	  memory save, p;
	  struct node_list *ptr;

	  /* find index */
	  for (index = 0, ptr = nodes; (index < SIZES) && ((signed)ptr->size < nb);
	       ptr++, index++) /* null loop statement */ ;

	  if (ptr->next_node == NULL) { /* get more memory */
	       /* minimum request is BLOCK_SIZE */
	       size = (ptr->size > BLOCK_SIZE) ? ptr->size : BLOCK_SIZE;

	       if ((save = p = (memory) morecore(size)) == NULL) {
#ifdef VXWORKS
		    give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
		    return NULL;
	       }

	       for (;;) { /* set up linked list of the nodes */
		    another_node(ptr); /* keep track of number of nodes */
#if DO_CHECK > 2
		    LOW_BORDER(p) = BORDER_BITS;
		    HIGH_BORDER(p) = BORDER_BITS;
#endif
		    if ((size -= ptr->size) <  (signed)ptr->size)
			 break;
		    p = NEXT(p) = (memory) (((void *) p) + ptr->size);
	       }

	       NEXT(p) = ptr->next_node; /* should be null */
	       ptr->next_node = save; /* head of free list */
	  }

	  p = ptr->next_node;
	  ptr->next_node = NEXT(p); /* update free-list */
	  HEAD(p).which_size = index; /* save the list index */
	  HEAD(p).refcount = 1; /* block is in use */
#ifdef MALLOC_MOUCHARD
	  if (nb < MAX_MOUCHARD) allocation[nb]++;
#endif
	  ptr->num_free--;
	  ptr->nmalloc++;
	  ptr->bytes_wasted += ptr->size - nb;
#if DO_CHECK > 1
	  HEAD(p).our_block = MAGIC; /* tag it for later */
#if DO_CHECK > 2
	  BAD_BORDER("malloc",p, ((int) p) + HEADSIZE());
#endif
#endif
#ifdef VXWORKS
	  give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
	  oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	  return (OPRS_NODE) (((void *) p) + HEADSIZE());
     }
}


/************************************************************************
 * dup_alloc - Duplicate a node. Actually this just increments *
 * the node's reference count *
 ************************************************************************/

void* _DUP_ALLOC(OPRS_NODE ap)	/* pointer to the user's block */
{
     register memory mp;
     register long_memory lp;

     if (ap == NULL) return NULL; /* ignore this call */

     if (is_small(ap)) { /* small sized node */

	  mp = (memory) ((char *)ap - HEADSIZE()); /* point back to header */

	  ASSERT_MAGIC(mp); /* make sure it's one of ours */

	  if (HEAD(mp).refcount < MAX_REFCOUNT) /* so we don't overflow char */
	       ++HEAD(mp).refcount; /* bump the reference count */
	  else
	       fprintf(stderr, "Warning: dup %p duplicated more than %d times.\n", ap, MAX_REFCOUNT);
     } else {
	
	  /* long_node are referenced by a list to make sure we do not swap
	     them in each time we access them... */
#ifdef VXWORKS
	  take_or_create_sem(&malloc_sem);
#elif defined(USE_MULTI_THREAD)
     oprs_pthread_mutex_lock(&malloc_mtx);
#endif
	  for (lp = long_nodes; lp != NULL; lp = lp->next_node)
	       if (lp->node == ap) break;
#ifdef VXWORKS
	  give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif 
	  ASSERT(lp != NULL);
	  if (lp->head.refcount < MAX_REFCOUNT) /* so we don't overflow char */
	       ++lp->head.refcount; /* bump the reference count */
	  else
	       fprintf(stderr, "Warning: dup %p duplicated more than %d times.\n", ap, MAX_REFCOUNT);
     }
     return ap;
}

int _ALLOC_REFCOUNT(register OPRS_NODE ap) /* pointer to the user's block */
{
     register memory mp;
     register long_memory lp;

     if (ap == NULL) return 0; /* ignore this call */

     if (is_small(ap)) { /* small sized node */

	  mp = (memory) ((char *)ap - HEADSIZE()); /* point back to header */

	  BAD_POINTER("alloc_refcount", mp, ap);/* do additional checking */

	  return HEAD(mp).refcount;
     }

#ifdef VXWORKS
     take_or_create_sem(&malloc_sem);
#elif defined(USE_MULTI_THREAD)
     oprs_pthread_mutex_lock(&malloc_mtx);
#endif
     for (lp = long_nodes; lp != NULL; lp = lp->next_node)
	  if (lp->node == ap) break;
#ifdef VXWORKS
     give_sem(malloc_sem);		    
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif

     ASSERT(lp != NULL);

     return lp->head.refcount;
}

void _FREE(OPRS_NODE ap) /* pointer to the user's block */
{
     memory mp;
     long_memory lp;
     struct node_list *ptr;

     if (ap == NULL) return; /* ignore this call */

     if (is_small(ap)) { /* small sized node */

	  mp = (memory) ((char *)ap - HEADSIZE()); /* point back to header */

	  BAD_POINTER("free", mp, ap); /* do additional checking */
#if DO_CHECK > 2
	  BAD_BORDER("free", mp, ap); /* check for user errors */
#endif

	  if (HEAD(mp).refcount == MAX_REFCOUNT) return; /* in use */
	  if (--HEAD(mp).refcount > 0) return; /* still in use */
	  ASSERT(HEAD(mp).which_size < (unsigned short) BIG_LIST); /* a valid list index */

#ifdef VXWORKS
	  take_or_create_sem(&malloc_sem);
#elif defined(USE_MULTI_THREAD)
     oprs_pthread_mutex_lock(&malloc_mtx);
#endif
	  ptr = &(nodes[ HEAD(mp).which_size ]); /* get list structure */

	  NEXT(mp) = ptr->next_node;
	  ptr->next_node = mp;
	  ptr->num_free++;

#ifdef VXWORKS
	  give_sem(malloc_sem);
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif

	  return;
     }
     /* we do not need the semaphore in this case */

#ifdef VXWORKS
     take_or_create_sem(&malloc_sem);
#elif defined(USE_MULTI_THREAD)
     oprs_pthread_mutex_lock(&malloc_mtx);
#endif
     for (lp = long_nodes; lp != NULL; lp = lp->next_node)
	  if (lp->node == ap) break;

     ASSERT(lp != NULL);

     if (lp->head.refcount == MAX_REFCOUNT) {
#ifdef VXWORKS
	  give_sem(malloc_sem);
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	  return; /* in use */
     }
#if DO_CHECK > 1
     if (lp->head.refcount == 0) {
	  fprintf(stderr, free_err, "free", ap);
#ifdef VXWORKS
	  give_sem(malloc_sem);
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	  return;
     }
#endif

     if (--lp->head.refcount != 0){
#ifdef VXWORKS
	  give_sem(malloc_sem);
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
	  return; /* it is freed */
     }
     nodes[ BIG_LIST ].num_free++;
#ifdef VXWORKS
     give_sem(malloc_sem);
#elif defined(USE_MULTI_THREAD)
		    oprs_pthread_mutex_unlock(&malloc_mtx);
#endif
     return;
}

#ifdef VXWORKS
#define size_of_node(val, ap) \
	if (is_small(ap)) { /* small sized node */ \
	register memory mp; /* temp. register */ \
	mp = (memory) (ap - HEADSIZE()); /* point to header */ \
	ASSERT_MAGIC(mp); /* it's one of ours? */ \
	ASSERT(HEAD(mp).which_size < (unsigned short)BIG_LIST); /* valid list index? */ \
	val = nodes[ HEAD(mp).which_size ].size - NODESIZE(); \
	} \
	else { /* big-list node */ \
	register long_memory lp; /* temp. register */ \
	take_or_create_sem(&malloc_sem); \
	for (lp = long_nodes; lp != NULL; lp = lp->next_node) \
	if (lp->node == ap) break; /* found the record? */ \
	give_sem(malloc_sem); \
	ASSERT(lp != NULL); /* check to be sure */ \
	val = ((lp->head.which_size) << BLOCK_POWER); /* load the value */ \
	}
#elif defined(USE_MULTI_THREAD)
#define size_of_node(val, ap) \
	if (is_small(ap)) { /* small sized node */ \
	register memory mp; /* temp. register */ \
	mp = (memory) (ap - HEADSIZE()); /* point to header */ \
	ASSERT_MAGIC(mp); /* it's one of ours? */ \
	ASSERT(HEAD(mp).which_size < (unsigned short)BIG_LIST); /* valid list index? */ \
	val = nodes[ HEAD(mp).which_size ].size - NODESIZE(); \
	} \
	else { /* big-list node */ \
	register long_memory lp; /* temp. register */ \
        oprs_pthread_mutex_lock(&malloc_mtx); \
	for (lp = long_nodes; lp != NULL; lp = lp->next_node) \
	if (lp->node == ap) break; /* found the record? */ \
	oprs_pthread_mutex_unlock(&malloc_mtx); \
	ASSERT(lp != NULL); /* check to be sure */ \
	val = ((lp->head.which_size) << BLOCK_POWER); /* load the value */ \
	}
#else
#define size_of_node(val, ap) \
	if (is_small(ap)) { /* small sized node */ \
	register memory mp; /* temp. register */ \
	mp = (memory) (ap - HEADSIZE()); /* point to header */ \
	ASSERT_MAGIC(mp); /* it's one of ours? */ \
	ASSERT(HEAD(mp).which_size < (unsigned short)BIG_LIST); /* valid list index? */ \
	val = nodes[ HEAD(mp).which_size ].size - NODESIZE(); \
	} \
	else { /* big-list node */ \
	register long_memory lp; /* temp. register */ \
	for (lp = long_nodes; lp != NULL; lp = lp->next_node) \
	if (lp->node == ap) break; /* found the record? */ \
	ASSERT(lp != NULL); /* check to be sure */ \
	val = ((lp->head.which_size) << BLOCK_POWER); /* load the value */ \
	}

#endif
unsigned alloc_size(register OPRS_NODE ap)
{
     register unsigned temp;

     if (ap == NULL) return 0;

     size_of_node(temp, (void *)ap);

     return temp;
}

OPRS_NODE _REALLOC(OPRS_NODE ap, size_t nbytes)
{
     OPRS_NODE res;
     unsigned int size;

     if (ap == NULL) return _MALLOC(nbytes);

     /* treat this as a no-op because some applications think *
      * reallocing to 0 should keep the storage around */
     if (nbytes == 0) return ap;

     size_of_node(size, (void *)ap); /* how big is it? */

     if ((res = _MALLOC(nbytes)) != NULL)
	  BCOPY(ap, res, (nbytes > size) ? size : nbytes);

     _FREE(ap);

     return res;
}
/* void *malloc1(unsigned nbytes);*/

OPRS_NODE copy_alloc(register OPRS_NODE ap)
{
     register OPRS_NODE res;
     register unsigned int onb;

     if (ap == NULL) return NULL;

     size_of_node(onb, (void *)ap);

     if ((res = malloc1(onb)) == NULL) return(NULL);

     BCOPY(ap, res, (int) onb);

     return res;
}

OPRS_NODE _CALLOC(size_t number, size_t nbytes)
{
     register OPRS_NODE p;
     register unsigned int onb;

     onb = number * nbytes; /* number of bytes to be allocated */
     if ((p = _MALLOC(onb)) == NULL) return NULL;

     BZERO(p, onb);
     return p;
}

#if defined(DEFINE_MEMALIGN) && defined(_OPRS_MALLOC)

OPRS_NODE memalign(unsigned align, unsigned size)
{
     register OPRS_NODE cp, *temp, first = 0;
     register int mask = (align-1);

     /* we normally align everthing here, anyway */
     if (align < 8) return _MALLOC(size);

     /* for now, force all requests to be for > 1k blocks */
     if (size < BLOCK_SIZE_ALIGNED) size = BLOCK_SIZE_ALIGNED;

     /* make sure that there are big-list pointers available */
     _FREE(_MALLOC(sizeof(struct long_memory_struct)));

     /* while the sbrk point is improperly aligned */
     while ((((int) (cp = (OPRS_NODE) sbrk(0))) & mask) != 0) {

	  /* get another 1k block, and chain it on */
	  temp = (OPRS_NODE *) _MALLOC(BLOCK_SIZE_ALIGNED);
	  *temp = first;
	  first = (OPRS_NODE) temp;

	  /* make sure that there are big-list pointers available */
	  _FREE(_MALLOC(sizeof(struct long_memory_struct)));
     }

     /* get the requested memory */
     cp = _MALLOC(size);

     /* free the chain */
     while (first != 0) {
	  _FREE(first);
	  first = * ((OPRS_NODE *) first);
     }

     return cp;
}


#ifndef HAVE_VALLOC
OPRS_NODE valloc(unsigned size)
{
     return memalign(getpagesize(), size);
}
#endif

#endif

#if defined(DEBUG_MAIN_MALLOC) && defined(_OPRS_MALLOC)

#define BLOCKS 1500

main() 
{
     OPRS_NODE my_nodes[ BLOCKS ], from_malloc, from_calloc, calloc();
     int i, j;

     /* make sure we're the only library running */
     from_malloc = _MALLOC(1);
     _FREE(from_malloc);
     from_calloc = _CALLOC(1, 1);
     _FREE(from_calloc);
     if (from_malloc != from_calloc)
	  fprintf(stdout, "Warning: malloc() and calloc() not using same heap\n");

     for (i=0; i<BLOCKS; i++) {
	  my_nodes[i] = _MALLOC(i);
	  for (j=0; j<i; j++) my_nodes[i][j] = j;
     }
     pr_mstats(stdout, "After initial allocation");

     for (i=0; i<BLOCKS; i+=2)
	  _FREE(my_nodes[i]);
     pr_mstats(stdout, "After partial freeing");

     for (i=0; i<BLOCKS; i+=2) {
	  my_nodes[i] = _MALLOC(i);
	  for (j=0; j<i; j++) my_nodes[i][j] = j;
     }
     pr_mstats(stdout, "After second allocation (should be same as first)");
 
     for (i=0; i<BLOCKS; i++)
	  _FREE(my_nodes[i]);
     pr_mstats(stdout, "After final free (everything free except 16's)");
}
#endif

#ifdef _OPRS_MALLOC
void *malloc1(unsigned nbytes)
{
     return malloc(nbytes);
}

void *realloc1(void *ptr, unsigned nbytes)
{
     return realloc(ptr,nbytes);
}

void free1(void *mem)
{
     free(mem);
}

#endif /* _OPRS_MALLOC */
#endif /* STD_MALLOC */
