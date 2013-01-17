/*                               -*- Mode: C -*-
 * ope-graphic_f.h --
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

void hscroll_bar_moved(Widget w, Draw_Data *dd, XmScrollBarCallbackStruct *call_data);
void vscroll_bar_moved(Widget w, Draw_Data *dd, XmScrollBarCallbackStruct *call_data);
void create_scrollbars(Widget parent, Draw_Data *dd);
void create_gc(Draw_Data *dd);
void handle_exposures(Widget w, Draw_Data *dd, XmDrawingAreaCallbackStruct *cb);
void handle_g_exposures(Widget w, Draw_Data *dd, XEvent *event);
void scroll_bar_moved(Widget w, Draw_Data *dd, XmScrollBarCallbackStruct *call_data);
void resize(Widget w, Draw_Data *dd, XtPointer call_data);
void draw_inst(Widget w, Draw_Data *dd, int x, int y, int wi, int h, PBoolean sel);
void draw_node(Widget w, Draw_Data *dd, int x, int y, int wi, int h, Gnode *n, PBoolean sel);
void draw_edge(Widget w, Draw_Data *, Gedge *e);
void erase_edge(Widget w, Draw_Data *dd, Gedge *e);
void draw_edge_text(Widget w, Draw_Data *dd, int x, int y, int width, Gedge_text *et, PBoolean selected);
void draw_text(Widget w, Draw_Data *dd, int x, int y, int width, Gtext *et, PBoolean selected);
void erase_inst(Widget w, Draw_Data *dd, OG *og);
void erase_og(Widget w, Draw_Data *dd, OG *og);
void draw_og(Widget w, Draw_Data *dd, OG *og);
void set_canvas_view(Draw_Data *dd, int x, int y);
void set_canvas_view_rel(Draw_Data *dd, int xv, int yv);
void oprs_canvas_mouse_motion(Widget w, Draw_Data *dd, XEvent *event);
void oprs_canvas_mouse_release(Widget w, Draw_Data *dd, XEvent *event);
void oprs_canvas_mouse_press(Widget w, Draw_Data *dd, XEvent *event);
PBoolean sort_op(Op_Structure *op1, Op_Structure *op2);
OG *then_edge_og_from_if_og(OG *og);
OG *else_edge_og_from_if_og(OG *og);
OG *then_og_from_if_og(OG *og);
OG *else_og_from_if_og(OG *og);
OG *if_og_from_t_or_f_og(OG* og);

void display_op_pos(Op_Structure * op, Draw_Data *dd, int x, int y);
void display_op_no_dd(Op_Structure * op);
void display_op_edge(Op_Structure * op, Edge *edge);
void undisplay_op_edge(Op_Structure * op, Edge *edge);
