/*                               -*- Mode: C -*- 
 * goprs-intention.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand, LAAS/CNRS.
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


#include <gtk/gtk.h>

#include "xm2gtk.h"

#include <math.h>

#include "macro.h"
#include "oprs-type.h"
#include "op-structure.h"
#include "goprs-main.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"

#include "gope-graphic.h"
#include "goprs-intention.h"
#include "goprs-intention_f.h"

#include "intention.h"
#include "op-instance.h"
#include "op-structure.h"
#include "fact-goal.h"
#include "int-graph.h"
#include "int-graph_f.h"

#include "ope-external_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs-pprint_f.h"
#include "oprs-type_f-pub.h"

#include "xm2gtk_f.h"

void touch_intention_ginode(Intention *intention)
{
}

void destroy_ginode(Int_Draw_Data *idd, IOG *iog)
{
}

IOG *create_ginode(Int_Draw_Data *idd, Intention *intention)
{
}

void draw_intention_graph(Int_Draw_Data *idd)
{
}

IOG *create_giedge(Int_Draw_Data *idd, Intention *in, Intention *out)
{
}

#ifdef IGNORE
void idd_create_scrollbars(Widget parent, Int_Draw_Data *idd)
{
     Arg args[10];
     int n = 0;

     n = 0;
     XtSetArg(args[n], XmNminimum, 0); n++;
     XtSetArg(args[n], XmNincrement, 10); n++;
     XtSetArg(args[n], XmNmaximum, idd->work_height); n++;
/* idd->canvas_height and width are 0 at this point (because the canvas exist but has its size has 
   not been choosen yet... Anyway, the first resize (when its size will be computed) will fix all this. */
/*     XtSetArg(args[n], XmNsliderSize, MIN(idd->canvas_height, idd->work_height)); n++;
     XtSetArg(args[n], XmNpageIncrement, idd->canvas_height); n++; */
     idd->vscrollbar = XtCreateManagedWidget("idd_vscrollbar", xmScrollBarWidgetClass, parent, args, n);
     XtAddCallback(idd->vscrollbar, XmNvalueChangedCallback, (XtCallbackProc)idd_vscroll_bar_moved, idd);
     XtAddCallback(idd->vscrollbar, XmNdragCallback, (XtCallbackProc)idd_vscroll_bar_moved, idd);
     XtManageChild(idd->vscrollbar);

     n = 0;
     XtSetArg(args[n], XmNminimum, 0); n++;
     XtSetArg(args[n], XmNincrement, 10); n++;
     XtSetArg(args[n], XmNmaximum, idd->work_width); n++;
/*     XtSetArg(args[n], XmNsliderSize, MIN(idd->canvas_width, idd->work_width)); n++;
     XtSetArg(args[n], XmNpageIncrement, idd->canvas_width); n++; */
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     idd->hscrollbar = XtCreateManagedWidget("idd_hscrollbar", xmScrollBarWidgetClass, parent, args, n);
     XtAddCallback(idd->hscrollbar, XmNvalueChangedCallback, (XtCallbackProc)idd_hscroll_bar_moved, idd);
     XtAddCallback(idd->hscrollbar, XmNdragCallback, (XtCallbackProc)idd_hscroll_bar_moved, idd);
     XtManageChild(idd->hscrollbar);
}

void idd_create_gc(Int_Draw_Data *idd)
{
     XGCValues gcv;
     Display *dpy = XtDisplay(idd->canvas);
     Window w = XtWindow(idd->canvas);
     int mask = GCFont | GCForeground | GCBackground;
     Arg args[10];
     int n;

     /*
      * Create a GC using the colors of the canvas widget.
      */

     gcv.font = idd->font->fid;

     n = 0;
     XtSetArg(args[n], XmNforeground, &gcv.background); n++;
     XtSetArg(args[n], XmNbackground, &gcv.foreground); n++;
     XtGetValues(idd->canvas, args, n);

     idd->sgc = XCreateGC(dpy, w, mask, &gcv);

     n = 0;
     XtSetArg(args[n], XmNforeground, &gcv.foreground); n++;
     XtSetArg(args[n], XmNbackground, &gcv.background); n++;
     XtGetValues(idd->canvas, args, n);

     idd->gc = XCreateGC(dpy, w, mask, &gcv);
}

/*
 * Woropround for DrawingArea widget deficiency:
 *
 * If a GraphicsExpose is recieved, redraw the window by calling the
 * DrawingArea widget's XmNexposeCallback list.
 */

void idd_handle_g_exposures(Widget w, Int_Draw_Data *idd, XEvent *event)
{
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
	  if ((cai = (Copy_Area_Index *)dequeue(idd->copy_area_index_queue)) == NULL) {
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
	  if ((cai = (Copy_Area_Index *)head_of_queue(idd->copy_area_index_queue)) == NULL) {
	       fprintf(stderr, LG_STR("Empty copy_area_index_queue on GraphicExpose event.\n",
				      "Empty copy_area_index_queue on GraphicExpose event.\n"));
	       return;
	  }
	  
	  rect.x = event->xgraphicsexpose.x - idd->left + cai->left;
	  rect.y = event->xgraphicsexpose.y - idd->top + cai->top;
	  rect.width = event->xgraphicsexpose.width;
	  rect.height = event->xgraphicsexpose.height;

	  /*
	    fprintf(stderr, "GraphicExpose x %d %d %d y %d %d %d wh %d %d rect %d %d %d %d.\n",event->xgraphicsexpose.x, idd->left, cai->left,
	    event->xgraphicsexpose.y, idd->top, cai->top, 
	    event->xgraphicsexpose.width, event->xgraphicsexpose.height,
	    rect.x,rect.y,rect.width,rect.height);
	    */

	  region = XCreateRegion();
	  XUnionRectWithRegion(&rect, region, region);
	  idd_redraw_all_in_region(idd->canvas, idd, region);
	  XDestroyRegion(region);


	  if (event->xgraphicsexpose.count == 0) {
	       dequeue(idd->copy_area_index_queue);
	       FREE(cai);
	  }
     }
}

void idd_add_expose_region(Int_Draw_Data *idd, Region region)
{
     XOffsetRegion(region, -idd->left, -idd->top);
     if (!idd->expose_region) {
	  idd->expose_region = XCreateRegion();
     }
     XUnionRegion(idd->expose_region, region, idd->expose_region);
}

void idd_redraw_all_in_region(Widget w, Int_Draw_Data *idd, Region region)
{
     IOG *iog;
     Region inter = XCreateRegion();

     /*
      * Set the clip mask of the GC.
      */
     if (debug_trace[GRAPHIC_INTEND]) {
     
	  if (idd->expose_region) {
	       XUnionRegion(idd->expose_region, region, region);
	       XDestroyRegion(idd->expose_region);
	       idd->expose_region = NULL;
	  }

	  XSetRegion(XtDisplay(w), idd->gc, region);
	  XOffsetRegion(region, idd->left, idd->top);

	  sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) {
	       XIntersectRegion(region, iog->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_ginode(idd, iog);
	  }

	  sl_loop_through_slist(idd->ig->list_iedge, iog, IOG *) {
	       XIntersectRegion(region, iog->region, inter);
	       if (!XEmptyRegion(inter))
		    draw_giedge(idd, iog);
	  }
	  XSetClipMask(XtDisplay(w), idd->gc, None);
     }

    XDestroyRegion(inter);


}

void idd_handle_exposures(Widget w, Int_Draw_Data *idd, XmDrawingAreaCallbackStruct *cb)
{
     static Region region = NULL;

     /*
      * Create a region and add the contents of the of the event
      */
     if (!region)
	  region = XCreateRegion();

     XtAddExposureToRegion(cb->event, region);

     if (cb->event->xexpose.count != 0)
	  return;

     idd_redraw_all_in_region(w, idd, region);
     /*
      * Free the region.
      */
     XDestroyRegion(region);
     region = NULL;
}

void idd_enqueue_index(Int_Draw_Data *idd,int top, int left)
{
     Copy_Area_Index *cai = MAKE_OBJECT(Copy_Area_Index);

     cai->top = top;
     cai->left = left;

     enqueue(idd->copy_area_index_queue, cai);
}

void idd_scroll_bars_moved(Display *disp, Int_Draw_Data *idd, int hsliderpos, int vsliderpos)
{
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
	  hdelta = ABS(idd->left - hsliderpos);
	  hdelta = MIN(hdelta, idd->canvas_width);
	  xsrc = redraw_left = 0;
	  if (hsliderpos >= idd->left) {
	       xsrc = hdelta;
	       redraw_left = idd->canvas_width - hdelta;
	  }
	  idd->left = hsliderpos;
     } else
	  xsrc = redraw_left = hdelta = 0;

     if (vmoved) {
	  vdelta = ABS(idd->top - vsliderpos);
	  vdelta = MIN(vdelta, idd->canvas_height);
	  ysrc = redraw_top = 0;
	  if (vsliderpos >= idd->top) {
	       ysrc = vdelta;
	       redraw_top = idd->canvas_height - vdelta;
	  }
	  idd->top = vsliderpos;
     } else
	  ysrc = redraw_top = vdelta =  0;

     XSetClipMask(disp, idd->gc, None);
     XCopyArea(disp, XtWindow(idd->canvas),
	       XtWindow(idd->canvas), idd->gc,
	       xsrc, ysrc,
	       idd->canvas_width - hdelta,
	       idd->canvas_height - vdelta,
	       hdelta - xsrc, vdelta - ysrc);

     idd_enqueue_index(idd,idd->top,idd->left);

     /*
      * Clear the remaining area of any old text,
      */
     region = XCreateRegion();

     if (hmoved) {
	  XClearArea(disp, XtWindow(idd->canvas),
		     redraw_left, 0,
		     hdelta, 0, FALSE);
	  rect.x = redraw_left;
	  rect.y = 0;
	  rect.width = hdelta;
	  rect.height = idd->canvas_height;
	  XUnionRectWithRegion(&rect, region, region);
     }

     if (vmoved) {
	  XClearArea(disp, XtWindow(idd->canvas),
		     0, redraw_top,
		     0, vdelta, FALSE);

	  rect.x = 0;
	  rect.y = redraw_top;
	  rect.width = idd->canvas_width;
	  rect.height = vdelta;
	  XUnionRectWithRegion(&rect, region, region);
     }

     idd_redraw_all_in_region(idd->canvas, idd, region);
     XDestroyRegion(region);

}

void idd_hscroll_bar_moved(Widget w, Int_Draw_Data *idd, XmScrollBarCallbackStruct *call_data)
{
     idd_scroll_bars_moved(XtDisplay(w), idd, call_data->value, -1);
}

void idd_vscroll_bar_moved(Widget w, Int_Draw_Data *idd, XmScrollBarCallbackStruct *call_data)
{
     idd_scroll_bars_moved(XtDisplay(w), idd, -1, call_data->value);
}

void idd_resize(Widget w, Int_Draw_Data *idd, XtPointer call_data)
{
     Arg args[10];
     int n;

     /*
      * Determine the new size of the canvas widget.
      */
     n = 0;
     XtSetArg(args[n], XmNheight, &idd->canvas_height); n++;
     XtSetArg(args[n], XmNwidth, &idd->canvas_width); n++;
     XtGetValues(w, args, n);
     /*
      * Reset the scrollbar slider to indictae the relative proportion of drawing displayed and also the new page size.
      */
     n = 0;
     XtSetArg(args[n], XmNsliderSize, MIN(idd->canvas_height, idd->work_height)); n++;
     XtSetArg(args[n], XmNpageIncrement, idd->canvas_height); n++;
     XtSetValues(idd->vscrollbar, args, n);

     n = 0;
     XtSetArg(args[n], XmNsliderSize, MIN(idd->canvas_width, idd->work_width)); n++;
     XtSetArg(args[n], XmNpageIncrement, idd->canvas_width); n++;
     XtSetValues(idd->hscrollbar, args, n);
}


PBoolean idd_moving = FALSE;

void idd_canvas_mouse_motion(Widget w, Int_Draw_Data *idd, XEvent *event)
{
     if (idd_moving) {
	  int dx, dy;
	  int x = event->xbutton.x;
	  int y = event->xbutton.y;

	  dx = x - idd->start_x;
	  dy = y - idd->start_y;
	  idd->start_x = x;
	  idd->start_y = y;
	  set_int_canvas_view_rel(idd, dx, dy);
     }
}

void idd_canvas_mouse_release(Widget w, Int_Draw_Data *idd, XEvent *event)
{
     if (event->xbutton.button == Button1 && idd_moving) {
	  int x = event->xbutton.x;
	  int y = event->xbutton.y;
	  int dx = x - idd->start_x;
	  int dy = y - idd->start_y;

	  set_int_canvas_view_rel(idd, dx, dy);
	  idd_moving = FALSE;
     }
}

void idd_canvas_mouse_press(Widget w, Int_Draw_Data *idd, XEvent *event)
{
     IOG *iog;
     int x = event->xbutton.x;
     int y = event->xbutton.y;

     switch (event->xbutton.button) {
     case Button2:
	  x = x + idd->left;
	  y = y + idd->top;
	  sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) {
	       if (XPointInRegion(iog->region, x, y)) {
		    xpShowIDialogManage(iog->u.ginode->intention);
		    return;
	       }
	  }
	  break;
     case Button3:
	  x = x + idd->left;
	  y = y + idd->top;
	  sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) {
	       if (XPointInRegion(iog->region, x, y)) {
		    Sprinter *sp = make_sprinter(0);
		    Intention *in = iog->u.ginode->intention;
		    
		    SPRINT(sp, strlen (in->top_op->op->name) + MAX_PRINTED_POINTER_SIZE + 32,
					      sprintf(f,LG_STR("Trace for <Intention %p> %s",
							       "Trace for <Intention %p> %s"), 
						      in, in->top_op->op->name));
		    xp_create_trace_intention_dialog(w, in, SPRINTER_STRING(sp));
		    free_sprinter(sp);
		    return;
	       }
	  }
	  break;
     case Button1:
	  idd_moving = TRUE;
	  idd->start_x = x;
	  idd->start_y = y;
	  break;
     default:
	  break;
     }
}

void set_int_canvas_view_rel(Int_Draw_Data *idd, int xv, int yv)
{
     Arg args[1];
     int x, y;

     int xss = MIN(idd->canvas_width, idd->work_width);
     int yss = MIN(idd->canvas_height, idd->work_height);

     XtSetArg(args[0], XmNvalue, &x);
     XtGetValues(idd->hscrollbar, args, 1);

     XtSetArg(args[0], XmNvalue, &y);
     XtGetValues(idd->vscrollbar, args, 1);

     x -= xv;
     y -= yv;

     x = MIN(x, (int)idd->work_width - xss);
     y = MIN(y, (int)idd->work_height - yss);

     x = MAX(x, 0);
     y = MAX(y, 0);

     XmScrollBarSetValues(idd->hscrollbar, x, xss, 10, idd->canvas_width, False);
     XmScrollBarSetValues(idd->vscrollbar, y, yss, 10, idd->canvas_height, False);
     idd_scroll_bars_moved(XtDisplay(idd->vscrollbar),idd,x,y);
}

void update_int_canvas_size(Int_Draw_Data *idd, int x, int y)
{
     Arg args[1];

     if ((int)idd->work_height < y + 20) {
	  idd->work_height = y + 20;
	  XtSetArg(args[0], XmNmaximum, idd->work_height);
	  XtSetValues(idd->vscrollbar, args, 1);

     }
     if ((int)idd->work_width < x + 20) {
	  idd->work_width = x + 20;
	  XtSetArg(args[0], XmNmaximum, idd->work_width);
	  XtSetValues(idd->hscrollbar, args, 1);
     }
}

#define IDD_VMARG 5
#define IDD_HMARG 30

PBoolean preceded_by_unplaced(Intention_Graph *ig,IOG *iog)
{
     Intention_List il;
     Intention *intention;
     PBoolean res = FALSE;

     il = predecessor_intention(ig,iog->u.ginode->intention);
     sl_loop_through_slist(il, intention, Intention *) {
	  if (!(intention->iog->placed)) {
	       res = TRUE;
	       break;
	  }
     }

     FREE_SLIST(il);

     return res;
}

int position_list(Int_Draw_Data *idd, Intention_List list, int x, int y)
{
     int max = 0;
     int height, yy = y;
     Intention *intention;
     

     sl_loop_through_slist(list, intention, Intention *) {
	  if (!intention->iog->placed)  {
	       max = MAX(max,(int)intention->iog->width);	/* Get the wider */
	  }
     }

     sl_loop_through_slist(list, intention, Intention *) {
	  if ((!intention->iog->placed) && 
	      (! preceded_by_unplaced(idd->ig,intention->iog))) {
	       int h2;
	       Intention_List free_me;

	       intention->iog->placed = TRUE;
	       y += IDD_VMARG;
	       h2 = position_list(idd, free_me = successor_intention(idd->ig,intention), max + x + IDD_HMARG, y);
	       height = MAX(h2, (int)intention->iog->height);
	       FREE_SLIST(free_me);
	       position_ginode(intention->iog,x, y + (int)(height - intention->iog->height)/2);
	       y += height + IDD_VMARG;
	  }
     }
     
     return y - yy;
}

void draw_intention_graph(Int_Draw_Data *idd)
{
     Widget w = idd->canvas;
     Display *dpy = XtDisplay(w);
     Window win = XtWindow(w);
     IOG *iog;

     sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) {
	  if (iog->text_changed) {
	       update_ginode_text(idd, iog);
	       position_ginode(iog, iog->x, iog->y); /* Do not really move but changed size...*/
	       XClearArea(dpy, win,
			  iog->x - idd->left, iog->y - idd->top,
			  iog->width + 1, iog->height + 1, TRUE);
	  }
     }
     
     if (! idd->reposition_all) {
	  sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) {
	       if (!iog->placed) {
		    idd->reposition_all = TRUE;
		    break;
	       }
	  }
     }
     if ( idd->reposition_all) {
	  sl_loop_through_slist(idd->ig->list_inode, iog, IOG *) 
	       iog->placed = FALSE;
	  sl_loop_through_slist(idd->ig->list_iedge, iog, IOG *)
	       iog->placed = FALSE;
     
	  position_list(idd,idd->ig->list_first, 10, 10);

	  sl_loop_through_slist(idd->ig->list_iedge, iog, IOG *) {
	       position_giedge(iog);
	  }
	  idd->reposition_all = FALSE;
	  XClearArea(dpy, win,0,0,0,0,TRUE);     
     }
}


IOG *create_ginode(Int_Draw_Data *idd, Intention *intention)
{
     Ginode *ginode = MAKE_OBJECT(Ginode);
     IOG *iog = MAKE_OBJECT(IOG);
     
     iog->region = XCreateRegion();
     iog->type = INODE;
     iog->u.ginode = ginode;
     iog->placed = FALSE;
     iog->text_changed = FALSE;
     iog->x = iog->y = iog->width = iog->height = 0;
     
     ginode->intention = intention;
     ginode->lgt_string = NULL;
     ginode->connect = sl_make_slist();

     update_ginode_text(idd,iog);

     sl_add_to_head(idd->ig->list_inode,iog);

     return iog;
}

IOG *create_giedge(Int_Draw_Data *idd, Intention *in, Intention *out)
{
     Giedge *giedge = MAKE_OBJECT(Giedge);
     IOG *iog = MAKE_OBJECT(IOG);
     
     iog->region = XCreateRegion();
     iog->type = IEDGE;
     iog->u.giedge = giedge;
     iog->placed = FALSE;
     iog->text_changed = FALSE;
     iog->x = iog->y = iog->width = iog->height = 0;

     sl_add_to_tail(in->iog->u.ginode->connect,iog);
     sl_add_to_tail(out->iog->u.ginode->connect,iog);
     giedge->in = in;
     giedge->out = out;

     sl_add_to_head(idd->ig->list_iedge,iog);

     return iog;
}

IOG *position_ginode(IOG *iog, int x, int y)
{
     XRectangle rect;

     iog->x = x;
     iog->y = y;
     /* iog->placed = TRUE;     Already done un position_list though. */

     rect.width = iog->width + 1;
     rect.height = iog->height + 1;
     rect.x = iog->x;
     rect.y = iog->y;
     XDestroyRegion(iog->region);
     iog->region = XCreateRegion();
     XUnionRectWithRegion(&rect, iog->region, iog->region);

     update_int_canvas_size(global_int_draw_data, x + iog->width, y + iog->height);

     return iog;
}

IOG *update_ginode_text(Int_Draw_Data *idd, IOG *iog)
{
     char text[BUFSIZ];
     char *message = text;
     char *inv_string = NULL;
     int total_size;
     int nb_printed;
     unsigned int pw, ph;
     Ginode *ginode = iog->u.ginode;
     Op_Instance *op_ins = ginode->intention->top_op;
     PBoolean curr_int, identified;
     char *statuss;
     int save_print_length, pp_width;
     PBoolean save_pp, save_pvn, save_pvv, save_fl;

     if (!XEmptyRegion(iog->region))
	  erase_ginode(idd,iog);

     if (ginode->lgt_string)  free_lgt_string(ginode->lgt_string);

     switch (ginode->intention->status) {
     case IS_ACTIVE:
	  statuss = "";
	  break;
     case IS_SUSP_ACTIVE:
     case IS_SLEEPING:
     case IS_SUSP_SLEEPING:
	  statuss = "S ";
	  break;
     default:
	  statuss = "";
	  break;
     }
     identified = (ginode->intention->id != NULL);

     curr_int = (sl_in_slist(idd->ig->current_intentions, ginode->intention));

     nb_printed = NUM_CHAR_SPRINT(sprintf(message, "%s%s%s %d%s%s%s%s\n",
					statuss,
					(curr_int ? "->" : ""), 
					op_ins->op->name, 
					ginode->intention->priority,
					(identified ? " " : ""),
					(identified ? ginode->intention->id : ""),
					(identified ? " " : ""),
					(curr_int ? "<-" : "")
					));
     if (nb_printed > BUFSIZ) {
	  fprintf(stderr,LG_STR("Buffer capacity exceded in update_ginode_text\n",
				"Buffer capacity exceded in update_ginode_text\n")); 
	  nb_printed = BUFSIZ;
     }

     pp_width = MAX (nb_printed, INT_WIDTH);

     save_pp = pretty_print;
     save_pvn = print_var_name;
     save_pvv = print_var_value;
     save_fl = pretty_fill;
     save_print_length = print_length;

     pretty_print = FALSE;
     print_var_name = FALSE;
     print_var_value = FALSE;
     pretty_fill = TRUE;
     print_length = 5;


     if ( op_ins->goal != NULL) {
	  inv_string = build_string_from_list_lines(pretty_print_expr(pp_width, op_ins->goal->statement));
     } else if ( op_ins->fact != NULL){
	  inv_string = build_string_from_list_lines(pretty_print_expr(pp_width, op_ins->fact->statement));
     } else  fprintf(stderr,LG_STR("update_ginode_tex: Expecting a fact or a goal in the op instance\n",
				   "update_ginode_tex: Expecting a fact or a goal in the op instance\n"));

     pretty_print = save_pp;
     print_var_name = save_pvn;
     print_var_value = save_pvv;
     pretty_fill = save_fl;
     print_length = save_print_length;

     pw = iog->width;
     ph = iog->height;

     if ((total_size = strlen(inv_string) + nb_printed) < BUFSIZ) {
	  strcat(text, inv_string);
	  ginode->lgt_string = ope_string_to_lgt_string(idd->fontlist, text, "edge_cs", TT_TEXT_NONE,
					       &iog->width, &iog->height);
	  FREE(inv_string);
     } else {
	  char *ns = (char *) MALLOC((total_size +1) * sizeof(char));

	  sprintf(ns, "%s%s",text,inv_string);
	  ginode->lgt_string = ope_string_to_lgt_string(idd->fontlist, ns, "edge_cs", TT_TEXT_NONE,
					       &iog->width, &iog->height);
	  FREE(inv_string);
	  FREE(ns);
     }	  

     iog->width += 3;
     iog->height += 3;
     
     if (pw >= iog->width) iog->width = pw; /* if it grows, redraw... */
     else idd->reposition_all = TRUE;
     if (ph >= iog->height) iog->height = ph;
     else idd->reposition_all = TRUE;

     iog->text_changed = FALSE;

     return iog;
}

void touch_intention_ginode(Intention *intention)
{
     if (intention->iog) 
	  intention->iog->text_changed = TRUE;
     else
	  fprintf(stderr, LG_STR("Touching a deleted intention ginode .\n",
				 "Touching a deleted intention ginode .\n"));
}

void destroy_ginode(Int_Draw_Data *idd, IOG *iog)
{
     IOG *ioge;
     Ginode *ginode = iog->u.ginode;

     erase_ginode(idd, iog);

     if (ginode->lgt_string)  free_lgt_string(ginode->lgt_string);

     sl_loop_through_slist(ginode->connect, ioge, IOG *) {
	  if (ioge->u.giedge->out->iog == iog) {
	       sl_delete_slist_node(ioge->u.giedge->in->iog->u.ginode->connect,ioge);
	  } else {
	       sl_delete_slist_node(ioge->u.giedge->out->iog->u.ginode->connect,ioge);
	  }
	  destroy_giedge(idd,ioge);
     }

     XDestroyRegion(iog->region);
     
     sl_delete_slist_node(idd->ig->list_inode,iog);

     FREE_SLIST(ginode->connect);

     FREE(ginode);
     FREE(iog);
}

void destroy_giedge(Int_Draw_Data *idd, IOG *iog)
{
     Giedge *giedge = iog->u.giedge;

     erase_giedge(idd, iog);

     XDestroyRegion(iog->region);

     sl_delete_slist_node(idd->ig->list_iedge,iog);
     
     FREE(giedge);
     FREE(iog);
}

void position_giedge(IOG *iog)
{
     Giedge *giedge = iog->u.giedge;
     IOG *gin = giedge->in->iog;
     IOG *gout = giedge->out->iog;

     XRectangle rect; 
     double tga,alpha;
     int x1,x2,h1,h2,yy1,y2,w1,w2,x11,y11,x22,y22;
     XPoint min, max;

     tga = 0.0;			/* To avoid GCC warning... */

     w1 = gin->width / 2; 
     h1 = gin->height / 2;
     w2 = gout->width / 2; 
     h2 = gout->height / 2;

     x22 = x1 = gin->x + w1;
     y22 = yy1 = gin->y + h1;

     x11 = x2 =  gout->x + w2;
     y11 = y2 =  gout->y + h2;

     if ((x2 - x1) == 0) {
	  giedge->x1 = x1;
	  giedge->y1 = yy1 + h1 * SIGN(y2 -yy1);
	  giedge->x2 = x2;
	  giedge->y2 = y2 + h2 * SIGN(yy1 -y2);
     } else {     
	  tga = (float)(yy1 - y2) / (float)(x2 - x1);
	  giedge->x1 = (int)(x1 + MIN(ABS(h1/tga),w1)*SIGN(x2-x1));
	  giedge->y1 = (int)(yy1 + MIN(ABS(w1*tga),h1)*SIGN(y2-yy1));
	  giedge->x2 = (int)(x2 - MIN(ABS(h2/tga),w2)*SIGN(x2-x1));
	  giedge->y2 = (int)(y2 - MIN(ABS(w2*tga),h2)*SIGN(y2-yy1));
     }

     XDestroyRegion(iog->region);
     iog->region = XCreateRegion();

     min.x = giedge->x1;
     min.y = giedge->y1;
     max.x = giedge->x1;
     max.y = giedge->y1;

     max.x = MAX(max.x, giedge->x2);
     max.y = MAX(max.y, giedge->y2);
     min.x = MIN(min.x, giedge->x2);
     min.y = MIN(min.y, giedge->y2);
     rect.x = min.x;
     rect.y =  min.y;
     rect.width =  max.x - min.x + 1;
     rect.height =  max.y - min.y + 1;
     XUnionRectWithRegion(&rect,iog->region,iog->region);

     if ((x2 - x1) == 0) {
	  if (y2 > yy1) alpha = M_PI_2;
	  else alpha = - M_PI_2;
     } else alpha = atan(tga);
	       
     giedge->fx1 = giedge->x2 - (int)(I_ARR_LENGTH * SIGN(x2-x1) * cos(alpha - I_ARR_ANGLE));
     giedge->fy1 = giedge->y2 - (int)(I_ARR_LENGTH * SIGN(x1-x2) * sin(alpha - I_ARR_ANGLE));
     giedge->fx2 = giedge->x2 - (int)(I_ARR_LENGTH * SIGN(x2-x1) * cos(alpha + I_ARR_ANGLE));
     giedge->fy2 = giedge->y2 - (int)(I_ARR_LENGTH * SIGN(x1-x2) * sin(alpha + I_ARR_ANGLE));

     rect.x = MIN3(giedge->fx1,giedge->fx2,giedge->x2);
     rect.y = MIN3(giedge->fy1,giedge->fy2,giedge->y2);
     rect.width = rect.height = I_ARR_LENGTH + 1;
     XUnionRectWithRegion(&rect,iog->region,iog->region);

     iog->placed = TRUE;
}


void draw_ginode(Int_Draw_Data *idd, IOG *iog)
{
     Widget w = idd->canvas;
     Display *dpy = XtDisplay(w);
     Window win = XtWindow(w);
     Gtext_String *gt_str;

     int x = iog->x - idd->left;
     int y = iog->y - idd->top;
     int wi = iog->width;
     int h = iog->height;

     XDrawRectangle(dpy, win, idd->gc,
		    x, y,
		    wi, h);

     sl_loop_through_slist(iog->u.ginode->lgt_string, gt_str, Gtext_String *) { 
	  XmStringDraw(dpy, win, idd->fontlist, gt_str->xmstring, idd->gc,
			    x + gt_str->off_x +2,
			    y + gt_str->off_y +2,
			    wi - 2,
			    XmALIGNMENT_BEGINNING,
			    XmSTRING_DIRECTION_L_TO_R,
			    NULL);
     } 
}

void erase_ginode(Int_Draw_Data *idd, IOG *iog)
{
     Widget w = idd->canvas;
     Display *dpy = XtDisplay(w);
     Window win = XtWindow(w);

     XClearArea(dpy, win,
		iog->x - idd->left, iog->y - idd->top,
		iog->width + 1, iog->height + 1, TRUE);
}

void erase_giedge(Int_Draw_Data *idd, IOG *iog)
{
     Widget w = idd->canvas;
     Display *dpy = XtDisplay(w);
     Window win = XtWindow(w);

     Giedge *e= iog->u.giedge;
     XPoint arrows[3];
     XPoint lines[2];

     idd_add_expose_region(idd,iog->region);

     lines[0].x = e->x1 - idd->left;
     lines[0].y = e->y1 - idd->top;

     lines[1].x = e->x2 - idd->left;
     lines[1].y = e->y2 - idd->top;

     XDrawLines(dpy, win, idd->sgc, lines, 2, CoordModeOrigin);

     arrows[0].x = e->x2 - idd->left;
     arrows[0].y = e->y2 - idd->top;
     arrows[1].x = e->fx1 - idd->left;
     arrows[1].y = e->fy1 - idd->top;
     arrows[2].x = e->fx2 - idd->left;
     arrows[2].y = e->fy2 - idd->top;

     XFillPolygon(dpy, win, idd->sgc,
		  arrows, 3, Convex, CoordModeOrigin);

}

void draw_giedge(Int_Draw_Data *idd, IOG *iog)
{
     Widget w = idd->canvas;
     Display *dpy = XtDisplay(w);
     Window win = XtWindow(w);

     Giedge *e =iog->u.giedge;

     XPoint arrows[3];
     XPoint lines[2];

     lines[0].x = e->x1 - idd->left;
     lines[0].y = e->y1 - idd->top;

     lines[1].x = e->x2 - idd->left;
     lines[1].y = e->y2 - idd->top;

     XDrawLines(dpy, win, idd->gc, lines, 2, CoordModeOrigin);

     arrows[0].x = e->x2 - idd->left;
     arrows[0].y = e->y2 - idd->top;
     arrows[1].x = e->fx1 - idd->left;
     arrows[1].y = e->fy1 - idd->top;
     arrows[2].x = e->fx2 - idd->left;
     arrows[2].y = e->fy2 - idd->top;

     XFillPolygon(dpy, win, idd->gc,
		  arrows, 3, Convex, CoordModeOrigin);
}

#endif
