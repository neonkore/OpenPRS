
/*                               -*- Mode: C -*- 
 * top-structure.c -- 
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


#ifdef VXWORKS
#else
#endif

#include "slistPack.h"

#include "constant.h"

#ifdef GRAPHIX
#include <Xm/Xm.h>
#include "op-structure.h"
#ifdef GTK
#include <gtk/gtk.h>
#include "gope-graphic.h"
#else
#include "ope-graphic.h"
#endif
#else
#include "op-structure.h"
#endif
#include "op-default.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#include "top-structure_f.h"
#include "op-structure_f.h"
#include "oprs-print_f.h"
#include "oprs-type_f.h"

#ifdef GRAPHIX
#include "ope-external_f.h"
#endif

PBoolean really_build_node = TRUE;

static Slist *break_list = NULL;
static Slist *label_list = NULL;
static Slist *goto_list = NULL;

/* Local prototyupes */
void free_instruction(Instruction *inst);

#ifdef GRAPHIX
void clean_inst_sl_in_slist_og_inst(List_OG list_og_inst)
{
     /*
      * This function is used to clean all the pointer to instructions 
      * that will be deleted in the X-OPRS kernel. 
      */
     OG *og_inst;
     sl_loop_through_slist(list_og_inst, og_inst, OG *) { 
	  og_inst->u.ginst->inst = NULL;
     }
}
#endif

/* Free functions */
void free_list_instruction( List_Instruction insts)
{
     if (insts) {
	  Instruction *inst;
     
	  sl_loop_through_slist(insts, inst, Instruction *) {
	       free_instruction(inst);
	  }
	  FREE_SLIST(insts);
     }
}

void free_body(Body *body)
{
     if (body) {
	  free_list_instruction(body->insts);

	  FREE(body);
     }
}

void free_simple_instruction(Simple_Instruction *simple)
{
     FREE(simple);
}

void free_if_instruction(If_Instruction *if_inst)
{
     free_list_instruction(if_inst->then_insts);
     if (if_inst->elseif)
	  free_instruction(if_inst->u.elseif_inst);
     else
	  free_list_instruction(if_inst->u.else_insts);
     FREE(if_inst);
}

void free_while_instruction(While_Instruction *while_inst)
{
     free_list_instruction(while_inst->insts);
     FREE(while_inst);
}

void free_do_instruction(Do_Instruction *do_inst)
{
     free_list_instruction(do_inst->insts);
     FREE(do_inst);
}

void free_par_instruction(Par_Instruction *par_inst)
{
     if (par_inst->bodys) {
	  Body *body;

	  sl_loop_through_slist(par_inst->bodys, body, Body *) {
	       free_body(body);
	  }
	  FREE_SLIST(par_inst->bodys);
	  FREE(par_inst);
     }
}

void free_instruction(Instruction *inst)
{
     switch(inst->type) {
     case IT_SIMPLE:
	  free_simple_instruction(inst->u.simple_inst);
	  break;
     case IT_IF:
	  free_if_instruction(inst->u.if_inst);
	  break;
     case IT_WHILE:
	  free_while_instruction(inst->u.while_inst);
	  break;
     case IT_DO:
	  free_do_instruction(inst->u.do_inst);
	  break;
     case IT_PAR:
	  free_par_instruction(inst->u.par_inst);
	  break;
     case IT_COMMENT:
	  FREE(inst->u.comment);
	  break;
     case IT_GOTO:
     case IT_LABEL:
     case IT_BREAK:
	  break;
     default: 	 
	  fprintf(stderr,LG_STR("Unknown instruction type in free_instruction.\n",
				"Unknown instruction type in free_instruction.\n")); 
	  break;
     }

     FREE(inst);
}

/* Creation function */

Body *make_body(List_Instruction insts)
{
     Body *res = MAKE_OBJECT(Body);

     res->insts = insts;

     return res;
}

Instruction *make_simple_instruction(Expression *expr)
{
     Instruction *res = MAKE_OBJECT(Instruction);
     Simple_Instruction *res_simple = MAKE_OBJECT(Simple_Instruction);

     res_simple->expr = expr;
#ifdef GRAPHIX
     res_simple->og = NULL;
#endif
     res->type = IT_SIMPLE;
     res->u.simple_inst = res_simple;

     return res;
} 

Instruction *make_comment_instruction(PString comment)
{
     Instruction *res = MAKE_OBJECT(Instruction);

     res->type = IT_COMMENT;
     res->u.comment = comment;

     return res;
} 

Instruction *make_goto_instruction(Symbol goto_label)
{
     Instruction *res = MAKE_OBJECT(Instruction);

     res->type = IT_GOTO;
     res->u.goto_inst = goto_label;

     return res;
} 

Instruction *make_label_instruction(Symbol label)
{
     Instruction *res = MAKE_OBJECT(Instruction);

     res->type = IT_LABEL;
     res->u.label_inst = label;

     return res;
} 

Instruction *make_break_instruction()
{
     Instruction *res = MAKE_OBJECT(Instruction);

     res->type = IT_BREAK;

     return res;
} 

Instruction *make_simple_if_instruction(Expression *condition, List_Instruction then_insts)
{
     Instruction *res = MAKE_OBJECT(Instruction);
     If_Instruction *res_if = MAKE_OBJECT(If_Instruction);

     res_if->condition = condition;
     res_if->then_insts = then_insts;

#ifdef GRAPHIX
     res_if->og = NULL;
#endif

     res->type = IT_IF;
     res->u.if_inst = res_if;

     return res;
}

Instruction *make_if_instruction(Expression *condition, List_Instruction then_insts, List_Instruction else_insts)
{
     Instruction *res = make_simple_if_instruction(condition,then_insts);

     res->u.if_inst->elseif = FALSE;
     res->u.if_inst->u.else_insts = else_insts;

     return res;
}

Instruction *make_elseif_instruction(Expression *condition, List_Instruction then_insts, Instruction *elseif_inst)
{
     Instruction *res = make_simple_if_instruction(condition,then_insts);

     res->u.if_inst->elseif = TRUE;
     if (elseif_inst->type != IT_IF)
	       fprintf(stderr,LG_STR("Error, expecting a IT_IF instruction in an elseif.\n",
				     "Error, expecting a IT_IF instruction in an elseif.\n"));
     else {
	  res->u.if_inst->u.elseif_inst = elseif_inst;
     }	  
     return res;
}

Instruction *make_while_instruction(Expression *condition, List_Instruction insts)
{
     Instruction *res = MAKE_OBJECT(Instruction);
     While_Instruction *res_while = MAKE_OBJECT(While_Instruction);

     res_while->condition = condition;
     res_while->insts = insts;
#ifdef GRAPHIX
     res_while->og = NULL;
#endif
     res->type = IT_WHILE;
     res->u.while_inst = res_while;

     return res;
}

Instruction *make_do_instruction(Expression *condition, List_Instruction insts)
{
     Instruction *res = MAKE_OBJECT(Instruction);
     Do_Instruction *res_do = MAKE_OBJECT(Do_Instruction);

     res_do->condition = condition;
     res_do->insts = insts;
#ifdef GRAPHIX
     res_do->og = NULL;
#endif
     res->type = IT_DO;
     res->u.do_inst = res_do;

     return res;
}

Instruction *make_par_instruction(List_Body bodys)
{
     Instruction *res = MAKE_OBJECT(Instruction);
     Par_Instruction *res_par = MAKE_OBJECT(Par_Instruction);

     res_par->bodys = bodys;
     res->type = IT_PAR;
     res->u.par_inst = res_par;

     return res;
}

Double_Node *make_double_node(Control_Point *head, Control_Point *tail)
{
     Double_Node *res = MAKE_OBJECT(Double_Node);

     res->head = head;
     res->tail = tail;
     DN_INST(res) = NULL;

     return res;     
}

Node *make_simple_node()
{
     if (really_build_node) {
	  Node *node = MAKE_OBJECT(Node);
     
	  node->join = FALSE;
	  node->split = FALSE;
	  node->in = sl_make_slist();
	  node->out = sl_make_slist();
	  node->type = NT_PROCESS;
	  sl_add_to_tail(current_op->node_list,node);
#ifdef GRAPHIX
	  node->og = NULL;
	  node->name = declare_atom("no_name_node");
#endif
	  return node;
     } else return NULL;
}

Node *make_complex_node(Symbol name, Node_Type type)
{
     if (really_build_node) {
	  Node *node = make_simple_node();
//	  node->name = name;	/* recently added... */
	  node->type = type;

	  return node; 
     } else return NULL;
}

void free_simple_node(Node *node)
{
     if (node) {
	  FREE_SLIST(node->in);
	  FREE_SLIST(node->out);
	  sl_delete_slist_node(current_op->node_list,node);
	  FREE(node);
     }
}

Edge *make_and_connect_simple_edge(Control_Point *head, Control_Point *tail, Expression *expr)
{
     Edge *edge = MAKE_OBJECT(Edge);

     edge->type = ET_GOAL;
#ifdef GRAPHIX
     edge->in=head;
#endif
     edge->out=tail;
     sl_add_to_tail((edge->out)->in,edge);
     sl_add_to_tail(head->out,edge);
     sl_add_to_head(current_op->edge_list,edge);

     edge->expr=expr;
#ifdef GRAPHIX
     edge->og = NULL;
#endif
     return edge;
}


Edge *make_and_connect_edge(Control_Point *head, Control_Point *tail, Expression *expr, Edge_Type type)
{
     Edge *res = make_and_connect_simple_edge(head, tail, expr);

     res->type = type;

     return res;
	  
}

Double_Node *make_empty_double_node()
{
     Double_Node *res;

     if (really_build_node) {
	  res = make_double_node(make_simple_node(),make_simple_node());
     
	  make_and_connect_simple_edge(res->head, res->tail, NULL);
     } else 
	  res = make_double_node(NULL,NULL);     
     
     return res;     
}

void free_double_node(Double_Node *dn)
{
     if (dn) FREE(dn);
}

Control_Point *merge_top_node(Control_Point *first, Control_Point *second)
{
     Edge *edge;

     if (first->split) {
	  if (! (sl_slist_empty(second->out)) && ! second->split)
	       fprintf(stderr,LG_STR("merge_top_node: incompatible spliting node merge.\n",
				     "merge_top_node: incompatible spliting node merge.\n"));
	  else
	       second->split = TRUE;
     }
     if (first->join)
	  fprintf(stderr,LG_STR("merge_top_node: incompatible joining node merge.\n",
				"merge_top_node: incompatible joining node merge.\n"));

     sl_loop_through_slist(first->in, edge, Edge *) {
	  sl_add_to_tail(second->in, edge);
	  edge->out = second;
     }

     sl_loop_through_slist(first->out, edge, Edge *) {
	  sl_add_to_tail(second->out, edge);
#ifdef GRAPHIX
	  edge->in = second;
#endif
     }

     free_simple_node(first);

     return second;
}

Double_Node *add_inst_to_list_inst(Double_Node *list_inst, Double_Node *inst)
{
     Double_Node *res;

     if (!list_inst) {	/* If NULL list_inst... returns the other one. */
	  if (!inst) return NULL;
	  else {
	       Slist *insts = sl_make_slist();
	       sl_add_to_tail(insts,DN_INST(inst));
	       DN_LIST_INST(inst) = insts;
	       return inst;
	  }
     }

     if (!inst) return list_inst;

     if (really_build_node) {
	  res = make_double_node(list_inst->head, inst->tail);
	  merge_top_node(list_inst->tail,inst->head);
     } else {
	  res = make_double_node(NULL, NULL);
     }

     sl_add_to_tail(DN_LIST_INST(list_inst),DN_INST(inst));
     DN_LIST_INST(res) = DN_LIST_INST(list_inst);

     free_double_node(list_inst);
     free_double_node(inst);

     return res;     
}

Double_Node *build_par_branch(Slist *branch)
{
     Node *n1 = NULL;
     Node *n2 = NULL;
     Double_Node *dn, *dn2 = NULL, *res;
     List_Body bodys;

     if (sl_slist_empty(branch)) {
	  FREE_SLIST(branch);	  
	  return NULL;
     }

     bodys = sl_make_slist();

     if (really_build_node) {
	  n1 = make_simple_node();
	  dn2 = make_empty_double_node();
	  n2 = DN_HEAD(dn2);

	  n1->split = TRUE;
	  n2->join = TRUE;
     }

     sl_loop_through_slist(branch, dn, Double_Node *) {
	  if (really_build_node) {
	       n1 = merge_top_node(DN_HEAD(dn), n1);
	       n2 = merge_top_node(DN_TAIL(dn), n2);
	  }
	  sl_add_to_tail(bodys,DN_BODY(dn));
	  free_double_node(dn);
     }
     
     FREE_SLIST(branch);

     if (really_build_node) {
	  res= make_double_node(n1,DN_TAIL(dn2));
	  free_double_node(dn2);
     } else
	  res= make_double_node(NULL,NULL);
     
     DN_INST(res) = make_par_instruction(bodys);
     
     return res;
}

void insert_branch(Double_Node *branch, Node *head, Node *tail)
{
     if (really_build_node) {
	  if (branch) {
	       merge_top_node(DN_HEAD(branch), head);
	       merge_top_node(DN_TAIL(branch), tail);
	  } else 
	       merge_top_node(head,tail);
     }
}

Double_Node *build_do_until(Double_Node *insts, Expression *cond)
{
     Double_Node *res;
#ifdef GRAPHIX
     Edge *edge = NULL;
#endif
     Instruction *inst_res;

     if (really_build_node) {
	  Symbol nelse = NULL, nthen = NULL, nif = NULL;
	  Symbol *nifp, *nelsep, *nthenp;
	  Double_Node *dn = make_empty_double_node();
	  Node *node = make_simple_node();
	  Node *nnif, *nnthen, *nnelse;

	  nifp = &nif;
	  nelsep = &nelse;
	  nthenp = &nthen;
				 
	  new_if_then_else_node_name(current_op, nifp, nthenp, nelsep);
				 
	  nnif = make_complex_node(nif, NT_IF);
	  nnthen = make_complex_node(nthen, NT_THEN);
	  nnelse = make_complex_node(nelse, NT_ELSE);

	  make_and_connect_edge(nnif, nnthen, NULL, ET_THEN);
	  make_and_connect_edge(nnif, nnelse, NULL, ET_ELSE);
#ifdef GRAPHIX
	  edge =
#endif
	     make_and_connect_edge(node, nnif, cond, ET_IF);

	  merge_top_node(DN_TAIL(dn),nnthen);

	  insert_branch(insts,nnthen,node);
	  res = make_double_node(DN_HEAD(dn),nnelse);
	  free_double_node(dn);
     } else
	  res = make_double_node(NULL,NULL);

     DN_INST(res) = inst_res = make_do_instruction(cond,(insts ? DN_LIST_INST(insts) : NULL));
#ifdef GRAPHIX
     inst_res->u.do_inst->og = make_inst_graphic(inst_res, (really_build_node ? edge : NULL));
#endif
     free_double_node(insts);

     return res;
}

Double_Node *build_while(Expression *cond, Double_Node *insts)
{
     Double_Node *res;
     Edge *edge = NULL;
     Instruction *inst_res;

     if (really_build_node) {
	  Symbol nelse = NULL, nthen = NULL, nif = NULL;
	  Symbol *nifp, *nelsep, *nthenp;
	  Node *nnif, *nnthen, *nnelse;
	  Node *node = make_simple_node();
	  Double_Node *dn = make_empty_double_node();

	  nifp = &nif;
	  nelsep = &nelse;
	  nthenp = &nthen;
				 
	  new_if_then_else_node_name(current_op, nifp, nthenp, nelsep);
				 
	  nnif = make_complex_node(nif, NT_IF);
	  nnthen = make_complex_node(nthen, NT_THEN);
	  nnelse = make_complex_node(nelse, NT_ELSE);

	  make_and_connect_edge(nnif, nnthen, NULL, ET_THEN);
	  make_and_connect_edge(nnif, nnelse, NULL, ET_ELSE);
	  edge = make_and_connect_edge(node, nnif, cond, ET_IF);

	  merge_top_node(DN_TAIL(dn),node);

	  insert_branch(insts,nnthen,node);

	  res= make_double_node(DN_HEAD(dn),nnelse);     
	  free_double_node(dn);
     } else
	  res= make_double_node(NULL, NULL);     

     DN_INST(res) = inst_res = make_while_instruction(cond, (insts ? DN_LIST_INST(insts) : NULL));
#ifdef GRAPHIX
     inst_res->u.while_inst->og = make_inst_graphic(inst_res, (really_build_node ? edge : NULL));
#endif

     free_double_node(insts);

     return res;
}

Double_Node *build_if(Expression *cond, Double_Node *thenb, Double_Node *elseb, PBoolean elseif)
{
     Double_Node *res;
     Edge *edge = NULL;
     Instruction *inst_res;

     if (really_build_node) {

	  if (! elseb && ! thenb) {
	       res = make_double_node(make_simple_node(),make_simple_node());
	       edge = make_and_connect_simple_edge(res->head, res->tail, cond);
	  } else {

	       Symbol nelse = NULL, nthen = NULL, nif = NULL;
	       Symbol *nifp, *nelsep, *nthenp;
	       Node *nnif, *nnthen, *nnelse;
	       Node *node = make_simple_node();
	       Double_Node *dn1 = make_empty_double_node();

	       nifp = &nif;
	       nelsep = &nelse;
	       nthenp = &nthen;
				 
	       new_if_then_else_node_name(current_op, nifp, nthenp, nelsep);
				 
	       nnif = make_complex_node(nif, NT_IF);
	       nnthen = make_complex_node(nthen, NT_THEN);
	       nnelse = make_complex_node(nelse, NT_ELSE);

	       make_and_connect_edge(nnif, nnthen, NULL, ET_THEN);
	       make_and_connect_edge(nnif, nnelse, NULL, ET_ELSE);
	       edge = make_and_connect_edge(node, nnif, cond, ET_IF);

	       if (!thenb) {
		    if (!elseb) {
			 fprintf(stderr, LG_STR("build_if: error, we should not get here...\n",
						"build_if: error, we should not get here...\n"));
			 res = NULL;
		    } else {
			 insert_branch(elseb,nnelse,DN_HEAD(dn1));
			 merge_top_node(nnthen,DN_HEAD(dn1));
			 res= make_double_node(node,DN_TAIL(dn1));
		    }
	       } else {
		    insert_branch(thenb,nnthen,DN_HEAD(dn1));
	  
		    if (elseb) {
			 insert_branch(elseb,nnelse,DN_HEAD(dn1));
		    } else {
			 merge_top_node(nnelse,DN_HEAD(dn1));
		    }
		    res= make_double_node(node,DN_TAIL(dn1));
	       }
	       free_double_node(dn1);
	  }
     } else
	  res= make_double_node(NULL,NULL);

     if (res) {
       DN_INST(res) = inst_res = (elseif ? 
			      make_elseif_instruction(cond,
						  (thenb ? DN_LIST_INST(thenb) : NULL),
						  (elseb ? DN_INST(elseb) : NULL)):
			      make_if_instruction(cond,
						  (thenb ? DN_LIST_INST(thenb) : NULL),
						  (elseb ? DN_LIST_INST(elseb) : NULL)));
     

#ifdef GRAPHIX
       inst_res->u.if_inst->og = make_inst_graphic(inst_res, (really_build_node ? edge : NULL));
#endif
     }

     free_double_node(thenb);
     free_double_node(elseb);

     return res;
}

Double_Node *build_inst(Expression *inst)
{
     Double_Node *res;
     Edge *edge = NULL;
     Instruction *inst_res;

     if (really_build_node) {
	  res = make_double_node(make_simple_node(),make_simple_node());
	  edge = make_and_connect_simple_edge(res->head, res->tail, inst);
     } else
	  res = make_double_node(NULL, NULL);

     DN_INST(res) = inst_res = make_simple_instruction(inst);

#ifdef GRAPHIX
     inst_res->u.simple_inst->og = make_inst_graphic(inst_res, (really_build_node ? edge : NULL));
#endif
     return res;
}

Double_Node *build_comment(PString comment)
{
     Double_Node *res;
     
     if (really_build_node) {
	  res = make_empty_double_node();
     } else
	  res = make_double_node(NULL, NULL);

     DN_INST(res) = make_comment_instruction(comment);

     return res;
}

Goto_Label_Edge *make_goto_label(Symbol label, Edge *edge)
{
     Goto_Label_Edge *res = MAKE_OBJECT(Goto_Label_Edge);

     res->name = label;
     res->edge = edge;

     return res;
}

PBoolean eq_goto_label_name(Symbol name, Goto_Label_Edge *n)
{
     return (name == n->name);
}

Double_Node *build_goto_inst(Symbol label)
{
     Double_Node *res;

     res = make_empty_double_node();

     DN_INST(res) = make_goto_instruction(label);

     if (really_build_node) 
	  sl_add_to_tail(goto_list, make_goto_label(label,(Edge *)sl_get_slist_head(res->head->out)));
     
     return res;
}

Double_Node *build_label_inst(Symbol label)
{
     Double_Node *res;
     
     res = make_empty_double_node();

     DN_INST(res) = make_label_instruction(label);

     if (really_build_node) {
	  if (! sl_search_slist(label_list, label, eq_goto_label_name))	/* S'il n'y est pas deja. */
	       sl_add_to_tail(label_list, make_goto_label(label, (Edge *)sl_get_slist_head(res->head->out)));
	  else
	       printf(LG_STR("ERROR: Label %s declared more than once in OP %s.\n",
			     "ERROR: Label %s declared more than once in OP %s.\n"), label, current_op->name);
     }
     return res;
}

Double_Node *build_break_inst()
{
     Double_Node *res;
     
     res = make_empty_double_node();
     
     DN_INST(res) = make_break_instruction();

     if (really_build_node)
	  sl_add_to_tail(break_list, sl_get_slist_head(res->head->out));

     return res;
}

void parse_break_list(Double_Node *dn)
{
     Logic *logic;

     if (really_build_node) {
	  sl_loop_through_slist(break_list, logic , Logic *) {
	       logic->out = DN_TAIL(dn);
	       sl_add_to_tail(DN_TAIL(dn)->in, logic);
	  }
     }
     
     FLUSH_SLIST(break_list);
}

void build_body(Op_Structure *op, Symbol name, Body *body, int  x, int y, 
		PBoolean visible, int pp_width, PBoolean pp_fill, Draw_Data *dd) 
{
     op->body = body;
#ifdef GRAPHIX
     if (dd) 
	  op->gbody = make_og_text_field(dd, op, FT_BODY, TT_BODY, x, y, visible, 
					       pp_width, pp_fill, bd_width);
     if (really_build_node) { 
	  /* All the OG instructions have been filled,
	   * link the edges to them.
	   */
	  OG *og;
	  sl_loop_through_slist(op->list_og_inst, og, OG *) {
	       og->u.ginst->edge->og = og;
	  }
     } 
#endif
}

void  finish_loading_top(Op_Structure *op,  Draw_Data *dd)
{
     Goto_Label_Edge *goto_edge, *label_edge;

     if (! op->context)
#ifndef GRAPHIX
	  build_context(current_op, "CONTEXT", NULL, 0, 0,
			FALSE, 0, TRUE, dd);
#else
	  build_context(current_op, "CONTEXT", NULL, ctxt_x, ctxt_y,
			FALSE, ctxt_width, TRUE, dd);
#endif

     if (! op->call)
#ifndef GRAPHIX
	  build_call(current_op, "CALL", NULL, 0, 0,
			FALSE, 0, TRUE, dd);
#else
	  build_call(current_op, "CALL", NULL, call_x, call_y,
			FALSE, call_width, TRUE, dd);
#endif
     if (! op->setting)
#ifndef GRAPHIX
	  build_setting(current_op, "SETTING", NULL, 0, 0,
			FALSE, 0, TRUE, dd);
#else
	  build_setting(current_op, "SETTING", NULL, set_x, set_y,
			FALSE, set_width, TRUE, dd);
#endif

     if (! op->effects)
#ifndef GRAPHIX
	  build_effects(current_op, "EFFECTS", NULL, 0, 0,
			       FALSE, 0, TRUE, dd);
#else
	  build_effects(current_op, "EFFECTS", NULL, eff_x, eff_y,
			       FALSE, eff_width, TRUE, dd);
#endif

#ifdef GRAPHIX
     if (! op->documentation)
	  build_documentation(current_op, "DOCUMENTATION", NULL, doc_x, doc_y,
				  FALSE, doc_width, TRUE, dd);
#endif
     if (sl_slist_empty(op->properties))
#ifndef GRAPHIX
	  build_properties(current_op, "PROPERTIES", NULL, 0, 0,
			   FALSE, 0, TRUE, dd);
#else
          build_properties(current_op, "PROPERTIES", NULL, prop_x, prop_y,
			   FALSE, prop_width, TRUE, dd);
#endif
     
     if (! sl_slist_empty(break_list)) 
	  printf(LG_STR("WARNING: Unclaimed BREAK in %s.\n",
			"WARNING: Unclaimed BREAK in %s.\n"), current_op->name);
     FLUSH_SLIST(break_list);

     sl_loop_through_slist(goto_list, goto_edge, Goto_Label_Edge *) {
	  if ( ! (label_edge = (Goto_Label_Edge *)sl_search_slist(label_list, goto_edge->name, eq_goto_label_name))) 
	       printf(LG_STR("ERROR: Undeclared LABEL %s in %s.\n",
			     "ERROR: Undeclared LABEL %s in %s.\n"), goto_edge->name, current_op->name);
	  else {
	       goto_edge->edge->out = label_edge->edge->out;
	       sl_add_to_tail(label_edge->edge->out->in, goto_edge->edge);
	  }
	  FREE(goto_edge);
     }
     FLUSH_SLIST(goto_list);

     sl_loop_through_slist(label_list, label_edge, Goto_Label_Edge *) {
	  FREE(label_edge);
     }
     FLUSH_SLIST(label_list);
}

void init_make_top(Symbol name, PBoolean graphic)
{
     init_make_op(name,graphic);

     if (! break_list) break_list = sl_make_slist();
     else FLUSH_SLIST(break_list);

     if (! label_list) label_list = sl_make_slist();
     else FLUSH_SLIST(label_list);

     if (! goto_list) goto_list = sl_make_slist();
     else FLUSH_SLIST(goto_list);
}

/* text op print functions */
void fprint_instruction(FILE *f, Instruction *inst);

void fprint_list_instruction(FILE *f, List_Instruction insts)
{
     if (insts) {
	  Instruction *inst;

	  sl_loop_through_slist(insts, inst, Instruction *) {
	       fprint_instruction(f,inst);
	       fprintf(f,"\n");
	  }
     }
}

void fprint_body(FILE *f,Body *body) 
{ 
     if (! body) {
	  fprintf(f,"(Body *)NULL");
     } else {
	  fprintf(f,"(");
	  fprint_list_instruction(f,body->insts);
	  fprintf(f,")");
     }
}

void print_body(Body *body)
{
     fprint_body(stdout, body);
}

void fprint_simple_instruction(FILE *f, Simple_Instruction *simple)
{
     fprint_expr(f, simple->expr);
}

void fprint_part_if_instruction(FILE *f, If_Instruction *if_inst)
{
     fprint_expr(f, if_inst->condition);
     fprint_list_instruction(f,if_inst->then_insts);
     if (if_inst->elseif) {
	  fprintf(f,(lower_case_id ? "elseif " : "ELSEIF "));
	       fprint_part_if_instruction(f,if_inst->u.elseif_inst->u.if_inst);
     } else
	  if (if_inst->u.else_insts) {
	       fprintf(f,(lower_case_id ? "else " : "ELSE "));
	       fprint_list_instruction(f,if_inst->u.else_insts);
	  }
}

void fprint_if_instruction(FILE *f, If_Instruction *if_inst)
{
     fprintf(f,(lower_case_id ? "(if " : "(IF "));
     fprint_part_if_instruction(f, if_inst);
     fprintf(f,") ");

}

void fprint_while_instruction(FILE *f,While_Instruction *while_inst)
{
     fprintf(f,(lower_case_id ? "(while " : "(WHILE "));
     fprint_expr(f, while_inst->condition);
     fprint_list_instruction(f,while_inst->insts);
     fprintf(f,") ");
}

void fprint_do_instruction(FILE *f,Do_Instruction *do_inst)
{
     fprintf(f,(lower_case_id ? "(do " : "(DO "));
     fprint_list_instruction(f,do_inst->insts);
     fprintf(f,(lower_case_id ? "while " : "WHILE "));
     fprint_expr(f, do_inst->condition);
     fprintf(f,") ");
}

void fprint_par_instruction(FILE *f,Par_Instruction *par_inst)
{
     if (par_inst->bodys) {
	  Body *body;
	  fprintf(f,"(// ");

	  sl_loop_through_slist(par_inst->bodys, body, Body *) {
	       fprint_body(f, body);
	  }

	  fprintf(f,") ");
     }
}

void fprint_instruction(FILE *f,Instruction *inst)
{
     switch(inst->type) {
     case IT_SIMPLE:
	  fprint_simple_instruction(f,inst->u.simple_inst);
	  break;
     case IT_COMMENT:
	  fprintf(f,"%s",inst->u.comment);
	  break;
     case IT_LABEL:
	  fprintf(f,(lower_case_id ? "label %s" : "LABEL %s"),inst->u.label_inst);
	  break;
     case IT_GOTO:
	  fprintf(f,(lower_case_id ? "goto %s" : "GOTO %s"),inst->u.goto_inst);
	  break;
     case IT_BREAK:
	  fprintf(f,(lower_case_id ? "break" : "BREAK"));
	  break;
     case IT_IF:
	  fprint_if_instruction(f,inst->u.if_inst);
	  break;
     case IT_WHILE:
	  fprint_while_instruction(f,inst->u.while_inst);
	  break;
     case IT_DO:
	  fprint_do_instruction(f,inst->u.do_inst);
	  break;
     case IT_PAR:
	  fprint_par_instruction(f,inst->u.par_inst);
	  break;
     default: 	 
	  fprintf(stderr,LG_STR("Unknown instruction type in fprint_instruction.\n",
				"Unknown instruction type in fprint_instruction.\n")); 
	  break;
     }
}

/* text op sprint functions */
void sprint_instruction(Sprinter *sp, Instruction *inst);

void sprint_list_instruction(Sprinter *sp, List_Instruction insts)
{
     if (insts) {
	  Instruction *inst;

	  sl_loop_through_slist(insts, inst, Instruction *) {
	       sprint_instruction(sp,inst);
	       SPRINT(sp,1,sprintf(f,"\n"));
	  }
     }
}

void sprint_body(Sprinter *sp,Body *body) 
{ 
  if (! body) {
    SPRINT(sp,12,sprintf(f,"(Body *)NULL"));
  } else {
    SPRINT(sp,1,sprintf(f,"("));
    sprint_list_instruction(sp,body->insts);
    SPRINT(sp,1,sprintf(f,")"));
  }
}

void sprint_simple_instruction(Sprinter *sp, Simple_Instruction *simple)
{
     sprint_expr(sp, simple->expr);
}

void sprint_part_if_instruction(Sprinter *sp, If_Instruction *if_inst)
{
     sprint_expr(sp, if_inst->condition);
     sprint_list_instruction(sp,if_inst->then_insts);
     if (if_inst->elseif) {
       SPRINT(sp,7,sprintf(f,(lower_case_id ? "elseif " : "ELSEIF ")));
	       sprint_part_if_instruction(sp,if_inst->u.elseif_inst->u.if_inst);
     } else
	  if (if_inst->u.else_insts) {
	    SPRINT(sp,5,sprintf(f,(lower_case_id ? "else " : "ELSE ")));
	       sprint_list_instruction(sp,if_inst->u.else_insts);
	  }
}

void sprint_if_instruction(Sprinter *sp, If_Instruction *if_inst)
{
  SPRINT(sp,4,sprintf(f,(lower_case_id ? "(if " : "(IF ")));
     sprint_part_if_instruction(sp, if_inst);
     SPRINT(sp,2,sprintf(f,") "));

}

void sprint_while_instruction(Sprinter *sp,While_Instruction *while_inst)
{
  SPRINT(sp,7,sprintf(f,(lower_case_id ? "(while " : "(WHILE ")));
     sprint_expr(sp, while_inst->condition);
     sprint_list_instruction(sp,while_inst->insts);
     SPRINT(sp,2,sprintf(f,") "));
}

void sprint_do_instruction(Sprinter *sp,Do_Instruction *do_inst)
{
  SPRINT(sp,4,sprintf(f,(lower_case_id ? "(do " : "(DO ")));
     sprint_list_instruction(sp,do_inst->insts);
     SPRINT(sp,6,sprintf(f,(lower_case_id ? "while " : "WHILE ")));
     sprint_expr(sp, do_inst->condition);
     SPRINT(sp,2,sprintf(f,") "));
}

void sprint_par_instruction(Sprinter *sp,Par_Instruction *par_inst)
{
     if (par_inst->bodys) {
	  Body *body;
	  SPRINT(sp,4,sprintf(f,"(// "));

	  sl_loop_through_slist(par_inst->bodys, body, Body *) {
	       sprint_body(sp, body);
	  }

	  SPRINT(sp,2,sprintf(f,") "));
     }
}

void sprint_instruction(Sprinter *sp,Instruction *inst)
{
     switch(inst->type) {
     case IT_SIMPLE:
	  sprint_simple_instruction(sp,inst->u.simple_inst);
	  break;
     case IT_COMMENT:
       SPRINT(sp,strlen(inst->u.comment),sprintf(f,"%s",inst->u.comment));
	  break;
     case IT_LABEL:
       SPRINT(sp,6 + strlen(inst->u.label_inst),sprintf(f,(lower_case_id ? "label %s" : "LABEL %s"),inst->u.label_inst));
	  break;
     case IT_GOTO:
       SPRINT(sp,5 + strlen(inst->u.goto_inst),sprintf(f,(lower_case_id ? "goto %s" : "GOTO %s"),inst->u.goto_inst));
	  break;
     case IT_BREAK:
       SPRINT(sp,5,sprintf(f,(lower_case_id ? "break" : "BREAK")));
	  break;
     case IT_IF:
	  sprint_if_instruction(sp,inst->u.if_inst);
	  break;
     case IT_WHILE:
	  sprint_while_instruction(sp,inst->u.while_inst);
	  break;
     case IT_DO:
	  sprint_do_instruction(sp,inst->u.do_inst);
	  break;
     case IT_PAR:
	  sprint_par_instruction(sp,inst->u.par_inst);
	  break;
     default: 	 
	  fprintf(stderr,LG_STR("Unknown instruction type in sprint_instruction.\n",
				"Unknown instruction type in sprint_instruction.\n")); 
	  break;
     }
}
