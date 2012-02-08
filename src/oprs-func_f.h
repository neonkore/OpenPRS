/*                               -*- Mode: C -*- 
 * oprs-func_f.h -- 
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

#include "oprs-type_f-pub.h"

#define FUNC_NAME(func_rec) ((func_rec)->name)

#define DECLARE_FUNC_EF(func_rec,evf) ((func_rec)->u.ef = (evf))

#define DECLARE_ACT_EF(func_rec,evf) ((func_rec)->act = (evf))

Func_Rec *create_var_func(Envar *var);
Func_Rec *make_func_rec(Symbol name);
Func_Rec *find_or_create_func(Symbol func_name);
Func_Rec *find_func(Symbol func_name);
void make_func_hash(void);
void free_func_hash(void);
void init_hash_size_func(int hash_size);
void stat_func_hashtable(void);
Term *evaluate_term_function(Eval_Funct *ef, char *ef_name, TermList tl);
Term *evaluate_term_action(Action *ef, char *ac_name, TermList tl);

