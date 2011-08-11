/*                               -*- Mode: C -*- 
 * op-structure_f.h -- Declaration of external functions of op-structure.c
 * 
 * $Id$
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

Edge_Type build_edge_type_from_type(PString type);

void init_make_op(PString name, PBoolean graphic);
void print_op(Op_Structure *op);
NodeList build_node_list(NodeList nl, Node *n);
EdgeList build_edge_list(EdgeList el, Edge *e);

Edge *make_edge(void);
Node *make_node(PString name, PBoolean graphic);
Op_Structure *make_op();
Op_Structure *dup_op(Op_Structure *op);
void free_op(Op_Structure *op);
void free_node_from_free_op(Node *node);
void free_edge_from_free_op(Edge *edge);
void free_node_og(OG *og);
void free_edge_og(OG *og);
void free_edge_text_og(OG *og);
void free_knot_og(OG *og);
void free_text_og(OG *og);
void free_op_title(OG *og);

#ifdef GRAPHIX
void sl_free_slist_og_inst(List_OG list_og_inst);
#endif

void build_invocation(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_call(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_context(Op_Structure *op, PString name, ExprList pcnd,
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_setting(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_properties(Op_Structure *op, PString name, PropertyList pl, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_documentation(Op_Structure *op, PString name, PString documentation, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_effects(Op_Structure *op, PString name, ExprList adl,
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);
void build_action(Op_Structure *op, PString name, Action_Field *action, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd);

#ifdef GRAPHIX
OG *
#else
void
#endif
build_and_add_node(Op_Structure *op, PString name, Node_Type nt, PBoolean join, PBoolean split,
			int  x, int y, Draw_Data *dd);

void build_and_add_edge(Op_Structure *op,  PString in,  PString out,
			Edge_Type et, Expression *expr, Slist *knots, int x, int y,
			int pp_width, PBoolean pp_fill, Draw_Data *dd);

void build_and_add_then_else_edge(Op_Structure *op,  PString in,  PString out,
				  Edge_Type et, Draw_Data *dd);

Node *else_node_from_if_node(Node *node);
Node *then_node_from_if_node(Node *node);
void finish_loading_op(Op_Structure *op,  Draw_Data *dd);
Node *find_node_or_create(PString name,PBoolean graphic);
Control_Point *valid_cp(Control_Point *cp);
Logic *valid_logic(Logic *logic);

PString new_node_name(Op_Structure *op);
PString new_edge_name(Op_Structure *op);
void new_then_else_node_name_from_if_name (PString if_name, PString *then_name_p, PString *else_name_p);
void new_if_then_else_node_name(Op_Structure *op, PString *nif, PString *nthen, PString *nelse);
PString new_end_node_name(Op_Structure *op);

PBoolean sort_op(Op_Structure *op1, Op_Structure *op2);
