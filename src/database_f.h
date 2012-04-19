/*                               -*- Mode: C -*- 
 * database_f.h -- Declaration des fonctions externes a database.c
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

/* enzo */
#include "database.h"


void init_hash_size_database(int hash_size);
Database *make_database(void);
void conclude(Expression *expr,Database *db);
void delete_expr(Expression *expr,Database *db);
void delete_fact_expr(Expression *expr,Database *db);
void delete_basic_events(Oprs *oprs);
void parser_consult(Expression *expr, Database *db);
void parser_conclude(Expression *expr,Database *db);
ExprFrameList consult(Expression *expr, Frame *frame, Database *db, PBoolean fact_res);
void clear_database(Database *db);
void stat_database(Database *db);
void print_database(Database *db);
void save_database(Database *db, PString filename);
void dump_database_from_parser(Database *db, PString filename);
void load_database_from_parser(Database *db, PString file_name);

void sprint_database(Sprinter *sp, Database *db);
int collect_exprs(ExprList exprs_to_print, Key_Elt *ko);
void free_database(Database *db);
ExprList collect_expr_mentioning_term(Database *db, Term *term);
ExprList collect_expr_mentioning_term_2(Database *db, Term *term);
ExprList get_all_expr_from_db(Database *db);

/* enzo */
ExprFrameList consult_expr(Expression*, Frame*, Database*, PBoolean);
