static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * intend.c -- creation of intentions...
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand.
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

#include "macro.h"
#include "constant.h"
#include "op-instance.h"

#ifndef NO_GRAPHIX
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif

#include "op-structure.h"
#include "oprs.h"

#include "int-graph.h"
#include "fact-goal.h"
#include "intention.h"
#include "intend.h"
#include "lisp-list.h"
#include "oprs-sprint.h"
#include "oprs-error.h"

#include "oprs-type_f.h"
#include "oprs-sprint_f.h"
#include "int-graph_f.h"
#include "intention_f.h"
#include "lisp-list_f.h"
#include "op-instance_f.h"
#include "fact-goal_f.h"
#include "activate_f.h"
#include "conditions_f.h"
#include "oprs-error_f.h"
#include "user-trace_f.h"

void intend_op_in_thread_intention_block(Thread_Intention_Block *tib,Op_Instance *op_inst)
{
     Thread_Intention_Block *new_tib;
     
     if (! (sl_slist_empty(tib->fils))) return; /* Just checking nobody else already working on this goal. */
     new_tib = make_thread_intention_block(op_inst);
     sl_add_to_tail(tib->fils, new_tib);
     if (tib->intention->critical_section == tib) /* it cs is set, it should be tib... */
	  tib->intention->critical_section = new_tib;
     new_tib->pere = tib;
     new_tib->intention = tib->intention;
     add_in_active_tib(new_tib);
     remove_from_active_tib(tib);

     if (debug_trace[INTEND]) {
	  static Sprinter *sp = NULL;
		  
	  if (!sp) sp = make_sprinter(0);
	  else reset_sprinter(sp);
	  
	  SPRINT(sp,32 + strlen(op_inst->op->name),
				    sprintf(f,LG_STR("Intending %s in intention.\n",
						     "Intend %s dans une intention.\n"), op_inst->op->name));
#ifndef NO_GRAPHIX
	  if (tib->intention->trace_dialog) 
	       xpTraceIDialogUpdate(tib->intention->trace_scrl_txt, SPRINTER_STRING(sp));
	  else
#endif
	       printf("%s",SPRINTER_STRING(sp));
     }

     if (user_trace[USER_TRACE] && user_trace[UT_INTEND])
	  user_trace_intend(tib,op_inst);

     if (op_inst->maintain) {
	  /* this is a maintenance thread intention block */
	  tib->maintain = op_inst->maintain;
	  tib->maintain_condition = make_and_install_condition(MAINTAIN_ACTIVATION, NULL, tib, EXPR_EXPR1(tib->maintain), TIB_FRAME(tib));
     }
}

void intend_op_in_new_intention_before(Intention_Graph *ig, Op_Instance *op_inst, Intention_List before, int prio)
{
     insert_intention_in_ig(make_intention(op_inst,ig,prio), ig, empty_list, before);
}

void intend_op_in_new_intention(Intention_Graph *ig, Op_Instance *op_inst, Intention_List after, Intention_List before, int prio)
{
     Intention_List tmp_il;

     if (property_p(op_inst,decision_procedure_sym)) {
	  /* We do a COPY_SLIST because the insertion is going to "change" this very list. */
	  intend_op_in_new_intention_before(ig,	op_inst,tmp_il = COPY_SLIST(ig->list_first),prio); 
	  FREE_SLIST(tmp_il);	/* Free it after. */
     } else insert_intention_in_ig(make_intention(op_inst,ig,prio), ig, after, before);
}

void intend_goal_in_new_intention(Intention_Graph *ig, Goal *goal, Intention_List after,
				  Intention_List before, int prio)
{
     Op_Instance *opi_to_free;

     insert_intention_in_ig(
	  make_intention_for_goal(
	       opi_to_free = make_op_instance_for_intention_from_goal(goal->frame,
								      goal),
	       ig,
	       prio,
	       goal),
	  ig, after, before);
}

void intend(Intention_Graph *ig, Op_Instance *op_inst, Intention_List after, Intention_List before, int prio)
{
     Goal *goal;
     Thread_Intention_Block *tib;

     if (op_inst->intended != FALSE) {  /* this op_instance has already been intended. */
	 fprintf(stderr,LG_STR("WARNING: This OP Instance has already been intended.\n",
			       "ATTENTION: Cette OP Instance a déjà été intendée.\n"));
     	 return;
     }
     if ((goal = op_inst->goal) && (tib = goal->tib)) {
	  if (tib->status == TIBS_ACTIVE) { 
	       /* this thread has neither been suspended 
		  nor removed after posting this goal */

	       if (valid_intention(ig, tib->intention)) {
		    intend_op_in_thread_intention_block(tib,op_inst);

	       } else {
		    fprintf(stderr,LG_STR("WARNING: Cannot intend an op_instance in a dead intention.\n",
					  "ATTENTION: Ne peut pas intendre une  OP Instance dans une intention morte.\n"));
	       }
	  }
     } else {
	  intend_op_in_new_intention(ig,op_inst,after,before,prio);
	  
	  if (debug_trace[INTEND]) {
	       printf(LG_STR("Intending %s in new intention.\n",
			     "Intend %s dans une nouvelle intention.\n"), op_inst->op->name);
	       }
	  
	  if (user_trace[USER_TRACE] && user_trace[UT_INTEND])
	       user_trace_intend(NULL,op_inst);

     }
}

Term *action_intend_op (TermList terms)
{
     Term *t,*res;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != TT_OP_INSTANCE) 
	  oprs_perror("action_intend_op", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
     intend(ig, t->u.opi, after, empty_list, 0);

     FREE_SLIST(after);

     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_with_priority (TermList terms)
{
     Term *t1,*t2,*res;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);

     if (t1->type != TT_OP_INSTANCE) {
	  oprs_perror("action_intend_op_with_priority", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	 return res;
     }
     if (t2->type != INTEGER) {
	  oprs_perror("action_intend_op_with_priority", PE_EXPECTED_INTEGER_TERM_TYPE);
	 return res;
     }

     intend(ig, t1->u.opi, after, empty_list, t2->u.intval);

     FREE_SLIST(after);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_before_after (TermList terms)
{
     Term *t, *res, *before, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List bl, al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_pos(terms,1);
     before =  (Term *)sl_get_slist_pos(terms,2);
     after =  (Term *)sl_get_slist_pos(terms,3);

     if (t->type != TT_OP_INSTANCE) {
	  oprs_perror("action_op_intend_before_after", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	 return res;
     }

     if (before->type != LISP_LIST) {
	  oprs_perror("action_op_intend_before_after: before", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }
     if (after->type != LISP_LIST) {
	  oprs_perror("action_op_intend_before_after: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);
     bl =  make_and_check_c_list_from_l_list_of_intentions(ig, before->u.l_list);

     intend(ig, t->u.opi, al, bl, 0);

     FREE_SLIST(al);
     FREE_SLIST(bl);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_after_before (TermList terms)
{
     Term *t, *res, *before, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List bl, al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_pos(terms,1);
     after =  (Term *)sl_get_slist_pos(terms,2);
     before =  (Term *)sl_get_slist_pos(terms,3);

     if (t->type != TT_OP_INSTANCE) {
	  oprs_perror("action_op_intend_after_before", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	  return res;
     }

     if (after->type != LISP_LIST) {
	  oprs_perror("action_op_intend_after_before: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }

     if (before->type != LISP_LIST) {
	  oprs_perror("action_op_intend_after_before: before", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }


     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);
     bl =  make_and_check_c_list_from_l_list_of_intentions(ig, before->u.l_list);

     intend(ig, t->u.opi, al, bl, 0);

     FREE_SLIST(al);
     FREE_SLIST(bl);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_with_priority_after_before (TermList terms)
{
     Term *t1,*t2,*res, *before, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List bl, al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     after =  (Term *)sl_get_slist_pos(terms,3);
     before =  (Term *)sl_get_slist_pos(terms,4);

     if (t1->type != TT_OP_INSTANCE) {
	  oprs_perror("action_intend_op_with_priority_after_before", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	 return res;
     }

     if (t2->type != INTEGER) {
	  oprs_perror("action_intend_op_with_priority_after_before", PE_EXPECTED_INTEGER_TERM_TYPE);
	 return res;
     }

     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_op_with_priority_after_before: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     if (before->type != LISP_LIST){
	  oprs_perror("action_op_with_priority_intend_before_after: before", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);
     bl =  make_and_check_c_list_from_l_list_of_intentions(ig, before->u.l_list);

     intend(ig, t1->u.opi,
	    al, bl, t2->u.intval);

     FREE_SLIST(bl);
     FREE_SLIST(al);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_after (TermList terms)
{
     Term *t,*res, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     after =  (Term *)sl_get_slist_pos(terms,2);

     if (t->type != TT_OP_INSTANCE) {
		  oprs_perror("action_intend_op_after", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	 return res;
     }

     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_op_after: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);

     intend(ig, t->u.opi, al, empty_list, 0);

     FREE_SLIST(al);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_op_with_priority_after (TermList terms)
{
     Term *t1,*t2,*res, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     after =  (Term *)sl_get_slist_pos(terms,3);

     if (t1->type != TT_OP_INSTANCE) {
	  oprs_perror("action_intend_op_with_priority_after", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	 return res;
     }
     if (t2->type != INTEGER) {
	  oprs_perror("action_intend_op_with_priority_after", PE_EXPECTED_INTEGER_TERM_TYPE);
	 return res;
     }
     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_op_with_priority_after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);

     intend(ig, t1->u.opi,
	    al, empty_list, t2->u.intval);

     FREE_SLIST(al);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_ops_as_root (TermList terms)
{
     Term *t,*res;
     L_List l;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List root = COPY_SLIST(ig->list_first);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) {
	 oprs_perror("action_intend_all_ops_as_root", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     l = t->u.l_list;

     while (l != l_nil){
	  if ((CAR(l))->type != TT_OP_INSTANCE) {
	       oprs_perror("action_intend_all_ops_as_root", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	       return res;
	   } else
	       intend_op_in_new_intention_before(ig, (CAR(l)->u.opi), root, 0);
	  l = CDR(l);
     }
     FREE_SLIST(root);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_ops (TermList terms)
{
     Term *t,*res;
     L_List l;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) {
	 oprs_perror("action_intend_all_ops", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }
     l = t->u.l_list;

     while (l != l_nil) {
	  if ((CAR(l))->type != TT_OP_INSTANCE) {
	       oprs_perror("action_intend_all_ops", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	       return res;
	   } else
	       intend(ig, CAR(l)->u.opi,  after, empty_list, 0);
	  l = CDR(l);
     }

     FREE_SLIST(after);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_ops_after (TermList terms)
{
     Term *t,*res, *after;
     L_List l;
     Intention_List al;
     Intention_Graph *ig = current_oprs->intention_graph;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_head(terms);
     after =  (Term *)sl_get_slist_pos(terms,2);

     if (t->type != LISP_LIST) {
	  oprs_perror("action_intend_all_ops_after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }
     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_all_ops_after: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }
     l = t->u.l_list;
     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);

     while (l != l_nil) {
	  if ((CAR(l))->type != TT_OP_INSTANCE) {
	       oprs_perror("action_intend_all_ops_after", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	       return res;
	  }
	  else
	       intend(ig, CAR(l)->u.opi,  al, empty_list, 0);
	  l = CDR(l);
     }

     FREE_SLIST(al);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_goals_par (TermList terms)
{
     Term *t,*res;
     L_List l;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) {
	 oprs_perror("action_intend_all_goals_par", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     l = t->u.l_list;

     while (l != l_nil){
	 if (CAR(l)->type != TT_GOAL) {
	     oprs_perror("action_intend_all_goals_par", PE_EXPECTED_GOAL_TERM_TYPE);
	     return res;
	 } else
	   intend_goal_in_new_intention(ig,
					copy_goal(CAR(l)->u.goal),
					after, empty_list, 0);
	 l = CDR(l);
     }

     FREE_SLIST(after);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_goals_par_as_roots(TermList terms)
{
     Term *t1, *res;
     L_List l1;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List root = COPY_SLIST(ig->list_first);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     if (t1->type != LISP_LIST) {
	  oprs_perror("action_intend_all_goals_par_as_roots: first", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }
     l1 = t1->u.l_list;

     while (l1 != l_nil){
	  if (CAR(l1)->type != TT_GOAL) {
	       oprs_perror("action_intend_all_goals_par_as_roots", PE_EXPECTED_GOAL_TERM_TYPE);
	       return res;
	   } else 
	     intend_goal_in_new_intention(ig, 
					  copy_goal(CAR(l1)->u.goal),
					  empty_list, root, 0);
	  l1 = CDR(l1);
     }

     FREE_SLIST(root);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_goals_par_after (TermList terms)
{
     Term *t,*res, *after;
     L_List l;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List al;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST){
	  oprs_perror("action_intend_all_goals_par_after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
      }

     l = t->u.l_list;
     after =  (Term *)sl_get_slist_pos(terms,2);
     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_all_goals_par_after: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	  return res;
     }
     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);

     while (l != l_nil){
	  if (CAR(l)->type != TT_GOAL) {
	      oprs_perror("action_intend_all_goals_par_after", PE_EXPECTED_GOAL_TERM_TYPE);
	      return res;
	  } else
	    intend_goal_in_new_intention(ig,
					 copy_goal(CAR(l)->u.goal),
					 al, empty_list, 0);
	  l = CDR(l);
     }

     FREE_SLIST(al);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_goals_par_as_roots_with_priority(TermList terms)
{
     Term *t1, *t2, *res;
     L_List l1, l2;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List root = COPY_SLIST(ig->list_first);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     if (t1->type != LISP_LIST) {
	  oprs_perror("action_intend_all_goals_par_as_roots_with_priority: first", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     if (t2->type != LISP_LIST) {
	  oprs_perror("action_intend_all_goals_par_as_roots_with_priority: second", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     l1 = t1->u.l_list;
     l2 = t2->u.l_list;

     while (l1 != l_nil){
	  if (CAR(l1)->type != TT_GOAL) {
	       oprs_perror("action_intend_all_goals_par_as_roots_with_priority", PE_EXPECTED_GOAL_TERM_TYPE);
	       return res;
	  } else if (!CAR(l2) || (CAR(l2)->type != INTEGER)) {
	       oprs_perror("action_intend_all_goals_par_as_roots_with_priority", PE_EXPECTED_INTEGER_TERM_TYPE);
	       return res;
	  } else 
	       intend_goal_in_new_intention(ig, 
					    copy_goal(CAR(l1)->u.goal),
					    empty_list, 
					    root, 
					    CAR(l2)->u.intval);
	  l1 = CDR(l1);
	  l2 = CDR(l2);
     }

     FREE_SLIST(root);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_all_goals_par_after_roots (TermList terms)
{
     Term *t,*res;
     L_List l;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List root = COPY_SLIST(ig->list_first);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != LISP_LIST) {
	 oprs_perror("action_intend_all_goals_par_after_roots", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     l = t->u.l_list;

     while (l != l_nil){
	  if (CAR(l)->type != TT_GOAL) {
	      oprs_perror("action_intend_all_goals_par_after_roots", PE_EXPECTED_GOAL_TERM_TYPE);
	      return res;
	  } else
	       intend_goal_in_new_intention(ig,
					    copy_goal(CAR(l)->u.goal),
					    root, empty_list, 0);
	  l = CDR(l);
     }

     FREE_SLIST(root);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_goal (TermList terms)
{
     Term *t,*res;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != TT_GOAL) {
	 oprs_perror("action_intend_goal", PE_EXPECTED_GOAL_TERM_TYPE);
	 return res;
     }

     intend_goal_in_new_intention(ig,
				  copy_goal(t->u.goal),
				  after, empty_list, 0);

     FREE_SLIST(after);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_goal_with_priority (TermList terms)
{
     Term *t1,*t2,*res;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *ci = current_intention;
     
     Intention_List after = sl_make_slist();

     sl_add_to_head(after,ci);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);

     if (t1->type != TT_GOAL) {
	 oprs_perror("action_intend_goal_with_priority", PE_EXPECTED_GOAL_TERM_TYPE);
	 return res;
     }

     if (t2->type != INTEGER) {
	 oprs_perror("action_intend_goal_with_priority", PE_EXPECTED_INTEGER_TERM_TYPE);
	 return res;
     }

     intend_goal_in_new_intention(ig,
				  copy_goal(t1->u.goal),
				  after, empty_list, t2->u.intval);
     FREE_SLIST(after);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_goal_after_before (TermList terms)
{
     Term *t, *res, *before, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List bl, al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t = (Term *)sl_get_slist_pos(terms,1);
     after =  (Term *)sl_get_slist_pos(terms,2);
     before =  (Term *)sl_get_slist_pos(terms,3);

    if (t->type != TT_GOAL) {
	  oprs_perror("action_goal_intend_after_before", PE_EXPECTED_GOAL_TERM_TYPE);
	 return res;
     }

     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_goal_after_before: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     if (before->type != LISP_LIST) {
	  oprs_perror("action_intend_goal_after_before: before", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);
     bl =  make_and_check_c_list_from_l_list_of_intentions(ig, before->u.l_list);

     intend_goal_in_new_intention(ig,
				  copy_goal(t->u.goal),
				  al, bl, 0);

     FREE_SLIST(al);
     FREE_SLIST(bl);

     res->u.id = lisp_t_sym;

     return res;
}

Term *action_intend_goal_with_priority_after_before (TermList terms)
{
     Term *t1,*t2,*res, *before, *after;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention_List bl, al;
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms,1);
     t2 = (Term *)sl_get_slist_pos(terms,2);
     after =  (Term *)sl_get_slist_pos(terms,3);
     before =  (Term *)sl_get_slist_pos(terms,4);

     if (t1->type != TT_GOAL) {
	  oprs_perror("action_intend_goal_with_priority_after_before\n", PE_EXPECTED_GOAL_TERM_TYPE);
	 return res;
     }

     if (t2->type != INTEGER) {
	  oprs_perror("action_intend_goal_with_priority\n", PE_EXPECTED_INTEGER_TERM_TYPE);
	 return res;
     }

     if (after->type != LISP_LIST) {
	  oprs_perror("action_intend_goal_with_priority_after_before: after", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     if (before->type != LISP_LIST) {
	  oprs_perror("action_intend_goal_with_priority_after_before: before", PE_EXPECTED_LISP_LIST_TERM_TYPE);
	 return res;
     }

     al =  make_and_check_c_list_from_l_list_of_intentions(ig, after->u.l_list);
     bl =  make_and_check_c_list_from_l_list_of_intentions(ig, before->u.l_list);

     intend_goal_in_new_intention(ig,
				  copy_goal(t1->u.goal),
				  al, bl, t2->u.intval);

     FREE_SLIST(bl);
     FREE_SLIST(al);

     res->u.id = lisp_t_sym;

     return res;
}
