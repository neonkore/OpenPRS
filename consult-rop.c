static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * relevant_op.c -- Gestion de la table des procedures "relevants"
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

/*
 * Ce code semble marcher.... Il me faut attendre que le soak module soit ecrit pour le verifier...
 *
 */

#include "config.h"
#include "constant.h"
#include "macro.h"
#include "oprs-type-pub.h"


#ifdef OPRS_PROFILING
#include "user-external.h"
#endif

#include "oprs-profiling.h"

#ifndef NO_GRAPHIX
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif

#include "op-structure.h"
#include "relevant-op.h"
#include "relevant-op_f.h"
#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs.h"
#include "database.h"

void parser_consult_expr_relevant_op(Expression *expr, Relevant_Op *rk)
{
     if (expr_temporal_type(expr) || expr_db_type(expr)) 
	  parser_consult_goal_relevant_op(expr, rk);
     else 
	  parser_consult_fact_relevant_op(expr, rk); 
}

void parser_consult_fact_relevant_op(Expression *expr, Relevant_Op *rk)
{
     consult_fact_relevant_op(expr, rk, TRUE);
}

Op_Expr_List consult_fact_relevant_op(Expression *expr, Relevant_Op *rk, PBoolean trace_forced)
/* 
 * consult_fact_relevant_op - return a list of pair OP - Mexpr (from the relevant-op table 
 *                            rk) which are relevant to the Gexpression gexpr.
 */
{
     Op_Expr_List kml;

     kml = consult_fact_relevant_op_internal(expr);

     if ((debug_trace[RELEVANT_OP]) ||trace_forced)  {
	  Op_Expr * km;
	  printf(LG_STR("The relevant OPs for the fact: ",
			"Les OPs relevant pour le fait: "));
	  print_expr(expr);
	  printf(LG_STR(" are:\n",
			" sont:\n"));
	  if (sl_slist_empty(kml)) printf("NULL\n");
	  sl_loop_through_slist(kml, km, Op_Expr *) {
	       printf("%s ",km->op->name);
	       printf("\n");
	  }
	  printf("\n");
     }
     return kml;
}

void parser_consult_goal_relevant_op(Expression *expr, Relevant_Op *rk)
{
     consult_goal_relevant_op(expr, rk, TRUE);
}

Op_Expr_List consult_goal_relevant_op(Expression *expr, Relevant_Op *rk, PBoolean trace_forced)
{
     Op_Expr_List kml;
    
     kml = consult_goal_relevant_op_internal(expr);

     if  ((debug_trace[RELEVANT_OP]) || trace_forced) {
	  Op_Expr * km;
	  printf(LG_STR("The relevant OPs for the goal: ",
			"Les OPs relevant pour le but: "));
	  print_expr(expr);
	  printf(LG_STR(" are:\n",
			" sont:\n"));
	  if (sl_slist_empty(kml)) printf("NULL\n");
	  sl_loop_through_slist(kml, km, Op_Expr *) {
	       printf("%s ",km->op->name);
	       printf("\n");
	  }
	  printf("\n");
     }
     return kml;
}
