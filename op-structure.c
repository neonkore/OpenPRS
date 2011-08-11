/*                               -*- Mode: C -*- 
 * op-structure.c -- Functions used for and with op-structure.
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

#include "config.h"

#include "stdio.h"


#ifdef GRAPHIX
#ifdef GTK
#include <gtk/gtk.h>
#include "xm2gtk.h"
#else
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#endif
#endif

#include "slistPack.h"
#include "slistPack_f.h"
#include "shashPack.h"
#include "shashPack_f.h"
#include "macro.h"
#include "constant.h"
#include "op-structure.h"

#ifdef GRAPHIX
#ifdef GTK
#include "gope-graphic.h"
#include "gope-global.h"
#include "gope-external_f.h"
#else
#include "ope-graphic.h"
#include "ope-global.h"
#include "ope-external_f.h"
#endif
#endif

#include "top-structure_f.h"
#include "op-structure_f.h"
#include "oprs-type_f.h"
#include "oprs-print.h"
#include "oprs-print_f.h"

#ifdef GRAPHIX
#ifdef GTK
#include "xm2gtk_f.h"
#endif
#endif

void free_list_og_inst(List_OG list_og_inst);

char *current_graph_file_name = NULL;
Slist *load_op_list = NULL;	/* liste of name of OPs to load, if applicable */

Op_Structure *goal_satisfied_in_db_op_ptr;
Op_Structure *goal_for_intention_op_ptr;
Op_Structure *goal_waiting_op_ptr;

static int edge_index = 1;
static int node_index = 1;

PBoolean compile_graphix = TRUE;

#ifdef GRAPHIX
Draw_Data *global_draw_data = NULL;

int  ip_x;
int  ip_y;
int  ip_width;
int  ctxt_x;
int  ctxt_y;
int  ctxt_width;
int  call_x;
int  call_y;
int  call_width;
int  set_x;
int  set_y;
int  set_width;
int  eff_x;
int  eff_y;
int  eff_width;
int  prop_x;
int  prop_y;
int  prop_width;
int  doc_x;
int  doc_y;
int  doc_width;
int  act_x;
int  act_y;
int  act_width;
int  bd_x;
int  bd_y;
int  bd_width;
int  edge_width;

int current_body_indent = 0;
int current_body_line = 0;
/* test for strings in body, TO DELETE
int nb_cr_in_strings = 0;
*/
#endif

Op_Structure *current_op;
List_OG current_list_og_inst;

Op_Structure *make_op()
{
     Op_Structure *t = MAKE_OBJECT(Op_Structure);

     t->start_point = (Control_Point *)NULL;
     t->invocation = (Expression *)NULL;
     t->call = (Expression *)NULL;
     t->context = NULL;
     t->setting = (Expression *)NULL;
     t->effects = NULL;
     t->action = (Action_Field *)NULL;
     t->body = (Body *)NULL;
     t->list_var = NULL; /* This is a leak... sl_make_slist(); */
     t->properties = sl_make_slist();
     t->node_list = sl_make_slist();
     t->edge_list = sl_make_slist();
     t->node_name_list = sl_make_slist();
     t->text_traced = FALSE;
     t->step_traced = FALSE;

#ifdef OPRS_PROFILING
     t->time_active = zero_date;
     t->nb_relevant = 0;
     t->nb_applicable = 0;
     t->nb_intended = 0;
     t->nb_succes = 0;
     t->nb_failure = 0;
#endif     

#ifdef GRAPHIX
     t->documentation = (PString)NULL;
     t->graphic = TRUE;
     t->graphic_traced = FALSE;
     t->op_title = NULL;
     t->ginvocation = NULL;
     t->gcall = NULL;
     t->gcontext = NULL;
     t->gsetting = NULL;
     t->geffects = NULL;
     t->gproperties = NULL;
     t->gdocumentation = NULL;
     t->gaction = NULL;
     t->gbody = NULL;

     t->xms_name = NULL;	/* Name in the various XmList of the interface. */

     t->list_og_node = sl_make_slist();
     t->list_og_edge = sl_make_slist();
     t->list_og_edge_text = sl_make_slist();
     t->list_og_text = sl_make_slist();
     t->list_og_inst = sl_make_slist();
     t->list_movable_og = sl_make_slist();
     t->list_destroyable_og = sl_make_slist();
     t->list_editable_og = sl_make_slist();
     t->last_view_x = t->last_view_y = 0;
#endif

     NEWSTR( (current_graph_file_name ? current_graph_file_name : "None"), t->file_name);
     edge_index = node_index = 1;

     return t;
}

void init_make_op(PString name, PBoolean graphic)
{
     Op_Structure *op;

     enable_variable_parsing();		/* will initialize current_var_list */
     op=make_op();
     op->name=name;

     op->list_var = dup_lenv(current_var_list); /* There was a dup_lenv here... a priori not needed  */
     current_op = op;

#ifdef GRAPHIX
     current_list_og_inst = op->list_og_inst;

     if (graphic) {
	  op->op_title = make_op_title(global_draw_data, name);
     }
#endif
}

void print_edge(Edge *e)
{
     printf(LG_STR("Edge %p",
		   "Arc %p"), e);
     print_expr(e->expr);
}

void print_edge_list(LogicList el)
{
     Edge *e;

     if (sl_slist_empty(el)) printf(LG_STR("(Edge List)NULL\n",
					   "(Liste Arc)NULL\n"));
     else {
	  sl_loop_through_slist(el, e, Edge *) {
	       print_edge(e); printf("\n");
	  }
     }
}

void print_node(Node *n)
{
     printf(LG_STR("Node %p",
		   "Noeud %p"), n);
     if (n) print_edge_list(n->out);
}


void print_op(Op_Structure *op)
{
#ifdef GRAPHIX
     Node *node;
#endif

     printf(LG_STR("OP %s\n",
		   "OP %s\n"), op->name);
     printf(LG_STR("File: %s\n",
		   "Fichier: %s\n"), op->file_name);
     printf(LG_STR("Starting node: ",
		   "Noeud Début: "));
     print_node(op->start_point);
     printf("\n");
     printf(LG_STR("Invocation: ",
		   "Invocation: "));
     print_expr(op->invocation);
     printf("\n");
     printf(LG_STR("Call: ",
		   "Call: "));
     print_expr(op->call);
     printf("\n");
     printf(LG_STR("Context: ",
		   "Contexte: "));
     print_exprlist(op->context);
     printf("\n");
     printf(LG_STR("Setting: ",
		   "Setting: "));
     print_expr(op->setting);
     printf("\n");
     printf(LG_STR("Effects: ",
		   "Effets: "));
     print_exprlist(op->effects);
     printf("\n");
#ifdef GRAPHIX
     printf(LG_STR("Documentation: ",
		   "Documentation: "));
     printf("%s\n", op->documentation);
#endif
     printf(LG_STR("Variables List: ",
		   "Liste Variables: "));
     print_lenv(op->list_var);
     printf("\n");
     printf(LG_STR("Properties List: ",
		   "Liste Propriétés: "));
     print_list_property(op->properties);
     printf("\n");
     if (op->action) {
	  printf(LG_STR("Action: ",
			"Action: "));
	  print_action_field(op->action);
	  printf("\n");
#ifdef GRAPHIX
     } else if (op->graphic) {
	  printf(LG_STR("with the node(s): ",
			"avec les noeuds: "));
	  sl_loop_through_slist(op->node_list, node, Node *) {
	       print_node(node);
	       printf("\n");
	  }
#endif
     } else if (op->body) {
	  printf(LG_STR("Body: ",
			"Corps: ")); print_body(op->body); printf("\n");
     }
}

#ifndef GRAPHIX
void register_node(Op_Structure *op, Symbol name, Node *n)
{
     Node_Name *nn = MAKE_OBJECT(Node_Name);

     nn->name = name;
     nn->node = n;
     sl_add_to_tail(op->node_name_list, nn);
}
#endif

Node *make_node(PString name, PBoolean graphic)
{
     Node *node = MAKE_OBJECT(Node);
     
#ifdef GRAPHIX
     node->name = name;
#else
     register_node(current_op,name, node);
#endif
     node->join = FALSE;
     node->split = FALSE;
     node->in = sl_make_slist();
     node->out = sl_make_slist();
     sl_add_to_tail(current_op->node_list,node);

#ifdef GRAPHIX
     if (graphic) {
	  node->og = make_node_graphic(name, node);
     } else {
	  node->og = NULL;
     }
#endif
     return node;
}

#ifdef GRAPHIX
PBoolean equal_node_name (Symbol name, Node *n) 
{
     return (name == n->name);
}
#else
PBoolean equal_node_name_name_list (Symbol name, Node_Name *n) 
{
     return (name == n->name);
}
#endif

Node *find_node_or_create(Symbol name,PBoolean graphic)
{
#ifdef GRAPHIX
     Node *n;

     if ((n = (Node *)sl_search_slist(current_op->node_list,name,equal_node_name)) == NULL) 
	  n =  make_node(name,graphic);

     return n;
#else
     Node_Name *n;

     if ((n = (Node_Name *)sl_search_slist(current_op->node_name_list,name,equal_node_name_name_list)) == NULL) 
	  return make_node(name,graphic);
     else
	  return n->node;
#endif
}

Edge *make_edge(void)
{
     Edge *e = MAKE_OBJECT(Edge);

#ifdef GRAPHIX
     e->og = NULL;
#endif
     e->expr = NULL;

     return e;
}

Edge *build_edge(PString n1, PString n2, Expression *expr, PBoolean graphic)
{
     Edge *edge = MAKE_OBJECT(Edge);

#ifdef GRAPHIX
     edge->in = find_node_or_create(n1,graphic);
#endif
     edge->type = ET_GOAL;
     edge->out=find_node_or_create(n2,graphic);
     sl_add_to_tail((edge->out)->in,edge);
     sl_add_to_head(current_op->edge_list,edge);

     edge->expr=expr;

#ifdef GRAPHIX
     if (graphic) 
       build_edge_graphic(edge, expr, global_draw_data);
     else {
	  edge->og = NULL;
     }
#endif

     return edge;
}

PString op_name(Op_Structure *op)
{
	return op->name;	
}

PString op_file_name(Op_Structure *op)
{
	return op->file_name;	
}

/*
 * Free Functions
 */

Op_Structure *dup_op(Op_Structure *op)
{
     DUP(op);
     return op;
}

void free_op(Op_Structure *op)
{
     if (LAST_REF(op)) {     
	  Node *node;
	  Edge *edge;

	  sl_loop_through_slist(op->node_list, node, Node *) {
	       free_node_from_free_op(node);
	  }
	  FREE_SLIST(op->node_list);
	  sl_loop_through_slist(op->edge_list, edge, Edge *) {
	       free_edge_from_free_op(edge);
	  }
	  FREE_SLIST(op->edge_list);
	  FREE_SLIST(op->node_name_list);

	  free_expr(op->invocation);
	  free_expr(op->call);
	  free_expr_list(op->context);
	  free_expr(op->setting);
	  free_list_property(op->properties);
	  free_expr_list(op->effects);
	  /*     if (op->action != (Gexpression *)TRUE)  In the Op-Editor we do not need the gexpr,
		 this fiels is just set to TRUE when it is an action OP. */
	  free_action_field(op->action); 

	  free_lenv(op->list_var);

#ifdef GRAPHIX
	  FREE(op->documentation);
	  if (op->op_title != NULL){ /* graphic objects */
	       free_op_title(op->op_title);
	  
	       free_text_og(op->gbody);
	       free_text_og(op->ginvocation);
	       free_text_og(op->gcall);
	       free_text_og(op->gcontext);
	       free_text_og(op->gsetting);
	       free_text_og(op->geffects);
	       free_text_og(op->gproperties);
	       free_text_og(op->gdocumentation);
	       free_text_og(op->gaction);
	       if (op->xms_name) 
		 XmStringFree(op->xms_name);

	       /* */
	       clear_specified_op_graphic(global_draw_data, op);
	  }

	  SAFE_SL_FREE_SLIST(op->list_og_node);
	  SAFE_SL_FREE_SLIST(op->list_og_edge);
	  SAFE_SL_FREE_SLIST(op->list_og_edge_text);
	  SAFE_SL_FREE_SLIST(op->list_og_text);
/*	  SAFE_SL_FREE_SLIST(op->list_og_inst); */ /* freed by the free_text_og(op->gbody)... */
	  SAFE_SL_FREE_SLIST(op->list_movable_og);
	  SAFE_SL_FREE_SLIST(op->list_destroyable_og);
	  SAFE_SL_FREE_SLIST(op->list_editable_og);
#endif

	  FREE(op->file_name);
     }

     FREE(op);
}


void free_node_from_free_op(Node *node) 
{
     /* The edges are freed directly  from free_op(). */
/*
 We know that all the op will be freed, so we only free outgoing edges
     sl_loop_through_slist(node->out, edge, Edge *) {
	  free_edge_from_free_op(edge);
     }
 */
     FREE_SLIST(node->out);
     FREE_SLIST(node->in);

#ifdef GRAPHIX
     if (node->og != NULL)   free_node_og(node->og);
#endif

     FREE(node);
}

void free_edge_from_free_op(Edge *edge) 
{
     free_expr(edge->expr);

#ifdef GRAPHIX
     if (edge->og != NULL) free_edge_og(edge->og);
#endif

     FREE(edge);
}

#ifdef GRAPHIX
void free_node_og(OG *og)
{
     Gnode *gnode;
     
     if (og != NULL) {
	  gnode = og->u.gnode;

	  XmStringFree(gnode->xmstring);
	  FREE(gnode);

	  XDestroyRegion(og->region);
	  FREE(og);
     }
}

void free_edge_og(OG *og)
{
     Gedge *gedge;
     OG *og_knot;

     if (og != NULL) {
	  switch (og->type) {
	  case DT_EDGE:
	       gedge = og->u.gedge;

	       sl_loop_through_slist(gedge->list_knot, og_knot, OG *)
		    free_knot_og(og_knot);
	       FREE_SLIST(gedge->list_knot);

	       free_edge_text_og(gedge->text);

	       FREE(gedge);
	       XDestroyRegion(og->region);
	       FREE(og);
	       break;
	  case DT_INST:
	       /* The inst og will be free directly from free_op */
	       break;
	  default:
	       fprintf(stderr,LG_STR("ERROR: Unexected og type in free_edge_og.\n",
				     "ERREUR: Type d'og inattendu dans la fonction free_edge_og.\n")); 
	       break;
	  }
     }
}

void free_edge_text_og(OG *og)
{
     Gedge_text *gedge_text;

     if (og != NULL) {
	  gedge_text  = og->u.gedge_text;
	  
	  FREE(gedge_text->log_string);
/*
 *	  XmStringFree(gedge_text->xmstring);
 */
	  free_lgt_string(gedge_text->lgt_log_string);
	  FREE(gedge_text);

	  XDestroyRegion(og->region);
	  FREE(og);
     }
}

void free_knot_og(OG *og)
{
     Gknot *gknot;
     
     if (og != NULL) {
	  gknot = og->u.gknot;
	  FREE(gknot);

	  XDestroyRegion(og->region);
	  FREE(og);
     }
}

void free_text_og(OG *og)
{
     Gtext *text;

     if (og != NULL) {
	  text  = og->u.gtext;
	  
	  FREE(text->string);
	  free_lgt_string(text->lgt_string);
	  free_list_og_inst(text->list_og_inst);
	  FREE(text);

	  XDestroyRegion(og->region);
	  FREE(og);
     }
}

void free_op_title(OG *og)
{
     Gtext *text = og->u.gtext;

     FREE(text->string);
     free_lgt_string(text->lgt_string);
     FREE(text);

     XDestroyRegion(og->region);
     FREE(og);
}

void free_inst_og(OG *og)
{
     Ginst *ginst;
     
     if (og != NULL) {
	  ginst = og->u.ginst;

	  FREE(ginst);
	  if (og->region != NULL)
	       XDestroyRegion(og->region);
	  FREE(og);
     }
}

void free_list_og_inst(List_OG list_og_inst) 
{
     OG *og_inst;
     SAFE_SL_LOOP_THROUGH_SLIST(list_og_inst, og_inst, OG *) { 
	  free_inst_og(og_inst);
     }
     SAFE_SL_FREE_SLIST(list_og_inst);
}

#endif

void build_invocation(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->invocation = expr;
#ifdef GRAPHIX
     if (dd) 
	  op->ginvocation = make_og_text_field(dd, op, FT_INVOCATION, TT_INVOCATION, x, y, visible, 
					       pp_width, pp_fill, ip_width);
#endif
}

void build_call(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->call = expr;
#ifdef GRAPHIX
     if (dd) 
	  op->gcall = make_og_text_field(dd, op, FT_CALL, TT_CALL, x, y, visible, 
					       pp_width, pp_fill, call_width);
#endif
}

void build_context(Op_Structure *op, PString name, ExprList pcnd,
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->context = pcnd;
#ifdef GRAPHIX
     if (dd) 
	  op->gcontext = make_og_text_field(dd, op, FT_CONTEXT, TT_CONTEXT, x, y, visible, 
					   pp_width, pp_fill, ctxt_width);
#endif
}

void build_setting(Op_Structure *op, PString name, Expression *expr, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->setting = expr;
#ifdef GRAPHIX
     if (dd) 
	  op->gsetting = make_og_text_field(dd, op, FT_SETTING, TT_SETTING, x, y, visible, 
					   pp_width, pp_fill, set_width);
#endif
}

void build_properties(Op_Structure *op, PString name, PropertyList pl, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     if (op->properties) FREE_SLIST(op->properties); /* it was intialized with a empty list. */
     op->properties = (pl ? pl : sl_make_slist());
#ifdef GRAPHIX
     if (dd) 
	  op->gproperties = make_og_text_field(dd, op, FT_PROPERTIES, TT_PROPERTIES, x, y, visible, 
					       pp_width, pp_fill, prop_width);
#endif
}

#ifdef GRAPHIX
void build_documentation(Op_Structure *op, PString name, PString documentation, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->documentation = documentation;
     if (dd) 
	  op->gdocumentation = make_og_text_field(dd, op, FT_DOCUMENTATION, TT_DOCUMENTATION, x, y, visible, 
						  pp_width, pp_fill, doc_width);
}
#endif

Add_Del_List make_adl_from_el(ExprList el)
{
     return el;
}

void build_effects(Op_Structure *op, PString name,  ExprList adl, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
/*      if (op->effects) FREE_SLIST(op->effects);  it was intialized with a empty list. */
     
     op->effects = make_adl_from_el(adl);
#ifdef GRAPHIX
     if (dd) 
	  op->geffects = make_og_text_field(dd, op, FT_EFFECTS, TT_EFFECTS, x, y, visible, 
					    pp_width, pp_fill, eff_width);
#endif
}

void build_action(Op_Structure *op, PString name, Action_Field *action, 
		      int  x, int y, PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd)
{
     op->action = action;
#ifdef GRAPHIX
     if (dd) 
	  op->gaction = make_og_text_field(dd, op, FT_ACTION, TT_ACTION, x, y, visible, 
					   pp_width, pp_fill, act_width);
#endif
}

#ifdef GRAPHIX
OG *
#else
void
#endif
build_and_add_node(Op_Structure *op, PString name, Node_Type nt, PBoolean join, PBoolean split,
			int  x, int y, Draw_Data *dd)
{
  Node *node = MAKE_OBJECT(Node);
  
  node->join = join;
  node->split = split;
  node->type= nt;
  
#ifdef GRAPHIX
     node->name = name;
#else
     register_node(current_op,name, node);
#endif
     node->in = sl_make_slist();
     node->out = sl_make_slist();
     sl_add_to_tail(current_op->node_list,node);

     
     if (nt == NT_START) {
       op->start_point=node;
     }
     
#ifdef GRAPHIX
         if (dd)
	  node->og = make_og_node(dd, op, node, x, y);
     else
	  node->og = NULL;

     return node->og;
#endif
}

Node *find_node(Op_Structure *op, Symbol name)
{
#ifdef GRAPHIX
     Node *n;

     if ((n = (Node *)sl_search_slist(op->node_list,name,equal_node_name)) == NULL)
	  fprintf(stderr,LG_STR("ERROR: enable to find node %s in the OP %s.\n",
				"ERREUR: Incapable de trouver le noeud %s dans le OP %s.\n"), name, op->name); 

     return n;
#else
     Node_Name *n;

     if ((n = (Node_Name *)sl_search_slist(op->node_name_list,name,equal_node_name_name_list)) == NULL)
	  fprintf(stderr,LG_STR("ERROR: enable to find node %s in the OP %s.\n",
				"ERREUR: Incapable de trouver le noeud %s dans le OP %s.\n"), name, op->name); 

     return n->node;
#endif
}

void build_and_add_edge(Op_Structure *op,  PString in,  PString out,
			Edge_Type et, Expression *expr, Slist *knots, int x, int y,
			int pp_width, PBoolean pp_fill,  Draw_Data *dd) 
{
     Edge *edge = MAKE_OBJECT(Edge);
     Node *node_in = find_node(op,in);

#ifdef GRAPHIX
     edge->in = node_in;
#endif
     edge->out = find_node(op, out);
     sl_add_to_tail((edge->out)->in,edge);
     sl_add_to_tail(node_in->out,edge);

     edge->type = et;

     sl_add_to_head(op->edge_list,edge);
     edge->expr=expr;

     if (dd) {
#ifdef GRAPHIX
	  edge->og = make_og_edge(dd, op, edge, edge->in, edge->out, knots, x, y, pp_width, pp_fill);
#endif
     } else {
	  FREE_SLIST(knots);
#ifdef GRAPHIX
	  edge->og = NULL;
#endif
     }
}

void build_and_add_then_else_edge(Op_Structure *op, PString in,  PString out,
				  Edge_Type et, Draw_Data *dd) 
{
     Edge *edge = MAKE_OBJECT(Edge);
     Node *node_in = find_node(op,in);

#ifdef GRAPHIX
     edge->in = node_in;
#endif
     edge->out = find_node(op, out);
     sl_add_to_tail((edge->out)->in,edge);
     sl_add_to_tail(node_in->out,edge);

     edge->expr = NULL;
     edge->type = et;
#ifdef GRAPHIX
     edge->og = NULL;
#endif
     sl_add_to_head(op->edge_list,edge);
}

Node *else_node_from_if_node(Node *node)
{
     LogicList out;
     Edge *edge = NULL;

     out = node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_ELSE)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_ELSE)))
	  return edge->out;
     else {
	  fprintf(stderr, LG_STR("ERROR: else_node_from_if_node: arghh.\n",
				 "ERREUR: else_node_from_if_node: oups.\n"));
	  return NULL;
     }
}

Node *then_node_from_if_node(Node *node)
{
     LogicList out;
     Edge *edge = NULL;

     out = node->out;
     if (((edge = (Edge *)sl_get_slist_head(out)) && (edge->type == ET_THEN)) ||
	 ((edge = (Edge *)sl_get_slist_tail(out)) && (edge->type == ET_THEN)))
	  return edge->out;
     else {
	  fprintf(stderr, LG_STR("ERROR: in then_node_from_if_node.\n",
				 "ERREUR: then_node_from_if_node.\n"));
	  return NULL;
     }
}

void  finish_loading_op(Op_Structure *op,  Draw_Data *dd)
{
#ifdef GRAPHIX
     Node *node;

     if (dd) {
	  sl_loop_through_slist(op->node_list, node, Node *) {
	       if (node->type == NT_IF && node->og) {
		    position_then_else(node->og,node->og->x, node->og->y);
	       }
	  }
	  if (! op->gcall) 
	       op->gcall = make_og_text_field(dd, op, FT_CALL, TT_CALL,  call_x, call_y, FALSE, 
					      call_width, FALSE , call_width);
     }
#else
     Node_Name *nn;
     sl_loop_through_slist(op->node_name_list, nn, Node_Name *) {
	  FREE(nn);
     }
     FREE_SLIST(op->node_name_list);
     op->node_name_list = NULL;
#endif
}

PString new_node_name(Op_Structure *op)
{
     char name[20];
     char *node_name;
     PBoolean save_cs = check_symbol;

     check_symbol = FALSE;

     do {
	  sprintf(name, "N%d", node_index++);
	  DECLARE_ID(name, node_name);
     } while
#ifdef GRAPHIX
	  (sl_search_slist(op->node_list, node_name, equal_node_name) != NULL)
#else
          (sl_search_slist(op->node_name_list, node_name, equal_node_name_name_list) != NULL)
#endif
	     ;
     check_symbol = save_cs;

     return node_name;
}


void new_then_else_node_name_from_if_name (PString if_name, PString *then_name_p,
					   PString *else_name_p)
{
     PString then_name;
     PString else_name;

     int length = strlen (if_name);

     then_name = *then_name_p = (PString) MALLOC(length +3); 
     else_name = *else_name_p = (PString) MALLOC(length +3); 

     if (if_name[0] == '|') {  /* The name is an "idbar" */
	  strncpy(then_name, if_name, length - 1);
	  strncpy(else_name, if_name, length - 1);
	  then_name[length-1] = else_name[length-1] = '_';
	  then_name[length] = 'T';
	  else_name[length] = 'F';
	  then_name[length+1] = else_name[length+1] = '|';
	  then_name[length+2] = else_name[length+2] = '\0';
     } else {
	  strncpy(then_name, if_name, length);
	  strncpy(else_name, if_name, length );
	  then_name[length] = else_name[length] = '_';
	  then_name[length+1] = 'T';
	  else_name[length+1] = 'F';
	  then_name[length+2] = else_name[length+2] = '\0';
     }
}

void new_if_then_else_node_name(Op_Structure *op, PString *nif, PString *nthen, PString *nelse)
{
     char name[20];
     char name_else[20];
     char name_then[20];
     PBoolean save_cs = check_symbol;

     check_symbol = FALSE;

     do {
	  sprintf(name, "N%d", node_index);
	  sprintf(name_else, "N%d_F",node_index);
	  sprintf(name_then, "N%d_T", node_index++);
	  DECLARE_ID(name, *nif);
	  DECLARE_ID(name_else, *nelse);
	  DECLARE_ID(name_then, *nthen);
     } while
#ifdef GRAPHIX
	  ((sl_search_slist(op->node_list, *nif, equal_node_name) != NULL) ||
	   (sl_search_slist(op->node_list, *nelse, equal_node_name) != NULL) ||
	   (sl_search_slist(op->node_list, *nthen, equal_node_name) != NULL))
#else
	  ((sl_search_slist(op->node_name_list, *nif, equal_node_name_name_list) != NULL) ||
	   (sl_search_slist(op->node_name_list, *nelse, equal_node_name_name_list) != NULL) ||
	   (sl_search_slist(op->node_name_list, *nthen, equal_node_name_name_list) != NULL))
#endif
	     ;

     check_symbol = save_cs;
}

PString new_end_node_name(Op_Structure *op)
{
     char name[20];
     char *node_name;
     PBoolean save_cs = check_symbol;

     check_symbol = FALSE;

     do {
	  sprintf(name, "END%d", node_index++);
	  DECLARE_ID(name, node_name);
     } while
#ifdef GRAPHIX
	  (sl_search_slist(op->node_list, node_name, equal_node_name) != NULL)
#else
	  (sl_search_slist(op->node_name_list, node_name, equal_node_name_name_list) != NULL)
#endif
	     ;

     check_symbol = save_cs;

     return node_name;
}     


PBoolean sort_op(Op_Structure *op1, Op_Structure *op2)
{
     return (strcmp(op1->name,op2->name) < 0);
}
