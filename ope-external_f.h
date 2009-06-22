/*                               -*- Mode: C -*-
 * ope-external_f.h --
 *
 * $Id$
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

void clear_op_graphic(Draw_Data *dd);
void clear_specified_op_graphic(Draw_Data *dd, Op_Structure *op);
XmString ope_string_to_xmstring(XmRenderTable rt, char *string, char *cs, Text_Type tt, Dimension *w, Dimension * h);
XmString xs_str_array_to_xmstr_cs(char *string_array[], int n, char *cs);
OG *make_op_title(Draw_Data *dd, char *name);
void free_lgt_string(List_Gtext_String lge_str);
List_Gtext_String ope_string_to_lgt_string(XmRenderTable rt, char *string, char *cs, Text_Type tt, Dimension * w, Dimension * h);
void report_syntax_error(PString message);

void build_edge_graphic(Edge *edge, Expression *gtexpr);

OG *make_cp_graphic(PString name, Node *node);
OG *make_og_edge(Draw_Data *dd, Op_Structure *op,  Edge *edge, Node *in, Node *out,  Slist *knots, int x, int y, int pp_width, PBoolean pp_fill);
OG *make_og_text_field(Draw_Data *dd, Op_Structure *op, Field_Type ft, Text_Type tt, int x, int y, PBoolean visible, int string_width, PBoolean pp_fill, int default_width);
void update_list_og_inst(Draw_Data *dd, Op_Structure *op, OG *og_body);
OG *make_inst_graphic(Instruction *inst, Edge *edge);
void update_canvas_size(Draw_Data *dd, int x, int y);
void position_then_else(OG *og, int x, int y);
void position_edge(OG *og_edge);
