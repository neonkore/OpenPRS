/*                               -*- Mode: C -*- 
 * op-structure.h -- definition of op-structure
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

#ifndef INCLUDE_op_structure
#define INCLUDE_op_structure

#include "opaque.h"
#include "oprs-type.h"
#include "oprs-profiling.h"

typedef enum {NT_PROCESS, NT_START, NT_END,  NT_ELSE, NT_THEN, NT_IF} Node_Type;

typedef enum {IT_SIMPLE, IT_IF, IT_WHILE, IT_DO, IT_PAR, IT_COMMENT, IT_LABEL, IT_GOTO, IT_BREAK} Instruction_Type;

typedef enum {FT_INVOCATION, FT_CONTEXT, FT_SETTING, FT_PROPERTIES, FT_DOCUMENTATION, FT_EFFECTS, FT_ACTION, FT_BODY, FT_CALL } Field_Type;

#define START_NODE_P(n) (((n)->type) == NT_START)

#define END_NODE_P(n) (((n)->type) == NT_END)

#define IF_THEN_ELSE_NODE_P(n) ((((n)->type) == NT_IF) || \
				(((n)->type) == NT_THEN) || \
				(((n)->type) == NT_ELSE))

#define NODE_NAME_OG(og) ((og)->u.gnode->node->name)

typedef enum {ET_GOAL, ET_IF, ET_ELSE, ET_THEN} Edge_Type;

typedef Slist *List_Instruction;

typedef struct if_instruction If_Instruction;
typedef struct instruction Instruction;

typedef Slist *List_Body;

struct body {
     List_Instruction	insts;
};

typedef struct simple_instruction {
     Expression *expr;
#ifdef GRAPHIX
     OG *og;
#endif
} Simple_Instruction;

struct if_instruction {
     Expression *condition;
     List_Instruction	then_insts;
     PBoolean elseif;
     union{
	  List_Instruction else_insts;
	  Instruction *elseif_inst;
     } u;
     List_Instruction	else_insts;
#ifdef GRAPHIX
     OG *og;
#endif
};

typedef struct while_instruction {
     Expression *condition;
     List_Instruction	insts;
#ifdef GRAPHIX
     OG *og;
#endif
} While_Instruction;

typedef struct while_instruction Do_Instruction;

typedef struct Par_instruction {
     List_Body bodys;
} Par_Instruction;

struct instruction {
     Instruction_Type type;
     union {
	  Simple_Instruction *simple_inst;
	  If_Instruction *if_inst;
	  While_Instruction *while_inst;
	  Par_Instruction *par_inst;
	  Do_Instruction *do_inst;
	  PString comment;
	  Symbol label_inst;
	  Symbol goto_inst;
     } u;	  
};

struct  op_structure {
     Symbol name;
     char *file_name;
     Expression *invocation;
     Expression *call;
     ExprList context;
     Expression *setting;	/* rename */
     Add_Del_List effects;
     NodeList node_list;
     EdgeList edge_list;
     Control_Point *start_point;
     Action_Field *action;
     Body *body;
     List_Envar list_var;
     PropertyList properties;
     Slist *node_name_list;

#ifdef OPRS_PROFILING
     PDate time_active;
     unsigned int nb_relevant;
     unsigned int nb_applicable;
     unsigned int nb_intended;
     unsigned int nb_succes;
     unsigned int nb_failure;
#endif

#ifdef GRAPHIX
     PString documentation;
#ifdef GTK
     char *xms_name;
#else
  XmString xms_name;
#endif
     OG *ginvocation;
     OG *gcall;
     OG *gcontext;
     OG *gsetting;
     OG *geffects;
     OG *gproperties;
     OG *gdocumentation;
     OG *gaction;
     OG *gbody;
     OG *op_title;

     List_OG      	list_og_node;
     List_OG      	list_og_edge;
     List_OG      	list_og_edge_text;
     List_OG      	list_og_text;
     List_OG      	list_og_inst;
     List_OG      	list_movable_og;
     List_OG      	list_destroyable_og;
     List_OG      	list_editable_og;

     int last_view_x, last_view_y;
     unsigned int graphic BITFIELDS(:1);
     unsigned int graphic_traced BITFIELDS(:1);
#endif
     unsigned int text_traced BITFIELDS(:1);
     unsigned int step_traced BITFIELDS(:1);
};

typedef Slist *Op_Structure_List;

typedef struct goto_label_edge {
     Symbol name;
     Edge *edge;
} Goto_Label_Edge;

struct control_point {
     LogicList out;
     LogicList in;
     Node_Type type BITFIELDS(:4);
     unsigned int join BITFIELDS(:1);
     unsigned int split BITFIELDS(:1);
#ifdef GRAPHIX
     Symbol name;
     OG *og;
#endif
};

struct logic {
     Expression *expr;
     Control_Point *out;
     Edge_Type type;
#ifdef GRAPHIX
     Control_Point *in;
     OG *og;
#endif
};

typedef struct double_node {
     Control_Point *head;
     Control_Point *tail;
     union {
	  Instruction *inst;
	  List_Instruction list_inst;
	  Body *body;
     } u;
} Double_Node;

typedef struct node_name {
     Node *node;
     Symbol name;
} Node_Name;

extern char *current_graph_file_name;
extern Slist *load_op_list;	/* liste of name of OPs to load, if applicable */

extern Op_Structure *current_op;

extern Op_Structure *goal_satisfied_in_db_op_ptr;
extern Op_Structure *goal_for_intention_op_ptr;
extern Op_Structure *goal_waiting_op_ptr;
extern PBoolean really_build_node;
#ifdef GRAPHIX
extern List_OG current_list_og_inst;
extern int current_body_indent;
extern int current_body_line;
/* test for strings in body, TO DELETE
extern int nb_cr_in_strings;
*/
#endif

extern PBoolean compile_graphix;

#endif /* INCLUDE_op_structure */
