static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * oprs-sprint.c -- 
 * 
 * Copyright (c) 1991-2010 Francois Felix Ingrand.
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
#include "oprs-error.h"
#include "oprs-rerror.h"
#include "oprs-pred-func.h"
#include "oprs-sprint.h"
#include "type.h"

#include "oprs-sprint_f.h"
#include "op-instance_f.h"
#include "intention_f.h"
#include "fact-goal_f.h"
#include "lisp-list_f.h"
#include "oprs-error_f.h"
#include "oprs-rerror_f.h"

Sprinter *make_sprinter(int size)
{
     Sprinter *res = MAKE_OBJECT(Sprinter);

     if (size == 0) size = 128;
     SPRINTER_STRING(res) = (char *)MALLOC(size);
     SPRINTER_STRING(res)[0] = '\0'; /* In case we do not write anything... */
     SPRINTER_SIZE(res) = 0;
     SPRINTER_TSIZE(res) = size;

     return res;
}

void enlarge_sprinter(Sprinter *sp, unsigned int min)
{
     if (SPRINTER_TSIZE(sp) < min) 
	  SPRINTER_TSIZE(sp) = 2 * min;
     else
	  SPRINTER_TSIZE(sp) *= 2;
     SPRINTER_STRING(sp) = (char *)REALLOC(SPRINTER_STRING(sp), SPRINTER_TSIZE(sp));
}

void free_sprinter(Sprinter *sp)
{
     FREE(SPRINTER_STRING(sp));
     FREE(sp);
}

char *sprinter_string(Sprinter *sp)
{
     return SPRINTER_STRING(sp);
}

int sprinter_size(Sprinter *sp)
{
     return SPRINTER_SIZE(sp);
}

char *sprinter_cur_pos(Sprinter *sp)
{
     return SPRINTER_CUR_POS(sp);
}

int sprinter_remaining_size(Sprinter *sp)
{
     return SPRINTER_REMAINING_SIZE(sp);
}

void add_sprinter_size(Sprinter *sp, int i)
{
     SPRINTER_SIZE(sp) += i;
}

void reset_sprinter(Sprinter *sp)
{
     SPRINTER_STRING(sp)[0] = '\0'; /* In case we do not write anything... */
     SPRINTER_SIZE(sp) = 0;
}

void sprint_int_array(Sprinter *sp, Int_array *ia) 
{
     PBoolean first = TRUE;
     unsigned short i;

     SPRINT(sp,1,sprintf(f,"["));
     for(i = 0; i < ia->size ; i++) {
	  if (! first) {
	       SPRINT(sp,1,sprintf(f," "));
	  } else  first = FALSE;

	  SPRINT(sp,MAX_PRINTED_INT_SIZE,sprintf(f,"%d",ia->array[i]));
     }
     SPRINT(sp,1,sprintf(f,"]"));
}

void sprint_float_array(Sprinter *sp, Float_array *fa) 
{
     PBoolean first = TRUE;
     unsigned short i;
     
     SPRINT(sp,1,sprintf(f,"["));
     for(i = 0; i < fa->size ; i++) {
	  if (! first) {
	       SPRINT(sp,1,sprintf(f," "));
	  } else  first = FALSE;

	  SPRINT(sp,MAX_PRINTED_DOUBLE_SIZE,sprintf(f,print_float_format,fa->array[i]));
     }
     SPRINT(sp,1,sprintf(f,"]"));
}

void sprint_l_list(Sprinter *sp, L_List l)
{
     L_List tmp_list;

     SPRINT(sp,2,sprintf(f,"(."));
     
     for(tmp_list = l; tmp_list != l_nil ; tmp_list = CDR(tmp_list)) {
	  SPRINT(sp,1,sprintf(f," "));
	  sprint_term(sp,CAR(tmp_list));
     }
     SPRINT(sp,3,sprintf(f," .)"));
}

void sprint_property(Sprinter *sp, Property *p)
{
	 SPRINT(sp,strlen(p->name) + 2,sprintf(f,"(%s ", p->name));
     sprint_term(sp, p->value);
     SPRINT(sp,1,sprintf(f,")")); 
}

void sprint_list_property(Sprinter *sp, PropertyList pl)
{
	 PBoolean first = TRUE;
	 Property *p;
     
     SPRINT(sp,1,sprintf(f,"("));

     sl_loop_through_slist(pl,p, Property *) {
	  if (! first) {
	       SPRINT(sp,1,sprintf(f," "));
	  } else first = FALSE;
	  sprint_property(sp, p);
     }
     SPRINT(sp,1,sprintf(f,")"));
}

void sprint_var_list(Sprinter *sp,VarList vl)
{
     PBoolean first = TRUE;
     Envar *v;

     SPRINT(sp,1,sprintf(f,"("));
     sl_loop_through_slist(vl,v, Envar *)  {
	  if (! first) {
	       SPRINT(sp,1,sprintf(f," "));
	  } else  first = FALSE;

	  sprint_envar(sp,v);
     }
     SPRINT(sp,1,sprintf(f,")"));
}

void sprint_frame(Sprinter *sp,FramePtr fp)
{
     if (fp==empty_env) {
	  SPRINT(sp,2,sprintf(f,"{}"));
     } else {
	  SPRINT(sp,1,sprintf(f,"{"));

	  if (fp->installe) SPRINT(sp,2,sprintf(f,"1 "));
	  else SPRINT(sp,2,sprintf(f,"0 "));

	  sprint_undo_list(sp,fp->list_undo);

	  SPRINT(sp,1,sprintf(f," "));

	  sprint_frame(sp,fp->previous_frame);

	  SPRINT(sp,1,sprintf(f,"}"));
     } 
}

void sprint_lenv(Sprinter *sp,List_Envar le)
{
     Envar *ptr1;

     if (sl_slist_empty(le)) {
	  SPRINT(sp,2,sprintf(f,"()"));
     } else {
	  PBoolean first = TRUE; 

	  SPRINT(sp,1,sprintf(f,"("));
	  sl_loop_through_slist(le, ptr1, Envar *) {
	       if (! first) {
		    SPRINT(sp,1,sprintf(f," "));
	       } else  first = FALSE;

	       sprint_envar(sp,(Envar *)ptr1);
	  }
	  SPRINT(sp,1,sprintf(f,")"));
     } 
}

void sprint_envar(Sprinter *sp,Envar *ev)
{
     if (print_var_value && ev->value)
	  sprint_term(sp,ev->value);
     else if (print_var_name || print_var_value) { /* ev->value was NULL */
	  SPRINT(sp,strlen(ev->name),sprintf(f,"%s", ev->name));
     } else {

	  if (ev->unif_type)
	       SPRINT(sp,strlen (ev->name)+strlen (ev->unif_type->name)+4,sprintf(f,"{%s:%s->", ev->name, ev->unif_type->name));
	  else
	       SPRINT(sp,strlen (ev->name)+3,sprintf(f,"{%s->", ev->name));

	  sprint_term(sp,ev->value);
	  
	  SPRINT(sp,1,sprintf(f,"}"));
     }
}

void sprint_undo_list(Sprinter *sp,List_Undo lu)
{

     Undo *u;
     
     if (SAFE_SL_SLIST_EMPTY(lu)){
	  SPRINT(sp,2,sprintf(f,"()"));
     } else {
	  PBoolean first = TRUE;
	  SPRINT(sp,1,sprintf(f,"("));
	  sl_loop_through_slist(lu, u, Undo *) {
	       if (! first) {
		    SPRINT(sp,1,sprintf(f," "));
	       } else first = FALSE;
	       sprint_undo(sp,u);
	  }
	  SPRINT(sp,1,sprintf(f,")"));
	 }
}

void sprint_undo(Sprinter *sp,Undo *ud)
{
     SPRINT(sp,1,sprintf(f,"["));
     
     sprint_envar(sp,ud->envar);

     SPRINT(sp,2,sprintf(f,">>"));

     sprint_term(sp,ud->value);

     SPRINT(sp,1,sprintf(f,"]"));
}

void sprint_pred_func_rec(Sprinter *sp, Pred_Func_Rec *pr)
{
     if (pr->name) {
	  SPRINT(sp,strlen(pr->name)+1,sprintf(f,"%s", pr->name));
     } else {
	 sprint_envar(sp, pr->u.envar);
     }
}

void sprint_expr(Sprinter *sp,Expression *expr)
{

     SPRINT(sp,1,sprintf(f,"("));
     sprint_pred_func_rec(sp,expr->pfr);
     if (!sl_slist_empty(expr->terms)) {
	  SPRINT(sp,1,sprintf(f," "));
	  sprint_tl(sp,expr->terms);
     }
     SPRINT(sp,1,sprintf(f,")"));
}

void sprint_expr_frame(Sprinter *sp,ExprFrame *ef)
{
     SPRINT(sp,1,sprintf(f,"["));
     sprint_expr(sp,ef->expr);
     
     SPRINT(sp,1,sprintf(f," "));
     sprint_frame(sp,ef->frame);
     
     SPRINT(sp,1,sprintf(f,"]"));
     
}

void sprint_tl(Sprinter *sp,TermList tl)
{
     Term *t;
     PBoolean first = TRUE;
     sl_loop_through_slist(tl, t, Term *) {
	  if (! first) SPRINT(sp,1,sprintf(f," "));
	  else first = FALSE;
	  sprint_term(sp,t);
     }
}

void sprint_list_expr(Sprinter *sp,ExprList tl)
{
     Expression *e;
     PBoolean first = TRUE;
     if (tl) {
	  sl_loop_through_slist(tl, e, Expression *) {
	       if (! first) SPRINT(sp,1,sprintf(f," "));
	       else first = FALSE;
	       sprint_expr(sp,e);
	  }
     }
}

void sprint_exprlist(Sprinter *sp,ExprList tl)
{
     SPRINT(sp,1,sprintf(f,"("));
     sprint_list_expr(sp,tl);
     SPRINT(sp,1,sprintf(f,")"));
}

void sprint_backslash_string(Sprinter *sp, PString string)
{
     int i; 
     char c;

     SPRINT(sp,1,sprintf(f,"\""));

     for (i = 0;  i < (int)strlen(string); i++) {
	  switch (c = string[i]) {
	  case '"':
	       SPRINT(sp,2,sprintf(f,"\\\""));
	       break;
	  case '\\':
	       SPRINT(sp,2,sprintf(f,"\\\\"));
	       break;
	  case '\n':
	       if (replace_cr) {
		    SPRINT(sp,2,sprintf(f,"\\n"));
	       } else {
		    SPRINT(sp,1,sprintf(f,"%c",c));
	       }
	       break;
	  case '\t':
	       if (replace_cr) {
		    SPRINT(sp,2,sprintf(f,"\\t"));
	       } else {
		    SPRINT(sp,1,sprintf(f,"%c",c));
	       }
	       break;
	  default:
	       SPRINT(sp,1,sprintf(f,"%c",c));
	  }
     }
     SPRINT(sp,1,sprintf(f,"\""));
}

void sprint_term(Sprinter *sp,Term *term)
{

     if (!term) {
	  SPRINT(sp,4,sprintf(f,"NULL"));
     } else {
	  PBoolean first = TRUE; 
	  int i;
	  switch (term->type) {
	  case INTEGER: 	 
	       SPRINT(sp,MAX_PRINTED_INT_SIZE, sprintf(f,"%d",term->u.intval));
	       break;
	  case LONG_LONG: 	 
	       SPRINT(sp,MAX_PRINTED_INT_SIZE, sprintf(f,"%lldll",term->u.llintval));
	       break;
	  case TT_FACT:
	       sprint_fact(sp, term->u.fact);
	       break;
	  case TT_GOAL:
	       sprint_goal(sp,term->u.goal);
	       break;
	  case TT_OP_INSTANCE:
	       sprint_op_instance(sp,term->u.opi);
	       break;
	  case TT_INTENTION:
	       sprint_intention(sp, term->u.in);
	       break;
	  case U_POINTER: 	 
	       SPRINT(sp,MAX_PRINTED_POINTER_SIZE,sprintf(f,"%p",term->u.u_pointer));
	       break;
	  case U_MEMORY: 	 
	       SPRINT(sp,MAX_PRINTED_POINTER_SIZE,sprintf(f,"<%p %d>",
							  term->u.u_memory->mem,
							  term->u.u_memory->size));
	       break;
	  case TT_FLOAT: 	 
	       SPRINT(sp,MAX_PRINTED_DOUBLE_SIZE,sprintf(f,print_float_format,*(term->u.doubleptr)));
	       break;
	  case STRING:
	       if (print_backslash)
		    sprint_backslash_string(sp,term->u.string);
	       else
		    SPRINT(sp,strlen(term->u.string),sprintf(f,"%s",term->u.string));
	       break;
	  case TT_ATOM:
	       SPRINT(sp,strlen(term->u.id),sprintf(f,"%s",term->u.id));
	       break;
	  case VARIABLE:
	       sprint_envar(sp,term->u.var);
	       break;
	  case EXPRESSION:
	       sprint_expr(sp,term->u.expr);
	       break;
/* 	  case LENV: */
/* 	       sprint_var_list(sp,term->u.var_list); */
/* 	       break; */
	  case INT_ARRAY: 	 
	       sprint_int_array(sp,term->u.int_array);
	       break;
	  case FLOAT_ARRAY: 	 
	       sprint_float_array(sp,term->u.float_array);
	       break;
	  case LISP_LIST: 	 
	       sprint_l_list(sp,term->u.l_list);
	       break;
	  case C_LIST:
	       SPRINT(sp,1,sprintf(f,"("));
               for(i = sl_slist_length(term->u.c_list); i > 0; i--) {
		    if (! first) {
			 SPRINT(sp,1,sprintf(f," "));
		    } else first = FALSE;
		    
		    SPRINT(sp,1,sprintf(f,"."));
	       }
	       SPRINT(sp,1,sprintf(f,")"));
	       break;
	  default: 	 
	       oprs_perror("sprint_term", PE_UNEXPECTED_TERM_TYPE);
	       break;
	  }
     }
}
