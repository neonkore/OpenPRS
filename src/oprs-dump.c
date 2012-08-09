/*                               -*- Mode: C -*- 
 * oprs-dump.c -- 
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

#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <shashPack.h>
#include <slistPack.h>

#ifdef GRAPHIX
#ifdef GTK
#include <gtk/gtk.h>
#include "xm2gtk.h"
#else
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif
#endif

#include "string.h"
#include "macro.h"
#include "constant.h"
#include "oprs-type.h"


#include "oprs.h"
#include "oprs-pred-func.h"
#include "database.h"
#include "relevant-op.h"
#include "oprs-error.h"
#include "oprs-dump.h"
#include "op-structure.h"
#include "op-compiler.h"
#include "op-instance.h"
#include "intention.h"
#include "fact-goal.h"
#include "int-graph.h"
#include "conditions.h"
#include "type.h"
#include "oprs-sprint.h"

#ifdef GRAPHIX
#ifdef GTK
#include "gope-graphic.h"
#include "goprs-intention.h"
#else
#include "ope-graphic.h"
#include "xoprs-intention.h"
#endif
#include "xt-util_f.h"
#endif



#include <shashPack_f.h>
#include <slistPack_f.h>

#include "oprs-error_f.h"
#include "lisp-list_f.h"
#include "database_f.h"
#include "oprs-dump_f.h"
#include "oprs-pred-func_f.h"

int dump_format_version = 5;	/* I use 5 for 64 bits machine... assuming pointers are now 64 bits. */

int dump_file;

PBoolean dev_env = TRUE;

Slist *object_to_dump_list = NULL;
Shash *dump_table = NULL;
static int dump_table_size = DUMP_LOAD_TABLE_SIZE, dump_table_mod = DUMP_LOAD_TABLE_SIZE - 1;

void dump_symbol(Symbol string);

int dump_table_hash_func(void *addr)
{
     return ((unsigned int)(addr)>>3) & dump_table_mod; 
}

PBoolean dump_table_match_func(void *ad1, Object_Dump *od)
{
     return (ad1 == od->addr);
}

int free_od_from_hash(void *ignore, Object_Dump *od)
{
#ifdef DEBUG
     if (REF(od->addr) != od->obj_refcount)
	  fprintf(stderr,"Refcount <> Number of pointers.\n");
#endif
     FREE_OBJECT(od);
     return 1;
}

#define DUMP_BUFSIZ BUFSIZ

static unsigned char dump_buf[DUMP_BUFSIZ];
static int char_dumped = 0;

int dump_write_to_file(void *buf, size_t nbyte)
{
     int i;

     if ((i = write(dump_file,buf,nbyte)) < 0)
	  perror("dump_write: write");
     if ((unsigned)i != nbyte)
	  fprintf(stderr, LG_STR("dump_write: write: did not write all the bytes.\n",
				 "dump_write: write: n'a pas écrit tous les octets.\n"));

     return i;
}

int dump_write(void *buf, size_t nbyte)
{
     if (nbyte + char_dumped > DUMP_BUFSIZ) { /* Will not fit... */
	  dump_write_to_file(dump_buf, char_dumped);
	  char_dumped = 0;
     }
	       
     if (nbyte > DUMP_BUFSIZ)	/* Will not even fit in one buf */
	  dump_write_to_file(buf,nbyte); /* has already flushed. */
     else {
	  BCOPY(buf,dump_buf+char_dumped,nbyte);
	  char_dumped += nbyte;
     }
     return nbyte;
}


#define DUMP_LIST_RELOC_ELT(list,type,dt_type) \
	do { \
            type object; \
            dump_ushort(list_length(list)); \
            dump_short(sl_slist_type(list)); \
	    sl_loop_through_slist(list,object,type) { \
               WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(dt_type,object); \
	    } \
	} while (0)

#define SAFE_DUMP_LIST_RELOC_ELT(list,type,dt_type) \
	do { \
	    if (list) { \
		 dump_boolean(TRUE); \
		 DUMP_LIST_RELOC_ELT(list,type,dt_type); \
	    } else dump_boolean(FALSE); \
	} while (0)

#define DUMP_LIST_ELT_HERE(list,type,dump_function) \
	do { \
            type object; \
            dump_ushort(list_length(list)); \
            dump_short(sl_slist_type(list)); \
	    sl_loop_through_slist(list,object,type) { \
               dump_function(object); \
	    } \
	} while (0)

#define SAFE_DUMP_LIST_ELT_HERE(list,type,dump_function) \
	do { \
	    if (list) { \
		 dump_boolean(TRUE); \
		 DUMP_LIST_ELT_HERE(list,type,dump_function); \
	    } else dump_boolean(FALSE); \
	} while (0)

int dump_char(char c)
{
     return dump_write(&c, sizeof(char));
}

#define dump_boolean(bool) dump_char(bool)

int dump_uchar(unsigned char c)
{
     return dump_write(&c, sizeof(unsigned char));
}

int dump_short(short s)
{
     short sn = htons(s);

     return dump_write(&sn, sizeof(short));
}

#define dump_position(s) dump_short(s)

int dump_ushort(unsigned short s)
{
       short usn = htons(s);
       
       return dump_write(&usn, sizeof(unsigned short));
}

#define dump_dimension(s) dump_ushort(s)

int dump_int(int i)
{
     int in = htonl(i);

     return dump_write(&in, sizeof(int));
}

int dump_long(long i)
{
     long in = htonl(i);

     return dump_write(&in, sizeof(long));
}

int dump_long_long(long long i)
{
     u_char buf[8];

     htonll(&i, buf);

     return dump_write(buf, 8);
}

#ifdef _LP64
int dump_ptr(void *ptr)
{
     u_char buf[8];

     htonptr(ptr,buf);

     return dump_write(buf, 8);
}
#else
int dump_ptr(void *ptr)
{
     void *buf;

     htonptr(ptr,&buf);

     return dump_write(buf, 8);
}

#endif

int dump_float(double f)
{
     u_char buf[8];

     htond(&f,buf);

     return dump_write(buf, 8);
}

void flush_dump_table_and_list()
{
     if (dump_table) {
	  for_all_hashtable(dump_table,NULL,free_od_from_hash);
     	  sh_flush_hashtable(dump_table);
     }
     if (object_to_dump_list) {
	  sl_flush_slist(object_to_dump_list);
     }
}

void clear_dump_table_and_list()
{
     flush_dump_table_and_list();

     if (dump_table) {
     	  sh_free_hashtable(dump_table);
	  dump_table = NULL;
     }
     if (object_to_dump_list) {
	  sl_free_slist(object_to_dump_list);
	  object_to_dump_list=NULL;
     }
}

static int comp_old_value = FALSE;

int start_dump_session(char *filename)
{
     clear_dump_table_and_list();
     comp_old_value = disable_slist_compaction();

     dump_table = sh_make_hashtable(dump_table_size, dump_table_hash_func, dump_table_match_func);
     object_to_dump_list = sl_make_slist();
     if ((dump_file = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644)) < 0) {
	  perror("start_dump_session: open");
     } else {
	  char_dumped = 0;
	  dump_ushort(dump_format_version);
     }

     return dump_file;
}

void flush_dump_session(void)
{
     flush_dump_table_and_list();
     dump_char(DPT_FLUSH);		/* This is the flush marker... */
     if (!be_quiet) printf("+");
}

void end_dump_session(void)
{
     clear_dump_table_and_list();
     dump_char(DPT_END);		/* This is the final marker... */
     if (!be_quiet) printf("+*");
     dump_write_to_file(dump_buf, char_dumped);
     char_dumped = 0;
     if (dump_file >= 0) close(dump_file);
     if (comp_old_value) enable_slist_compaction();
}

Object_Dump *object_dumped(void *addr)
{
     return (Object_Dump *)sh_get_from_hashtable(dump_table, addr);
}

void add_object_to_dump(Dump_Type dt, void *addr)
{
     if (addr) {
	  Object_Dump *od;
	  if ((od = object_dumped(addr))) {
	       od->obj_refcount++;
	  } else {
	       Object_Dump *od = MAKE_OBJECT(Object_Dump);

	       od->type = dt;
	       od->addr = addr;
	       od->obj_refcount = 1;
	       sh_add_to_hashtable(dump_table,od,addr);
	       sl_add_to_head(object_to_dump_list,od); /* May be better to do a depth first
							  (limit the number of unreference) */
	  }
     }
}

void dump_property(Property *p)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,p->name);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TERM,p->value);
}

void dump_list_property(PropertyList pl)
{
     DUMP_LIST_RELOC_ELT(pl, Property *, DPT_PROPERTY);
}

void dump_var_list(VarList vl)
{
     DUMP_LIST_RELOC_ELT(vl, Envar *, DPT_ENVAR);
}

void dump_undo_list(List_Undo lu)
{
     DUMP_LIST_RELOC_ELT(lu, Undo *, DPT_UNDO);
}

void dump_frame(FramePtr fp)
{
     dump_boolean(fp->installe);

     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_UNDO_LIST,fp->list_undo);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FRAME,fp->previous_frame);
}

void dump_lenv(List_Envar le)
{
     DUMP_LIST_RELOC_ELT(le, Envar *, DPT_ENVAR);
}

void dump_envar(Envar *ev)
{
     dump_char(ev->type);
     dump_symbol(ev->name);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TYPE,ev->unif_type);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TERM,ev->value);
}

void dump_list_symbol(Slist *list_symbol)
{
     DUMP_LIST_RELOC_ELT(list_symbol, Symbol, DPT_SYMBOL);
}

void dump_list_type(TypeList types)
{
     DUMP_LIST_RELOC_ELT(types, Type *, DPT_TYPE);
}

void dump_type(Type *t)
{
     dump_symbol(t->name);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TYPE,t->father);
     dump_list_type(t->sur_types);
     dump_list_symbol(t->elts);
}

void dump_undo(Undo *ud)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_ENVAR,ud->envar);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TERM,ud->value);
}

void dump_int_array(Int_array *ia)
{
     unsigned short i; 

     dump_ushort(ia->size);
     for(i = 0; i < ia->size ; i++) {
	  dump_int(ia->array[i]);
     }
}

void dump_float_array(Float_array *fa)
{
     unsigned short i; 

     dump_ushort(fa->size);
     for(i = 0; i < fa->size ; i++) {
	  dump_float(fa->array[i]);
     }
}

void dump_expr(Expression *expr)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_PRED_FUNC_REC, expr->pfr);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TL,expr->terms);
}

void dump_string(PString string)
{
     dump_ushort(strlen(string));
     dump_write(string,strlen(string));
}

void safe_dump_string(PString string)
{
     if (string) {
	  dump_boolean(TRUE); 
	  dump_string(string);
     } else dump_boolean(FALSE); 
}

void dump_symbol(Symbol string)
{
     dump_ushort(strlen(string));
     dump_write(string,strlen(string));
}

void safe_dump_symbol(Symbol string)
{
    if (string) {
	  dump_boolean(TRUE); 
	  dump_symbol(string);
    } else dump_boolean(FALSE); 
}

void dump_pred_func_rec(Pred_Func_Rec *pfr)
{
     safe_dump_symbol(pfr->name);

     if (pfr->name) {
	  dump_boolean((pfr->u.u.ef!=NULL));
	  dump_boolean((pfr->u.u.act!=NULL));
	  dump_boolean((pfr->u.u.pr!=NULL));
	  if  (pfr->u.u.pr) {

	       dump_boolean((pfr->u.u.pr->ep!=NULL));
	       /* cached_rop will be built by the compiler. */
	       dump_char(pfr->u.u.pr->ff);
	       dump_char(pfr->u.u.pr->arity);
	       dump_boolean(pfr->u.u.pr->cwp);
	       dump_boolean(pfr->u.u.pr->be);
	       dump_boolean(pfr->u.u.pr->op_pred);
	  }
     } else 
	  dump_envar(pfr->u.envar);
}

void dump_tl(TermList tl)
{
     DUMP_LIST_RELOC_ELT(tl, Term *, DPT_TERM);
}

void dump_l_list( L_List l)
{
     L_List tmp;

     dump_ushort(L_LENGTH(l));
     for(tmp = l; tmp != l_nil ; tmp = CDR(tmp)) {
	  Term *car = CAR(tmp);

          WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TERM,car);
     }
}
 
void dump_term(Term *term)
{
     dump_char(term->type);
     switch (term->type) {
     case INTEGER: 	 
	  dump_int(term->u.intval);
	  break;
     case LONG_LONG: 	 
	  dump_long_long(term->u.llintval);
	  break;
     case TT_FACT:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FACT, term->u.fact);
	  break;
     case TT_GOAL:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_GOAL,term->u.goal);
	  break;
     case TT_OP_INSTANCE:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OP_INSTANCE,term->u.opi);
	  break;
     case TT_INTENTION:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION, term->u.in);
	  break;
     case U_POINTER: 	 
	  dump_ptr(term->u.u_pointer);
	  break;
     case U_MEMORY: 	 
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_U_MEMORY, term->u.u_memory);
	  break;
     case TT_FLOAT: 	 
	  dump_float(*term->u.doubleptr);
	  break;
     case STRING:
	  dump_string(term->u.string);
	  break;
     case TT_ATOM:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,term->u.id);
	  break;
     case VARIABLE:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_ENVAR,term->u.var);
	  break;
     case EXPRESSION:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,term->u.expr);
	  break;
     case INT_ARRAY: 	 
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INT_ARRAY,term->u.int_array);
	  break;
     case FLOAT_ARRAY: 	 
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FLOAT_ARRAY,term->u.float_array);
	  break;
     case LISP_LIST: 	 
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_L_LIST,term->u.l_list);
	  break;
     case C_LIST:
	  oprs_perror("dump_term", PE_UNEXPECTED_TERM_TYPE);
	  break;
     }
}

void dump_list_expr(ExprList exprs_to_dump)
{
     DUMP_LIST_RELOC_ELT(exprs_to_dump,  Expression *, DPT_EXPR);
}

void dump_list_op(Op_List opl)
{
     DUMP_LIST_RELOC_ELT(opl,  Op_Structure *, DPT_OP);
}

void dump_op_instance(Op_Instance *op_inst)
{
     if (op_inst->op == goal_satisfied_in_db_op_ptr)
	  dump_char(1);
     else if (op_inst->op == goal_for_intention_op_ptr)
	  dump_char(2);
     else if (op_inst->op == goal_waiting_op_ptr)
	  dump_char(3);
     else {
	  dump_char(0);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OP,op_inst->op);
     }

     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_GOAL,op_inst->goal);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FACT,op_inst->fact);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FRAME,op_inst->frame);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LENV,op_inst->lenv);
     dump_boolean(op_inst-> intended);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,op_inst->maintain);
     
     return;
}


void dump_u_memory(U_Memory *um)
{
     dump_int(um->size);
     dump_write(um->mem,um->size);
     return;
}

void dump_goal(Goal *goal)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,goal->statement);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FRAME,goal->frame);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,goal->interne);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB,goal->tib);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OP_INSTANCE,goal->succes);
     SAFE_DUMP_LIST_RELOC_ELT(goal->echec, Op_Instance *, DPT_OP_INSTANCE);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EDGE,goal->edge);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE,goal->cp);
     dump_uchar(goal->echoue);
     dump_boolean(goal->pris_en_compte);
     dump_boolean(goal->etat);
#ifdef OPRS_PROFILING
     PDate creation;	
     PDate reception;
     PDate soak;
     PDate reponse;
#endif
     return;
}

void dump_fact(Fact *fact)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,fact->statement);
     dump_boolean(fact->pris_en_compte);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,fact->message_sender);
#ifdef OPRS_PROFILING		/* Not implemented for dump for now */
     int executed_ops;
     PDate creation;		/* Date de creation, reception, soak et reponse */
     PDate reception;
     PDate soak;
     PDate reponse;
#endif

     return;
}

void dump_oprs_date(PDate *pd)
{
     dump_long(pd->tv_sec);
     dump_long(pd->tv_usec);
     return;
}

void dump_sprinter(Sprinter *sp)
{
     dump_string(sp->str);
     return;
}

void dump_condition(Relevant_Condition *rc)
{
     dump_uchar(rc->type);
     dump_boolean(rc->evolving);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,rc->expr);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FRAME,rc->frame);

     switch (rc->type) {
     case INTENTION_ACTIVATION:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,rc->u.intention);
	  break;
     case ACTION_WAIT_ACTIVATION:
     case TIB_ACTIVATION:
     case MAINTAIN_ACTIVATION:
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB,rc->u.tib);
     }
     return;
}

void dump_intention(Intention *intention)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_FACT,intention->fact);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_GOAL,intention->goal);
     DUMP_LIST_RELOC_ELT(intention->fils, Thread_Intention_Block *, DPT_TIB);
     DUMP_LIST_RELOC_ELT(intention->active_tibs, Thread_Intention_Block *, DPT_TIB);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OP_INSTANCE,intention->top_op);
     dump_short(intention->priority);
     dump_uchar(intention->status);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,intention->id);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB,intention->critical_section);
     dump_oprs_date(&(intention->creation));	
     DUMP_LIST_RELOC_ELT(intention->activation_conditions_list, Relevant_Condition *, DPT_COND);
#ifdef OPRS_PROFILING
     PDate active;	
#endif
     dump_sprinter(intention->failed_goal_sprinter);

#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_IOG,intention->iog);
     } else
#endif
	  dump_boolean(0);		/* no graphic information */

     return;
}

void dump_tib(Thread_Intention_Block *tib)
{
     dump_uchar(tib->status);
     if (tib->status != TIBS_DEAD) { /* It is virtually gone... */
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OP_INSTANCE,tib->curr_op_inst);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,tib->intention);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB,tib->pere);
	  DUMP_LIST_RELOC_ELT(tib->fils, Thread_Intention_Block *, DPT_TIB);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE,tib->current_node);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_GOAL,tib->current_goal);
	  SAFE_DUMP_LIST_RELOC_ELT(tib->list_to_try, Edge *, DPT_EDGE);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,tib->maintain);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_COND,tib->activation_condition);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_COND,tib->maintain_condition);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_TIB,tib->suspended);
	  dump_ushort(tib->num_called);
     }
     return;
}

void dump_intention_paire(Intention_Paire *ip)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,ip->first);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,ip->second);
#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_IOG,ip->iog);
     } else
#endif
     dump_boolean(0);		/* no graphic information */
     return;
}

void dump_ig(Intention_Graph *ig)
{
     DUMP_LIST_RELOC_ELT(ig->list_intentions, Intention *, DPT_INTENTION);
     DUMP_LIST_RELOC_ELT(ig->list_first, Intention *, DPT_INTENTION);
     DUMP_LIST_RELOC_ELT(ig->list_runnable, Intention *, DPT_INTENTION);
     DUMP_LIST_RELOC_ELT(ig->list_condamned, Intention *, DPT_INTENTION);
     DUMP_LIST_ELT_HERE(ig->list_pairs, Intention_Paire *, dump_intention_paire);
     DUMP_LIST_RELOC_ELT(ig->current_intentions, Intention *, DPT_INTENTION);
     /* Oprs * superior;		 Dieu le pere */
     DUMP_LIST_RELOC_ELT(ig->sleeping, Intention *, DPT_INTENTION);
     DUMP_LIST_RELOC_ELT(ig->sleeping_on_cond, Intention *, DPT_INTENTION);
     /* PFB sort_predicat; */
#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  DUMP_LIST_RELOC_ELT(ig->list_inode, IOG *, DPT_IOG);
	  DUMP_LIST_RELOC_ELT(ig->list_iedge, IOG *, DPT_IOG);
     } else
#endif
	  dump_boolean(0);		/* no graphic information */

     return;
}

#ifndef GRAPHIX
void dump_og(OG *og)
{
     fprintf(stderr, "dump_og: this kernel does not know how to handle OG dump.");
     return;
}

void dump_iog(IOG *iog)
{
     fprintf(stderr, "dump_iog: this kernel does not know how to handle IOG dump.");
     return;
}
#else
void dump_xmstring(XmString xm)
{
#ifdef GTK
#else
     if (xm) {
	  XmStringContext xmsc;
	  char *text;
	  XmStringCharSet cs;
	  XmStringDirection dir;
	  Boolean sep;
	  
	  if (!XmStringInitContext(&xmsc,xm)) {
	       dump_boolean(FALSE);
	       fprintf(stderr, "dump_xmstring: Failed XmStringInitContext.");
	       return;
	  }
	  
	  while (XmStringGetNextSegment(xmsc,&text, &cs, &dir, &sep)) {
	       dump_boolean(TRUE);
	  
	       dump_string(text);
	       dump_string(cs);
	       dump_uchar(dir);
	       dump_boolean(sep);
	       XtFree(text);
	       XtFree(cs);
	  }
	  XmStringFreeContext(xmsc);
	  dump_boolean(FALSE);
     } else
	  dump_boolean(FALSE);
#endif
     
     return;
}

void dump_gnode(Gnode *gnode)
{
     dump_dimension(gnode->swidth);
     dump_dimension(gnode->sheight);
     dump_xmstring(gnode->xmstring);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE,gnode->node);
     return;
}

void dump_gedge(Gedge *gedge)
{
     dump_position(gedge->x1);
     dump_position(gedge->y1);
     dump_position(gedge->x2);
     dump_position(gedge->y2);
     dump_position(gedge->fx1);
     dump_position(gedge->fx2);
     dump_position(gedge->fy1);
     dump_position(gedge->fy2);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EDGE, gedge->edge);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG, gedge->text);
     DUMP_LIST_RELOC_ELT(gedge->list_knot, OG *, DPT_OG);

     return;
}

void dump_gtext_string(Gtext_String *gtext_string)
{
     dump_dimension(gtext_string->off_x);
     dump_dimension(gtext_string->off_y);
     dump_xmstring(gtext_string->xmstring);

     return;
}

void dump_gedge_text(Gedge_text *gedge_text)
{
     dump_position(gedge_text->dx);
     dump_position(gedge_text->dy);
     dump_string(gedge_text->log_string);
     dump_dimension(gedge_text->text_width);
     dump_boolean(gedge_text->fill_lines);
     DUMP_LIST_ELT_HERE(gedge_text->lgt_log_string, Gtext_String *, dump_gtext_string);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG, gedge_text->edge);

     return;
}



void dump_gtext(Gtext *gtext)
{
     dump_char(gtext->text_type);
     dump_string(gtext->string);
     dump_dimension(gtext->text_width);
     dump_boolean(gtext->fill_lines);
     dump_boolean(gtext->visible);
     DUMP_LIST_ELT_HERE(gtext->lgt_string,Gtext_String *,dump_gtext_string);
     SAFE_DUMP_LIST_RELOC_ELT(gtext->list_og_inst, OG *, DPT_OG);

     return;
}
     

void dump_ginst(Ginst *ginst)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INSTRUCTION, ginst->inst);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EDGE, ginst->edge);
     /* int num_line, nb_lines, indent, width; */

     return;
}

void dump_gknot(Gknot *gknot)
{
     dump_position(gknot->x);
     dump_position(gknot->y);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG, gknot->edge);
     return;
}

void dump_og(OG *og)
{
     dump_char(og->type);
     dump_position(og->x);
     dump_position(og->y);
     dump_dimension(og->width);
     dump_dimension(og->height);
     /* Region region; */
     dump_boolean(og->selected);
     switch (og->type) {
     case DT_IF_NODE:
     case DT_THEN_NODE:
     case DT_ELSE_NODE:
     case DT_NODE:
	  dump_gnode(og->u.gnode);
	  break;
     case DT_EDGE:
	  dump_gedge(og->u.gedge);
	  break;
     case DT_EDGE_TEXT:
	  dump_gedge_text(og->u.gedge_text);
	  break;
     case DT_TEXT:
	  dump_gtext(og->u.gtext);
	  break;
     case DT_INST:
	  dump_ginst(og->u.ginst);
	  break;
     case DT_KNOT:
	  dump_gknot(og->u.gknot);
     }
}

void dump_list_og(List_OG log)
{
     DUMP_LIST_RELOC_ELT(log, OG *, DPT_OG);
}

void dump_iog(IOG *iog);

void dump_ginode(Ginode *gn)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,gn->intention);
     DUMP_LIST_RELOC_ELT(gn->connect,IOG *,DPT_IOG);
     return;
}

void dump_giedge(Giedge *ge)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,ge->in);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INTENTION,ge->out);
     return;
}

void dump_iog(IOG *iog)
{
     dump_char(iog->type);
     switch (iog->type) {
     case INODE:
	  dump_ginode(iog->u.ginode);
	  break;
     case IEDGE:
	  dump_giedge(iog->u.giedge);
	  break;
     }
     return;
}

#endif

void dump_op(Op_Structure *op)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,op->name);
     dump_string(op->file_name);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,op->invocation);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,op->call); 
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_EXPR,op->context); 
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,op->setting);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_EXPR,op->effects);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_NODE,op->node_list);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_EDGE,op->edge_list);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE,op->start_point);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_ACTION_FIELD,op->action);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_BODY,op->body);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LENV,op->list_var);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_PROPERTY,op->properties);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_SYMBOL,op->node_name_list);
     dump_boolean(op->text_traced);
     dump_boolean(op->step_traced);


#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  dump_boolean(op->graphic);
	  dump_boolean(op->graphic_traced);
	  safe_dump_string(op->documentation);
	  dump_xmstring(op->xms_name);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->ginvocation);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gcall);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gcontext);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gsetting);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->geffects);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gproperties);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gdocumentation);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gaction);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->gbody);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,op->op_title);

	  dump_list_og(op->list_og_node);
	  dump_list_og(op->list_og_edge);
	  dump_list_og(op->list_og_edge_text);
	  dump_list_og(op->list_og_text);
	  dump_list_og(op->list_movable_og);
	  dump_list_og(op->list_destroyable_og);
	  dump_list_og(op->list_editable_og);
	  dump_list_og(op->list_og_inst);
     } else
#endif
	  dump_boolean(0);		/* no graphic information */

     return;
}

void dump_list_node(NodeList list_node)
{
     DUMP_LIST_RELOC_ELT(list_node, Node *, DPT_NODE);
}

void dump_list_edge(EdgeList list_edge)
{
     DUMP_LIST_RELOC_ELT(list_edge, Edge *, DPT_EDGE);
}

void dump_node(Node *node)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_EDGE, node->out);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_LIST_EDGE, node->in);
     dump_char(node->type);
     dump_boolean(node->join);
     dump_boolean(node->split);

#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_SYMBOL,node->name);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,node->og);
     } else
#endif
	  dump_boolean(0);		/* no graphic information */

    return;
}

void dump_edge(Edge *edge)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,edge->expr);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE, edge->out);
     dump_char(edge->type);

#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);		/* Will save graphic information */
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_NODE, edge->in);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,edge->og);
     } else
#endif
	  dump_boolean(0);		/* no graphic information */


     return;
}

void dump_list_instruction(List_Instruction insts)
{
     DUMP_LIST_RELOC_ELT(insts, Instruction *, DPT_INSTRUCTION);
}

void dump_body(Body *body)
{
     dump_list_instruction(body->insts);
}

void dump_simple_instruction(Simple_Instruction *simple)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,simple->expr);
#ifdef GRAPHIX
     if (compile_graphix) {
	  dump_boolean(1);
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_OG,simple->og);
     } else
#endif
	  dump_boolean(0);

}

void dump_part_if_instruction(If_Instruction *if_inst)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,if_inst->condition);
     dump_list_instruction(if_inst->then_insts);
     dump_boolean(if_inst->elseif);
     if (if_inst->elseif) {
	  WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_INSTRUCTION, if_inst->u.elseif_inst);
     } else {
	  dump_boolean(if_inst->u.else_insts != NULL);
	  if (if_inst->u.else_insts) {
	       dump_list_instruction(if_inst->u.else_insts);
	  }
     }
}

void dump_if_instruction(If_Instruction *if_inst)
{
     dump_part_if_instruction(if_inst);
}


void dump_while_instruction(While_Instruction *while_inst)
{
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR, while_inst->condition);
     dump_list_instruction(while_inst->insts);
}

void dump_do_instruction(Do_Instruction *do_inst)
{
     dump_list_instruction(do_inst->insts);
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR, do_inst->condition);
}

void dump_par_instruction(Par_Instruction *par_inst)
{
     dump_boolean(par_inst->bodys != NULL);
     if (par_inst->bodys) {
	  DUMP_LIST_ELT_HERE(par_inst->bodys, Body *, dump_body);
     }
}

void dump_instruction(Instruction *inst)
{
     dump_char(inst->type);
     switch(inst->type) {
     case IT_SIMPLE:
	  dump_simple_instruction(inst->u.simple_inst);
	  break;
     case IT_COMMENT:
	  dump_string(inst->u.comment);
	  break;
     case IT_LABEL:
	  dump_symbol(inst->u.label_inst);
	  break;
     case IT_GOTO:
	  dump_symbol(inst->u.goto_inst);
	  break;
     case IT_BREAK:
	  break;
     case IT_IF:
	  dump_if_instruction(inst->u.if_inst);
	  break;
     case IT_WHILE:
	  dump_while_instruction(inst->u.while_inst);
	  break;
     case IT_DO:
	  dump_do_instruction(inst->u.do_inst);
	  break;
     case IT_PAR:
	  dump_par_instruction(inst->u.par_inst);
	  break;
#ifdef DEFAULT
     default: 	 
	  oprs-perror("dump_instruction", PE_UNEXPECTED_INSTRUCTION_TYPE);
	  break;
#endif
     }
}

void dump_action_field(Action_Field *ac) 
{ 
     dump_boolean(ac->special);
     if (ac->special) {
	  dump_boolean(ac->multi);
	  if (ac->multi)
	       dump_lenv(ac->u.list_envar);
	  else
	       WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_ENVAR,ac->u.envar);
     }
     WRITE_ADDR_AND_ADD_OBJECT_TO_DUMP(DPT_EXPR,ac->function);
}

void dump_object(Object_Dump *od)
{
     static unsigned int i = 0;

     if (!be_quiet) 
	  if ((i++ << 24) == 0) {
	       printf(".");
#ifdef GRAPHIX
	       process_xt_events();
#endif	  
	  }
     
     dump_char(od->type); /* Its type. */
     dump_ptr(od->addr); /* Its addr. */
     switch(od->type) {
     case DPT_ENVAR:
	  dump_envar(od->addr);
	  break;
     case DPT_EXPR:
	  dump_expr(od->addr);
	  break;
     case DPT_FACT:
	  dump_fact(od->addr);
	  break;
     case DPT_FLOAT_ARRAY:
	  dump_float_array(od->addr);
	  break;
     case DPT_FRAME:
	  dump_frame(od->addr);
	  break;
     case DPT_GOAL:
	  dump_goal(od->addr);
	  break;
     case DPT_U_MEMORY:
	  dump_u_memory(od->addr);
	  break;
     case DPT_INTENTION:
	  dump_intention(od->addr);
	  break;
     case DPT_COND:
	  dump_condition(od->addr);
	  break;
     case DPT_TIB:
	  dump_tib(od->addr);
	  break;
     case DPT_INT_ARRAY:
	  dump_int_array(od->addr);
	  break;
     case DPT_LIST_OP:
	  dump_list_op(od->addr);
	  break;
     case DPT_OP:
	  dump_op(od->addr);
	  break;
     case DPT_OP_INSTANCE:
	  dump_op_instance(od->addr);
	  break;
     case DPT_LENV:
	  dump_lenv(od->addr);
	  break;
     case DPT_L_LIST:
	  dump_l_list(od->addr);
	  break;
     case DPT_PRED_FUNC_REC:
 	  dump_pred_func_rec(od->addr);
	  break;
     case DPT_PROPERTY:
	  dump_property(od->addr);
	  break;
     case DPT_SYMBOL:
	  dump_symbol(od->addr);
	  break;
     case DPT_TERM:
	  dump_term(od->addr);
	  break;
     case DPT_TL:
	  dump_tl(od->addr);
	  break;
     case DPT_UNDO:
	  dump_undo(od->addr);
	  break;
     case DPT_UNDO_LIST:
	  dump_undo_list(od->addr);
	  break;
     case DPT_VAR_LIST:
	  dump_var_list(od->addr);
	  break;
     case DPT_LIST_EXPR:
	  dump_list_expr(od->addr);
	  break;
     case DPT_LIST_PROPERTY:
	  dump_list_property(od->addr);
	  break;
     case DPT_LIST_NODE:
	  dump_list_node(od->addr);
	  break;
     case DPT_LIST_EDGE:
	  dump_list_edge(od->addr);
	  break;
     case DPT_NODE:
	  dump_node(od->addr);
	  break;
     case DPT_EDGE:
	  dump_edge(od->addr);
	  break;
     case DPT_ACTION_FIELD:
	  dump_action_field(od->addr);
	  break;
     case DPT_BODY:
	  dump_body(od->addr);
	  break;
     case DPT_LIST_SYMBOL:
	  dump_list_symbol(od->addr);
	  break;
     case DPT_INSTRUCTION:
	  dump_instruction(od->addr);
	  break;
     case DPT_OG:
	  dump_og(od->addr);
	  break;
     case DPT_IOG:
	  dump_iog(od->addr);
	  break;
     case DPT_TYPE:
	  dump_type(od->addr);
	  break;
     case DPT_IG:
     case DPT_END:
     case DPT_MAX:
	  oprs_perror("dump_object", PE_UNEXPECTED_DUMP_TYPE);
	  break;
     }
}


int dump_object_list(void)
{ 
     int i = 0;

     while (! (sl_slist_empty(object_to_dump_list))) {
	  Object_Dump *od;
	  Slist *copy=object_to_dump_list;

	  object_to_dump_list = sl_make_slist();
	  
	  sl_loop_through_slist(copy, od, Object_Dump *) {
	       dump_object(od);
	       i++;
	  }
	  FREE_SLIST(copy);
     }
     return i;
}
