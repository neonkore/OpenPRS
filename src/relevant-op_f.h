/*                               -*- Mode: C -*- 
 * relevant-op_f.h -- Declaration des fonctions externes a relevant-op.c
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2003 Francois Felix Ingrand.
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

Relevant_Op *make_relevant_op(void);
Op_Expr_List consult_fact_relevant_op(Expression *expr, Relevant_Op *rk, PBoolean trace_forced);
Op_Expr_List consult_goal_relevant_op(Expression *expr, Relevant_Op *rk, PBoolean trace_forced);
Op_List consult_ipp_goal_relevant_op(Oprs *); /* nzo */
Op_Expr_List consult_fact_relevant_op_internal(Expression *expr);
Op_Expr_List consult_goal_relevant_op_internal(Expression *expr);
void add_op_to_relevant_op_internal(Op_Structure *op, Relevant_Op *rk);
void add_op_expr_to_relevant_op(Op_Structure *op, Expression *expr);
void clear_relevant_op(Relevant_Op *rk);
void parser_consult_goal_relevant_op(Expression *gtexpr, Relevant_Op *rk);
void parser_consult_fact_relevant_op(Expression *gexpr, Relevant_Op *rk);
void stat_relevant_op(Relevant_Op *rk);
void delete_opf_from_rop(PString file_name, Relevant_Op *rk, PBoolean graphic, PBoolean verbose);
void delete_op_from_rop(Op_Structure *op,Relevant_Op *rk, PBoolean graphic, PBoolean verbose, PBoolean free_it);
void delete_op_name_from_rop(PString name,Relevant_Op *rk, PBoolean graphic);
void list_ops(Relevant_Op *rk);
void list_ops_variables(Relevant_Op *rk);
void list_opfs(Relevant_Op *rk);
void print_op_from_rop(PString name,Relevant_Op *rk);

void trace_graphic_op(PString name, Relevant_Op *rk, PBoolean state);
void trace_text_op(PString name, Relevant_Op *rk, PBoolean state);
void trace_graphic_opf(PString name, Relevant_Op *rk, PBoolean state);
void trace_text_opf(PString name, Relevant_Op *rk, PBoolean state);
void trace_step_op(PString name, Relevant_Op *rk, PBoolean state);
void trace_step_opf(PString file_name, Relevant_Op *rk, PBoolean state);

void add_op_file_name(PString current_graph_file_name, Relevant_Op *rk);
void change_op_file_name(PString old_file_name, PString new_file_name, Relevant_Op *rk);

void store_trace_status(PString file_name, Relevant_Op *rk, Slist **text, Slist **graphic, Slist **step);
void restore_trace_status(PString file_name, Relevant_Op *rk, Slist *text, Slist *graphic, Slist *step);

Op_List find_rel_ops_expr(Expression *expr);

void free_relevant_op(Relevant_Op *rk);

void dump_op_database_from_parser(Relevant_Op *rk, PString file_name);
void load_op_database_from_parser(Relevant_Op *rk, PString file_name);

PBoolean equal_op_fname(PString name, Op_Structure *op);
