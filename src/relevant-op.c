/*                               -*- Mode: C -*- 
 * relevant_op.c -- Gestion de la table des procedures "relevants"
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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


#include "constant.h"

#ifdef OPRS_PROFILING
#endif


#ifdef GRAPHIX
#include <Xm/Xm.h>
#include "ope-graphic.h"
#include "ope-external_f.h"
#endif

#include "op-structure.h"
#include "relevant-op.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"

#include "op-structure_f.h"
#include "op-compiler_f.h"
#include "relevant-op_f.h"
#include "oprs-type_f.h"
#include "oprs-print_f.h"
#include "oprs-error_f.h"
#include "oprs-pred-func_f.h"

void add_op_call_to_relevant_op(Op_Structure *op, Expression *expr);

PBoolean equal_op_name_and_file(Op_Structure *op1, Op_Structure *op2)
{
     return ((op1->name == op2->name) && ((strcmp(op1->file_name,op2->file_name)) == 0));
}

void add_op_file_name(PString file_name, Relevant_Op * rk)
{
     if (file_name) {
	  PString new;

	  if (!sl_search_slist(rk->opf_list, file_name, equal_string)) {
	       NEWSTR(file_name, new);
	       sl_add_to_head(rk->opf_list,new);
	  } 
     }
}

void add_op_to_relevant_op_internal(Op_Structure *op, Relevant_Op *rk)
{
     if (! (op->invocation || op->call)) {
	  fprintf(stderr,LG_STR("ERROR: This op has no invocation or call part!\n",
				"ERREUR: Ce OP n'a pas de partie d'invocation ou d'appel!\n"));
	  return;
     }
     if ((op->start_point == NULL) && (op->action == NULL) && (op->body == NULL)){
	  fprintf(stderr,LG_STR("ERROR: The OP %s has neither start node nor action part.\n",
				"ERREUR: Le OP n'a ni noeud start ni partie action.\n"), op->name);
	  return;
     }

     if (sl_search_slist(rk->op_list, op, equal_op_name_and_file)) {
	  printf(LG_STR("WARNING: The OP: %s in \"%s\"\
\n\tis already loaded, ignoring it. \n",
			"ATTENTION:Le OP: %s dans \"%s\"\
\n\ttest déjà chargé, on l'ignore. \n"), 
		 op->name, op->file_name);
	  free_op(op);
	  return;
     }
	  
     if (op->invocation) add_op_expr_to_relevant_op(op, op->invocation);
     if (op->call) add_op_call_to_relevant_op(op, op->call);
     sl_add_to_tail(rk->op_list,op);
     add_op_file_name(op->file_name, rk); /* Over do it... but what the heck.. */
}

void change_op_file_name(PString old_file_name, PString new_file_name, Relevant_Op *rk)
{
     PString found;

     if ((found = (PString)sl_search_slist(rk->opf_list, old_file_name, equal_string))) {
	  sl_delete_slist_node(rk->opf_list, found);
	  FREE(found);
     } else 
	  printf(LG_STR("change_op_file_name: no OP file named %s found in this kernel.\n",
			"change_op_file_name: pas de fichier de OP nommé %s dans ce noyau.\n"), old_file_name);

     if (new_file_name) {
	  PString new;

	  NEWSTR(new_file_name, new);
	  sl_add_to_head(rk->opf_list,new);
     } 
}

Op_Expr *make_op_expr(Op_Structure *op, Expression *expr)
{
     Op_Expr *opexpr = MAKE_OBJECT(Op_Expr);

     opexpr->op = op;
     opexpr->expr = expr;

     return opexpr;
}

PBoolean equal_opm_opm(Op_Expr *opm1, Op_Expr *opm2)
{
     return ((opm1->op == opm2->op) && (opm1->expr == opm2->expr));
}

void delete_op_expr_from_pfr(Pred_Func_Rec *pfr, Temporal_Ope index, Op_Expr *opm)
{
     Op_Expr *opm_to_del;

     if ((opm_to_del = (Op_Expr *)sl_search_slist(PFR_PR(pfr)->cached_rop[index], opm, equal_opm_opm))) {
	  sl_delete_slist_node(PFR_PR(pfr)->cached_rop[index],opm_to_del);
	  FREE(opm_to_del);
     } else
	  fprintf(stderr,LG_STR("ERROR: delete_op_expr_from_pr: cannot find the OP in the predicat.\n",
				"ERREUR: delete_op_expr_from_pr: ne retrouve pas le OP dans le prédicat.\n"));
}

void delete_op_call_from_pfr(Op_Structure *op, Expression *expr)
{
     Op_Expr *opm_to_del;
     Pred_Rec *pr;

     pr = PFR_PR(expr->pfr);
     
     opm_to_del = (Op_Expr *)sl_get_from_head(pr->cached_rop[CALL]);
     if (opm_to_del) FREE(opm_to_del);
}

void delete_op_from_pfr(Op_Structure *op, Expression *expr)
{
     Temporal_Ope top;
     Pred_Func_Rec *pfr;
     Op_Expr *opg;


     if (expr == NULL)  {
	  fprintf(stderr,LG_STR("delete_op_from_cacher_pred: no Invocation Part.\n",
				"delete_op_from_cacher_pred: pas de partie d'invocation.\n"));
	  return;
     }

     if (expr == NULL)  {
	  fprintf(stderr,LG_STR("add_op_expr_to_relevant_op: no invocation part.\n",
				"add_op_expr_to_relevant_op: pas de partie d'invocation.\n"));
	  return;
     }

     
     if ((top = expr_temporal_type(expr))) {
	  pfr = EXPR_EXPR1(expr)->pfr;
	  opg = make_op_expr(op, expr);
	  delete_op_expr_from_pfr(pfr, top, opg);
	  FREE(opg);
     } else if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
	  Term *term;
	  sl_loop_through_slist(expr->terms, term, Term *)
	       delete_op_from_pfr(op, TERM_EXPR(term));
     } else {
	  pfr = expr->pfr;
	  opg = make_op_expr(op, expr);
	  delete_op_expr_from_pfr(pfr, 0, opg);
	  FREE(opg);
     }
}

void clear_relevant_op(Relevant_Op *rk)
{
     PString opf_name;
     Op_Structure *op;

     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  if (op->invocation) delete_op_from_pfr(op, op->invocation);
	  if (op->call) delete_op_call_from_pfr(op, op->call);
	  free_op(op);
     }
     FLUSH_SLIST(rk->op_list);

     sl_loop_through_slist(rk->opf_list, opf_name, PString) {
	  FREE(opf_name);
     }
     FLUSH_SLIST(rk->opf_list);
}

void free_relevant_op(Relevant_Op *rk)
{
     clear_relevant_op(rk);
     FREE_SLIST(rk->op_list);
     FREE_SLIST(rk->opf_list);

     FREE(rk);
}

Relevant_Op *make_relevant_op(void)
{
     Relevant_Op *rk =  MAKE_OBJECT(Relevant_Op);

     rk->op_list = sl_make_slist();
     rk->opf_list = sl_make_slist();

     return rk;
}

void add_op_expr_to_pr(Pred_Func_Rec *pfr, Temporal_Ope index, Op_Expr *opm)
{
     if (! (pfr->u.u.pr))
	  pfr->u.u.pr = make_pred_rec(); 
     if (! (pfr->u.u.pr->cached_rop[index]))
	  pfr->u.u.pr->cached_rop[index] = sl_make_slist();
     else if ((index == CALL) && !sl_slist_empty(pfr->u.u.pr->cached_rop[CALL])) {
	  Op_Expr *opm2;
	  
	  opm2 =  (Op_Expr *)sl_get_from_head(pfr->u.u.pr->cached_rop[CALL]);

	  fprintf(stderr,LG_STR("add_op_expr_to_pr: Already an OP (%s) with this CALL predicate %s.\n\tRedefining it with %s.\n",
				"add_op_expr_to_pr: Already an OP (%s) with this CALL predicate %s.\n\tRedefining it with %s.\n"),
		  opm2->op->name, pfr->name, opm->op->name);
	  FREE(opm2);
     }
     sl_add_to_tail(pfr->u.u.pr->cached_rop[index],opm);
}


void add_op_call_to_relevant_op(Op_Structure *op, Expression *expr)
{
     Pred_Func_Rec *pfr;
     Op_Expr *opg;

     pfr = expr->pfr;
	  
     if (! (check_ep_in_ip(op, pfr)))
	  return;

     opg = make_op_expr(op, NULL);
     add_op_expr_to_pr(pfr,CALL,opg);
}

void add_op_expr_to_relevant_op(Op_Structure *op, Expression *expr)
{
     Temporal_Ope top;
     Pred_Func_Rec *pfr;
     Op_Expr *opg;

     if (expr == NULL)  {
	  fprintf(stderr,LG_STR("add_op_expr_to_relevant_op: no invocation part.\n",
				"add_op_expr_to_relevant_op: pas de partie d'invocation.\n"));
	  return;
     }

     
     if ((top = expr_temporal_type(expr))) {
	  pfr = EXPR_EXPR1(expr)->pfr;
	  opg = make_op_expr(op, expr);
	  add_op_expr_to_pr(pfr,top,opg);
     } else if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
	  Term *term;
	  sl_loop_through_slist(expr->terms, term, Term *)
	       add_op_expr_to_relevant_op(op, TERM_EXPR(term));
     } else {
	  pfr = expr->pfr;

	  if (! (check_ep_in_ip(op, pfr)))
	       return;

	  opg = make_op_expr(op, expr);
	  add_op_expr_to_pr(pfr,0,opg);
     }
     return;
}

Op_Expr_List consult_fact_relevant_op_internal(Expression *expr)
/* 
 * consult_fact_relevant_op - return a list of pair OP - Expr (from the relevant-op table 
 *                            rk) which are relevant to the Expression expr.
 */
{
     Pred_Func_Rec *pfr;
#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     pfr = expr->pfr;
     
#ifdef  OPRS_PROFILING
     TIME_STOP(&pd,T_RELEVANT);
#endif /* OPRS_PROFILING */

     return (pfr->u.u.pr && pfr->u.u.pr->cached_rop[0] ? pfr->u.u.pr->cached_rop[0] : empty_list);
}

Op_Expr_List consult_goal_relevant_op_internal(Expression *expr)
{
     Pred_Func_Rec *pfr;
     Temporal_Ope temp_oper;
#ifdef OPRS_PROFILING
     PDate pd;

     TIME_START(&pd);
#endif /* OPRS_PROFILING */

     pfr = EXPR_EXPR1(expr)->pfr;


    temp_oper = expr_temporal_type(expr);

#ifdef OPRS_PROFILING
     TIME_STOP(&pd,T_RELEVANT);
#endif /* OPRS_PROFILING */

     return (pfr->u.u.pr && pfr->u.u.pr->cached_rop[temp_oper] ? pfr->u.u.pr->cached_rop[temp_oper] : empty_list);
}

#ifdef IGNORE
/* nzo */
Op_List consult_ipp_goal_relevant_op(Oprs *oprs)
/* This is the preliminary draft from this function: we simply look for the
   (ipp xxx) property for relevance... */
{
    Op_Structure *ops;
    Op_List osl = sl_make_slist();
    PBoolean is_relevant;
    Property *prop;
    Symbol sym_ipp = declare_atom("IPP");

#ifdef OPRS_PROFILING
    PDate pd;

    TIME_START(&pd);
#endif /* OPRS_PROFILING */

    sl_loop_through_slist(oprs->relevant_op->op_list, ops, Op_Structure *){
	is_relevant = FALSE;
	sl_loop_through_slist(ops->properties, prop, Property*){
	    if (sym_ipp == prop->name){
/* 	    if (! (strcmp("IPP", prop->name) && (strcmp("ipp", prop->name)))) { */
		is_relevant = TRUE;
		break;
	    }
	}
	if (is_relevant)
	    sl_add_to_head(osl, ops);
    }
    
/*     free1(sym_ipp); */

#ifdef OPRS_PROFILING
    TIME_STOP(&pd,T_RELEVANT);
#endif /* OPRS_PROFILING */

    return osl;
}

#endif

PBoolean equal_op_name(PString name, Op_Structure *op)
{
     return (name == op->name);
}

PBoolean equal_op_op(Op_Structure *op, Op_Structure *op1)
{
     return (op == op1);
}

PBoolean equal_op_fname(PString name, Op_Structure *op)
{
     return (strcmp(name, op->file_name) == 0);
}

PBoolean equal_opm_op(Op_Structure *op, Op_Expr *opm)
{
     return (op == opm->op);
}

void list_ops(Relevant_Op *rk)
{   
     Op_Structure *op;

     printf(LG_STR("List of OP:\n",
		   "Liste des OP:\n"));
     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  printf ("%s\n", op->name);
     }
     printf(LG_STR("Done.\n",
		   "Fin.\n"));
}

void list_ops_variables(Relevant_Op *rk)
{   
     Op_Structure *op;
     PBoolean save_pv = print_var_name;

     print_var_name = TRUE;

     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  printf ("%s ", op->name); print_lenv(op->list_var);  printf ("\n");
     }
     print_var_name = save_pv;
}

void list_opfs(Relevant_Op *rk)
{   
     PString opf;

     printf(LG_STR("List of OP file:\n",
		   "Liste des fichiers de OP:\n"));
     sl_loop_through_slist(rk->opf_list, opf, PString) {
	  printf ("%s\n", opf);
     }
     printf(LG_STR("Done.\n",
		   "Fin.\n"));
}

void trace_graphic_op(PString name, Relevant_Op *rk, PBoolean state)
{
#ifdef GRAPHIX

     Op_Structure *op;
     PBoolean found_one = FALSE;
     
      sl_loop_through_slist(rk->op_list, op , Op_Structure *) {
	   if (equal_op_name(name,op)) {
		
		if (found_one) printf(LG_STR("WARNING: There is more than one OP named: %s in this OP Database.\n",
					     "ATTENTION: Il y a plus d'un OP nommé: %s dans la base de OP.\n"), name);	  
		op->graphic_traced = state;
		
		printf(LG_STR("OP: %s graphic trace is %s.\n",
			      "OP: %s trace graphique est %s.\n"), name, (state ? "ON" : "OFF"));	  
		
		found_one = TRUE;
	   }
      }
     if (! found_one) printf(LG_STR("trace_graphic_op: no OP named: %s found in the OP Database.\n",
				    "trace_graphic_op: pas de OP nommé: %s dans la base de OP.\n"), name);	  
#endif
}

void trace_graphic_opf(PString file_name, Relevant_Op *rk, PBoolean state)
{
#ifdef GRAPHIX
     Op_Structure *op;
     PBoolean found_one = FALSE;

     sl_loop_through_slist(rk->op_list, op , Op_Structure *) {
	  if (equal_op_fname(file_name,op)) {
	       op->graphic_traced = state;
	  
	       printf(LG_STR("OP: %s graphic trace is %s.\n",
			     "OP: %s trace graphique est %s.\n"), op->name, (state ? "ON" : "OFF"));	  
	       found_one = TRUE;
	  }
     }
     if (! found_one) printf(LG_STR("trace_graphic_opf: no OP from %s found in the OP Database.\n",
				    "trace_graphic_opf: pas de OP de %s dans la base de OP.\n"), file_name);	  
#endif
}

void trace_step_op(PString name, Relevant_Op *rk, PBoolean state)
{
     Op_Structure *op;
     PBoolean found_one = FALSE;
     
      sl_loop_through_slist(rk->op_list, op , Op_Structure *) {
	   if (equal_op_name(name,op)) {
		
		if (found_one) printf(LG_STR("WARNING: There is more than one OP named: %s in this OP Database.\n",
					     "ATTENTION: Il y a plus d'un OP nommé: %s dans la base de OP.\n"), name);	  
		op->step_traced = state;
		
		printf(LG_STR("OP: %s step trace is %s.\n",
			      "OP: %s trace pas à pas est %s.\n"), name, (state ? "ON" : "OFF"));	  
		
		found_one = TRUE;
	   }
      }
     if (! found_one) printf(LG_STR("trace_step_op: no OP named: %s found in the OP Database.\n",
				    "trace_step_op: pas de OP nommé: %s dans la base de OP.\n"), name);	  
}

void trace_step_opf(PString file_name, Relevant_Op *rk, PBoolean state)
{
     Op_Structure *op;
     PBoolean found_one = FALSE;

     sl_loop_through_slist(rk->op_list, op , Op_Structure *) {
	  if (equal_op_fname(file_name,op)) {
	       op->step_traced = state;
	  
	       printf(LG_STR("OP: %s step trace is %s.\n",
			     "OP: %s trace pas à pas est %s.\n"), op->name, (state ? "ON" : "OFF"));	  
	       found_one = TRUE;
	  }
     }
     if (! found_one) printf(LG_STR("trace_step_opf: no OP from %s found in the OP Database.\n",
				    "trace_step_opf: pas de OP de %s dans la base de OP.\n"), file_name);	  
}

void trace_text_op(PString name, Relevant_Op *rk, PBoolean state)
{
     Op_Structure *op;
     PBoolean found_one = FALSE;
     
      sl_loop_through_slist(rk->op_list, op , Op_Structure *) {
	   if (equal_op_name(name,op)) {

	       if (found_one) printf(LG_STR("WARNING: There is more than one OP named: %s in this OP Database.\n",
					    "ATTENTION: Il y a plus d'un OP nommé: %s dans la base de OP.\n"), name);	  
	       op->text_traced = state;
	  
	       printf(LG_STR("OP: %s text trace is %s.\n",
			     "OP: %s trace textuelle est %s.\n"), name, (state ? "ON" : "OFF"));	  

	       found_one = TRUE;
	  }
     }
     if (! found_one) printf(LG_STR("trace_text_op: no OP named: %s found in the OP Database.\n",
				    "trace_text_op: pas de OP nommé: %s dans la base de OP.\n"), name);	  
}

void trace_text_opf(PString file_name, Relevant_Op *rk, PBoolean state)
{
     Op_Structure *op;
     PBoolean found_one = FALSE;

     sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	  if (equal_op_fname(file_name,op)) {
	  
	       op->text_traced = state;
	  
	       printf(LG_STR("OP: %s text trace is %s.\n",
			     "OP: %s trace textuelle est %s.\n"), op->name, (state ? "ON" : "OFF"));	  

	       found_one = TRUE;
	  }
     }
     if (! found_one) printf(LG_STR("trace_text_opf: no OP from %s found in the OP Database.\n",
				    "trace_text_opf: pas de OP de %s dans la base de OP.\n"), file_name);
}

void store_trace_status(PString file_name, Relevant_Op *rk, Slist **text, Slist **graphic, Slist **step)
{
     if (sl_search_slist(rk->opf_list, file_name, equal_string)) {
	  Op_Structure *op;

	  sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
	       if (strcmp(file_name, op->file_name) == 0) {
#ifdef GRAPHIX
		    if (op->graphic_traced)
			 sl_add_to_head(*graphic, op->name);
#endif
		    if (op->step_traced)
			 sl_add_to_head(*step, op->name);
		    if (op->text_traced)
			 sl_add_to_head(*text, op->name);
	       }
	  }
     }     
}

PBoolean equal_id(PString s1, PString s2)
{
     return (s1 == s2);
}

void restore_trace_status(PString file_name, Relevant_Op *rk, Slist *text, Slist *graphic, Slist *step)
{
     if (sl_search_slist(rk->opf_list, file_name, equal_string)) {
	  Op_Structure *op;

	  sl_loop_through_slist(rk->op_list, op, Op_Structure *) {
#ifdef GRAPHIX
	       if (sl_search_slist(graphic, op->name, equal_id))
		    op->graphic_traced = TRUE;
#endif
	       if (sl_search_slist(step, op->name, equal_id))
		    op->step_traced = TRUE;
	       if (sl_search_slist(text, op->name, equal_id))
		    op->text_traced = TRUE;
	  }
     }     
}

void delete_opf_from_rop(PString file_name, Relevant_Op *rk, PBoolean graphic, PBoolean verbose)
{
     PString found;
     Op_Structure *op;
     PBoolean found_one = FALSE;

     if ((found = (PString)sl_search_slist(rk->opf_list, file_name, equal_string))) {
	  sl_delete_slist_node(rk->opf_list, found);
	  FREE(found);

	  while ((op = (Op_Structure *)sl_search_slist(rk->op_list, file_name, equal_op_fname))) {

	       sl_delete_slist_node(rk->op_list,op);

	       delete_op_from_pfr(op, op->invocation);
	       if (op->call) delete_op_call_from_pfr(op, op->call);


	       if (verbose) printf(LG_STR("OP: %s deleted from the OP Database.\n",
					  "OP: %s retiré de la base de OP.\n"), op->name);	  

#ifdef GRAPHIX
	       if (graphic) {
		    clear_specified_op_graphic(global_draw_data, op);
	       }
#endif

	       free_op(op);

	       found_one = TRUE;
	  }
	  if (! found_one) printf(LG_STR("delete_opf_from_rop: no OP from %s found in the OP Database.\n",
					 "delete_opf_from_rop: pas de OP de %s dans la base de OP.\n"), file_name);
     } else 
	  printf(LG_STR("delete_opf_from_rop: no OP file named %s found in this kernel.\n",
			"delete_opf_from_rop: pas de fichier de OP nommé %s dans ce noyau.\n"), file_name);
}

void delete_op_from_rop(Op_Structure *op, Relevant_Op *rk, PBoolean graphic, PBoolean verbose, PBoolean free_it)
{
     if (sl_search_slist(rk->op_list, op, equal_op_op)) {

	  sl_delete_slist_node(rk->op_list,op);
	  
	  delete_op_from_pfr(op, op->invocation);
	  if (op->call) delete_op_call_from_pfr(op, op->call);

#ifdef GRAPHIX
	  if (graphic) {
	       clear_specified_op_graphic(global_draw_data, op);
	  }
#endif
	  if (verbose) printf(LG_STR("OP: [%s] %s deleted from the OP Database.\n",
				     "OP: [%s] %s retiré de la base de OP.\n"), op->file_name, op->name);	  

	  if (free_it) free_op(op);
     } else 
	  fprintf(stderr,LG_STR("ERROR: The OP was not found in the OP Database.\n",
				"ERREUR: Le OP n'a pas été trouvé dans la base de OP.\n"));	  
}

void delete_op_name_from_rop(PString name, Relevant_Op *rk, PBoolean graphic)
{
     Op_Structure *op;
     PBoolean found_one = FALSE;
     

     while ((op = (Op_Structure *)sl_search_slist(rk->op_list, name, equal_op_name))) {

	  if (found_one) fprintf(stderr,LG_STR("WARNING: There is more than one OP named: %s in this OP Database.\n",
					       "ATTENTION: Il y a plus d'un OP nommé: %s dans la base de OP.\n"), name);	  

	  sl_delete_slist_node(rk->op_list,op);

	  delete_op_from_pfr(op, op->invocation);
	  if (op->call) delete_op_call_from_pfr(op, op->call);

#ifdef GRAPHIX
	  if (graphic) {
	       clear_specified_op_graphic(global_draw_data, op);
	  }
#endif
	  free_op(op);

	  printf(LG_STR("OP: %s deleted from the OP Database.\n",
			"OP: %s retiré de la base de OP.\n"), name);	  

	  found_one = TRUE;
     }

     if (! found_one) fprintf(stderr,LG_STR("No OP named: %s found in the OP Database.\n",
					    "Pas de OP nommé: %s dans la base de OP.\n"), name);	  
}

void print_op_from_rop(PString name,Relevant_Op *rk)
{
     Op_Structure *op;

     if ((op = (Op_Structure *)sl_search_slist(rk->op_list, name, equal_op_name))) {
	  print_op(op);
     } else {
	  fprintf(stderr,LG_STR("No OP named: %s found in the OP Database.\n",
				"Pas de OP nommé: %s dans la base de OP.\n"), name);	  
     }
}

Op_List find_rel_ops_expr(Expression *expr)
{
     Op_Expr_List ropl = NULL;
     Op_Expr *opexpr;
     DB_Ope dbop;
     Temporal_Ope top;
     Op_List res = sl_make_slist();

     if (OR_P(expr) || AND_P(expr) || NOT_P(expr)) {
	  Term *term;
			 
	  sl_loop_through_slist(expr->terms, term, Term *) {
	       Op_Structure *op;
	       Op_List res2 = find_rel_ops_expr(TERM_EXPR(term));

	       sl_loop_through_slist(res2, op, Op_Structure *) {
		    if (! (sl_in_slist(res, op)))
			 sl_add_to_tail(res,op);
	       }
	       FREE_SLIST(res2);
	  }
     } else if ((top = expr_temporal_type(expr))) {
	  switch (top) {
	  case TEST:		
	  case CALL:		
	  case ACT_MAINT:
	  case ACHIEVE:
	       ropl = consult_goal_relevant_op_internal(expr);
	       break;
	  case WAIT:
	  case PASS_MAINT:
	       break;
	  default:
	       oprs_perror("find_rel_ops_expr", PE_UNEXPECTED_TEMPORAL_OPER);
	       break;
	  }
     } else {
	  if ((dbop = expr_db_type(expr))) {
	       switch (dbop) { 
	       case DB_CONC:
	       case DB_RET:
	       case DB_COND_CONC:
	       case DB_COND_RET:
		    expr = EXPR_EXPR1(expr);
		    break;
	       }
	  }
	  ropl = consult_fact_relevant_op_internal(expr);
     }

     if (ropl) {
	  sl_loop_through_slist(ropl, opexpr, Op_Expr *) {
	       if (! (sl_in_slist(res, opexpr->op)))
		    sl_add_to_tail(res,opexpr->op);
	  }
     }

     return res;
}
