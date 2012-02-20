
/*                               -*- Mode: C -*-
 * ope-save.c --
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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "ope-graphic.h"
#include "ope-global.h"
#include "oprs-type.h"
#include "oprs-print.h"
#include "oprs-print_f.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#include "ope-edit_f.h"
#include "ope-save_f.h"
#include "ope-op-opf_f.h"
#include "ope-graphic_f.h"

#include "version.h"

void write_edge(FILE * fpo, Edge *edge)
{
     char *type; 
     OG *ogk;
     
     switch (edge->type) {
     case ET_GOAL:
	  type = "GOAL";
	  break;
     case ET_ELSE:
	  type = "ELSE";
	  break;
     case ET_THEN:
	  type = "THEN";
	  break;
     case ET_IF:
	  type = "IF";
	  break;
     default:
	  fprintf(stderr, LG_STR("Untyped edge in write_edge. Defaulting to GOAL.\n",
				 "Untyped edge in write_edge. Defaulting to GOAL.\n"));
	  type = "GOAL";
	  break;
     }

     fprintf(fpo, "(%s %s %s %s", type, edge->in->name, "NN", edge->out->name);

     if ((edge->type == ET_GOAL) ||  (edge->type == ET_IF)) {
	  if (edge->og) {
	       fprintf(fpo, "\n%s\n", edge->og->u.gedge->text->u.gedge_text->log_string);
	       fprintf(fpo, "(");
	       sl_loop_through_slist(edge->og->u.gedge->list_knot, ogk, OG *)
		    fprintf(fpo, "(%d %d) ", ogk->u.gknot->x, ogk->u.gknot->y);
	       fprintf(fpo, ") ");
	       fprintf(fpo, " %d %d %d %d", 
		       edge->og->u.gedge->text->u.gedge_text->dx,
		       edge->og->u.gedge->text->u.gedge_text->dy,
		       edge->og->u.gedge->text->u.gedge_text->text_width,
		       edge->og->u.gedge->text->u.gedge_text->fill_lines);
	  } else {
	       fprint_expr(fpo, edge->expr);
	       fprintf(fpo, "() ");
	       fprintf(fpo, "%d %d %d %d", 0, 0, EDGE_WIDTH, 0);
	  }
     }
     fprintf(fpo, ")\n");
}

void write_node(FILE * fpo, Node *node)
{
     char *type;

     switch (node->type) {
     case NT_START:
	  type = "START";
	  break;
     case NT_END:
	  type = "END";
	  break;
     case NT_PROCESS:
	  type = "PROCESS";
	  break;
     case NT_IF:
	  type = "IF";
	  break;
     case NT_THEN:
	  type = "THEN";
	  break;
     case NT_ELSE:
	  type = "ELSE";
	  break;
     default:
	  fprintf(stderr, LG_STR("Untyped node in write_node. Defaulting to DISJ_DISJ.\n",
				 "Untyped node in write_node. Defaulting to DISJ_DISJ.\n"));
	  type = "PROCESS";
	  break;
     }

     fprintf(fpo, "(%s %s %d %d %d %d)\n", type, node->name, node->join, node->split, (node->og ? node->og->x : 20), (node->og ? node->og->y : 20));
}


void write_fields(FILE * fpo, Op_Structure * op)
{
     fprintf(fpo, "(INVOCATION INVOCATION ");
     if (op->ginvocation) {
	  fprintf(fpo, "%s ", op->ginvocation->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->ginvocation->x, op->ginvocation->y, 1,
		   op->ginvocation->u.gtext->text_width, op->ginvocation->u.gtext->fill_lines);
     } else {
	  fprint_expr(fpo, op->invocation);
	  fprintf(fpo, "%d %d %d %d %d", IP_X, IP_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     fprintf(fpo, "(CONTEXT CONTEXT ");
     if (op->gcontext) {
	  fprintf(fpo, "%s ", op->gcontext->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gcontext->x, op->gcontext->y,
		  op->gcontext->u.gtext->visible,
		  op->gcontext->u.gtext->text_width,
		  op->gcontext->u.gtext->fill_lines);
     } else {
	  fprint_exprlist(fpo, op->context);
	  fprintf(fpo, "%d %d %d %d %d ",  CTXT_X, CTXT_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     fprintf(fpo, "(CALL CALL_NODE ");
     if (op->gcall) {
	  fprintf(fpo, "%s ", op->gcall->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gcall->x, op->gcall->y,
		  op->gcall->u.gtext->visible,
		  op->gcall->u.gtext->text_width,
		  op->gcall->u.gtext->fill_lines);
     } else {
	  fprint_expr(fpo, op->call);
	  fprintf(fpo, "%d %d %d %d %d ",  CALL_X, CALL_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     fprintf(fpo, "(SETTING SETTING ");
     if (op->gsetting) {
	  fprintf(fpo, "%s ", op->gsetting->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gsetting->x, op->gsetting->y,
		  op->gsetting->u.gtext->visible,
		  op->gsetting->u.gtext->text_width,
		  op->gsetting->u.gtext->fill_lines);
     } else {
	  fprint_expr(fpo, op->setting);
	  fprintf(fpo, "%d %d %d %d %d ", SET_X, SET_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     fprintf(fpo, "(EFFECTS EFFECTS ");
     if (op->geffects) {
	  fprintf(fpo, "%s ", op->geffects->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->geffects->x, op->geffects->y,
		  op->geffects->u.gtext->visible,
		  op->geffects->u.gtext->text_width,
		  op->geffects->u.gtext->fill_lines);
     } else {
	  if (op->effects) fprint_exprlist(fpo, op->effects);
	  fprintf(fpo, "%d %d %d %d %d ",  EFF_X, EFF_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     if (op->gaction) {
	  fprintf(fpo, "(ACTION ACTION ");
	  fprintf(fpo, "%s ", op->gaction->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gaction->x, op->gaction->y, 1,
		  op->gaction->u.gtext->text_width,
		  op->gaction->u.gtext->fill_lines);
	  fprintf(fpo, ")\n");
     } else if (op->action) {
	  fprintf(fpo, "(ACTION ACTION ");
	  fprint_action_field(fpo, op->action);
	  fprintf(fpo, "%d %d %d %d %d ",  ACT_X, ACT_Y, 1, 0, 0);
	  fprintf(fpo, ")\n");
     }

     fprintf(fpo, "(DOCUMENTATION DOCUMENTATION ");

     if (op->gdocumentation) {
	  fprintf(fpo, "%s ", op->gdocumentation->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gdocumentation->x, op->gdocumentation->y,
		  op->gdocumentation->u.gtext->visible,
		  op->gdocumentation->u.gtext->text_width,
		  op->gdocumentation->u.gtext->fill_lines);
     } else if (op->documentation) {
	  fprintf(fpo, "%s %d %d %d %d %d ", op->documentation, DOC_X, DOC_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

     fprintf(fpo, "(PROPERTIES PROPERTIES ");
     if (op->gproperties) {
	  fprintf(fpo, "%s ", op->gproperties->u.gtext->string);
	  fprintf(fpo, "%d %d %d %d %d ", op->gproperties->x, op->gproperties->y,
		  op->gproperties->u.gtext->visible,
		  op->gproperties->u.gtext->text_width,
		  op->gproperties->u.gtext->fill_lines);
     } else if (!sl_slist_empty(op->properties)) {
	  fprint_list_property(fpo, op->properties);
	  fprintf(fpo, "%d %d %d %d %d ",  PROP_X, PROP_Y, 1, 0, 0);
     }
     fprintf(fpo, ")\n");

}

void write_gop(FILE * fpo, Op_Structure * op)
{
     Node *node;
     Edge *edge;

     fprintf(fpo, "(%s (\n", op->name);

     write_fields(fpo, op);

     fprintf(fpo, ") (\n");

     sl_loop_through_slist(op->node_list, node, Node *) {
	  write_node(fpo, node);
     }

     fprintf(fpo, ") (\n");

     sl_loop_through_slist(op->edge_list, edge, Edge *) {
	  write_edge(fpo, edge);
     }

     fprintf(fpo, ") )\n");
}

void write_string_and_string_indent(FILE * fpo, char * string_pref, char *string)
{
     PBoolean first = TRUE;
     char *index;
     int indent = strlen(string_pref);

     fprintf(fpo, "%s", string_pref);

     while ((index = strchr(string,'\n'))) {
	  *index= '\0';
	  if (first) {
	       fprintf(fpo,"%s\n", string);
	       first = FALSE;
	  } else {
	       fprintf(fpo,"%*s%s\n", indent , "", string);
	  }
	  *index= '\n';
	  string = index+1;	  
     }
     if (first) {
	  fprintf(fpo,"%s\n", string);
     } else {
	  fprintf(fpo,"%*s%s\n", indent , "", string);
     }
}

void write_top(FILE * fpo, Op_Structure * op)
{
     fprintf(fpo, "\n\
;;;;;;;;;;;;;;;;;;;;;;;;;\n\
;;; %s\n\
;;;;;;;;;;;;;;;;;;;;;;;;;\n", op->name);

     fprintf(fpo, "(defop %s\n", op->name);

     write_string_and_string_indent(fpo, "    :invocation ", op->ginvocation->u.gtext->string);

     if (op->gcall && NOT_EMPTY_STRING(op->gcall->u.gtext->string)) {
	  write_string_and_string_indent(fpo, "    :call ", op->gcall->u.gtext->string);
     }

     if (op->gcontext && NOT_EMPTY_STRING(op->gcontext->u.gtext->string)) {
	  write_string_and_string_indent(fpo, "    :context ", op->gcontext->u.gtext->string);
     }

     if (op->gsetting && NOT_EMPTY_STRING(op->gsetting->u.gtext->string)) {
	  write_string_and_string_indent(fpo, "    :setting ", op->gsetting->u.gtext->string);
     } 

     if (op->gbody) 
	  write_string_and_string_indent(fpo, "    :body ", op->gbody->u.gtext->string);
     else if (op->gaction) 
	  write_string_and_string_indent(fpo, "    :action ", op->gaction->u.gtext->string);

     if (op->geffects && NOT_EMPTY_STRING(op->geffects->u.gtext->string)) {
	  write_string_and_string_indent(fpo, "    :effects ", op->geffects->u.gtext->string);
     }

     if (op->gproperties && NOT_EMPTY_STRING(op->gproperties->u.gtext->string)) {
	  write_string_and_string_indent(fpo, "    :properties ", op->gproperties->u.gtext->string);
     }

     if (op->gdocumentation && NOT_EMPTY_STRING(op->gdocumentation->u.gtext->string)) {
	  fprintf(fpo, "    :documentation %s\n", op->gdocumentation->u.gtext->string);
     }

     fprintf(fpo, ")\n");
}

void write_op(FILE * fpo, Op_Structure * op)
{
     if (op->graphic) write_gop(fpo,op);
     else write_top(fpo,op);
}

#define OP_FILE_FORMAT 2

#include "time.h"

extern time_t time(time_t *tloc);

void write_opfile_header(FILE * fpo, char *name) 
{
     time_t tp;
     char host_name[MAX_HOST_NAME]; /* name of the host */

     if ((tp = time(NULL)) == -1) tp = 0;

     if (gethostname(host_name, MAX_HOST_NAME) != 0) {
	  perror("write_opfile_header: gethostname");
	  host_name[0] = '\0';
     }

     fprintf(fpo, "\
;;;\n\
;;; File: %s\n\
;;; %c%c%c%c\n\
;;; Date: %s\
;;; Host: %s\n\
;;; OP Editor Version: %s\n\
;;; Compiled on %s (%s)\n\
;;; OP File Format: %d\n\
;;;\n",
	     name, '$', 'I', 'd', '$', (tp ? ctime(&tp) : "Unknown\n"), host_name,
	     package_version, host, date, OP_FILE_FORMAT);
}

int write_opfile(char *file_name, OPFile *opfile)
{
     FILE *fpo;
     Op_Structure *op;
     PString sym;
     PBoolean all_text = TRUE;
     PBoolean save_pv = print_var_name;
     PBoolean save_peo = print_english_operator ;

     if (! opfile->backed_up) {
	  char mv_command[BUFSIZ];
	  sprintf(mv_command, "mv -f %s %s.bak >/dev/null 2>&1 ", file_name, file_name);

	  if (system(mv_command) == -1) 
	       perror("write_op_file: system");
	   opfile->backed_up = TRUE;
     }

     if ((fpo = (FILE *) fopen(file_name, "w")) == NULL) {
	  fprintf(stderr, LG_STR("write_opfile: failed fdopen %s\n",
				 "write_opfile: failed fdopen %s\n"), file_name);
	  return -1;
     }

     print_var_name = TRUE;
     print_english_operator = FALSE;

     UpdateMessageWindow(LG_STR("Saving current OP File.",
				"Saving current OP File."));


     sl_loop_through_slist(opfile->list_op, op, Op_Structure *)
	  all_text &= !op->graphic;

     write_opfile_header(fpo, file_name);

     if (all_text) 
	  fprintf(fpo, "(\n");
     else {
	  fprintf(fpo, "(%d \n(\n", OP_FILE_FORMAT);
	  sl_loop_through_slist(opfile->list_sym, sym,PString)
	       fprintf(fpo, "%s\n", sym);

	  fprintf(fpo, ")\n");
     }

     sl_loop_through_slist(opfile->list_op, op, Op_Structure *)
	  write_op(fpo, op);

     fprintf(fpo, ")\n");

     UpdateMessageWindow("Saved current OP File.");

     fclose(fpo);
     report_opfile_saved();

     print_var_name = save_pv;
     print_english_operator = save_peo;
     
     return 0;
}

void copy_op (Op_Structure *op, char *new_name)
{
     FILE *fpo;
     char *save_op_name = op->name;
     char s[LINSIZ];
     PBoolean res, save_bq = be_quiet;

     be_quiet = TRUE;
     if ((fpo = (FILE *) fopen(file_name_for_copy, "w")) == NULL) {
	  fprintf(stderr, LG_STR("Copy_op: fdopen %s\n",
				 "Copy_op: fdopen %s\n"), file_name_for_copy);
	  return;
     }

     UpdateMessageWindow("Duplicating current OP.");
     op->name = new_name;

     write_op(fpo, op);

     fclose(fpo);
     op->name = save_op_name;

     sprintf(s, "copy_op \"%s\"", file_name_for_copy);
     if (res = yyparse_one_command_string(s))
	  UpdateMessageWindow("Current OP duplicated.");
     else
	  UpdateMessageWindow("Current OP NOT duplicated.");

     unlink(file_name_for_copy);
     be_quiet = save_bq;

     sl_sort_slist_func(current_opfile->list_op,sort_op);

}

void write_op_tex(FILE * fpo, Op_Structure * op)
{
  Sprinter *sp = make_sprinter(0);

  fprintf(fpo, "\\item\n\\textbf{%s}\n\n", op->name);

  if (op->graphic)
    fprintf(fpo, LG_STR("A graphic ",
			"A graphic "));
  else
    fprintf(fpo, LG_STR("A text ",
			"A text "));

  if (op->action) {
    fprintf(fpo, LG_STR("action OP.\n\n",
			"action OP.\n\n"));
  } else {
    fprintf(fpo, LG_STR("OP.\n\n",
			"OP.\n\n"));
  }
  fprintf(fpo, "\\begin{description}\n\n");
  if (op->invocation) {
    reset_sprinter(sp);
    sprint_expr(sp,op->invocation);
    fprintf(fpo, "\\item[Invocation:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n", SPRINTER_STRING(sp));
       
  }

  if (op->call) { 		/*  && NOT_EMPTY_STRING(op->gcall->u.gtext->string)) */
    reset_sprinter(sp);
    sprint_expr(sp,op->call);
    fprintf(fpo, "\\item[Call:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n",  SPRINTER_STRING(sp));
  }

  if (op->context) { /* && NOT_EMPTY_STRING(op->gcontext->u.gtext->string)) */
    reset_sprinter(sp);
    sprint_exprlist(sp,op->context);
    fprintf(fpo, "\\item[Context:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n", SPRINTER_STRING(sp));
  }

  if (op->setting){ /*  && NOT_EMPTY_STRING(op->gsetting->u.gtext->string)) */
    reset_sprinter(sp);
    sprint_expr(sp,op->setting);
    fprintf(fpo, "\\item[Setting:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n",  SPRINTER_STRING(sp));
  }

  if (op->effects) { /*  && NOT_EMPTY_STRING(op->geffects->u.gtext->string)) */
    reset_sprinter(sp);
    sprint_exprlist(sp,op->effects);
    fprintf(fpo, "\\item[Effects:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n",  SPRINTER_STRING(sp));
  }

  if (op->action) {
    reset_sprinter(sp);
    sprint_action_field(sp,op->action);
    fprintf(fpo, "\\item[Action:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n", SPRINTER_STRING(sp));
  }

  if (op->body) {
    reset_sprinter(sp);
    sprint_body(sp,op->body);
    fprintf(fpo, "\\item[Body:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n",  SPRINTER_STRING(sp));
  }

  if (!sl_slist_empty(op->properties)) {
    reset_sprinter(sp);
    sprint_list_property(sp,op->properties);
    fprintf(fpo, "\\item[Properties:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n",SPRINTER_STRING(sp));
  }

  if (op->documentation) {
    fprintf(fpo, "\\item[Documentation:]\n\n\\begin{verbatim}\n%s\n\\end{verbatim}\n\n", op->documentation);
  }

  fprintf(fpo, "\\end{description}\n\n");
}

int write_opfile_tex(char *file_name, OPFile *opfile)
{
     FILE *fpo;
     Op_Structure *op;
     PBoolean save_pv = print_var_name;
     PBoolean save_pp = pretty_print;

     if ((fpo = (FILE *) fopen(file_name, "w")) == NULL) {
	  fprintf(stderr, "write_opfile_tex: failed fdopen %s\n", file_name);
	  return -1;
     }
     pretty_print = TRUE;
     print_var_name = TRUE;

     fprintf(fpo, "\\begin{itemize}\n\n");

     sl_loop_through_slist(opfile->list_op, op, Op_Structure *)
	  write_op_tex(fpo, op);


     fprintf(fpo, "\\end{itemize}\n\n");

     fclose(fpo);

     print_var_name = save_pv;
     pretty_print = save_pp;
     
     return 0;
}

long int cs_node(Node *n)
{
     return 1L; 
}

long int cs_edge(Edge *e)
{
     return 1L; 
}

long int cs_fields(Op_Structure *op)
{
     return 1L; 
}

long int cs_gop(Op_Structure * op)
{
     long int res = 1;
     Node *node;
     Edge *edge;

     res += cs_fields(op);

     sl_loop_through_slist(op->node_list, node, Node *) {
	  res += 23 * cs_node(node);
     }

     sl_loop_through_slist(op->edge_list, edge, Edge *) {
	  res += 29 * cs_edge(edge);
     }

     return res;
}
