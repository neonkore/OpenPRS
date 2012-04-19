/*                               -*- Mode: C -*- 
 * oprs-pprint_f.h -- 
 * 
 * $Id$
 * 
 * Copyright (c) 1991-2005 Francois Felix Ingrand.
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

ListLines pretty_print_exprlist(int width, ExprList lexpr);
ListLines pretty_print_int_array(int width, Int_array *ia);
ListLines pretty_print_float_array(int width, Float_array *fa);
ListLines pretty_print_integer(int width, int i);
ListLines pretty_print_long_long(int width, long long int i);
ListLines pretty_print_user_pointer(int width, void *i);
ListLines pretty_print_float(int width, double d);
ListLines pretty_print_property(int width, Property *p);
ListLines pretty_print_list_property(int width, PropertyList pl);
ListLines pretty_print_frame(int width, FramePtr fp);
ListLines pretty_print_lenv(int width, List_Envar le);
ListLines pretty_print_envar(int width, Envar *ev);
ListLines pretty_print_undo_list(int width, List_Undo lu);
ListLines pretty_print_undo(int width, Undo *ud);
ListLines pretty_print_expr(int width, Expression *expr);
ListLines pretty_print_action_field(int width, Action_Field *ac);
ListLines pretty_print_expr_frame(int width, ExprFrame *ef);
ListLines pretty_print_tl(int width, TermList tl);
ListLines pretty_print_term(int width, Term *term);
ListLines pretty_print_string(int width, PString string);
ListLines pretty_print_backslash_string(int width, PString string);
ListLines pretty_print_list_expr(int width, ExprList lexpr);
ListLines pretty_print_unknown_lisp_object(int width, L_List l);
ListLines pretty_print_l_list(int width, L_List l);
ListLines pretty_print_fact(int width, Fact *fact);
ListLines pretty_print_goal(int width, Goal *goal);
ListLines pretty_print_intention(int width, Intention *in);
ListLines pretty_print_op_instance(int width,  Op_Instance *opi);
ListLines pretty_print_body(int width, Body *body);
/* ListLines pretty_print_add_del_list(int width, Add_Del_List *adl); */

Line *make_line(void);
Line *create_line(char *s);
void free_line(Line *l);
void update_indent( ListLines ll, int indent);
Line *concat_lines(Line *l1, Line *l2);
void sl_concat_slist_lines(ListLines ll);
void sl_concat_slist_lines_space(ListLines ll);
Line *concat_all_lines_space(Line *l1, ListLines ll, Line *l2);
int total_size(ListLines ll);
int total_size_sep(ListLines ll, int size_sep);
char *build_string_from_list_lines(ListLines ll);
