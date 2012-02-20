/*                               -*- Mode: C -*-
 * ope-edit_f.h --
 *
 * Copyright (c) 1991-2011 Francois Felix Ingrand, LAAS/CNRS
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

void rename_and_redraw_node(Widget w, Draw_Data *dd, CairoGCs *cgcsp, OG *og, PString name);
void canvas_mouse_press(Widget w, Draw_Data *dd, CairoGCs *cgcsp, GdkEventButton *event);
void canvas_mouse_motion(Widget w, Draw_Data *dd, CairoGCs *cgcsp, GdkEventMotion *event);
void canvas_mouse_release(Widget w, Draw_Data *dd, CairoGCs *cgcsp, GdkEventButton *event);
OG *create_text(Widget w, int x, int y, Draw_Data *dd, CairoGCs *cgcsp, Text_Type tt, PString test, int text_width, PBoolean fill_lines);
void create_edge(Widget w, Draw_Data *dd, CairoGCs *cgcsp, OG *og1, OG *og2, char *t1, int width, PBoolean fill_lines, Draw_Type gtype, Edge_Type type, List_Knot list_knot, Expression *expr);
void edit_og(Widget w, Draw_Data *dd, CairoGCs *cgcsp, OG *og, char *string);
void set_editable_og_width(OG *og, int width);
void set_editable_og_fill_lines(OG *og, PBoolean fill_lines);
void make_start_node(Widget w, Draw_Data *dd, CairoGCs *cgcsp);
void set_draw_mode(Draw_Data *dd, Draw_Mode mode);
void set_draw_mode_from_menu(Draw_Data *dd, CairoGCs *cgcsp, Draw_Mode mode);
void rename_and_redraw_if_node(Widget w, Draw_Data *dd, CairoGCs *cgcsp, OG *og, PString if_name, PString then_name, PString else_name);
