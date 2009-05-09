static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-pred.c -- 
 * 
 * Copyright (c) 1991-2009 Francois Felix Ingrand.
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
#include "semLib.h"
#endif

#include <stdio.h>
#include <shashPack.h>
#include <shashPack_f.h>
#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "oprs-type.h"
#include "oprs-pred-func.h"
#include "ev-predicate.h"
#include "ev-function.h"

#include "oprs-print_f.h"
#include "oprs-type_f.h"
#include "oprs-pred-func_f.h"

Pred_Func_Rec *soak_pred;
Pred_Func_Rec *app_ops_fact_pred;
Pred_Func_Rec *fact_invoked_ops_pred;
Pred_Func_Rec *db_satisf_goal_pred;
Pred_Func_Rec *app_ops_goal_pred;
Pred_Func_Rec *goal_invoked_ops_pred;
Pred_Func_Rec *failed_goal_pred;
Pred_Func_Rec *failed_pred;
Pred_Func_Rec *request_pred;
Pred_Func_Rec *achieved_pred;
Pred_Func_Rec *wake_up_intention_pred;
Pred_Func_Rec *read_response_id_pred;
Pred_Func_Rec *read_response_pred;

#define PRED_HASHTABLE_MINSIZE 64
#define PRED_HASHTABLE_DEFAULTSIZE 64 /* has to be a power of 2 for the bitwise modulo */
static int pred_func_hashtable_size, pred_func_hashtable_mod;

Shash *pred_func_hash;
Slist *var_pred_func_list;

#ifdef VXWORKS
static SEM_ID pred_func_hash_sem = NULL;
#endif     

PBoolean check_pfr = TRUE;

int hash_a_string_pred_func(PString name )	
{
  return (hash_a_string(name) & pred_func_hashtable_mod);
}

PBoolean match_pred_func(PString s1, Pred_Func_Rec *pfr)
/* their names are registered ID */
{
     return ( s1 == pfr->name ) ;
}


Symbol pred_func_rec_symbol(Pred_Func_Rec *pfr)
{
     return pfr->name;
}

void make_pred_func_hash()
{
     pred_func_hash = sh_make_hashtable(pred_func_hashtable_size, hash_a_string_pred_func, match_pred_func);
     var_pred_func_list = sl_make_slist();
}

Pred_Rec *make_pred_rec(void)
{
     int i;
     Pred_Rec *pr = MAKE_OBJECT(Pred_Rec);

     pr->ep = NULL;
     pr->cwp = FALSE;
     pr->be = FALSE;
     pr->op_pred = FALSE;
     pr->ff = -1;
     pr->arity = -1;
     for(i = 0; i < PASS_MAINT; i++)
	  pr->cached_rop[i] = NULL;

     pr->conditions = NULL;

     return pr;
}

void free_pred_rec(Pred_Rec *pr)
{
     int i;

     if (pr->ep) FREE(pr->ep);
     for(i = 0; i < PASS_MAINT; i++)
	  if (pr->cached_rop[i])
	       FREE_SLIST(pr->cached_rop[i]);
     SAFE_SL_FREE_SLIST(pr->conditions) ;
     FREE(pr);
}

Pred_Rec *declare_pfr_as_pred(Pred_Func_Rec *pfr)
{
     if (!pfr->u.u.pr) pfr->u.u.pr = make_pred_rec();

     return pfr->u.u.pr;
}

void free_pred_func_rec(void *ignore, Pred_Func_Rec *pfr)
{
     if (pfr->name) {
	  if (pfr->u.u.pr) free_pred_rec(pfr->u.u.pr);
	  if (pfr->u.u.ef) FREE(pfr->u.u.ef);
	  if (pfr->u.u.act) FREE(pfr->u.u.act);
     } else {
	  free_envar(pfr->u.envar);
     }
     FREE(pfr);
}

void free_pred_func_hash(void)
{
     Pred_Func_Rec *pr;

     sh_for_all_hashtable(pred_func_hash, NULL, free_pred_func_rec);
     sh_free_hashtable(pred_func_hash);

     sl_loop_through_slist(var_pred_func_list, pr, Pred_Func_Rec *) {
	  free_pred_func_rec(NULL, pr);
     }
     
     FREE_SLIST(var_pred_func_list);

     pred_func_hash = NULL;
     var_pred_func_list = NULL;
}

Pred_Func_Rec *make_pred_func_rec(Symbol name)
{

     Pred_Func_Rec *pfr = MAKE_OBJECT(Pred_Func_Rec);
     pfr->name = name;
     pfr->u.u.pr =  NULL;
     pfr->u.u.act=  NULL;
     pfr->u.u.ef= NULL;
     return pfr;
}

Pred_Func_Rec *create_var_pred_func(Envar *var)
{
     Pred_Func_Rec *pr = MAKE_OBJECT(Pred_Func_Rec);

     pr->name = NULL;
     pr->u.envar = var;
     
     sl_add_to_head(var_pred_func_list, pr);

     return pr;

}

Pred_Func_Rec *find_or_create_pred_func(Symbol pred_func_name)
{
     Pred_Func_Rec *pred_func_rec;

#ifdef VXWORKS
     take_or_create_sem(&pred_func_hash_sem);
#endif     
/*      fprintf(stderr,"pfr: %s\n", pred_func_name);  */

     if ((pred_func_rec = (Pred_Func_Rec *)sh_get_from_hashtable(pred_func_hash,pred_func_name)) == NULL) {
	  if ((compiler_option[CHECK_PFR]) && check_pfr) 
	       fprintf(stdout, LG_STR("WARNING: The predicate \"%s\" has not been declared.\n",
				      "ATTENTION: Le prédicat \"%s\" n'a pas été declaré.\n"), pred_func_name); 
	  pred_func_rec = make_pred_func_rec(pred_func_name); 
	  sh_add_to_hashtable(pred_func_hash,pred_func_rec,pred_func_name); 
     }

#ifdef VXWORKS
     give_sem(pred_func_hash_sem);
#endif     

     return pred_func_rec;
}

Pred_Func_Rec *find_pred_func(Symbol pred_func_name)
{
     Pred_Func_Rec *pred_func_rec;

#ifdef VXWORKS
     take_or_create_sem(&pred_func_hash_sem);
#endif     

     pred_func_rec = (Pred_Func_Rec *)sh_get_from_hashtable(pred_func_hash,pred_func_name);

#ifdef VXWORKS
     give_sem(pred_func_hash_sem);
#endif     

     return pred_func_rec;
}

void init_hash_size_pred_func(int hash_size)
{
     if(hash_size == 0)
	  pred_func_hashtable_size = PRED_HASHTABLE_DEFAULTSIZE;
     else {
	  if(hash_size < PRED_HASHTABLE_MINSIZE) {
	       pred_func_hashtable_size = PRED_HASHTABLE_MINSIZE;
	  } else {
	       int i, size = hash_size -1;
	       for (i = 0; size >0; size = size>>1, i++); 
	       pred_func_hashtable_size = 1<<i;
	  }
	  printf (LG_STR("The size of the predicates hashtable is: %d\n",
			 "La taille de la table de hashage des prédicats est: %d\n"), pred_func_hashtable_size);
     }

     pred_func_hashtable_mod =  pred_func_hashtable_size - 1;
}

int stat_pred_func_hash_bucket (int *stat_array, Pred_Func_Rec *pr)
{
     int index = hash_a_string_pred_func(pr->name);

     stat_array[index] +=1;
     return 1;
}

void stat_pred_func_hashtable(void)
{
     int i, *stat_array;
     int nb_elt, nb_bucket = 0, nb_elt_bucket, nb_max = 0;

     stat_array = (int *) MALLOC(pred_func_hashtable_size * sizeof(int));
     for (i=0; i <pred_func_hashtable_size; i++)
	  stat_array[i] = 0;

     nb_elt = sh_for_all_hashtable(pred_func_hash, stat_array, stat_pred_func_hash_bucket);

     for (i=0; i <pred_func_hashtable_size; i++)
	  if ((nb_elt_bucket = stat_array[i]) != 0){
	       nb_bucket++;
	       if (nb_elt_bucket > nb_max) nb_max = nb_elt_bucket; 
	  }
     printf (LG_STR("The predicate hashtable contains:\n",
		    "La table de hashage des prédicats contient:\n"));
     printf (LG_STR("\t%d element(s)\n",
		    "\t%d élément(s)\n"), nb_elt);
     printf (LG_STR("\tin  %d buckets ( %d );\n",
		    "\tdans %d baquets ( %d );\n"), nb_bucket, pred_func_hashtable_size);
     printf (LG_STR("\twith a maximal number of %d element(s) in one bucket.\n",
		    "\tavec un nombre maximum de %d élément(s) dans un baquet.\n"), nb_max);
     FREE(stat_array);
}

Pred_Func_Rec *declare_pred_from_symbol(Symbol pfr_name)
{
     Pred_Func_Rec *pfr;

     check_pfr = FALSE;
     pfr= find_or_create_pred_func(pfr_name);
     check_pfr = TRUE;
     
     declare_pred(pfr);

     return pfr;
}

Pred_Rec *declare_pred(Pred_Func_Rec *pfr)
{
     return declare_pfr_as_pred(pfr);
}

void declare_cwp(Pred_Func_Rec *pfr)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr)))
	  DECLARE_PRED_CWP(pr);
}

void declare_predicate_op(Pred_Func_Rec *pfr)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr)))
	  DECLARE_PRED_OP(pr);
}

void un_declare_predicate_op(Pred_Func_Rec *pfr)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr)))
	  UNDECLARE_PRED_OP(pr);
}

void declare_ff(Pred_Func_Rec *pfr, int ar)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr))) {
	  if ((pr->ff) >= 0) {
	       printf(LG_STR("WARNING: declare_ff: you are changing or redeclaring\
\n\tthe functional predicate: %s.\n",
			     "ATTENTION: declare_ff: vous modifiez ou redéclarez\
\n\tle prédicat fonctionel: %s.\n"), PRED_NAME(pfr));
	  }
	  DECLARE_PRED_FF(pr,ar);
     }
}


void declare_be(Pred_Func_Rec *pfr)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr)))
	  DECLARE_PRED_BE(pr);
}

void un_declare_be(Pred_Func_Rec *pfr)
{
     Pred_Rec *pr;
     
     if ((pr = declare_pred(pfr)))
	  UNDECLARE_PRED_BE(pr);
}

void  check_or_set_pred_func_arity(Pred_Func_Rec *pfr, int arity)
{

}


/* Expression *build_expr_pfr(Pred_Func_Rec *pfr, TermList i) */
/* { */
/*      int arity = sl_slist_length(i); */
/*      Expression *t = MAKE_OBJECT(Expression); */

/*      declare_pfr_as_pred(pfr); */
/*      TYPE(t) = TERM_LIST_ET; */
/*      t->pfr = pfr; */
/*      check_or_set_pred_func_arity(pfr, arity); */
/*      t->u.terms = i; */

/*      if ((pr->name) && (pr->u.ep) && (pr->u.ep->arity >= 0) && (pr->u.ep->arity != arity)) { */
/* 	  printf(LG_STR("WARNING: while compiling: ", */
/* 			"ATTENTION: lors de la compilation de: ")); */
/* 	  print_expr(t); */
/* 	  printf(LG_STR("the evaluable predicat %s is called with %d arguments but has been declared with %d.\n", */
/* 			"le prédicat évaluable %s est appelé avec %d arguments mais a été déclaré pour %d.\n"),  */
/* 		 PRED_NAME(pr), */
/* 		 arity, */
/* 		 pr->u.ep->arity); */
/*      } */

PBoolean sort_pr(Pred_Func_Rec *pr1 , Pred_Func_Rec *pr2)
{
     return (strcmp(pr1->name,pr2->name) < 0);
}

void list_pred_func(int (*collect_func)(Slist *, Pred_Func_Rec *), void (*print_func)(Pred_Func_Rec *), char *string)
{
     Pred_Func_Rec *pr;
     Slist *ep_to_print = sl_make_slist();

     sh_for_all_hashtable(pred_func_hash, ep_to_print, collect_func);
     sl_sort_slist_func(ep_to_print,sort_pr);

     printf("%s",string);
     sl_loop_through_slist(ep_to_print, pr, Pred_Func_Rec *) {
	  (* print_func)(pr);
     }
     printf(".\n");

     FREE_SLIST(ep_to_print);
}

void print_hash_pred_func_cwp_be(Pred_Func_Rec *pr)
{
     printf("%s\n", pr->name);
}

void print_hash_ep(Pred_Func_Rec *pfr)
{
     printf("%s %d\n", pfr->name, pfr->u.u.pr->arity);
}

void print_hash_ff(Pred_Func_Rec *pfr)
{
     printf("%s %d\n", pfr->name, pfr->u.u.pr->ff);
}


int collect_hash_ff(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr && pfr->u.u.pr->ff != -1)
	  sl_add_to_head(pfr_to_print,pfr);
     
     return 1;			/* Just to count them.... */
}

int collect_hash_predicate(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr)
	  sl_add_to_head(pfr_to_print,pfr);

     return 1;			/* Just to count them.... */
}

int collect_hash_op_predicate(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr && pfr->u.u.pr->op_pred)
	  sl_add_to_head(pfr_to_print,pfr);

     return 1;			/* Just to count them.... */
}

int collect_hash_cwp(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr && pfr->u.u.pr->cwp)
	  sl_add_to_head(pfr_to_print,pfr);

     return 1;			/* Just to count them.... */
}

int collect_hash_be(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr && pfr->u.u.pr->be)
	  sl_add_to_head(pfr_to_print,pfr);

     return 1;			/* Just to count them.... */
}

int collect_hash_ep(Slist *pfr_to_print, Pred_Func_Rec *pfr)
{
     if (pfr->u.u.pr && pfr->u.u.pr->ep)
	  sl_add_to_head(pfr_to_print,pfr);
     return 1;			/* Just to count them.... */
}

void list_be()
{
     list_pred_func(collect_hash_be, print_hash_pred_func_cwp_be, "The predicates declared basic event are: \n");
}

void list_ff()
{
     list_pred_func(collect_hash_ff, print_hash_ff, "The predicates declared functional facts are: \n");
}

void list_cwp()
{
     list_pred_func(collect_hash_cwp, print_hash_pred_func_cwp_be, "The predicates declared as closed world predicate are: \n");
}

void list_ep()
{
     list_pred_func(collect_hash_ep, print_hash_ep, "The predicates declared evaluable are: \n");
}

void list_op_predicate()
{
     list_pred_func(collect_hash_op_predicate, print_hash_pred_func_cwp_be, "The OP predicates are: \n");
}

void list_predicate()
{
     list_pred_func(collect_hash_predicate, print_hash_pred_func_cwp_be, "The predicates are: \n");
}


void print_hash_ac(Pred_Func_Rec *fr)
{
     printf("%s %d\n", fr->name, fr->u.u.act->arity);
}

void print_hash_ef(Pred_Func_Rec *fr)
{
     printf("%s %d\n", fr->name, fr->u.u.ef->arity);
}

void print_hash_function(Pred_Func_Rec *fr)
{
     printf("%s\n", fr->name);
}

int collect_hash_ef(Slist *fr_to_print, Pred_Func_Rec * fr)
{
     if (fr->u.u.ef)
	  sl_add_to_head(fr_to_print,fr);

     return 1;			/* Just to count them.... */
}

int collect_hash_function(Slist *fr_to_print, Pred_Func_Rec * fr)
{
     sl_add_to_head(fr_to_print,fr);

     return 1;			/* Just to count them.... */
}

int collect_hash_ac(Slist *fr_to_print, Pred_Func_Rec * fr)
{
     if (fr->u.u.act)
	  sl_add_to_head(fr_to_print,fr);

     return 1;			/* Just to count them.... */
}

void list_function()
{
     list_pred_func(collect_hash_function, print_hash_function,"The functions or symbol recognized as functions are: \n");
}

void list_ef()
{
     list_pred_func(collect_hash_ef, print_hash_ef,"The functions declared evaluable are: \n");
}

void list_ac()
{
     list_pred_func(collect_hash_ac, print_hash_ac,"The actions declared are: \n");
}


void make_and_declare_eval_funct(char *name, PFPT funct, int ar)
{
     Symbol name_tmp2;
     Pred_Func_Rec *pfr;
     Eval_Funct *ef;

     DECLARE_TEXT_ID(name,name_tmp2);

     check_pfr = FALSE;
     pfr = find_or_create_pred_func(name_tmp2);
     check_pfr = TRUE;

     if ((ef = pfr->u.u.ef)) {
	  fprintf(stderr, LG_STR("WARNING: make_and_declare_eval_funct: You are redefining the evaluable function: %s.\n",
				 "ATTENTION: make_and_declare_eval_funct: Vous redéfinissez la fonction évaluable: %s.\n"), pfr->name);
     } else {
	  ef = MAKE_OBJECT(Eval_Funct);
	  pfr->u.u.ef = ef;
#ifdef OPRS_PROFILING
     	  ef->time_active = zero_date;
	  ef->nb_call = 0;
#endif
     }
     ef->arity = ar;
     ef->function = funct;
}

void make_and_declare_action(char *name,  PFPT funct,  int ar)
{
     Symbol name_tmp2;
     Pred_Func_Rec *pfr;
     Action *ac;

     DECLARE_TEXT_ID(name,name_tmp2);
     check_pfr = FALSE;
     pfr = find_or_create_pred_func(name_tmp2);
     check_pfr = TRUE;

    if ((ac = pfr->u.u.act)) {
	 fprintf(stderr, LG_STR("WARNING: make_and_declare_action: You are redefining the action: %s.\n",
				"ATTENTION: make_and_declare_action: Vous redéfinissez l'action: %s.\n"), pfr->name);
    } else {
	  ac = MAKE_OBJECT(Action);
#ifdef OPRS_PROFILING
     	  ac->time_active = zero_date;
	  ac->nb_call = 0;
#endif
	  pfr->u.u.act = ac;
    }
    ac->arity = ar;
    ac->function = funct;
}


void make_and_declare_eval_pred_internal(Symbol name,  PFB pred, int ar, PBoolean cwp, PBoolean eval_var)
{
     char *name_tmp2;
     Pred_Func_Rec *pfr;
     Pred_Rec *pr;
     Eval_Pred *ep;

     DECLARE_TEXT_ID(name,name_tmp2);
     check_pfr = FALSE;
     pfr = find_or_create_pred_func(name_tmp2);
     pr = declare_pred(pfr);
     check_pfr = TRUE;
     if ((ep = pr->ep)) {		/* If it is already an evaluable predicate... */
	  fprintf(stderr, LG_STR("WARNING: make_and_declare_eval_pred: You are redefining the evaluable predicate: %s.\n",
				 "ATTENTION: make_and_declare_eval_pred: Vous redéfinissez le prédicat évaluable: %s.\n"), pfr->name);
     } else {
	  ep = MAKE_OBJECT(Eval_Pred);
#ifdef OPRS_PROFILING
     	  ep->time_active = zero_date;
	  ep->nb_call = 0;
#endif
	  DECLARE_PRED_EP(pr,ep);
     }

     ep->arity = ar;
     ep->predicat = pred;
     ep->eval_var = eval_var;

     if (cwp) pr->cwp = TRUE;
}

void make_and_declare_eval_pred(Symbol name,  PFB pred, int ar, PBoolean cwp)
{
     make_and_declare_eval_pred_internal(name, pred, ar, cwp, TRUE);

}

void make_and_declare_eval_pred_no_eval_var(Symbol name,  PFB pred, int ar, PBoolean cwp)
{
     make_and_declare_eval_pred_internal(name, pred, ar, cwp, FALSE);
}

