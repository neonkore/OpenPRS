/*                               -*- Mode: C -*- 
 * oprs-print_f.h -- Declaration of external function to oprs-type.c
 * 
 * $Id$
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

#include "oprs-print_f-pub.h"

void fprint_int_array(FILE *f,  Int_array *ia);
void fprint_float_array(FILE *f,  Float_array *fa);

void fprint_property(FILE *f,Property *p);
void fprint_list_property(FILE *f,PropertyList pl);
void fprint_frame(FILE *f,FramePtr fp);
void fprint_lenv(FILE *f,List_Envar le);
void fprint_envar(FILE *f,Envar *ev);
void fprint_undo_list(FILE *f,List_Undo lu);
void fprint_undo(FILE *f,Undo *ud);
void fprint_expr(FILE *f,Expression *expr);
void fprint_action_field(FILE *f,Action_Field *ac);
void fprint_expr_frame(FILE *f,ExprFrame *ef);
void fprint_term(FILE *f,Term *term);
void fprint_tl(FILE *f,TermList tl);
void fprint_l_list(FILE *f, L_List l);
void fprint_unknown_lisp_object(FILE *f, L_List l);
void fprint_exprlist(FILE *f,ExprList tl);
void fprint_list_expr(FILE *f,ExprList tl);


void print_property(Property *p);
void print_list_property(PropertyList pl);
void print_frame(FramePtr fp);
void print_lenv(List_Envar le);
void print_envar(Envar *ev);
void print_undo_list(List_Undo lu);
void print_undo(Undo *ud);
void print_expr(Expression *expr);
void print_action_field(Action_Field *ac);
void print_expr_frame(ExprFrame *ef);
void print_term(Term *term);
void print_tl(TermList tl);
void print_list_expr(ExprList tl);
void print_exprlist(ExprList tl);
/* void print_add_del_list(Add_Del_List *adl); */

