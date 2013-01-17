/*                               -*- Mode: C -*- 
 * intend_f.h -- 
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

void intend(Intention_Graph *ig, Op_Instance *op_inst, Intention_List after, Intention_List before, int prio);
void intend_op_in_new_intention_before(Intention_Graph *ig, Op_Instance *op_inst, Intention_List before,int prio);
Term *action_intend_all_ops (TermList terms);
Term *action_intend_all_ops_as_root (TermList terms);
Term *action_intend_all_ops_after (TermList terms);

Term *action_intend_op (TermList terms);
Term *action_intend_op_with_priority (TermList terms);
Term *action_intend_op_after_before (TermList terms);
Term *action_intend_op_with_priority_after_before (TermList terms);
Term *action_intend_op_after (TermList terms);
Term *action_intend_op_with_priority_after (TermList terms);

Term *action_intend_all_goals_par(TermList terms);
Term *action_intend_all_goals_par_as_roots(TermList terms);
Term *action_intend_all_goals_par_after (TermList terms);
Term *action_intend_all_goals_par_as_roots_with_priority(TermList terms);
Term *action_intend_all_goals_par_after_roots (TermList terms);

Term *action_intend_goal (TermList terms);
Term *action_intend_goal_with_priority (TermList terms);
Term *action_intend_goal_after_before (TermList terms);
Term *action_intend_goal_with_priority_after_before (TermList terms);

Term *action_intend_op_before_after (TermList terms);
