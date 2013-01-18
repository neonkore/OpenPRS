/*                               -*- Mode: C -*- 
 * top-structure_f.h -- 
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

Double_Node *make_double_node(Control_Point *head, Control_Point *tail);
Body *make_body(List_Instruction insts);
Node *make_simple_node(void);
void free_double_node(Double_Node *dn);
Double_Node *merge_double_node(Double_Node *first, Double_Node *second);
Double_Node *build_par_branch(Slist *branch);
Double_Node *build_do_until(Double_Node *insts, Expression *cond);
Double_Node *build_while(Expression *cond, Double_Node *insts);
Double_Node *build_if(Expression *cond, Double_Node *thenb,  Double_Node *elseb, PBoolean elseif);
Double_Node *build_inst(Expression *inst);
Double_Node *build_comment(PString comment);
Double_Node *build_goto_inst(Symbol label);
Double_Node *build_break_inst(void);
Double_Node *build_label_inst(Symbol label);
void build_body(Op_Structure *op, Symbol name, Body *body, int  x, int y, 
		PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void parse_break_list(Double_Node *dn);

Double_Node *add_inst_to_list_inst(Double_Node *list_inst, Double_Node *inst);
void  finish_loading_top(Op_Structure *op,  Draw_Data *dd);
void free_body(Body *body);
void init_make_top(Symbol name, PBoolean graphic);
void print_body(Body *body);

#define DN_HEAD(dn) ((dn)->head)
#define DN_TAIL(dn) ((dn)->tail)
#define DN_INST(dn) ((dn)->u.inst)
#define DN_BODY(dn) ((dn)->u.body)
#define DN_LIST_INST(dn) ((dn)->u.list_inst)
