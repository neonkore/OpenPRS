static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * test-malloc.c -- USed to test the speed of an malloc library.
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

/* 
 * This small utility is done to test the speed of a particular malloc library
 */

#include "config.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

#include "slistPack.h"
#include "slistPack_f.h"

typedef struct timeval Date;

void time_stamp(Date *pd)
{
     struct timezone tzp;

     if (gettimeofday(pd,&tzp) != 0)
	  perror("gettimeofday");
}

main()
{
     Date start;
     Date stop;
     long sec, usec;

     time_stamp(&start);

     do_malloc_test();

     time_stamp(&stop);
     sec = stop.tv_sec - start.tv_sec;
     usec = stop.tv_usec - start.tv_usec;
     if (usec < 0L) {
	  sec -= 1;
	  usec += 1000000L;
     }

     fprintf(stderr, "-> %ld sec, %ld usec.\n", sec, usec);
     mstats(stderr,"OPRS Malloc functions.");
}

int table[10][2] = {
     { 8, 	10000},
     { 16, 	10000},
     { 24,      20000},
     { 32, 	10000},
     { 64, 	8000},
     { 128, 	6000},
     { 256, 	4000},
     { 512, 	2000},
     { 1300, 	400},
     {0 , 	0}
};

do_malloc_test()
{
     void *ptr;
     int i = 0, k, j;
     Slist *alloced_list = sl_make_slist();


     while (table[i][0] != 0) {
	  
	  int size = table[i][0];
	  int nombre = table[i][1];
	  
	  for (j = 0 ; j < 100; j++) {
	       if (j == 0)
		    fprintf(stderr, "Allocating %d blocks of %d bytes...", nombre, size);
	       else
		    fprintf(stderr, "+");

	       for (k = 0 ; k < nombre; k++)
		    sl_add_to_tail(alloced_list, malloc(size));

	       if (j == 0) 
		    fprintf(stderr, "Freeing them...\n");
	       else 
		    fprintf(stderr, "-");
	       while (ptr = sl_get_from_head(alloced_list))
		    free(ptr);
	  }

	  fprintf(stderr, "\n");
	  i++;
     }
}
