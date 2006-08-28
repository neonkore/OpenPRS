static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-load.c -- 
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

#include "config.h"

#ifdef VXWORKS
#include <vxWorks.h>
#endif

 
#include <sys/types.h>
#ifndef WIN95
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>

#include <shashPack.h>
#include <slistPack.h>

#include "string.h"
#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "oprs-pred-func.h"
#include "database.h"
#include "oprs-error.h"
#include "oprs-dump.h"
#include "op-structure.h"
#include "op-instance.h"
#include "intention.h"
#include "fact-goal.h"
#include "int-graph.h"
#include "conditions.h"
#include "oprs-sprint.h"
#include "oprs.h"
#include "type.h"

#ifndef NO_GRAPHIX
#include "xoprs-main.h"
#include "ope-graphic.h"
#include "xoprs-intention.h"
#include "xoprs-intention_f.h"
#include "xt-util_f.h"
#endif

#include <shashPack_f.h>
#include <slistPack_f.h>

#include "oprs-error_f.h"
#include "oprs-type_f.h"
#include "oprs-pred-func_f.h"
#include "lisp-list_f.h"
#include "database_f.h"
#include "op-structure_f.h"
#include "oprs-dump_f.h"
#include "relevant-op_f.h"
#include "op-structure_f.h"
#include "type_f.h"


char *dump_type_name[DPT_MAX] = {
     "DPT_END",
     "DPT_ACTION_FIELD",
     "DPT_BODY",
     "DPT_COND",
     "DPT_EDGE",
     "DPT_ENVAR",
     "DPT_EXPR",
     "DPT_FACT",
     "DPT_FLOAT_ARRAY",
     "DPT_FLUSH",
     "DPT_FRAME",
     "DPT_GOAL",
     "DPT_IG",
     "DPT_INSTRUCTION",
     "DPT_INTENTION",
     "DPT_INT_ARRAY",
     "DPT_IOG",
     "DPT_LENV",
     "DPT_LIST_EDGE",
     "DPT_LIST_EXPR"
     "DPT_LIST_NODE",
     "DPT_LIST_OP",
     "DPT_LIST_PROPERTY",
     "DPT_LIST_SYMBOL",
     "DPT_L_LIST",
     "DPT_NODE",
     "DPT_OG",
     "DPT_OP",
     "DPT_OP_INSTANCE",
     "DPT_PRED_FUNC_REC",
     "DPT_PROPERTY",
     "DPT_SYMBOL",
     "DPT_TERM",
     "DPT_TIB",
     "DPT_TL",
     "DPT_UNDO",
     "DPT_UNDO_LIST",
     "DPT_VAR_LIST",
};

#define TRUE_OR_FALSE_ST(bool) ((bool)?LG_STR("true","vrai"):LG_STR("false","faux"))

int load_file;

TypeList type_conversion;

Symbol load_symbol(void);

Shash *reloc_table = NULL;
static int reloc_table_size = DUMP_LOAD_TABLE_SIZE, reloc_table_mod = DUMP_LOAD_TABLE_SIZE - 1;

int reloc_table_hash_func(void *addr)
{
     return ((unsigned)addr>>3) & reloc_table_mod;
}

PBoolean reloc_table_match_func(void *ad1, Object_Reloc *or)
{
     return (ad1 == or->old_addr);
}

void make_reloc_table()
{
     if (! reloc_table)
	  reloc_table = sh_make_hashtable(reloc_table_size, reloc_table_hash_func, reloc_table_match_func);
}

Object_Reloc *already_referenced(void *addr)
{
     return (Object_Reloc *)sh_get_from_hashtable(reloc_table, addr);
}

void relocate_object(void *old_addr, void *new_addr, Dump_Type dt)
{
     Object_Reloc *or;

     if ((or = already_referenced(old_addr))) {
	  void **addr_ref;

	  if (or->type != dt)
	       fprintf(stderr, LG_STR("relocate_object: type mismatch.\n",
				      "relocate_object: types diffèrent.\n"));


	  if (or->loaded) 
	       fprintf(stderr, LG_STR("relocate_object: Object already loaded.\n",
				      "relocate_object: Objet déjà chargé.\n"));

	  sl_loop_through_slist(or->u.addr_ref_list, addr_ref, void *) {
	       *addr_ref = new_addr;
	  }
	  FREE_SLIST(or->u.addr_ref_list);
     } else {
	  or = MAKE_OBJECT(Object_Reloc);
	  
	  or->refcount = 1;	/* Must be pointed by something anyway... */
	  or->type = dt;
	  or->old_addr = old_addr;
	  sh_add_to_hashtable(reloc_table,or,old_addr);
     }
     or->loaded = TRUE;
     or->u.new_addr = new_addr;
}

void reference_object(void *old_addr, void **addr_ref, Dump_Type dt)
{
     Object_Reloc *or;

     if ((or = already_referenced(old_addr))) {
	  if (or->type != dt)
	       fprintf(stderr, LG_STR("reference_object: type mismatch.\n",
				      "reference_object: types différent.\n"));

	  or->refcount++;
	  if (or->loaded)
	       *addr_ref = or->u.new_addr;
	  else
	       sl_add_to_tail(or->u.addr_ref_list, addr_ref);
     } else {
	  or = MAKE_OBJECT(Object_Reloc);
	  or->type = dt;
	  or->refcount = 1;
	  or->loaded = FALSE;
	  or->old_addr = old_addr;
	  or->u.addr_ref_list = sl_make_slist();
	  sl_add_to_tail(or->u.addr_ref_list, addr_ref);
	  sh_add_to_hashtable(reloc_table,or,old_addr);
     }
}

void reference_unused_object(void *old_addr, Dump_Type dt)
{
     Object_Reloc *or;

     if ((or = already_referenced(old_addr))) {
	  if (or->type != dt)
	       fprintf(stderr, LG_STR("reference_unused_object: type mismatch.\n",
				      "reference_unused_object: types différent.\n"));
     } else {
	  or = MAKE_OBJECT(Object_Reloc);
	  or->type = dt;
	  or->refcount = 0;
	  or->loaded = FALSE;
	  or->old_addr = old_addr;
	  or->u.addr_ref_list = sl_make_slist();
	  sh_add_to_hashtable(reloc_table,or,old_addr);
     }
}

int free_or_from_hash(void *ignore, Object_Reloc *or)
{
     FREE_OBJECT(or);
     return 1;
}

int load_read_from_file(void *buf, size_t nbyte)
{
     int i;

     if ((i = read(load_file,buf,nbyte)) < 0)
	  perror("load_read: read");
     return i;
}

#define LOAD_BUFSIZ BUFSIZ

static unsigned char load_buf[LOAD_BUFSIZ];
static int char_loaded = 0;
static int char_used = 0;

int load_read(void *buf_par, size_t nbyte)
{
     int to_read = nbyte;
     char *buf = buf_par;

     while (to_read) {
	  int char_to_copy;

	  if (char_used == char_loaded) { /* Buf empty... */
	       char_loaded = load_read_from_file(load_buf,LOAD_BUFSIZ);
	       if (char_loaded <= 0) {
		    fprintf(stderr, LG_STR("load_read: read: empty file or problem in file.\n",
					   "load_read: read: fichier vide ou problème avec le fichier.\n"));
		    return nbyte - to_read;
	       }
	       char_used = 0;
	  }
	  char_to_copy = MIN(to_read,char_loaded - char_used);
	  BCOPY(load_buf+char_used,buf+nbyte-to_read,char_to_copy);
	  to_read -= char_to_copy;
	  char_used += char_to_copy ;
	  }
     return nbyte;
}

char load_char(void)
{
     char c;

     load_read(&c, sizeof(char));

     return c;

}

unsigned char load_uchar(void)
{
     unsigned char c;

     load_read(&c, sizeof(unsigned char));

     return c;

}

short load_short(void)
{
     short s;
     load_read(&s, sizeof(short));
     return ntohs(s);
}

#define load_position load_short

unsigned short load_ushort(void)
{
     unsigned short us;
     load_read(&us, sizeof(unsigned short));
     return ntohs(us);
}

#define load_dimension load_ushort

int load_int(void)
{
     int i;
     load_read(&i, sizeof(int));
     return ntohl(i);
}

long load_long(void)
{
     long i;
     load_read(&i, sizeof(long));
     return ntohl(i);
}

void *load_ptr(void)
{
     void *ptr;

     load_read(&ptr, sizeof(void *));
     return (void *)ntohl((u_long)ptr);
}

#define SWAP(x,y,tmp) tmp=y;y=x;x=tmp

void htond(double *dbl, u_char *buf)
{
#if defined(BIG_ENDIAN)
     BCOPY(dbl,buf, 8);
#elif defined(LITTLE_ENDIAN)
     u_char tmp;

     BCOPY(dbl,buf, 8);
     SWAP(buf[0],buf[7],tmp);
     SWAP(buf[1],buf[6],tmp);
     SWAP(buf[2],buf[5],tmp);
     SWAP(buf[3],buf[4],tmp);
#else
#error BIG_ENDIAN or LITTLE_ENDIAN should be defined in config.h.
#endif     
     return;
}

void ntohd(u_char *buf, double *dbl)
{
#if defined(BIG_ENDIAN)
     BCOPY(buf,dbl, 8);
#elif defined(LITTLE_ENDIAN)
     u_char tmp;

     BCOPY(buf,dbl, 8);
     SWAP(buf[0],buf[7],tmp);
     SWAP(buf[1],buf[6],tmp);
     SWAP(buf[2],buf[5],tmp);
     SWAP(buf[3],buf[4],tmp);

     BCOPY(buf,dbl, 8);
#else
#error BIG_ENDIAN or LITTLE_ENDIAN should be defined in config.h.
#endif     
     return;
}

void htonll(long long *ll, u_char *buf)
{
#if defined(BIG_ENDIAN)
     BCOPY(ll,buf, 8);
#elif defined(LITTLE_ENDIAN)
     u_char tmp;

     BCOPY(ll,buf, 8);
     SWAP(buf[0],buf[7],tmp);
     SWAP(buf[1],buf[6],tmp);
     SWAP(buf[2],buf[5],tmp);
     SWAP(buf[3],buf[4],tmp);
#else
#error BIG_ENDIAN or LITTLE_ENDIAN should be defined in config.h.
#endif     
     return;
}

void ntohll(u_char *buf, long long *ll)
{
#if defined(BIG_ENDIAN)
     BCOPY(buf,ll, 8);
#elif defined(LITTLE_ENDIAN)
     u_char tmp;

     BCOPY(buf,ll, 8);
     SWAP(buf[0],buf[7],tmp);
     SWAP(buf[1],buf[6],tmp);
     SWAP(buf[2],buf[5],tmp);
     SWAP(buf[3],buf[4],tmp);

     BCOPY(buf,dbl, 8);
#else
#error BIG_ENDIAN or LITTLE_ENDIAN should be defined in config.h.
#endif     
     return;
}

long long load_long_long(void)
{
     long long hi;
     u_char buf[8];

     load_read(buf, 8);
     ntohll(buf, &hi);
     return hi;
}

double load_float(void)
{
     double dbl;
     u_char buf[8];

     load_read(buf, sizeof(double));
     ntohd(buf,&dbl);

     return dbl;
}

#define LOAD_READ(obj) load_read(&(obj),sizeof(obj))

void flush_reloc_table_and_list()
{
     if (reloc_table) {
	  for_all_hashtable(reloc_table,NULL,free_or_from_hash);
     	  sh_flush_hashtable(reloc_table);
     }
}

void clear_reloc_table_and_list()
{
     flush_reloc_table_and_list();
     if (reloc_table) {
     	  sh_free_hashtable(reloc_table);
	  reloc_table = NULL;
     }
}

static PBoolean comp_old_value = FALSE;
PBoolean save_check_pfr;

int start_load_session(char *filename)
{
     int version;

     clear_reloc_table_and_list();

     comp_old_value = disable_slist_compaction();
     save_check_pfr = check_pfr;
     check_pfr = FALSE;

     reloc_table = sh_make_hashtable(reloc_table_size, reloc_table_hash_func, reloc_table_match_func);
     if ((load_file = open(filename,O_RDONLY,0)) < 0) {
	  perror("start_load_session: open");
	  return load_file;
     }

     char_loaded = char_used = 0;
     version = load_ushort();

     if (version != dump_format_version) {
	  fprintf(stderr, LG_STR("start_load_session: format version differs.\n",
				 "start_load_session: versions de format différent.\n"));
	  close(load_file);
	  return -1;
     }
     return load_file;
}

int check_or_and_refcount_from_hash(Slist *not_loaded, Object_Reloc *or)
{
     if (! or->loaded) {
#ifdef NO_GRAPHIX
	  if  (or->type == DPT_OG || /* These are the allowed UNUSED addr. */
	       or->type == DPT_IOG) { 
	       FREE_SLIST(or->u.addr_ref_list);
	  } else
#endif
	       sl_add_to_tail(not_loaded,or);
     } else /* These are the types for which we do not refcount. */
	  if (!(or->type == DPT_SYMBOL || or->type == DPT_NODE || or->type == DPT_EDGE ||
		or->type == DPT_OG || or->type == DPT_PRED_FUNC_REC)) {
	       if (or->refcount)
		    while (or->refcount-- != 1)
			 DUP(or->u.new_addr);
	       else
		    FREE(or->u.new_addr); /* It was referenced, loaded but is not really used */
	  }
     return 1;
}

void flush_load_session()
{
     Slist *not_loaded = sl_make_slist();
     Object_Reloc *or;

     if (!be_quiet) printf("+");
     for_all_hashtable(reloc_table,not_loaded,check_or_and_refcount_from_hash);
     if (!sl_slist_empty(not_loaded)) {
	  fprintf(stderr, LG_STR("flush_load_session: %d object(s) are not loaded.\n",
				 "flush_load_session: %d objet(s) n'ont pas été chargé(s).\n"),
		  sl_slist_length(not_loaded));
	  sl_loop_through_slist(not_loaded, or, Object_Reloc *) {
	       void **addr_ref;
	   
	       fprintf(stderr,"type: %s, old_addr: %#x, # referencers: %d.\n",
		       dump_type_name[or->type], (unsigned int)or->old_addr,
		       sl_slist_length(or->u.addr_ref_list));
	       sl_loop_through_slist(or->u.addr_ref_list, addr_ref, void *) {
		    *addr_ref = NULL;
	       }
	       FREE_SLIST(or->u.addr_ref_list);
	  }
     }
     FREE_SLIST(not_loaded);
     flush_reloc_table_and_list();
}

void end_load_session()
{
     flush_load_session();
     if (!be_quiet) printf("*");
     clear_reloc_table_and_list();
     if (load_file >= 0) close(load_file);
     char_loaded = char_used = 0;
     if (comp_old_value) enable_slist_compaction();

     check_pfr = save_check_pfr;
}

#define DUMMY_ADDR (void *)0x5555

#define LOAD_UNUSED_ADDR(type) \
do {void *addr = load_ptr(); \
    if (addr) reference_unused_object(addr,type); \
} while(0)

void load_unused_list_reloc_elt(Dump_Type dt)
{
     unsigned short length,i;

     length = load_ushort();
     load_short();		/* the type */
     for(i=length;i;i--) {
	  LOAD_UNUSED_ADDR(dt);
     }
}

void safe_load_unused_list_reloc_elt(Dump_Type dt)
{
     if (load_boolean()) load_unused_list_reloc_elt(dt);
}
     
void load_unused_list_elt_here(void (*load_function)(void))
{     
     unsigned short length,i;

     length = load_ushort();
     load_short();		/* the type */
     for(i=length;i;i--) {
	  load_function();
     }
}

void *load_list_reloc_elt(Dump_Type dt)
{
     Slist *l;
     unsigned short length,i;
     Slist_Type type;

     l=sl_make_slist();
     length = load_ushort();
     type = load_short();

     if (type == SLT_DYNAMIC) {
	  for(i=length;i;i--)
	       LOAD_ADDR_AND_REF_LOC_ADDR(dt,sl_add_to_tail_ret_ref(l,DUMMY_ADDR));
     } else {
	  l->length = length;
	  l->dynamic = length + 1;
	  switch (length) {
	  case 0: 
	       l->current.sd = NULL;
	       l->first.sd = NULL;
	       l->last.sd = NULL;
	       l->type = SLT_EMPTY;
	       break;
	  case 1:
	       l->current.sd = NULL;
	       LOAD_ADDR_AND_REF_LOC(dt,l->first.sd);
	       l->last.sd = NULL;
	       l->type = SLT_SINGLE;
	       break;
	  case 2: {
	       l->current.sd = NULL;
	       LOAD_ADDR_AND_REF_LOC(dt,l->first.sd);
	       LOAD_ADDR_AND_REF_LOC(dt,l->last.sd);
	       l->type = SLT_DOUBLE;
	       break;
	  }
	  default: {
	       void **array = MALLOC(l->length*sizeof(void *));

	       for(i=0;i<length;i++) {
		    LOAD_ADDR_AND_REF_LOC_ADDR(dt,array + i);
	       }
	       l->current.st = NULL;
	       l->first.st = array;
	       l->last.st = array + length - 1;
	       l->type = SLT_STATIC;
	       break;
	  }
	  }
     }
     return l;
}

void *safe_load_list_reloc_elt(Dump_Type dt)
{

     if (load_boolean()) return load_list_reloc_elt(dt);
     else return NULL;
}
     
void *load_list_elt_here(void * (*load_function)(void))
{     
     Slist *l;
     Slist_Type type;
     unsigned short length,i;

     l=sl_make_slist();
     length = load_ushort();
     type = load_short();

     if (type == SLT_DYNAMIC) {
	  for(i=length;i;i--)
	       sl_add_to_tail(l,load_function());
     } else {
	  l->length = length;
	  l->dynamic = length + 1;
	  switch (length) {
	  case 0: 
	       l->current.sd = NULL;
	       l->first.sd = NULL;
	       l->last.sd = NULL;
	       l->type = SLT_EMPTY;
	       break;
	  case 1:
	       l->current.sd = NULL;
	       l->first.sd = load_function();
	       l->last.sd = NULL;
	       l->type = SLT_SINGLE;
	       break;
	  case 2: {
	       l->current.sd = NULL;
	       l->first.sd = load_function();
	       l->last.sd = load_function();
	       l->type = SLT_DOUBLE;
	       break;
	  }
	  default: {
	       void **array = MALLOC(l->length*sizeof(void *));

	       for(i=0;i<length;i++) {
		    *(array + i) = load_function();
	       }
	       l->current.st = NULL;
	       l->first.st = array;
	       l->last.st = array + length - 1;
	       l->type = SLT_STATIC;
	       break;
	  }
	  }
     }
     return l;
}

void *safe_load_list_elt_here(void * (*load_function)(void))
{
     if (load_boolean()) return load_list_elt_here(load_function);
     else return NULL;
}

void *load_property(void)
{
     Property *p = MAKE_OBJECT(Property);

     LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,p->name);
     LOAD_ADDR_AND_REF_LOC(DPT_TERM,p->value);

     return p;
}

void *load_list_property(void)
{
     return load_list_reloc_elt(DPT_PROPERTY);
}

void *load_var_list(void)
{
     return load_list_reloc_elt(DPT_ENVAR);
}

void *load_undo_list(void)
{
    return load_list_reloc_elt(DPT_UNDO);
}

void *load_frame(void)
{
     FramePtr fp = MAKE_OBJECT(Frame);

     fp->installe = load_boolean();
     LOAD_ADDR_AND_REF_LOC(DPT_UNDO_LIST,fp->list_undo);
     LOAD_ADDR_AND_REF_LOC(DPT_FRAME,fp->previous_frame);

     return fp;
}

void *load_lenv(void)
{
     return load_list_reloc_elt(DPT_ENVAR);
}

void *load_envar(void)
{
     Variable_Type type;
     Envar *var;
     Symbol name;

     type = load_char();
     name = load_symbol();	/* Need the name right away... */
     
     if ((type == PROGRAM_VARIABLE) && (name[1] == '@')) {
	  if((var = (Envar *) sl_search_slist(global_var_list, name, match_var))) {
	       LOAD_UNUSED_ADDR(DPT_TERM);
	       dup_envar(var);		/* Tricky... but required. */
	       return var;		/* It is a global and it alrady exists... */
	  } else {
	       var = MAKE_OBJECT(Envar); 	/* It is a new global... */
	       sl_add_to_head(global_var_list, var);
	  }
     } else {	
	  var = MAKE_OBJECT(Envar);
     }
     
     var->type = type;
     var->name = name;
     LOAD_ADDR_AND_REF_LOC(DPT_TYPE,var->unif_type);
     LOAD_ADDR_AND_REF_LOC(DPT_TERM,var->value);	/* The value can be loaded now... */

     return var;
}


void *load_list_symbol(void)
{
     return load_list_reloc_elt(DPT_SYMBOL);
}

void *load_list_type(void)
{
     return load_list_reloc_elt(DPT_TYPE);
}

void *load_type(void)
{
     Symbol name;
     Type *t;

     name = load_symbol();
     if ((t = sl_search_slist(current_oprs->types, name, eq_stype_type))) {
	  SymList elts;
	  fprintf(stderr, LG_STR("ERROR: load_kernel_from_parser: Type already defined: %s\n\
\tusing already loaded version.\n",
				 "ERREUR: load_kernel_from_parser: Type deja defini: %s\n\
\tutilisation de la version deja definie.\n"), name);
	  
	  LOAD_UNUSED_ADDR(DPT_TYPE);
	  load_unused_list_reloc_elt(DPT_TYPE);
	  elts = load_list_symbol();
	  sl_add_to_tail(type_conversion, t);
	  sl_add_to_tail(type_conversion, elts);
     } else {
	  t = MAKE_OBJECT(Type);
	  t->name = name;
	  LOAD_ADDR_AND_REF_LOC(DPT_TYPE,t->father);
	  t->sur_types = load_list_type();
	  t->elts = load_list_symbol();
     }

     return t;
}


void *load_undo(void)
{
     Undo *u = MAKE_OBJECT(Undo);

     LOAD_ADDR_AND_REF_LOC(DPT_ENVAR,u->envar);
     LOAD_ADDR_AND_REF_LOC(DPT_TERM,u->value);

     return u;
}

void *load_int_array(void)
{
     unsigned short i; 
     Int_array *ia = MAKE_OBJECT(Int_array);

     ia->size = load_ushort();
     ia->array = (int *)MALLOC(sizeof(int) * ia->size);
     for(i = 0; i < ia->size ; i++) {
	  ia->array[i] = load_int();
     }
     
     return ia;
}

void *load_float_array(void)
{
     unsigned short i; 
     Float_array *fa = MAKE_OBJECT(Float_array);

      fa->size = load_ushort();
     fa->array = (double *)MALLOC(sizeof(double) * fa->size);

     for(i = 0; i < fa->size ; i++) {
	  fa->array[i] = load_float();
     }
     return fa;
}

void *load_expr(void)
{
     Expression *expr = MAKE_OBJECT(Expression);

     LOAD_ADDR_AND_REF_LOC(DPT_PRED_FUNC_REC, expr->pfr);
     LOAD_ADDR_AND_REF_LOC(DPT_TL,expr->terms);

     return expr;
}

char *load_string(void)
{
     unsigned short size;
     char *string;

     size = load_ushort();
     string = MALLOC(sizeof(char) * (size + 1));
     load_read(string,size);
     string[size] = '\0';

     return string;
}

char *safe_load_string(void)
{
     if (load_boolean()) return load_string();
     else return NULL;
}

Symbol load_symbol(void)
{
     char *string = load_string();
     Symbol sym;

     DECLARE_ID(string,sym);
     FREE(string);

     return sym;
}

Symbol safe_load_symbol(void)
{
     if (load_boolean()) return load_symbol();
     else return NULL;
}

void *load_pred_func_rec(void)
{
     Pred_Func_Rec *pfr;
     Symbol name;
     PBoolean ef, act, old_pfr;

     name = safe_load_symbol();	/* Need the name right away... */

     if (name) {
	  PBoolean pr_defined;

	  old_pfr = (find_pred_func(name) != NULL);

	  pfr = find_or_create_pred_func(name);

	  ef = load_boolean();
	  act = load_boolean();

	  if (old_pfr) {
	       if (!ef != !(pfr->u.u.ef))
		    fprintf(stderr, LG_STR("load_func_rec: %s ef status of the loading (%s) and loaded (%s) elt differs.\n",
					   "load_func_rec: %s ef propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(ef), TRUE_OR_FALSE_ST(pfr->u.u.ef));

	       if (!act != (!pfr->u.u.act)) 
		    fprintf(stderr, LG_STR("load_func_rec: %s act status of the loading (%s) and loaded (%s) elt differs.\n",
					   "load_func_rec: %s act propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(act), TRUE_OR_FALSE_ST(pfr->u.u.act));
	  } else {
	       if (ef)
		    fprintf(stderr, LG_STR("load_func_rec: %s was saved as evaluable.\n",
					   "load_func_rec: %s avait été sauvée évaluable.\n"), name);
	       if (act)
		    fprintf(stderr, LG_STR("load_func_rec: %s was saved as an action.\n",
					   "load_func_rec: %s avait été sauvé comme action.\n"), name);
	       pfr->u.u.ef = NULL;
	       pfr->u.u.act = NULL;
	  }
	  
	  pr_defined = load_boolean();

	  if (pr_defined) {
	       signed char arity, ff;
	       PBoolean ep, cwp, be, op_pred;
	       Pred_Rec *old_pr, *pr;

	       ep = load_boolean();
	       ff = load_char();
	       arity = load_char();
	       cwp = load_boolean();
	       be = load_boolean();
	       op_pred = load_boolean();

	       if (old_pfr && (old_pr = pfr->u.u.pr)) {
		    if (!ep != !(old_pr->ep))
			 fprintf(stderr, LG_STR("load_pred_rec: %s, evaluable-predicat status of the loading (%s) and loaded elt (%s) differs.\n",
						"load_pred_rec: %s, evaluable-predicat propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(ep), TRUE_OR_FALSE_ST(old_pr->ep));

		    if (ff != old_pr->ff)
			 fprintf(stderr, LG_STR("load_pred_rec: %s, functional-fact status of the loading (%s) and loaded (%s) elt differs.\n",
						"load_pred_rec: %s, functional-fact propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(ff), TRUE_OR_FALSE_ST(old_pr->ff));

		    if (arity != old_pr->arity) {
			 if (old_pr->arity != -1)
			      fprintf(stderr, LG_STR("load_pred_rec: %s, arity status of the loading (%d) and loaded (%d) elt differs.\n",
						     "load_pred_rec: %s, arity propriété de la version à charger (%d) et chargée (%d) différent.\n"), name, arity, old_pr->arity);
			 else
			      old_pr->arity = arity;
		    }
		    if (cwp != old_pr->cwp)
			 fprintf(stderr, LG_STR("load_pred_rec: %s, cwp status of the loading (%s) and loaded (%s) elt differs.\n",
						"load_pred_rec: %s, cwp propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(cwp), TRUE_OR_FALSE_ST(old_pr->cwp));

		    if (be != old_pr->be)
			 fprintf(stderr, LG_STR("load_pred_rec: %s, basic-event status of the loading (%s) and loaded (%s) elt differs.\n",
						"load_pred_rec: %s, basic-event propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(be), TRUE_OR_FALSE_ST(old_pr->be));

		    if ((op_pred != old_pr->op_pred) && ! (op_pred)) /* if promote op_pred, OK. */
			 fprintf(stderr, LG_STR("load_pred_rec: %s, op_pred status of the loading (%s) and loaded (%s) elt differs.\n",
						"load_pred_rec: %s, op_pred propriété de la version à charger (%s) et chargée (%s) différent.\n"), name, TRUE_OR_FALSE_ST(op_pred), TRUE_OR_FALSE_ST(old_pr->op_pred));
	       } else {
		    pfr->u.u.pr = pr = make_pred_rec();

		    if (ep)
			 fprintf(stderr, LG_STR("load_pred_rec: %s, was saved as evaluable.\n",
						"load_pred_rec: %s, avait été sauvé évaluable.\n"), name);
		    pr->ep = NULL; 
		    pr->ff = ff; 
		    pr->arity = arity; 
		    pr->cwp = cwp; 
		    pr->be = be; 
		    pr->op_pred = op_pred; 
	       }
	  } else {
	       pfr->u.u.pr = NULL;
	  }

     } else {
	  Envar *var;
	  var = load_envar();
	  pfr = create_var_pred_func(var);
     }
     return pfr;
}

void *load_tl(void)
{
     return load_list_reloc_elt(DPT_TERM);
}

void *load_l_list(void)
{
     L_List cons,final_res  = l_nil;
     L_List *back_cdr = NULL;
     unsigned short length,i;

     length = load_ushort();
     for(i=length;i;i--) {
	  if (i == length)	/* first iteration */
	       final_res = cons = MAKE_OBJECT(struct l_list);
	  else 
	       cons = MAKE_OBJECT(struct l_list);
	  if (back_cdr) *back_cdr = cons; /* subsequent iteration */
	  back_cdr = &(cons->cdr);
	  cons->cdr = l_nil;
	  cons->size = i;
	  LOAD_ADDR_AND_REF_LOC(DPT_TERM,cons->car);
     }
     return final_res;
}
 
void *load_term(void)
{
     Term *term = MAKE_OBJECT(Term);

     term->type = load_char();
     switch (term->type) {
     case INTEGER: 	 
	  term->u.intval = load_int();
	  break;
     case LONG_LONG: 	 
	  term->u.llintval = load_long_long();
	  break;
     case TT_FACT:
	  LOAD_ADDR_AND_REF_LOC(DPT_FACT, term->u.fact);
	  break;
     case TT_GOAL:
	  LOAD_ADDR_AND_REF_LOC(DPT_GOAL,term->u.goal);
	  break;
     case TT_OP_INSTANCE:
	  LOAD_ADDR_AND_REF_LOC(DPT_OP_INSTANCE,term->u.opi);
	  break;
     case TT_INTENTION:
	  LOAD_ADDR_AND_REF_LOC(DPT_INTENTION, term->u.in);
	  break;
     case U_MEMORY: 	 
	  LOAD_ADDR_AND_REF_LOC(DPT_U_MEMORY, term->u.u_memory);
	  break;
     case U_POINTER: 	 
	  term->u.u_pointer = load_ptr();
	  break;
     case TT_FLOAT: 	 
	  term->u.doubleptr = make_doubleptr(load_float());
	  break;
     case STRING:
	  term->u.string = load_string();
	  break;
     case TT_ATOM:
	  LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,term->u.id);
	  break;
     case VARIABLE:
	  LOAD_ADDR_AND_REF_LOC(DPT_ENVAR,term->u.var);
	  break;
     case EXPRESSION:
	  LOAD_ADDR_AND_REF_LOC(DPT_EXPR,term->u.expr);
	  break;
     case INT_ARRAY: 	 
	  LOAD_ADDR_AND_REF_LOC(DPT_INT_ARRAY,term->u.int_array);
	  break;
     case FLOAT_ARRAY: 	 
	  LOAD_ADDR_AND_REF_LOC(DPT_FLOAT_ARRAY,term->u.float_array);
	  break;
     case LISP_LIST: 	 
	  LOAD_ADDR_AND_REF_LOC(DPT_L_LIST,term->u.l_list);
	  break;
     case C_LIST:
	  break;
     }
     
     return term;
}

void *load_list_expr()
{
     return load_list_reloc_elt(DPT_EXPR);
}

void *load_list_op(void)
{
     return load_list_reloc_elt(DPT_OP);
}

Op_Instance *load_op_instance(void)
{
     Op_Instance *op_inst = MAKE_OBJECT(Op_Instance);
     char type = load_char();

     if (type == 1)
	  op_inst->op = dup_op(goal_satisfied_in_db_op_ptr);
     else if (type == 2)
	  op_inst->op = dup_op(goal_for_intention_op_ptr);
     else if (type == 3) 
	  op_inst->op = dup_op(goal_waiting_op_ptr);
     else if (type == 0)
	  LOAD_ADDR_AND_REF_LOC(DPT_OP,op_inst->op);
     else
	  fprintf(stderr, LG_STR("ERROR: load_op_instance: unknown op type.\n",
				 "ERREUR: load_op_instance: type d'op inconnu.\n"));

     LOAD_ADDR_AND_REF_LOC(DPT_GOAL,op_inst->goal);
     LOAD_ADDR_AND_REF_LOC(DPT_FACT,op_inst->fact);
     LOAD_ADDR_AND_REF_LOC(DPT_FRAME,op_inst->frame);
     LOAD_ADDR_AND_REF_LOC(DPT_LENV,op_inst->lenv);
     op_inst->intended = load_boolean();
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,op_inst->maintain);
     
     return op_inst;
}


U_Memory *load_u_memory(void)
{
     U_Memory *um = MAKE_OBJECT(U_Memory);

     um->size = load_int();
     um->mem = MALLOC(um->size);

     load_read(um->mem,um->size);

     return um;
}

Goal *load_goal(void)
{
     Goal *goal = MAKE_OBJECT(Goal);
     
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,goal->statement);
     LOAD_ADDR_AND_REF_LOC(DPT_FRAME,goal->frame);
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,goal->interne);
     LOAD_ADDR_AND_REF_LOC(DPT_TIB,goal->tib);
     LOAD_ADDR_AND_REF_LOC(DPT_OP_INSTANCE,goal->succes);
     goal->echec = safe_load_list_reloc_elt(DPT_OP_INSTANCE);
     LOAD_ADDR_AND_REF_LOC(DPT_EDGE,goal->edge);
     LOAD_ADDR_AND_REF_LOC(DPT_NODE,goal->cp);
     goal->echoue = load_uchar();
     goal->pris_en_compte = load_boolean();
     goal->etat = load_boolean();
#ifdef OPRS_PROFILING
     PDate creation;	
     PDate reception;
     PDate soak;
     PDate reponse;
#endif
     return goal;
}

Fact *load_fact(void)
{
     Fact *fact = MAKE_OBJECT(Fact);
     
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,fact->statement);
     fact->pris_en_compte = load_boolean();
     LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,fact->message_sender);
#ifdef OPRS_PROFILING		/* Not implemented for dump for now */
     int executed_ops;
     PDate creation;		/* Date de creation, reception, soak et reponse */
     PDate reception;
     PDate soak;
     PDate reponse;
#endif

     return fact;
}

PDate *load_oprs_date(PDate *pd)
{
     pd->tv_sec = load_long();
     pd->tv_usec = load_long();
     return pd;
}

Sprinter *load_sprinter(void)
{
     Sprinter *sp = MAKE_OBJECT(Sprinter);
     
     sp->str = load_string();
     sp->size = sp->tsize = strlen(sp->str)+1;
     return sp;
}

Relevant_Condition *load_condition(void)
{
     Relevant_Condition *rc = MAKE_OBJECT(Relevant_Condition);
     
     rc->type = load_uchar();
     rc->evolving = load_boolean();
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,rc->expr);
     LOAD_ADDR_AND_REF_LOC(DPT_FRAME,rc->frame);

     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,rc->u.intention);
	  break;
     case TIB_ACTIVATION:
     case MAINTAIN_ACTIVATION:
     case ACTION_WAIT_ACTIVATION:
	  LOAD_ADDR_AND_REF_LOC(DPT_TIB,rc->u.tib);
     }
     return rc;
}

void report_graphix_mismatch()
{
     static PBoolean reported = FALSE;
     char *mess= "WARNING: Graphic information mismatch.\n\
\tGraphic loaded in a non graphic kernel or vice verca.\n\
\tIf you are in a graphic environement, the OP are executable,\n\
\tbut will not display on the screen.\n\
\tIf you are in a non graphic environement, the graphical\n\
\tinformation is discarded.\n\
\tThis message will be printed once.\n";

     if (! reported) {
	  fprintf(stderr,"%s",mess);
#ifndef NO_GRAPHIX
	  printf("%s",mess);
#endif
	  reported = TRUE;
     }
}

Intention *load_intention(void)
{
     PBoolean graphix;
     Intention *intention = MAKE_OBJECT(Intention);
     
     LOAD_ADDR_AND_REF_LOC(DPT_FACT,intention->fact);
     LOAD_ADDR_AND_REF_LOC(DPT_GOAL,intention->goal);
     intention->fils = load_list_reloc_elt(DPT_TIB);
     intention->active_tibs = load_list_reloc_elt(DPT_TIB);
     LOAD_ADDR_AND_REF_LOC(DPT_OP_INSTANCE,intention->top_op);
     intention->priority = load_short();
     intention->status = load_uchar();
     LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,intention->id);
     LOAD_ADDR_AND_REF_LOC(DPT_TIB,intention->critical_section);
     load_oprs_date(&(intention->creation));
     intention->activation_conditions_list = load_list_reloc_elt(DPT_COND);
#ifdef OPRS_PROFILING
     PDate active;	
#endif
     intention->failed_goal_sprinter = load_sprinter();
     intention->failed_goal_stack = sl_make_slist();
     graphix = load_boolean();
#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  LOAD_UNUSED_ADDR(DPT_IOG);
     }
#else
     if (graphix) {
	  LOAD_ADDR_AND_REF_LOC(DPT_IOG,intention->iog);
	  intention->trace_dialog = NULL;
	  intention->trace_scrl_txt = NULL;
     } else {
	  report_graphix_mismatch();
	  intention->iog = NULL;
	  intention->trace_dialog = NULL;
	  intention->trace_scrl_txt = NULL;
     }
#endif
     return intention;
}

Thread_Intention_Block *load_tib(void)
{
     Thread_Intention_Block *tib = MAKE_OBJECT(Thread_Intention_Block);
     
     tib->status = load_uchar();
     if (tib->status != TIBS_DEAD) { /* It is virtually gone... */
	  LOAD_ADDR_AND_REF_LOC(DPT_OP_INSTANCE,tib->curr_op_inst);
	  LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,tib->intention);
	  LOAD_ADDR_AND_REF_LOC(DPT_TIB,tib->pere);
	  tib->fils = load_list_reloc_elt(DPT_TIB);
	  LOAD_ADDR_AND_REF_LOC(DPT_NODE,tib->current_node);
	  LOAD_ADDR_AND_REF_LOC(DPT_GOAL,tib->current_goal);
	  tib->list_to_try = safe_load_list_reloc_elt(DPT_EDGE);
	  LOAD_ADDR_AND_REF_LOC(DPT_EXPR,tib->maintain);
	  LOAD_ADDR_AND_REF_LOC(DPT_COND,tib->activation_condition);
	  LOAD_ADDR_AND_REF_LOC(DPT_COND,tib->maintain_condition);
	  LOAD_ADDR_AND_REF_LOC(DPT_TIB,tib->suspended);
	  tib->num_called = load_ushort();
     }
     return tib;
}

void *load_intention_paire(void)
{
     PBoolean graphix;
     Intention_Paire *ip = MAKE_OBJECT(Intention_Paire);
     
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,ip->first);
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,ip->second);

     graphix = load_boolean();
#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  LOAD_UNUSED_ADDR(DPT_IOG);
     }
#else
     if (graphix) {
	  LOAD_ADDR_AND_REF_LOC(DPT_IOG,ip->iog);
     } else {
	  report_graphix_mismatch();
	  ip->iog = NULL;
     }
#endif
     return ip;
}

Intention_Graph *load_ig(void)
{
     PBoolean graphix;
     Intention_Graph *ig = MAKE_OBJECT(Intention_Graph);

     ig->list_intentions = load_list_reloc_elt(DPT_INTENTION);
     ig->list_first = load_list_reloc_elt(DPT_INTENTION);
     ig->list_runnable = load_list_reloc_elt(DPT_INTENTION);
     ig->list_condamned = load_list_reloc_elt(DPT_INTENTION);
     ig->list_pairs = load_list_elt_here(load_intention_paire);
     ig->current_intentions = load_list_reloc_elt(DPT_INTENTION);
     ig->sleeping = load_list_reloc_elt(DPT_INTENTION);
     ig->sleeping_on_cond = load_list_reloc_elt(DPT_INTENTION);
     /* PFB sort_predicat; */
     graphix = load_boolean();
#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  load_unused_list_reloc_elt(DPT_IOG);
	  load_unused_list_reloc_elt(DPT_IOG);
     }
#else
     if (graphix) {
	  ig->list_inode = load_list_reloc_elt(DPT_IOG);
	  ig->list_iedge = load_list_reloc_elt(DPT_IOG);
     } else {
	  report_graphix_mismatch();
	  ig->list_inode = sl_make_slist();
	  ig->list_iedge = sl_make_slist();
     }
#endif
     return ig;
}

#ifdef NO_GRAPHIX
void load_xmstring(void)
{
     while (load_boolean()) {
	  FREE(load_string());
	  FREE(load_string());
	  load_uchar();
	  load_boolean();
     }
}

void load_gnode(void)
{
     load_dimension();
     load_dimension();
     load_xmstring();
     LOAD_UNUSED_ADDR(DPT_NODE);
}

void load_gedge(void)
{
     load_position();
     load_position();
     load_position();
     load_position();
     load_position();
     load_position();
     load_position();
     load_position();
     LOAD_UNUSED_ADDR(DPT_EDGE);
     LOAD_UNUSED_ADDR(DPT_OG);
     load_unused_list_reloc_elt(DPT_OG);
}

void load_gtext_string(void)
{
     load_dimension();
     load_dimension();
     load_xmstring();
}

void load_gedge_text(void)
{
     load_position();
     load_position();
     FREE(load_string());
     load_dimension();
     load_boolean();
     load_unused_list_elt_here(load_gtext_string);
     LOAD_UNUSED_ADDR(DPT_OG);
}

void load_gtext(void)
{
     load_char();
     FREE(load_string());
     load_dimension();
     load_boolean();
     load_boolean();
     load_unused_list_elt_here(load_gtext_string);
     safe_load_unused_list_reloc_elt(DPT_OG);
}
     

void load_ginst(void)
{
     LOAD_UNUSED_ADDR(DPT_INSTRUCTION);
     LOAD_UNUSED_ADDR(DPT_EDGE);
}

void load_gknot(void)
{
     load_position();
     load_position();
     LOAD_UNUSED_ADDR(DPT_OG);
}

void load_og(void)
{
     Draw_Type type;

     type = load_char();
     load_position();
     load_position();
     load_dimension();
     load_dimension();
     load_boolean();

     switch (type) {
     case DT_IF_NODE:
     case DT_THEN_NODE:
     case DT_ELSE_NODE:
     case DT_NODE:
	  load_gnode();
	  break;
     case DT_EDGE:
	  load_gedge();
	  break;
     case DT_EDGE_TEXT:
	  load_gedge_text();
	  break;
     case DT_TEXT:
	  load_gtext();
	  break;
     case DT_INST:
	  load_ginst();
	  break;
     case DT_KNOT:
	  load_gknot();
     }
}

void load_list_og(void)
{
     load_unused_list_reloc_elt(DPT_OG);
}

void load_iog(void);

void load_ginode(void)
{
     LOAD_UNUSED_ADDR(DPT_INTENTION);
     load_unused_list_reloc_elt(DPT_IOG);
}

void load_giedge(void)
{
     LOAD_UNUSED_ADDR(DPT_INTENTION);
     LOAD_UNUSED_ADDR(DPT_INTENTION);
}

void load_iog(void)
{
     IDraw_Type dt;

     dt = load_char();
     switch (dt) {
     case INODE:
	  load_ginode();
	  break;
     case IEDGE:
	  load_giedge();
	  break;
     }
}
#else
XmString load_xmstring(void)
{
     XmString res=NULL;
     
     while (load_boolean()) {
	  char *text;
	  XmStringCharSet cs;
	  XmStringDirection dir;
	  Boolean sep;
	  XmString res2;
	  
	  text = load_string();
	  cs = load_string();
	  dir = load_uchar();
	  sep = load_boolean();

	  res2 = XmStringSegmentCreate(text,cs,dir,sep);

	  FREE(text);
	  FREE(cs);

	  if (res) {
	       XmString to_free = res;

	       res = XmStringConcat(res, res2);
	       XmStringFree(to_free);
	       XmStringFree(res2);
	  } else
	       res = res2;
     }
     
     return res;
}

Gnode *load_gnode(void)
{
     Gnode *gnode = MAKE_OBJECT(Gnode);

     gnode->swidth = load_dimension();
     gnode->sheight = load_dimension();
     gnode->xmstring = load_xmstring();
     LOAD_ADDR_AND_REF_LOC(DPT_NODE,gnode->node);
     return gnode;
}

Gedge *load_gedge(void)
{
     Gedge *gedge = MAKE_OBJECT(Gedge);

     gedge->x1 = load_position();
     gedge->y1 = load_position();
     gedge->x2 = load_position();
     gedge->y2 = load_position();
     gedge->fx1 = load_position();
     gedge->fx2 = load_position();
     gedge->fy1 = load_position();
     gedge->fy2 = load_position();
     LOAD_ADDR_AND_REF_LOC(DPT_EDGE, gedge->edge);
     LOAD_ADDR_AND_REF_LOC(DPT_OG, gedge->text);
     gedge->list_knot = load_list_reloc_elt(DPT_OG);

     return gedge;
}

void *load_gtext_string(void)
{
     Gtext_String *gtext_string = MAKE_OBJECT(Gtext_String);

     gtext_string->off_x = load_dimension();
     gtext_string->off_y = load_dimension();
     gtext_string->xmstring = load_xmstring();

     return gtext_string;
}

Gedge_text *load_gedge_text(void)
{
     Gedge_text *gedge_text = MAKE_OBJECT(Gedge_text);

     gedge_text->dx = load_position();
     gedge_text->dy = load_position();
     gedge_text->log_string = load_string();
     gedge_text->text_width = load_dimension();
     gedge_text->fill_lines = load_boolean();
     gedge_text->lgt_log_string = load_list_elt_here(load_gtext_string);
     LOAD_ADDR_AND_REF_LOC(DPT_OG, gedge_text->edge);

     return gedge_text;
}



Gtext *load_gtext(void)
{
     Gtext *gtext = MAKE_OBJECT(Gtext);

     gtext->text_type = load_char();
     gtext->string = load_string();
     gtext->text_width = load_dimension();
     gtext->fill_lines = load_boolean();
     gtext->visible = load_boolean();
     gtext->lgt_string = load_list_elt_here(load_gtext_string);
     gtext->list_og_inst = safe_load_list_reloc_elt(DPT_OG);

     return gtext;
}
     

Ginst *load_ginst(void)
{
     Ginst *ginst = MAKE_OBJECT(Ginst);

     LOAD_ADDR_AND_REF_LOC(DPT_INSTRUCTION, ginst->inst);
     LOAD_ADDR_AND_REF_LOC(DPT_EDGE, ginst->edge);
     ginst->num_line = ginst->nb_lines = ginst->indent = ginst->width = 0;

     return ginst;
}

Gknot *load_gknot(void)
{
     Gknot *gknot = MAKE_OBJECT(Gknot);

     gknot->x = load_position();
     gknot->y = load_position();
     LOAD_ADDR_AND_REF_LOC(DPT_OG, gknot->edge);

     return gknot;
}

OG *load_og(void)
{
     XRectangle rect;
     OG *og = MAKE_OBJECT(OG);

     og->type = load_char();
     og->x = load_position();
     og->y = load_position();
     og->width = load_dimension();
     og->height = load_dimension();
     
     rect.width = og->width;
     rect.height = og->height;
     rect.x = og->x;
     rect.y = og->y;

     og->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og->region, og->region);

     og->selected = load_boolean();
     switch (og->type) {
     case DT_IF_NODE:
     case DT_THEN_NODE:
     case DT_ELSE_NODE:
     case DT_NODE:
	  og->u.gnode = load_gnode();
	  break;
     case DT_EDGE:
	  og->u.gedge = load_gedge();
	  break;
     case DT_EDGE_TEXT:
	  og->u.gedge_text = load_gedge_text();
	  break;
     case DT_TEXT:
	  og->u.gtext = load_gtext();
	  break;
     case DT_INST:
	  og->u.ginst = load_ginst();
	  break;
     case DT_KNOT:
	  og->u.gknot = load_gknot();
     }
     return og;
}

List_OG  load_list_og(void)
{
     return load_list_reloc_elt(DPT_OG);
}

void *load_iog(void);

Ginode *load_ginode(void)
{
     Ginode *gn = MAKE_OBJECT(Ginode);
     
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,gn->intention);
     gn->lgt_string = NULL;
     gn->connect = load_list_reloc_elt(DPT_IOG);
     return gn;
}

Giedge *load_giedge(void)
{
     Giedge *ge = MAKE_OBJECT(Giedge);
     
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,ge->in);
     LOAD_ADDR_AND_REF_LOC(DPT_INTENTION,ge->out);
     return ge;
}

void *load_iog(void)
{
     IOG *iog = MAKE_OBJECT(IOG);
     
     iog->type = load_char();
     iog->x = iog->y = iog->width = iog->height = 0;
     iog->region = XCreateRegion();
     iog->placed = FALSE;
     iog->text_changed = TRUE;
     switch (iog->type) {
     case INODE:
	  iog->u.ginode = load_ginode();
	  break;
     case IEDGE:
	  iog->u.giedge = load_giedge();
	  break;
     }
     return iog;
}
#endif

void *load_op(void)
{
     PBoolean graphix;
     Op_Structure *op = MAKE_OBJECT(Op_Structure);

     LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,op->name);
     op->file_name = load_string();
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,op->invocation);
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,op->call);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_EXPR,op->context);
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,op->setting);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_EXPR,op->effects);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_NODE,op->node_list);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_EDGE,op->edge_list);
     LOAD_ADDR_AND_REF_LOC(DPT_NODE,op->start_point);
     LOAD_ADDR_AND_REF_LOC(DPT_ACTION_FIELD,op->action);
     LOAD_ADDR_AND_REF_LOC(DPT_BODY,op->body);
     LOAD_ADDR_AND_REF_LOC(DPT_LENV,op->list_var);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_PROPERTY,op->properties);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_SYMBOL,op->node_name_list);
     op->text_traced = load_boolean();
     op->step_traced = load_boolean();

     graphix = load_boolean();
#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  load_boolean();
	  load_boolean();
	  FREE(safe_load_string());
	  load_xmstring(); 
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);
	  LOAD_UNUSED_ADDR(DPT_OG);

	  load_list_og();
	  load_list_og();
	  load_list_og();
	  load_list_og();
	  load_list_og();
	  load_list_og();
	  load_list_og();
	  load_list_og();
     }
#else
     if (graphix) {
	  op->graphic = load_boolean();
	  op->graphic_traced = load_boolean();
	  op->documentation = safe_load_string();
	  op->xms_name = load_xmstring(); 
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->ginvocation);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gcall);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gcontext);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gsetting);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->geffects);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gproperties);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gdocumentation);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gaction);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->gbody);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,op->op_title);

	  op->list_og_node=load_list_og();
	  op->list_og_edge=load_list_og();
	  op->list_og_edge_text=load_list_og();
	  op->list_og_text=load_list_og();
	  op->list_movable_og=load_list_og();
	  op->list_destroyable_og=load_list_og();
	  op->list_editable_og=load_list_og();
	  op->list_og_inst=load_list_og();
     } else {
	  report_graphix_mismatch();
	  op->graphic = FALSE;
	  op->graphic_traced = FALSE;
	  op->documentation = NULL;
	  op->xms_name = NULL;
	  op->ginvocation= NULL;
	  op->gcall= NULL;
	  op->gcontext= NULL;
	  op->gsetting= NULL;
	  op->geffects= NULL;
	  op->gproperties= NULL;
	  op->gdocumentation= NULL;
	  op->gaction= NULL;
	  op->gbody= NULL;
	  op->op_title= NULL;

	  op->list_og_node=NULL;
	  op->list_og_edge=NULL;
	  op->list_og_edge_text=NULL;
	  op->list_og_text=NULL;
	  op->list_movable_og=NULL;
	  op->list_destroyable_og=NULL;
	  op->list_editable_og=NULL;
	  op->list_og_inst= NULL;
     }
     op->last_view_x = op->last_view_y = 0;
#endif

     return op;
}

void *load_list_node(void)
{
     return load_list_reloc_elt(DPT_NODE);
}

void *load_list_edge(void)
{
     return load_list_reloc_elt(DPT_EDGE);
}

void *load_node(void)
{
     PBoolean graphix;
     Node *node = MAKE_OBJECT(Node);

     LOAD_ADDR_AND_REF_LOC(DPT_LIST_EDGE, node->out);
     LOAD_ADDR_AND_REF_LOC(DPT_LIST_EDGE, node->in);
     node->type = load_char();
     node->join = load_boolean();
     node->split = load_boolean();

     graphix = load_boolean();

#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  LOAD_UNUSED_ADDR(DPT_SYMBOL);
	  LOAD_UNUSED_ADDR(DPT_OG);
     }
#else
     if (graphix) {
	  LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,node->name);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,node->og);
     } else {
	  report_graphix_mismatch();
	  node->name = NULL;
	  node->og = NULL;
     }
#endif

     return node;
}

void *load_edge(void)
{
     PBoolean graphix;
     Edge *edge = MAKE_OBJECT(Edge);

     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,edge->expr);
     LOAD_ADDR_AND_REF_LOC(DPT_NODE, edge->out);
     edge->type = load_char();

     graphix = load_boolean();

#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  LOAD_UNUSED_ADDR(DPT_NODE);
	  LOAD_UNUSED_ADDR(DPT_OG);
     }
#else
     if (graphix) {
	  LOAD_ADDR_AND_REF_LOC(DPT_NODE, edge->in);
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,edge->og);
     } else {
	  report_graphix_mismatch();
	  edge->in = NULL;
	  edge->og = NULL;
     }
#endif
     return edge;
}

List_Instruction load_list_instruction(void)
{
     return load_list_reloc_elt(DPT_INSTRUCTION);
}

void *load_body(void)
{
     Body *body = MAKE_OBJECT(Body);

     body->insts = load_list_instruction();
     return body;
}

Simple_Instruction *load_simple_instruction(void)
{
     PBoolean graphix;
     Simple_Instruction *res_simple = MAKE_OBJECT(Simple_Instruction);

     LOAD_ADDR_AND_REF_LOC(DPT_EXPR, res_simple->expr);

     graphix = load_boolean();

#ifdef NO_GRAPHIX
     if (graphix) {
	  report_graphix_mismatch();
	  LOAD_UNUSED_ADDR(DPT_OG);
     }
#else
     if (graphix) {
	  LOAD_ADDR_AND_REF_LOC(DPT_OG,res_simple->og);
     } else {
	  res_simple->og = NULL;
     }
#endif
     return res_simple;
}

If_Instruction *load_part_if_instruction(void)
{
     If_Instruction *if_inst = MAKE_OBJECT(If_Instruction);
     PBoolean else_bool;

     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,if_inst->condition);
     if_inst->then_insts = load_list_instruction();
     if_inst->elseif = load_boolean();
     if (if_inst->elseif) {
	  LOAD_ADDR_AND_REF_LOC(DPT_INSTRUCTION,if_inst->u.elseif_inst);
     } else {
	  else_bool = load_boolean();
	  if (else_bool) {
	       if_inst->u.else_insts = load_list_instruction();
	  }
     }
     return if_inst;
}


If_Instruction *load_if_instruction(void)
{
     return load_part_if_instruction();
}


While_Instruction *load_while_instruction(void)
{
     While_Instruction *while_inst = MAKE_OBJECT(While_Instruction);

     LOAD_ADDR_AND_REF_LOC(DPT_EXPR, while_inst->condition);
     while_inst->insts = load_list_instruction();

     return while_inst;
}

Do_Instruction *load_do_instruction(void)
{
     Do_Instruction *do_inst = MAKE_OBJECT(Do_Instruction);

     do_inst->insts = load_list_instruction();
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR, do_inst->condition);

     return do_inst;
}

Par_Instruction *load_par_instruction(void)
{
     Par_Instruction *par_inst = MAKE_OBJECT(Par_Instruction);
     PBoolean bodys;

     bodys = load_boolean();
     if (bodys) 
	  par_inst->bodys = load_list_elt_here(load_body);
     else
	  par_inst->bodys = NULL;

     return par_inst;
}

Instruction *load_instruction(void)
{
     Instruction *inst = MAKE_OBJECT(Instruction);

     inst->type = load_char();
     switch(inst->type) {
     case IT_SIMPLE:
	  inst->u.simple_inst = load_simple_instruction();
	  break;
     case IT_COMMENT:
	  inst->u.comment = load_string();
	  break;
     case IT_LABEL:
	  LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,inst->u.label_inst);
	  break;
     case IT_GOTO:
	  LOAD_ADDR_AND_REF_LOC(DPT_SYMBOL,inst->u.goto_inst);
	  break;
     case IT_BREAK:
	  break;
     case IT_IF:
	  inst->u.if_inst = load_if_instruction();
	  break;
     case IT_WHILE:
	  inst->u.while_inst = load_while_instruction();
	  break;
     case IT_DO:
	  inst->u.do_inst = load_do_instruction();
	  break;
     case IT_PAR:
	  inst->u.par_inst = load_par_instruction();
	  break;
#ifdef DEFAULT
     default: 	 
	  oprs_perror("load_instruction",PE_UNEXPECTED_INSTRUCTION_TYPE);
	  break;
#endif
     }
     return inst;
}


void *load_action_field(void) 
{
     Action_Field *ac = MAKE_OBJECT(Action_Field);

     ac->special = load_boolean();
     if (ac->special) {
	  ac->multi =load_boolean();
	  if (ac->multi)
	       ac->u.list_envar = load_lenv();
	  else
	       LOAD_ADDR_AND_REF_LOC(DPT_ENVAR,ac->u.envar);
     }
     LOAD_ADDR_AND_REF_LOC(DPT_EXPR,ac->function);

     return ac;
}

void *load_object(Dump_Type *return_type)
{
     Dump_Type type;
     void *old_addr;
     void *new_addr = NULL;
     static unsigned int i = 0;

     if (!be_quiet) 
	  if ((i++ << 24) == 0) {
	       printf(".");
#ifndef NO_GRAPHIX
	       process_xt_events();
#endif	  
	  }
     type = load_char(); /* Its type. */

     if (type == DPT_END || type == DPT_FLUSH) {
	  *return_type = type;
	  return NULL;
     }

     old_addr = load_ptr(); /* Its addr. */

     switch(type) {
     case DPT_ENVAR:
	  new_addr = load_envar();
	  break;
     case DPT_EXPR:
	  new_addr = load_expr();
	  break;
     case DPT_FACT:
	  new_addr = load_fact();
	  break;
     case DPT_FLOAT_ARRAY:
	  new_addr = load_float_array();
	  break;
     case DPT_FRAME:
	  new_addr = load_frame();
	  break;
     case DPT_GOAL:
	  new_addr = load_goal();
	  break;
     case DPT_INTENTION:
	  new_addr = load_intention();
	  break;
     case DPT_U_MEMORY:
	  new_addr = load_u_memory();
	  break;
     case DPT_INT_ARRAY:
	  new_addr = load_int_array();
	  break;
     case DPT_LIST_OP:
	  new_addr = load_list_op();
	  break;
     case DPT_OP:
	  new_addr = load_op();
	  break;
     case DPT_OP_INSTANCE:
	  new_addr = load_op_instance();
	  break;
     case DPT_LENV:
	  new_addr = load_lenv();
	  break;
     case DPT_L_LIST:
	  new_addr = load_l_list();
	  break;
     case DPT_PRED_FUNC_REC:
 	  new_addr = load_pred_func_rec();
	  break;
     case DPT_PROPERTY:
	  new_addr = load_property();
	  break;
     case DPT_SYMBOL:
	  new_addr = load_symbol();
	  break;
     case DPT_TERM:
	  new_addr = load_term();
	  break;
     case DPT_TL:
	  new_addr = load_tl();
	  break;
     case DPT_UNDO:
	  new_addr = load_undo();
	  break;
     case DPT_UNDO_LIST:
	  new_addr = load_undo_list();
	  break;
     case DPT_VAR_LIST:
	  new_addr = load_var_list();
	  break;
     case DPT_LIST_EXPR:
	  new_addr = load_list_expr();
	  break;
     case DPT_LIST_PROPERTY:
	  new_addr = load_list_property();
	  break;
     case DPT_LIST_NODE:
	  new_addr = load_list_node();
	  break;
     case DPT_LIST_EDGE:
	  new_addr = load_list_edge();
	  break;
     case DPT_NODE:
	  new_addr = load_node();
	  break;
     case DPT_EDGE:
	  new_addr = load_edge();
	  break;
     case DPT_ACTION_FIELD:
	  new_addr = load_action_field();
	  break;
     case DPT_BODY:
	  new_addr = load_body();
	  break;
     case DPT_LIST_SYMBOL:
	  new_addr = load_list_symbol();
	  break;
     case DPT_OG:
#ifdef NO_GRAPHIX
	  new_addr = NULL;
	  load_og();
#else
	  new_addr = load_og();
#endif
	  break;
     case DPT_INSTRUCTION:
	  new_addr = load_instruction();
	  break;
     case DPT_COND:
	  new_addr = load_condition();
	  break;
     case DPT_TIB:
	  new_addr = load_tib();
	  break;
     case DPT_IOG:
#ifdef NO_GRAPHIX
	  new_addr = NULL;
	  load_iog();
#else
	  new_addr = load_iog();
#endif
	  break;
     case DPT_TYPE:
	  new_addr = load_type();
	  break;
     case DPT_IG:
     case DPT_END:
     case DPT_MAX:
     case DPT_FLUSH:
	  oprs_perror("load_object", PE_UNEXPECTED_DUMP_TYPE);
	  
	  return NULL;
     }
     if (new_addr)
	  relocate_object(old_addr,new_addr,type);
     else {
#ifdef NO_GRAPHIX
	  if (type == DPT_OG || type == DPT_IOG)
	       new_addr = DUMMY_ADDR;
	  else
#endif
	       fprintf(stderr, LG_STR("load_object: NULL allocation for type %s.\n",
				      "load_object: adresse NULL  pour le type %s.\n"),
		       dump_type_name[type]);
     }

     return new_addr;
}

PBoolean load_all_objects(void) /* Return TRUE if there are other dump following. */
{
     Dump_Type type;

     while (load_object(&type));

     if (type == DPT_FLUSH)
	  return TRUE;
     else
	  return FALSE;
}
