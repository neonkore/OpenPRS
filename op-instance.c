static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * op-instance.c -- Function dealing with op-instance.
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
#include "unification_f.h"
#include "oprs-type_f.h"
#include "lisp-list.h"
#include "oprs-sprint.h"
#include "oprs-error.h"

#include "fact-goal.h"

#include "lisp-list_f.h"
#include "op-structure_f.h"
#include "op-instance_f.h"
#include "op-instance_f-pub.h"
#include "fact-goal_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "oprs-error_f.h"

Op_Instance *make_op_instance(Op_Structure *op, FramePtr frame, List_Envar lenv)
{
     Op_Instance *opi = MAKE_OBJECT(Op_Instance);

     opi->op = dup_op(op);
     opi->goal = (Goal *)NULL;
     opi->fact = (Fact *)NULL;
     opi->frame = frame;
     opi->lenv = lenv;
     opi->intended = FALSE;
     opi->maintain = (Expression *)NULL;
     return opi;
}


Op_Instance *dup_op_instance(Op_Instance *opi)
{
     DUP(opi);
     return opi;
}

void fprint_op_instance(FILE *f,Op_Instance *opi)
{
      fprintf(f,LG_STR("<Op_instance %p>",
		       "<Op_instance %p>"), opi);
}

void sprint_op_instance(Sprinter *sp, Op_Instance *opi)
{
     SPRINT(sp, 14 + MAX_PRINTED_POINTER_SIZE,sprintf(f,LG_STR("<Op_instance %p>",
							       "<Op_instance %p>"), opi));
}

void free_op_instance_not_goal(Op_Instance *opi)
{
     if (! LAST_REF(opi))
	  fprintf(stderr,LG_STR("ERROR: free_op_instance_from_goal: wrong ref count (not 1).\n",
				"ERREUR: free_op_instance_from_goal: nombre de reférence erroné (pas 1).\n"));
     if (opi->lenv != empty_list) /* You do not want to free the empty_list constant... */
	  free_lenv(opi->lenv);
     
     if (opi->frame) free_frame(opi->frame); /* It is NULL for op_instance_from_waiting_goal */

     if (opi->maintain) free_expr(opi->maintain);
     free_op(opi->op);
     FREE(opi);
}

void free_op_instance_for_real(Op_Instance *opi)
{
     if (opi->fact) free_fact(opi->fact);
     if (opi->goal) free_goal(opi->goal);
     free_op_instance_not_goal(opi);
}

void free_op_instance(Op_Instance *opi)
{
    if ((REF(opi) == 2) && (opi->goal) && SAFE_SL_IN_SLIST(opi->goal->echec,opi)) {     
	 FREE(opi);
	 free_goal_from_op_instance(opi->goal);
    } else if (LAST_REF(opi)) {
	 free_op_instance_for_real(opi);
    } else
	 FREE(opi);
}

Op_Instance *make_op_instance_from_fact(Op_Structure *op, FramePtr frame, List_Envar lenv, Fact *fact)
{
     Op_Instance *opi = make_op_instance(op,frame,lenv);

     opi->fact = dup_fact(fact);

     return opi;
}

Op_Instance *make_op_instance_from_goal(Op_Structure *op, FramePtr frame, List_Envar lenv, Goal *goal)
{
     Op_Instance *opi = make_op_instance(op,frame,lenv);

     opi->goal = dup_goal(goal);

     return opi;
}

Op_Instance *make_op_instance_for_maint_goal(Op_Structure *op, FramePtr frame, List_Envar lenv, Goal *goal, Expression *maint_expr)
{
     Op_Instance *opi = make_op_instance(op,frame,lenv);

     opi->goal = dup_goal(goal);
     opi->maintain = dup_expr(maint_expr);

     return opi;
}

Op_Instance *make_op_instance_for_intention_from_goal(FramePtr frame, Goal *goal)
{
     Op_Instance *opi = MAKE_OBJECT(Op_Instance);

     opi->op = dup_op(goal_for_intention_op_ptr);
     opi->goal = dup_goal(goal);
     opi->fact = (Fact *)NULL;
     opi->frame = dup_frame (frame);
     opi->lenv = empty_list;
     opi->intended = FALSE;
     opi->maintain = (Expression *)NULL;

     return opi;
}

Op_Instance *make_op_instance_from_goal_satisfied_in_db(FramePtr frame, Goal *goal)
{
     Op_Instance *opi = MAKE_OBJECT(Op_Instance);

     opi->op = dup_op(goal_satisfied_in_db_op_ptr);
     opi->goal = dup_goal(goal);
     opi->fact = (Fact *)NULL;
     opi->frame = frame;
     opi->lenv = empty_list;
     opi->intended = FALSE;
     opi->maintain = (Expression *)NULL;

     return opi;
}

Op_Instance *make_op_instance_from_waiting_goal(Goal *goal)
{
     Op_Instance *opi = MAKE_OBJECT(Op_Instance);

     opi->op = dup_op(goal_waiting_op_ptr);
     opi->goal = dup_goal(goal);
     opi->fact = (Fact *)NULL;
     opi->frame = NULL;
     opi->lenv = empty_list;
     opi->intended = FALSE;
     opi->maintain = (Expression *)NULL;

     return opi;
}

PBoolean match_property(Symbol name, Property *prop)
{
     return (prop->name == name);
}

Term *property(Op_Instance *opi, PString p_name)
{
     Term *res,*tmp_res;
     Property *prop;
     FramePtr tmp_frame = NULL;	/* Initialized to avoid gcc warning... */


     if ((prop = (Property *)sl_search_slist(opi->op->properties, p_name, match_property)) != NULL) {
	  if (! opi->intended) { /* The frame is thus not installed. */
	       tmp_frame = reinstall_frame(opi->frame);
	  }
	  tmp_res = subst_lenv_in_term(prop->value, opi->lenv, QET_NONE);
	  res = find_binding_ef(tmp_res);
	  free_term(tmp_res);
	  if (! opi->intended) { /* The frame is thus not installed. */
	       desinstall_frame(opi->frame, tmp_frame);
	  }
     } else 
	  res = build_nil();
     
     return res;
}

PBoolean property_p(Op_Instance *opi, PString prop)
{
     PBoolean res;
     Term *term;

     term = property(opi, prop);
     res = ! ((term->type == TT_ATOM)  && (term->u.id == nil_sym));
     free_term(term); /* There was a big bug here... it was free and not free_term. */

     return res;
	  
}

Term *property_of_ef(TermList tl)
{
     Term *t1,*t2;

     t1 = (Term *)sl_get_slist_pos(tl,1);
     t2 = (Term *)sl_get_slist_pos(tl,2);
     if (t1->type != TT_ATOM) {
	  oprs_perror("property_of_ef", PE_EXPECTED_ATOM_TERM_TYPE);
	  return build_nil();
     }
     if (t2->type != TT_OP_INSTANCE) {
	  oprs_perror("property_of_ef", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	  return build_nil();
     }
     return(property(t2->u.opi, t1->u.id));
}

PBoolean property_p_ep(TermList tl)
{
     Term *t1,*t2;

     t1 = (Term *)sl_get_slist_pos(tl,1);
     t2 = (Term *)sl_get_slist_pos(tl,2);
     if (t1->type != TT_ATOM) {
	  oprs_perror("property_of_ep", PE_EXPECTED_ATOM_TERM_TYPE);
	  return FALSE;
     }
     if (t2->type != TT_OP_INSTANCE) {
	  oprs_perror("property_of_ep", PE_EXPECTED_OP_INSTANCE_TERM_TYPE);
	  return FALSE;
     }

     return(property_p(t2->u.opi, t1->u.id));
}

L_List satisfy_prop_l_list_opi(L_List l, PString prop)
{
     L_List res = l_nil;
     Term *t;

     while (l != l_nil) {
	  t = property(CAR(l)->u.opi, prop);
	  if ((t->type != TT_ATOM)  || (t->u.id != nil_sym))
	       res = cons(build_opi_term(dup_op_instance(CAR(l)->u.opi)), res);
	  free_term(t);
	  l = CDR(l);
     }
     
     return res;
}

L_List fact_invoked_ops_l_list_opi(L_List l)
{
     Op_Instance *opi;
     L_List res = l_nil;
     
     while (l != l_nil) {
	  opi = CAR(l)->u.opi;
	  if ((opi->fact) != NULL)
	       res = cons(build_opi_term(dup_op_instance(opi)), res);
	  l = CDR(l);
     }
     
     return res;
}

Op_Structure *op_instance_op(Op_Instance *opi)
{
	return opi->op;
}

Goal *op_instance_goal(Op_Instance *opi)
{
	return opi->goal;
}

Fact *op_instance_fact(Op_Instance *opi)
{
	return opi->fact;
}
