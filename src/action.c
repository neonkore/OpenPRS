/*                               -*- Mode: C -*- 
 * action.c -- 
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




#ifdef GRAPHIX
#ifdef GTK
#else
#include <Xm/Xm.h>
#endif
#endif

#include "relevant-op.h"


#ifdef VXWORKS
#else
#endif


#include "shashPack_f.h"

#include "constant.h"

#include "action.h"
#include "oprs-sprint.h"
#include "oprs.h"
#include "fact-goal.h"
#include "int-graph.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"
#include "oprs-rerror.h"

#include "conditions_f.h"
#include "oprs-print_f.h"
#include "oprs-sprint_f.h"
#include "lisp-list_f.h"
#include "unification_f.h"
#include "intend_f.h"
#include "oprs-type_f.h"
#include "oprs_f.h"
#include "intention_f.h"
#include "int-graph_f.h"
#include "action_f.h"
#include "fact-goal_f.h"
#include "oprs-rerror_f.h"
#include "oprs-error_f.h"
#include "parser-funct_f.h"
#include "oprs-send-m_f.h"
#include "oprs-sprint_f.h"


#include "tcl_f.h"


#ifdef GRAPHIX
#ifdef GTK
#include "goprs-dialog_f.h"
#include "goprs-textwin_f.h"
#else
#include "xoprs-dialog_f.h"
#include "xoprs-textwin_f.h"
#endif
#else
#include "op-x-opaque_f.h"
#endif


/*********Log function and variables*************/
/* Who wrote this? I did not (Felix) */
#define MAX_LOG_FILE 10
FILE *f_log_a[MAX_LOG_FILE];
int   f_log_init = 0;

void init_log_array(){
  int i;
  
  for(i=0 ; i<MAX_LOG_FILE ; i++)
    f_log_a[i] = NULL;
  
  f_log_init = 1;
}

Term *action_log_init(TermList terms){

  Term *t_file_name, *t_file_nb, *res;
  PString s_file_name;
  int file_nb;
  
  res       = MAKE_OBJECT(Term);
  res->type = TT_ATOM;
  res->u.id = nil_sym;

  if(f_log_init==0)
    init_log_array();
  
  t_file_name = (Term *)sl_get_slist_head(terms);
  if (t_file_name->type != STRING)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
  s_file_name = t_file_name->u.string;

  t_file_nb  = (Term *)sl_get_slist_next(terms,t_file_name); 
  if (t_file_nb->type != INTEGER)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
  file_nb = t_file_nb->u.intval;
  
  /* create the file, if exists, clean content */
  f_log_a[file_nb] = fopen(s_file_name, "w");
  
  if (f_log_a[file_nb])
    res->u.id = lisp_t_sym;
  
  return res;
}

Term *action_log_end (TermList terms){

//  Term *t_file_name,
     Term *t_file_nb, *res;
//  PString s_file_name;
     int file_nb;
     int ret_close;
  
  res       = MAKE_OBJECT(Term);
  res->type = TT_ATOM;
  res->u.id = nil_sym;

  if(f_log_init==0)
    init_log_array();
  
  t_file_nb  =  (Term *)sl_get_slist_head(terms);
  if (t_file_nb->type != INTEGER)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
  file_nb = t_file_nb->u.intval;
  
  if (f_log_a[file_nb]!=NULL){
    ret_close = fclose(f_log_a[file_nb]);
    f_log_a[file_nb] = NULL;
    if (ret_close)
      res->u.id = nil_sym;
    else
      res->u.id = lisp_t_sym;
  }
  else
    res->u.id = lisp_t_sym;
  
  return res;
}

Term *action_log_printf(TermList terms)
/* Print a list term. */
{
  Term *t,*nb_term,*res;
  int  file_nb;
  PString fmt_str, fmt_str2;
  Term *term;
  Expression *tc;
  PBoolean save_pb;
  
  if(f_log_init==0)
    report_fatal_external_error(LG_STR("Log files Array not initialized.",
				       "Les fichiers de Log (Vecteurs) n'ont pas etait initialisés."));
  
  res = MAKE_OBJECT(Term);
  res->type = TT_ATOM;
  res->u.id = nil_sym;
  

  t = (Term *)sl_get_slist_head(terms);
  if (t->type != EXPRESSION)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
  
  
  nb_term = (Term *)sl_get_slist_next(terms,t); 
  if (nb_term->type != INTEGER)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
  
  file_nb = nb_term->u.intval;
  if((file_nb >= MAX_LOG_FILE)||
     (file_nb < 0))
    report_fatal_external_error(LG_STR("Incorrect file number.",
				       "Le numero de fichier est incorrect."));
  if(f_log_a[file_nb] == NULL)
    report_fatal_external_error(LG_STR("This log has not been initialized.",
				       "Ce fichier n'a pas été initializé correctement."));

  tc = t->u.expr;
  if (tc->pfr->name != format_sym)
    report_fatal_external_error(LG_STR("Expecting a keyword FORMAT in action_printf.",
				       "Attendait un mot clef FORMAT dans la fonction action_printf."));
  
  terms = (tc->terms);
  
  term = find_binding((Term *)sl_get_slist_next(terms, NULL));
  if (term->type != STRING)
    report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
  
  fmt_str = term->u.string;
  
  
  for (fmt_str2 = fmt_str; *fmt_str2 ; fmt_str2++) {
    if (*fmt_str2 != '%') {
      fputc (*fmt_str2 ,f_log_a[file_nb]);
    } else {
      switch (*++fmt_str2) {
      case 'g':
      case 'd':
      case 'f':
      case 's':
	if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
	  report_fatal_external_error(LG_STR("Directive and no term left to print in action_printf.",
					     "Des directives subsistent mais plus de termes à imprimer dans la fonction action_printf."));
	save_pb = print_backslash;
	print_backslash = FALSE;
	fprint_term(f_log_a[file_nb],find_binding(term));
	print_backslash = save_pb;
	break;
      case 't':
	if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
	  report_fatal_external_error(LG_STR("Directive and no term left to print in action_printf.",
					     "Des directives subsistent mais plus de termes à imprimer dans la fonction action_printf."));
	save_pb = print_backslash;
	print_backslash = TRUE;
	fprint_term(f_log_a[file_nb],find_binding(term));
	print_backslash = save_pb;
	break;
      case '%':
	fputc ('%' ,f_log_a[file_nb]);
	break;
      default:
	fprintf(stderr,LG_STR("Unknown directive %%%c in action_printf.\n",
			      "Directive %%%c inconnue dans la fonction action_printf.\n"), *fmt_str2);
	break;
      }
    }
  }
  
  if ((term = (Term *)sl_get_slist_next(terms, term)) != 0)
    report_recoverable_external_error(LG_STR("term(s) left to print in action_printf.",
					     "Des term(s) restent à imprimer dans la fonction action_printf."));
  
  fflush(f_log_a[file_nb]);
  
  res->u.id = lisp_t_sym;
  
  return res;
}

Term *evaluate_term_action(Action *ac, Symbol ac_name, TermList tl)
/* This is the main function to evaluate a composed term with an evaluable function */
{
     Term *res;
#ifdef OPRS_PROFILING     
     PDate tp;
#endif

     current_error_type = ACTION;
     current_ac = ac_name;

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_AC])
	  GET_PROFILE_TIMER(&tp);
#endif

     res = (Term *)(* (ac->function))(tl);

#ifdef OPRS_PROFILING     
     if (profiling_option[PROFILING] && profiling_option[PROF_AC]) {
	  ADD_PROFILE_TIMER(&tp,ac->time_active);
	  ac->nb_call++;
     }
#endif

     current_error_type = ET_NONE;

     return res;
}

Term *action_send_message(TermList terms)
/* Send message. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != TT_ATOM)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     if (t2->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     send_message_term(t2->u.expr,t1->u.id);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_send_string(TermList terms)
/* Send message. */
{
     Term *t1, *t2,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != TT_ATOM)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
     if (t2->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     oprs_send_message_string(t2->u.string,t1->u.id);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_multicast_message(TermList terms)
/* multicast message. */
{
     Term *t1, *t2,*res;
     Symbol *recs;
     int i, nb_recs;
     L_List l;
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t1->type != LISP_LIST)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
     if (t2->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     l = t1->u.l_list;
     nb_recs = l_length(l);
     recs = MALLOC(sizeof(Symbol)*nb_recs);

     for (i=0; i!= nb_recs;i++) {
	  Term *car=CAR(l);

	  if (!car || car->type != TT_ATOM)
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	  
	  l= CDR(l);
	  
	  recs[i] = car->u.id;
     }

     multicast_message_term(t2->u.expr,nb_recs,recs);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     FREE(recs);

     return res;
}

Term *action_broadcast_message(TermList terms)
/* Send message. */
{
     Term *t1,*res;
     
     res = MAKE_OBJECT(Term);
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if ((t1->type != EXPRESSION)) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
     broadcast_message_term(t1->u.expr);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_print_term(TermList terms)
/* Print a term (called from the print action op). */
{
     Term *t,*res;
     PBoolean save_pb;

     res = MAKE_OBJECT(Term);
     
     t = (Term *)sl_get_slist_head(terms);

     save_pb = print_backslash;
     print_backslash = FALSE;
     print_term(t);
     print_backslash = save_pb;

     printf("\n");
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

void print_list_term(TermList terms)
/* 
 * print_list_term - We have got our own copy of the list (because we are called by
 * 			action_print_list_term so we can dismantel the list... 
 */
{

     PString str, str2;
     Term *term;
     PBoolean save_pb;

     term = find_binding((Term *)sl_get_from_head(terms));
     if (term->type != STRING) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
     str = term->u.string;
     
     for (str2 = str; *str2 ; str2++) { /* str + 1 to get read of the starting double quote */
	  switch (*str2) {
	  case '~':
	       switch (*++str2) {
	       case '&':
	       case '%':
		    putchar('\n');
		    break;
	       case 'a':
	       case 'A':
		    if ((term = (Term *)sl_get_from_head(terms)) == 0)
			 report_fatal_external_error(LG_STR("A ~a directive and no term left to print in print_list_term.",
							    "Une directive ~a restant et pas de terms restant à imprimer dans la fonction print_list_term."));
		    save_pb = print_backslash;
		    print_backslash = FALSE;
		    print_term(find_binding(term));
		    print_backslash = save_pb;
		    break;
	       case '~':
		    putchar('~');
		    break;
	       case '\0':	/* the ~ was the last char... backup one. */
		    --str2;
		    report_recoverable_external_error(LG_STR("The char '~' should not be the last char in print_list_term.",
							     "Le caractère '~' ne devrait pas être le dernier caractère à imrpimer dans la fonction print_list_term."));
		    break;
	       default:
		    report_recoverable_external_error(LG_STR("Expecting only ~a directive in print_list_term.",
							     "Seules les directives ~a sont interprétées dans la fonction print_list_term."));
	       }
	       break;
	  case '\\':
	       switch (*++str2) {
	       case 'n':
		    putchar('\n');
		    break;
	       case '\\':
		    putchar('\\');
		    break;
	       default:
		    putchar('\\');
		    putchar(*str2);
		    break;
	       }
	       break;
	  default: 
	       putchar(*str2);
	       break;
	  }
     }
     if ((term = (Term *)sl_get_from_head(terms)) != 0)
	  report_recoverable_external_error(LG_STR("Term(s) left to print in print_list_term.",
						   "Des term(s) restent à imprimer dans la fonction print_list_term."));
}

Term *action_print_list_term(TermList terms)
/* Print a list term. */
{
     Term *t,*res, *nil;
     Expression *tc;
     TermList terms2;

     res = MAKE_OBJECT(Term);
     
     t = (Term *)sl_get_slist_head(terms);
     if (t->type != EXPRESSION) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tc = t->u.expr;
     if (tc->pfr->name != format_sym)
	  report_fatal_external_error(LG_STR("Expecting a FORMAT in action_print_list_term.",
					     "Attendait le symbol FORMAT dans la fonction action_print_list_term."));
     terms2 = COPY_SLIST(tc->terms);
     if (!(nil = (Term *)sl_get_from_head(terms2)) || nil->type != TT_ATOM || nil->u.id != nil_sym)
	  report_fatal_external_error(LG_STR("Expecting a NIL in action_print_list_term.",
					     "Attendait un NIL dans la fonction action_print_list_term."));
     
     print_list_term(terms2);	/* Now contains "asdasd asdad" $x $y $z */

     FREE_SLIST(terms2);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_printf(TermList terms)
/* Print a list term. */
{
     Term *t,*res;
     PString fmt_str, fmt_str2;
     Term *term;
     Expression *tc;
     PBoolean save_pb;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;


     t = (Term *)sl_get_slist_head(terms);
     if (t->type != EXPRESSION) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tc = t->u.expr;
     if (tc->pfr->name != format_sym)
	  report_fatal_external_error(LG_STR("Expecting a keyword FORMAT in action_printf.",
					     "Attendait un mot clef FORMAT dans la fonction action_printf."));

     terms = (tc->terms);

     term = find_binding((Term *)sl_get_slist_next(terms, NULL));
     if (term->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     fmt_str = term->u.string;
     

     for (fmt_str2 = fmt_str; *fmt_str2 ; fmt_str2++) {
	  if (*fmt_str2 != '%') {
	       putchar(*fmt_str2);
	  } else {
	       switch (*++fmt_str2) {
	       case 'g':
	       case 'd':
	       case 'f':
	       case 's':
		    if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
			 report_fatal_external_error(LG_STR("Directive and no term left to print in action_printf.",
							    "Des directives subsistent mais plus de termes à imprimer dans la fonction action_printf."));
		    save_pb = print_backslash;
		    print_backslash = FALSE;
		    print_term(find_binding(term));
		    print_backslash = save_pb;
		    break;
	       case '%':
		    putchar('%');
		    break;
	       default:
		    fprintf(stderr,LG_STR("Unknown directive %%%c in action_printf.\n",
					  "Directive %%%c inconnue dans la fonction action_printf.\n"), *fmt_str2);
		    break;
	       }
	  }
     }

     if ((term = (Term *)sl_get_slist_next(terms, term)) != 0)
	  report_recoverable_external_error(LG_STR("term(s) left to print in action_printf.",
						   "Des term(s) restent à imprimer dans la fonction action_printf."));
     
     res->u.id = lisp_t_sym;
     return res;
}

Term *action_manage_window(TermList terms)
/* Print a list term. */
{
     Term *t,*res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != U_POINTER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_U_POINTER_TERM_TYPE));
     
     manage_ut_window(t->u.u_pointer);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_unmanage_window(TermList terms)
/* Print a list term. */
{
     Term *t,*res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != U_POINTER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_U_POINTER_TERM_TYPE));
     
     unmanage_ut_window(t->u.u_pointer);
     res->u.id = lisp_t_sym;

     return res;
}


Term *action_delete_window(TermList terms)
/* Print a list term. */
{
     Term *t,*res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != U_POINTER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_U_POINTER_TERM_TYPE));
     
     delete_ut_window(t->u.u_pointer);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_rename_window(TermList terms)
/* Print a list term. */
{
     Term *t,*res;
     void *window;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != U_POINTER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_U_POINTER_TERM_TYPE));
     
     window = t->u.u_pointer;

     t = (Term *)sl_get_slist_pos(terms, 2);
     if (t->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
     
     rename_ut_window(window, t->u.string);
     res->u.id = lisp_t_sym;

     return res;
}

Term *action_printf_window(TermList terms)
/* Print a list term. */
{
     Term *t,*res;
     PString fmt_str, fmt_str2;
     Term *term;
     Expression *tc;
     PBoolean save_pb;
     void *window;
     static Sprinter *sp = NULL;

     if (sp) reset_sprinter(sp);
     else sp = make_sprinter(0);

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t = (Term *)sl_get_slist_pos(terms, 1);
     if (t->type != U_POINTER)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_U_POINTER_TERM_TYPE));
     
     window = t->u.u_pointer;

     t = (Term *)sl_get_slist_pos(terms, 2);
     if (t->type != EXPRESSION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));

     tc = t->u.expr;
     if (tc->pfr->name != format_sym)
	  report_fatal_external_error(LG_STR("Expecting a keyword FORMAT in action_printf.",
					     "Attendait un mot clef FORMAT dans la fonction action_printf."));

     terms = tc->terms;

     term = find_binding((Term *)sl_get_slist_next(terms, NULL));
     if (term->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     fmt_str = term->u.string;
     

     for (fmt_str2 = fmt_str; *fmt_str2 ; fmt_str2++) {
	  if (*fmt_str2 != '%') {
	       SPRINT(sp, 1,sprintf(f,"%c",*fmt_str2));
	  } else {
	       switch (*++fmt_str2) {
	       case 'g':
	       case 'd':
	       case 'f':
	       case 's':
		    if ((term = (Term *)sl_get_slist_next(terms, term)) == 0)
			 report_fatal_external_error(LG_STR("Directive and no term left to print in action_printf.",
							    "Des directives subsistent mais plus de termes à imprimer dans la fonction action_printf."));
		    save_pb = print_backslash;
		    print_backslash = FALSE;
		    sprint_term(sp,find_binding(term));
		    print_backslash = save_pb;
		    break;
	       case '%':
		    SPRINT(sp, 1,sprintf(f,"%c",'%'));
		    putchar('%');
		    break;
	       default:
		    fprintf(stderr,LG_STR("Unknown directive %%%c in action_printf.\n",
					  "Directive %%%c inconnue dans la fonction action_printf.\n"), *fmt_str2);
		    break;
	       }
	  }
     }

     if ((term = (Term *)sl_get_slist_next(terms, term)) != 0)
	  report_recoverable_external_error(LG_STR("term(s) left to print in action_printf.",
						   "Des term(s) restent à imprimer dans la fonction action_printf."));
     
     if (window) {
	  manage_ut_window(window);
	  AppendTextWindow(window,SPRINTER_STRING(sp),FALSE);
	  
     } else printf("%s",SPRINTER_STRING(sp));

     res->u.id = lisp_t_sym;
     return res;
}

Term *action_fail(TermList terms)
{
     return build_nil();
}

Term *action_succeed(TermList terms)
{
     return build_t();
}

Term *action_tcl_command(TermList terms)
{
     Term *term, *res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != STRING) {
	  report_recoverable_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
	  res->u.id = nil_sym;
     } else {
	  PString command;

	  command = (char *)MALLOC((strlen(term->u.string) + 2) * sizeof(char));
	  sprintf(command, "%s\n", term->u.string);
	  exec_tcl_command(command);
	  FREE(command);
	  res->u.id = lisp_t_sym;
     }
     return res;
}

Term *action_execute_command(TermList terms)
{
     Term *term, *res;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;

     term = (Term *)sl_get_slist_pos(terms,1);
     if (term->type != STRING) {
	  report_recoverable_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));
	  res->u.id = nil_sym;
     } else {
	  PString command;

	  command = (char *)MALLOC((strlen(term->u.string) + 2) * sizeof(char));
	  sprintf(command, "%s\n", term->u.string);
	  send_command_to_parser(command);
	  FREE(command);
	  res->u.id = lisp_t_sym;
     }
     return res;
}

Term *action_read_inside_id_var(TermList terms)
/* Will add a goal on the stack, waiting for the fact
 * (READ-RESPONSE-ID T1 VAL)
 */
{
     Term *t1, *t2, *res;
     Thread_Intention_Block *tib;
     Expression *expr;
     Expression *cond;
     TermList tl;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
     
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);

     tib = current_tib;
     if (tib->num_called == 0) {
	  if (t1->type !=  TT_ATOM)  {
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	       return res;
	  }
	  printf(LG_STR("Respond with a fact like: (%s %s <response>).\n",
			"Répondre avec un fait tel que: (%s %s <response>).\n"),read_response_id_sym, t1->u.id);

	  tl = sl_make_slist();
	  tl = build_term_list(tl, (dup_term(t1)));
	  tl = build_term_list(tl, (dup_term(t2)));
	  cond = build_expr_pfr_terms(read_response_id_pred, tl);
	  
	  expr = build_expr_sym_expr(wait_sym, cond);

	  add_goal((tib->current_goal = make_goal_from_edge(expr,
							    make_frame(TIB_FRAME(tib)),
							    tib, NULL, NULL)),
		   current_oprs);
	  res->u.id = wait_sym;
     } else if (tib->current_goal->succes) { /* The goal was successfuly achieved... */
	  free_frame(tib->current_goal->frame);
	  free_goal(tib->current_goal); /* This is all over for this one */
	  tib->current_goal = NULL;
	  res->u.id = lisp_t_sym;

     } else  { 
	  /*
	   * The Goal has already been built and posted, but the op-instance
	   * has been discarded by the meta-level mechanism.
	   * We repost it !
	   */
	  if (tib->current_goal->echoue != NOT_FAILED) {
	       desinstall_frame(tib->current_goal->frame, TIB_FRAME(tib));
	       free_frame(tib->current_goal->frame);
	       free_goal(tib->current_goal); /* This is all over for this one */
	       tib->current_goal = NULL;
	       res->u.id = nil_sym;
	  } else {
	       add_goal(tib->current_goal,current_oprs); /* We repost it damnit. */
	       res->u.id = wait_sym;
	  }
     }
     return res;
}

Term *read_inside_id_action(TermList terms)
/* Will add a goal on the stack, waiting for the fact
 * (READ-RESPONSE-ID T1 VAL)
 */
{
     Term *t1, *res, *res2;
     Expression *expr;
     Expression *cond;
     Thread_Intention_Block *tib;
     TermList tl;

     char var_name[16];
     Symbol var_nm_tmp;
     int i=0;

     t1 = (Term *)sl_get_slist_pos(terms, 1);

     tib = current_tib;
     if (tib->num_called == 0) {
	  res = MAKE_OBJECT(Term);
	  res->type = TT_ATOM;
     
	  if (t1->type !=  TT_ATOM)  {
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	       res->u.id = nil_sym;
	       return res;
	  }
          printf(LG_STR("Respond with a fact like: (%s %s <response>).\n",
			"Répondre avec un fait tel que: (%s %s <response>).\n"),read_response_id_sym, t1->u.id);

	  sprintf(var_name, "$RI%i", i); /* Any variable would do */
	  DECLARE_ID(var_name, var_nm_tmp);
     
	  res2 = build_term_from_var(build_envar(var_nm_tmp, LOGICAL_VARIABLE));

	  tl = sl_make_slist();
	  tl = build_term_list(tl, (dup_term(t1)));
	  tl = build_term_list(tl, res2);
	  cond = build_expr_pfr_terms(read_response_id_pred, tl);

	  expr = build_expr_sym_expr(wait_sym, cond);
	  add_goal((tib->current_goal = make_goal_from_edge(expr,
							    make_frame(TIB_FRAME(tib)),
							    tib, NULL, NULL)),
		   current_oprs);
	  res->u.id = wait_sym;

     } else if (tib->current_goal->succes) { /* The goal was successfuly achieved... */

	  tl = EXPR_TERMS(EXPR_EXPR1(tib->current_goal->statement));
	  res = dup_term((Term *) sl_get_slist_tail(tl));
	  free_frame(tib->current_goal->frame);
	  free_goal(tib->current_goal); /* This is all over for this one */
	  tib->current_goal = NULL;
     } else {
	  /*
	   * The Goal has already been built and posted, but the op-instance
	   * has been discarded by the meta-level mechanism.
	   * We repost it !
	   */
	  res = MAKE_OBJECT(Term);
	  res->type = TT_ATOM;
     
	  if (tib->current_goal->echoue != NOT_FAILED) {
	       desinstall_frame(tib->current_goal->frame, TIB_FRAME(tib));
	       free_frame(tib->current_goal->frame);
	       free_goal(tib->current_goal); /* This is all over for this one */
	       tib->current_goal = NULL;
	       res->u.id = nil_sym;
	  } else {
	       add_goal(tib->current_goal,current_oprs); /* We repost it damnit. */
	       res->u.id = wait_sym;
	  }
     }
     return res;
}

Term *read_inside_action(TermList terms)
/* Will add a goal on the stack, waiting for the fact
 * (READ-RESPONSE VAL)
 */
{
     Term *res, *res2;
     Thread_Intention_Block *tib;
     Expression *expr;
     Expression *cond;
     TermList tl;

     char var_name[16];
     Symbol var_nm_tmp;
     int i=0;
     tib = current_tib;
     if (tib->num_called == 0) {
	  res = MAKE_OBJECT(Term);
	  res->type = TT_ATOM;

          printf(LG_STR("Respond with a fact like: (%s <response>).\n",
			"Répondre avec un fait tel que: (%s <response>).\n"),read_response_sym);

	  sprintf(var_name, "$RI%i", i); /* Any variable would do */
	  DECLARE_ID(var_name, var_nm_tmp);
     
	  res2 = build_term_from_var(build_envar(var_nm_tmp, LOGICAL_VARIABLE));

	  tl = sl_make_slist();
	  tl = build_term_list(tl, res2);
	  cond = build_expr_pfr_terms(read_response_pred, tl);

	  expr = build_expr_sym_expr(wait_sym, cond);
	  add_goal((tib->current_goal = make_goal_from_edge(expr,
							   make_frame(TIB_FRAME(tib)),
							   tib, NULL, NULL)),
		   current_oprs);
	  res->u.id = wait_sym;
     } else if (tib->current_goal->succes) { /* The goal was successfuly achieved... */
	
	  tl = EXPR_TERMS(EXPR_EXPR1(tib->current_goal->statement));
	  res = dup_term((Term *) sl_get_slist_tail(tl));
	  free_frame(tib->current_goal->frame);
	  free_goal(tib->current_goal); /* This is all over for this one */
	  tib->current_goal = NULL;

     } else {
	  /*
	   * The Goal has already been built and posted, but the op-instance
	   * has been discarded by the meta-level mechanism.
	   * We repost it !
	   */
	  res = MAKE_OBJECT(Term);
	  res->type = TT_ATOM;
     
	  if (tib->current_goal->echoue != NOT_FAILED) {
	       desinstall_frame(tib->current_goal->frame, TIB_FRAME(tib));
	       free_frame(tib->current_goal->frame);
	       free_goal(tib->current_goal); /* This is all over for this one */
	       tib->current_goal = NULL;
	       res->u.id = nil_sym;
	  } else {
	       add_goal(tib->current_goal,current_oprs); /* We repost it damnit. */
	       res->u.id = wait_sym;
	  }
     }
     return res;
}

Term *kill_other_intentions_action(TermList terms)
{
     Term *res;
     Intention_Graph *ig;
     Intention *ci, *in;
     Intention_List tmp_il;

     res = MAKE_OBJECT(Term);
     
     ig = current_oprs->intention_graph;
     ci = current_intention;

     tmp_il = COPY_SLIST(ig->list_intentions); 

     sl_loop_through_slist(tmp_il, in, Intention *)
	  if (in != ci)
	       if (! sl_in_slist(ig->list_condamned,in)) 
		    /* Add if it was not already here */ 
		    sl_add_to_head(ig->list_condamned,in);			      

     FREE_SLIST(tmp_il);	/* Free it after. */
     
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

Term *kill_intentions_action(TermList terms)
{
     Term *res, *t1;
     Intention_Graph *ig;
     Intention *ci, *in;
     L_List l;

     ig = current_oprs->intention_graph;
     ci = current_intention;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST) 
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));


     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     l = t1->u.l_list;

     while (l != l_nil) {
	  Term *l_car;

	  l_car = CAR(l);

	  if (!l_car || l_car->type != TT_INTENTION)
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));

	  if (ci ==  (in = l_car->u.in))
	       report_fatal_external_error(LG_STR("Cannot kill the current intention from itself in kill_intentions_action.",
						  "Ne peut pas tuer l'intention courante à partir de soi même dans la fonction kill_intentions_action."));

	  if (valid_intention(ig, in)) {
	       if (! sl_in_slist(ig->list_condamned,in)) 
		    /* Add if it was not already here */ 
		    sl_add_to_head(ig->list_condamned,in);			      
	  } else {
	       report_recoverable_external_error(oprs_strerror(PE_DEAD_INTENTION));
	       res->u.id = nil_sym;
	  }

	  l = CDR(l);
     }
     return res;
}

Term *kill_intention_action(TermList terms)
{
     Term *t1;
     Intention_Graph *ig;
     Intention *ci, *in;

     ig = current_oprs->intention_graph;
     ci = current_intention;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_INTENTION)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));

     if (ci ==  (in = t1->u.in))
	  report_fatal_external_error(LG_STR("Cannot kill the current intention from itself in kill_intention_action.",
					     "Ne peut pas tuer l'intention courante à partir de soi même dans la fonction kill_intention_action."));
     
     if (valid_intention(ig, in)) {
	       if (! sl_in_slist(ig->list_condamned,in)) 
		    /* Add if it was not already here */ 
		    sl_add_to_head(ig->list_condamned,in);			      
          return build_t();
     } else 
	  report_recoverable_external_error(oprs_strerror(PE_DEAD_INTENTION));
          return build_nil();
}

Term *asleep_intentions_action(TermList terms)
{
     Term *res, *t1, *t2;
     Intention_Graph *ig;
     Intention *ci, *in;
     L_List l;
     Expression *cond;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
	  return res;
     }
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	  return res;
     }
     ig = current_oprs->intention_graph;
     ci = current_intention;

     res->u.id = lisp_t_sym;
 
     l = t1->u.l_list;
     while (l != l_nil) {
	  Term *l_car;

	  l_car = CAR(l);
	  if (!l_car || l_car->type != TT_INTENTION) {
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	  } else if (ci ==  (in = l_car->u.in)) {
	       report_fatal_external_error(LG_STR("Cannot asleep the current intention from itself in asleep_intentions_action.",
						  "Cannot asleep the current intention from itself dans la fonction asleep_intentions_action."));
	  } else if (valid_intention(ig, in)) {
	       TermList tl = sl_make_slist();

	       tl = build_term_list(tl, (dup_term(t2)));
	       cond = build_expr_pfr_terms(wake_up_intention_pred, tl);

	       make_and_install_condition(INTENTION_ACTIVATION, in, NULL, cond, NULL);

	  } else {
	       report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
	  }
	  l = CDR(l);
     }

     return res;
}

Term *asleep_intention_action(TermList terms)
{
     Term *res, *t1, *t2;
     Intention_Graph *ig;
     Intention *ci, *in;
     Expression *cond;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	  return res;
     }
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	  return res;
     }
     ig = current_oprs->intention_graph;
     ci = current_intention;
     if (ci ==  (in = t1->u.in)) {
	  report_fatal_external_error(LG_STR("Cannot asleep the current intention from itself in asleep_intention_action.",
					     "Cannot asleep the current intention from itself dans la fonction asleep_intention_action."));
     } else if (valid_intention(ig, in)) {
	  TermList tl = sl_make_slist();

	  tl = build_term_list(tl, (dup_term(t2)));
	  cond = build_expr_pfr_terms(wake_up_intention_pred, tl);

	  make_and_install_condition(INTENTION_ACTIVATION, in, NULL, cond, NULL);
     
	  res->u.id = lisp_t_sym;
     } else {
	  report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
     }
     return res;
}

Term *wake_up_intention_action(TermList terms)
{
     Term *res, *t1;
     TermList tl = sl_make_slist();
     
     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	  return res;
     }
     tl = build_term_list(tl, (dup_term(t1)));
     add_fact(make_fact_from_expr(build_expr_pfr_terms(wake_up_intention_pred, tl)),
	      current_oprs);

     res->u.id = lisp_t_sym;
     return res;
}

Term *asleep_intention_with_cond_action(TermList terms)
{
     Term *res, *t1, *t2;
     Intention_Graph *ig;
     Intention *ci, *in;
     Expression *cond;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
return res;
}
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type !=  EXPRESSION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
	  return res;
     }

     ig = current_oprs->intention_graph;
     ci = current_intention;

     if (ci ==  (in = t1->u.in)) {
	  report_fatal_external_error(LG_STR("Cannot asleep the current intention from itself in asleep_intention_with_cond_action.",
					     "Cannot asleep the current intention from itself dans la fonction asleep_intention_with_cond_action."));
     } else if (valid_intention(ig, in)) {
	  cond = copy_expr(t2->u.expr);

	  make_and_install_condition(INTENTION_ACTIVATION, in, NULL, cond, NULL);
     
	  res->u.id = lisp_t_sym;
     } else {
	  report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
     }
     return res;
}

Term *asleep_intentions_with_cond_action(TermList terms)
{
     Term *res, *t1, *t2;
     Intention_Graph *ig;
     Intention *ci, *in;
     L_List l;
     Expression *cond;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != LISP_LIST) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_LISP_LIST_TERM_TYPE));
	  return res;
     }
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type !=  EXPRESSION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_EXPRESSION_TERM_TYPE));
	  return res;
     }

     ig = current_oprs->intention_graph;
     ci = current_intention;

     l = t1->u.l_list;
     while (l != l_nil) {
	  Term *l_car;

	  l_car = CAR(l);
    
	  if (!l_car || l_car->type != TT_INTENTION) {
	       report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
	       return res;
	  } else if (ci ==  (in = l_car->u.in)) {
	       report_fatal_external_error(LG_STR("Cannot asleep the current intention from itself in asleep_intentions_with_cond_action.",
						  "Cannot asleep the current intention from itself dans la fonction asleep_intentions_with_cond_action."));
	  } else if (valid_intention(ig, in)) {
	       cond = copy_expr(t2->u.expr);

	       make_and_install_condition(INTENTION_ACTIVATION, in, NULL, cond, NULL);
	       
	       res->u.id = lisp_t_sym;
	  } else {
	       report_fatal_external_error(oprs_strerror(PE_DEAD_INTENTION));
	  }
	  l = CDR(l);
     }
     return res;
}

Term *tag_current_intention_action(TermList terms)
{
     Term *res, *t1;
     Intention *ci;

     res = MAKE_OBJECT(Term);

     ci = current_intention;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_ATOM) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_ATOM_TERM_TYPE));
	  res->type = TT_ATOM;
	  res->u.id = nil_sym;
	  return res;
     }
     set_intention_id(ci, t1->u.id);
     
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;

     return res;
}

extern PFB intention_list_sort_predicate;

Term *action_sort_all_intentions(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     Intention_List il =  intention_graph_intention_list(oprs_intention_graph(current_oprs));
     if (intention_list_sort_predicate) {
	  sl_sort_slist_func(il,intention_list_sort_predicate);
     }
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     return res;
}

Term *action_sort_intention_by_priority(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     intention_list_sort_predicate = &intention_list_sort_by_priority;
     printf (LG_STR("The intentions are now sorted by priority\n",
		    "Les intentions sont maintenant triées par priorité\n"));
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     return res;
}

Term *action_sort_intention_by_time(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     intention_list_sort_predicate = &intention_list_sort_by_time;
     printf (LG_STR("The intentions are now sorted by time\n",
		    "Les intentions sont maintenant triées par date\n"));
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     return res;
}

Term *action_sort_intention_by_priority_time(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     intention_list_sort_predicate = &intention_list_sort_by_priority_time;
     printf (LG_STR("The intentions are now sorted by priority then time\n",
		    "Les intentions sont maintenant triées par priorité et date\n"));
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     return res;
}

Term *action_sort_intention_none(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     intention_list_sort_predicate = NULL;
     printf (LG_STR("The intentions are not sorted anymore\n",
		    " Les intentions ne sont plus triées\n"));
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;
     return res;
}

Term *set_intention_priority_action(TermList terms)
{
     Term *res, *t1, *t2;
     Intention_Graph *ig = current_oprs->intention_graph;
     Intention *in;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;

     t1 = (Term *)sl_get_slist_pos(terms, 1);
     if (t1->type != TT_INTENTION) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTENTION_TERM_TYPE));
return res;
}
     t2 = (Term *)sl_get_slist_pos(terms, 2);
     if (t2->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
	  return res;
     }
    
     in = t1->u.in;
     if (valid_intention(ig, in)) {
	  set_intention_priority(in,  t2->u.intval);
	  res->u.id = lisp_t_sym;
     }
     return res;
}

Term *action_show_memory(TermList terms)
{
     Term *res;

     pmstats();

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     return res;
}

Term *action_start_cs(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     if (current_oprs->critical_section) {
	  report_fatal_external_error(LG_STR("Action START-CRITICAL-SECTION called while you are already in a critical section.",
					     "Action START-CRITICAL-SECTION alors qu'on est déjà dans une section critique."));
	  res->u.id = nil_sym;	/* Return NIL */
	  return res;
     }

     begin_critical_section(current_tib);

     return res;
}

Term *action_end_cs(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);

     res->type = TT_ATOM;
     res->u.id = lisp_t_sym;	/* Return T */

     if (! current_oprs->critical_section) {
	  report_fatal_external_error(LG_STR("Action END-CRITICAL-SECTION called while you are not in a critical section.",
					     "Action END-CRITICAL-SECTION en dehors d'une section critique."));
	  res->u.id = nil_sym;	/* Return NIL */
	  return res;
     }

     if (current_oprs->critical_section != current_tib->pere) {
	  printf (LG_STR("WARNING: Action END-CRITICAL-SECTION called in a different OP\n\
\tthan the call to START-CRITICAL-SECTION.\n",
			 "ATTENTION: Action END-CRITICAL-SECTION appelé d'un OP différent\n\
\tde celui dans lequel START-CRITICAL-SECTION avait été appelé.\n"));
     }

     end_critical_section(current_tib);
     return res;
}

Term *set_int_array_action(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);
     Term *t2 = (Term *)sl_get_slist_pos(terms, 2);
     Term *t3 = (Term *)sl_get_slist_pos(terms, 3);

     res->type = TT_ATOM;
     res->u.id = nil_sym;

     if (t1->type != INT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INT_ARRAY_TERM_TYPE));
     } else if (t2->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     } else if (t3->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     } else {
	  int i = t2->u.intval;

	  if ((i >= 0) && (i < (int) t1->u.int_array->size)) {
	       t1->u.int_array->array[t2->u.intval] = t3->u.intval;
	       res->u.id = lisp_t_sym;
	  } else
	       report_fatal_external_error(oprs_strerror(PE_OUT_OF_BOUND_ARRAY_ACCES));
     }
     
     return res;
}

Term *set_float_array_action(TermList terms)
{
     Term *res = MAKE_OBJECT(Term);
     Term *t1 = (Term *)sl_get_slist_pos(terms, 1);
     Term *t2 = (Term *)sl_get_slist_pos(terms, 2);
     Term *t3 = (Term *)sl_get_slist_pos(terms, 3);

     res->type = TT_ATOM;
     res->u.id = nil_sym;

     if (t1->type != FLOAT_ARRAY) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_ARRAY_TERM_TYPE));
     } else if (t2->type != INTEGER) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_INTEGER_TERM_TYPE));
     } else if (t3->type != TT_FLOAT) {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_FLOAT_TERM_TYPE));
     } else  {
	  int i = t2->u.intval;

	  if ((i >= 0) && (i < (int) t1->u.float_array->size)) {
	       t1->u.float_array->array[t2->u.intval] = *t3->u.doubleptr;
	       res->u.id = lisp_t_sym;
	  } else
	       report_fatal_external_error(oprs_strerror(PE_OUT_OF_BOUND_ARRAY_ACCES));
     }
     return res;
}

Term *current_intention_ef(TermList terms);
Term *list_all_intentions_ef(TermList terms);
Term *list_other_intentions_ef(TermList terms);
Term *list_sleeping_intentions_ef(TermList terms);
Term *list_root_intentions_ef(TermList terms);
Term *get_intention_priority_ef(TermList terms);
Term *get_intention_time_ef(TermList terms);
Term *make_int_array_ef(TermList terms);
Term *make_float_array_ef(TermList terms);
Term *get_int_array_ef(TermList terms);
Term *get_float_array_ef(TermList terms);

/* static Thread_Intention_Block *using_tib = NULL; */ /* who is planning... */


Term *intend_aop_for_goal_ac(TermList terms)
{
     Term *t1, *t2, *res;
     Thread_Intention_Block *tib;
     Goal *goal;
     Op_Instance *opi;

     res = MAKE_OBJECT(Term);
     res->type = TT_ATOM;
     res->u.id = nil_sym;
  
     t1 = (Term *)sl_get_slist_pos(terms, 1);
     t2 = (Term *)sl_get_slist_pos(terms, 2);

     tib = current_tib;
     if (t1->type !=  TT_OP_INSTANCE)  {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_OP_INSTANCE_TERM_TYPE));
	  return res;
     }
     opi =  t1->u.opi;

     if (t2->type !=  TT_GOAL)  {
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_GOAL_TERM_TYPE));
	  return res;
     }
	  
     goal = t2->u.goal;

     if (tib->num_called == 0) {
	  goal->tib = current_tib;
	  
	  tib->current_goal = dup_goal(goal);

	  intend(current_oprs->intention_graph,opi,empty_list,empty_list,0);

	  res->u.id = wait_son_sym;
     } else if (goal->succes) { /* The goal was successfuly achieved... */
	  res->u.id = lisp_t_sym;
	  tib->current_goal = NULL;
	  goal->tib = NULL;
	  free_goal(goal);
     } else {			/* failed with this OP. */
/* 	  SAFE_SL_ADD_TO_HEAD(goal->echec,dup_op_instance(opi)); */
/* A VERIFIER... 	  desinstall_frame(goal->frame,empty_env); */
	  res->u.id = nil_sym;
	  tib->current_goal = NULL;
	  goal->tib = NULL;
	  free_goal(goal);
     }
     return res;
}



void declare_action(void)
{
     make_and_declare_action("GET-CURRENT-INTENTION",
					current_intention_ef, 0);
     make_and_declare_action("GET-ALL-INTENTIONS",
					list_all_intentions_ef, 0);
     make_and_declare_action("GET-OTHER-INTENTIONS",
					list_other_intentions_ef, 0);
     make_and_declare_action("GET-SLEEPING-INTENTIONS",
					list_sleeping_intentions_ef, 0);
     make_and_declare_action("GET-ROOT-INTENTIONS",
					list_root_intentions_ef, 0);
     make_and_declare_action("GET-INTENTION-PRIORITY",
					get_intention_priority_ef, 1);
     make_and_declare_action("GET-INTENTION-TIME",
					get_intention_time_ef, 1);
     make_and_declare_action("MAKE-INT-ARRAY",
			     make_int_array_ef, 1);
     make_and_declare_action("MAKE-FLOAT-ARRAY",
			     make_float_array_ef, 1);
     make_and_declare_action("GET-INT-ARRAY",
			     get_int_array_ef, 2);
     make_and_declare_action("GET-FLOAT-ARRAY",
			     get_float_array_ef, 2);

     make_and_declare_action("TCL-COMMAND"  ,action_tcl_command, 1);

	  /* some log operations */
     make_and_declare_action("LOG-INIT"  ,action_log_init    , 2);
     make_and_declare_action("LOG-END"   ,action_log_end     , 1);
     make_and_declare_action("LOG-PRINTF",action_log_printf  , 2);

     make_and_declare_action("PRINTF-WINDOW",action_printf_window, 2);

     make_and_declare_action("RENAME-WINDOW",action_rename_window, 2);
     make_and_declare_action("MANAGE-WINDOW",action_manage_window, 1);
     make_and_declare_action("DELETE-WINDOW",action_delete_window, 1);
     make_and_declare_action("UNMANAGE-WINDOW",action_unmanage_window, 1);

     make_and_declare_action("PRINT",action_print_term, 1);
     make_and_declare_action("PRINTF",action_printf, 1);
     make_and_declare_action("PRINT-INSIDE",action_print_list_term, 1);

     make_and_declare_action("SEND-MESSAGE",action_send_message, 2);
     make_and_declare_action("BROADCAST-MESSAGE",action_broadcast_message, 1);
     make_and_declare_action("MULTICAST-MESSAGE",action_multicast_message, 2);
     make_and_declare_action("SEND-STRING",action_send_string, 2);

     /* new names and definition */

     make_and_declare_action("INTEND-ALL-OPS-AS-ROOT",action_intend_all_ops_as_root, 1);
     make_and_declare_action("INTEND-ALL-OPS",action_intend_all_ops, 1);
     make_and_declare_action("INTEND-ALL-OPS-AFTER",action_intend_all_ops_after, 2);
     make_and_declare_action("INTEND-OP",action_intend_op, 1);
     make_and_declare_action("INTEND-OP-AFTER",action_intend_op_after, 2);
     make_and_declare_action("INTEND-OP-AFTER-BEFORE",action_intend_op_after_before,3 );
     make_and_declare_action("INTEND-OP-WITH-PRIORITY",action_intend_op_with_priority, 2);
     make_and_declare_action("INTEND-OP-WITH-PRIORITY-AFTER-BEFORE",
			     action_intend_op_with_priority_after_before, 4);
     make_and_declare_action("INTEND-OP-WITH-PRIORITY-AFTER",
			     action_intend_op_with_priority_after, 3);
     make_and_declare_action("INTEND-OP-BEFORE-AFTER",action_intend_op_before_after,3 );

     make_and_declare_action("INTEND-ALL-GOALS-//",
			     action_intend_all_goals_par, 1);
     make_and_declare_action("INTEND-ALL-GOALS-//-AS-ROOTS",
			     action_intend_all_goals_par_as_roots, 1);
     make_and_declare_action("INTEND-ALL-GOALS-//-AFTER",
			     action_intend_all_goals_par_after, 2);
     make_and_declare_action("INTEND-ALL-GOALS-//-AFTER-ROOTS",
			     action_intend_all_goals_par_after_roots, 1);
     make_and_declare_action("INTEND-ALL-GOALS-//-AS-ROOTS-WITH-PRIORITY",
			     action_intend_all_goals_par_as_roots_with_priority, 2);

     make_and_declare_action("INTEND-GOAL",action_intend_goal, 1);
     make_and_declare_action("INTEND-GOAL-AFTER-BEFORE",
			     action_intend_goal_after_before,3 );
     make_and_declare_action("INTEND-GOAL-WITH-PRIORITY",
			     action_intend_goal_with_priority, 2);
     make_and_declare_action("INTEND-GOAL-WITH-PRIORITY-AFTER-BEFORE",
			     action_intend_goal_with_priority_after_before, 4);

     make_and_declare_action("TAG-CURRENT-INTENTION",tag_current_intention_action, 1);
     
     make_and_declare_action("KILL-INTENTIONS",
			     kill_intentions_action, 1); 
     make_and_declare_action("KILL-INTENTION",
			     kill_intention_action, 1); 

     make_and_declare_action("ASLEEP-INTENTIONS",
			     asleep_intentions_action, 2); 
     make_and_declare_action("ASLEEP-INTENTION",
			     asleep_intention_action, 2); 

     make_and_declare_action("WAKE-UP-INTENTION",
			     wake_up_intention_action, 1); 

     make_and_declare_action("ASLEEP-INTENTION-COND",
			     asleep_intention_with_cond_action, 2); 
     make_and_declare_action("ASLEEP-INTENTIONS-COND",
			     asleep_intentions_with_cond_action, 2); 

     make_and_declare_action("APPLY-SORT-PREDICATE-TO-ALL",
			     action_sort_all_intentions, 0);
     make_and_declare_action("SORT-INTENTION-PRIORITY",
			     action_sort_intention_by_priority, 0);
     make_and_declare_action("SORT-INTENTION-TIME",
			     action_sort_intention_by_time, 0);
     make_and_declare_action("SORT-INTENTION-PRIORITY-TIME",
			     action_sort_intention_by_priority_time, 0);
     make_and_declare_action("SORT-INTENTION-NONE",
			     action_sort_intention_none, 0);
     make_and_declare_action("SET-INTENTION-PRIORITY",
			     set_intention_priority_action, 2);

     make_and_declare_action("KILL-OTHER-INTENTIONS",
			     kill_other_intentions_action, 0);

     make_and_declare_action("READ-INSIDE-ID-VAR",
			     action_read_inside_id_var, 2);
     make_and_declare_action("READ-INSIDE-ID",
			     read_inside_id_action, 1);
     make_and_declare_action("READ-INSIDE",
			     read_inside_action, 0);

     make_and_declare_action("START-CRITICAL-SECTION",action_start_cs, 0);
     make_and_declare_action("END-CRITICAL-SECTION",action_end_cs, 0);
     make_and_declare_action("SHOW-MEMORY",action_show_memory, 0);

     make_and_declare_action("EXECUTE-COMMAND", action_execute_command, 1);
     make_and_declare_action("FAIL", action_fail, 0);
     make_and_declare_action("SUCCEED", action_succeed, 0);

     make_and_declare_action("SET-INT-ARRAY",
			     set_int_array_action, 3);
     make_and_declare_action("SET-FLOAT-ARRAY",
			     set_float_array_action, 3);

/*      make_and_declare_action("RESET-TRIED-OPS-FOR-GOAL", reset_tried_ops_for_goal_ac, 1); */
     make_and_declare_action("INTEND-AND-EXECUTE-AOP-FOR-GOAL", intend_aop_for_goal_ac, 2);

     /* declare_user_action(); */

}


#ifdef IGNORE
Term *ef_ipp_main(TermList terms)
{
     char *res_plan;
     Term *res;
     Term *ops, *fcts;
     int ipp_err;

     ops = (Term *)sl_get_slist_pos(terms, 1);
     fcts = (Term *)sl_get_slist_pos(terms, 2);
     if (ops->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     if (fcts->type != STRING)
	  report_fatal_external_error(oprs_strerror(PE_EXPECTED_STRING_TERM_TYPE));

     if ((ipp_err = setjmp(ipp_plan_jmp)) == 0) {
	  if (ipp_main(ops->u.string, fcts->u.string, &res_plan)) {
	       res = build_string(res_plan);  /* res_plan is in a sprinter... not ours */
	       return res;
	  } else {
	       fprintf(stderr, LG_STR("ERROR: IPP failed. Check the error %d.\n",
				      "ERREUR: Ipp a echoue' avec l'erreur %d.\n"), ipp_err);
	       return build_nil();
	  }
     } else {
	  return build_nil();
     }
}
#endif
