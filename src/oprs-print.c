/*                               -*- Mode: C -*- 
 * oprs-print.c -- Fonction de construction et de print pour les types...
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

#include "string.h"
#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "oprs-print.h"
#include "oprs-error.h"
#include "oprs-pred-func.h"
#include "type.h"

#include "oprs-print_f.h"
#include "lisp-list_f.h"
#include "oprs-error_f.h"
#include "fact-goal_f.h"
#include "op-instance_f.h"
#include "intention_f.h"

char *print_float_format = "%f";
PBoolean print_english_operator = FALSE;
PBoolean pretty_print = FALSE;
PBoolean print_backslash = TRUE;
PBoolean pretty_fill = FALSE;
PBoolean print_var_value = FALSE; /* This flag control if we should print the variable value only
				       (for message passing for example)  */
PBoolean print_var_name = FALSE; /* Just the var name or the whole thing */

PBoolean replace_cr = TRUE;  /* This flag control if we replace <carriege return>
				  by the string "\n" */


int pretty_indent = 0;		/* How much to indent according to the leader */
int new_line_index = 0;

/* Number of element in a list printed */
int print_length = 0;		/* 0 means all the element */

void fprint_property(FILE *f,Property *p)
{
     fprintf(f,"(%s ", p->name);
     fprint_term(f,p->value);
     fprintf(f,")");
}

void fprint_list_property(FILE *f,PropertyList pl)
{
     Property *p;
     PBoolean first = TRUE;
     
     fprintf(f,"(");
     sl_loop_through_slist(pl,p, Property *) {
	  if (! first) {
	       fprintf(f," ");
	  }  else first = FALSE;
	  fprint_property(f,p);
     }
     fprintf(f,")");
}

void fprint_var_list(FILE *f,VarList vl)
{
     PBoolean first = TRUE;
     Envar *v;
     
     fprintf(f,"(");
     sl_loop_through_slist(vl,v, Envar *) {
	  if (! first) {
	       fprintf(f," ");
	  }  else first = FALSE;
	  fprint_envar(f,v);
     } 
     fprintf(f,")");
}

void fprint_frame(FILE *f,FramePtr fp)
{
     if (fp==empty_env) fprintf(f,"{}");
     else {
	  fprintf(f,"{");
	  if (fp->installe)
	       fprintf(f,"1 ");
	  else fprintf(f,"0 ");
	  fprint_undo_list(f,fp->list_undo);
	  fprint_frame(f,fp->previous_frame);
	  fprintf(f,"}");
	  
     } 
}

void fprint_lenv(FILE *f,List_Envar le)
{
     Envar *ptr1;

     if (sl_slist_empty(le)) fprintf(f,"()");
     else {
	  PBoolean first = TRUE; 

	  fprintf(f,"(");
	  sl_loop_through_slist(le, ptr1, Envar *) {
	       if (! first) {
		    fprintf(f," ");
	       }  else first = FALSE;

	       fprint_envar(f,(Envar *)ptr1);
	  }
	  fprintf(f,")");
     } 
}

void fprint_envar(FILE *f,Envar *ev)
{
     if (print_var_value && ev->value) {
	  fprint_term(f,ev->value);
     } else if (print_var_name || print_var_value) {
	  fprintf(f,"%s", ev->name);
     } else {
	  if (ev->unif_type)
	       fprintf(f,"{%s:%s->", ev->name, ev->unif_type->name);
	  else 
	       fprintf(f,"{%s->", ev->name);
	  fprint_term(f,ev->value);
	  fprintf(f,"}");
     }
}

void fprint_undo_list(FILE *f,List_Undo lu)
{
     Undo *u;
     
     if (SAFE_SL_SLIST_EMPTY(lu)) fprintf(f,"()");
     else {
	  PBoolean first = TRUE;

	  fprintf(f,"(");
	  sl_loop_through_slist(lu, u, Undo *) {
	       if (! first) {
		    fprintf(f," ");
	       }  else first = FALSE;
	       fprint_undo(f,u);
	  }
	  fprintf(f,")");
     }
}

void fprint_undo(FILE *f,Undo *ud)
{
     fprintf(f,"[");
     fprint_envar(f,ud->envar);
     fprintf(f,">>");
     fprint_term(f,ud->value);
     fprintf(f,"]");
}

void fprint_int_array(FILE *f,  Int_array *ia)
{
     PBoolean first = TRUE;
     unsigned short i; 

     fprintf(f,"[");
     for(i = 0; i < ia->size ; i++) {
	  if (! first) {
	       fprintf(f," ");
	  }  else first = FALSE;
	  fprintf(f,"%d", ia->array[i]);
     }
     fprintf(f," ]");
}

void fprint_float_array(FILE *f,  Float_array *fa)
{
     PBoolean first = TRUE;
     unsigned short i; 

     fprintf(f,"[");
     for(i = 0; i < fa->size ; i++) {
	  if (! first) {
	       fprintf(f," ");
	  }  else first = FALSE;
	  fprintf(f,print_float_format, fa->array[i]);
     }
     fprintf(f,"]");
}

void fprint_pred_func_rec(FILE *f, Pred_Func_Rec *pr)
{
     if (pr->name) {
	  fprintf(f,"%s", pr->name);
     } else {
	  fprint_envar(f, pr->u.envar);
     }
}

void fprint_expr(FILE *f,Expression *expr)
{
     if (expr == NULL) {
	  if (print_var_name) {
	       /* fprintf(f,""); NO-OP */
	  } else { 
	       fprintf(f,"(Expr *)NULL");
	  }
     } else {
	  fprintf(f,"(");
	  fprint_pred_func_rec(f, expr->pfr);
	  fprint_tl(f,expr->terms);
	  fprintf(f,")");
     }
}

void fprint_action_field(FILE *f,Action_Field *ac) 
{ 
     if (ac == NULL) {
	  if (! print_var_name) {
	       fprintf(f,"(Action_Field *)NULL");
	  }
     } else { 
	  if (ac->special) {
	       fprintf(f,"( %s ", spec_act_sym);
	       if (ac->multi)
		    fprint_lenv(f,ac->u.list_envar);
	       else
		    fprint_envar(f,ac->u.envar);
	  }
	  fprint_expr(f,ac->function);
	  if (ac->special) {
	       fprintf(f,")");
	  }
     }
}

void fprint_tl(FILE *f,TermList tl)
{
     Term *t;
     
     sl_loop_through_slist(tl, t, Term *) {
	  fprintf(f," ");
	  fprint_term(f,t);
     }
}


void fprint_list_expr(FILE *f,ExprList tl)
{
     Expression *t;
     
     if (tl) {
	  sl_loop_through_slist(tl, t, Expression *) {
	       fprintf(f," ");
	       fprint_expr(f,t);
	  }
     }
}

void fprint_exprlist(FILE *f,ExprList tl)
{
     fprintf(f,"(");
     fprint_list_expr(f,tl);
     fprintf(f,")");
}

/* void fprint_expr(FILE *f,Expression *tc) */
/* { */
/*      fprintf(f,"("); */
/*      fprint_pred_func_rec(f,tc->pfr); */
/*      fprint_tl(f,tc->terms); */
/*      fprintf(f,")"); */
/* } */

void fprint_l_list(FILE *f, L_List l)
{
     PBoolean first = TRUE;
     L_List tmp;

     fprintf(f,"(.");
     for(tmp = l; tmp != l_nil ; tmp = CDR(tmp)) {
	  if (! first) {
	       fprintf(f," ");
	  }  else first = FALSE;
          fprint_term(f,CAR(tmp));
     }
     fprintf(f," .)");
}

void fprint_backslash_string(FILE *f, PString string)
{
     int i; 
     char c;

     fprintf(f,"\"");

     for (i = 0; i < (int)strlen(string); i++) {
	  switch (c = string[i]) {
	       case '"':
		    fprintf(f,"\\\"");
		    break;
	       case '\\':
		    fprintf(f,"\\\\");
		    break;
	       default:
		    fprintf(f,"%c",c);
	       }
     }
     fprintf(f,"\"");
}


void fprint_term(FILE *f,Term *term)
{
     if (!term) {
	  fprintf(f,"NULL");
     } else
	  switch (term->type) {
	       int i;
	  case INTEGER: 	 
	       fprintf(f,"%d",term->u.intval);
	       break;
	  case LONG_LONG: 	 
	       fprintf(f,"%lldll",term->u.llintval);
	       break;
	  case TT_FACT:
	       fprint_fact(f, term->u.fact);
	       break;
	  case TT_GOAL:
	       fprint_goal(f,term->u.goal);
	       break;
	  case TT_OP_INSTANCE:
	       fprint_op_instance(f,term->u.opi);
	       break;
	  case TT_INTENTION:
	       fprint_intention(f, term->u.in);
	       break;
	  case U_POINTER: 	 
	       fprintf(f, "%p", term->u.u_pointer);
	       break;
	  case U_MEMORY: 	 
	       fprintf(f,"<%p %d>", term->u.u_memory->mem,
		       term->u.u_memory->size);
	       break;
	  case TT_FLOAT: 	 
	       fprintf(f,print_float_format,*(term->u.doubleptr));
	       break;
	  case STRING:
	       if (print_backslash)
		    fprint_backslash_string(f, term->u.string);
	       else
		    fprintf(f,"%s",term->u.string);
	       break;
	  case TT_ATOM:
	       fprintf(f,"%s",term->u.id);
	       break;
	  case VARIABLE:
	       fprint_envar(f,term->u.var);
	       break;
	  case EXPRESSION:
	       fprint_expr(f,term->u.expr);
	       break;
/* 	  case LENV: */
/* 	       fprint_var_list(f,term->u.var_list); */
/* 	       break; */
	  case INT_ARRAY: 	 
	       fprint_int_array(f,term->u.int_array);
	       break;
	  case FLOAT_ARRAY: 	 
	       fprint_float_array(f,term->u.float_array);
	       break;
	  case LISP_LIST: 	 
	       fprint_l_list(f,term->u.l_list);
	       break;
	  case C_LIST:
	       fprintf(f,"(");
               for(i = sl_slist_length(term->u.c_list); i > 0; i--)
		    fprintf(f,".");
	       fprintf(f,")");
	       break;
	  default: 	 
	       oprs_perror("fprint_term", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
}

void fprint_expr_frame(FILE *f,ExprFrame *ef)
{
     fprintf(f,"[");
     fprint_expr(f,ef->expr);
     fprint_frame(f,ef->frame);
     fprintf(f,"]");
}

void print_property(Property *p)
{
      fprint_property(stdout,p);
}

void print_list_property(PropertyList pl)
{
      fprint_list_property(stdout,pl);
}

void print_frame(FramePtr fp)
{
      fprint_frame(stdout,fp);
}

void print_lenv(List_Envar le)
{
      fprint_lenv(stdout,le);
}

void print_envar(Envar *ev)
{
      fprint_envar(stdout,ev);
}

void print_undo_list(List_Undo lu)
{
      fprint_undo_list(stdout,lu);
}

void print_undo(Undo *ud)
{
      fprint_undo(stdout,ud);
}

void print_expr(Expression *expr)
{
      fprint_expr(stdout,expr);
}

void print_action_field(Action_Field *ac)
{
     fprint_action_field(stdout,ac);
}

void print_expr_frame(ExprFrame *ef)
{
      fprint_expr_frame(stdout,ef);
}

void print_term(Term *t)
{
      fprint_term(stdout,t);
}

void print_tl(TermList tl)
{
      fprint_tl(stdout,tl);
}

void print_list_expr(ExprList tl)
{
      fprint_list_expr(stdout,tl);
}

void print_exprlist(ExprList tl)
{
      fprint_exprlist(stdout,tl);
}
