/*                               -*- Mode: C -*- 
 * xoprs-intention_f.h -- 
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

void idd_create_scrollbars(Widget parent, Int_Draw_Data *idd);
void idd_create_gc(Int_Draw_Data *idd);
void idd_handle_g_exposures(Widget w, Int_Draw_Data *idd, XEvent *event);
void idd_redraw_all_in_region(Widget w, Int_Draw_Data *idd, Region region);
void idd_handle_exposures(Widget w, Int_Draw_Data *idd, XmDrawingAreaCallbackStruct *cb);
void idd_hscroll_bar_moved(Widget w, Int_Draw_Data *idd, XmScrollBarCallbackStruct *call_data);
void idd_vscroll_bar_moved(Widget w, Int_Draw_Data *idd, XmScrollBarCallbackStruct *call_data);
void idd_resize(Widget w, Int_Draw_Data *idd, XtPointer call_data);
void idd_canvas_mouse_motion(Widget w, Int_Draw_Data *idd, XEvent *event);
void idd_canvas_mouse_release(Widget w, Int_Draw_Data *idd, XEvent *event);
void idd_canvas_mouse_press(Widget w, Int_Draw_Data *idd, XEvent *event);
void set_int_canvas_view_rel(Int_Draw_Data *idd, int xv, int yv);
void draw_intention_graph(Int_Draw_Data *idd);
IOG *create_ginode(Int_Draw_Data *idd, Intention *intention);
IOG *create_giedge(Int_Draw_Data *idd, Intention *in, Intention *out);
IOG *position_ginode(IOG *iog, int x, int y);
IOG *update_ginode_text(Int_Draw_Data *idd, IOG *iog);
void destroy_ginode(Int_Draw_Data *idd, IOG *iog);
void destroy_giedge(Int_Draw_Data *idd, IOG *iog);
void position_giedge(IOG *iog);
void draw_ginode(Int_Draw_Data *idd, IOG *iog);
void erase_ginode(Int_Draw_Data *idd, IOG *iog);
void erase_giedge(Int_Draw_Data *idd, IOG *iog);
void draw_giedge(Int_Draw_Data *idd, IOG *iog);
void touch_intention_ginode(Intention *intention);
