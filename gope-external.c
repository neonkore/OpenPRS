/*                               -*- Mode: C -*-
 * ope-external.c --
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
#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "op-structure.h"

#include "gope-graphic.h"
#include "gope-global.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs-pprint_f.h"
#include "gope-graphic_f.h"
#include "gope-external_f.h"
#include "oprs-type_f.h"
#include "xm2gtk_f.h"

ListLines pretty_print_body(int width, Body *bd);

void clear_op_graphic(Draw_Data *dd)
{
  if (!dd->just_compiling)
    XClearWindow(XtDisplay(dd->canvas), dd->window);
  dd->op = NULL;
}

void clear_specified_op_graphic(Draw_Data *dd, Op_Structure *op)
{
  if (dd->op == op) {
    XClearWindow(XtDisplay(dd->canvas), dd->window);
    dd->op = NULL;
  }
}

#ifndef GTK
int xmstrcmp(XmString xs1, XmString xs2)
{
     char s1[LINSIZ];
     char s2[LINSIZ];

     char *sp1, *sp2;
     
     sp1 = s1;			/* We need that for an implicit casting... */
     sp2 = s2;
	  
     if ((XmStringGetLtoR(xs1,XmSTRING_DEFAULT_CHARSET,&sp1)) &&
	 (XmStringGetLtoR(xs2,XmSTRING_DEFAULT_CHARSET,&sp2)))
	  return (strcmp(s1, s2));
     else
	  return 0;
}
#endif

XmString xs_str_array_to_xmstr_cs(char *string_array[], int n)
{
  XmString xmstr;
  int i;

  /*
   * If the array is empty just return an empty string.
   */
  if (n <= 0)
    return (XmStringCreate(""));

  xmstr = (XmString) NULL;

  for (i = 0; i < n; i++) {
    XmString x1,x2;
    if (i > 0) {
      xmstr = XmStringConcat(x1 = xmstr, x2 = XmStringSeparatorCreate());
      XmStringFree(x1);
      XmStringFree(x2);
    }
    xmstr = XmStringConcat(x1 = xmstr, x2 = XmStringCreate(string_array[i]));
    if (x1) XmStringFree(x1);
    XmStringFree(x2);
  }
  return (xmstr);
}

XmString ope_string_to_xmstring(cairo_t *cr, char *string, Text_Type tt, Dimension * w, Dimension * h)
{
  char **s;
  char *search, *string_array, *tmp, *title;
  int i = 0;
  XmString res, res2;
  XmString x1,x2;
  Dimension w2, h2;

  if (tt == TT_TEXT_NONE) {
    res = NULL;
    *w = *h = 0;
  } else {  
    switch (tt) {
    case TT_INVOCATION:
      title = "INVOCATION:";
      break;
    case TT_CONTEXT:
      title = "CONTEXT:";
      break;
    case TT_CALL:
      title = "CALL:";
      break;
    case TT_SETTING:
      title = "SETTING:";
      break;
    case TT_PROPERTIES:
      title = "PROPERTIES:";
      break;
    case TT_DOCUMENTATION:
      title = "DOCUMENTATION:";
      break;
    case TT_EFFECTS:
      title = "EFFECTS:";
      break;
    case TT_ACTION:
      title = "ACTION:";
      break;
    case TT_BODY:
      title = "BODY:";
      break;
    default:
      title = "ERROR:"; /* To avoid gcc warnings */
      fprintf(stderr, LG_STR("ope_string_to_xmstring: unknown text_type string...\n",
			     "ope_string_to_xmstring: unknown text_type string...\n"));
      break;
    }
    x1 = XmStringCreate(title);
    res = XmStringConcat(x1 , x2 = XmStringSeparatorCreate());
    XmStringFree(x1);
    XmStringFree(x2);
    cairo_text_extents_t extents;
 
    cairo_text_extents(cr, res, &extents);
    *w = extents.width;
    *h = extents.height;
  }

  NEWSTR(string, string_array);
  tmp = string_array;

  i = 1;
  while ((search = strchr(string_array, '\n')) != NULL) {
    string_array = search + 1;
    i++;
  }
  s = (char **) MALLOC( i * sizeof(char *));

  i = 0;
  string_array = tmp;
  s[i++] = string_array;
  while ((search = strchr(string_array, '\n')) != NULL) {
    *search = '\0';
    s[i++] = string_array = search + 1;
  }
  cairo_text_extents_t extents;
 
  res2 = xs_str_array_to_xmstr_cs(s, i);
  cairo_text_extents(cr, res2, &extents);
  w2 = extents.width;
  h2 = extents.height;
  FREE(tmp);
  FREE(s);

  *w = MAX(*w, w2);
  *h = *h + h2;
  if (res) {
    res2 = XmStringConcat(x1 = res, x2 = res2);
    XmStringFree(x1);
    XmStringFree(x2);
  }
  return res2;
}


Gtext_String *create_gt_str(char *s, Dimension h, Dimension w )
{
     int i = 0;
     char *tmp = s;
     Gtext_String *gt_str =  MAKE_OBJECT(Gtext_String);

     while (*tmp == ' ') {
	  tmp++;
	  i ++;
     }
     gt_str->xmstring = XmStringCreate(tmp);
     gt_str->off_x = w * i;
     gt_str->off_y = h;

     return gt_str;
}
 
List_Gtext_String xs_str_array_to_lgt_str_cs(char *string_array[], int n, cairo_t *cr,
					     Dimension *tw, Dimension *th)
{
  List_Gtext_String lgt_str = sl_make_slist();
  Dimension height, width, h = *th;
  XmString xmstr;
  int i;

  Gtext_String *gt_str;
  cairo_text_extents_t extents;
  //cairo_t *cr;

  //  cr = mainCGCsp->cr_title;
  cairo_text_extents(cr, " ", &extents);
  height = extents.height;
  width = extents.height;
	
  /*
   * If the array is empty just return an empty string.
   */
  if (n <= 0) {
    sl_add_to_tail(lgt_str, create_gt_str("", 0, width));
    return lgt_str;
  }
	  
  for (i = 0; i < n; i++) {
    gint sw;
    cairo_text_extents_t extents;

    gt_str = create_gt_str(string_array[i], h, width);
    sl_add_to_tail(lgt_str, gt_str);
    h += height;
 
    cairo_text_extents(cr, gt_str->xmstring, &extents);
    sw = extents.width;

    *tw = MAX(*tw, ( sw +  (Dimension)gt_str->off_x));
  }
  *th = h;

  return (lgt_str);
}

List_Gtext_String ope_string_to_lgt_string(cairo_t *cr, char *string, 
					  Text_Type tt, Dimension * w, Dimension * h)
{
     char **s;
     char *search, *string_array, *tmp, *title;
     int i = 0;
     XmString xms_title;

     Gtext_String *gt_str_title;
     List_Gtext_String res;
     
     if (tt == TT_TEXT_NONE) {
	  gt_str_title = NULL;
	  *w = *h = 0;
     } else {  
	  switch (tt) {
	  case TT_INVOCATION:
	       title = "INVOCATION:";
	       break;
	  case TT_CONTEXT:
	       title = "CONTEXT:";
	       break;
	  case TT_CALL:
	       title = "CALL:";
	       break;
	  case TT_SETTING:
	       title = "SETTING:";
	       break;
	  case TT_PROPERTIES:
	       title = "PROPERTIES:";
	       break;
	  case TT_DOCUMENTATION:
	       title = "DOCUMENTATION:";
	       break;
	  case TT_EFFECTS:
	       title = "EFFECTS:";
	       break;
	  case TT_ACTION:
	       title = "ACTION:";
	       break;
	  case TT_BODY:
	       title = "BODY:";
	       break;
	  default:
       	       title = "ERROR:"; /* To avoid gcc warnings */
	       fprintf(stderr, LG_STR("ope_string_to_lgt_string: unknown text_type string...\n",
				      "ope_string_to_lgt_string: unknown text_type string...\n"));
	       break;
	  }
	  xms_title = XmStringCreate(title);
	  cairo_text_extents_t extents;
	  
	  cairo_text_extents(cr, xms_title, &extents);
	  *w = extents.width;
	  *h = extents.height;

	  gt_str_title =  MAKE_OBJECT(Gtext_String);
	  gt_str_title->xmstring = xms_title;
	  gt_str_title->off_x = 0;
	  gt_str_title->off_y = 0;
     }

     NEWSTR(string, string_array);
     tmp = string_array;

     i = 1;
     while ((search = strchr(string_array, '\n')) != NULL) {
	  string_array = search + 1;
	  i++;
     }
     s = (char **) MALLOC( i * sizeof(char *));

     i = 0;
     string_array = tmp;
     s[i++] = string_array;
     while ((search = strchr(string_array, '\n')) != NULL) {
	  *search = '\0';
	  s[i++] = string_array = search + 1;
     }

     res = xs_str_array_to_lgt_str_cs(s, i, cr, w, h);

     FREE(tmp);
     FREE(s);

     if (gt_str_title != NULL) {
	  sl_add_to_head(res, gt_str_title);
     }
     return res;
}

void free_lgt_string(List_Gtext_String lgt_str) 
{
     Gtext_String *gt_str;

     sl_loop_through_slist(lgt_str, gt_str, Gtext_String *) { 
	  XmStringFree(gt_str->xmstring);
	  FREE(gt_str);
     }
     FREE_SLIST(lgt_str);
}

OG *make_op_title(Draw_Data *dd, char *name)
{
     Gtext *text;
     OG *og;
     XRectangle rect;
     char *s, *s2;

     text = MAKE_OBJECT(Gtext);
     og = MAKE_OBJECT(OG);
     og->type = DT_TEXT;
     og->u.gtext = text;
     og->region = XCreateRegion();
     og->selected = FALSE;

     NEWSTR(name, s);
     if (s[0] == '|') {
	  s[strlen(s) - 1] = '\0';
	  NEWSTR(s + 1, s2);
	  FREE(s);
	  text->string = s2;
     } else 
	  text->string = s;
     
     text->text_width = 0;	/* This need to be initialized... even if it is not used...*/
     text->fill_lines = FALSE;	/* see comment above */

     text->visible = TRUE;
     text->text_type = TT_TEXT_NONE;
     text->list_og_inst = NULL;
     text->lgt_string = ope_string_to_lgt_string(mainCGCsp->cr_title, text->string,
						 TT_TEXT_NONE, &og->width, &og->height);
     rect.x = og->x = OP_TITLE_X;
     rect.y = og->y = OP_TITLE_Y;

     rect.width = og->width;
     rect.height = og->height;
     XUnionRectWithRegion(&rect, og->region, og->region);
	  
     return og;
}

void position_edge_text(OG *og_edge_text, OG *og_edge)
{
     XRectangle rect;
     int x1, yy1, x2, y2;
     List_Knot list_knot = og_edge->u.gedge->list_knot;
     Gedge *edge = og_edge->u.gedge;

     if (sl_slist_empty(list_knot)) {

	  x1 = edge->x1;
	  yy1 = edge->y1;
	  x2 = edge->x2;
	  y2 = edge->y2;

     } else {
	  OG *og1, *og2;
	  int val = sl_slist_length(list_knot);

	  if ((val % 2) == 0) {
	       og1 = (OG *) sl_get_slist_pos(list_knot, val / 2);
	       og2 = (OG *) sl_get_slist_pos(list_knot, val / 2 + 1);
	  } else {
	       og1 = (OG *) sl_get_slist_pos(list_knot, val / 2 + 1);
	       og2 = og1;
	  }
	  x1 = og1->x;
	  yy1 = og1->y;
	  x2 = og2->x;
	  y2 = og2->y;
     }

     rect.x = og_edge_text->x = og_edge_text->u.gedge_text->dx + (x1 + x2 - (int)og_edge_text->width) / 2;
     rect.y = og_edge_text->y = og_edge_text->u.gedge_text->dy + (yy1 + y2 - (int)og_edge_text->height) / 2;

     rect.width = og_edge_text->width;
     rect.height = og_edge_text->height;
     XDestroyRegion(og_edge_text->region);
     og_edge_text->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og_edge_text->region, og_edge_text->region);
}

void position_then_else(OG *og, int x, int y)
{
     XRectangle rect;
     int width = og->width;
     OG *ogt = then_og_from_if_og(og);
     OG *ogf = else_og_from_if_og(og);

     rect.width = ogt->width;
     rect.height = ogt->height;
     ogt->x = rect.x=  x - rect.width;
     ogt->y = rect.y = y;
     XDestroyRegion(ogt->region);
     ogt->region = XCreateRegion();
     XUnionRectWithRegion(&rect, ogt->region, ogt->region);

     rect.width = ogf->width;
     rect.height = ogf->height;
     ogf->x = rect.x = x + width;
     ogf->y = rect.y = y;
     XDestroyRegion(ogf->region);
     ogf->region = XCreateRegion();
     XUnionRectWithRegion(&rect, ogf->region, ogf->region);
}

void position_edge(OG *og_edge)
{
     XRectangle rect;
     OG *ogk;
     double tga, alpha;
     int x1, x2, h1, h2, yy1, y2, w1, w2, x11, y11, x22, y22;
     List_Knot list_knot = og_edge->u.gedge->list_knot;
     Gedge *gedge = og_edge->u.gedge;
     Edge *edge = og_edge->u.gedge->edge;
     XPoint min, max;

     /* XPoint points[MAX_KNOT*2 + 5]; */

     tga = 0.0;		/* To avoid GCC warning... */


     w1 = edge->in->og->width / 2;
     h1 = edge->in->og->height / 2;
     w2 = edge->out->og->width / 2;
     h2 = edge->out->og->height / 2;

     x22 = x1 = edge->in->og->x + w1;
     y22 = yy1 = edge->in->og->y + h1;

     x11 = x2 = edge->out->og->x + w2;
     y11 = y2 = edge->out->og->y + h2;

     if (!sl_slist_empty(list_knot)) {
	  OG *knot1 = (OG *) sl_get_slist_head(list_knot);
	  OG *knot2 = (OG *) sl_get_slist_tail(list_knot);

	  x11 = knot1->u.gknot->x;
	  y11 = knot1->u.gknot->y;

	  x22 = knot2->u.gknot->x;
	  y22 = knot2->u.gknot->y;
     }
     if ((x11 - x1) == 0) {
	  gedge->x1 = x1;
	  gedge->y1 = yy1 + h1 * SIGN(y11 - yy1);
     } else {
	  tga = (float) (yy1 - y11) / (float) (x11 - x1);
	  gedge->x1 = (int) (x1 + MIN(ABS(h1 / tga), w1) * SIGN(x11 - x1));
	  gedge->y1 = (int) (yy1 + MIN(ABS(w1 * tga), h1) * SIGN(y11 - yy1));
     }

     if ((x2 - x22) == 0) {
	  gedge->x2 = x2;
	  gedge->y2 = y2 + h2 * SIGN(y22 - y2);
     } else {
	  tga = (float) (y22 - y2) / (float) (x2 - x22);
	  gedge->x2 = (int) (x2 - MIN(ABS(h2 / tga), w2) * SIGN(x2 - x22));
	  gedge->y2 = (int) (y2 - MIN(ABS(w2 * tga), h2) * SIGN(y2 - y22));
     }

     XDestroyRegion(og_edge->region);
     og_edge->region = XCreateRegion();

     min.x = gedge->x1;
     min.y = gedge->y1;
     max.x = gedge->x1;
     max.y = gedge->y1;

     sl_loop_through_slist(list_knot, ogk, OG *) {
	  max.x = MAX(max.x, ogk->u.gknot->x);
	  max.y = MAX(max.y, ogk->u.gknot->y);
	  min.x = MIN(min.x, ogk->u.gknot->x);
	  min.y = MIN(min.y, ogk->u.gknot->y);
	  rect.x = min.x;
	  rect.y = min.y;
	  rect.width = max.x - min.x + 1;
	  rect.height = max.y - min.y + 1;
	  XUnionRectWithRegion(&rect, og_edge->region, og_edge->region);
	  max.x = ogk->u.gknot->x;
	  max.y = ogk->u.gknot->y;
	  min.x = ogk->u.gknot->x;
	  min.y = ogk->u.gknot->y;
     }

     max.x = MAX(max.x, gedge->x2);
     max.y = MAX(max.y, gedge->y2);
     min.x = MIN(min.x, gedge->x2);
     min.y = MIN(min.y, gedge->y2);
     og_edge->x = rect.x = min.x;
     og_edge->y = rect.y = min.y;
     og_edge->width = rect.width = max.x - min.x + 1;
     og_edge->height = rect.height = max.y - min.y + 1;
     XUnionRectWithRegion(&rect, og_edge->region, og_edge->region);

     if ((x2 - x22) == 0) {
	  if (y2 > y22)
	       alpha = M_PI_2;
	  else
	       alpha = -M_PI_2;
     } else
	  alpha = atan(tga);

     gedge->fx1 = gedge->x2 - (int) (ARR_LENGTH * SIGN(x2 - x22) * cos(alpha - ARR_ANGLE));
     gedge->fy1 = gedge->y2 - (int) (ARR_LENGTH * SIGN(x22 - x2) * sin(alpha - ARR_ANGLE));
     gedge->fx2 = gedge->x2 - (int) (ARR_LENGTH * SIGN(x2 - x22) * cos(alpha + ARR_ANGLE));
     gedge->fy2 = gedge->y2 - (int) (ARR_LENGTH * SIGN(x22 - x2) * sin(alpha + ARR_ANGLE));

     rect.x = MIN3(gedge->fx1, gedge->fx2, gedge->x2);
     rect.y = MIN3(gedge->fy1, gedge->fy2, gedge->y2);
     rect.width = rect.height = ARR_LENGTH + 1;
     XUnionRectWithRegion(&rect, og_edge->region, og_edge->region);

     /*
      * points[n].x = gedge->x1; points[n].y = gedge->y1; n++;
      * sl_loop_through_slist(list_knot, ogk, OG*){ points[n].x = ogk->u.gknot->x;
      * points[n].y = ogk->u.gknot->y; n++; } points[n].x = gedge->fx1;
      * points[n].y = gedge->fy1; n++; points[n].x = gedge->x2; points[n].y =
      * gedge->y2; n++; points[n].x = gedge->fx2; points[n].y = gedge->fy2;
      * n++; for (i = n - 4; i >= 0 ; i--) { points[n].x = points[i].x;
      * points[n].y = points[i].y; n++; }
      * 
      * og_edge->region = XPolygonRegion(points,n,WindingRule);
      */

     position_edge_text(og_edge->u.gedge->text, og_edge);

}

void update_canvas_size(Draw_Data *dd, int x, int y)
{
  //     Arg args[1];

     if ((int)dd->work_height < y + 20) {
	  dd->work_height = y + 20;
	  // XtSetArg(args[0], XmNmaximum, dd->work_height);
	  //XtSetValues(dd->vscrollbar, args, 1);

     }
     if ((int)dd->work_width < x + 20) {
	  dd->work_width = x + 20;
	  //XtSetArg(args[0], XmNmaximum, dd->work_width);
	  //XtSetValues(dd->hscrollbar, args, 1);
     }
}

void change_canvas_size(Draw_Data *dd, int x, int y)
{
  // Arg args[2];

     dd->work_height = y;
     /* XtSetArg(args[0], XmNmaximum, dd->work_height); */
     /* XtSetArg(args[1], XmNsliderSize, MIN(dd->canvas_height, dd->work_height)); */
     /* XtSetValues(dd->vscrollbar, args, 2); */

     dd->work_width = x;
     /* XtSetArg(args[0], XmNmaximum, dd->work_width); */
     /* XtSetArg(args[1], XmNsliderSize, MIN(dd->canvas_width, dd->work_width)); */
     /* XtSetValues(dd->hscrollbar, args, 2); */
}



List_Knot parse_edge_location(Slist *edge_loc)
{
     List_Knot list_knot =  sl_make_slist();
     PBoolean first = TRUE;
     int x,i;

     x = 0 ;			/* Just to avoid GCC warning */
     sl_loop_through_slist(edge_loc, i, int) {
	  if (first) {
	       x = i;
	       first = FALSE;
	  } else {
	       XRectangle rect;
	       Gknot *gknot = MAKE_OBJECT(Gknot);
	       OG *og = MAKE_OBJECT(OG);
	       
	       og->type = DT_KNOT;
	       og->selected = FALSE;
	       og->u.gknot = gknot;
	       og->u.gknot->x = MAX(0,x);
	       og->u.gknot->y = MAX(0,i);
	       sl_add_to_tail(list_knot,og);
	       rect.x = og->x = og->u.gknot->x - (KNOT_SIZE/2);
	       rect.y = og->y = og->u.gknot->y - (KNOT_SIZE/2);
	       rect.width = rect.height = og->width = og->height = KNOT_SIZE;
	       og->region = XCreateRegion();
	       XUnionRectWithRegion(&rect,og->region,og->region);
	       sl_add_to_tail(current_op->list_movable_og,og);
	       sl_add_to_tail(current_op->list_destroyable_og,og);
	       
	       if (!global_draw_data->just_compiling)
		    update_canvas_size(global_draw_data, x, i);

	       first= TRUE;
	  }
     }
     return list_knot;
}

void build_edge_graphic(Edge *edge, Expression *expr, Draw_Data *dd)
{
     Gedge *gedge;
     Gedge_text *gedge_text;
     OG *og_edge, *ogk, *og_edge_text;
     char *text_string;
     PBoolean save_pp, save_pv, save_fl;

     int width;
     ListLines ll;

     gedge = MAKE_OBJECT(Gedge);
     og_edge = MAKE_OBJECT(OG);

     edge->og = og_edge;
     gedge->edge = edge;
     gedge->list_knot = parse_edge_location(edge_location);
     FREE_SLIST(edge_location);

     sl_loop_through_slist(gedge->list_knot, ogk, OG *)
	  ogk->u.gknot->edge = og_edge;

     og_edge->type = DT_EDGE;
     og_edge->u.gedge = gedge;
     og_edge->region = XCreateRegion();
     og_edge->selected = FALSE;

     gedge_text = MAKE_OBJECT(Gedge_text);
     gedge->text = og_edge_text = MAKE_OBJECT(OG);
     og_edge_text->type = DT_EDGE_TEXT;
     og_edge_text->u.gedge_text = gedge_text;
     og_edge_text->selected = FALSE;
     og_edge_text->region = XCreateRegion();

     gedge_text->dx = text_dx;
     gedge_text->dy = text_dy;

     save_pv =  print_var_name;
     save_pp = pretty_print;
     save_fl = pretty_fill;

     pretty_print = TRUE;
     print_var_name = TRUE;
     pretty_fill = fill_lines;

     if (string_width <= 0) 
	  width = edge_width;
     else
	  width = string_width;
     ll = pretty_print_expr(width, expr);
     text_string = build_string_from_list_lines(ll);

     print_var_name = save_pv;
     pretty_print = save_pp;
     pretty_fill = save_fl;

     gedge_text->log_string = text_string;
     gedge_text->text_width = width;
     gedge_text->fill_lines = fill_lines;
     gedge_text->edge = og_edge;
     gedge_text->lgt_log_string = ope_string_to_lgt_string(mainCGCsp->cr_edge, text_string,TT_TEXT_NONE,
						       &og_edge_text->width,&og_edge_text->height);
     position_edge(og_edge);

     sl_add_to_tail(current_op->list_og_edge,og_edge);
     sl_add_to_tail(current_op->list_og_edge_text,og_edge_text);
     sl_add_to_tail(current_op->list_movable_og,og_edge_text);
     sl_add_to_tail(current_op->list_destroyable_og,og_edge_text);
     sl_add_to_tail(current_op->list_editable_og,og_edge_text);
}


OG *make_og_text_field(Draw_Data *dd, Op_Structure *op, Field_Type ft, Text_Type tt, int x, int y, PBoolean visible,
			int string_width, PBoolean pp_fill, int default_width)
{
     PBoolean save_pp, save_pv, save_fl;
     PBoolean save_cr;
     int width;
     XRectangle rect; 
     char *text_string = NULL;	/* Just to please GCC. */
     Gtext *text = MAKE_OBJECT(Gtext);
     OG *og = MAKE_OBJECT(OG);

     og->type = DT_TEXT;
     og->u.gtext = text;
     og->region = XCreateRegion();
     og->selected = FALSE;
	       
     save_pv =  print_var_name;
     save_pp = pretty_print;
     save_fl = pretty_fill;

     pretty_print = TRUE;
     print_var_name = TRUE;
     /* pretty_fill = fill_lines;  this seems wrong... Can you check it Vianney...*/
     pretty_fill = pp_fill;

     if (string_width <= 0) 
	  width = default_width;
     else
	  width = string_width;
     
     switch (ft) {
     case FT_INVOCATION:
	  text_string = build_string_from_list_lines(pretty_print_expr(width, op->invocation));
	  break;
     case FT_CALL:
	  text_string = build_string_from_list_lines(pretty_print_expr(width, op->call));
	  break;
     case FT_CONTEXT:
	  text_string = build_string_from_list_lines(pretty_print_exprlist(width, op->context));
	  break;
     case FT_SETTING:
	  text_string = build_string_from_list_lines(pretty_print_expr(width, op->setting));
	  break;
     case FT_EFFECTS:
	  text_string = build_string_from_list_lines(pretty_print_exprlist(width, op->effects));
	  break;
     case FT_ACTION:
	  text_string = build_string_from_list_lines(pretty_print_action_field(width, op->action));
	  break;
     case FT_BODY:
	  current_body_indent = 0;
	  current_body_line = 0;
	  text_string = build_string_from_list_lines(pretty_print_body(width, op->body));
	  break;
     case FT_PROPERTIES:
	  text_string = build_string_from_list_lines(pretty_print_list_property(width, op->properties));
	  break;
     case FT_DOCUMENTATION:
	  save_cr = replace_cr;
	  replace_cr = FALSE;
	  text_string = build_string_from_list_lines(pretty_print_backslash_string(width, op->documentation));
	  replace_cr = save_cr;
	  break;
     default:
	  fprintf(stderr,LG_STR("unexected node type in build node..\n",
				"unexected node type in build node..\n")); 
	  break;
     }

     print_var_name = save_pv;
     pretty_print = save_pp;
     pretty_fill = save_fl;

     text->visible = visible;
     text->text_width = width;
     text->fill_lines = pp_fill;
     text->string = text_string;
     text->text_type = tt;
     text->list_og_inst = NULL;

     text->lgt_string = ope_string_to_lgt_string(mainCGCsp->cr_text,text_string,tt,
						 &og->width,&og->height);

     rect.x = og->x =  MAX(0,x);
     rect.y = og->y =  MAX(0,y);

     if (!dd->just_compiling)
	  update_canvas_size(dd, x + og->width , y + og->height);

     rect.width = og->width;
     rect.height = og->height;
     XUnionRectWithRegion(&rect,og->region,og->region);

     sl_add_to_tail(op->list_og_text,og);
     if (visible) {
	  sl_add_to_tail(op->list_movable_og,og);
	  sl_add_to_tail(op->list_editable_og,og);
     }
     if ((ft == FT_BODY) && op->body) { /* Need to create all instructions bounding boxes */
	  update_list_og_inst(dd, op, og);
     }
     return og;
}

void update_list_og_inst(Draw_Data *dd, Op_Structure *op, OG *og_body)
{
  XmFontList fl;// = dd->fontlist;
     char *cs = "text_cs";
     Dimension height, width;
     XRectangle rect; 
     XmString xmstr;
     OG *og;
     Ginst *og_inst;
     Instruction *inst;
     Gtext_String *gt_str;
     int x0, y0;
	  
     x0 = og_body->x;
     y0 = og_body->y;

     /* the first gt_string of the list is the title */
     gt_str = sl_get_slist_pos(og_body->u.gtext->lgt_string, 2);
	  
     x0 += gt_str->off_x; /* should be 0 */
     y0 += gt_str->off_y; 

     /* size of indent for this charset */
     xmstr = XmStringCreate(" ");
     height = XmStringHeight(fl, xmstr);
     width = XmStringWidth(fl, xmstr);
     XmStringFree(xmstr);
	  
     sl_loop_through_slist(op->list_og_inst, og, OG *) {
	  og_inst = og->u.ginst;
	  
	  /* Check the type of instruction, this should be deleted */
	  inst = og_inst->inst;

	  switch (inst->type) {
	  case IT_SIMPLE:
	  case IT_IF:
	  case IT_WHILE:
	  case IT_DO:
	       break;
	  default:
	       fprintf(stderr, LG_STR(": there is no OG for this type of instruction, \n",
				      ": there is no OG for this type of instruction, \n"));
	       break;
	  }
	  og->x = x0 + og_inst->indent * width;
	  og->y = y0 + og_inst->num_line * height;
	  og->width = og_inst->width * width;
	  og->height = og_inst->nb_lines * height;
	  
	  og->region = XCreateRegion();
	  rect.x = og->x;
	  rect.y = og->y;
	  rect.width = og->width;
	  rect.height = og->height;
	  XUnionRectWithRegion(&rect,og->region,og->region);

     }
     og_body->u.gtext->list_og_inst = op->list_og_inst;
}

List_Knot parse_knots(Op_Structure *op, Draw_Data *dd, Slist *edge_loc)
{
     List_Knot list_knot =  sl_make_slist();
     PBoolean first = TRUE;
     int x,i;

     x = 0 ;			/* Just to avoid GCC warning */
     sl_loop_through_slist(edge_loc, i, int) {
	  if (first) {
	       x = i;
	       first = FALSE;
	  } else {
	       XRectangle rect;
	       Gknot *gknot = MAKE_OBJECT(Gknot);
	       OG *og = MAKE_OBJECT(OG);
	       
	       og->type = DT_KNOT;
	       og->selected = FALSE;
	       og->u.gknot = gknot;
	       og->u.gknot->x = MAX(0,x);
	       og->u.gknot->y = MAX(0,i);
	       sl_add_to_tail(list_knot,og);
	       rect.x = og->x = og->u.gknot->x - (KNOT_SIZE/2);
	       rect.y = og->y = og->u.gknot->y - (KNOT_SIZE/2);
	       rect.width = rect.height = og->width = og->height = KNOT_SIZE;
	       og->region = XCreateRegion();
	       XUnionRectWithRegion(&rect,og->region,og->region);
	       sl_add_to_tail(op->list_movable_og,og);
	       sl_add_to_tail(op->list_destroyable_og,og);
	       
	       if (!dd->just_compiling)
		    update_canvas_size(dd, x, i);

	       first= TRUE;
	  }
     }
     return list_knot;
}

OG *make_og_edge(Draw_Data *dd, Op_Structure *op,  Edge *edge, Node *in, Node *out,  Slist *knots, int x, int y,
		 int pp_width, PBoolean pp_fill)
{
     Gedge *gedge;
     Gedge_text *gedge_text;
     OG *og_edge, *ogk, *og_edge_text;
     char *text_string;
     PBoolean save_pp, save_pv, save_fl;
     Draw_Type dt = DT_EDGE;
     int width;
     ListLines ll;
     Expression *expr = edge->expr;

     gedge = MAKE_OBJECT(Gedge);
     og_edge = MAKE_OBJECT(OG);

     edge->og = og_edge;
     og_edge->u.gedge = gedge;
     gedge->edge = edge;

     gedge->list_knot = parse_knots(op, dd, knots);
     FREE_SLIST(knots);

     sl_loop_through_slist(gedge->list_knot, ogk, OG *)
	  ogk->u.gknot->edge = og_edge;

     og_edge->type = dt;
     og_edge->region = XCreateRegion();
     og_edge->selected = FALSE;

     gedge_text = MAKE_OBJECT(Gedge_text);
     gedge->text = og_edge_text = MAKE_OBJECT(OG);
     og_edge_text->type = DT_EDGE_TEXT;
     og_edge_text->u.gedge_text = gedge_text;
     og_edge_text->selected = FALSE;
     og_edge_text->region = XCreateRegion();

     gedge_text->dx = x;
     gedge_text->dy = y;

     save_pv =  print_var_name;
     save_pp = pretty_print;
     save_fl = pretty_fill;

     pretty_print = TRUE;
     print_var_name = TRUE;
     pretty_fill = pp_fill;

     if (pp_width <= 0) 
	  width = edge_width;
     else
	  width = pp_width;
     ll = pretty_print_expr(width, expr);
     text_string = build_string_from_list_lines(ll);

     print_var_name = save_pv;
     pretty_print = save_pp;
     pretty_fill = save_fl;

     gedge_text->log_string = text_string;
     gedge_text->text_width = width;
     gedge_text->fill_lines = pp_fill;
     gedge_text->edge = og_edge;
     gedge_text->lgt_log_string = ope_string_to_lgt_string(mainCGCsp->cr_edge,text_string,TT_TEXT_NONE,
						       &og_edge_text->width,&og_edge_text->height);
     position_edge(og_edge);

     sl_add_to_tail(op->list_og_edge,og_edge);
     sl_add_to_tail(op->list_og_edge_text,og_edge_text);
     sl_add_to_tail(op->list_movable_og,og_edge_text);
     sl_add_to_tail(op->list_destroyable_og,og_edge_text);
     sl_add_to_tail(op->list_editable_og,og_edge_text);

     return og_edge;

}

OG *make_cp_graphic(PString name, Node *node)
{
     PString stripped_name;
     Gnode *gnode = MAKE_OBJECT(Gnode);
     OG *og = MAKE_OBJECT(OG);
	   
     og->x = 0;			/* Just to initialize it */
     og->y = 0;
	
     og->u.gnode = gnode;
     gnode->node = node;

     stripped_name = remove_vert_bar(name);

     gnode->xmstring = XmStringCreate(stripped_name); /*  XmSTRING_DEFAULT_CHARSET */
     FREE(stripped_name);

     XmStringExtent(NULL,gnode->xmstring,&gnode->swidth, &gnode->sheight);
     gnode->swidth += 2;
     gnode->sheight += 2;
     og->width = gnode->swidth + 5;
     og->height = gnode->sheight + 5;
     og->selected = FALSE;
     og->type = DT_NODE;
     sl_add_to_tail(current_op->list_og_node,og);
     sl_add_to_tail(current_op->list_movable_og,og);
     sl_add_to_tail(current_op->list_destroyable_og,og);
     sl_add_to_tail(current_op->list_editable_og,og);

     return og;
}

OG *make_inst_graphic(Instruction *inst, Edge *edge)
{
     Ginst *ginst = MAKE_OBJECT(Ginst);
     OG *og = MAKE_OBJECT(OG);
	   
     og->x = 0;			/* Just to initialize it */
     og->y = 0;
	
     og->u.ginst = ginst;
     ginst->inst = inst;
     ginst->edge = edge;

     ginst->indent = 0;
     ginst->num_line = 0;
     ginst->width = 0;
     ginst->nb_lines = 0;

     og->width = 0;
     og->height = 0;

     /*
	og->selected = TRUE;
	*/

     og->selected = FALSE;

     og->type = DT_INST;
     og->region = NULL;
     sl_add_to_tail(current_list_og_inst,og);

     return og;
}


