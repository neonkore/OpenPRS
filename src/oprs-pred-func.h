/*                               -*- Mode: C -*- 
 * oprs-pred.h -- 
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


#ifndef INCLUDE_oprs_pred_func
#define INCLUDE_oprs_pred_func

#include "conditions.h"

struct pred_rec {
     Eval_Pred *ep;
     Op_Expr_List cached_rop[PASS_MAINT]; /* 0 is pred alone, 1 !, 2 ? et 3 % */
     Condition_List conditions;	/* The conditions sleeping or preserved mentioning this predicate. */
     signed char ff;
     signed char arity;
     unsigned int cwp BITFIELDS(:1);
     unsigned int be BITFIELDS(:1);
     unsigned int op_pred BITFIELDS(:1);
};

extern Pred_Func_Rec *soak_pred;
extern Pred_Func_Rec *app_ops_fact_pred;
extern Pred_Func_Rec *fact_invoked_ops_pred;
extern Pred_Func_Rec *db_satisf_goal_pred;
extern Pred_Func_Rec *app_ops_goal_pred;
extern Pred_Func_Rec *goal_invoked_ops_pred;
extern Pred_Func_Rec *failed_goal_pred;
extern Pred_Func_Rec *failed_pred;
extern Pred_Func_Rec *request_pred;
extern Pred_Func_Rec *achieved_pred;
extern Pred_Func_Rec *wake_up_intention_pred;
extern Pred_Func_Rec *read_response_id_pred;
extern Pred_Func_Rec *read_response_pred;


extern PBoolean check_predicate;

#endif
