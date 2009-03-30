/*                               -*- Mode: C -*- 
 * shashPack_f.h -- 
 * 
 * $Id$
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

#include "shashPack.h"

/*
 * These macros are just here to cast function type.
 */

#define	sh_make_hashtable(size, hashfunc, matchfunc)	\
	_sh_make_hashtable(size, hashfunc, matchfunc)

#define sh_for_all_hashtable(table, node, func)		\
	_sh_for_all_hashtable(table, node, func)

#define sh_for_all_2hashtable(table, node, node2, func)	\
	_sh_for_all_2hashtable(table, node, node2, (SL_PFI) (func))

#define sh_for_all_3hashtable(table, node, node2, node3, func)	\
	_sh_for_all_3hashtable(table, node, node2, node3, (SL_PFI) (func))

#define sh_hashtable_empty(table)	(sh_size_of_hashtable(table) == 0)

#define sh_size_of_hashtable(table)	((table)->number)

Shash *_sh_make_hashtable(int size, SL_PFI hashfunc, SL_PFC matchfunc);

void sh_free_hashtable(Shash *hash);
void sh_flush_hashtable(Shash *hash);

int _sh_for_all_hashtable(Shash *table, void *node, SL_PFI func);
int _sh_for_all_2hashtable(Shash *table, void *node,void *node2, SL_PFI func);
int _sh_for_all_3hashtable(Shash *table, void *node,void *node2,void *node3, SL_PFI func);

void *sh_add_to_hashtable(Shash *table, void *node, void *key);
void *sh_delete_from_hashtable(Shash *table, void *key);
void *sh_get_from_hashtable(Shash *table, void *key);

#define make_hashtable(size, hashfunc, matchfunc) sh_make_hashtable(size, hashfunc, matchfunc)
#define free_hashtable(hash) sh_free_hashtable(hash)
#define for_all_hashtable(table, node, func) sh_for_all_hashtable(table, node, func)
#define for_all_2hashtable(table, node,node2, func) sh_for_all_2hashtable(table, node,node2, func)
#define for_all_3hashtable(table, node,node2,node3, func) sh_for_all_3hashtable(table, node,node2,node3, func)
#define add_to_hashtable(table, node, key) sh_add_to_hashtable(table, node, key)
#define delete_from_hashtable(table, key) sh_delete_from_hashtable(table, key)
#define get_from_hashtable(table, key) sh_get_from_hashtable(table, key)
