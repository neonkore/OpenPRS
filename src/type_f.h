/*                               -*- Mode: C -*- 
 * type_f.h -- 
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

Type *make_type(Symbol name, Symbol stype);
void make_and_print_type(Symbol name, Symbol stype);
SymList elts_of_type(Symbol stype);
void free_type(Type *t);
void print_type(Type *t);
void show_type(Symbol stype);

void set_type_by_name(Symbol stype, Symbol sym);
void set_type(Type *type, Symbol sym);
void set_var_type_by_name(Symbol stype, Envar *var);

PBoolean is_of_type_sym(Symbol sym, Symbol stype);
PBoolean is_of_type(Symbol sym, Type *type);
SymList elts_of_type(Symbol stype);
Symbol type_of(Symbol sym);
Term *build_id_from_type(Type *t);
TypeList surtypes_of_type(Symbol stype);

PBoolean eq_stype_type(Symbol stype, Type *t);
void set_type_from_reload(Type *type, Symbol sym);

void free_type_hash(void);
void init_hash_size_type(int hash_size);
void make_type_hash();
