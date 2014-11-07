/*                               -*- Mode: C -*-
 * database.h -- Database structures and declarations.
 *
 * Copyright (c) 1991-2014 Francois Felix Ingrand.
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


#ifndef INCLUDE_database
#define INCLUDE_database

#include <shashPack.h>
#include "opaque.h"

struct database {
     Shash *table;			  /* La hash table elle meme */
#ifdef OPRS_PROFILING
     unsigned int nb_consult;
     unsigned int nb_delete;
     unsigned int nb_assert;
#endif
};

typedef enum {
     KEY_KEY, KEY_TERM		/* Do not add anything unless you increase the width of the bit field. */
}    Key_Type;

struct key {
     union {
	  Key *subkey;
	  Term *term;
     }     u;
     Symbol pred;
     unsigned int ar;
     unsigned int pos;
     Key_Type key_or_term BITFIELDS(:1);	/* 2 values only... */
};

typedef Slist *Key_List;
typedef Slist *Key_Elt_List;

struct key_elt {			  /* Ce sont les elements de la hash table */
     Key *key;
     ExprList statements;
     unsigned short nombre;
};

typedef struct be_clean BE_Clean;

struct be_clean {
     Expression *expr;
     Key_Elt_List ko_list;
}; 

#endif	/* INCLUDE_database */
