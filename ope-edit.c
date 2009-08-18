static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*-
 * ope-edit.c --
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

#include "config.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>

#include <Xm/ScrollBar.h>
#include <Xm/DrawingA.h>

/* #include <X11/X10.h> */

#include "macro.h"

#include "oprs-type.h"
#include "ope-graphic.h"
#include "ope-global.h"

#include "oprs-type_f.h"
#include "ope-edit_f.h"
#include "ope-graphic_f.h"
#include "ope-external_f.h"
#include "op-structure_f.h"

#include "ope-rop_f.h"

void draw_clip_box(Widget w, Draw_Data *dd, OG *og);

void ChangeCursor(Draw_Data *dd, PBoolean on, unsigned int shape)
{
    Display *dpy = XtDisplay(dd->canvas);

    XDefineCursor(dpy, dd->window, on ? (XCreateFontCursor(dpy, shape)) : None);
}

void reset_draw_data_no_redraw(Draw_Data *dd)
{
     if (dd->node_selected) {
	  dd->node_selected->selected = FALSE;
	  dd->node_selected = NULL;			 
     }

     if (dd->sensitive_og) {
	  dd->sensitive_og->selected = FALSE;
	  dd->sensitive_og = NULL;			 
     }

     if (dd->edge_selected) {
	  dd->edge_selected->u.gedge->text->selected = FALSE;
	  dd->edge_selected = NULL;
     }

     if (dd->og_align) {
	  dd->og_align->selected = FALSE;
	  dd->og_align = NULL;
     }

     if (dd->sensitive_og) {
	  dd->sensitive_og->selected = FALSE;
	  dd->sensitive_og = NULL;			 
     }
}

void reset_draw_data(Draw_Data *dd)
{
     if (dd->node_selected) {
	  dd->node_selected->selected = FALSE;
	  draw_og(dd->canvas, dd, dd->node_selected);
	  dd->node_selected = NULL;			 
     }

     if (dd->sensitive_og) {
	  dd->sensitive_og->selected = FALSE;
	  draw_og(dd->canvas, dd, dd->sensitive_og);
	  dd->sensitive_og = NULL;			 
     }

     if (dd->edge_selected) {
	  dd->edge_selected->u.gedge->text->selected = FALSE;
	  draw_og(dd->canvas, dd, dd->edge_selected);
	  draw_og(dd->canvas, dd, dd->edge_selected->u.gedge->text);
	  dd->edge_selected = NULL;
     }

     if (dd->og_align) {
	  dd->og_align->selected = FALSE;
	  draw_og(dd->canvas, dd, dd->og_align);
	  /* draw_clip_box(dd->canvas, dd, dd->og_align); */
	  dd->og_align = NULL;
     }

}

void reset_draw_mode(Draw_Data *dd)
{
     Draw_Mode mode = dd->mode;

     switch (mode) {
     case ALIGNING_OG:
	  set_draw_mode(dd, ALIGN_OG);
	  break;
     case ALIGNING_OG_V:
	  set_draw_mode(dd, ALIGN_OG_V);
	  break;
     case ALIGNING_OG_H:
	  set_draw_mode(dd, ALIGN_OG_H);
	  break;
     case NODE_SELECTED:
	  set_draw_mode(dd, DRAW_EDGE);
	  break;
     case EDGE_SELECTED:
	  set_draw_mode (dd, DRAW_KNOT);
	  break;
     case MERGING_NODE:
	  set_draw_mode (dd, MERGE_NODE);
	  break;
     case DUPLICATING_EDGE:
     case DUPLICATING_EDGE2:
	  set_draw_mode(dd, DUPLICATE_OBJECTS);
	  break;
     case DUPLICATING_NODE:
	  set_draw_mode(dd, DUPLICATE_OBJECTS);
	  break;
     default:
	  break;
     }
}

void set_draw_mode(Draw_Data *dd, Draw_Mode mode)
{
     unsigned int shape = False;
     PBoolean on = True;

     dd->mode = mode;

     if (mode_help[mode])
	  UpdateMessageWindow(mode_help[mode]);

     switch (mode) {
     case MOVING_OG:
     case MOVING_CANVAS:
	  shape = XC_fleur;
	  break;
     case  EDIT_OG:
	  shape =  XC_pencil;
	  break;
     case DESTROY_OG:
	  shape = XC_pirate;
	  break;
     default:
	  on = False;
	  break;
     }

     ChangeCursor(dd, on , shape);
}


void set_draw_mode_from_menu(Draw_Data *dd, Draw_Mode mode)
{
     reset_draw_data(dd);
     set_draw_mode(dd, mode);
}

void resize_node(Draw_Data *dd, OG *og)
{
     XRectangle rect;
     Gnode *gnode = og->u.gnode;
     int center_x , center_y;
     PString stripped_name = remove_vert_bar(gnode->node->name);

     center_x = og->x + (int)(og->width / 2);
     center_y = og->y + (int)(og->height / 2);

     gnode->xmstring = XmStringCreate(stripped_name, "node_cs");
     FREE(stripped_name);

     XmStringExtent(dd->fontlist, gnode->xmstring, &gnode->swidth, &gnode->sheight);
     gnode->swidth += 2;
     gnode->sheight += 2;
     og->width = gnode->swidth + 5;
     og->height = gnode->sheight + 5;

     og->x = center_x -  (int)(og->width / 2);
     og->y = center_y -  (int)(og->height / 2);

     rect.width = og->width;
     rect.height = og->height;
     rect.x = og->x;
     rect.y = og->y;
     XDestroyRegion(og->region);
     og->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og->region, og->region);

}

void resize_text(Draw_Data *dd, OG *og_text)
{
     XRectangle rect;

/*      XmStringExtent(dd->fontlist,og_text->u.gtext->xmstring,&og_text->width,&og_text->height); */

     rect.x = og_text->x;
     rect.y = og_text->y;

     rect.width = og_text->width;
     rect.height = og_text->height;
     XDestroyRegion(og_text->region);
     og_text->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og_text->region, og_text->region);
}

void resize_edge_text(Draw_Data *dd, OG *og_edge_text)
{
     XRectangle rect;

     rect.x = og_edge_text->x;
     rect.y = og_edge_text->y;

     rect.width = og_edge_text->width;
     rect.height = og_edge_text->height;
     XDestroyRegion(og_edge_text->region);
     og_edge_text->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og_edge_text->region, og_edge_text->region);
}

void reposition_edge_text(OG *og_edge_text, int x, int y)
{
     og_edge_text->u.gedge_text->dx += x - og_edge_text->x;
     og_edge_text->u.gedge_text->dy += y - og_edge_text->y;
}


OG *create_text(Widget w, int x, int y, Draw_Data *dd, Text_Type tt, PString test, int text_width, PBoolean fill_lines)
{
     Gtext *text;
     OG *og;
     XRectangle rect;
     char *t2;

     text = MAKE_OBJECT(Gtext);
     og = MAKE_OBJECT(OG);
     og->type = DT_TEXT;
     og->u.gtext = text;
     og->region = XCreateRegion();
     og->selected = FALSE;        /* This need to be initialized... */

     NEWSTR(test, t2);
     text->string = t2;
     text->visible = TRUE;
     text->text_width = text_width;	/* This need to be initialized... */
     text->fill_lines = fill_lines;	/* This need to be initialized... */
     text->text_type = tt;
     text->list_og_inst = NULL;
/*
 *     text->xmstring = ope_string_to_xmstring(dd->fontlist, t2, "text_cs", tt, &og->width, &og->height);
 */
     /* XmStringExtent(dd->fontlist,text->xmstring,); */

     text->lgt_string = ope_string_to_lgt_string(dd->fontlist, t2, "text_cs", tt, &og->width, &og->height);
     rect.x = og->x = x;
     rect.y = og->y = y;

     rect.width = og->width;
     rect.height = og->height;
     XUnionRectWithRegion(&rect, og->region, og->region);

     draw_og(w, dd, og);

     sl_add_to_tail(dd->op->list_og_text, og);
     sl_add_to_tail(dd->op->list_movable_og, og);
     sl_add_to_tail(dd->op->list_editable_og, og);

     return og;
}

void edit_og(Widget w, Draw_Data *dd, OG *og, char *string)
{
     report_opfile_modification();

     switch (og->type) {
     case DT_EDGE_TEXT:
	  erase_og(w, dd, og);
	  FREE(og->u.gedge_text->log_string);
	  og->u.gedge_text->log_string = string;
/*
 *	  XmStringFree(og->u.gedge_text->xmstring);
 *	  og->u.gedge_text->xmstring = ope_string_to_xmstring(dd->fontlist, string, "edge_cs", TEXT_NONE,
 *						     &og->width, &og->height);
 */
	  free_lgt_string(og->u.gedge_text->lgt_log_string);
	  og->u.gedge_text->lgt_log_string = ope_string_to_lgt_string(dd->fontlist, string, "edge_cs", TT_TEXT_NONE,
								  &og->width, &og->height);

	  resize_edge_text(dd, og);
	  draw_og(w, dd, og);
	  break;
     case DT_TEXT:
	  erase_og(w, dd, og);
	  FREE(og->u.gtext->string);
	  og->u.gtext->string = string;
	  free_lgt_string(og->u.gtext->lgt_string);
	  og->u.gtext->lgt_string = ope_string_to_lgt_string(dd->fontlist, string, "text_cs", og->u.gtext->text_type,
						     &og->width, &og->height);
	  resize_text(dd, og);
	  if (og->u.gtext->text_type == TT_BODY) {
	       update_list_og_inst(dd, current_op, og);
	  }
	  draw_og(w, dd, og);
	  break;
     default:
	  fprintf(stderr, LG_STR("edit_og: this graphic type has no editable string.\n",
				 "edit_og: ce type graphique n'a pas de chaine editable.\n"));
	  break;
     }

}


void create_edge(Widget w, Draw_Data *dd, OG *og_selected, OG *og, char *t1, int width, PBoolean fill_lines, Draw_Type gtype, Edge_Type type, List_Knot list_knot, Expression *expr)
{
     Gedge *gedge;
     Edge *edge;
     Gedge_text *gedge_text;
     XRectangle rect;
     OG *og_edge, *ogk, *og_edge_text;

     gedge = MAKE_OBJECT(Gedge);
     og_edge = MAKE_OBJECT(OG);
     og_edge->selected = FALSE; /* To avoid the sliht possibility to be born selected... */

     edge = make_edge();
     edge->expr = expr;
     edge->type = type;
     sl_add_to_head(current_op->edge_list,edge);

     edge->og = og_edge;
     gedge->edge = edge;
     gedge->list_knot = list_knot;

     sl_loop_through_slist(list_knot, ogk, OG *) {
	  rect.x = ogk->x = ogk->u.gknot->x - (KNOT_SIZE / 2);
	  rect.y = ogk->y = ogk->u.gknot->y - (KNOT_SIZE / 2);
	  rect.width = rect.height = ogk->width = ogk->height = KNOT_SIZE;
	  ogk->region = XCreateRegion();
	  XUnionRectWithRegion(&rect, ogk->region, ogk->region);
	  ogk->u.gknot->edge = og_edge;
	  sl_add_to_tail(dd->op->list_movable_og, ogk);
	  sl_add_to_tail(dd->op->list_destroyable_og, ogk);
     }

     og_edge->type = gtype;
     og_edge->u.gedge = gedge;
     og_edge->region = XCreateRegion();

     sl_add_to_tail(og_selected->u.gnode->node->out, edge);
     edge->in = og_selected->u.gnode->node;
     sl_add_to_tail(og->u.gnode->node->in, edge);
     edge->out = og->u.gnode->node;

     if (gtype == DT_EDGE) {
	  gedge_text = MAKE_OBJECT(Gedge_text);
	  gedge->text = og_edge_text = MAKE_OBJECT(OG);
	  og_edge_text->selected = FALSE; /* To avoid the sliht possibility to be born selected... */
	  og_edge_text->type = DT_EDGE_TEXT;
	  og_edge_text->u.gedge_text = gedge_text;
	  og_edge_text->region = XCreateRegion();

	  gedge_text->dx = 0;
	  gedge_text->dy = 0;
	  gedge_text->log_string = t1;
	  gedge_text->text_width = width;
	  gedge_text->fill_lines = fill_lines;
	  gedge_text->lgt_log_string = ope_string_to_lgt_string(dd->fontlist, t1, "edge_cs", TT_TEXT_NONE,
				 &og_edge_text->width, &og_edge_text->height);
	  gedge_text->edge = og_edge;

	  position_edge(og_edge);
	  draw_og(w, dd, og_edge);
	  draw_og(w, dd, og_edge_text);
	  sl_add_to_tail(dd->op->list_og_edge, og_edge);

	  sl_add_to_tail(dd->op->list_og_edge_text, og_edge_text);
	  sl_add_to_tail(dd->op->list_movable_og, og_edge_text);
	  sl_add_to_tail(dd->op->list_destroyable_og, og_edge_text);
	  sl_add_to_tail(dd->op->list_editable_og, og_edge_text);
     } else {
	  sl_add_to_tail(dd->op->list_og_edge, og_edge);
	  gedge->text = NULL;
     }
}

PBoolean check_merge_compatible(OG *og1, OG *og2)
{
     Node_Type type1 = og1->u.gnode->node->type;
     Node_Type type2 = og2->u.gnode->node->type;

     PBoolean out2 = (! sl_slist_empty(og2->u.gnode->node->out));

     if (og1 == og2) {
	  report_user_error(LG_STR("You select twice the same node in merge operation.",
				   "Vous avez sélectionné deux fois le même noeud dans une opération de regroupement."));
	  return FALSE;
     } else if ((IF_THEN_ELSE_NODE_P(og1->u.gnode->node)) || (IF_THEN_ELSE_NODE_P(og2->u.gnode->node))) {
	  report_user_error(LG_STR("No IF-THEN-ELSE node in merge operation.",
				   "Pas de noeud SI-ALORS-SINON dans une opération de regroupement."));
	  return FALSE;
     } else if ((type1 == NT_END) && out2) {
	  report_user_error(LG_STR("This is an incompatible merge, node1 is end, but node2 as outgoing edges.",
				   "Opération de regroupement interdite: node1 est final, et node2 a des arcs sortants."));
     	  return FALSE;
     } else if ((type2 == NT_END) && (type1 != NT_END)) {
	  report_user_error(LG_STR("This is an incompatible merge, node1 is not end, but node2 is.",
				   "Opération de regroupement interdite: node1 n'est pas final, mais node2 l'est."));
	  return FALSE;
     } else if (type2 == NT_START) {
	  report_user_error(LG_STR("This is an incompatible merge, node 2 is start, you will loose it...\nDo it the other way around.",
				   "Opération de regroupement interdite: node2 est un noeud début, vous le perdrez. Inversez votre choix."));
	  return FALSE;
     } else 
	  return TRUE;
}

void rename_and_redraw_node(Widget w, Draw_Data *dd, OG *og, PString name)
{   
     char *node_name;
     Node *node = og->u.gnode->node;
     Edge *edge;
  
     erase_og(w, dd, og);
     sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	  erase_og(w, dd, edge->og);
	  erase_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	  erase_og(w, dd, edge->og);
	  erase_og(w, dd, edge->og->u.gedge->text);
     }

     NEWSTR(name, node_name);	/* Sometime the name is in the text executable */

     DECLARE_ID(node_name, node->name);
     FREE(node_name);

     resize_node(dd, og);
     sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	  position_edge(edge->og);
	  draw_og(w, dd, edge->og);
	  draw_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	  position_edge(edge->og);
	  draw_og(w, dd, edge->og);
	  draw_og(w, dd, edge->og->u.gedge->text);
     }
     draw_og(w, dd, og);
}

void rename_and_redraw_if_node(Widget w, Draw_Data *dd, OG *og,
			       PString if_name, PString then_name, PString else_name)
{   
     char *node_name;
     Node *node = og->u.gnode->node;
     Edge *edge;
     Node *then_node, *else_node;

     then_node = then_node_from_if_node(node);
     else_node = else_node_from_if_node(node);

     erase_og(w,dd,else_og_from_if_og(og));
     sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	  erase_og(w, dd, edge->og);
	  erase_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	  if (edge->og) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
     }

     erase_og(w,dd,then_og_from_if_og(og));
     sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	  erase_og(w, dd, edge->og);
	  erase_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	  if (edge->og) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
     }
     erase_og(w, dd, og);
     sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	  erase_og(w, dd, edge->og);
	  erase_og(w, dd, edge->og->u.gedge->text);
     }

     NEWSTR(if_name, node_name);	/* Sometime the name is in the text executable */
     DECLARE_ID(node_name, node->name);
     FREE(node_name);

     NEWSTR(then_name, node_name);	
     DECLARE_ID(node_name, then_node->name);
     FREE(node_name);

     NEWSTR(else_name, node_name);	
     DECLARE_ID(node_name, else_node->name);
     FREE(node_name);

     resize_node(dd, og);
     position_then_else(og,  og->x,  og->y);
     sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	  position_edge(edge->og);
	  draw_og(w, dd, edge->og);
	  draw_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	  if (edge->og) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
     }
     sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	  position_edge(edge->og);
	  draw_og(w, dd, edge->og);
	  draw_og(w, dd, edge->og->u.gedge->text);
     }
     sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	  if (edge->og) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
     }
     draw_og(w,dd,else_og_from_if_og(og));
     draw_og(w,dd,then_og_from_if_og(og));

     sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	  position_edge(edge->og);
	  draw_og(w, dd, edge->og);
	  draw_og(w, dd, edge->og->u.gedge->text);
     }
     draw_og(w, dd, og);
}

void convert_end(Widget w, Draw_Data *dd, OG *og)
{
     char *name;
     Node *node = og->u.gnode->node;

     if (START_NODE_P(node)) {
	  report_user_error(LG_STR("You cannot convert the start node in an end node.",
				   "Vous ne pouvez convertir le noeud début en noeud fin."));
	  return;
     } else if (IF_THEN_ELSE_NODE_P(node)) {
	  report_user_error(LG_STR("You cannot convert the IF_THEN_ELSE node in an end node.",
				   "Vous ne pouvez convertir un noeud SI-ALORS-SINON en noeud fin."));
	  return;
     } else if (node->type == NT_END) {
	  name = new_node_name(dd->op);
	  node->type = NT_PROCESS;
     } else {
	  if (sl_slist_empty(node->out)) {
	       name = new_end_node_name(dd->op);
	       node->type = NT_END;
	  } else {
	       report_user_error(LG_STR("This node has outgoing edges, you cannot convert it to an end node.",
					"Ce noeud a des arcs sortants, vous ne pouvez le convertir en noeud fin."));
	       return;
	  }
     }
     rename_and_redraw_node(w,dd,og,name);
}

void flip_split(Widget w, Draw_Data *dd, OG *og)
{
     if (og->type != DT_IF_NODE) {
	  og->u.gnode->node->split = ! (og->u.gnode->node->split);
	  draw_og(w, dd, og);
	  report_opfile_modification();
     } else 
	  report_user_error(LG_STR("You cannot split from an IF node.",
				   "Vous ne pouvez spliter d'un noeud SI."));
}

void flip_join(Widget w, Draw_Data *dd, OG *og)
{
     if (og->u.gnode->node->join || /* For these which have been set by error in previous releases... */
	 ((og->type != DT_IF_NODE) &&(og->type != DT_THEN_NODE) && (og->type != DT_ELSE_NODE))) {
	  og->u.gnode->node->join = ! (og->u.gnode->node->join);
	  draw_og(w, dd, og);
	  report_opfile_modification();
     } else 
	  report_user_error(LG_STR("You cannot join on an IF, THEN or ELSE node.",
				   "Vous ne pouvez joiner sur un noeud SI, ALORS ou SINON."));
}

void convert_start(Widget w, Draw_Data *dd, OG *og)
{
     Node *node = og->u.gnode->node;
     
     if  (START_NODE_P(node)) {
	  report_user_error(LG_STR("This node is the start node, you cannot convert it to a standard node\n\
you need to convert a standard node to a start node\n\
(and this one will automatically become standard).",
"This node is the start node, you cannot convert it to a standard node\n\
you need to convert a standard node to a start node\n\
(and this one will automatically become standard)."));
	  return;
     } else if (og->type == DT_IF_NODE || og->type == DT_THEN_NODE ||  og->type == DT_ELSE_NODE  ) {
	  report_user_error(LG_STR("This node is an IF node, you cannot convert it to a START node.\n",
"This node is an IF node, you cannot convert it to a START node.\n"));
	  return;
     } else {
	  Node *old_start_node;
	  OG *og_osn;

	  node->type = NT_START;
	  old_start_node = current_op->start_point;
	  og_osn = old_start_node->og;
	  current_op->start_point = node;
	  sl_delete_slist_node(dd->op->list_destroyable_og, og);
	  sl_delete_slist_node(dd->op->list_editable_og, og);
	  
	  rename_and_redraw_node(w,dd,og,"START");

	  old_start_node->type = NT_PROCESS;
	  sl_add_to_tail(dd->op->list_destroyable_og, og_osn);
	  sl_add_to_tail(dd->op->list_editable_og, og_osn);
	  	  
	  rename_and_redraw_node(w,dd,og_osn, new_node_name(dd->op));
	  report_opfile_modification();
     }
     
}

void make_start_node(Widget w, Draw_Data *dd)
{
     char name[] = "START";
     char *node_name;
     Node *node;
     OG *og;
     XRectangle rect;

     DECLARE_ID(name, node_name);
     node = make_cp(node_name, TRUE);
     node->type = NT_START;
     current_op->start_point = node;
     og = node->og;
     og->x = rect.x = START_X;
     og->y = rect.y = START_Y;
     rect.width = og->width;
     rect.height = og->height;
     og->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og->region, og->region);
     draw_og(w, dd, og);
/*      sl_add_to_tail(dd->op->list_og_node,og);
     sl_add_to_tail(dd->op->list_movable_og,og); */
     sl_delete_slist_node(dd->op->list_destroyable_og, og);
     sl_delete_slist_node(dd->op->list_editable_og, og);

}

OG *create_node(Widget w, int x, int y, Draw_Data *dd, PBoolean split, PBoolean join)
{
     Node *node;
     OG *og;
     XRectangle rect;

     node = make_cp(new_node_name(dd->op), TRUE);
     node->type = NT_PROCESS;
     node->split = split;
     node->join = join;
     og = node->og;
     og->x = rect.x = x;
     og->y = rect.y = y;
     rect.width = og->width;
     rect.height = og->height;
     og->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og->region, og->region);
     draw_og(w, dd, og);

     return og;
}

OG *create_ifnode(Widget w, int x, int y, Draw_Data *dd)
{
     OG *og, *ogt, *ogf;
     PString nelse = NULL;
     PString nthen = NULL;
     PString nif = NULL;
     PString *nifp;
     PString *nelsep;
     PString *nthenp;

     nifp = &nif;
     nelsep = &nelse;
     nthenp = &nthen;

     new_if_then_else_node_name(dd->op, nifp, nthenp, nelsep);

     og = build_and_add_node(dd->op, nif, NT_IF, FALSE, FALSE, x, y, dd);
     ogt = build_and_add_node(dd->op, nthen, NT_THEN, FALSE, FALSE, x , y, dd);
     ogf = build_and_add_node(dd->op, nelse, NT_ELSE, FALSE, FALSE, x , y, dd);

     build_and_add_then_else_edge(dd->op,
				  og->u.gnode->node->name,
				  ogt->u.gnode->node->name, ET_THEN, dd);
     build_and_add_then_else_edge(dd->op,
				  og->u.gnode->node->name,
				  ogf->u.gnode->node->name, ET_ELSE, dd);
     
     position_then_else(og, x, y);

     draw_og(w, dd, og);
     draw_og(w, dd, ogt);
     draw_og(w, dd, ogf);

     return og;
}

void open_node(Widget w, Draw_Data *dd, OG *og)
{
     OG *new_node; 
     Edge *edge;
     PString text;
     Slist *list_edge;
     Node *old_node = og->u.gnode->node;

     if (IF_THEN_ELSE_NODE_P(old_node)) {
	  report_user_error(LG_STR("Cannot open/split an IF_THEN_ELSE node... Yet.",
				   "Cannot open/split an IF_THEN_ELSE node... Yet."));
	  return;
     }

     new_node = create_node(w, og->x, og->y + 20 , dd, og->u.gnode->node->split, og->u.gnode->node->join);

     list_edge = COPY_SLIST(old_node->out);
     sl_loop_through_slist(list_edge, edge, Edge *) {
	  dd->list_knot = sl_make_slist();
	  NEWSTR(edge->og->u.gedge->text->u.gedge_text->log_string, text);
	  create_edge(dd->canvas, dd, new_node, edge->out->og, text,
		      edge->og->u.gedge->text->u.gedge_text->text_width,
		      edge->og->u.gedge->text->u.gedge_text->fill_lines,
		      edge->og->type, edge->type, dd->list_knot, copy_expr(edge->expr));
	  destroy_og(dd, edge->og);
     }
     FREE_SLIST(list_edge);
}

void duplicate_node(Widget w, int x, int y, Draw_Data *dd, Node *old_node)
{
     OG *new_node;
     Edge *edge;
     PString text;

     new_node = create_node(w,x,y,dd, old_node->split, old_node->join);

     sl_loop_through_slist(old_node->in, edge, Edge *) {
	  dd->list_knot = sl_make_slist();
	  NEWSTR(edge->og->u.gedge->text->u.gedge_text->log_string, text);
	  create_edge(dd->canvas, dd, edge->in->og ,new_node, text, 
		      edge->og->u.gedge->text->u.gedge_text->text_width,
		      edge->og->u.gedge->text->u.gedge_text->fill_lines,
		      edge->og->type, edge->type, dd->list_knot, copy_expr(edge->expr));
     }

     sl_loop_through_slist(old_node->out, edge, Edge *) {
	  dd->list_knot = sl_make_slist();
	  NEWSTR(edge->og->u.gedge->text->u.gedge_text->log_string, text);
	  create_edge(dd->canvas, dd, new_node, edge->out->og, text,
		      edge->og->u.gedge->text->u.gedge_text->text_width,
		      edge->og->u.gedge->text->u.gedge_text->fill_lines,
		      edge->og->type, edge->type, dd->list_knot, copy_expr(edge->expr));
     }
}

void merge_node(Widget w, Draw_Data *dd, OG *new_node, OG *og2)
{
     Edge *edge;
     PString text;

     Node *old_node = og2->u.gnode->node;

     sl_loop_through_slist(old_node->in, edge, Edge *) {
	  dd->list_knot = sl_make_slist();
	  NEWSTR(edge->og->u.gedge->text->u.gedge_text->log_string, text);
	  create_edge(dd->canvas, dd, edge->in->og ,new_node, text,
		      edge->og->u.gedge->text->u.gedge_text->text_width,
		      edge->og->u.gedge->text->u.gedge_text->fill_lines,
		      edge->og->type, edge->type, dd->list_knot, copy_expr(edge->expr));
     }

     sl_loop_through_slist(old_node->out, edge, Edge *) {
	  dd->list_knot = sl_make_slist();
	  NEWSTR(edge->og->u.gedge->text->u.gedge_text->log_string, text);
	  create_edge(dd->canvas, dd, new_node, edge->out->og, text,
		      edge->og->u.gedge->text->u.gedge_text->text_width,
		      edge->og->u.gedge->text->u.gedge_text->fill_lines,
		      edge->og->type, edge->type, dd->list_knot, copy_expr(edge->expr));
     }
     
     destroy_og(dd,og2);
}

typedef enum {ANY,VERT,HOR} Dir_Type;

void align_og(Draw_Data *dd, OG *og, Dir_Type direction)
{
     OG *algn_og = dd->og_align;
     Widget w = dd->canvas;
     Edge *edge;
     XRectangle rect;

     erase_og(w, dd, og);
     switch (og->type) {
     case DT_IF_NODE:
	  erase_og(w,dd,else_og_from_if_og(og));
	  sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
	  erase_og(w,dd,then_og_from_if_og(og));
	  sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
	  sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	       if (edge->og) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	  }
	  erase_og(w,dd,then_og_from_if_og(og));
	  sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	       if (edge->og) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	  }
	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
	  switch (direction) {
	  case ANY:
	       if ((ABS(algn_og->x - og->x)) > (ABS(algn_og->y - og->y))) {
		    og->y = algn_og->y + (int)(algn_og->height - og->height)/2;

	       } else {
		    og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
	       }
	       break;
	  case VERT:
	       og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
	       break;
	  case HOR:
	       og->y = algn_og->y + (int)(algn_og->height - og->height)/2;
	       break;
	  }
	  break;
     case DT_NODE:
	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
	  sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	       erase_og(w, dd, edge->og);
	       erase_og(w, dd, edge->og->u.gedge->text);
	  }
	  switch (direction) {
	  case ANY:
	       if ((ABS(algn_og->x - og->x)) > (ABS(algn_og->y - og->y))) {
		    og->y = algn_og->y + (int)(algn_og->height - og->height)/2;

	       } else {
		    og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
	       }
	       break;
	  case VERT:
	       og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
	       break;
	  case HOR:
	       og->y = algn_og->y + (int)(algn_og->height - og->height)/2;
	       break;
	  }
	  break;
     case DT_KNOT:
	  erase_og(w, dd, og->u.gknot->edge->u.gedge->text);

	  switch (direction) {
	  case ANY:
	       if ((ABS(algn_og->x - og->x)) > (ABS(algn_og->y - og->y))) {
		    og->y = algn_og->y + (int)(algn_og->height - og->height)/2;
		    og->u.gknot->y =  og->y + (KNOT_SIZE / 2);
	       } else {
		    og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
		    og->u.gknot->x =  og->x + (KNOT_SIZE / 2);
	       }
	       break;
	  case VERT:
	       og->x = algn_og->x + (int)(algn_og->width - og->width)/2;
	       og->u.gknot->x =  og->x + (KNOT_SIZE / 2);
	       break;
	  case HOR:
	       og->y = algn_og->y + (int)(algn_og->height - og->height)/2;
	       og->u.gknot->y =  og->y + (KNOT_SIZE / 2);
	       break;
	  }
	  break;
     case DT_EDGE_TEXT: {
	  int x = og->x;
	  int y = og->y;

	  switch (direction) {
	  case ANY:
	       if ((ABS(algn_og->x - og->x)) > (ABS(algn_og->y - og->y)))
		    y = algn_og->y;
	       else
		    x = algn_og->x;
	       break;
	  case VERT:
	       x = algn_og->x;
	       break;
	  case HOR:
	       y = algn_og->y;
	       break;
	  }
	  reposition_edge_text(og, x, y);
	  og->x = x;
	  og->y = y;
     }
	  break;
     default: 	 
	  switch (direction) {
	  case ANY:
	       if ((ABS(algn_og->x - og->x)) > (ABS(algn_og->y - og->y)))
		    og->y = algn_og->y;
	       else
		    og->x = algn_og->x;
	       break;
	  case VERT:
	       og->x = algn_og->x;
	       break;
	  case HOR:
	       og->y = algn_og->y;
	       break;
	  }
	  break;
     }

     rect.x = og->x;
     rect.y = og->y;
     rect.width = og->width;
     rect.height = og->height;
     XDestroyRegion(og->region);
     og->region = XCreateRegion();
     XUnionRectWithRegion(&rect, og->region, og->region);
     switch (og->type) {
     case DT_IF_NODE:
	  position_then_else(og,  og->x,  og->y);
	  sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
	  sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
	  sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	       if (edge->og) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	  }
	  sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
	       if (edge->og) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	  }
	  draw_og(w,dd,else_og_from_if_og(og));
	  draw_og(w,dd,then_og_from_if_og(og));
	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
	  draw_og(w, dd, og);
	  break;
     case DT_NODE:
	  sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
	  sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
	       position_edge(edge->og);
	       draw_og(w, dd, edge->og);
	       draw_og(w, dd, edge->og->u.gedge->text);
	  }
	  draw_og(w, dd, og);
	  break;
     case DT_KNOT:
	  position_edge(og->u.gknot->edge);
	  draw_og(w, dd, og->u.gknot->edge);
	  draw_og(w, dd, og->u.gknot->edge->u.gedge->text);
	  break;
     default:
	  draw_og(w, dd, og);
	  break;
     }
}

void draw_moving_clip_box(Widget w, Draw_Data *dd, OG *og)
{
     Display *dpy = XtDisplay(w);
     Window win = dd->window;

     XDrawRectangle(dpy, win, dd->xorgc,
		    dd->start_x - dd->left - dd->dx,
		    dd->start_y - dd->top - dd->dy,
		    og->width - 1, og->height - 1);
}

void check_sensitive_item(Widget w, Draw_Data *dd, XEvent *event)
{

}

void canvas_mouse_motion(Widget w, Draw_Data *dd, XEvent *event)
{
     XRectangle rect;
     OG *og = dd->og_moving;
     Edge *edge;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;
     /* int button = event->xbutton.button; */

     x = MAX(10,x);
     y = MAX(10,y);

     x = MIN((int)dd->work_width - 10,x);
     y = MIN((int)dd->work_height - 10,y);

     switch (dd->mode) {
     case MOVING_OG:
	  erase_og(w, dd, og);
	  switch (og->type) {
	  case DT_IF_NODE:
	       erase_og(w,dd,else_og_from_if_og(og));
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 erase_og(w, dd, edge->og);
			 erase_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       erase_og(w,dd,then_og_from_if_og(og));
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 erase_og(w, dd, edge->og);
			 erase_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       break;
	  case DT_NODE:
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       break;
	  case DT_EDGE_TEXT:
	       og->u.gedge_text->edge->u.gedge->edge->in->og->selected = FALSE;
	       draw_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->in->og);
	       og->u.gedge_text->edge->u.gedge->edge->out->og->selected = FALSE;
	       draw_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->out->og);
	       reposition_edge_text(og, x - dd->dx, y - dd->dy);
	       break;
	  case DT_KNOT:
	       erase_og(w, dd, og->u.gknot->edge->u.gedge->text);
	       og->u.gknot->x = x - dd->dx + (KNOT_SIZE / 2);
	       og->u.gknot->y = y - dd->dy + (KNOT_SIZE / 2);
	       break;
	  default: 	 
	       break;
	  }
	  rect.x = og->x = x - dd->dx;
	  rect.y = og->y = y - dd->dy;
	  rect.width = og->width;
	  rect.height = og->height;
	  XDestroyRegion(og->region);
	  og->region = XCreateRegion();
	  XUnionRectWithRegion(&rect, og->region, og->region);
	  switch (og->type) {
	  case DT_IF_NODE:
	       position_then_else(og,  og->x,  og->y);
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 position_edge(edge->og);
			 draw_og(w, dd, edge->og);
			 draw_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 position_edge(edge->og);
			 draw_og(w, dd, edge->og);
			 draw_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       draw_og(w,dd,else_og_from_if_og(og));
	       draw_og(w,dd,then_og_from_if_og(og));
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       draw_og(w, dd, og);
	       break;
	  case DT_NODE:
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       draw_og(w, dd, og);
	       break;
	  case DT_KNOT:
	       position_edge(og->u.gknot->edge);
	       draw_og(w, dd, og->u.gknot->edge);
	       draw_og(w, dd, og->u.gknot->edge->u.gedge->text);
	       break;
	  default:
	       draw_og(w, dd, og);
	       break;
	  }
	  break;
     case MOVING_CANVAS:
     {
	  int xx = event->xbutton.x;
	  int yy = event->xbutton.y;
	  int dx = xx - dd->start_x;
	  int dy = yy - dd->start_y;

	  if (((dd->left == 0) && dx > 0)|| ((dd->left == dd->work_width - dd->canvas_width) && dx < 0))
	       dx = 0;
	  else 
	       dd->start_x = xx;

	  if (((dd->top == 0) && dy > 0)|| ((dd->top == dd->work_height - dd->canvas_height) && dy < 0))
	       dy = 0;
	  else 
	       dd->start_y = yy;

/* 	  set_draw_mode(dd, MOVE_OG); */
	  set_canvas_view_rel(dd, dx, dy);
	  break;
     }
     default:
	  break;
     }

#ifdef FELIX
     Window root,child;
     int rx,ry,wx,wy;
     unsigned int mask;

     unsigned int button_mask =  event->xbutton.state;
	  
     if (dd->mode == MOVING_OG) {
	  

	  int dx = 0;
	  int dy = 0;

	  int x = event->xbutton.x + dd->left;
	  int y = event->xbutton.y + dd->top;

/*
	  switch (event->xbutton.button) {
	  case Button1:
	       button_mask = Button1Mask;
	       break;
	  case Button2:
	       button_mask = Button2Mask;
	       break;
	  default:
	       fprintf(stderr, LG_STR("Moving with an unknown button combination...\n",
	       "Moving with an unknown button combination...\n"));
	       return;
	       break;
	  }
*/	  
	  x = MAX(10,x);
	  y = MAX(10,y);

	  x = MIN((int)dd->work_width - 10,x);
	  y = MIN((int)dd->work_height - 10,y);
	  
	  while (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,&rx,&ry,
			&wx,&wy,&mask) &&
		 (mask == button_mask) &&
		 ((wx < 0) || (wx > (int)dd->canvas_width) || 
		  (wy < 0) || (wy > (int)dd->canvas_height))) {

	       dx = dy = 0;

	       if (wx < 0) {
		    dx = - wx;
	       } else if (wx > (int)dd->canvas_width) {
		    dx =  dd->canvas_width - wx;
	       }
	       if (wy < 0) {
		    dy = - wy;
	       } else if (wy > (int)dd->canvas_height) {
		    dy =  dd->canvas_height - wy;
	       }

	       draw_moving_clip_box(w, dd, dd->og_moving);

	       set_canvas_view_rel(dd, dx, dy);
	       
	       x = MAX(10,wx + dd->left);
	       y = MAX(10,wy + dd->top);

	       x = MIN((int)dd->work_width - 10, x);
	       y = MIN((int)dd->work_height - 10, y);

	       dd->last_x = dd->start_x;
	       dd->start_x = x;
	       dd->last_y = dd->start_y;
	       dd->start_y = y;

	       draw_moving_clip_box(w, dd, dd->og_moving);

	  }

	  draw_moving_clip_box(w, dd, dd->og_moving);

	  dd->last_x = dd->start_x;
	  dd->start_x = x;
	  dd->last_y = dd->start_y;
	  dd->start_y = y;

	  draw_moving_clip_box(w, dd, dd->og_moving);

     } else if (dd->mode == MOVING_CANVAS) {
/*
	  int x = event->xbutton.x;
	  int y = event->xbutton.y;
	  int dx = x - dd->start_x;
	  int dy = y - dd->start_y;
*/
	  int x, y, dx, dy;

	  if ( !(XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
			       &rx,&ry,&x,&y,&mask) &&
	      (mask == button_mask))) {
	       x = event->xbutton.x;
	       y = event->xbutton.y;
	  }

	  dx = x - dd->start_x;
	  dy = y - dd->start_y;

	  if (((dd->left == 0) && dx > 0) || ((dd->left == dd->work_width - dd->canvas_width) && dx < 0))
	       dx = 0;
	  else 
	       dd->start_x = x; 

/*
 {
	       if (dx > 0 && dx > dd->left)
		    dd->start_x = x - dx - dd->left ;
	       else if  (dx < 0 && dx < dd->canvas_width + dd->left - dd->work_width)
		    dd->start_x = x +  dd->canvas_width + dd->left - dd->work_width - dx;
	       else 
	  }
*/

	  if (((dd->top == 0) && dy > 0) || ((dd->top == dd->work_height - dd->canvas_height) && dy < 0))
	       dy = 0;
	  else 
	       dd->start_y = y;
	  
	  if (! (dx == 0 && dy == 0))
	       set_canvas_view_rel(dd, dx, dy);
     } else {
	  /* code for sensitive item */
	  check_sensitive_item(w, dd,event);
     }
#endif

}

void mouse_release_move(Widget w, Draw_Data *dd, XEvent *event)
{
     XRectangle rect;
     OG *og = dd->og_moving;
     Edge *edge;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;
     /* int button = event->xbutton.button; */

     x = MAX(10,x);
     y = MAX(10,y);

     x = MIN((int)dd->work_width - 10,x);
     y = MIN((int)dd->work_height - 10,y);

     switch (dd->mode) {
     case MOVING_OG:
	  report_opfile_modification();
	  set_draw_mode(dd, MOVE_OG);
/* 	  draw_moving_clip_box(w, dd, og); */
	  og->selected = FALSE;
	  erase_og(w, dd, og);
	  switch (og->type) {
	  case DT_IF_NODE:
	       erase_og(w,dd,else_og_from_if_og(og));
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 erase_og(w, dd, edge->og);
			 erase_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       erase_og(w,dd,then_og_from_if_og(og));
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 erase_og(w, dd, edge->og);
			 erase_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       break;
	  case DT_NODE:
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
		    erase_og(w, dd, edge->og);
		    erase_og(w, dd, edge->og->u.gedge->text);
	       }
	       break;
	  case DT_EDGE_TEXT:
	       og->u.gedge_text->edge->u.gedge->edge->in->og->selected = FALSE;
	       erase_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->in->og);
	       og->u.gedge_text->edge->u.gedge->edge->out->og->selected = FALSE;
	       erase_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->out->og);
	       reposition_edge_text(og, x - dd->dx, y - dd->dy);
	       break;
	  case DT_KNOT:
	       erase_og(w, dd, og->u.gknot->edge->u.gedge->text);
	       og->u.gknot->x = x - dd->dx + (KNOT_SIZE / 2);
	       og->u.gknot->y = y - dd->dy + (KNOT_SIZE / 2);
	       break;
	  default: 	 
	       break;
	  }
	  rect.x = og->x = x - dd->dx;
	  rect.y = og->y = y - dd->dy;
	  rect.width = og->width;
	  rect.height = og->height;
	  XDestroyRegion(og->region);
	  og->region = XCreateRegion();
	  XUnionRectWithRegion(&rect, og->region, og->region);
	  switch (og->type) {
	  case DT_IF_NODE:
	       position_then_else(og,  og->x,  og->y);
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(then_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 position_edge(edge->og);
			 draw_og(w, dd, edge->og);
			 draw_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       sl_loop_through_slist(else_og_from_if_og(og)->u.gnode->node->in, edge, Edge *) {
		    if (edge->og) {
			 position_edge(edge->og);
			 draw_og(w, dd, edge->og);
			 draw_og(w, dd, edge->og->u.gedge->text);
		    }
	       }
	       draw_og(w,dd,else_og_from_if_og(og));
	       draw_og(w,dd,then_og_from_if_og(og));
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       draw_og(w, dd, og);
	       break;
	  case DT_NODE:
	       sl_loop_through_slist(og->u.gnode->node->in, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       sl_loop_through_slist(og->u.gnode->node->out, edge, Edge *) {
		    position_edge(edge->og);
		    draw_og(w, dd, edge->og);
		    draw_og(w, dd, edge->og->u.gedge->text);
	       }
	       draw_og(w, dd, og);
	       break;
	  case DT_KNOT:
	       position_edge(og->u.gknot->edge);
	       draw_og(w, dd, og->u.gknot->edge);
	       draw_og(w, dd, og->u.gknot->edge->u.gedge->text);
	       break;
	  default:
	       draw_og(w, dd, og);
	       break;
	  }
	  break;
     case MOVING_CANVAS:
     {
	  int xx = event->xbutton.x;
	  int yy = event->xbutton.y;
	  int dx = xx - dd->start_x;
	  int dy = yy - dd->start_y;

	  if (((dd->left == 0) && dx > 0)|| ((dd->left == dd->work_width - dd->canvas_width) && dx < 0))
	       dx = 0;
	  else 
	       dd->start_x = xx;

	  if (((dd->top == 0) && dy > 0)|| ((dd->top == dd->work_height - dd->canvas_height) && dy < 0))
	       dy = 0;
	  else 
	       dd->start_y = yy;

	  set_draw_mode(dd, MOVE_OG);
	  set_canvas_view_rel(dd, dx, dy);
	  break;
     }
     default:
	  break;
     }
}

void canvas_mouse_release(Widget w, Draw_Data *dd, XEvent *event)
{
     OG *og = dd->og_moving;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;

#ifdef FELIX
     if (button != dd->pressed_button) return;
#endif

     if (dd->pressed_button == Button2) {
	  mouse_release_move(w, dd, event);
	  set_draw_mode(dd, dd->old_mode);
     } else {
	  switch (dd->mode) {
	  case ALIGNING_OG:
	  case ALIGNING_OG_H:
	  case ALIGNING_OG_V:
	       report_opfile_modification();
	       if (dd->og_aligning) {
		    dd->og_aligning->selected = FALSE;
		    draw_og(dd->canvas, dd, dd->og_aligning);
	       }
	       sl_loop_through_slist(dd->op->list_movable_og, og, OG *)
		    if (XPointInRegion(og->region, x, y)) {
			 if (dd->og_aligning == og) {
			      if ((og->type == DT_THEN_NODE) || (og->type == DT_ELSE_NODE))
				   og = if_og_from_t_or_f_og(dd->og_aligning);
			      switch (dd->mode) {
			      case ALIGNING_OG:
				   align_og(dd, og, ANY);
				   break;
			      case ALIGNING_OG_H:
				   align_og(dd, og, HOR);
				   break;
			      case ALIGNING_OG_V:
				   align_og(dd, og, VERT);
				   break;
			      default:
				   break;
			      }
			      break;
			 }
		    }
	       dd->og_aligning = NULL;
	       break;
	  case MOVING_OG:
	  case MOVING_CANVAS:
	       mouse_release_move(w, dd, event);
	       break;
	  case DM_RELEVANT_OP:
	       if (dd->og_selected_on_press) {
		    draw_clip_box(w, dd, dd->og_selected_on_press);
		    if (XPointInRegion(dd->og_selected_on_press->region, x, y)) {
			 if(dd->og_selected_on_press->type == DT_EDGE_TEXT)
			      ope_find_rel_ops(dd->og_selected_on_press->u.gedge_text->edge->u.gedge->edge);
			 else if (dd->og_selected_on_press->type == DT_INST)
			      ope_find_rel_ops_inst(dd->og_selected_on_press->u.ginst->inst);
			 else if(dd->og_selected_on_press->type == DT_TEXT)
			      if (dd->og_selected_on_press->u.gtext->text_type == TT_INVOCATION)
				   ope_find_rel_ops_gmexpr_except_me(current_op->invocation, current_op);
		/* 	      else if (dd->og_selected_on_press->u.gtext->text_type == TT_EFFECTS) */
/* 				   ope_find_rel_ops_list_expr(current_op->effects); */
			      else
				   fprintf(stderr, LG_STR("This graphic type has no expr.\n",
							  "This graphic type has no expr.\n"));
			 else 
			      fprintf(stderr, LG_STR("This graphic type has no expr.\n",
						     "This graphic type has no expr.\n"));
			      
		    }
		    dd->og_selected_on_press = NULL;
	       }
	       break;
	  case DESTROY_OG:
	       if (dd->og_selected_on_press) {
		    if ((dd->og_selected_on_press->type == DT_THEN_NODE) || (dd->og_selected_on_press->type == DT_ELSE_NODE)) 
			 draw_clip_box(w, dd, if_og_from_t_or_f_og(dd->og_selected_on_press));
		    else
			 draw_clip_box(w, dd, dd->og_selected_on_press);
		    if (XPointInRegion(dd->og_selected_on_press->region, x, y)) {
			 report_opfile_modification();
			 if ((dd->og_selected_on_press->type == DT_THEN_NODE) || (dd->og_selected_on_press->type == DT_ELSE_NODE)) 
			      destroy_og(dd, if_og_from_t_or_f_og(dd->og_selected_on_press));
			 else
			      destroy_og(dd, dd->og_selected_on_press);
		    }
	       }
	       dd->og_selected_on_press = NULL;
	       break;
	  default: 	 
	       break;
	  }
     }
#ifdef FELIX
     dd->pressed_button = 0;
#endif
}

char *get_editable_og_string(OG *og)
{
     switch (og->type) {
	  case DT_EDGE_TEXT:
	  return og->u.gedge_text->log_string;
	  break;
     case DT_TEXT:
	  return og->u.gtext->string;
	  break;
     default:
	  fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
				 "This graphic type has no editable string...\n"));
	  return NULL;
	  break;
     }
}

int get_editable_og_width(OG *og)
{
     switch (og->type) {
	  case DT_EDGE_TEXT:
	  return og->u.gedge_text->text_width;
	  break;
     case DT_TEXT:
	  return og->u.gtext->text_width;
	  break;
     default:
	  fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
				 "This graphic type has no editable string...\n"));
	  return 0;
	  break;
     }
}

PBoolean get_editable_og_fill_lines(OG *og)
{
     switch (og->type) {
     case DT_EDGE_TEXT:
	  return og->u.gedge_text->fill_lines;
	  break;
     case DT_TEXT:
	  return og->u.gtext->fill_lines;
	  break;
     default:
	  fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
				 "This graphic type has no editable string...\n"));
	  return 0;
	  break;
     }
}

void set_editable_og_width(OG *og, int width)
{
     switch (og->type) {
	  case DT_EDGE_TEXT:
	  og->u.gedge_text->text_width = width;
	  break;
     case DT_TEXT:
	  og->u.gtext->text_width = width;
	  break;
     default:
	  fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
				 "This graphic type has no editable string...\n"));
	  break;
     }
}

void set_editable_og_fill_lines(OG *og, PBoolean fill_lines)
{
     switch (og->type) {
	  case DT_EDGE_TEXT:
	  og->u.gedge_text->fill_lines = fill_lines;
	  break;
     case DT_TEXT:
	  og->u.gtext->fill_lines = fill_lines;
	  break;
     default:
	  fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
				 "This graphic type has no editable string...\n"));
	  break;
     }
}

void mouse_press_edit(Widget w, Draw_Data *dd, XEvent *event)
{
     OG *og;
     char *string;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;

     sl_loop_through_slist(dd->op->list_editable_og, og, OG *)
	  if (XPointInRegion(og->region, x, y)) {
	       Arg arg[1];
	       char s[LINSIZ];

	       if((og->type == DT_EDGE_TEXT) || (og->type == DT_TEXT))  {
		    PBoolean line_filling = get_editable_og_fill_lines(og);

		    dd->edited_og = og;
		    XtSetArg(arg[0], XmNuserData, dd);
		    XtSetValues(editObjectStruct->editObjectForm, arg, 1);
		    string = get_editable_og_string(og);
		    XmTextSetString(editObjectStruct->text, string);
		    sprintf (s,"%d", get_editable_og_width(og));
		    XmTextFieldSetString(editObjectStruct->width, s);
	       
		    XmToggleButtonSetState(editObjectStruct->fill_true, 
					   line_filling, False);
		    XmToggleButtonSetState(editObjectStruct->fill_false, 
					   (line_filling ? False : True), False);
		    XtManageChild(editObjectStruct->editObjectForm);
		    return;
	       } else if (og->type == DT_NODE) {
		    Node *node = og->u.gnode->node;
		    
		    if ((node->type != NT_PROCESS) && (node->type != NT_END)) {
			 fprintf(stderr,LG_STR("Uneditable Node type: %s in editNodeName.\n",
					       "Uneditable Node type: %s in editNodeName.\n"),node->name);
			 return;
		    }
		    dd->edited_og = og;
 		    XmTextSetString(XmSelectionBoxGetChild(editNodeNameDialog,XmDIALOG_TEXT),
				    node->name);
		    XtManageChild(editNodeNameDialog);
		    return;
	       } else if ((og->type == DT_IF_NODE) ||
			  (og->type == DT_THEN_NODE) || 
			  (og->type == DT_ELSE_NODE)) {
		    Node *node;
		    
		    if ((og->type == DT_THEN_NODE) || (og->type == DT_ELSE_NODE)) 
			 dd->edited_og = if_og_from_t_or_f_og(og);
		    else
			 dd->edited_og = og;
		    node = dd->edited_og->u.gnode->node;
 		    XmTextSetString(XmSelectionBoxGetChild(editNodeNameDialog,XmDIALOG_TEXT),
				    node->name);
		    XtManageChild(editNodeNameDialog);
		    return;
	       } else
		    fprintf(stderr, LG_STR("This graphic type has no editable string...\n",
					   "This graphic type has no editable string...\n"));
	  }
}

void mouse_press_move(Widget w, Draw_Data *dd, XEvent *event)
{
     OG *og;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;

     if (dd->op->graphic) {
	  sl_loop_through_slist(dd->op->list_movable_og, og, OG *)
	       if (XPointInRegion(og->region, x, y)) {
		    set_draw_mode(dd, MOVING_OG);
		    if ((og->type == DT_THEN_NODE) || (og->type == DT_ELSE_NODE)) 
			 dd->og_moving = if_og_from_t_or_f_og(og);
		    else
			 dd->og_moving = og;
		    dd->last_x = dd->start_x = x;
		    dd->last_y = dd->start_y = y;
		    dd->dx = x - dd->og_moving->x;
		    dd->dy = y - dd->og_moving->y;
		    dd->og_moving->selected = TRUE;
		    draw_og(w, dd, dd->og_moving);
/* 		    draw_moving_clip_box(w, dd, dd->og_moving); */
		    if (og->type == DT_EDGE_TEXT) {
			 og->u.gedge_text->edge->u.gedge->edge->in->og->selected = TRUE;
			 draw_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->in->og);
			 og->u.gedge_text->edge->u.gedge->edge->out->og->selected = TRUE;
			 draw_og(w, dd, og->u.gedge_text->edge->u.gedge->edge->out->og);
		    }
		    return;
	       }
     }
     set_draw_mode(dd, MOVING_CANVAS);
     dd->start_x = event->xbutton.x;
     dd->start_y = event->xbutton.y;
}

void canvas_mouse_press(Widget w, Draw_Data *dd, XEvent *event)
{
     OG *og;
     int x = event->xbutton.x + dd->left;
     int y = event->xbutton.y + dd->top;
     int button = event->xbutton.button;

#ifdef FELIX
     if (dd->pressed_button)
	  return;
     else 
	  dd->pressed_button = button;
#endif
     dd->pressed_button = button;

     if (dd->op && current_op) {
	  if (button == Button2) {
	       dd->old_mode = dd->mode;
	       mouse_press_move(w, dd, event);
	  } else if ((button == Button3) &&
		     (dd->mode !=  ALIGNING_OG) &&
		     (dd->mode !=  ALIGNING_OG_V) &&
		     (dd->mode !=  ALIGNING_OG_H) &&
		     (dd->mode !=  NODE_SELECTED) &&
		     (dd->mode !=  DUPLICATING_EDGE) &&
		     (dd->mode !=  DUPLICATING_NODE) &&
		     (dd->mode !=  DUPLICATING_EDGE2) &&
		     (dd->mode !=  MERGING_NODE) &&
		     (dd->mode !=  EDGE_SELECTED)) {
	       mouse_press_edit(w, dd, event);
	  } else {
	       switch (dd->mode) {
	       case ALIGNING_OG:
	       case ALIGNING_OG_V:
	       case ALIGNING_OG_H:
		    if (button == Button3) {
			 dd->og_align->selected = FALSE;
			 draw_og(dd->canvas, dd, dd->og_align);
			 switch (dd->mode) {
			 case ALIGNING_OG:
			      set_draw_mode(dd, ALIGN_OG);
			      break;
			 case ALIGNING_OG_V:
			      set_draw_mode(dd, ALIGN_OG_V);
			      break;
			 case ALIGNING_OG_H:
			      set_draw_mode(dd, ALIGN_OG_H);
			      break;
			 default:
			      break;
			 }
			 dd->og_align = NULL;
			 return;
		    }
		    sl_loop_through_slist(dd->op->list_movable_og, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      dd->og_aligning = og;
			      dd->og_align->selected = TRUE;
			      draw_og(dd->canvas, dd, dd->og_align);
			      return;
			 }
		    break;
	       case ALIGN_OG:
	       case ALIGN_OG_V:
	       case ALIGN_OG_H:
		    if (dd->op->graphic) {
			 sl_loop_through_slist(dd->op->list_movable_og, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   switch (dd->mode) {
				   case ALIGN_OG:
					set_draw_mode(dd, ALIGNING_OG);
					break;
				   case ALIGN_OG_V:
					set_draw_mode(dd, ALIGNING_OG_V);
					break;
				   case ALIGN_OG_H:
					set_draw_mode(dd, ALIGNING_OG_H);
					break;
				   default:
					break;
				   }
				   if ((og->type == DT_THEN_NODE) || (og->type == DT_ELSE_NODE))
					dd->og_align =  if_og_from_t_or_f_og(og);
				   else 
					dd->og_align = og;
				   dd->og_align->selected = TRUE;
				   draw_og(dd->canvas, dd, dd->og_align);
				   return;
			      }
		    } else 
			 ope_information_report(LG_STR("This operation is not allowed on Text OP.",
						       "This operation is not allowed on Text OP."));
		    break;
	       case MOVE_OG:
			 mouse_press_move(w, dd, event);
		    break;
	       case EDIT_OG:
		    mouse_press_edit(w, dd, event);
		    break;
	       case DESTROY_OG:
		    sl_loop_through_slist(dd->op->list_destroyable_og, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      dd->og_selected_on_press = og;
			      if ((og->type == DT_THEN_NODE) || (og->type == DT_ELSE_NODE)) 
				   draw_clip_box(w, dd, if_og_from_t_or_f_og(og));
			      else
				   draw_clip_box(w, dd, og);
			      return;
			 }
		    break;
	       case DM_RELEVANT_OP:
		    sl_loop_through_slist(dd->op->list_og_edge_text, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      dd->og_selected_on_press = og;
			      draw_clip_box(w, dd, og);
			      return;
			 }
		    sl_loop_through_slist(dd->op->list_og_inst, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      dd->og_selected_on_press = og;
			      draw_clip_box(w, dd, og);
			      return;
			 }
		    if (XPointInRegion(dd->op->ginvocation->region, x, y)) {
			 dd->og_selected_on_press = dd->op->ginvocation;
			 draw_clip_box(w, dd, dd->op->ginvocation);
			 return;
		    }
		    if (XPointInRegion(dd->op->geffects->region, x, y)) {
			 dd->og_selected_on_press = dd->op->geffects;
			 draw_clip_box(w, dd, dd->op->geffects);
			 return;
		    }

		    break;
	       case CONVERT_END:
		    if (!(dd->op->gaction || dd->op->gbody ))
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   report_opfile_modification();
				   convert_end(w, dd, og);
				   return;
			      }
		    break;
	       case CONVERT_START:
		    if (!(dd->op->gaction || dd->op->gbody))
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   convert_start(w, dd, og);
				   return;
			      }
		    break;
	       case FLIP_SPLIT:
		    sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      flip_split(w, dd, og);
			      return;
			 }
		    break;
	       case FLIP_JOIN:
		    sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      flip_join(w, dd, og);
			      return;
			 }
		    break;
	       case OPEN_NODE:
		    sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			 if (XPointInRegion(og->region, x, y)) {
			      open_node(w, dd, og);
			      return;
			 }
		    break;
	       case DRAW_NODE:
		    if (!(dd->op->gaction || dd->op->gbody)) {
			 report_opfile_modification();
			 create_node(w, x, y, dd, FALSE, FALSE);
		    }
		    break;
	       case DRAW_IFNODE:
		    if (!(dd->op->gaction || dd->op->gbody)) {
			 report_opfile_modification();
			 create_ifnode(w, x, y, dd);
		    }
		    break;
	       case DRAW_EDGE:
		    if (!(dd->op->gaction || dd->op->gbody))
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   if (og->u.gnode->node->type == NT_END) {
					report_user_error(LG_STR("This is an End node, convert it first.",
								 "This is an End node, convert it first."));
				   } else if (og->type == DT_IF_NODE) {
					report_user_error(LG_STR("This is an IF node... No outgoing edges allowed.",
								 "This is an IF node... No outgoing edges allowed."));
				   } else {
					report_opfile_modification();
					set_draw_mode(dd, NODE_SELECTED);
					dd->node_selected = og;
					dd->list_knot = sl_make_slist();
					og->selected = TRUE;
					draw_og(w, dd, og);
				   }
				   return;
			      }
		    break;
	       case NODE_SELECTED:
	       {
		    PBoolean node = FALSE;

		    if (button != Button3) {
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   Arg arg[1];
				   
				   dd->second_node_selected = og;
				   XtSetArg(arg[0], XmNuserData, dd);
				   XtSetValues(createEdgeStruct->createEdgeForm, arg, 1);
				   node = TRUE;
				   XtManageChild(createEdgeStruct->createEdgeForm);
				   break; /* We need to break because there may be a recursive loop_though_list... */

			      }
			 if (!node) {
			      if (sl_slist_length(dd->list_knot) < MAX_KNOT) {
				   Gknot *gknot = MAKE_OBJECT(Gknot);
				   OG *og_tmp = MAKE_OBJECT(OG);

				   og_tmp->type = DT_KNOT;
				   og_tmp->u.gknot = gknot;
				   og_tmp->selected = FALSE; /* even if the selected field is not used yet !*/
				   gknot->x = x;
				   gknot->y = y;
				   sl_add_to_tail(dd->list_knot, og_tmp);
				/* All the other setting is done in the edge creation... */
			      } else {
				   report_user_error(LG_STR("You reach the Maximal Number of knots enabled per edge.",
							    "You reach the Maximal Number of knots enabled per edge."));
			      }
			 }
		    }
		    if (node || (button == Button3)) {
			 set_draw_mode(dd, DRAW_EDGE);
			 dd->node_selected->selected = FALSE;
			 draw_og(w, dd, dd->node_selected);
		    }
	       }
		    break;
	       case DRAW_KNOT:
		    if (!(dd->op->gaction || dd->op->gbody))
			 sl_loop_through_slist(dd->op->list_og_edge_text, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   set_draw_mode(dd, EDGE_SELECTED);
				   dd->edge_selected = og->u.gedge_text->edge;
				   og->selected = TRUE;
				   draw_og(w, dd, og);
				   return;
			      }
		    break;
	       case EDGE_SELECTED:
		    if (button != Button3) {   
			 if (sl_slist_length(dd->edge_selected->u.gedge->list_knot) < MAX_KNOT) {
			      int dx, dy;
			      long dist = -1, dist_tmp, dist_prev, dist_next;
			      int i = 0, j =0;
			      XRectangle rect;
			      Gknot *gknot = MAKE_OBJECT(Gknot);
			      OG *ogk_tmp;
			      OG *ogk = MAKE_OBJECT(OG);
		    
			      report_opfile_modification();

			      og = dd->edge_selected; 
			      ogk->type = DT_KNOT;
			      ogk->u.gknot = gknot;
			      ogk->selected = FALSE; /* even if yhe selected field is not used yet !*/
			      gknot->x = x;
			      gknot->y = y;
			      erase_og(w, dd, og);
			      erase_og(w, dd, og->u.gedge->text);
			      rect.x = ogk->x = ogk->u.gknot->x - (KNOT_SIZE / 2);
			      rect.y = ogk->y = ogk->u.gknot->y - (KNOT_SIZE / 2);
			      rect.width = rect.height = ogk->width = ogk->height = KNOT_SIZE;
			      ogk->region = XCreateRegion();
			      XUnionRectWithRegion(&rect, ogk->region, ogk->region);
			      ogk->u.gknot->edge = og;
			      sl_add_to_tail(dd->op->list_movable_og, ogk);
			      sl_add_to_tail(dd->op->list_destroyable_og, ogk);
		    
			      sl_loop_through_slist(dd->edge_selected->u.gedge->list_knot, ogk_tmp, OG *) {
				   dx = ogk_tmp->x - x;
				   dy = ogk_tmp->y - y;
				   dist_tmp = dx*dx + dy*dy;
				   i++;
				   if ((dist <0) ||  (dist_tmp < dist)) {
					dist = dist_tmp;
					j = i;
				   }
			      }
			      if (i == 0) { /* The knot_list is empty */
				   dist_prev = 0;

			      } else if (j == 1) { /* the new knot is near the first one */
				   dx = dd->edge_selected->u.gedge->edge->in->og->x -x;
				   dy = dd->edge_selected->u.gedge->edge->in->og->y -y;
				   dist_prev =  dx*dx + dy*dy;
				   
			      } else {
				   ogk_tmp = (OG *) sl_get_slist_pos(dd->edge_selected->u.gedge->list_knot,(j - 1));
				   dx = ogk_tmp->x - x;
				   dy = ogk_tmp->y - y;
				   dist_prev = dx*dx + dy*dy;
			      }
			      if (j == 0) { /* The knot_list is empty */
				   dist_next = 0;
			      } else if (i == j ) { /* the new knot is near the last one */
				   dx = dd->edge_selected->u.gedge->edge->out->og->x -x;
				   dy = dd->edge_selected->u.gedge->edge->out->og->y -y;
				   dist_next =  dx*dx + dy*dy;
			      } else {
				   ogk_tmp = (OG *) sl_get_slist_pos(dd->edge_selected->u.gedge->list_knot,(j +1));
				   dx = ogk_tmp->x - x;
				   dy = ogk_tmp->y - y;
				   dist_next = dx*dx + dy*dy;
			      }
			      if (dist_prev <= dist_next) { /* insert the knot before j */
				   sl_insert_slist_pos(dd->edge_selected->u.gedge->list_knot, ogk, j);

			      } else {  /* insert the knot after  j */
				   sl_insert_slist_pos(dd->edge_selected->u.gedge->list_knot, ogk, (j+1));
			      }
			      position_edge(ogk->u.gknot->edge);
			      draw_og(w, dd, ogk->u.gknot->edge);
			      draw_og(w, dd, ogk->u.gknot->edge->u.gedge->text);
			 } else {
			      report_user_error(LG_STR("You reach the Maximal Number of knots enabled per edge.",
						       "You reach the Maximal Number of knots enabled per edge."));
			 }
			 	       
		    } else {
			 set_draw_mode(dd, DRAW_KNOT);
			 dd->edge_selected->u.gedge->text->selected = FALSE;
			 draw_og(w, dd, dd->edge_selected);
			 draw_og(w, dd, dd->edge_selected->u.gedge->text);
			 dd->edge_selected = NULL;
		    }
		    break;
	       case DUPLICATING_EDGE:
		    if (button != Button3) {
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   if (og->u.gnode->node->type == NT_END) {
					report_user_error(LG_STR("This is an End node, convert it first.",
								 "This is an End node, convert it first."));
				   } else if (og->type == DT_IF_NODE) {
					report_user_error(LG_STR("This is an IF node... No outgoing edges allowed.",
								 "This is an IF node... No outgoing edges allowed."));
				   } else {
					set_draw_mode(dd, DUPLICATING_EDGE2);
					dd->node_selected = og;
					dd->list_knot = sl_make_slist();
					og->selected = TRUE;
					draw_og(w, dd, og);
				   }
				   return;
			      }
		    } else {
			 set_draw_mode(dd, DUPLICATE_OBJECTS);
			 dd->edge_selected->u.gedge->text->selected = FALSE;
			 draw_og(w, dd, dd->edge_selected);
			 draw_og(w, dd, dd->edge_selected->u.gedge->text);
			 dd->edge_selected = NULL;
		    }
		    break;
	       case DUPLICATING_EDGE2:
	       {
		    PBoolean node = FALSE;

		    if (button != Button3) {
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   Arg arg[2];

				   dd->second_node_selected = og;
				   XtSetArg(arg[0], XmNuserData, dd);
				   XtSetValues(createEdgeStruct->createEdgeForm, arg, 1);
				   XmTextSetString(createEdgeStruct->text, dd->edge_selected->u.gedge->text->u.gedge_text->log_string);
				   node = TRUE;
				   XtManageChild(createEdgeStruct->createEdgeForm);

				   break; /* We need to break because there may be a recursive loop_though_list... */

			      }
			 if (!node) {
			      if (sl_slist_length(dd->list_knot) < MAX_KNOT) {
				   Gknot *gknot = MAKE_OBJECT(Gknot);
				   OG *og_tmp = MAKE_OBJECT(OG);
				   
				   og_tmp->type = DT_KNOT;
				   og_tmp->u.gknot = gknot;
				   og_tmp->selected = FALSE; /* even if yhe selected field is not used yet !*/
				   gknot->x = x;
				   gknot->y = y;
				   sl_add_to_tail(dd->list_knot, og_tmp);
			      } else {
				   report_user_error(LG_STR("You reach the Maximal Number of knots enabled per edge.",
							    "You reach the Maximal Number of knots enabled per edge."));
			      }
			 }
		    }
		    if (node || (button == Button3)) {
			 set_draw_mode(dd, DUPLICATING_EDGE);
			 dd->node_selected->selected = FALSE;
			 draw_og(w, dd, dd->node_selected);
		    }
	       }
		    break;
	       case DUPLICATE_OBJECTS:
		    if (!(dd->op->gaction || dd->op->gbody)) {
			 sl_loop_through_slist(dd->op->list_og_edge_text, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   set_draw_mode(dd, DUPLICATING_EDGE);
				   dd->edge_selected = og->u.gedge_text->edge;
				   og->selected = TRUE;
				   draw_og(w, dd, og);
				   return;
			      }
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   if (IF_THEN_ELSE_NODE_P(og->u.gnode->node)) {
					report_user_error(LG_STR("Cannot duplicate an IF_THEN_ELSE node... Yet.",
								 "Cannot duplicate an IF_THEN_ELSE node... Yet."));
				   } else if (START_NODE_P(og->u.gnode->node)) {
					report_user_error(LG_STR("This is a Start node, convert it first.",
								 "This is a Start node, convert it first."));
				   } else {
					set_draw_mode(dd, DUPLICATING_NODE);
					dd->node_selected = og;
					og->selected = TRUE;
					draw_og(w, dd, og);
				   }
				   return;
			      }
		    }
		    break;
	       case DUPLICATING_NODE:
		    if (button != Button3) {
			 report_opfile_modification();
			 duplicate_node(w, x, y, dd, dd->node_selected->u.gnode->node);
		    } else {
			 set_draw_mode(dd, DUPLICATE_OBJECTS);
			 dd->node_selected->selected = FALSE;
			 draw_og(w, dd, dd->node_selected);
			 dd->node_selected = NULL;
		    }
		    break;
	       case MERGE_NODE:
		    if (!(dd->op->gaction || dd->op->gbody))
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y))
				  if (IF_THEN_ELSE_NODE_P(og->u.gnode->node)) {
				       report_user_error(LG_STR("Cannot merge an IF_THEN_ELSE node... Yet.",
								"Cannot merge an IF_THEN_ELSE node... Yet."));
				       return;
				  } else {
				       set_draw_mode(dd, MERGING_NODE);
				       dd->node_selected = og;
				       dd->list_knot = sl_make_slist();
				       og->selected = TRUE;
				       draw_og(w, dd, og);
				       return;
				  }
		    break;
	       case MERGING_NODE:
	       {
		    PBoolean node = FALSE;

		    if (button != Button3) {
			 sl_loop_through_slist(dd->op->list_og_node, og, OG *)
			      if (XPointInRegion(og->region, x, y)) {
				   
				   OG *og1 = dd->node_selected;
				   OG *og2 = og;
				   
				   if (check_merge_compatible(og1, og2)) {
					node = TRUE;
					report_opfile_modification();
					merge_node(w, dd, og1, og2);
				   }
				   break; /* We need to break because there is a recursive sl_loop_through_slist... */
			      }
		    }
		    if (node || (button == Button3)) {
			 set_draw_mode(dd, MERGE_NODE);
			 dd->node_selected->selected = FALSE;
			 draw_og(w, dd, dd->node_selected);
			 dd->node_selected = NULL;
		    }
	       }
		    break;
	       default:
		    fprintf(stderr, LG_STR("Unknown graphic mode.\n",
					   "Unknown graphic mode.\n"));
		    break;
	       }
	  }
     } else {
	  ope_information_report(LG_STR("You have to create or select a OP first.",
					"You have to create or select a OP first."));
     }
}
