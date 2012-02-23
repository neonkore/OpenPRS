/*                               -*- Mode: C -*- 
 * type.c -- 
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
#include "config.h"

#include <stdio.h>

#include "shashPack_f.h"

#include "macro.h"
#include "constant-pub.h" /* enzo */
#include "lang.h"
#include "type.h"
#include "oprs-type.h"
#include "oprs.h"
#include "oprs-type-pub.h"

#include "oprs-type_f.h"
#include "type_f.h"

Type *current_type = NULL;


PBoolean eq_stype_type(Symbol stype, Type *t)
{
     return (stype == t->name);
}

Type *make_type(Symbol name, Symbol stype)
{
     Type *t, *tmp_type;
     Type *father = NULL;

     if (stype && (stype != object_sym) && (! (father = sl_search_slist(current_oprs->types, stype, eq_stype_type))))
	  fprintf(stderr, LG_STR("ERROR: make_type: Undeclared type %s\n",
				 "ERREUR: make_type: type non declare %s\n"), stype);

     if (! (t = (sl_search_slist(current_oprs->types, name, eq_stype_type)))) {
	 t = MAKE_OBJECT(Type);

	 t->name = name;
	 t->father = father;
	 t->elts = sl_make_slist();
	 t->sur_types = (father?COPY_SLIST(father->sur_types):sl_make_slist());
	 sl_add_to_head(t->sur_types, t); /* add itsef to the list. */
	 sl_add_to_head(current_oprs->types, t); 
     } else if (father) {
	 sl_loop_through_slist(father->sur_types, tmp_type, Type *) {
	     if (! sl_in_slist(t->sur_types, tmp_type))
		 sl_add_to_head(t->sur_types, tmp_type);
	 }
     }
     
     return t;
}

void make_and_print_type(Symbol name, Symbol stype)
{
     print_type(make_type(name, stype));
}

void delete_type(Type *t)
{				
     /* untype/retype all the elts... */
     sl_delete_slist_node(current_oprs->types,t);
     free_type(t);
}

void free_type(Type *t)
{
     FREE_SLIST(t->sur_types);
     FREE_SLIST(t->elts);
     FREE(t);
}

void fprint_type(FILE *f, Type *t)
{
     Type *tmp;

     if (t) {
	  fprintf(f,"Type: %s, sur types: (", t->name);
      
	  sl_loop_through_slist(t->sur_types,tmp, Type *)
	       fprintf(f,"%s ", tmp->name);
	  fprintf(f,")\n");
     } else
	  fprintf(f,"Type: object, contains all the elts.\n");
}

void print_type(Type *t)
{
      fprint_type(stdout, t);
}

void show_type(Symbol stype)
{
     Type *type;
     Symbol tmp;

     if (stype == object_sym) {
	  fprint_type(stdout, NULL);
     } else if (! (type = sl_search_slist(current_oprs->types, stype, eq_stype_type)))
	  fprintf(stderr, LG_STR("ERROR: show_type: Undeclared type %s\n",
				 "ERREUR: show_type: type non declare %s\n"), stype);
     else {
	  print_type(type);
	  
	  printf("\telts: (");
		  
	  sl_loop_through_slist(type->elts, tmp, Symbol) {
	       printf("%s ", tmp);
	  }
	  printf(")\n");
     }
}



#define TYPE_HASHTABLE_MINSIZE 64
#define TYPE_HASHTABLE_DEFAULTSIZE 512 /* has to be a power of 2 for the bitwise modulo */

static int type_hashtable_size, type_hashtable_mod;
Shash *type_hash;

void init_hash_size_type(int hash_size)
{
     if(hash_size == 0)
	  type_hashtable_size = TYPE_HASHTABLE_DEFAULTSIZE;
     else if(hash_size < TYPE_HASHTABLE_MINSIZE) {
	  type_hashtable_size = TYPE_HASHTABLE_MINSIZE;
	  printf (LG_STR("The size of the type hashtable is: %d\n",
			 "La taille de la table des symboles est: %d\n"), 
		  type_hashtable_size);
     } else {
	  int i, size = hash_size -1;
	  for (i = 0; size >0; size = size>>1, i++); 
	  type_hashtable_size = 1<<i;
	  printf (LG_STR("The size of the type hashtable is: %d\n",
			 "La taille de la table des symboles est: %d\n"),
		  type_hashtable_size);
     }
     type_hashtable_mod =  type_hashtable_size - 1;
}

int hash_a_string_type(PString name)
{
     return (hash_a_string(name) & type_hashtable_mod);	/* replaced with a bitwise & because the size = 2**n... */
}

Type_Symbol *make_type_symbol(Type *t, Symbol sym)
{
     Type_Symbol *ts = MAKE_OBJECT(Type_Symbol);

     ts->symbol = sym;
     ts->type = t;

     return ts;
}

void free_type_symbol(Type_Symbol *ts)
{
     FREE(ts);
}

PBoolean match_symbol(Symbol s1, Type_Symbol* ts)
{
     return s1 == ts->symbol;
}


void make_type_hash()
{
     type_hash = sh_make_hashtable(type_hashtable_size, hash_a_string_type, match_symbol);
}

void free_type_symbol_from_hash(void *ignore, Type_Symbol *ts)
{
     free_type_symbol(ts);
}

void free_type_hash(void)
{
     sh_for_all_hashtable(type_hash, NULL, (SL_PFI)free_type_symbol_from_hash);
     sh_free_hashtable(type_hash);
}

void free_type_list(void)
{
     Type *type;

     sl_loop_through_slist(current_oprs->types, type, Type *)
	  free_type(type);
     FREE_SLIST(current_oprs->types);
}

int stat_type_hash_bucket (int *stat_array, PString name)
{
     int index = hash_a_string_type(name);

     stat_array[index] +=1;
     return 1;
}

void stat_type_hashtable(void)
{
     int i, *stat_array;
     int nb_elt, nb_bucket = 0, nb_elt_bucket, nb_max = 0;

     stat_array = (int *) MALLOC(type_hashtable_size * sizeof(int));
     for (i=0; i <type_hashtable_size; i++)
	  stat_array[i] = 0;

     nb_elt = sh_for_all_hashtable(type_hash, stat_array, stat_type_hash_bucket);

     for (i=0; i <type_hashtable_size; i++)
	  if ((nb_elt_bucket = stat_array[i]) != 0){
	       nb_bucket++;
	       if (nb_elt_bucket > nb_max) nb_max = nb_elt_bucket; 
	  }
     printf (LG_STR("The type hashtable contains:\n",
		    "The type hashtable contains:\n"));
     printf (LG_STR("\t%d element(s)\n",
		    "\t%d element(s)\n"), nb_elt);
     printf (LG_STR("\tin  %d buckets ( %d );\n",
		    "\tin  %d buckets ( %d );\n"), nb_bucket, type_hashtable_size);
     printf (LG_STR("\twith a maximal number of %d element(s) in one bucket.\n",
		    "\twith a maximal number of %d element(s) in one bucket.\n"), nb_max);
     FREE(stat_array);
}

Type_Symbol *get_symbol_type(Symbol sym)
{
     return sh_get_from_hashtable(type_hash, sym);
}

Type *get_type(Symbol sym)
{
     Type_Symbol *ts;

     if ((ts = get_symbol_type(sym)))
	  return ts->type;
     else
	  return NULL;
}

void report_type_change(Symbol tfrom, Symbol tto, Symbol sym) 
{
     fprintf(stderr, LG_STR("ERROR: set_var_type_by_name: Changing type of %s from %s to %s \n",
			    "ERREUR: set_var_type_by_name: Changement du type de %s de %s vers %s\n"), 
	     sym, tfrom, tto);
}

void add_elt_to_sur_types(Type *type, Symbol sym)
{
    Type *tmp_type;

    if (type->sur_types->length) {
	sl_loop_through_slist(type->sur_types, tmp_type, Type *){
	    if ((tmp_type != type) && (! sl_in_slist(tmp_type->elts, sym))) {
		sl_add_to_tail(tmp_type->elts, sym);
		add_elt_to_sur_types(tmp_type, sym);
	    }
	}
    }
}

void set_type(Type *type, Symbol sym)
{
     Type_Symbol *ts;

     if (!type) {
	  if ((ts = sh_delete_from_hashtable(type_hash, sym))) {
	       report_type_change(ts->type->name, object_sym, sym);
	       sl_delete_slist_node(ts->type->elts,sym);
	       free_type_symbol(ts);
	  }
     } else  if ((ts = get_symbol_type(sym))) { /* Already declared. */
	  if (type != ts->type)	{
	       report_type_change(ts->type->name, type->name, sym);
	       sl_delete_slist_node(ts->type->elts,sym);
	       sl_add_to_tail(type->elts,sym);
	       add_elt_to_sur_types(type, sym);
	       ts->type = type;
	  }
     } else {
	  ts = make_type_symbol(type,sym);
	  sl_add_to_tail(type->elts,sym);
	  add_elt_to_sur_types(type, sym);
	  sh_add_to_hashtable(type_hash,ts,sym);
     }
}

void set_type_from_reload(Type *type, Symbol sym)
{
     Type_Symbol *ts;

     if ((ts = get_symbol_type(sym))) {
	  if (type != ts->type)	{ /* Already declared. */
	       report_type_change(ts->type->name, type->name, sym);
	       sl_delete_slist_node(ts->type->elts,sym);
	       ts->type = type;
	  }
     } else {
	  ts = make_type_symbol(type,sym);
	  sh_add_to_hashtable(type_hash,ts,sym);
     }
}

void set_type_by_name(Symbol stype, Symbol sym)
{
     Type *type;
     
     if (stype == object_sym) {
	  set_type(NULL,sym);
     } else if ((type = sl_search_slist(current_oprs->types, stype, eq_stype_type)))
	  set_type(type,sym);
     else
	  fprintf(stderr, LG_STR("ERROR: set_type_by_name: Undeclared type %s\n",
				 "ERREUR: set_type_by_name: type non declare %s\n"), stype);

}

void set_var_type_by_name(Symbol stype, Envar *var)
{
     Type *type;
    
     if (stype == object_sym) {
	  if (var->unif_type) {
	       report_type_change(var->unif_type->name, stype, var->name);
	       var->unif_type = NULL;
	  } 
     } else if ((type = sl_search_slist(current_oprs->types, stype, eq_stype_type))) {
	  if (var->unif_type && var->unif_type->name != stype)
	       report_type_change(var->unif_type->name, stype, var->name);
	  var->unif_type=type;
     } else
	  fprintf(stderr, LG_STR("ERROR: set_var_type_by_name: Undeclared type %s\n",
				 "ERREUR: set_var_type_by_name: type non declare %s\n"), stype);
}

PBoolean is_of_type_sym(Symbol sym, Symbol stype)
{
     Type *type, *tmp;
     
     return ((stype == object_sym) ||
	     ((type = sl_search_slist(current_oprs->types, stype, eq_stype_type)) &&
	      (tmp = get_type(sym)) &&
	      sl_in_slist(type->sur_types, tmp)));
}

PBoolean is_of_type(Symbol sym, Type *type)
{
     Type *tmp;
     
     return (!type ||
	     ((tmp = get_type(sym)) &&
	      sl_in_slist(tmp->sur_types, type)));
}

SymList elts_of_type(Symbol stype)
{  
     Type *type;
     
     if (stype == object_sym || !(type = (Type *)sl_search_slist(current_oprs->types, stype, eq_stype_type)))
	  return NULL;

     return type->elts;
}

Term *build_id_from_type(Type *t)
{
     return build_id(t->name);
}

TypeList surtypes_of_type(Symbol stype)
{  
     Type *type;
     
     if (stype == object_sym || !(type = (Type *)sl_search_slist(current_oprs->types, stype, eq_stype_type)))
	  return NULL;
     else {
	  return type->sur_types;	  
     }
}

Symbol type_of(Symbol sym)
{  
     Type *tmp;
     
     if ((tmp = get_type(sym)))
	  return tmp->name;
     else 
	  return object_sym;
}
