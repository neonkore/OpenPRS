/*                               -*- Mode: C -*- 
 * shashPackTest.c -- 
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
#include "shashPack.h"
#include "shashPack_f.h"

#define HashNum	256			/* maximum hash entry */
#define	NUM	1000

struct foo {
     int dummy;
     char name[80];
} names[NUM];


static int hash_name(char *str)
{
     register int i;

     for (i = 0; *str != '\0'; i += *str++);

     return (i & 0xff);
}


static match_segments(char *str,  struct foo *str2)
{
     return strcmp(str, str2->name) == 0;
}


static print_name(char *dummy, struct foo *str)
{
     printf("String '%s'\n", str->name);
}

main() 
{
     char temp[80],
	  *format = "hash entry #%d";
     Shash *hash;
     int i, j;
     struct foo *str;

     for (i=0; i<NUM; i++) sprintf(names[i].name, format, i);

     printf("making a hash table\n");
     hash = sh_make_hashtable(HashNum, hash_name, match_segments);

     printf("adding hash entries\n");
     for (i=0; i<NUM; i++)
	  sh_add_to_hashtable(hash, &names[i], names[i].name);
 
     printf("checking hash entries\n");
     sh_for_all_hashtable(hash, NULL, print_name);

     for (i=0; i<10; i++) {
	  printf("Retrieve entry #");
	  scanf("%d", &j);
	  sprintf(temp, format, j);
	  if ((str = (struct foo *) sh_get_from_hashtable(hash, temp)) == NULL)
	       printf("Entry #%d not found\n", j);
	  else
	       printf("Entry #%d is '%s'\n", j, str->name);
     }

     for (i=0; i<10; i++) {
	  printf("Delete entry #");
	  scanf("%d", &j);
	  sprintf(temp, format, j);
	  if ((str = (struct foo *) sh_delete_from_hashtable(hash, temp)) == NULL)
	       printf("Entry #%d not found\n", j);
	  else
	       printf("Entry #%d is '%s'\n", j, str->name);
     }

     printf("deleting hash entries\n");
     for (i=0; i<NUM; i++)
	  sh_delete_from_hashtable(hash, names[i].name);

     sh_free_hashtable(hash);

     mstats(stdout, "after freeing table");
}


