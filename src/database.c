/*                               -*- Mode: C -*-
 * database.c -- Database management functions.
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

/* **************************************************************
 * The database stores only simple expression (expr), but can be used to retrieve complexe gexpression
 * with logical operator. It uses term indexing, and has many features to handle closed world predicate
 * functional fact, etc... Use the source Luke.
 * ************************************************************** */

/* 
 * Most (if not all) the leaks are now fixed. All the leak test, 
 * go through without any leak appearing... However, never say 
 * never... (FFI 1/22/92)
 */

#include "macro.h"

#include "shashPack_f.h"

#ifdef OPRS_PROFILING
#endif

#include "database.h"
#include "oprs-type.h"
#include "oprs.h"
#include "oprs-sprint.h"
#include "oprs-dump.h"
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-pred-func.h"
#include "oprs-pred-func_f.h"  /* enzo */
#include "oprs-main.h"
#include "ev-predicate.h"


#include "database_f.h"
#include "oprs-type_f.h"
#include "oprs-array_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "oprs-error_f.h"
#include "oprs-dump_f.h"
#include "oprs-rerror_f.h"
#include "unification_f.h"
#include "ev-predicate_f.h"
#include "lisp-list_f.h"

#define DATABASE_HASHTABLE_MINSIZE 64
#define DATABASE_HASHTABLE_DEFAULTSIZE 1024 /* has to be a power of 2 for the bitwise modulo */
static int database_hashtable_size, database_hashtable_mod;

Key_List make_key_elt(Symbol p, int a, int i, Term *t, PBoolean var_allowed, List_Envar *dup_var_list);
Key_List delete_expr_from_conclude_expr(Expression *expr, Database *db);
Key_List revert_expr_from_conclude_expr(Expression *expr, Database *db);
Key_List delete_ff_expr(Expression *expr, int ff,Database *db);

List_Envar locked_local_var = NULL;
static PBoolean concluding = FALSE;
static Expression *expr_being_concluded = NULL;
static PBoolean variables_have_literal_semantic = FALSE;

jmp_buf key_computation_jmp;

PBoolean eq_expr(Expression *e1, Expression *e2)
/* ex_expr -- This is a pointer comparison. */
{
     return (e1 == e2);
}

int hash_a_term(Term *t)
/*  
 *  hash_a_term - computes the hash value for a term... Some are 
 *                standard functions other have to be adjusted.
 */
{
     switch (t->type) {
     case INTEGER:
	  return t->u.intval;
     case LONG_LONG:
	  return (int)t->u.llintval;
     case INT_ARRAY:
	  return (int)t->u.int_array;
     case FLOAT_ARRAY:
	  return (int)t->u.float_array;
     case TT_FACT:
     case TT_GOAL:
     case TT_OP_INSTANCE:
     case TT_INTENTION:
     case U_MEMORY:
     case U_POINTER:
	  return (int)t->u.u_pointer;
     case TT_FLOAT:
	  return ((int) (*t->u.doubleptr));
     case STRING:
	  return hash_a_string(t->u.string);
     case LISP_LIST:
	  return L_LENGTH(t->u.l_list);
     case C_LIST:
	  return sl_slist_length(t->u.c_list);
     case TT_ATOM:
	  return hash_a_string(t->u.id);
     case VARIABLE:		/* These are literals... */
	  return 37;		/* why 37? why not? */
/*      case LENV: */
     default:
	  oprs_perror("hash_a_term", PE_UNEXPECTED_TERM_TYPE);
	  return 0;
     }
}

int db_hash_func(Key *key)
/* db_hash_func - computes the hash value for a key. */
{
     int hash;

     /* hash = hash_a_string(key->pred); */
     hash = (int)(key->pred);	/* This is the adress of the string pred... */
     hash += key->ar;
     if (key->pos) {
	  hash += key->pos;
	  if (key->key_or_term == KEY_KEY)
	       hash += db_hash_func(key->u.subkey);
	  else
	       hash += hash_a_term(key->u.term);
     }

     return hash & database_hashtable_mod;
}

static PBoolean key_term_cmp(Term *t1, Term *t2)
/* 
 * key_term_cmp - does a term comparison (not an unification)... nothing 
 *            particular, mostly == except for strings.
 */
{
     if (t1->type != t2->type)
	  return FALSE;
     else {
	  switch (t1->type) {
	  case INTEGER:
	       return ((t1->u.intval) == (t2->u.intval));
	       break;
	  case LONG_LONG:
	       return ((t1->u.llintval) == (t2->u.llintval));
	       break;
	  case INT_ARRAY:
	       return (equal_int_array(t1->u.int_array,t2->u.int_array));
	       break;
	  case FLOAT_ARRAY:
	       return (equal_float_array(t1->u.float_array,t2->u.float_array));
	       break;
	  case TT_FACT:
	  case TT_GOAL:
	  case TT_OP_INSTANCE:
	  case TT_INTENTION:
	  case U_MEMORY:
	  case U_POINTER:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	       break;
	  case TT_FLOAT:
	       return ((*t1->u.doubleptr) == (*t2->u.doubleptr));
	       break;
	  case LISP_LIST:
	       return (equal_l_list_literal_var(t1->u.l_list,t2->u.l_list));
	       break;
	  case C_LIST:
	       return ((t1->u.c_list) == (t2->u.c_list));
	       break;
	  case STRING:
	       return (strcmp(t1->u.string, t2->u.string) == 0);
	       break;
	  case TT_ATOM:
	       return (strcmp(t1->u.id,t2->u.id) == 0);
	       break;
	  case VARIABLE:	/* In this context (variable literal semantic) */
	       return TRUE;	/* variable are always equal... */
	       break;		/* However, this should not happend has a consult variable */
				/* as a variable (not literal) semantic...(FFI 4/4/97) */
	  default:
	       oprs_perror("key_term_cmp", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
	  return (FALSE);
     }
}

PBoolean key_cmp(Key *k1, Key *k2) /* OPTIMIZE if possible */
/* key_cmp - compares two keys */
{
     if ((k1->pred == k2->pred) && (k1->ar == k2->ar) && (k1->pos == k2->pos))
	  if (k1->pos == 0)
	       return TRUE;
	  else if (k1->key_or_term == k2->key_or_term)
	       if (k1->key_or_term == KEY_KEY)
		    return key_cmp(k1->u.subkey, k2->u.subkey);
	       else
		    return key_term_cmp(k1->u.term, k2->u.term);
	  else
	       return FALSE;
     else
	  return FALSE;
}

PBoolean db_match_func(Key *key, Key_Elt *elt)
/* db_match_func - That's the database hash table matching function. */
{
     return key_cmp(key, elt->key);
}


void init_hash_size_database(int hash_size)
{
     if(hash_size == 0)
	  database_hashtable_size = DATABASE_HASHTABLE_DEFAULTSIZE;
     else if(hash_size < DATABASE_HASHTABLE_MINSIZE) {
	  database_hashtable_size = DATABASE_HASHTABLE_MINSIZE;
	  printf (LG_STR("The size of the database hashtable is: %d\n",
			 "La taille de la table de hachage de la base de faits est: %d\n"), database_hashtable_size);
     } else {
	  int i, size = hash_size -1;
	  for (i = 0; size >0; size = size>>1, i++); 
	  database_hashtable_size = 1<<i;
	  printf (LG_STR("The size of the database hashtable is: %d\n",
			  "La taille de la table de hachage de la base de faits est: %d\n"), database_hashtable_size);
     }

     database_hashtable_mod =  database_hashtable_size - 1;
}

Database *make_database(void)
/* make_database - create a database, linked to a oprs agent. */
{
     Database *db = MAKE_OBJECT(Database);

     db->table = sh_make_hashtable(database_hashtable_size, db_hash_func, db_match_func);
#ifdef OPRS_PROFILING
     db->nb_consult = 0;
     db->nb_delete = 0;
     db->nb_assert = 0;
#endif
     return db;
}

Key *make_key_all(Symbol p, int n)
/* Create a key for "all". (p a b) -> p 2 */
{
     Key *key = MAKE_OBJECT(Key);

     key->pred = p;
     key->ar = n;
     key->pos = 0;		/* This is the way I say it is a all key. */

     return key;
}

Key_List make_key_list_term(Expression *t, PBoolean var_allowed, List_Envar *dup_var_list)
/* Return the key list for a composed term */
{
     Key_List kl;
     int i = 1, n;
     Symbol p;
     Term *tt;

     kl = sl_make_slist();
     n = sl_slist_length(t->terms);
     p = t->pfr->name;
     sl_loop_through_slist(t->terms, tt, Term *) {
	  sl_concat_slist(kl, make_key_elt(p, n, i, tt, var_allowed, dup_var_list));
	  i++;
     }
     sl_add_to_tail(kl, make_key_all(p, n));
     return kl;
}

Key_List make_key_elt(Symbol p, int a, int i, Term *t, PBoolean var_allowed, List_Envar *dup_var_list)
{
     Key *key2;
     Key_List kl2, kl;

     kl = sl_make_slist();
     if (t->type == EXPRESSION) { /* if it is a term composed, we make "recursive" keys. */
	  kl2 = make_key_list_term(t->u.expr, var_allowed, dup_var_list);
	  sl_loop_through_slist(kl2, key2, Key *) {

	       Key *key = MAKE_OBJECT(Key);

	       key->pred = p;
	       key->ar = a;
	       key->pos = i;
	       key->key_or_term = KEY_KEY;
	       key->u.subkey = key2;
	       sl_add_to_head(kl, key);
	  }
	  FREE_SLIST(kl2);
     } else if (t->type != VARIABLE ||  /* Variable are "special". If deleting_expr_from_key */
		variables_have_literal_semantic) { /* or concluding. variable with a literal semantic. */
	  Key *key = MAKE_OBJECT(Key); 		   

	  key->pred = p;
	  key->ar = a;
	  key->pos = i;
	  key->key_or_term = KEY_TERM;
	  key->u.term = dup_term(t);
	  sl_add_to_head(kl, key);
     } else if (! var_allowed) { /* It is a variable */
				/* I am not sure we will ever come here again. */
	  fprintf(stderr, LG_STR("ERROR: make_key_elt: making a key of a variable\n\
\twhile it is not allowed,\n\
\tprobably while concluding the expr: ",
				 "ERREUR: make_key_elt: Interdit de construire une clef\n\
\tpour une variable dans le contexte courant,\n\
\tde conclusion de l'expression: "));
	  fprint_expr(stderr,expr_being_concluded);
	  fprintf(stderr, ".\n");
	  longjmp(key_computation_jmp,-1);
     } else {		/* It is a variable, and we allow them with a variable semantic. */
	  if (dup_var_list && *dup_var_list) {  /* We are looking for duplicates, and not found one yet. */
	       if (sl_in_slist(*dup_var_list,t->u.var)){ /* we found one */
		    FREE_SLIST(*dup_var_list);
		    *dup_var_list = NULL; /* Stop looking for them. one is enough */
	       } else {
		    sl_add_to_head(*dup_var_list,t->u.var);
	       }
	  }
     }
     return kl;
}

void free_key(Key *key)
/* Free a key. */
{
     if (LAST_REF(key) && key->pos) {
	  if (key->key_or_term == KEY_KEY)
	       free_key(key->u.subkey);
	  else
	       free_term(key->u.term);
     }
     FREE(key);
}

Key *dup_key(Key *key)
/* Dup a key. Not use for now... but you never know. */
{
    return DUP(key);
}

void fprint_key(FILE *f, Key *key)
{
     fprintf(f," %s %i ", key->pred, key->ar);
     if (key->pos) {
	  fprintf(f,"%i ", key->pos);
	  if (key->key_or_term == KEY_KEY)
	       fprint_key(f,key->u.subkey);
	  else
	       fprint_term(f,key->u.term);
     }
}
     
void print_key(Key *key)
{
     fprint_key(stdout, key);
}

Key_List make_ff_arg_key_list(Expression *expr, int ff)
/* return the key list for the argument part of a functional fact expression. */
{
     Key_List kl;
     int i = 1, n;
     Symbol p;
     Key *key;
     Term *t;

     /* Processing */
     kl = sl_make_slist();
     n = sl_slist_length(expr->terms);
     p = expr->pfr->name;
     sl_loop_through_slist(expr->terms, t, Term *) {
	  if (i > ff) {sl_deprotect_loop(expr->terms);break;}
	  sl_concat_slist(kl, make_key_elt(p, n, i, t, FALSE, NULL));
	  i++;
     }

     sl_add_to_tail(kl, make_key_all(p, n));

     if (debug_trace[DATABASE_KEY]) {
	  printf(LG_STR("The ff args keys are: ",
			"Les clefs des arguments du ff sont: "));
	  sl_loop_through_slist(kl, key, Key *) {
	       print_key(key);
	       printf(", ");
	  }
	  printf("\n");
     }
     return kl;
}

Key_List make_ff_val_key_list(Expression *expr, int ff)
/* return the key list for the value part of a functional fact expression. */
{
     Key_List kl;
     int i = 1, n;
     Symbol p;
     Key *key;
     Term *t;

     /* Processing */
     kl = sl_make_slist();
     n = sl_slist_length(expr->terms);
     p = expr->pfr->name;
     sl_loop_through_slist(expr->terms, t, Term *) {
	  if (i > ff )
	       sl_concat_slist(kl, make_key_elt(p, n, i, t, FALSE, NULL));
	  i++;
     }

     if (debug_trace[DATABASE_KEY]) {
	  printf(LG_STR("The ff vals keys are: ",
			"Les clefs des arguments du ff sont: "));
	  sl_loop_through_slist(kl, key, Key *) {
	       print_key(key);
	       printf(", ");
	  }
	  printf("\n");
     }
     return kl;
}



Key_List make_key_list(Expression *expr, PBoolean var_allowed, List_Envar *dup_var_list)
/* return the key list for an expression. */
{
     Key_List kl;
     int i = 1, n;
     Symbol p;
     Key *key;
     Term *t;

     /* Processing */
     kl = sl_make_slist();
     n = sl_slist_length(expr->terms);
     p = expr->pfr->name;
     sl_loop_through_slist(expr->terms, t, Term *) {
	  sl_concat_slist(kl, make_key_elt(p, n, i, t, var_allowed, dup_var_list));
	  i++;
     }

     sl_add_to_tail(kl, make_key_all(p, n));

     if (debug_trace[DATABASE_KEY]) {
	  printf(LG_STR("The keys are: ",
			"Les clefs sont: "));
	  sl_loop_through_slist(kl, key, Key *) {
	       print_key(key);
	       printf(", ");
	  }
	  printf("\n");
     }
     return kl;
}

int print_hash_bucket(void *ignore, Key_Elt *ko)
{
     Expression *expr;


     printf(LG_STR("The key: ",
		   "La clef: "));
     print_key(ko->key);
     printf(LG_STR("contains ",
		   "contient "));
     sl_loop_through_slist(ko->statements, expr, Expression *)
	  print_expr(expr);

     printf(".\n");

     return 1;
}

void print_database_key(Database *db)
{
     sh_for_all_hashtable(db->table, NULL, print_hash_bucket);
}

int collect_expr_term_bucket(ExprList el, Term *term, Key_Elt *ko)
{
     Expression *expr;
     Key *key = ko->key;

     while (key->pos && (key->key_or_term == KEY_KEY))
	  key = key->u.subkey;

     if (key->pos && equal_term(term,key->u.term))
	  sl_loop_through_slist(ko->statements, expr, Expression *)
	       if (! sl_in_slist(el,expr))
		    sl_add_to_tail(el,expr);

     return 1;
}

int collect_expr_term_bucket_2(ExprList el, Term *term, Key_Elt *ko)
{
     Expression *expr;
     Key *key = ko->key;

     while (key->pos && (key->key_or_term == KEY_KEY))
	  key = key->u.subkey;

     if (key->pos && equal_term(term,key->u.term)) {
	  sl_loop_through_slist(ko->statements, expr, Expression *)
	       if (! sl_in_slist(el,expr))
		    sl_add_to_tail(el,expr);
     } else {
	  if ((key->pos == 0) && (term->type == TT_ATOM) && (term->u.id == key->pred)) 
	       sl_loop_through_slist(ko->statements, expr, Expression *)
		    if (! sl_in_slist(el,expr))
			 sl_add_to_tail(el,expr);
	  
     }
     return 1;
}

ExprList collect_expr_mentioning_term(Database *db, Term *term)
{
     Expression *expr;
     ExprList el = sl_make_slist();
     ExprList res = sl_make_slist();

     sh_for_all_2hashtable(db->table, el, term, collect_expr_term_bucket);

     sl_loop_through_slist(el, expr, Expression *)
	  sl_add_to_tail(res,copy_expr(expr));

     FREE_SLIST(el);

     return res;
}

ExprList collect_expr_mentioning_term_2(Database *db, Term *term)
{
     Expression *expr;
     ExprList el = sl_make_slist();
     ExprList res = sl_make_slist();

     sh_for_all_2hashtable(db->table, el, term, collect_expr_term_bucket_2);

     sl_loop_through_slist(el, expr, Expression *)
	  sl_add_to_tail(res,copy_expr(expr));

     FREE_SLIST(el);

     return res;
}

int collect_exprs(ExprList exprs_to_print, Key_Elt *ko)
{
     Expression *expr;

     sl_loop_through_slist(ko->statements, expr, Expression *)
	   if (! sl_search_slist(exprs_to_print, expr, eq_expr))	/* S'il n'y est pas deja. */
		sl_add_to_head(exprs_to_print, expr);
     return 1;
}

void sprint_database(Sprinter *sp, Database *db)
{
     Expression *expr;
     ExprList exprs_to_print = sl_make_slist();

     sh_for_all_hashtable(db->table, exprs_to_print, collect_exprs);

     sl_sort_slist_func(exprs_to_print,expr_sort_pred);

     SPRINT(sp,32,sprintf(f,LG_STR("The database contains:\n",
				   "La base de faits contient:\n")));
     sl_loop_through_slist(exprs_to_print, expr,  Expression *) {
	  sprint_expr(sp,expr);
	  SPRINT(sp,1,sprintf(f,"\n"));
     }
     SPRINT(sp,1,sprintf(f,"\n"));

     FREE_SLIST(exprs_to_print);
}

void print_database(Database *db)
{
     static Sprinter *sp = NULL;

     if (! sp)
       sp = make_sprinter(0);
     else
       reset_sprinter(sp);

     sprint_database(sp,db);
     printf("%s",SPRINTER_STRING(sp));
}

ExprList get_all_expr_from_db(Database *db)
{ 
     ExprList exprs_to_get = sl_make_slist();
	  
     sh_for_all_hashtable(db->table, exprs_to_get, collect_exprs);

     return exprs_to_get;

}

void dump_database_from_parser(Database *db, PString file_name)
{
     ExprList exprs_to_dump = get_all_expr_from_db(db);
	  
     sh_for_all_hashtable(db->table, exprs_to_dump, collect_exprs);

     if (start_dump_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: dump_database_from_parser: failed start_dump_session: %s\n",
				 "ERREUR: dump_database_from_parser: echec start_dump_session: %s\n"), file_name);
     } else {
	  dump_list_expr(exprs_to_dump = get_all_expr_from_db(db));
	  dump_object_list();

	  end_dump_session();
     }

     FREE_SLIST(exprs_to_dump);
}

void load_database_from_parser(Database *db, PString file_name)
{
     if (start_load_session(file_name) < 0) {
	  fprintf(stderr, LG_STR("ERROR: load_database_from_parser: failed start_load_session: %s\n",
				 "ERREUR: load_database_from_parser: echec start_load_session: %s\n"), file_name);

     } else {
	  Expression *expr;
	  ExprList exprs_to_load;

	  exprs_to_load = load_list_expr();

	  load_all_objects();

	  end_load_session();

	  sl_loop_through_slist(exprs_to_load, expr,  Expression *) {
	       conclude(expr,db);
	       free_expr(expr);
	  }

	  FREE_SLIST(exprs_to_load);
     }
}

void save_database(Database *db, PString file_name)
{
     FILE *fpo;

     Expression *expr;
     ExprList exprs_to_print = sl_make_slist();

     if (!(fpo = (FILE *) fopen(file_name, "w"))) {
	  fprintf(stderr, LG_STR("ERROR: save_database: failed fdopen %s\n",
				 "ERREUR: save_database: echec fdopen %s\n"), file_name);
	  return;
     }
	  
     sh_for_all_hashtable(db->table, exprs_to_print, collect_exprs);

     sl_sort_slist_func(exprs_to_print,expr_sort_pred);

     fprintf(fpo, "(\n");
     sl_loop_through_slist(exprs_to_print, expr,  Expression *) {
	  fprint_expr(fpo,expr);
	  fprintf(fpo,"\n");
     }
     fprintf(fpo,")\n");
     fclose (fpo);
     FREE_SLIST(exprs_to_print);
}

int free_hash_bucket(ExprList exprs_to_free, Key_Elt *ko)
{
     Expression *expr;

     /* free_term_in_key(ko->key); */
     free_key(ko->key);
     sl_loop_through_slist(ko->statements, expr, Expression *)
	   if (!sl_search_slist(exprs_to_free, expr, eq_expr))	/* S'il n'y est pas deja. */
		sl_add_to_head(exprs_to_free, expr);
     FREE_SLIST(ko->statements);
     FREE(ko); 
     
     return 1;
}

void free_database_hash(Database *db)
{
     Expression *expr;
     ExprList exprs_to_free = sl_make_slist();

     sh_for_all_hashtable(db->table, exprs_to_free, free_hash_bucket);

     sl_loop_through_slist(exprs_to_free, expr,  Expression *)
	  free_expr(expr);

     FREE_SLIST(exprs_to_free);

     sh_free_hashtable(db->table);

     return;
}

void free_database(Database *db)
{
     free_database_hash(db);
     FREE(db);	  
     return;
}

void clear_database(Database *db)
/* 
 * clear_database -  Note that we do not reset the cwp, ev_pred and ff hash table.
 */
{
     free_database_hash(db);
     db->table = sh_make_hashtable(database_hashtable_size, db_hash_func, db_match_func);
}

int stat_table_hash_bucket (int *stat_array, Key_Elt *ko)
{
     int index = db_hash_func(ko->key);

     stat_array[index] +=1;
     return 1;
}

void stat_table_hashtable(Shash *table)
{
     int i, *stat_array;
     int nb_elt, nb_bucket = 0, nb_elt_bucket, nb_max = 0;

     stat_array = (int *) MALLOC(database_hashtable_size * sizeof(int));
     for (i=0; i <database_hashtable_size; i++)
	  stat_array[i] = 0;

     nb_elt = sh_for_all_hashtable(table, stat_array, stat_table_hash_bucket);

     for (i=0; i <database_hashtable_size; i++)
	  if ((nb_elt_bucket = stat_array[i])) {
	       nb_bucket++;
	       if (nb_elt_bucket > nb_max) nb_max = nb_elt_bucket; 
	  }
     printf (LG_STR("The database hashtable contains:\n",
		    "La base de faits contient:\n"));
     printf (LG_STR("\t%d element(s)\n",
		    "\t%d élément(s)\n"), nb_elt);
     printf (LG_STR("\tin  %d buckets ( %d );\n",
		    "\tdans  %d baquets ( %d );\n"), nb_bucket, database_hashtable_size);
     printf (LG_STR("\twith a maximal number of %d element(s) in one bucket.\n",
		    "\tavec au plus %d élément(s) par baquet.\n"), nb_max);
     FREE(stat_array);
}

void stat_database(Database *db)
{
     stat_table_hashtable(db->table);
}

ExprList get_expr_from_key(Key *key, Shash *table)
/* Return the list of expressions pointed by this key. */
{
     Key_Elt *ko;

     if (!(ko = (Key_Elt *) sh_get_from_hashtable(table, key)))
		  return empty_list;	/* List vide */
     else
	  return ko->statements;
}

Expression *expr_save;
Key_Elt *ko_save;

PBoolean add_expr_to_key(Expression *expr, Key *key, Shash *table)
/* 
 * add_expr_to_key - will add a new expr in the bucket pointed by key 
 *                   or will create a new bucket if the bucket does not exist.
 *                   Could be optimize a little bit. Return TRUE if
 *                   expr has to be freed, FALSE otherwise.
 *                  For Basic Events, save the Key_Elt and the Expr in global.
 */
{
     Key_Elt *ko;

     if (!(ko = (Key_Elt *) sh_get_from_hashtable(table, key))) {
	  ko = MAKE_OBJECT(Key_Elt);
	  ko->nombre = 1;
	  /* dup_term_in_key(key); 	  To fix the taxi bug, the term pointed by the key must be duplicated */
	  ko->key = key;
	  ko->statements = sl_make_slist();
	  sl_add_to_head(ko->statements, expr);
	  sh_add_to_hashtable(table, ko, key);
	  ko_save = ko;
	  expr_save = expr;
	  return FALSE;
     } else {
	  free_key(key);	/* The key already exist, I can free this one */
	  if (!sl_search_slist(ko->statements, expr, equal_expr_literal_var)) {	/* S'il n'y est pas deja. */
	       sl_add_to_head(ko->statements, expr);
	       ko->nombre++;
	       ko_save = ko;
	       expr_save = expr;
	       return FALSE; 
	  } else {
	       return TRUE;    /* If it is already in there, we can free it. */
	  }
     }
}

void delete_expr_from_key(Expression *expr, Key *key, Shash *table)
/*
 * delete_expr_from_key - will eradicate expr from the the bucket pointed by key... We
 *                        know expr is in there, so no unification... just get it out. We do not 
 *                        free expr... but we free the bucket and the key if necessary.
 */
{
     Key_Elt *ko;

     if (!(ko = (Key_Elt *) sh_get_from_hashtable(table, key))) {
	  fprintf(stderr, LG_STR("ERROR: delete_expr_from_key: could not find the bucket in the hash_table.\n",
				 "ERREUR: delete_expr_from_key: Ne retrouve pas le baquet dans la table de hachage.\n"));
	  fprintf(stderr, LG_STR("\twith the key: ",
				 "\tavec la clef: "));
	  fprint_key(stderr, key); 
	  fprintf(stderr, "\n");
	  return;
     }
     if (!sl_delete_slist_node(ko->statements, expr)) {
	  fprintf(stderr, LG_STR("ERROR: delete_expr_from_key: could not find the expr in the bucket.\n",
				 "ERREUR: delete_expr_from_key: Ne retrouve pas l'expression dans le baquet.\n"));
	  fprintf(stderr, LG_STR("\twith the key: ",
				 "\tavec la clef: "));
	  fprint_key(stderr, key); 
	  fprintf(stderr, LG_STR("\n\tand the expr: ",
				 "\n\tet l'expr: "));
	  fprint_expr(stderr, expr); 
	  fprintf(stderr, "\n");
	  return;
     }
     ko->nombre--;
     if (ko->nombre == 0) {
	  sh_delete_from_hashtable(table, key);
	  FREE_SLIST(ko->statements);
	  /* free_term_in_key(ko->key); */
	  free_key(ko->key);
	  FREE(ko);
     }
}

Expression *delete_expr_equal_from_key(Expression *expr, Key *key, Shash *table)
{
     Key_Elt *ko;
     Expression *expr2, *expr3 = NULL;

     if ((ko = (Key_Elt *) sh_get_from_hashtable(table, key))) /* This should work with expr_equal */
	  while ((expr2 = (Expression *) sl_search_slist(ko->statements, expr, equal_expr_literal_var))) {
	       sl_delete_slist_node(ko->statements, expr2);
	       ko->nombre--;
	       if (!expr3) expr3 = expr2;
	       else if (expr3 != expr2) fprintf(stderr, LG_STR("ERROR: delete_expr_equal_from_key: different exprs.\n",
							       "ERREUR: delete_expr_equal_from_key: les expressions sont différentes.\n"));
	       else  fprintf(stderr, LG_STR("ERROR: delete_expr_equal_from_key: same expr twice in same bucket.\n",
					    "ERREUR: delete_expr_equal_from_key: La même expression se trouve en double dans un baquet.\n"));
	       if (ko->nombre == 0) {
		    sh_delete_from_hashtable(table, key);
		    FREE_SLIST(ko->statements);
		    /* free_term_in_key(ko->key); */
		    free_key(ko->key);
		    FREE(ko);
		    break;	/* will get us out of the while */
	       }
	  }
     return expr3;
}

Expression *find_expr_equal_from_key(Expression *expr, Key *key, Shash *table)
{
     Key_Elt *ko;

     if ((ko = (Key_Elt *) sh_get_from_hashtable(table, key))) /* This should work with expr_equal */
	  return ((Expression *) sl_search_slist(ko->statements, expr, equal_expr_literal_var));
     else
	  return NULL;
}

void conclude(Expression *expr, Database *db)
/* conclude - will conclude a copy of expr in the DB. */
{ 
#ifdef OPRS_PROFILING
     PDate pd;
#endif /* OPRS_PROFILING */
     int ff;

#ifdef OPRS_PROFILING
     if (profiling_option[PROFILING] && profiling_option[PROF_DB]) 
	  db->nb_assert++;
#endif


     if (debug_trace[DATABASE]) {
	  printf(LG_STR("Concluding the expr: ",
			"Ajoute l'expression: "));
	  print_expr(expr);
	  printf("\n");
     }

     concluding = TRUE;
     variables_have_literal_semantic = TRUE;

#ifdef OPRS_PROFILING
     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     expr_being_concluded = expr;

     if (PFR_EP(expr->pfr))
	  fprintf(stderr, LG_STR("WARNING: conclude: you should not be concluding statement\n\
\tfor evaluable predicate: %s.\n",
				 "ATTENTION: conclude: Vous ne devriez pas rajouter un fait\n\
\tpour le prédicat évaluable: %s.\n"), expr->pfr->name);

     else if (PFR_OP_PRED(expr->pfr))
	  fprintf(stderr, LG_STR("WARNING: conclude: you should not be concluding statement\n\
\tfor OP predicate: %s.\n",
				 "ATTENTION: conclude: Vous ne devriez pas rajouter un fait\n\
\tpour le OP prédicat: %s.\n"), expr->pfr->name);
     else {
	  Slist *key_list;
	  Key *kt;
	  Expression *not_expr;
	  TermList tl_tmp;
	  PBoolean fact_be = FALSE;

	  Slist *key_list2;
	  Key *key;

	  copy_var_uniquely = sl_make_slist();

	  expr = copy_expr(expr); /* This will make a full real copy of the expr. */

	  FREE_SLIST(copy_var_uniquely);
	  copy_var_uniquely = NULL;

	  tl_tmp = expr->terms;
	  expr->terms = find_binding_ef_tl(expr->terms);
	  free_tl(tl_tmp);

	  not_expr = not_expr_dup(expr);

	  if (!PFR_BE(expr->pfr)
	      && ((NOT_P(expr) || /* ~ expr */
		   (! (PFR_CWP(not_expr->pfr) || /* or expr and ! (CWP or FF) */
		       PFR_FF(not_expr->pfr) >= 0))))) {
	  
	       key_list = delete_expr_from_conclude_expr(not_expr, db); /* Remove the opposite */
	       sl_loop_through_slist(key_list, kt, Key *) {
		    free_key(kt);
	       }
	       FREE_SLIST(key_list);
	  }

	       if (!NOT_P(expr) ||
		   (! ((PFR_CWP(not_expr->pfr)) ||
		       (PFR_FF(not_expr->pfr) >= 0)))) {
		    /* Positive or not a cwp */
		    PBoolean first = TRUE;
		    PBoolean free_it = TRUE;  /* Just to avoid GCC Warning */

		    if ((ff = PFR_FF(expr->pfr)) >= 0) {

			 /* Expression *ff_exp;
			    ff_exp = build_ff_expr(expr, ff);
			 */

			 if (setjmp(key_computation_jmp) == 0) {
			      key_list = delete_ff_expr(expr, ff, db);
			      /* free_ff_expr(ff_exp, ff); */
			 } else {
			      fprintf(stderr,LG_STR("ERROR: conclude: Problem in key computation,\n\
\taborting conclude operation for expr: ",
						    "ERREUR: conclude: Problème de calcul de clefs,\n\
\tabandon de l'opération d'ajout de l'expression: "));
			      fprint_expr(stderr,expr_being_concluded);
			      fprintf(stderr, ".\n");
			      return;
			 }

		    } else {

			 /* Search the negation of this fact, and revert it */
			 /* expr->signe = !(expr->signe); */	/* Negate it */
			 if (setjmp(key_computation_jmp) == 0) {
			      key_list = revert_expr_from_conclude_expr(expr, db); 
			 } else {
			      fprintf(stderr,LG_STR("ERROR: conclude: Problem in key computation,\n\
\taborting conclude operation for expr: ",
						    "ERREUR: conclude: Problème de calcul de clefs,\n\
\tabandon de l'opération d'ajout de l'expression: "));
			      fprint_expr(stderr,expr_being_concluded);
			      fprintf(stderr, ".\n");
			      return;
			 }
			 /* expr->signe = !(expr->signe); */ /* Back to the initial value */
		    }

		    if (!key_list) { /* We found and revert the negation */
			 fprintf(stderr, LG_STR("ERROR: conclude: no key...\n",
						"ERREUR: conclude: pas de clef...\n"));
		    } else {
			 BE_Clean *be_clean = NULL; /* Just to avoid gcc warning. */
		    
			 if (PFR_BE(expr->pfr)) {
			      /* This is a basic event, we will save the keys and the expr to avoid recomputing it */
			      fact_be = TRUE;
			      be_clean = MAKE_OBJECT(BE_Clean);
			      be_clean->ko_list = sl_make_slist();
			 }

			 key_list2 = COPY_SLIST(key_list);
		    
			 sl_loop_through_slist(key_list, kt, Key *) {
			      if (first) {
				   free_it = add_expr_to_key(expr, kt, db->table);
				   first = FALSE;
			      } else if ( free_it != add_expr_to_key(expr, kt, db->table)) {
				   fprintf(stderr, LG_STR("SERIOUS ERROR: add_expr_to_key: one says free the other one not.\n",
							  "ERREUR GRAVE: add_expr_to_key: l'un dit FREE l'autre non.\n"));
				   fprintf(stderr, LG_STR("\tFree is : %s\n",
							  "\tFREE est : %s\n"), (free_it ? "True" : "False"));
				   fprintf(stderr, LG_STR("\tthe expr is: ",
							  "\tl'expr est: ")); fprint_expr(stderr, expr); fprintf(stderr, "\n");
							  fprintf(stderr, LG_STR("\tCurrent key: ",
										 "\\Clef actuelle: ")); fprint_key(stderr, kt); fprintf(stderr, "\n");
										 fprintf(stderr, LG_STR("\tKey list: ",
													"\tListe de clefs: "));
										 sl_loop_through_slist(key_list2, key, Key *) {
										      fprint_key(stderr, key); fprintf(stderr, ", ");
										 }
										 fprintf(stderr, "\n");
			      }
			      if (fact_be && (! (free_it))) {
				   DUP(ko_save);
				   sl_add_to_head(be_clean->ko_list, ko_save);
			      }
			 }
			 if (fact_be ) {
			      if (free_it) { /* This expr was already in the database */
				   FREE_SLIST(be_clean->ko_list);
				   FREE(be_clean);
			      }else {
				   be_clean->expr = dup_expr(expr_save);
				   sl_add_to_head(current_oprs->concluded_be, be_clean);
			      }
			 }
			 FREE_SLIST(key_list2);
			 FREE_SLIST(key_list);
			 if (free_it) 
			      free_expr(expr);
		    }

	       }

	       free_expr(not_expr);
	       }
#ifdef  OPRS_PROFILING
     TIME_STOP(&pd,T_CONCLUDE);
#endif /* OPRS_PROFILING */

     concluding = FALSE;
     variables_have_literal_semantic = FALSE;

}

Key_List delete_expr_from_conclude_expr(Expression *expr, Database *db)
/*
 * delete_from_conclude - delete the expression expr. It returns
 *                        the key list so we do not have to recompute it.
 */
{
     Slist *key_list;
     Key *kt;
     Expression *expr2, *expr3 = NULL;
     PBoolean first = TRUE;

     if (debug_trace[DATABASE]) {
	  printf(LG_STR("Delete_from_conclude the expr: ",
			"Delete_from_conclude l'expression: "));
	  print_expr(expr);
	  printf("\n");
     }

     /* conclude already evaluated the terms. */
     key_list = make_key_list(expr, FALSE, NULL);
     sl_loop_through_slist(key_list, kt, Key *) {
	  expr2 = delete_expr_equal_from_key(expr, kt, db->table);
	  if (first){
	       expr3 = expr2;
	       first = FALSE;
	  }else if (expr3 != expr2)
	       fprintf(stderr, LG_STR("ERROR: delete_expr_from_conclude_expr: different exprs.\n",
				      "ERREUR: delete_expr_from_conclude_expr: les expressions sont différentes.\n"));
     }
     if (expr3) free_expr(expr3);
     return key_list;
}

Key_List revert_expr_from_conclude_expr(Expression *expr, Database *db)
/*
 * revert_expr  if the expr is in the database, revert its signe and returns NULL,
 *              if not  returns the key list so we do not have to recompute it.
 */
{
     Slist *key_list;

     if (debug_trace[DATABASE]) {
	  printf(LG_STR("Revert_from_conclude the expr: ",
			"Revert_from_conclude l'expression: ")); print_expr(expr); printf("\n");
     }

     /*
      * conclude already evaluated the terms.
      */
     key_list = make_key_list(expr, FALSE, NULL);

     return key_list;
}

#ifdef CONNECT_TO_IXTET
void sprint_tl_before_last(Sprinter *sp,TermList tl)
{
     Term *t;
     Term *t2 = NULL;
     PBoolean first = TRUE;

     sl_loop_through_slist(tl, t, Term *) {
	  if (t2) {
	       if (first) {
		    first = FALSE;
	       } else {
		    SPRINT(sp,2,sprintf(f,", "));
	       }
	       sprint_term(sp,t2);
	  }
	  t2 = t;
     }
     return;
}
#endif

Key_List delete_ff_expr(Expression *expr, int ff,Database *db)
/* delete_ff_expr -- will delete the functional fact which is replaced by expr and return the key_list in 
 *  		     which we should put expr.
 */
{
     Slist *key_arg_list, *key_val_list;
     ExprList el, el2, el3;
     Expression *expr_tmp;
     void *ptr1;
     Key *kt;

     if (debug_trace[DATABASE]) {
	  printf(LG_STR("Concluding the ff expr: ",
			"Ajout de l'expression ff: "));
	  print_expr(expr);
	  printf("\n");
     }


     key_arg_list = make_ff_arg_key_list(expr, ff);
     key_val_list = make_ff_val_key_list(expr, ff);
     
     
     ptr1 = sl_get_slist_next(key_arg_list, NULL);
     el = COPY_SLIST(get_expr_from_key((Key *) ptr1, db->table));
     while ((ptr1 = sl_get_slist_next(key_arg_list, ptr1)) && !(sl_slist_empty(el))) {
	  el2 = get_expr_from_key((Key *) ptr1, db->table);
	  el3 = sl_make_slist();
	  sl_loop_through_slist(el2, expr_tmp, Expression *) {	/* Y a t'il moyen de faire une intersection moins chere??? */
	       if (sl_search_slist(el, expr_tmp, eq_expr))	/* They just need to be eq */
		    sl_add_to_head(el3, expr_tmp);
	  }
	  FREE_SLIST(el);
	  el = el3;
     }
     /* here el contains the intersection of the buckets... ie at most one element...*/

     if (sl_slist_length(el) > 1) {
	  fprintf(stderr, LG_STR("ERROR: delete_ff_expr: more than one functional fact\n\
\tfound for: ",
				 "ERREUR: delete_ff_expr: Trouvé plus d'un fait fonctionel\n\
\tpour: "));
	  fprint_expr(stderr,expr);
	  fprintf(stderr, "\n"); 
     }


#ifdef CONNECT_TO_IXTET
     if (ixtet_name && sl_slist_length(el) == 1) {
 	  Term *last_old_term = (Term* )sl_get_slist_tail(((Expression *)sl_get_slist_pos(el, 1))
							  ->terms);
	  Term *last_new_term = (Term* )sl_get_slist_tail(expr->terms);

	  if (! equal_term(last_new_term, last_old_term)) {
	       PDate ixtet_current_date;      
	       long tv_sec;
	       long tv_usec;
	       static Sprinter *ixtet_message = NULL;

	       if (!ixtet_message) 
		    ixtet_message = make_sprinter(0);
	       else 
		    reset_sprinter(ixtet_message);

	       time_stamp(&ixtet_current_date);
	       tv_sec = ixtet_current_date.tv_sec - ixtet_start_date.tv_sec;
	       
	       if ((tv_usec = ixtet_current_date.tv_usec - ixtet_start_date.tv_usec ) < 0) {
		    tv_usec += 1000000L;
		    tv_sec -= 1;
	       }
	       
	       SPRINT(ixtet_message, 6 + MAX_PRINTED_INT_SIZE,
		      sprintf(f,"time %ld.%.2ld\n",tv_sec,tv_usec/10000L));

	       send_message_string(SPRINTER_STRING(ixtet_message),ixtet_name);
	  
	       reset_sprinter(ixtet_message);

	       SPRINT(ixtet_message, 7 + strlen(expr->pfr->name),
		      sprintf(f,"send %s (",expr->pfr->name));

	       sprint_tl_before_last(ixtet_message,expr->terms);

	       SPRINT(ixtet_message, 5, sprintf(f,") : ("));
	       sprint_term(ixtet_message, last_old_term);
	       SPRINT(ixtet_message, 1, sprintf(f,","));
	       sprint_term(ixtet_message, last_new_term);
	       SPRINT(ixtet_message, 2, sprintf(f,")\n"));

	       send_message_string(SPRINTER_STRING(ixtet_message),ixtet_name);
	  }
     }
#endif

     sl_loop_through_slist(el, expr_tmp, Expression *) {
	  Slist *key_val2_list = make_ff_val_key_list(expr_tmp, ff);

	  sl_loop_through_slist(key_arg_list, kt, Key *)
	       delete_expr_from_key(expr_tmp, kt, db->table);

	  sl_loop_through_slist(key_val2_list, kt, Key *) {
	       delete_expr_from_key(expr_tmp, kt, db->table);
	       free_key(kt);
	  }
	  FREE_SLIST(key_val2_list);
	  free_expr(expr_tmp);
     }

     FREE_SLIST(el);

     return sl_concat_slist(key_val_list, key_arg_list);
}

ExprList delete_expr_internal(Expression *expr, Database *db)
/*
 * delete_expr - will delete all the exprs which match expr in the database. TO do so, it
 *               will first do some kind of consult, and then for each "expr" it will
 *               eradicate it from the DB.
 */
{
     Slist *key_list;
     Key *kt;

     ExprList el, el2, el3, el_a_del;
     Expression *expr_tmp;
     FramePtr fp;
     void *ptr1;
     TermList tl_tmp;
     List_Envar dup_var_list = sl_make_slist();

#ifdef OPRS_PROFILING
     if (profiling_option[PROFILING] && profiling_option[PROF_DB]) 
	  db->nb_delete++;
#endif

     if (debug_trace[DATABASE]) {
	  printf(LG_STR("Deleting the expr: ",
			"Retire l'expression: "));
	  print_expr(expr);
	  printf("\n");
     }

     tl_tmp = expr->terms;	/* save the terms list at one level... */
/*      expr->terms = find_binding_ef_tl(expr->terms); */

     expr->terms = find_binding_ef_or_prog_var_tl(expr->terms);
     
     key_list = make_key_list(expr, TRUE, &dup_var_list);

     ptr1 = sl_get_slist_next(key_list, NULL);
     el = COPY_SLIST(get_expr_from_key((Key *) ptr1, db->table));
     while ((ptr1 = sl_get_slist_next(key_list, ptr1)) && !(sl_slist_empty(el))) {
	  el2 = get_expr_from_key((Key *) ptr1, db->table);
	  el3 = sl_make_slist();
	  sl_loop_through_slist(el2, expr_tmp, Expression *) {	/* Y a t'il moyen de faire une intersection moins
								 * chere??? */
	       if (sl_search_slist(el, expr_tmp, eq_expr))	/* They just need to be eq */
		    sl_add_to_head(el3, expr_tmp);
	  }
	  FREE_SLIST(el);
	  el = el3;
     }
     /* here el contains the intersection of the buckets... */

     sl_loop_through_slist(key_list, kt, Key *)
	  free_key(kt);		/* Free the keys... */

     FREE_SLIST(key_list);

     /* We need to unify only for expr like (FOO $X $X) */

     if (! dup_var_list) {	/* there was duplicate variable */

	  el_a_del = sl_make_slist();
	  sl_loop_through_slist(el, expr_tmp, Expression *) {
	       if (unify_expr_from_db(expr, (Expression *) expr_tmp,
				      fp = make_frame(empty_env),
				      TRUE)) {
		    desinstall_frame(fp, empty_env);
		    sl_add_to_head(el_a_del, expr_tmp);
	       }
	       free_frame(fp);
	  }
	  /* here, el_del contains those which unify with the expr to delete... */
	  FREE_SLIST(el);
     } else {			/* No duplicate variables... So all of them unify. */
	  FREE_SLIST(dup_var_list);
	  el_a_del = el;
     }	  

     variables_have_literal_semantic = TRUE;
     sl_loop_through_slist(el_a_del, expr_tmp, Expression *) {
	  key_list = make_key_list(expr_tmp,FALSE, NULL);
	  sl_loop_through_slist(key_list, kt, Key *) {
	       delete_expr_from_key(expr_tmp, kt, db->table);
	       free_key(kt);
	  }
	  FREE_SLIST(key_list);
     }
     variables_have_literal_semantic = FALSE;

     free_tl(expr->terms);	/* We free the copy. */
     expr->terms = tl_tmp;	/* on lui restitue sa term list (pour les variable eventuelles) */

     return el_a_del;
}

void delete_fact_expr(Expression *expr, Database *db)
{
     ExprList el;
     Expression *expr_tmp;

     el = delete_expr_internal(expr,db);

     if (sl_slist_empty(el)) {
	  if (debug_trace[DATABASE]) {
	       printf(LG_STR("Could not find any fact matching: ",
			     "N'a pas pu trouver de fait unifiant: "));
	       print_expr(expr);
	       printf(LG_STR(" to delete.\n",
			     " a retracter.\n"));
	  }
     } else {
	  if (debug_trace[DATABASE]) {
	       printf(LG_STR("Deleted the fact matching: ",
			     "Retire les faits unifiant: "));
	       print_expr(expr);
	       printf(" ->");
	  }
	  sl_loop_through_slist(el, expr_tmp, Expression *) {
	       if (debug_trace[DATABASE]) {
		    printf(" ");
		    print_expr(expr_tmp);
	       }
	       free_expr(expr_tmp);
	  }
	  if (debug_trace[DATABASE]) printf(".\n");
     }
     FREE_SLIST(el);
}

void delete_expr(Expression *expr, Database *db)
{
     ExprList el;
     Expression *expr_tmp;

     el = delete_expr_internal(expr,db);

     sl_loop_through_slist(el, expr_tmp, Expression *) {
	  free_expr(expr_tmp);
     }

     FREE_SLIST(el);
}


void delete_basic_events(Oprs *oprs)
/*
 * The pointers to the expr and the buckets have been saved,
 * we just have to verify if they are not already deleted from the database.
 */
{
     Expression *expr_to_del;
     BE_Clean *be_clean;
     Key_Elt *ko;
     Shash *table = oprs->database->table;

     sl_loop_through_slist(oprs->concluded_be, be_clean, BE_Clean *) {
	  expr_to_del = be_clean->expr;

	  if (debug_trace[DATABASE]) {
	       printf(LG_STR("Delete the Basic Event expr: ",
			     "Retire le fait Basic Event: "));
	       print_expr(expr_to_del);
	       printf("\n");
	  }

	  if (LAST_REF(expr_to_del)) { /* The expr has already been deleted from the DB */
	       sl_loop_through_slist(be_clean->ko_list, ko, Key_Elt *) {
		    FREE(ko);         /* We have dup it, so we need to free it */
	       }
	  } else {
	       sl_loop_through_slist(be_clean->ko_list, ko, Key_Elt *) {
		    if (!sl_delete_slist_node(ko->statements, expr_to_del))
			 fprintf(stderr, LG_STR("ERROR: delete_basic_events: could not find the expr in the bucket.\n",
						"ERREUR: delete_basic_events: Ne retrouve pas l'expression dans le baquet.\n"));
		    ko->nombre--;
		    if (ko->nombre == 0) {
			 sh_delete_from_hashtable(table, ko->key);
			 FREE_SLIST(ko->statements);
			 /* free_term_in_key(ko->key); */
			 free_key(ko->key);
			 FREE(ko);
		    }
		    FREE(ko);
	       }
	       free_expr (expr_to_del); /* The one in the buckets */
	  }
	  free_expr (expr_to_del); /* This free is for the dup we do in conclude */
	  FREE_SLIST(be_clean->ko_list);
	  FREE(be_clean);
     }
     FLUSH_SLIST(oprs->concluded_be);
}

void report_database_ctxt_error(Expression *expr) 
{
     static Sprinter *sp = NULL;

     if (! sp)
	  sp = make_sprinter(0);
     else
	  reset_sprinter(sp);

     SPRINT(sp,64,sprintf(f,LG_STR("The previous error occured while checking: ",
				   "La précédent erreur s'est produite lors de la consultation de: ")));
     sprint_expr(sp,expr);
     SPRINT(sp,64,sprintf(f,LG_STR(" in the database.\n",
				   " dans la base de faits.\n")));

     fprintf(stderr, "%s", SPRINTER_STRING(sp));
     printf("%s", SPRINTER_STRING(sp));
}

ExprFrameList consult_expr(Expression *expr, Frame *frame, Database *db, PBoolean fact_res)
/*
 * consult_expr -  Return a list of pair expression-frame (expression is NULL if not fact_res) such
 *		   that these expressions unify expr in their respective fame. The returned
 *		   expressions are copies of the original
 */
{
     ExprFrameList efl = sl_make_slist();

#ifdef OPRS_PROFILING
     if (profiling_option[PROFILING] && profiling_option[PROF_DB]) 
	  db->nb_consult++;
#endif

     if (! PFR_OP_PRED(expr->pfr)) {
	  Eval_Pred *ep;
	  TermList tl_tmp;
	  Expression *not_expr = NULL;

	  tl_tmp = expr->terms;	/* save the terms list... */

	  if (NOT_P(expr))
	       not_expr = not_expr_dup(expr);
	  
	  if (NOT_P(expr) && ((PFR_CWP(not_expr->pfr)) || (PFR_FF(not_expr->pfr) >= 0))) {
	       /* It is is negated and it is a CWP or a FF. */
	       ExprFrameList efltmp;
	       ExprFrame *ef;
	       FrameList fl = sl_make_slist();

	       /* Look for the positive */
	       efltmp = consult_expr(not_expr, frame, db, FALSE);
	       if (sl_slist_empty(efltmp)) /* Not here. The cwp negation is therefore true... */
		    sl_add_to_head(efl, make_ef((fact_res ? dup_expr(expr) : NULL), dup_frame(frame)));

	       sl_loop_through_slist(efltmp, ef, ExprFrame *) {
		    free_frame(ef->frame);
		    FREE(ef);
	       }
	       
	       FREE_SLIST(fl);
	       FREE_SLIST(efltmp);
	       
	  } else if ((ep = PFR_EP(expr->pfr))) { /* It is an evaluable predicate... */
	       Error_Ctxt previous_error_ctxt = current_error_ctxt;

	       current_error_ctxt = EC_CONSULT_DATABASE;
	       if (setjmp(consult_database_jmp) == 0) {
		    if (ep->eval_var) {/* does it evaluate its argument.... */
			 expr->terms = find_binding_ef_tl(expr->terms); /* this will copy the list */
			 if (evaluate_expression_predicate(ep, PFR_NAME(expr->pfr), expr->terms))
			      sl_add_to_head(efl, make_ef((fact_res ? dup_expr(expr) : NULL), dup_frame(frame)));
			 free_tl(expr->terms);
		    } else {		/* No eval_var predicate. */
			 Frame *frp;
		    
			 if (evaluate_expression_predicate_frame(frp = make_frame(frame), ep, PFR_NAME(expr->pfr), 
								 expr->terms)) {
			      desinstall_frame(frp, frame);
			      sl_add_to_head(efl, make_ef((fact_res ? dup_expr(expr) : NULL), frp));
			 } else 
			      free_frame(frp);
		    }
	       } else {		/* There was a serious error. */
		    locked_local_var = NULL; /* Restore this variable value. */
		    report_database_ctxt_error(expr);
		    propagate_error(previous_error_ctxt);
	       }
	       current_error_ctxt = previous_error_ctxt;
	  } else {		/* Standard case... */

	       Slist *key_list;
	       ExprList el, el2, el3;
	       Expression *expr_tmp;
	       Key *kt;
	       FramePtr fp;
	       void *ptr1;
	       List_Envar dup_var_list = sl_make_slist();
	       PBoolean duplicate_var;

	       expr->terms = find_binding_ef_or_prog_var_tl(expr->terms);

	       key_list = make_key_list(expr,TRUE, &dup_var_list);

	       ptr1 = sl_get_slist_next(key_list, NULL);
	       el = COPY_SLIST(get_expr_from_key((Key *) ptr1, db->table));
	       while ((ptr1 = sl_get_slist_next(key_list, ptr1)) && !(sl_slist_empty(el))) {
		    el2 = get_expr_from_key((Key *) ptr1, db->table);
		    el3 = sl_make_slist();
		    sl_loop_through_slist(el2, expr_tmp, Expression *) { 
			 if (sl_search_slist(el, expr_tmp, eq_expr))
			      sl_add_to_head(el3, expr_tmp);
		    }
		    FREE_SLIST(el);
		    el = el3;
	       }

	       sl_loop_through_slist(key_list, kt, Key *)
		    free_key(kt); /* Free the keys... */

	       FREE_SLIST(key_list);


	       if (! dup_var_list) {	/* there was duplicate variable */
		    duplicate_var = TRUE;
	       } else {			/* No duplicate variables... So all of them unify. */
		    duplicate_var = FALSE;
		    FREE_SLIST(dup_var_list);
	       }

	       sl_loop_through_slist(el, expr_tmp, Expression *) {
		    if (unify_expr_from_db(expr, (Expression *) expr_tmp,
					   fp = make_frame(frame),
					   duplicate_var)) {
			 desinstall_frame(fp, frame);
			 sl_add_to_head(efl, make_ef((fact_res ? dup_expr(expr_tmp) : NULL), fp));
		    } else
			 free_frame(fp);
	       }

	       FREE_SLIST(el);
	       free_tl(expr->terms);
	  }

	  expr->terms = tl_tmp;	/* on lui restitue sa term list (pour les variable eventuelles) */
	  
	  if (not_expr) free_expr(not_expr);

     }
	  

     if (debug_trace[DATABASE]) {
	  ExprFrame *exprf;

	  printf(LG_STR("The consultation_expr of: ",
			"La consultation_expr de: "));
	  print_expr(expr);
	  printf(LG_STR(" gives:\n",
			" donne:\n"));
	  if (sl_slist_empty(efl))
	       printf("NULL\n");
	  sl_loop_through_slist(efl, exprf, ExprFrame *) {
	       print_expr_frame(exprf);
	       printf("\n");
	  }
	  printf("\n");
     }
     return efl;
}

ExprListFrameList consult_and(ExprList el, Frame *frame, Database *db, PBoolean fact_res)
/* Consult a conjunction */
{

     ExprListFrame *elf;
     ExprFrameList efl;
     ExprFrame *ef;
     ExprListFrameList elfl, elfl_res;
     ExprList el_tmp;
     Expression *expr;

     elfl_res = sl_make_slist();
     if (sl_slist_length(el) == 1) {	/* The last one in the list... */
	  efl = consult((Expression *) sl_get_from_head(el), frame, db, fact_res);
	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       if (fact_res) {
		    el_tmp = sl_make_slist();
		    sl_add_to_head(el_tmp, ef->expr);
		    sl_add_to_head(elfl_res, make_elf(el_tmp, ef->frame));
	       } else
		    sl_add_to_head(elfl_res, make_elf(NULL, ef->frame));
	       FREE(ef);
	  }
	  FREE_SLIST(efl);
     } else {
	  expr = (Expression *) sl_get_from_head(el);
	  efl = consult(expr, frame, db, fact_res);	/* idem... */
	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       reinstall_frame(ef->frame);
	       elfl = consult_and(el_tmp = (ExprList) COPY_SLIST(el),	/* si on ne la copie pas,  */
				   ef->frame, db, fact_res);	/* elle va etre reduite par les appels recursifs */
	       desinstall_frame(ef->frame, frame);
	       free_frame(ef->frame);
	       FREE_SLIST(el_tmp);

	       sl_loop_through_slist(elfl, elf, ExprListFrame *) {
		    if (fact_res) sl_add_to_head(elf->exprl, dup_expr(ef->expr));
		    sl_add_to_head(elfl_res, elf);
	       }
	       if (fact_res) free_expr(ef->expr);
	       FREE_SLIST(elfl);
	       FREE(ef);
	  }
	  FREE_SLIST(efl);
     }
     return elfl_res;
}

ExprListFrameList consult_sub_or(Expression *expr, ExprList el, Frame *frame, Database *db, PBoolean fact_res)
/* Consult a disjunction */
{
     ExprListFrameList elfl_res, elfl1;
     ExprFrameList efl;
     ExprFrame *ef;
     ExprList el_tmp, el2, el3;
     Expression *e;
     ExprListFrame *elf;

     elfl_res = sl_make_slist();
     efl = consult(expr, frame, db, fact_res);
     sl_loop_through_slist(efl, ef, ExprFrame *) {
	  if (fact_res) {
	       el_tmp = sl_make_slist();
	       sl_add_to_head(el_tmp, dup_expr(ef->expr)); /* This copy may be sperfluous if we
	       							consider the free below... */
	       sl_add_to_head(elfl_res, make_elf(el_tmp, ef->frame));
	  } else
	       sl_add_to_head(elfl_res, make_elf(NULL, ef->frame));

	  if (!(sl_slist_empty(el))) {
	       reinstall_frame(ef->frame);
	       elfl1 = sl_make_slist();
	       el2 = COPY_SLIST(el);
	       e = (Expression *) sl_get_from_head(el2);
	       while (e) {
		    sl_concat_slist(elfl1, consult_sub_or(e, el3 = COPY_SLIST(el2), ef->frame, db, fact_res));
		    FREE_SLIST(el3);
		    e = (Expression *) sl_get_from_head(el2);
	       }
	       FREE_SLIST(el2);
	       sl_loop_through_slist(elfl1, elf, ExprListFrame *) {
		    if (fact_res)
			 sl_add_to_head(elf->exprl, dup_expr(ef->expr));
		    sl_add_to_head(elfl_res, elf);
	       }
	       FREE_SLIST(elfl1);
	       desinstall_frame(ef->frame, frame);
	  }
	  if (fact_res) /* See comment above ... OPTIMIZE...Damnit */
	       free_expr(ef->expr);
	  FREE(ef);
     }
     FREE_SLIST(efl);
     return elfl_res;
}

ExprListFrameList consult_or(ExprList el, Frame *frame, Database *db, PBoolean fact_res)
/* consult a disjunction at the top level */
{
     ExprListFrameList elfl_res;
     Expression *e;
     ExprList el2;

     elfl_res = sl_make_slist();
     e = (Expression *) sl_get_from_head(el);
     while (e) {
	  sl_concat_slist(elfl_res, consult_sub_or(e, el2 = COPY_SLIST(el), frame, db, fact_res));
	  e = (Expression *) sl_get_from_head(el);
	  FREE_SLIST(el2);
     }
     return elfl_res;
}

ExprFrameList consult_lexpr(Expression *expr, Frame *frame, Database *db, PBoolean fact_res)
{
     ExprListFrameList elfl = NULL;
     ExprListFrame *elf;
     ExprFrameList lefl;
     ExprList el;

     lefl = sl_make_slist();
     el = make_el_from_tl(EXPR_TERMS(expr));
     if (AND_P(expr)){
	  elfl = consult_and(el, frame, db, fact_res);
	  sl_loop_through_slist(elfl, elf, ExprListFrame *) {
	       sl_add_to_head(lefl, make_ef((fact_res ? build_expr_sym_exprs(and_sym,elf->exprl)
					   : NULL), elf->frame));
	       FREE(elf);
	  }
     } else {/*  (OR_P(expr)) */
	  elfl = consult_or(el, frame, db, fact_res);
	  sl_loop_through_slist(elfl, elf, ExprListFrame *) {
	       sl_add_to_head(lefl, make_ef((fact_res ? build_expr_sym_exprs(or_sym,elf->exprl)
					   : NULL), elf->frame));
	       FREE(elf);
	  }
     }
     FREE_SLIST(elfl);
     FREE_SLIST(el);
     return lefl;
}

ExprFrameList consult(Expression *expr, Frame *frame, Database *db, PBoolean fact_res)
/* 
 * consult - which could be named consult_expr is the top level funtion for expr consultation. 
 *           Return a ExprFrameList with rebuilt facts according to fact_res (NULL if !fact_res).
 */
{
     ExprFrameList gefl;
     ExprFrameList efl;
     ExprFrame *ef;
     ExprFrameList lefl;
     ExprFrame *lef;
     ExprFrame *gef;
     PBoolean prev_locked_local_var = TRUE;
#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     if (! locked_local_var) {
	  prev_locked_local_var = FALSE;
	  locked_local_var = sl_make_slist();
     }

     gefl = sl_make_slist();

     if (OR_P(expr) || AND_P(expr)) {
	  lefl = consult_lexpr(expr, frame, db, fact_res);
	  sl_loop_through_slist(lefl, lef, ExprFrame *) {
	       sl_add_to_tail(gefl, make_ef((fact_res ? dup_expr(lef->expr)
					      : NULL), lef->frame));
	       FREE(lef);
	  }
	  FREE_SLIST(lefl);
     } else { /* EXPR */
	  efl = consult_expr(expr, frame, db, fact_res);
	  sl_loop_through_slist(efl, ef, ExprFrame *) {
	       sl_add_to_tail(gefl, make_ef((fact_res ? dup_expr(ef->expr)
					  : NULL), ef->frame));
	       FREE(ef);
	  }
	  FREE_SLIST(efl);
     }

#ifdef  OPRS_PROFILING
     TIME_STOP(&pd,T_RETRIEVE);
#endif /* OPRS_PROFILING */

     if (debug_trace[DATABASE]) {
	  printf(LG_STR("The consultation of the expr: ",
			"La consultation de la expr: "));
	  print_expr(expr);
	  printf(LG_STR(" gives:\n",
			" donne:\n"));
	  if (sl_slist_empty(gefl))
	       printf("NULL\n");
	  else 
	       sl_loop_through_slist(gefl, gef, ExprFrame *) {
		    print_expr_frame(gef);
		    printf("\n");
	       }
	  printf("\n");
     }

     if (! prev_locked_local_var) {
	  Undo *u;

	  sl_loop_through_slist(locked_local_var, u, Undo *) {
	       rebind_var(u);
	       free_undo(u);
	  }
	  FREE_SLIST(locked_local_var);
	  locked_local_var = NULL;
     }

     return gefl;
}

void parser_consult(Expression *expr, Database *db)
/* test_consult - is called from the yacc parser directly.  */
{
     ExprFrame *ef;
     ExprFrameList efl;

     efl = consult(expr, empty_env, db, TRUE);

     printf(LG_STR("The user consultation of: ",
		   "La consultation par l'utilisateur de: "));
     print_expr(expr);
     printf(LG_STR(" gives the following result:\n",
		   " donne le résultat suivant:\n"));
     if (sl_slist_empty(efl))
	  printf("NULL\n");

     sl_loop_through_slist(efl, ef, ExprFrame *) {

	  if (debug_trace[DATABASE_FRAME]) 
	       print_expr_frame(ef);
	  else
	       print_expr(ef->expr);

	  printf("\n");

	  free_expr(ef->expr);
	  free_frame(ef->frame);
	  FREE(ef);
     }
     FREE_SLIST(efl);
}

void parser_conclude(Expression *expr, Database *db)
/* 
 * test_conclude - is called from the yacc parser directly. No frame are used because we do not 
 *                 conclude anything else then standard expression (no variable allowed) 
 */
{
     conclude(expr, db);

     delete_basic_events(current_oprs);

     if (debug_trace[CONCLUDE_FROM_PARSER]) {
	  printf(LG_STR("The expression: ",
			"L'expression: "));
	  print_expr(expr);
	  printf(LG_STR(" has been concluded in the database.\n",
			" a été ajouté dans la base de faits.\n"));
     }
}
