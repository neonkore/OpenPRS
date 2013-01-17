/*                               -*- Mode: C -*-
 * oprs-type.c -- Fonction de construction et de print pour les types...
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

#ifdef VXWORKS
#endif

#include <stdlib.h>

#include "shashPack_f.h"

#include "constant.h"
#include "oprs-type.h"
#include "ev-function.h"

#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "lisp-list_f.h"
#include "oprs-pred-func_f.h"

const FramePtr empty_env = (FramePtr) NULL;

List_Envar current_var_list;		

Slist *empty_list;			  /* This is a constant... do not use it to start a list or I kill you */

Symbol spec_act_sym;
Symbol wait_son_sym;
Symbol wait_sym;
Symbol nil_sym;
Symbol current_sym;
Symbol object_sym;
Symbol quote_sym;
Symbol decision_procedure_sym;
Symbol not_sym;
Symbol lisp_t_sym;
Symbol soak_sym;
Symbol app_ops_fact_sym;
Symbol fact_invoked_ops_sym;
Symbol db_satisf_goal_sym;
Symbol app_ops_goal_sym;
Symbol goal_invoked_ops_sym;
Symbol failed_goal_sym;
Symbol failed_sym;
Symbol request_sym;
Symbol achieved_sym;
Symbol wake_up_intention_sym;
Symbol read_response_id_sym;
Symbol read_response_sym;

Symbol or_sym;
Symbol and_sym;
Symbol not_sym;
Symbol cond_add_sym;
Symbol cond_del_sym;
Symbol temp_conclude_sym;
Symbol retract_sym;
Symbol achieve_sym;
Symbol test_sym;
Symbol preserve_sym;
Symbol twait_sym;
Symbol maintain_sym;
Symbol call_sym;


char *oprs_prompt = NULL;

#define ID_HASHTABLE_MINSIZE 64
#define ID_HASHTABLE_DEFAULTSIZE 1024 /* has to be a power of 2 for the bitwise modulo */
static int id_hashtable_size, id_hashtable_mod;

Shash *id_hash;

#ifdef VXWORKS
SEM_ID id_hash_sem = NULL;
#endif     

List_Envar global_var_list;

PBoolean run_option[MAX_RUN_OPTION];

PBoolean debug_trace[MAX_DEBUG];

PBoolean compiler_option[MAX_COMPILER_OPTION];

PBoolean user_trace[MAX_USER_TRACE];

PBoolean meta_option[MAX_META_OPTION];

#ifdef OPRS_PROFILING
PBoolean profiling_option[MAX_PROFILING_OPTION];
#endif

PBoolean check_symbol = FALSE;

PBoolean allow_variable_parsing = FALSE;

PBoolean be_quiet = FALSE;

PBoolean lower_case_id = FALSE;
PBoolean no_case_id = TRUE;

int compare_expr(Expression *t1, Expression *t2);

#ifndef VXWORKS
PBoolean check_and_set_id_case_option(PString id_case_option)
{
     PBoolean res = FALSE;

     lower_case_id = FALSE;
     no_case_id = FALSE;

     if (strcasecmp(id_case_option, "LOWER") == 0) {
	  lower_case_id = TRUE;
	  res = TRUE;
     } else if (strcasecmp(id_case_option, "UPPER") == 0) {
	  res = TRUE;
     } else if (strcasecmp(id_case_option, "NONE") == 0) {
	  no_case_id = TRUE;
	  res = TRUE;
     } 
     return res;
}
#endif

void enable_variable_parsing(void)
{
     current_var_list = sl_make_slist();
     allow_variable_parsing = TRUE;
}

void disable_variable_parsing(void)
{
     free_lenv(current_var_list);
     current_var_list = NULL;
     allow_variable_parsing = FALSE;
}

Undo *make_undo(Envar *ev)
{
     Undo *u = MAKE_OBJECT(Undo);

     u->envar = ev;
     u->value = (Term *) NULL;
     return u;
}

FramePtr make_frame(FramePtr pf)
{
     FramePtr fp = MAKE_OBJECT(Frame);

     fp->list_undo = NULL;
     fp->previous_frame = dup_frame(pf);
     fp->installe = TRUE;
     return fp;
}

FramePtr make_frame_not_inst(FramePtr pf)
{
     FramePtr fp = make_frame(pf);

     fp->installe = FALSE;
     return fp;
}

ExprFrame *make_ef(Expression *expr, Frame *f)
{
     ExprFrame *ef = MAKE_OBJECT(ExprFrame);

     ef->frame = f;
     ef->expr = expr;
     return ef;
}

ExprListFrame *make_elf(ExprList exprl, Frame *f)
{
     ExprListFrame *elf = MAKE_OBJECT(ExprListFrame);

     elf->frame = f;
     elf->exprl = exprl;
     return elf;
}

Term *build_pointer(void *p)
{

     Term *t = MAKE_OBJECT(Term);

     t->type = U_POINTER;
     t->u.u_pointer = p;

     return t;
}

Term *build_memory(void *p, int size)
{

     Term *t = MAKE_OBJECT(Term);
     U_Memory *um = MAKE_OBJECT(U_Memory);

     um->mem = MALLOC(size);
     BCOPY(p,um->mem,size);
     um->size = size;
     t->type = U_MEMORY;
     t->u.u_memory = um;

     return t;
}

void *u_memory_mem(Term *t)
{
     return t->u.u_memory->mem;
}

int u_memory_size(Term *t)
{
     return t->u.u_memory->size;
}

Term *build_long_long(long long int i)
{

     Term *t = MAKE_OBJECT(Term);

     t->type = LONG_LONG;
     t->u.llintval = i;

     return t;
}

Term *build_integer(int i)
{

     Term *t = MAKE_OBJECT(Term);

     t->type = INTEGER;
     t->u.intval = i;

     return t;
}

double *make_doubleptr(double i)
{
     double *t = MALLOC(sizeof(double));

     *t = i;

     return t;
}

void free_doubleptr(double *i)
{
     FREE(i);
}

Term *build_float(double i)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_FLOAT;
     t->u.doubleptr = make_doubleptr(i);

     return t;
}

Term *build_goal_term(Goal *l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_GOAL;
     t->u.goal = l;

     return t;
}

Term *build_fact_term(Fact *l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_FACT;
     t->u.fact = l;

     return t;
}

Term *build_intention_term(Intention *l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_INTENTION;
     t->u.in = l;

     return t;
}

Term *build_opi_term(Op_Instance *l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_OP_INSTANCE;
     t->u.opi = l;

     return t;
}

Term *build_l_list(L_List l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = LISP_LIST;
     t->u.l_list = l;

     return t;
}

Term *build_c_list(Slist *l)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = C_LIST;
     t->u.c_list = l;

     return t;
}

Term *build_qstring(char *i)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = STRING;
     t->u.string = i;

     return t;
}

Term *build_string(const char *str)
/* build_string create a Term containing a COPY of the string str. */
{
     Term *t = MAKE_OBJECT(Term);

     t->type = STRING;
     NEWSTR(str,t->u.string);

     return t;
}

Term *build_id(Symbol i)
{

     Term *t = MAKE_OBJECT(Term);

     t->type = TT_ATOM;
     t->u.id = i;

     return t;
}

Term *build_nil(void)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_ATOM;
     t->u.id = nil_sym;

     return t;
}

Term *build_t(void)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = TT_ATOM;
     t->u.id = lisp_t_sym;

     return t;
}

PBoolean match_var(Symbol name, Envar *ev)
{
     return (name == ev->name);
}

Term *build_term_l_list_from_c_list(TermList tl)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = LISP_LIST;
     t->u.l_list = transform_c_list_to_l_list(tl);
     /* term_list has been destroyed by the transform... */

     return t;
}

/* Term *build_term_from_var_list(VarList vl) */
/* { */
/*      Term *t = MAKE_OBJECT(Term); */

/*      t->type = LENV; */
/*      t->u.var_list = vl; */

/*      return t; */
/* } */

Term *build_term_from_var(Envar *var)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = VARIABLE;
     t->u.var = var;

     return t;
}

PBoolean global_variable(Envar *ev)
{
     return ((ev->type == PROGRAM_VARIABLE) && (ev->name[1] == '@'));
}

Envar *build_var_in_frame(Symbol name, Variable_Type flag)
{
     Envar *var;

     
     if (allow_variable_parsing) {
	  if ((flag == PROGRAM_VARIABLE) && (name[1] == '@')) {
	       if ((var = (Envar *) sl_search_slist(global_var_list, name, match_var)) == (Envar *) NULL) {
		    var = build_envar(name, flag);
		    sl_add_to_head(global_var_list, var);
	       }
	  } else
	       if ((var = (Envar *) sl_search_slist(current_var_list, name, match_var)) == (Envar *) NULL) {
		    var = build_envar(name, flag);
		    sl_add_to_head(current_var_list, var);
	       }     
	  return dup_envar(var);
     } else {
	  fprintf(stdout, LG_STR("WARNING: Encountered the variable \"%s\" in a context where variables are not allowed.\n",
				 "WARNING: Encountered the variable \"%s\" in a context where variables are not allowed.\n"), name);
	  return build_envar(name, flag);

     }
}

Envar *build_envar(Symbol name, Variable_Type flag)
{

     Envar *var;

     var = MAKE_OBJECT(Envar);
     var->unif_type = NULL;
     var->type = flag;
     var->name = name;
     var->value = (Term *) NULL;

     return var;
}

Term *build_term_expr(Expression *termc)
{
     Term *t = MAKE_OBJECT(Term);

     t->type = EXPRESSION;
     t->u.expr = termc;

     return t;
}

Expression *build_simple_action_cached(Pred_Func_Rec *pfr, TermList i)
{

     Expression *expr = MAKE_OBJECT(Expression);
     Action *act;

     expr->pfr = pfr;
     expr->terms = i;

     if (!(act = pfr->u.u.act)) {
	  if (!(act = pfr->u.u.ef)) {
	       if (compiler_option[CHECK_ACTION]) {
		    printf(LG_STR("WARNING: while compiling the action form:\n\t",
				  "ATTENTION: en compilant l'action:\n\t"));
		    print_expr(expr);
		    printf(LG_STR("\n\tthe function %s has not been declared as\n\
\tan action or as an evaluable function.\n",
				  "\n\tla donction %s n'a pas été déclarée comme\n\
\tune action ou une fonction évaluable.\n"),
			   PFR_NAME(pfr));
	       }
	  } else {
	       if (compiler_option[CHECK_ACTION]) {
		    printf(LG_STR("The action %s will use the evaluable function of the same name.\n",
				  "L'action %s utilisera la function evaluable de mÍme nom.\n"), PFR_NAME(pfr));
	       }
	  }
     }

     if ((act) && (act->arity >= 0) && (act->arity != sl_slist_length(i))) {
	  printf(LG_STR("WARNING: while compiling the action form:\n\t",
			"WARNING: en compilant l'action:\n\t"));
	  print_expr(expr);
	  printf(LG_STR("\tthe function %s is called with %d arguments\n\
\tbut has been declared with %d.\n",
			"\tla fonction %s est appelée avec %d arguments\n\
\tmais a été déclarée avec %d.\n"), 
		 PFR_NAME(pfr),
		 sl_slist_length(i),
		 act->arity);
     }
     return expr;
}

Expression *build_expr_pfr_terms(Pred_Func_Rec *pfr, TermList i)
{

     Expression *expr = MAKE_OBJECT(Expression);

     expr->pfr = pfr;
     expr->terms = i;

     if (expr_temporal_type(expr) && (sl_slist_length(i) != 1 || ((Term *)sl_get_slist_head(i))->type != EXPRESSION)) {
	  fprintf(stderr, "Badly formed Temporal expression.\n");
	  return NULL;
     }


     return expr;
}

Expression *build_expr_sym_terms(Symbol predicat, TermList i)
{

     return build_expr_pfr_terms(find_or_create_pred_func(predicat), i);
}


Expression *build_expr_sym_terms_external(Symbol predicat, TermList i)
{

     return build_expr_pfr_terms_external(find_or_create_pred_func(predicat), i);
}

Expression *build_expr_pfr_terms_external(Pred_Func_Rec *pfr, TermList i)
{
     Expression *expr = build_expr_pfr_terms(pfr, i);

     if ((pfr->name) && (pfr->u.u.ef) && (pfr->u.u.ef->arity >= 0) &&
	 (pfr->u.u.ef->arity != sl_slist_length(i))) {
	  printf(LG_STR("WARNING: while compiling the function form: ",
			"WARNING: while compiling the function form: "));
	  print_expr(expr);
	  printf(LG_STR("the function %s is called with %d arguments but has been declared with %d.\n",
			"the function %s is called with %d arguments but has been declared with %d.\n"), 
		 PFR_NAME(pfr),
		 sl_slist_length(i),
		 pfr->u.u.ef->arity);
     }
     return expr;

}

Action_Field *build_action_field(Expression *tc, PBoolean special, PBoolean multi, List_Envar lv, Envar *var)
{

     Action_Field *res = MAKE_OBJECT(Action_Field);

     res->function = tc;
     res->special = special;
     res->multi = multi;
     if (special) {
	  if (multi) res->u.list_envar = lv;
	  else res->u.envar = var;
     }
     return res;
}

Expression *build_expr_pfr_expr(Pred_Func_Rec *pfr, Expression *i)
{

     Expression *t = MAKE_OBJECT(Expression);

     t->pfr = pfr;
     t->terms = sl_make_slist();
     sl_add_to_head(t->terms, build_term_expr(i));

     return t;
}

Expression *build_expr_sym_expr(Symbol predicat, Expression *i)
{

     return build_expr_pfr_expr(find_or_create_pred_func(predicat), i);
}

Expression *build_expr_pfr_exprs(Pred_Func_Rec *pfr, ExprList i)
{

     Expression *t = MAKE_OBJECT(Expression);

     t->pfr = pfr;
     t->terms = transform_el_tl(i);

     return t;
}

Expression *build_expr_sym_exprs(Symbol predicat, ExprList i)
{

     return build_expr_pfr_exprs(find_or_create_pred_func(predicat), i);
}

TermList build_term_list(TermList tl, Term *t)
{
     if (t) sl_add_to_tail(tl, t);

     return tl;
}

Property *build_property(PString name, Term *term)
{
     Property *p = MAKE_OBJECT(Property);

     p->name = name;
     p->value = term;
     return p;
}

PBoolean matching_envar(Envar *e1, Envar *e2)
{
     return ((! (e1->value && e2->value))  || (e1->value == e2->value));
}

PBoolean matching_lenv(List_Envar l1, List_Envar l2)
{
     Envar *ptr1, *ptr2;

     if (sl_slist_empty(l1) && sl_slist_empty(l2))
	  return TRUE;
     else {
	  ptr1 = (Envar *) sl_get_slist_next(l1, NULL);
	  ptr2 = (Envar *) sl_get_slist_next(l2, NULL);
	  while ((ptr1 != NULL) && (ptr2 != NULL)) {
	       if ((ptr1 == NULL) || (ptr2 == NULL))
		    return FALSE;
	       else if (matching_envar(ptr1, ptr2)) {
		    ptr1 = (Envar *) sl_get_slist_next(l1, ptr1);
		    ptr2 = (Envar *) sl_get_slist_next(l2, ptr2);
	       } else
		    return FALSE;
	  }
	  return TRUE;
     }
}

void init_hash_size_id(int hash_size)
{
     if(hash_size == 0)
	  id_hashtable_size = ID_HASHTABLE_DEFAULTSIZE;
     else if(hash_size < ID_HASHTABLE_MINSIZE) {
	  id_hashtable_size = ID_HASHTABLE_MINSIZE;
	  printf (LG_STR("The size of the id hashtable is: %d\n",
			 "La taille de la table des symboles est: %d\n"), 
		  id_hashtable_size);
     } else {
	  int i, size = hash_size -1;
	  for (i = 0; size >0; size = size>>1, i++); 
	  id_hashtable_size = 1<<i;
	  printf (LG_STR("The size of the id hashtable is: %d\n",
			 "La taille de la table des symboles est: %d\n"),
		  id_hashtable_size);
     }
     id_hashtable_mod =  id_hashtable_size - 1;
}

int hash_a_string_id(Symbol name)
{
     return (hash_a_string(name) & id_hashtable_mod);	/* replaced with a bitwise & because the size = 2**n... */
}

PBoolean match_string(PString s1, PString s2)
{
     return strcmp(s1, s2) == 0;
}

void free_global_var_list(void)
{
     Envar *env;
     sl_loop_through_slist(global_var_list, env,  Envar *) {
	  free_envar(env);
     }

     FREE_SLIST(global_var_list);

     global_var_list= NULL;
}

void make_global_var_list(void)
{
     global_var_list = sl_make_slist();
}

void show_global_variable(void)
{
     Envar *env;

     printf(LG_STR("The global variables are: (\n",
		   "Les variables globales sont: (\n"));
     sl_loop_through_slist(global_var_list, env,  Envar *) {
	  print_envar(env);
	  printf("\n");
     }
     printf(")\n");
}

void free_symbol(void *ignore, Symbol sym)
{
     char *free_sym = (char *)sym;	/* to avoid const warning. */
     FREE(free_sym);
}

void free_id_hash(void)
{
     sh_for_all_hashtable(id_hash, NULL, (SL_PFI)free_symbol);
     sh_free_hashtable(id_hash);
     
     id_hash = NULL;
}

void make_id_hash()
{
     id_hash = sh_make_hashtable(id_hashtable_size, hash_a_string_id, match_string);
}

void init_id_hash()
{
     DECLARE_TEXT_ID("SOAK", soak_sym);	/* Declare the soak symbol */
     DECLARE_TEXT_ID("APPLICABLE-OPS-FACT", app_ops_fact_sym);
     DECLARE_TEXT_ID("FACT-INVOKED-OPS", fact_invoked_ops_sym);
     DECLARE_TEXT_ID("DB-SATISFIED-GOAL", db_satisf_goal_sym);
     DECLARE_TEXT_ID("APPLICABLE-OPS-GOAL", app_ops_goal_sym);
     DECLARE_TEXT_ID("GOAL-INVOKED-OPS", goal_invoked_ops_sym);
     DECLARE_TEXT_ID("FAILED-GOAL", failed_goal_sym);
     DECLARE_TEXT_ID("FAILED", failed_sym);
     DECLARE_TEXT_ID("REQUEST", request_sym);
     DECLARE_TEXT_ID("ACHIEVED", achieved_sym);
     DECLARE_TEXT_ID("INTENTION-WAKE-UP", wake_up_intention_sym);
     DECLARE_TEXT_ID("READ-RESPONSE-ID", read_response_id_sym);
     DECLARE_TEXT_ID("READ-RESPONSE", read_response_sym);
     
     DECLARE_TEXT_ID("||", or_sym);
     DECLARE_TEXT_ID("&", and_sym);
     DECLARE_TEXT_ID("~", not_sym);
     DECLARE_TEXT_ID("+>", cond_add_sym);
     DECLARE_TEXT_ID("->", cond_del_sym);
     DECLARE_TEXT_ID("=>", temp_conclude_sym);
     DECLARE_TEXT_ID("~>", retract_sym);
     DECLARE_TEXT_ID("!", achieve_sym);
     DECLARE_TEXT_ID("?", test_sym);
     DECLARE_TEXT_ID("#", preserve_sym);
     DECLARE_TEXT_ID("^", twait_sym);
     DECLARE_TEXT_ID("%", maintain_sym);
     DECLARE_TEXT_ID("<>", call_sym);

     DECLARE_TEXT_ID("T", lisp_t_sym);/* Declare the T symbol */
     DECLARE_TEXT_ID("NIL", nil_sym);	/* Initialize the nil symbol */

     DECLARE_TEXT_ID("*=*", spec_act_sym);	/* Initialize the special action symbol */
     DECLARE_TEXT_ID(":WAIT", wait_sym);	/* Initialize the special action symbol */
     DECLARE_TEXT_ID(":WAIT-SON", wait_son_sym);	/* Initialize the special action symbol */
     DECLARE_TEXT_ID("CURRENT", current_sym);	/* declare the current keyword */
     DECLARE_TEXT_ID("QUOTE", quote_sym);	/* declare the quote keyword */
     DECLARE_TEXT_ID("DECISION-PROCEDURE", decision_procedure_sym);	/* declare the current keyword */

     DECLARE_TEXT_ID("OBJECT", object_sym);	/* Initialize the nil symbol */
     

}

int stat_id_hash_bucket (int *stat_array, PString name)
{
     int index = hash_a_string_id(name);

     stat_array[index] +=1;
     return 1;
}

void stat_id_hashtable(void)
{
     int i, *stat_array;
     int nb_elt, nb_bucket = 0, nb_elt_bucket, nb_max = 0;

     stat_array = (int *) MALLOC(id_hashtable_size * sizeof(int));
     for (i=0; i <id_hashtable_size; i++)
	  stat_array[i] = 0;

#ifdef VXWORKS
     take_or_create_sem(&id_hash_sem);
#endif     
     nb_elt = sh_for_all_hashtable(id_hash, stat_array, stat_id_hash_bucket);
#ifdef VXWORKS
     give_sem(id_hash_sem);
#endif     

     for (i=0; i <id_hashtable_size; i++)
	  if ((nb_elt_bucket = stat_array[i]) != 0){
	       nb_bucket++;
	       if (nb_elt_bucket > nb_max) nb_max = nb_elt_bucket; 
	  }
     printf (LG_STR("The id hashtable contains:\n",
		    "The id hashtable contains:\n"));
     printf (LG_STR("\t%d element(s)\n",
		    "\t%d element(s)\n"), nb_elt);
     printf (LG_STR("\tin  %d buckets ( %d );\n",
		    "\tin  %d buckets ( %d );\n"), nb_bucket, id_hashtable_size);
     printf (LG_STR("\twith a maximal number of %d element(s) in one bucket.\n",
		    "\twith a maximal number of %d element(s) in one bucket.\n"), nb_max);
     FREE(stat_array);
}

int hash_a_string(const char * name)
{
     register int hash_value = 0;
     register
     char *pointer;

     for (pointer = (char *)name; *pointer != '\0'; pointer++)
	  hash_value = (hash_value << 5) + hash_value + *pointer;	/* hash = hash * 33 + char */

     return hash_value;
}


void *select_randomly_c_list(Slist *l)
{
     int len = sl_slist_length(l);
     long rand_value;

     if (len == 0)
	  fprintf(stderr, LG_STR("empty list in select_randomly_c_list..\n",
				 "empty list in select_randomly_c_list..\n"));
     if (len == 1)
	  return sl_get_slist_head(l);
     rand_value = RANDOM();
     return sl_get_slist_pos(l, (rand_value % len) + 1);
}


PString get_and_check_from_hashtable(Shash *hash, PString from)
{
     static PBoolean first = TRUE;
     PString res = sh_get_from_hashtable(hash, from);

     if ((compiler_option[CHECK_SYMBOL]) && check_symbol && (res == NULL)) {
	  fprintf(stdout, LG_STR("WARNING: the symbol \"%s\" has not been declared.\n",
				 "ATTENTION: le symbole \"%s\" n'a pas été déclaré.\n"), from);
	  if (first) {
	       fprintf(stdout, LG_STR("\tUndeclared symbol means that this symbol has not been seen yet,\
\n\teither as an atom, a function or as a predicate.\
\n\tYou are currently compiling OPs and undeclared symbols are not an error,\
\n\thowever, it is a good practice to declare the symbol, predicate,\
\n\tand function name you will be using in one application.\
\n\tAmong other things, this facilitates the debugging of OPs.\
\n\t(this message will be printed once).\n",
"\tSymbole non déclaré signifie que le symbole n'a pas encore\
\n\tété rencontré comme atome, fonction ou prédicat.\
\n\tMême si la déclaration des symboles n'est pas obligatoire,\
\n\tles déclarer est préférable.\
\n\tCela facilite le debugging des OPs.\
\n\t(ce message ne sera imprimé qu'une fois).\n"));
	       first = FALSE;
	  }
     }
     return res;
}

PBoolean frame_eq(Frame *fr1, Frame *fr2)
{
     return (fr1 == fr2);
}

void add_frame_to_free(Frame *to_add, Frame *until, FrameList fl)
{
     if (to_add == until)
	  return;
     if (sl_search_slist(fl, to_add, frame_eq))
	  return;
     else {
	  sl_add_to_head(fl, to_add);
	  add_frame_to_free(to_add->previous_frame, until, fl);
     }
}

PString remove_vert_bar(PString name)
{
     PString s1, s2;

     NEWSTR(name, s1);
     if (s1[0] == '|') {
	  s1[strlen(s1) - 1] = '\0';
	  NEWSTR(s1 + 1, s2);
	  FREE(s1);
	  s1 = s2;
     }
     return s1;
}

Symbol make_atom(char *id)
{
     Symbol res;     

     if (id[0] == '|') {
	  int len = strlen(id);
	  if ((char *)strchr(id+1,'|') != (id + len - 1))
	       fprintf(stderr, LG_STR("Bad id in make_atom.\n",
				      "Bad id in make_atom.\n")); 
     }
     DECLARE_TEXT_ID(id,res);
     
     return res;
}

Symbol find_atom(char *id)
/* Call make_atom, but force symbol checking... Therefore, will generate a warning if 
   it has not been declared previously */
{
     Symbol res;
     PBoolean save_check_symbol = check_symbol;

     check_symbol = TRUE;
     res = make_atom(id);
     check_symbol = save_check_symbol;

     return res;
}

Symbol declare_atom(char *id)
/* Call make_atom, but disable symbol checking... */
{
     Symbol res;
     PBoolean save_check_symbol = check_symbol;

     check_symbol = FALSE;
     res = make_atom(id);
     check_symbol = save_check_symbol;

     return res;
}

PString make_string(char *string)
{
     int len = strlen(string);
     char *res;
     
     res = (char *)MALLOC(len+2+1);
     res[0] = res[len+1] = '"';
     res[len+2] ='\0';
     BCOPY(string,res+1,len);

     return res;
}

int debackslashify(char *string)
{				/* return the number of real new lines... */
     int i, j = 0; 
     int res = 0;
     char c;

     for (i = 0; i < (int)strlen(string); i++) {
	  switch (string[i]) {
	  case  '\\': 
	       switch (string[++i]) {
	       case '"':
		    c = '"';
		    break;
	       case 'n':
		    c = '\n';
		    break;
	       case 't':
		    c = '\t';
		    break;
	       case '\\':
		    c = '\\';
		    break;
	       default:
		    c = string[i];
	       }
	       break;
	  case  '\n':
	       res++;
	  default:
	       c = string[i];
	  }
	  string[j++] = c;
     }
     string[j] = '\0';

     return res;
}

PBoolean equal_string(PString s1, PString s2)
{
     return (strcmp(s1, s2) == 0);
}



#ifdef SPRINT_RETURN_STRING
int num_char_sprint(char *x) 
{
     return strlen(x);
}
#else
int num_char_sprint(int x) 
{
     return x;
}
#endif

void oprs_free_list(Slist *l)
{
     FREE_SLIST(l);
}

int compare_term(Term *t1, Term *t2)
{
     if (t1->type != t2->type)
	  return (t2->type - t1->type);
     else
	  switch(t1->type) { /* same type...*/
	  case INTEGER:
	       return ((t1->u.intval) - (t2->u.intval));
	  case LONG_LONG:
	       return ((t1->u.llintval) - (t2->u.llintval));
	  case TT_FLOAT:
	       return ((*t1->u.doubleptr) - (*t2->u.doubleptr));
	  case STRING:
	       return strcmp(t1->u.string, t2->u.string);
	  case TT_ATOM:
	       return strcmp(t1->u.id, t2->u.id);
	  case EXPRESSION:
	       return compare_expr(t1->u.expr, t2->u.expr);
	  case INT_ARRAY:
	  case FLOAT_ARRAY:
	  case LISP_LIST:
	  case U_POINTER:
	  case U_MEMORY:
	  case C_LIST:
	  case TT_FACT:
	  case TT_GOAL:
	  case TT_OP_INSTANCE:
	  case TT_INTENTION:
	  default:
	       return ((t1->u.u_pointer) == (t2->u.u_pointer));
	  }

}

int compare_terms(TermList terms1, TermList terms2)
{
     Term *t1 = NULL, *t2 = NULL;
     do {
	  int res;

	  t1 = (Term *)sl_get_slist_next(terms1, t1);
	  t2 = (Term *)sl_get_slist_next(terms2, t2);
	  if ((t1 == NULL) && (t2 == NULL)) return 0;
	  if (t1 == NULL) return -1;
	  if (t2 == NULL) return 1;
	  if ((res = compare_term(t1,t2)) != 0)
	       return res;
     } while (TRUE);
}

int compare_exprs(ExprList terms1, ExprList terms2)
{
     Expression *t1 = NULL, *t2 = NULL;
     do {
	  int res;

	  t1 = (Expression *)sl_get_slist_next(terms1, t1);
	  t2 = (Expression *)sl_get_slist_next(terms2, t2);
	  if ((t1 == NULL) && (t2 == NULL)) return 0;
	  if (t1 == NULL) return -1;
	  if (t2 == NULL) return 1;
	  if ((res = compare_expr(t1,t2)) != 0)
	       return res;
     } while (TRUE);
}

int compare_expr(Expression *expr1, Expression *expr2)
{
     if (expr1->pfr != expr2->pfr) 
	  return (strcmp(expr1->pfr->name, expr2->pfr->name));
     else
	  return compare_terms(expr1->terms, expr2->terms);
}

PBoolean expr_sort_pred(Expression *e1, Expression *e2)
{
     return (compare_expr(e1,e2) <= 0);
}

#ifdef IGNORE
Add_Del_List *make_add_del_list(void)
{
     Add_Del_List *adl = MAKE_OBJECT(Add_Del_List);
     
     adl->add = sl_make_slist();
     adl->del = sl_make_slist();

     adl->cond_add = sl_make_slist();
     adl->cond_del = sl_make_slist();

     return adl;
}

void free_add_del_list(Add_Del_List *adl)
{
     Expression *tmp;
     
     if (adl) {
	  sl_loop_through_slist(adl->add, tmp, Expression *)
	       free_expr(tmp);
	  sl_free_slist(adl->add);

	  sl_loop_through_slist(adl->del, tmp, Expression *)
	       free_expr(tmp);
	  sl_free_slist(adl->del);

	  sl_loop_through_slist(adl->cond_add, tmp, Expression *)
	       free_expr(tmp);
	  sl_free_slist(adl->cond_add);

	  sl_loop_through_slist(adl->cond_del, tmp, Expression *)
	       free_expr(tmp);
	  sl_free_slist(adl->cond_del);

	  FREE(adl);
     }
}

void fprint_add_del_list(FILE *f,Add_Del_List *adl)
{
     Expression *tmp;

     fprintf(f,"(");
     if (adl) {
	  if (! sl_slist_empty(adl->add)) {
	       fprintf(f,"(=> ");
	       sl_loop_through_slist(adl->add, tmp, Expression *)
		    fprint_expr(f,tmp);
		    fprintf(f,")");
	  }

	  if (! sl_slist_empty(adl->del)) {
	       fprintf(f,"(~> ");
	       sl_loop_through_slist(adl->del, tmp, Expression *)
		    fprint_expr(f,tmp);
		    fprintf(f,")");
	  }	  
	  if (! sl_slist_empty(adl->cond_add)) {
	       fprintf(f,"(+> ");
	       sl_loop_through_slist(adl->cond_add, tmp, Expression *)
		    fprint_expr(f,tmp);
		    fprintf(f,")");
	  }

	  if (! sl_slist_empty(adl->cond_del)) {
	       fprintf(f,"(-> ");
	       sl_loop_through_slist(adl->cond_del, tmp, Expression *)
		    fprint_expr(f,tmp);
		    fprintf(f,")");
	  }	  
     }
     fprintf(f,")");
}

void print_add_del_list(Add_Del_List *adl)
{
     fprint_add_del_list(stdout,adl);
}

#endif

Expression *not_expr_not (Expression *expr)
{
     if (!NOT_P(expr) || (sl_slist_length(expr->terms) != 1) ||
         (((Term *)sl_get_slist_head(expr->terms))->type != EXPRESSION)) {
          fprintf(stderr, "not_expr_not: not a negated expr\n");
          return NULL;
     } else
          return ((Term *)sl_get_slist_head(expr->terms))->u.expr;
}

Expression *not_expr_copy (Expression *expr)
{
     if (NOT_P(expr))
          if (sl_slist_length(expr->terms) == 1 &&
              (((Term *)sl_get_slist_head(expr->terms))->type == EXPRESSION))
               return copy_expr(((Term *)sl_get_slist_head(expr->terms))->u.expr);
          else {
               fprintf(stderr, "not_expr_copy: not 1 expr in a negated expr\n");
               return NULL;
          }
     else {
          return build_expr_sym_expr(not_sym,copy_expr(expr));
     }
}

Expression *not_expr_dup (Expression *expr)
{
     if (NOT_P(expr)) {
          if (sl_slist_length(expr->terms) == 1 &&
              (((Term *)sl_get_slist_head(expr->terms))->type == EXPRESSION))
               return dup_expr(((Term *)sl_get_slist_head(expr->terms))->u.expr);
          else {
               fprintf(stderr, "not_expr_dup: not 1 expr in a negated expr\n");
               return NULL;
          }
     } else {
          return build_expr_sym_expr(not_sym,dup_expr(expr));
     }
}

Temporal_Ope expr_temporal_type(Expression *expr)
{
     if (expr->pfr->name ==  achieve_sym)
	  return ACHIEVE;
     if (expr->pfr->name ==  test_sym)
	  return TEST;
     if (expr->pfr->name ==  twait_sym)
	  return WAIT;
     if (expr->pfr->name ==  call_sym)
	  return CALL;
     if (expr->pfr->name ==  maintain_sym)
	  return ACT_MAINT;
     if (expr->pfr->name ==  preserve_sym)
	  return PASS_MAINT;
     return FALSE;
}


DB_Ope expr_db_type (Expression *expr)
{
     if (expr->pfr->name ==  temp_conclude_sym)
	  return DB_CONC;
     if (expr->pfr->name ==  retract_sym)
	  return DB_RET;

     if (expr->pfr->name ==  cond_add_sym)
	  return DB_COND_CONC;
     if (expr->pfr->name ==  cond_del_sym)
	  return DB_COND_RET;

     return FALSE;
}

ExprList expr_list_from_term_list(TermList tl)
{
     Term *term;
     ExprList res = sl_make_slist();

     sl_loop_through_slist(tl, term, Term  *) 
	  if (TYPE(term) == EXPRESSION)
	       sl_add_to_tail(res, term->u.expr);
	  else
	       fprintf(stderr, "expr_list_from_term_list: not an expr in the term.\n");
     return res;
}

TermList transform_el_tl(ExprList tl)
{
     return sl_replace_slist_func(tl, (SL_PFPV)build_term_expr);
}

Expression *expr_from_term(Term *t)
{
     return TERM_EXPR(t);
}

ExprList make_el_from_tl(TermList tl)
{
     return sl_copy_slist_func(tl, (SL_PFPV)expr_from_term);
}

#ifdef VXWORKS
int strcasecmp(char *a,char *b) 
{

     char *aa, *bb;
     int res,i;

     NEWSTR(a,aa);
     NEWSTR(b,bb);

     for(i = 0; i < strlen(aa); i++) 
	  if (islower(aa[i])) aa[i] =  toupper(aa[i]);
     for(i = 0; i < strlen(bb); i++) 
	  if (islower(bb[i])) bb[i] =  toupper(bb[i]);

     res =  strcmp(aa,bb);
     FREE(aa);
     FREE(bb);
     
     return res;
}

#endif
