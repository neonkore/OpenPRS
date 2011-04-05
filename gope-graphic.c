/*                               -*- Mode: C -*-
 * gope-graphic.c --
 *
 * Copyright (c) 2011 LAAS/CNRS
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

#include "config.h"

#include <stdio.h>
#include <math.h>

#include <string.h>


#include <gtk/gtk.h>
#define GTK

#include "xm2gtk.h"


/* #include <X11/Intrinsic.h> */
/* #include <Xm/Xm.h> */

/* #include <Xm/ScrollBar.h> */
/* #include <Xm/DrawingA.h> */

/* #include <X11/X10.h> */

#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "op-structure.h"


#include "gope-graphic.h"
#include "ope-global.h"
#include "ope-edit_f.h"
#include "gope-graphic_f.h"
#include "ope-external_f.h"
#include "op-structure_f.h"

#include "xm2gtk_f.h"

void destroy_og(Draw_Data *dd, CairoGCs *cgcsp, OG *og);
void redraw_all_in_region(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, Region region);

void create_cgcs(CairoGCs *cgcs, GdkDrawable *window)
{
  cgcs->cr_basic = gdk_cairo_create(window);
  cairo_set_source_rgb(cgcs->cr_basic, 0, 0, 0);
  cairo_set_line_width(cgcs->cr_basic, 0.5);
  cairo_select_font_face(cgcs->cr_basic, "Helvetica",
			 CAIRO_FONT_SLANT_NORMAL,
			 CAIRO_FONT_WEIGHT_NORMAL);
  
  cairo_set_font_size(cgcs->cr_basic, 14);

  cgcs->cr_title = gdk_cairo_create(window);
  cairo_set_source_rgb(cgcs->cr_title, 0, 0, 0);
  cairo_set_line_width(cgcs->cr_title, 0.5);

  cairo_select_font_face(cgcs->cr_title, "Courier",
			 CAIRO_FONT_SLANT_NORMAL,
			 CAIRO_FONT_WEIGHT_BOLD);
  
  cairo_set_font_size(cgcs->cr_title, 35);	

  cgcs->cr_edge = gdk_cairo_create(window);
  cairo_set_source_rgb(cgcs->cr_edge, 0, 0, 0);
  cairo_set_line_width(cgcs->cr_edge, 0.5);
  cairo_select_font_face(cgcs->cr_edge, "Helvetica",
			 CAIRO_FONT_SLANT_NORMAL,
			 CAIRO_FONT_WEIGHT_NORMAL);
  
  cairo_set_font_size(cgcs->cr_edge, 12);

  cgcs->cr_node = gdk_cairo_create(window);
  cairo_set_source_rgb(cgcs->cr_node, 0, 0, 0);
  cairo_set_line_width(cgcs->cr_node, 0.5);
  cairo_select_font_face(cgcs->cr_node, "Helvetica",
			 CAIRO_FONT_SLANT_NORMAL,
			 CAIRO_FONT_WEIGHT_NORMAL);
  
  cairo_set_font_size(cgcs->cr_node, 18);

  cgcs->cr_text = gdk_cairo_create(window);
  cairo_set_source_rgb(cgcs->cr_text, 0, 0, 0);
  cairo_set_line_width(cgcs->cr_text, 0.5);
  cairo_select_font_face(cgcs->cr_text, "Helvetica",
			 CAIRO_FONT_SLANT_NORMAL,
			 CAIRO_FONT_WEIGHT_NORMAL);
  
  cairo_set_font_size(cgcs->cr_text, 12);
}


void destroy_cgcs(CairoGCs *cgcs)
{
  cairo_destroy(cgcs->cr_basic);
  cairo_destroy(cgcs->cr_title);
  cairo_destroy(cgcs->cr_edge);
  cairo_destroy(cgcs->cr_node);
  cairo_destroy(cgcs->cr_text);
}


void create_gc(Draw_Data *dd)
{
     /* XGCValues gcv; */
     /* Display *dpy = XtDisplay(dd->canvas); */
     /* Window w = dd->window; */
     /* int mask = GCFont | GCForeground | GCBackground; */
     /* Arg args[10]; */
     /* int n; */

     /* /\* */
     /*  * Create a GC using the colors of the canvas widget. */
     /*  *\/ */

     /* n = 0; */
     /* XtSetArg(args[n], XmNforeground, &gcv.background); n++; */
     /* XtSetArg(args[n], XmNbackground, &gcv.foreground); n++; */
     /* XtGetValues(dd->canvas, args, n); */
     /* gcv.font = dd->font->fid; */

     /* dd->sgc = XCreateGC(dpy, w, mask, &gcv); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNforeground, &gcv.foreground); n++; */
     /* XtSetArg(args[n], XmNbackground, &gcv.background); n++; */
     /* XtGetValues(dd->canvas, args, n); */

     /* dd->gc = XCreateGC(dpy, w, mask, &gcv); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNforeground, &gcv.foreground); n++; */
     /* XtSetArg(args[n], XmNbackground, &gcv.background); n++; */
     /* XtGetValues(dd->canvas, args, n); */

     /* gcv.foreground = gcv.foreground ^ gcv.background; */
     /* gcv.function = GXxor; */

     /* dd->xorgc = XCreateGC(dpy, w, mask | GCFunction, &gcv); */
}

/*
 * Woropround for DrawingArea widget deficiency:
 *
 * If a GraphicsExpose is recieved, redraw the window by calling the
 * DrawingArea widget's XmNexposeCallback list.
 */

void handle_g_exposures(GtkWidget *w, Draw_Data *dd, XEvent *event)
{
#ifdef GTK_IGNORE
     /*
      * This routine will be called for all non-masopble events. Make sure it's
      * the type we want.
      */
     if (event->type == NoExpose) {
	  Copy_Area_Index *cai;

/*
  fprintf(stderr, "NoExpose\n");
  */

	  /* If we get a NoExpose... we do not need to do anything.. just dequeue one cai... */
	  if ((cai = (Copy_Area_Index *)dequeue(dd->copy_area_index_queue)) == NULL) {
	       fprintf(stderr, LG_STR("Empty copy_area_index_queue on NoExpose event.\n",
				      "Empty copy_area_index_queue on NoExpose event.\n"));
	       return;
	  }
	  FREE(cai);
     }

     if (event->type == GraphicsExpose) {

	  Region region;
	  XRectangle rect;
	  Copy_Area_Index *cai;

	  /* If we get a GraphicExpose... we need to redraw the area... and before this
	     we need to shift it of the left and top difference between the moment where
	     the event was created and now... */
	  if ((cai = (Copy_Area_Index *)head_of_queue(dd->copy_area_index_queue)) == NULL) {
	       fprintf(stderr, LG_STR("Empty copy_area_index_queue on GraphicExpose event.\n",
				      "Empty copy_area_index_queue on GraphicExpose event.\n"));
	       return;
	  }
	  
	  rect.x = event->xgraphicsexpose.x - dd->left + cai->left;
	  rect.y = event->xgraphicsexpose.y - dd->top + cai->top;
	  rect.width = event->xgraphicsexpose.width;
	  rect.height = event->xgraphicsexpose.height;

/*
	  fprintf(stderr, "GraphicExpose x %d %d %d y %d %d %d wh %d %d rect %d %d %d %d.\n",event->xgraphicsexpose.x, dd->left, cai->left,
		  event->xgraphicsexpose.y, dd->top, cai->top, 
		  event->xgraphicsexpose.width, event->xgraphicsexpose.height,
		   rect.x,rect.y,rect.width,rect.height);
*/

	  region = XCreateRegion();
	  XUnionRectWithRegion(&rect, region, region);
	  //	  redraw_all_in_region(dd->canvas, dd, region);
	  XDestroyRegion(region);

	  if (event->xgraphicsexpose.count == 0) {
	       dequeue(dd->copy_area_index_queue);
	       FREE(cai);
	  }
     }
#endif
}

void add_expose_region(Draw_Data *dd, Region region)
{
#ifndef GTK
     XOffsetRegion(region, -dd->left, -dd->top);
     if (!dd->expose_region) {
	  dd->expose_region = XCreateRegion();
     }
     XUnionRegion(dd->expose_region, region, dd->expose_region);
#endif
}

void redraw_all_in_pixmap(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, unsigned int width, unsigned int heigh)
{
     if (dd->op) {
	  OG *og;


	  //	  XSetClipMask(XtDisplay(w), cgcsp->cr_basic, None);
	  //XSetClipMask(XtDisplay(w), dd->sgc, None);

	  //XFillRectangle(XtDisplay(dd->canvas), dd->window, dd->sgc, 0, 0, width, heigh);

	  sl_loop_through_slist(dd->op->list_og_edge, og, OG *) {
	       draw_og(w, dd, cgcsp, og);
	  }

	  sl_loop_through_slist(dd->op->list_og_node, og, OG *) {
	       draw_og(w, dd, cgcsp, og);
	  }

	  sl_loop_through_slist(dd->op->list_og_edge_text, og, OG *) {
	       draw_og(w, dd, cgcsp, og);
	  }

	  sl_loop_through_slist(dd->op->list_og_text, og, OG *) {
	       if (og->u.gtext->visible) {
		    draw_og(w, dd, cgcsp, og);
	       }
	  }

	  if (dd->op->op_title && dd->op->op_title->u.gtext->visible) draw_og(w, dd, cgcsp,  dd->op->op_title);

/* 	  XFlush(XtDisplay(dd->canvas)); */
     }
}

void redraw_all_in_region(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, Region region)
{
     if (dd->op) {
	  OG *og;
	  Region inter;

	  /*
	   * Set the clip mask of the GC.
	   */

	  inter = XCreateRegion();

	  if (dd->expose_region) {
	       XUnionRegion(dd->expose_region, region, region);
	       XDestroyRegion(dd->expose_region);
	       dd->expose_region = NULL;
	  }
	  //	  XSetRegion(XtDisplay(w), cgcsp->cr_basic, region);
	  XSetRegion(XtDisplay(w), dd->sgc, region);
	  XSetRegion(XtDisplay(w), dd->xorgc, region);

	  XOffsetRegion(region, dd->left, dd->top);
	  
	  if (dd->op->op_title && dd->op->op_title->u.gtext->visible) {
	       XIntersectRegion(region, dd->op->op_title->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_og(w, dd, cgcsp,  dd->op->op_title);
	  }

	  SAFE_SL_LOOP_THROUGH_SLIST(dd->op->list_og_edge, og, OG *) {
	       XIntersectRegion(region, og->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_og(w, dd, cgcsp, og);
	  }

	  SAFE_SL_LOOP_THROUGH_SLIST(dd->op->list_og_node, og, OG *) {
	       XIntersectRegion(region, og->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_og(w, dd, cgcsp, og);
	  }

	  SAFE_SL_LOOP_THROUGH_SLIST(dd->op->list_og_edge_text, og, OG *) {
	       XIntersectRegion(region, og->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_og(w, dd, cgcsp, og);
	  }

	  SAFE_SL_LOOP_THROUGH_SLIST(dd->op->list_og_text, og, OG *) {
	       if (og->u.gtext->visible) {
		    XIntersectRegion(region, og->region, inter);
		    if (!XEmptyRegion(inter))
			 draw_og(w, dd, cgcsp, og);
	       }
	  }

	  XDestroyRegion(inter);

#ifdef IGNORE_GTK
	  XSetClipMask(XtDisplay(w), cgcsp->cr_basic, None);
	  XSetClipMask(XtDisplay(w), dd->sgc, None);
	  XSetClipMask(XtDisplay(w), dd->xorgc, None);
#endif
     }
}

void handle_exposures(GtkWidget *w, Draw_Data *dd, XmDrawingAreaCallbackStruct *cb)
{
#ifdef GTK_IGNORE
     static Region region = NULL;

     /*
      * Create a region and add the contents of the of the event
      */
     if (!region)
	  region = XCreateRegion();

     XtAddExposureToRegion(cb->event, region);

     if (cb->event->xexpose.count != 0)
	  return;

     redraw_all_in_region(w, dd, cgcsp, region);
     /*
      * Free the region.
      */
     XDestroyRegion(region);
     region = NULL;
#endif
}

void enqueue_index(Draw_Data *dd,int top, int left)
{
     Copy_Area_Index *cai = MAKE_OBJECT(Copy_Area_Index);

     cai->top = top;
     cai->left = left;

     enqueue(dd->copy_area_index_queue, cai);
}

void scroll_bars_moved(Display *disp, Draw_Data *dd, int hsliderpos, int vsliderpos)
{
#ifdef GTK_IGNORE
     int xsrc, redraw_left;
     int ysrc, redraw_top;
     Dimension hdelta, vdelta;
     Region region;
     XRectangle rect;
     PBoolean hmoved = (hsliderpos >= 0);
     PBoolean vmoved = (vsliderpos >= 0);
     

     /*
      * Compute number of pixels the text needs to be moved.
      */
     /*
      * If we are scrolling down, we start at zero and simply move by the
      * delta. The portion that must be redrawn is simply between zero and
      * delta.
      */
     /*
      * If we are scrolling up, we start at the delta and move to zero. The
      * part to redraw lies between the bottom of the window and the bottom -
      * delta.
      */

     if (hmoved) {
	  hdelta = ABS(dd->left - hsliderpos);
	  hdelta = MIN(hdelta, dd->canvas_width);
	  xsrc = redraw_left = 0;
	  if (hsliderpos >= dd->left) {
	       xsrc = hdelta;
	       redraw_left = dd->canvas_width - hdelta;
	  }
	  dd->left = hsliderpos;
     } else
	  xsrc = redraw_left = hdelta = 0;

     if (vmoved) {
	  vdelta = ABS(dd->top - vsliderpos);
	  vdelta = MIN(vdelta, dd->canvas_height);
	  ysrc = redraw_top = 0;
	  if (vsliderpos >= dd->top) {
	       ysrc = vdelta;
	       redraw_top = dd->canvas_height - vdelta;
	  }
	  dd->top = vsliderpos;
     } else
	  ysrc = redraw_top = vdelta =  0;

#ifdef IGNORE_GTK
     XSetClipMask(disp, cgcsp->cr_basic, None);
     XSetClipMask(disp, dd->sgc, None);
     XCopyArea(disp, dd->window,
     	       dd->window, cgcsp->cr_basic,
     	       xsrc, ysrc,
     	       dd->canvas_width - hdelta,
     	       dd->canvas_height - vdelta,
     	       hdelta - xsrc, vdelta - ysrc);
#endif
/*
     fprintf(stderr, "h enqueue %d %d.\n", dd->top, dd->left );
*/
     enqueue_index(dd,dd->top,dd->left);

     /*
      * Clear the remaining area of any old text,
      */
     region = XCreateRegion();

     if (hmoved) {
	  /* XClearArea(disp, dd->window, */
	  /* 	     redraw_left, 0, */
	  /* 	     hdelta, 0, FALSE); */
	  rect.x = redraw_left;
	  rect.y = 0;
	  rect.width = hdelta;
	  rect.height = dd->canvas_height;
	  XUnionRectWithRegion(&rect, region, region);
     }

     if (vmoved) {
	  XClearArea(disp, dd->window,
		     0, redraw_top,
		     0, vdelta, FALSE);

	  rect.x = 0;
	  rect.y = redraw_top;
	  rect.width = dd->canvas_width;
	  rect.height = vdelta;
	  XUnionRectWithRegion(&rect, region, region);
     }

     redraw_all_in_region(dd->canvas, dd, cgcsp, region);
     XDestroyRegion(region);

#endif
}

void hscroll_bar_moved(GtkWidget *w, Draw_Data *dd, XmScrollBarCallbackStruct *call_data)
{
#ifdef GTK_IGNORE
     scroll_bars_moved(XtDisplay(w), dd, call_data->value, -1);
#endif
}

void vscroll_bar_moved(GtkWidget *w, Draw_Data *dd, XmScrollBarCallbackStruct *call_data)
{
#ifdef GTK_IGNORE
     scroll_bars_moved(XtDisplay(w), dd, -1, call_data->value);
#endif
}

void resize(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
#ifdef GTK_IGNORE
     Arg args[10];
     int n;

     /*
      * Determine the new size of the canvas widget.
      */
     n = 0;
     XtSetArg(args[n], XmNheight, &dd->canvas_height); n++;
     XtSetArg(args[n], XmNwidth, &dd->canvas_width); n++;
     XtGetValues(w, args, n);
     /*
      * Reset the scrollbar slider to indictae the relative proportion of text
      * displayed and also the new page size.
      */
     n = 0;
     XtSetArg(args[n], XmNsliderSize, MIN(dd->canvas_height, dd->work_height)); n++;
     XtSetArg(args[n], XmNpageIncrement, dd->canvas_height); n++;
     XtSetValues(dd->vscrollbar, args, n);

     n = 0;
     XtSetArg(args[n], XmNsliderSize, MIN(dd->canvas_width, dd->work_width)); n++;
     XtSetArg(args[n], XmNpageIncrement, dd->canvas_width); n++;
     XtSetValues(dd->hscrollbar, args, n);
#endif
}

OG *then_edge_og_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in then_og_from_if_og...\n",
				 "error in then_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_THEN)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_THEN)))
	  return edge->og;
     else {
	  fprintf(stderr, LG_STR("error in then_og_from_if_og...\n",
				 "error in then_og_from_if_og...\n"));
	  return NULL;
     }
}

OG *else_edge_og_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in else_og_from_if_og...\n",
				 "error in else_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_ELSE)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_ELSE)))
	  return edge->og;
     else {
	  fprintf(stderr, LG_STR("error in else_og_from_if_og...\n",
				 "error in else_og_from_if_og...\n"));
	  return NULL;
     }

}

Edge *then_edge_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in then_og_from_if_og...\n",
				 "error in then_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_THEN)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_THEN)))
	  return edge;
     else {
	  fprintf(stderr, LG_STR("error in then_from_if_og...\n",
				 "error in then_from_if_og...\n"));
	  return NULL;
     }
}

Edge *else_edge_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in else_og_from_if_og...\n",
				 "error in else_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_ELSE)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_ELSE)))
	  return edge;
     else {
	  fprintf(stderr, LG_STR("error in else_from_if_og...\n",
				 "error in else_from_if_og...\n"));
	  return NULL;
     }

}

OG *then_og_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in then_og_from_if_og...\n",
				 "error in then_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_THEN)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_THEN)))
	  return edge->out->og;
     else {
	  fprintf(stderr, LG_STR("error in then_og_from_if_og...\n",
				 "error in then_og_from_if_og...\n"));
	  return NULL;
     }
}

OG *else_og_from_if_og(OG *og)
{
     LogicList out;
     Edge *edge = NULL;

     if (og->type != DT_IF_NODE) {
	  fprintf(stderr, LG_STR("error in else_og_from_if_og...\n",
				 "error in else_og_from_if_og...\n"));
	  return NULL;
     }
     out = og->u.gnode->node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_ELSE)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_ELSE)))
	  return edge->out->og;
     else {
	  fprintf(stderr, LG_STR("error in else_og_from_if_og...\n",
				 "error in else_og_from_if_og...\n"));
	  return NULL;
     }

}

OG *if_og_from_t_or_f_og(OG* og)
{   
     if ((og->type != DT_THEN_NODE) && (og->type != DT_ELSE_NODE)) {
	  fprintf(stderr, LG_STR("error in if_og_from_t_or_f_og...\n",
				 "error in if_og_from_t_or_f_og...\n"));
	  return NULL;
     }

     return ((Edge *)sl_get_slist_head(og->u.gnode->node->in))->in->og;
}     

void destroy_og(Draw_Data *dd, CairoGCs *cgcsp, OG *og)
{
     Edge *edge;
     OG *edge_og;
     OG *knot_og;
     
     if (og->type == DT_EDGE_TEXT) {
	  destroy_og(dd, cgcsp, og->u.gedge_text->edge);
	  return;
     }
     sl_delete_slist_node(dd->op->list_destroyable_og, og);
     sl_delete_slist_node(dd->op->list_movable_og, og);
     sl_delete_slist_node(dd->op->list_editable_og, og);
     erase_og(dd->canvas, dd, cgcsp, og);
     switch (og->type) {
     case DT_IF_NODE:
	  sl_delete_slist_node(dd->op->list_og_node, og);
	  sl_delete_slist_node(dd->op->node_list, og->u.gnode->node);

	  destroy_og(dd, cgcsp, else_og_from_if_og(og));
	  destroy_og(dd, cgcsp, then_og_from_if_og(og));

	  sl_delete_slist_node(dd->op->edge_list, else_edge_from_if_og(og));
	  sl_delete_slist_node(dd->op->edge_list, then_edge_from_if_og(og));

	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       destroy_og(dd, cgcsp, edge->og);
	  }
	  break;
     case DT_NODE:
	  sl_delete_slist_node(dd->op->list_og_node, og);
	  sl_delete_slist_node(dd->op->node_list, og->u.gnode->node);
	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       destroy_og(dd, cgcsp, edge->og);
	  }
	  sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	       destroy_og(dd, cgcsp, edge->og);
	  }
	  break;
     case DT_THEN_NODE:
     case DT_ELSE_NODE:	  
	  sl_delete_slist_node(dd->op->list_og_node, og);
	  sl_delete_slist_node(dd->op->node_list, og->u.gnode->node);
	  sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	       destroy_og(dd, cgcsp, edge->og);
	  }
	  break;
     case DT_KNOT:
	  edge_og =  og->u.gknot->edge;
	  erase_og(dd->canvas, dd, cgcsp, og->u.gknot->edge->u.gedge->text);
	  sl_delete_slist_node(edge_og->u.gedge->list_knot, og);
	  position_edge(og->u.gknot->edge);
	  draw_og(dd->canvas, dd, cgcsp, og->u.gknot->edge);
	  draw_og(dd->canvas, dd, cgcsp, og->u.gknot->edge->u.gedge->text);
	  break;
/*
     case DT_THEN_EDGE:
     case DT_ELSE_EDGE:
	  sl_delete_slist_node(og->u.gedge->edge->in->out, og->u.gedge->edge);
	  sl_delete_slist_node(og->u.gedge->edge->out->in, og->u.gedge->edge);
	  sl_delete_slist_node(dd->op->edge_list, og->u.gedge->edge);

	  sl_delete_slist_node(dd->op->list_og_edge, og);
	  break;
*/
     case DT_EDGE:
	  sl_delete_slist_node(og->u.gedge->edge->in->out, og->u.gedge->edge);
	  sl_delete_slist_node(og->u.gedge->edge->out->in, og->u.gedge->edge);
	  sl_delete_slist_node(dd->op->edge_list, og->u.gedge->edge);

	  sl_delete_slist_node(dd->op->list_og_edge, og);

	  sl_delete_slist_node(dd->op->list_og_edge_text, og->u.gedge->text);
	  sl_delete_slist_node(dd->op->list_destroyable_og, og->u.gedge->text);
	  sl_delete_slist_node(dd->op->list_movable_og, og->u.gedge->text);
	  sl_delete_slist_node(dd->op->list_editable_og, og->u.gedge->text);
	  erase_og(dd->canvas, dd, cgcsp, og->u.gedge->text);

	  sl_loop_through_slist(og->u.gedge->list_knot, knot_og, OG *) {
	       sl_delete_slist_node(dd->op->list_destroyable_og, knot_og);
	       sl_delete_slist_node(dd->op->list_movable_og, knot_og);
	       sl_delete_slist_node(dd->op->list_editable_og, knot_og);
/* 	       erase_og(dd->canvas, dd, cgcsp, knot_og); */
	  }
	  break;
     default:
	  fprintf(stderr, LG_STR("destroy_og: unexpected graphic type..\n",
				 "destroy_og: unexpected graphic type..\n"));
	  break;
     }
}

void erase_og(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, OG *og)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     switch (og->type) {
     case DT_EDGE:
	  erase_edge(w, dd, cgcsp, og->u.gedge);
	  add_expose_region(dd, og->region);
	  /* XSetRegion(XtDisplay(w), cgcsp->cr_basic, og->region); */
	  /* XClipBox(og->region,&rect); */
	  /*
	   * XClearArea(dpy, win, rect.x - dd->left, rect.y - dd->top,
	   * rect.widthdd->op->list_og_nod, rect.height, TRUE);
	   */
	  /*
	   * XClearArea(dpy, win,
	   * MIN3(og->u.gedge->fx1,og->u.gedge->fx2,og->u.gedge->x2) -
	   * dd->left, MIN3(og->u.gedge->fy1,og->u.gedge->fy2,og->u.gedge->y2)
	   * - dd->top, ARR_LENGTH, ARR_LENGTH, TRUE);
	   */
	  /* XSetClipMask(XtDisplay(w), cgcsp->cr_basic, None); */
	  break;
     case DT_IF_NODE:
     case DT_THEN_NODE:
     case DT_ELSE_NODE:
     case DT_NODE:
     case DT_EDGE_TEXT:
     case DT_TEXT:
	  XClearArea(dpy, win,
		     og->x - dd->left, og->y - dd->top,
		     og->width + 1, og->height + 1, TRUE);
	  break;
     case DT_INST:
	  erase_inst(w, dd, cgcsp, og);
	  break;
     case DT_KNOT:
	  erase_og(w, dd, cgcsp, og->u.gknot->edge);
	  break;
/*
     case DT_THEN_EDGE:
     case DT_ELSE_EDGE:
	  break;
*/
     default:
	  fprintf(stderr, LG_STR("erase_og: unknown graphic type..\n",
				 "erase_og: unknown graphic type..\n"));
	  break;
     }
}

void draw_og(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, OG *og)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     int xs, ys;

     xs = og->x - dd->left;
     ys = og->y - dd->top;

/*      XFillRectangle(dpy, win, dd->sgc, */
/* 		    xs, ys, og->width, og->height); */

     if (og->selected && og->type != DT_KNOT)
       XDrawRectangle(dpy, win, cgcsp->cr_basic,
		      xs, ys, og->width-1, og->height-1);

     switch (og->type) {
     case DT_IF_NODE:
     case DT_THEN_NODE:
     case DT_ELSE_NODE:
     case DT_NODE:
       draw_node(w, dd, cgcsp, og->x, og->y, og->width, og->height, og->u.gnode, og->selected);
	  break;
     case DT_EDGE:
	  draw_edge(w, dd, cgcsp, og->u.gedge);
	  break;
     case DT_EDGE_TEXT:
	  draw_edge_text(w, dd, cgcsp, og->x, og->y, og->width, og->u.gedge_text, og->selected);
	  break;
     case DT_TEXT:
	  draw_text(w, dd, cgcsp, og->x, og->y, og->width, og->u.gtext,og->selected);
	  break;
     case DT_INST:
	  draw_inst(w, dd, cgcsp, og->x, og->y, og->width, og->height, og->selected);
	  break;
     case DT_KNOT:
	  draw_og(w, dd, cgcsp, og->u.gknot->edge);
	  break;
/*
     case DT_THEN_EDGE:
     case DT_ELSE_EDGE:
	  break;
*/
     default:
	  fprintf(stderr, LG_STR("unknown graphic type..\n",
				 "unknown graphic type..\n"));
	  break;
     }
}

void draw_node(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, int x, int y, int wi, int h, Gnode *n, PBoolean sel)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;
     Node *node = n->node;

     int xs, ys;

     xs = x - dd->left;
     ys = y - dd->top;

     XDrawRectangle(dpy, win,  cgcsp->cr_basic ,
		    xs + 1, ys + 1,
		    n->swidth + 2, n->sheight + 2);
     
     XmStringDraw(dpy, win, dd->fontlist, n->xmstring, 
		  cgcsp->cr_basic,
		  xs + 4, ys + 3, n->swidth - 2,
		  XmALIGNMENT_BEGINNING,
		  XmSTRING_DIRECTION_L_TO_R,
		  NULL);

     if (node->join) {
	  XDrawLine(dpy, win,  cgcsp->cr_basic,
		    xs + 1, ys + 2,
		    xs + n->swidth + 3 , ys + 2);
	  XDrawLine(dpy, win,  cgcsp->cr_basic,
		    xs + 1, ys + 3,
		    xs + n->swidth + 3 , ys + 3);
     }
     
     if (node->split) {
	  XDrawLine(dpy, win,  cgcsp->cr_basic ,
		    xs + 1, ys  + n->sheight + 2,
		    xs +  n->swidth + 3 , ys + n->sheight + 2);
	  XDrawLine(dpy, win,  cgcsp->cr_basic ,
		    xs + 1, ys + n->sheight + 1,
		    xs + n->swidth + 3 , ys + n->sheight + 1);
     }

}

void draw_text(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, int x, int y, int width, Gtext *et,PBoolean sel)
{
     Gtext_String *gt_str;
     OG *og_inst;

     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     sl_loop_through_slist(et->lgt_string, gt_str, Gtext_String *) { 
	  XmStringDraw(dpy, win, dd->fontlist, gt_str->xmstring, 
		       cgcsp->cr_basic,
		       x - dd->left + gt_str->off_x,
		       y - dd->top + gt_str->off_y,
		       width,
		       XmALIGNMENT_BEGINNING,
		       XmSTRING_DIRECTION_L_TO_R,
		       NULL);
     } 

     if ((et->text_type == TT_BODY)
	 && (et->list_og_inst != NULL)) {
	  sl_loop_through_slist(et->list_og_inst, og_inst, OG *) {
	       if (og_inst->selected) {
		    Display *dpy = XtDisplay(w);
		    Window win = dd->window;
		    
		    int xs, ys;
		    
		    xs = og_inst->x - dd->left;
		    ys = og_inst->y - dd->top;
		    XDrawRectangle(dpy, win, cgcsp->cr_basic,
				   xs, ys, og_inst->width-1, og_inst->height-1);
	       }
	  }
     }
}

void draw_edge_text(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, int x, int y, int width, Gedge_text *et, PBoolean selected)
{
     Gtext_String *gt_str;

     Display *dpy = XtDisplay(w);
     Window win = dd->window;

    sl_loop_through_slist(et->lgt_log_string, gt_str, Gtext_String *) { 
	  XmStringDraw(dpy, win, dd->fontlist, gt_str->xmstring, 
			    cgcsp->cr_basic,
			    x - dd->left + gt_str->off_x,
			    y - dd->top + gt_str->off_y,
			    width,
			    XmALIGNMENT_BEGINNING,
			    XmSTRING_DIRECTION_L_TO_R,
			    NULL);
     } 
}


void draw_inst(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, int x, int y, int wi, int h, PBoolean sel)
{
     return;
}

void draw_clip_box(GtkWidget *w, Draw_Data *dd, OG *og)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     XDrawRectangle(dpy, win, dd->xorgc,
		    og->x - dd->left,
		    og->y - dd->top,
		    og->width - 1, og->height - 1);
}

void draw_sel_box(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, OG *og)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     XDrawRectangle(dpy, win, cgcsp->cr_basic,
		    og->x - dd->left,
		    og->y - dd->top,
		    og->width - 1, og->height - 1);
}

void erase_inst(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, OG *og) 
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

/*
 * to erase og_inst, we do not generate expose event 
 * this function was used previously when we erase this og(s) before 
 * drawing  og_text.
 * It was not used anymore ( the og_text is erased before and includes
 * this region)
 */
     int xs, ys;
     int x = og->x;
     int y =  og->y;

     xs = x - dd->left;
     ys = y - dd->top;

     XClearArea(dpy, win,
		og->x - dd->left, og->y - dd->top,
		og->width + 1, og->height + 1, TRUE);

}

void erase_edge(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, Gedge *e)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;
     int n = 0;
     OG *og;
     XPoint arrows[3];
     XPoint lines[MAX_KNOT];

     lines[n].x = e->x1 - dd->left;
     lines[n].y = e->y1 - dd->top; n++;

     sl_loop_through_slist(e->list_knot, og, OG *) {
	  lines[n].x = og->u.gknot->x - dd->left;
	  lines[n].y = og->u.gknot->y - dd->top; n++;
     }

     lines[n].x = e->x2 - dd->left;
     lines[n].y = e->y2 - dd->top; n++;

     XDrawLines(dpy, win, dd->sgc, lines, n, CoordModeOrigin);

     arrows[0].x = e->x2 - dd->left;
     arrows[0].y = e->y2 - dd->top;
     arrows[1].x = e->fx1 - dd->left;
     arrows[1].y = e->fy1 - dd->top;
     arrows[2].x = e->fx2 - dd->left;
     arrows[2].y = e->fy2 - dd->top;

     XFillPolygon(dpy, win, dd->sgc,
		  arrows, 3, Convex, CoordModeOrigin);

}

void draw_edge(GtkWidget *w, Draw_Data *dd, CairoGCs *cgcsp, Gedge *e)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;
     int n = 0;
     OG *og;
     XPoint arrows[3];
     XPoint lines[MAX_KNOT];

     lines[n].x = e->x1 - dd->left;
     lines[n].y = e->y1 - dd->top; n++;

     sl_loop_through_slist(e->list_knot, og, OG *) {
	  lines[n].x = og->u.gknot->x - dd->left;
	  lines[n].y = og->u.gknot->y - dd->top; n++;
     }

     lines[n].x = e->x2 - dd->left;
     lines[n].y = e->y2 - dd->top; n++;

     XDrawLines(dpy, win, cgcsp->cr_basic, lines, n, CoordModeOrigin);

     arrows[0].x = e->x2 - dd->left;
     arrows[0].y = e->y2 - dd->top;
     arrows[1].x = e->fx1 - dd->left;
     arrows[1].y = e->fy1 - dd->top;
     arrows[2].x = e->fx2 - dd->left;
     arrows[2].y = e->fy2 - dd->top;

     XFillPolygon(dpy, win, cgcsp->cr_basic,
		  arrows, 3, Convex, CoordModeOrigin);

}

void set_canvas_view_rel(Draw_Data *dd, int xv, int yv)
{
#ifdef GTK_IGNORE
     Arg args[1];
     int x, y;

     int xss = MIN(dd->canvas_width, dd->work_width);
     int yss = MIN(dd->canvas_height, dd->work_height);

     XtSetArg(args[0], XmNvalue, &x);
     XtGetValues(dd->hscrollbar, args, 1);

     XtSetArg(args[0], XmNvalue, &y);
     XtGetValues(dd->vscrollbar, args, 1);

     x -= xv;
     y -= yv;

     x = MIN(x, (int)dd->work_width - xss);
     y = MIN(y, (int)dd->work_height - yss);

     x = MAX(x, 0);
     y = MAX(y, 0);

     XmScrollBarSetValues(dd->hscrollbar, x, xss, 10, dd->canvas_width, False);
     XmScrollBarSetValues(dd->vscrollbar, y, yss, 10, dd->canvas_height, False);
     scroll_bars_moved(XtDisplay(dd->vscrollbar),dd,x,y);
#endif
}

void set_canvas_view(Draw_Data *dd, int x, int y)
{
#ifdef GTK_IGNORE

     int xss = MIN(dd->canvas_width, dd->work_width);
     int yss = MIN(dd->canvas_height, dd->work_height);

     x = MIN(x, (int)dd->work_width - xss);
     y = MIN(y, (int)dd->work_height - yss);

     x = MAX(x, 0);
     y = MAX(y, 0);


     XmScrollBarSetValues(dd->hscrollbar, x, xss, 10, dd->canvas_width, False);
     XmScrollBarSetValues(dd->vscrollbar, y, yss, 10, dd->canvas_height, False);
     scroll_bars_moved(XtDisplay(dd->vscrollbar),dd,x,y);
#endif
}

void set_canvas_view_no_redraw(Draw_Data *dd, int x, int y)
{
#ifdef GTK_IGNORE
     int xss = MIN(dd->canvas_width, dd->work_width);
     int yss = MIN(dd->canvas_height, dd->work_height);

     x = MIN(x, (int)dd->work_width - xss);
     y = MIN(y, (int)dd->work_height - yss);

     x = MAX(x, 0);
     y = MAX(y, 0);

     dd->top = y;
     dd->left = x;

     XmScrollBarSetValues(dd->hscrollbar, x, xss, 10, dd->canvas_width, False);
     XmScrollBarSetValues(dd->vscrollbar, y, yss, 10, dd->canvas_height, False);
#endif
}

void display_op_pos(Op_Structure * op, Draw_Data *dd, CairoGCs *cgcsp, int x, int y)
{
     XRectangle rect;
     Region region;

     if (dd->op != op) {
	  current_op = op;
	  dd->op = op;
	  XClearWindow(XtDisplay(dd->canvas), dd->window);
	  set_canvas_view_no_redraw(dd, x, y);

	  region = XCreateRegion();
	  rect.x = 0;
	  rect.y = 0;
	  rect.width = dd->canvas_width;
	  rect.height = dd->canvas_height;
	  XUnionRectWithRegion(&rect, region, region);
	  redraw_all_in_region(dd->canvas, dd, cgcsp, region);	/* We want to
							 * synchronize... */
	  XDestroyRegion(region);
     } else {
	  set_canvas_view(dd, x, y);
     }
     XFlush(XtDisplay(dd->canvas));
}

void display_op_no_dd(Op_Structure * op)
{
  display_op_pos(op, global_draw_data, mainCGCsp, 0, 0);
}

void display_op_edge_internal(Op_Structure * op, Edge *edge, PBoolean selected)
{
     XRectangle rect;
     Region region;
     OG *og = NULL;
     Draw_Data *dd = global_draw_data;

     if (edge->og) {
	  switch (edge->og->type) {
	  case DT_EDGE:
	       og = edge->og->u.gedge->text;
	       og->selected = selected;
	       break;
	  case DT_INST:
	       og = edge->og;
	       og->selected = selected;
	       break;
	  default:
	       break;
	  }
     }

     if (dd->op != op) {
	       
	  current_op = op;
	  dd->op = op;
	  XClearWindow(XtDisplay(dd->canvas), dd->window);
	  rect.x = 0;		/* This will be replaced with previous pos...
				 * later */
	  rect.y = 0;
	  rect.width = dd->canvas_width;
	  rect.height = dd->canvas_height;
	  region = XCreateRegion();
	  XUnionRectWithRegion(&rect, region, region);
#ifdef GTK_IGNORE
	  if (og) {
	       if (XRectInRegion(region, og->x, og->y, og->width, og->height) != RectangleIn) {
		    set_canvas_view_no_redraw(dd, og->x + og->width / 2 - dd->canvas_width / 2,
					      og->y + og->height / 2 - dd->canvas_height / 2);
	       } else {
		    set_canvas_view_no_redraw(dd, 0, 0);
	       }
	  }
#endif
	  redraw_all_in_region(dd->canvas, dd, mainCGCsp, region);	/* We want to
							 * synchronize... */
	  XDestroyRegion(region);

     } else if (og) {
	  region = XCreateRegion();
	  rect.x = dd->left;
	  rect.y = dd->top;
	  rect.width = dd->canvas_width;
	  rect.height = dd->canvas_height;
	  XUnionRectWithRegion(&rect, region, region);
#ifdef GTK_IGNORE
	  if (XRectInRegion(region, og->x, og->y, og->width, og->height) != RectangleIn) {
	       set_canvas_view(dd, og->x + og->width / 2 - dd->canvas_width / 2,
			       og->y + og->height / 2 - dd->canvas_height / 2);
	  }
#endif
	  XDestroyRegion(region);
	  if (! selected) erase_og(dd->canvas, dd, mainCGCsp, og);
	  draw_og(dd->canvas, dd, mainCGCsp, og);
     }

     XFlush(XtDisplay(dd->canvas));
}

void display_op_edge(Op_Structure * op, Edge *edge)
{
     display_op_edge_internal(op, edge, TRUE);
}

void undisplay_op_edge(Op_Structure * op, Edge *edge)
{
     display_op_edge_internal(op, edge, FALSE);
}

PBoolean sort_op(Op_Structure *op1, Op_Structure *op2)
{
     return (strcmp(op1->name,op2->name) < 0);
}

