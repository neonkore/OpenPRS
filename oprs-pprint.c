static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * oprs-pprint.c -- 
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

#include "string.h"
#include "macro.h"
#include "constant.h"
#include "oprs-type.h"
#include "intention.h"
#include "op-instance.h"
#include "op-structure.h"
#include "fact-goal.h"
#include "oprs-print.h"
#include "oprs-pred-func.h"
#include "oprs-error.h"

#include "oprs-print_f.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#include "oprs-pprint_f.h"
#include "oprs-type_f-pub.h"
#include "lisp-list_f.h"
#include "oprs-error_f.h"

#ifndef NO_GRAPHIX
#include "ope-graphic.h"
#endif

/**
  function for pretty-printing
*/

Line *make_line(void)
{
     Line *line = MAKE_OBJECT(Line);
     line->str = NULL;
     line->indent = 0;
     line->width = 0;
     return line;
}

Line *transform_sprinter_in_line(Sprinter *sp)
{
     Line *res = make_line();

     res->str = SPRINTER_STRING(sp);
     res->width = strlen(SPRINTER_STRING(sp));
     
     FREE(sp);

     return (res);
}

Line *create_line(char *s)
{
     Line *line = MAKE_OBJECT(Line);

     line->indent = 0;
     line->width = strlen(s);
     line->str = (char *) MALLOC((line->width + 1) * sizeof(char));
     BCOPY(s,line->str, line->width + 1);

     return line;
}

void free_line(Line *l)
{
     if(l->str != NULL) FREE(l->str);
     FREE(l);
}

void update_indent(ListLines ll, int indent)
{
     Line *l;
     sl_loop_through_slist(ll, l, Line *) 
	  l->indent += indent;
}

Line *concat_lines(Line *l1, Line *l2) 
{
     char *new_str;
     int new_size;

     if (l1 == NULL)
	  return l2;
     if (l2 == NULL)
	  return l1;

     new_size = l1->width + l2->width;
     
     new_str = (char *) MALLOC((new_size +1) * sizeof(char));
     new_str[0] = '\0';
     strcat(new_str, l1->str);
     strcat(new_str, l2->str);
     
     FREE(l1->str);
     FREE(l2->str);
     FREE(l2);

     l1->str = new_str;
     l1->width = new_size;

     return l1;
}

void sl_concat_slist_lines(ListLines ll)
/* this function will concat all the lines in one */
{
     Line *l, *first_line;
     char *new_str, *tmp_str;
     int new_size = 0;

     if (sl_slist_empty(ll) )
	  return;

     sl_loop_through_slist(ll, l, Line *) {
	  new_size += l->width;
     }

     tmp_str = new_str = (char *) MALLOC((new_size +1) * sizeof(char));
     first_line =  (Line *) sl_get_from_head(ll); 

     tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", first_line->str));
     FREE(first_line->str);

     sl_loop_through_slist(ll, l, Line *) {
	  tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", l->str));
	  FREE(l->str);
	  FREE(l);
     }
     first_line->str = new_str;
     first_line->width = new_size;

     FLUSH_SLIST(ll);
     sl_add_to_head(ll, first_line);
     return;
}

void sl_concat_slist_lines_space(ListLines ll)
/* this function will concat all the lines, separated by a space, in one. */
{
     Line *l, *first_line;
     char *new_str, *tmp_str;
     int new_size;

     if (sl_slist_empty(ll) )
	  return;

     first_line =  (Line *) sl_get_from_head(ll); 
     new_size = first_line->width;

     sl_loop_through_slist(ll, l, Line *) {
	  new_size += l->width + 1;
     }

     tmp_str = new_str = (char *) MALLOC((new_size +1) * sizeof(char));

     tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", first_line->str));
     FREE(first_line->str);
     
     
     sl_loop_through_slist(ll, l, Line *) {
	  tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, " %s", l->str));
	  FREE(l->str);
	  FREE(l);
     }
     FLUSH_SLIST(ll);

     first_line->str = new_str;
     first_line->width = new_size;

     sl_add_to_head(ll, first_line);
     return;
}

Line *concat_all_lines_space(Line *l1, ListLines ll, Line *l2)
/* this function will concat all the lines in one, all the lines in the list
   are separated by a space */
{
     Line *l;
     char *new_str, *tmp_str;
     PBoolean first = TRUE;

     int new_size;

     if (l2 != NULL)
	  new_size = l1->width + l2->width;
     else
	  new_size = l1->width;

     sl_loop_through_slist(ll, l, Line *) {
	  new_size += l->width;
	  if (first) 
	       first = FALSE;
	  else
	       new_size++;
     }

     tmp_str = new_str = (char *) MALLOC((new_size +1) * sizeof(char));
     tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", l1->str));
     FREE(l1->str);

     first = TRUE;
     sl_loop_through_slist(ll, l, Line *) {
	  if (first) {
	       first = FALSE;
	       tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", l->str));
	  } else
	       tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, " %s", l->str));
	  FREE(l->str);
	  FREE(l);
     }
     if (l2 != NULL) {
	  tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str, "%s", l2->str));
	  FREE(l2->str);
	  FREE(l2);
     }

     l1->str = new_str;
     l1->width = new_size;

     FREE_SLIST(ll);
     return l1;
}

int max_size(ListLines ll)
{
     Line *l;
     int size = 0;
     int tmp_width;

     sl_loop_through_slist(ll, l, Line *) {
	  tmp_width = l->width+ l->indent;
	  if (tmp_width > size) size = tmp_width;
     }
     return size;
}

int total_size(ListLines ll)
{
     Line *l;
     int size = 0;

     sl_loop_through_slist(ll, l, Line *) {
	  size += l->width;
     }
     return size;
}

int total_size_sep(ListLines ll, int size_sep)   /* include the size of the separator */
{
     Line *l;
     int size;
     
     if (sl_slist_empty(ll) )
	  return 0;
     else 
	  size = size_sep * (sl_slist_length(ll) - 1);

     sl_loop_through_slist(ll, l, Line *) {
	  size += l->width;
     }
     return size;
}

char *build_string_from_list_lines(ListLines ll)
{
     Line *l;
     int size = 0;
     char *new_str, *tmp_str;
     PBoolean first_line = TRUE;
     
     if (sl_slist_empty(ll) ) {
	  NEWSTR("",new_str);
	  FREE_SLIST(ll);	/* We 've got to free this guy... Vianney... */
	  return new_str;
     }

     sl_loop_through_slist(ll, l, Line *) {
	  size += l->width + l->indent + 1;
     }
     
     tmp_str = new_str = (char *) MALLOC((size +1) * sizeof(char));

     sl_loop_through_slist(ll, l, Line *) {
	  if (l->indent > 0)
	       tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str,"%s%-*c%s", (first_line ? "" : "\n"), 
						  l->indent, ' ', l->str));
	  else
	       tmp_str += NUM_CHAR_SPRINT(sprintf(tmp_str,"%s%s", (first_line ? "" : "\n"), l->str));

	  FREE(l->str);
	  FREE(l);
	  first_line = FALSE;
     }
     FREE_SLIST(ll);

     return new_str;
}

/* pretty_print functions */

/* for theses two functions we don't know the size before printing */

ListLines pretty_print_integer(int width, int i)
{
     char tmp_str[LINSIZ];
     Line *tmp_line = make_line();
     ListLines lline = sl_make_slist();
     
     tmp_line->width = NUM_CHAR_SPRINT(sprintf(tmp_str,"%d", i));
     tmp_line->str = (char *) MALLOC((tmp_line->width + 1) * sizeof(char));
     sprintf(tmp_line->str,"%d", i);
     sl_add_to_head(lline, tmp_line);

     return lline;
}

ListLines pretty_print_user_pointer(int width, void *i)
{
     char tmp_str[LINSIZ];
     Line *tmp_line = make_line();
     ListLines lline = sl_make_slist();
     
     tmp_line->width = NUM_CHAR_SPRINT(sprintf(tmp_str,"%#x", (unsigned int)i));
     tmp_line->str = (char *) MALLOC((tmp_line->width + 1) * sizeof(char));
     sprintf(tmp_line->str,"%#x", (unsigned int)i);
     sl_add_to_head(lline, tmp_line);

     return lline;
}

ListLines pretty_print_user_memory(int width, U_Memory *i)
{
     char tmp_str[LINSIZ];
     Line *tmp_line = make_line();
     ListLines lline = sl_make_slist();
     
     tmp_line->width = NUM_CHAR_SPRINT(sprintf(tmp_str,"<%#x %d>",
							  (unsigned int)i->mem,
							  i->size));
     tmp_line->str = (char *) MALLOC((tmp_line->width + 1) * sizeof(char));
     sprintf(tmp_line->str, "<%#x %d>",
	     (unsigned int)i->mem,
	     i->size);
     sl_add_to_head(lline, tmp_line);

     return lline;
}

ListLines pretty_print_float(int width, double d)
{
     char tmp_str[LINSIZ];
     Line *tmp_line = make_line();
     ListLines lline = sl_make_slist();
     
     tmp_line->width = NUM_CHAR_SPRINT(sprintf(tmp_str,print_float_format, d));
     tmp_line->str = (char *) MALLOC((tmp_line->width + 1) * sizeof(char));
     sprintf(tmp_line->str,print_float_format, d);
     sl_add_to_head(lline, tmp_line);

     return lline;
}

ListLines pretty_print_int_array(int width, Int_array *ia)
{
     unsigned short i;
     int length = 0;

     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (ia->size == 0) {
	  first_line = create_line("[ ]");
	  sl_add_to_head(lline, first_line);
	  return lline;
     }

     first_line = create_line("[");
     last_line = create_line("]");
     
     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;

	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");

	  for(i = 0; i < ia->size ; i++) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_integer(width, ia->array[i]);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  { 
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  for(i = 0; i < ia->size ; i++) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_integer(width, ia->array[i]);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* merge the lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_float_array(int width, Float_array *fa)
{
     unsigned short i;
     int length = 0;

     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (fa->size == 0) {
	  first_line = create_line("[ ]");
	  sl_add_to_head(lline, first_line);
	  return lline;
     }

     first_line = create_line("[");
     last_line = create_line("]");
     
     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;

	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");

	  for(i = 0; i < fa->size ; i++) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_float(width, fa->array[i]);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  { 
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  for(i = 0; i < fa->size ; i++) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_float(width, fa->array[i]);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* merge the lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_l_list(int width, L_List l)
{
     L_List tmp_list;
     int length = 0;

     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (l == l_nil) {
	  first_line = create_line("(. .)");
	  sl_add_to_head(lline, first_line);
	  return lline;
     }

     first_line = create_line("(.");
     last_line = create_line(" .)");
     
     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;

	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");

	  for(tmp_list = l; tmp_list != l_nil ; tmp_list = CDR(tmp_list)) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_term(width, CAR(tmp_list));
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  { 
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  for(tmp_list = l; tmp_list != l_nil ; tmp_list = CDR(tmp_list)) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_term(width, CAR(tmp_list));
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* merge the lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *) sl_get_slist_tail(lline), last_line);   /* concat the end first 
									     in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_property(int width, Property *p)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;

     first_line = make_line();
     first_line->width = strlen(p->name) + 2 ;
     first_line->str = (char *) MALLOC((first_line->width + 1) * sizeof(char));

     sprintf(first_line->str,"(%s ", p->name);
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     if (sl_slist_length(lline = pretty_print_term(width, p->value)) <=1 ) {
	  concat_all_lines_space(first_line, lline, last_line);
	  /* concat free all , except the first_line */
	  lline = sl_make_slist();
	   sl_add_to_head(lline, first_line);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
							       in case it was only one line */
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}

ListLines pretty_print_list_property(int width, PropertyList pl)
{
     Property *p;
     int length = 0;
	  
     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if ((pl == NULL) ||  (sl_slist_empty(pl)))
	  return lline;
     
     first_line = create_line("(");
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;

	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");

	  sl_loop_through_slist(pl,p, Property *) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_property(width, p);
	       
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;

	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  {
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  if (pretty_print) cut = TRUE;

	  sl_loop_through_slist(pl,p, Property *) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_property(width, p);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
									    in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_var_list(int width, VarList vl)
{
     Envar *v;
     int length = 0;
     
     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (sl_slist_empty(vl)) {
	  first_line = create_line("()");
	  sl_add_to_head(lline, first_line);
	  return lline;
     }
     first_line = create_line("(");
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;

	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");

	  sl_loop_through_slist(vl,v, Envar *)  {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_envar(width, v);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;

	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  {
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
									    in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  sl_loop_through_slist(vl,v, Envar *)  {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_envar(width, v);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
									    in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_frame(int width, FramePtr fp)
{
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (fp==empty_env) {
	  first_line = create_line("{}");
	  sl_add_to_head(lline, first_line);
     } else {
	  first_line = create_line("{");
	  last_line = create_line("}");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

	  if (fp->installe) 
	       tmp_line = create_line("1");
	  else
	       tmp_line = create_line("0");
	  sl_add_to_tail(lline, tmp_line);
	  
	  tmp_ll = pretty_print_undo_list(width, fp->list_undo);
	  sl_concat_slist(lline, tmp_ll);
	  tmp_ll = pretty_print_frame(width, fp->previous_frame);
	  sl_concat_slist(lline, tmp_ll);

	  if (total_size_sep(lline, 1) <= width)  {  /* concat all lines */
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
								    in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }	  
     return lline;
}

ListLines pretty_print_lenv(int width, List_Envar le)
{
     Envar *ptr1;
     int length = 0;

     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (sl_slist_empty(le)) {
	  first_line = create_line("()");
	  sl_add_to_head(lline, first_line);
     } else {
	  first_line = create_line("(");
	  last_line = create_line(")");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

	  if (pretty_fill) {
	       Line *tmp_line2;
	       int tmp_width, tmp_width2;
	       
	       tmp_width= tmp_width2 = width;
	       tmp_line2 = create_line("");
	       
	       sl_loop_through_slist(le, ptr1, Envar *) {
		    if (++length == print_length) {
			 if (tmp_width >= 4) {
			      tmp_line = create_line(" ...");
			      concat_lines(tmp_line2, tmp_line);
			 } else {
			      tmp_line = create_line("...");
			      sl_add_to_tail(lline, tmp_line);
			 }
			 break;
		    }
		    tmp_ll = pretty_print_envar(width, (Envar *)ptr1);
		    if (sl_slist_length(tmp_ll) > 1 ) {
			 if (tmp_line2->width >= 1) {
			      sl_add_to_tail(lline, tmp_line2);
			      tmp_line2 = create_line("");
			      tmp_width = tmp_width2;
			 }
			 sl_concat_slist(lline, tmp_ll);
		    } else if (total_size(tmp_ll) >= tmp_width) {
			 if (tmp_line2->width >= 1) {
			      sl_add_to_tail(lline, tmp_line2);
			      tmp_line2 = create_line("");
			 } 
			 concat_all_lines_space(tmp_line2, tmp_ll, NULL);
			 tmp_width = width - tmp_line2->width;

		    } else {
			 if (tmp_line2->width >= 1)
			      concat_lines(tmp_line2, create_line(" "));
			 concat_all_lines_space(tmp_line2, tmp_ll, NULL);
			 tmp_width = width - tmp_line2->width;
		    }
	       }
	       if (tmp_line2->width >= 1)
		    sl_add_to_tail(lline, tmp_line2);
	       else
		    free_line(tmp_line2);
	       
	       if (sl_slist_length(lline) <= 1)  {
		    concat_all_lines_space(first_line, lline, last_line);
		    /* concat free all , except the first_line */
		    lline = sl_make_slist();
		    sl_add_to_head(lline, first_line);
	       } else {
		    concat_lines((Line *)sl_get_slist_tail(lline), last_line);
		    concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
		    update_indent(lline, my_indent);
		    sl_add_to_head(lline, first_line);
	       }
	  } else {
	       sl_loop_through_slist(le, ptr1, Envar *) {
		    if (++length == print_length) {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
			 break;
		    }
		    tmp_ll = pretty_print_envar(width, (Envar *)ptr1);
		    if (!cut && (sl_slist_length(tmp_ll) >1 ))
			 cut = TRUE;
		    sl_concat_slist(lline, tmp_ll);
	       }
	       if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
		    concat_all_lines_space(first_line, lline, last_line);
		    /* concat free all , except the first_line */
		    lline = sl_make_slist();
		    sl_add_to_head(lline, first_line);
	       } else {
		    concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
										 in case it was only one line */
		    concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
		    update_indent(lline, my_indent);
		    sl_add_to_head(lline, first_line);
	       }
	  } 
     }
     return lline;
}

ListLines pretty_print_envar(int width, Envar *ev)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;

     if (print_var_value && ev->value) {
	  return (pretty_print_term(width, ev->value));
     } else if (print_var_name || print_var_value) {
	  first_line = create_line(ev->name);
	  lline = sl_make_slist();
	  sl_add_to_head(lline, first_line);
     } else {
	  first_line = make_line();
	  first_line->width = strlen(ev->name) + 3 ;
	  first_line->str = (char *) MALLOC((first_line->width + 1) * sizeof(char));

	  sprintf(first_line->str,"{%s->", ev->name);
	  last_line = create_line("}");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

 	  if (sl_slist_length(lline = pretty_print_term(width, ev->value)) <=1 ) {
	       concat_all_lines_space(first_line, lline, last_line);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
							       in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

ListLines pretty_print_undo_list(int width, List_Undo lu)
{
     Undo *u;
     int length = 0;
     
     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     if (SAFE_SL_SLIST_EMPTY(lu)){
	  first_line = create_line("()");
	  sl_add_to_head(lline, first_line);
     } else {
	  first_line = create_line("(");
	  last_line = create_line(")");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;
	  
	  if (pretty_fill) {
	       Line *tmp_line2;
	       int tmp_width, tmp_width2;
	       
	       tmp_width= tmp_width2 = width;
	       tmp_line2 = create_line("");
	       
	       sl_loop_through_slist(lu, u, Undo *) {
		    if (++length == print_length) {
			 if (tmp_width >= 4) {
			      tmp_line = create_line(" ...");
			      concat_lines(tmp_line2, tmp_line);
			 } else {
			      tmp_line = create_line("...");
			      sl_add_to_tail(lline, tmp_line);
			 }
			 break;
		    }
		    tmp_ll = pretty_print_undo(width, u);
		    if (sl_slist_length(tmp_ll) > 1 ) {
			 if (tmp_line2->width >= 1) {
			      sl_add_to_tail(lline, tmp_line2);
			      tmp_line2 = create_line("");
			      tmp_width = tmp_width2;
			 }
			 sl_concat_slist(lline, tmp_ll);
		    } else if (total_size(tmp_ll) >= tmp_width) {
			 if (tmp_line2->width >= 1) {
			      sl_add_to_tail(lline, tmp_line2);
			      tmp_line2 = create_line("");
			 } 
			 concat_all_lines_space(tmp_line2, tmp_ll, NULL);
			 tmp_width = width - tmp_line2->width;

		    } else {
			 if (tmp_line2->width >= 1)
			      concat_lines(tmp_line2, create_line(" "));
			 concat_all_lines_space(tmp_line2, tmp_ll, NULL);
			 tmp_width = width - tmp_line2->width;
		    }
	       }
	       if (tmp_line2->width >= 1)
		    sl_add_to_tail(lline, tmp_line2);
	       else
		    free_line(tmp_line2);
	       
	       if (sl_slist_length(lline) <= 1)  {
		    sl_add_to_head(lline, first_line);
		    sl_add_to_tail(lline, last_line);
		    sl_concat_slist_lines(lline);
	       } else {
		    concat_lines((Line *)sl_get_slist_tail(lline), last_line);
		    concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
		    update_indent(lline, my_indent);
		    sl_add_to_head(lline, first_line);
	       }
	  } else {
	       sl_loop_through_slist(lu, u, Undo *) {
		    if (++length == print_length) {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
			 break;
		    }
		    tmp_ll = pretty_print_undo(width, u);
		    if (!cut && (sl_slist_length(tmp_ll) >1 ))
			 cut = TRUE;
		    sl_concat_slist(lline, tmp_ll);
	       }
	       if (!cut && (total_size(lline) <= width) ) {  /* concat all lines */
		    sl_add_to_head(lline, first_line);
		    sl_add_to_tail(lline, last_line);
		    sl_concat_slist_lines(lline);
	       } else {
		    concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
										 in case it was only one line */
		    concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
		    update_indent(lline, my_indent);
		    sl_add_to_head(lline, first_line);
	       }
	  }
     }
     return lline;
}

ListLines pretty_print_undo(int width, Undo *ud)
{
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line, *last_line;

     first_line = create_line("[");
     last_line = create_line("]");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     tmp_ll = pretty_print_envar(width, ud->envar);
     sl_concat_slist(lline, tmp_ll);

     tmp_line = create_line(">>");
     sl_add_to_tail(lline, tmp_line);

     tmp_ll = pretty_print_term(width, ud->value);
     sl_concat_slist(lline, tmp_ll);

     if (sl_slist_length(lline) <= 3) {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}

ListLines pretty_print_pred_func_rec(int width, Pred_Func_Rec *pr)
{
     if (pr->name) {
	  ListLines lline = sl_make_slist();
	  Line *line = create_line(pr->name);

	  sl_add_to_head(lline, line);

	  return lline;
     } else {
	  return pretty_print_envar(width, pr->u.envar);
     }
}

#define EXPR_INDENT 2 

ListLines pretty_print_expr(int width, Expression *expr)
{
     int my_indent, my_width = width;
     ListLines lline;
     Line *first_line, *tmp_line, *last_line;
     PBoolean concat_first;

     if (!expr) {
	  return ((ListLines) sl_make_slist());
     } 

     first_line = create_line("(");
     last_line = create_line(")");

     if (sl_slist_empty(expr->terms)) { 
	  lline = pretty_print_pred_func_rec(width, expr->pfr);
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
	  return lline;
     }

     lline = pretty_print_pred_func_rec(width, expr->pfr);
     sl_concat_slist_lines(lline);
     tmp_line = sl_get_slist_head(lline);
     FREE_SLIST(lline);

     if (tmp_line->width <=  EXPR_INDENT) {
	  concat_first = TRUE;
	  my_indent = first_line->width + tmp_line->width;
     } else {
	  concat_first = FALSE;
	  my_indent = first_line->width + EXPR_INDENT;
     }
     width -= my_indent + last_line->width;

     concat_lines(first_line, tmp_line);
     my_width -= first_line->width + last_line->width;

     if ((sl_slist_length(lline = pretty_print_tl(width, expr->terms)) <= 1) && 
	 (total_size_sep(lline, 1) <= my_width))  {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line); 
	  if (concat_first)
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}

#ifdef IGNORE
ListLines pretty_print_expr_old(int width, Expression *expr)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;

     first_line = create_line("(");
     last_line = create_line(")");

     concat_lines(first_line, create_line(expr->pfr->name)); 
	  
     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     if (sl_slist_length(lline = pretty_print_tl(width, expr->terms)) <= 1) {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
							       in case it was only one line */
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}
#endif

ListLines pretty_print_special_action_field(int width, Action_Field *ac)
{
     int length = 0, i;

     PBoolean cut = FALSE;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line = NULL, *tmp_line;

     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;
	  
	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");
	  
	  for(i = 0; i < 2; i++) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       if (i == 0)
		    if (ac->multi)
			 tmp_ll = pretty_print_lenv(width,ac->u.list_envar);
		    else
			 tmp_ll = pretty_print_envar(width,ac->u.envar);
	       else
		    tmp_ll = pretty_print_expr(width, ac->function);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;

	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
     } else {
	  if (pretty_print) cut = TRUE;
	  
	  for(i = 0; i < 2; i++) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       if (i == 0)
		    if (ac->multi)
			 tmp_ll = pretty_print_lenv(width,ac->u.list_envar);
		    else
			 tmp_ll = pretty_print_envar(width,ac->u.envar);
	       else
		    tmp_ll = pretty_print_expr(width, ac->function);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	       first_line = create_line("");
	       concat_all_lines_space(first_line, lline, NULL);
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } 
     }
     return lline;

}


ListLines pretty_print_action_field(int width, Action_Field *ac)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;

     if (ac->special) {
	  first_line = create_line("(*=* ");
	  last_line = create_line(")");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

	  if (sl_slist_length(lline =  pretty_print_special_action_field(width,ac)) <= 1) {
	       sl_add_to_head(lline, first_line);
	       sl_add_to_tail(lline, last_line);
	       sl_concat_slist_lines(lline);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line); /* concat the end first 
									  in case it was only one line */
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
	  return lline;
     } else
	  return pretty_print_expr(width,ac->function);
}

ListLines pretty_print_expr_frame(int width, ExprFrame *ef)
{
     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *last_line;

     first_line = create_line("[");
     last_line = create_line("]");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;

     tmp_ll = pretty_print_expr(width, ef->expr);
     if (sl_slist_length(tmp_ll) >1 ) cut = TRUE;
     sl_concat_slist(lline, tmp_ll);

     tmp_ll = pretty_print_frame(width, ef->frame);
     if (sl_slist_length(tmp_ll) >1 ) cut = TRUE;
     sl_concat_slist(lline, tmp_ll);

     if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	  concat_all_lines_space(first_line, lline, last_line);
	  /* concat free all , except the first_line */
	  lline = sl_make_slist();
	  sl_add_to_head(lline, first_line);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
							       in case it was only one line */
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     } 
     return lline;
}

ListLines pretty_print_tl(int width, TermList tl)
{
     Term *t;
     int length = 0;

     PBoolean cut = FALSE;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line;

     if (sl_slist_empty(tl)){
	  return lline;
     }
     first_line = create_line(" ");
     my_indent = first_line->width;
     width -= first_line->width;

     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;
	  
	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");
	  
	  sl_loop_through_slist(tl, t, Term *) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_term(width, t);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;

	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
	  if (sl_slist_length(lline) <= 1)  {
	       concat_all_lines_space(first_line, lline, NULL);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     } else {
	  sl_loop_through_slist(tl, t, Term *) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_term(width, t);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	       concat_all_lines_space(first_line, lline, NULL);
	       /* concat free all , except the first_line */
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } else {
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
     }
     return lline;
}

#ifdef IGNORE
#define EXPRESSION_INDENT 2

ListLines pretty_print_expr(int width, Expression *tc)
{
     int my_indent, my_width = width;
     ListLines lline;
     Line *first_line, *tmp_line, *last_line;
     PBoolean concat_first;

     first_line = create_line("(");
     last_line = create_line(")");
     if (sl_slist_empty(tc->terms)) { 
	  lline = pretty_print_pred_func_rec(width, tc->pfr);
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
	  return lline;
     }

     lline = pretty_print_pred_func_rec(width, tc->pfr);
     sl_concat_slist_lines(lline);
     tmp_line = sl_get_slist_head(lline);
     FREE_SLIST(lline);

     if (tmp_line->width <=  EXPRESSION_INDENT) {
	  concat_first = TRUE;
	  my_indent = first_line->width + tmp_line->width;
     } else {
	  concat_first = FALSE;
	  my_indent = first_line->width + EXPRESSION_INDENT;
     }
     width -= my_indent + last_line->width;

     concat_lines(first_line, tmp_line);
     my_width -= first_line->width + last_line->width;

     if ((sl_slist_length(lline = pretty_print_tl(width, tc->terms)) <=1 ) &&
	 (total_size_sep(lline, 1) <= my_width))  {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line); 
	  if (concat_first)
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}

ListLines pretty_print_expr_old(int width, Expression *tc)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;

     first_line = make_line();
     first_line->width = strlen(tc->pfr->name) + 1 ;
     first_line->str = (char *) MALLOC((first_line->width + 1) * sizeof(char));

     sprintf(first_line->str,"(%s",tc->pfr->name);
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
     if (sl_slist_length(lline = pretty_print_tl(width, tc->terms)) <=1 ) {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
							       in case it was only one line */
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}
#endif

ListLines pretty_print_backslash_string(int width, PString string)
{
     ListLines lline = sl_make_slist();

     if (string) { 
	  Line *tmp_line;
	  Sprinter *tmp_str = make_sprinter(BUFSIZ);

	  sprint_backslash_string(tmp_str, string);
	  tmp_line = transform_sprinter_in_line(tmp_str);
	  
	  sl_add_to_head(lline, tmp_line);
     }
     return lline;
}

ListLines pretty_print_string(int width, PString string)
{
     ListLines lline;
     Line *tmp_line;

     lline = sl_make_slist();
     if (string) { 
	  tmp_line = create_line(string);
	  sl_add_to_tail(lline, tmp_line);
     }

     return lline;
}

ListLines pretty_print_term(int width, Term *term)
{
     int my_indent;
     ListLines lline = NULL;	/* Just to please GCC. */
     Line *first_line, *tmp_line, *last_line;
     Line *current_line = NULL; /* Just to please GCC. */

     int i;
	  
     if (term == (Term *)NULL)
	  return ((ListLines) sl_make_slist());
    
     switch (term->type) {
     case INTEGER: 	 
	  return (pretty_print_integer(width, term->u.intval));
     case FLOAT_ARRAY: 	 
	  return (pretty_print_float_array(width, term->u.float_array));
     case INT_ARRAY: 	 
	  return (pretty_print_int_array(width, term->u.int_array));
     case U_POINTER: 	 
	  return (pretty_print_user_pointer(width, term->u.u_pointer));
     case U_MEMORY: 	 
	  return (pretty_print_user_memory(width, term->u.u_memory));
     case TT_FLOAT: 	 
	  return (pretty_print_float(width, *term->u.doubleptr));
     case STRING:
	  if (print_backslash) {
	       return (pretty_print_backslash_string(width, term->u.string));
	  } else {
	       lline = sl_make_slist();
	       tmp_line = create_line(term->u.string);
	       sl_add_to_tail(lline, tmp_line);
	  }
	  break;
     case TT_ATOM:
	  lline = sl_make_slist();
	  tmp_line = create_line(term->u.id);
	  sl_add_to_tail(lline, tmp_line);
	  break;
     case VARIABLE:
	  return (pretty_print_envar(width, term->u.var));
     case EXPRESSION:
	  return (pretty_print_expr(width, term->u.expr));
/*      case LENV: */
/* 	  return (pretty_print_var_list(width, term->u.var_list)); */
     case TT_FACT:
	  return pretty_print_fact(width, term->u.fact);
	  break;
     case TT_GOAL:
	  return pretty_print_goal(width,term->u.goal);
	  break;
     case TT_OP_INSTANCE:
	  return pretty_print_op_instance(width,term->u.opi);
	  break;
     case TT_INTENTION:
	  return pretty_print_intention(width, term->u.in);
	  break;
     case LISP_LIST: 	 
	  return (pretty_print_l_list(width, term->u.l_list));
     case C_LIST:
	  lline = sl_make_slist();
	  first_line = create_line("(");
	  last_line = create_line(")");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

	  for(i = sl_slist_length(term->u.c_list); i > 0; i--) {
	       tmp_line = create_line(". ");
	       current_line = concat_lines(current_line, tmp_line);
	       if (current_line->width >= width) {
		    sl_add_to_tail(lline, current_line);
		    current_line = NULL;
	       }
	  }
	  if (sl_slist_length(lline) <= 1) {
	       sl_add_to_head(lline, first_line);
	       sl_add_to_tail(lline, last_line);
	       sl_concat_slist_lines(lline);
	  } else {
	       concat_lines((Line *)sl_get_slist_tail(lline), last_line);
	       concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	       update_indent(lline, my_indent);
	       sl_add_to_head(lline, first_line);
	  }
	  break;
     default: 	 
	  oprs_perror("pretty_print_term",PE_UNEXPECTED_TERM_TYPE);
	  break;
     }
     return lline;
}

ListLines pretty_print_list_expr(int width, ExprList lexpr)
{
     Expression *expr;
     int length = 0;

     PBoolean cut = FALSE;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line = NULL, *tmp_line;

     if ((lexpr == NULL) ||  (sl_slist_empty(lexpr)))
	  return lline;

     if (pretty_fill) {
	  Line *tmp_line2;
	  int tmp_width, tmp_width2;
	  
	  tmp_width= tmp_width2 = width;
	  tmp_line2 = create_line("");
	  
	  sl_loop_through_slist(lexpr,expr, Expression *) {
	       if (++length == print_length) {
		    if (tmp_width >= 4) {
			 tmp_line = create_line(" ...");
			 concat_lines(tmp_line2, tmp_line);
		    } else {
			 tmp_line = create_line("...");
			 sl_add_to_tail(lline, tmp_line);
		    }
		    break;
	       }
	       tmp_ll = pretty_print_expr(width, expr);
	       if (sl_slist_length(tmp_ll) > 1 ) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
			 tmp_width = tmp_width2;
		    }
		    sl_concat_slist(lline, tmp_ll);
	       } else if (total_size(tmp_ll) >= tmp_width) {
		    if (tmp_line2->width >= 1) {
			 sl_add_to_tail(lline, tmp_line2);
			 tmp_line2 = create_line("");
		    } 
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;

	       } else {
		    if (tmp_line2->width >= 1)
			 concat_lines(tmp_line2, create_line(" "));
		    concat_all_lines_space(tmp_line2, tmp_ll, NULL);
		    tmp_width = width - tmp_line2->width;
	       }
	  }
	  if (tmp_line2->width >= 1)
	       sl_add_to_tail(lline, tmp_line2);
	  else
	       free_line(tmp_line2);
	  
     } else {
	  if (pretty_print) cut = TRUE;
	  
	  sl_loop_through_slist(lexpr,expr, Expression *) {
	       if (++length == print_length) {
		    tmp_line = create_line("...");
		    sl_add_to_tail(lline, tmp_line);
		    break;
	       }
	       tmp_ll = pretty_print_expr(width, expr);
	       if (!cut && (sl_slist_length(tmp_ll) >1 ))
		    cut = TRUE;
	       sl_concat_slist(lline, tmp_ll);
	  }
	  if (!cut && (total_size_sep(lline, 1) <= width) ) {  /* concat all lines */
	       first_line = create_line("");
	       concat_all_lines_space(first_line, lline, NULL);
	       lline = sl_make_slist();
	       sl_add_to_head(lline, first_line);
	  } 
     }
     return lline;
}


ListLines pretty_print_exprlist(int width, ExprList lexpr)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;
	  
     first_line = create_line("(");
     last_line = create_line(")");
     
     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
     if (sl_slist_length(lline = pretty_print_list_expr(width, lexpr)) <= 1) {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *)sl_get_slist_tail(lline), last_line);   /* concat the end first 
								       in case it was only one line */
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;

}


#ifdef IGNORE
ListLines pretty_print_add_del_list(int width, Add_Del_List *adl)
{
     int my_indent;
     ListLines lline;
     Line *first_line, *last_line;
     
	  
     if (adl == NULL)
	  return sl_make_slist();

     first_line = create_line("(");
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     lline = sl_make_slist();

     if (! sl_slist_empty(adl->add)) {
	  sl_concat_slist(lline, pretty_print_add_or_del(width, adl->add, ADD));
     }
	       
     if (! sl_slist_empty(adl->del)) {
	  sl_concat_slist(lline, pretty_print_add_or_del(width, adl->del, DEL));
     }


     if (! sl_slist_empty(adl->cond_add)) {
	  sl_concat_slist(lline, pretty_print_add_or_del(width, adl->cond_add, COND_ADD));
     }
	       
     if (! sl_slist_empty(adl->cond_del)) {
	  sl_concat_slist(lline, pretty_print_add_or_del(width, adl->cond_del, COND_DEL));
     }

     
     if (sl_slist_empty(lline)) {
	  sl_add_to_head(lline, first_line);
	  sl_add_to_tail(lline, last_line);
	  sl_concat_slist_lines(lline);
     } else {
	  concat_lines((Line *) sl_get_slist_tail(lline), last_line);
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  update_indent(lline, my_indent);
	  sl_add_to_head(lline, first_line);
     }
     return lline;
}
#endif

ListLines pretty_print_fact(int width, Fact *fact)
{
     char tmp_str[32]; /* 18 should be enough */
     ListLines lline = sl_make_slist();

     sprintf(tmp_str,LG_STR("<Fact %#x>",
			    "<Fait %#x>"), (unsigned int)fact);
     
     sl_add_to_head(lline, create_line(tmp_str));
     return lline;
}

ListLines pretty_print_goal(int width, Goal *goal)
{
     char tmp_str[32]; /* 18 should be enough */
     ListLines lline = sl_make_slist();

     sprintf(tmp_str,LG_STR("<Goal %#x>",
			    "<But %#x>"),(unsigned int)goal);
     
     sl_add_to_head(lline, create_line(tmp_str));
     return lline;
}

ListLines pretty_print_intention(int width, Intention *in)
{
     char tmp_str[32]; /* 23 should be enough */
     ListLines lline = sl_make_slist();
     Line *first_line, *tmp_line;

     if (in->id == NULL) {
	  sprintf(tmp_str,LG_STR("<Intention %#x>",
				 "<Intention %#x>"), (unsigned int)in);
	  first_line = create_line(tmp_str);
     } else {
	  sprintf(tmp_str,LG_STR("<Intention %#x",
				 "<Intention %#x"), (unsigned int)in);
	  first_line = create_line(tmp_str);

	  tmp_line = make_line();
	  tmp_line->width = strlen(in->id) + 4 ;
	  tmp_line->str = (char *) MALLOC((tmp_line->width + 1) * sizeof(char));
	  sprintf(tmp_line->str," (%s)>", in->id);
	  concat_lines(first_line, tmp_line);
     }
     sl_add_to_head(lline, first_line);
     return lline;
}

ListLines pretty_print_op_instance(int width,  Op_Instance *opi)
{
     char tmp_str[32]; /* 25 should be enough */
     ListLines lline = sl_make_slist();

     sprintf(tmp_str,LG_STR("<Op_instance %#x>",
			    "<Op_instance %#x>"), (unsigned int)opi);
     
     sl_add_to_head(lline, create_line(tmp_str));
     return lline;
}


/* pretty print functions */
ListLines pretty_print_instruction(int width, Instruction *inst);

ListLines pretty_print_list_instruction(int width, List_Instruction insts)
{
     Instruction *inst;
	  
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();

     if ((!insts) || (sl_slist_empty(insts))) {
	  return lline;
     }

     sl_loop_through_slist(insts, inst, Instruction *) {
	  tmp_ll = pretty_print_instruction(width,inst);
	  sl_concat_slist(lline, tmp_ll);
     }

     return lline;
}

ListLines pretty_print_body(int width, Body *body)
{
     if (!body)
	  return sl_make_slist();
     else {
	  int my_indent;
	  ListLines lline;
	  Line *first_line, *last_line;

	  if ((!body->insts) || (sl_slist_empty(body->insts))) {
	       lline = sl_make_slist();
	       first_line = create_line("()");
	       sl_add_to_head(lline, first_line);
#ifndef NO_GRAPHIX
	       current_body_line++;
#endif
	       return lline;
	  }

	  first_line = create_line("(");
	  last_line = create_line(")");

	  my_indent = first_line->width;
	  width -= first_line->width + last_line->width;

#ifndef NO_GRAPHIX
	  current_body_indent +=  my_indent;
#endif
	  lline = pretty_print_list_instruction(width,body->insts);
	  update_indent(lline, my_indent);
#ifndef NO_GRAPHIX
	  current_body_indent -=  my_indent;
#endif
	  sl_add_to_tail(lline,last_line);
	  concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
	  sl_add_to_head(lline, first_line);

#ifndef NO_GRAPHIX
	       current_body_line++;
#endif
	  return lline;
     }
}

#define IF_INDENT 3

ListLines pretty_print_part_if_instruction(int width, If_Instruction *if_inst, PBoolean elseif)
{
     ListLines lline, lline_then_ins, lline_else_ins;

     lline = pretty_print_expr(width - (elseif ? 8: 4), if_inst->condition);

#ifndef NO_GRAPHIX
     if (if_inst->og != NULL) {
	  Ginst *og_inst = if_inst->og->u.ginst;

	  og_inst->num_line = current_body_line;
	  og_inst->indent = current_body_indent + (elseif ? 5 : 1); /* Current_body_indent is already 4 chars ahead. */
	  current_body_line += og_inst->nb_lines = sl_slist_length(lline);
	  og_inst->width = max_size(lline);
     }
#endif

     update_indent(lline, (elseif ? 8: 4));	/* size of "(IF " */

     lline_then_ins = pretty_print_list_instruction(width - IF_INDENT,if_inst->then_insts);
     update_indent(lline_then_ins, IF_INDENT);
     sl_concat_slist(lline,lline_then_ins);

     if (if_inst->elseif) {
	  Line *elseif_line;

	  elseif_line = create_line((lower_case_id ? " elseif " : " ELSEIF "));

	  lline_else_ins = pretty_print_part_if_instruction(width,if_inst->u.elseif_inst->u.if_inst,TRUE);

	  concat_lines(elseif_line, (Line *)sl_get_from_head(lline_else_ins));
	  sl_add_to_head(lline_else_ins,elseif_line); 
 
	  sl_concat_slist(lline,lline_else_ins);
     } else 
	  if (if_inst->u.else_insts) {
	       sl_add_to_tail(lline, create_line((lower_case_id ? " else" : " ELSE")));

#ifndef NO_GRAPHIX
	  current_body_line++;
#endif
	       lline_else_ins = pretty_print_list_instruction(width - IF_INDENT,if_inst->u.else_insts);
	       update_indent(lline_else_ins, IF_INDENT);
	       sl_concat_slist(lline,lline_else_ins);
     }
     
     return lline;
}

ListLines pretty_print_if_instruction(int width, If_Instruction *if_inst)
{
     ListLines lline;
     Line *first_line, *last_line;

     first_line = create_line((lower_case_id ? "(if " : "(IF "));
     last_line = create_line(")");
          
#ifndef NO_GRAPHIX
     current_body_indent +=  IF_INDENT;
#endif

     lline = pretty_print_part_if_instruction(width,if_inst, FALSE);

     concat_lines(first_line, (Line *)sl_get_from_head(lline));
     sl_add_to_head(lline,first_line); 
     sl_add_to_tail(lline, last_line);

#ifndef NO_GRAPHIX
     current_body_indent -=  IF_INDENT;
     current_body_line++;
#endif

     return lline;
}

#define WHILE_INDENT 3

ListLines pretty_print_while_instruction(int width, While_Instruction *while_inst)
{
     ListLines lline, lline_ins;
     Line *first_line, *last_line;

     first_line = create_line((lower_case_id ? "(while " : "(WHILE "));
     last_line = create_line(")");

#ifndef NO_GRAPHIX
/* test for strings in body, TO DELETE
     nb_cr_in_strings = 0;
     */
#endif
     lline = pretty_print_expr(width - first_line->width,while_inst->condition);
#ifndef NO_GRAPHIX
     if (while_inst->og != NULL) {
	  Ginst *og_inst = while_inst->og->u.ginst;

	  og_inst->num_line = current_body_line;
	  og_inst->indent = current_body_indent + 7;	/* Size of "(while ". */

	  current_body_line += og_inst->nb_lines = sl_slist_length(lline);
	  og_inst->width = max_size(lline);
     }
#endif
     concat_lines(first_line, (Line *)sl_get_from_head(lline));
     update_indent(lline, 7);	/* Size of "(while ". */
     sl_add_to_head(lline, first_line); 

#ifndef NO_GRAPHIX
     current_body_indent +=  WHILE_INDENT;
#endif

     lline_ins = pretty_print_list_instruction(width - WHILE_INDENT,while_inst->insts);
     update_indent(lline_ins, WHILE_INDENT);
     
     sl_concat_slist(lline,lline_ins);
     sl_add_to_tail(lline, last_line);

#ifndef NO_GRAPHIX
     current_body_indent -=  WHILE_INDENT;
     current_body_line++;
#endif
     return lline;
}

#define DO_INDENT 3

ListLines pretty_print_do_instruction(int width, Do_Instruction *do_inst)
{
     ListLines lline, lline_tmp;
     Line *first_line, *until_line, *tmp_line;

     first_line = create_line((lower_case_id ? "(do" : "(DO"));

#ifndef NO_GRAPHIX
     current_body_line++;
     current_body_indent +=  DO_INDENT;
#endif
     lline = pretty_print_list_instruction(width - DO_INDENT,do_inst->insts);
     update_indent(lline, DO_INDENT);
#ifndef NO_GRAPHIX
     current_body_indent -=  DO_INDENT;
#endif

     sl_add_to_head(lline,first_line);
     
     until_line = create_line((lower_case_id ? " while " : " WHILE "));
     lline_tmp = pretty_print_expr(width - until_line->width - 1 ,do_inst->condition);
#ifndef NO_GRAPHIX
     if (do_inst->og != NULL) {
	  Ginst *og_inst = do_inst->og->u.ginst;

	  og_inst->num_line = current_body_line;
	  og_inst->indent = current_body_indent + 7; /* size of " while " */

	  current_body_line += og_inst->nb_lines = sl_slist_length(lline_tmp);
	  og_inst->width = max_size(lline_tmp);
     }
#endif
     concat_lines(until_line, (Line *)sl_get_from_head(lline_tmp));
     update_indent(lline_tmp, 7); /* size of " while " */
     sl_add_to_head(lline_tmp, until_line);

     tmp_line = concat_lines((Line *)sl_get_from_tail(lline_tmp), create_line(")"));
     sl_add_to_tail(lline_tmp, tmp_line); 
     
     sl_concat_slist(lline,lline_tmp);
     return lline;
}

#define PAR_INDENT 3

ListLines pretty_print_par_instruction(int width, Par_Instruction *par)
{
     Body *body;
     int my_indent;
     ListLines tmp_ll;
     ListLines lline = sl_make_slist();
     Line *first_line, *last_line;

     first_line = create_line("(// ");
     last_line = create_line(")");

     my_indent = first_line->width;
     width -= first_line->width + last_line->width;
     
#ifndef NO_GRAPHIX
     current_body_indent +=  my_indent;
#endif

     sl_loop_through_slist(par->bodys, body, Body *) {
	  tmp_ll = pretty_print_body(width - PAR_INDENT,body);
	  sl_concat_slist(lline, tmp_ll);
     }

     update_indent(lline, my_indent);
     sl_add_to_tail(lline, last_line);
     concat_lines(first_line, (Line *)sl_get_from_head(lline)); 
     sl_add_to_head(lline, first_line);

#ifndef NO_GRAPHIX
     current_body_indent -=  my_indent;
     current_body_line++; /* The last line */
#endif
     return lline;
}

ListLines pretty_print_simple_instruction(int width, Simple_Instruction *simple)
{
     ListLines lline = pretty_print_expr(width,simple->expr);

#ifndef NO_GRAPHIX
     if (simple->og != NULL) {
	  Ginst *og_inst = simple->og->u.ginst;

	  og_inst->num_line = current_body_line;
	  og_inst->indent = current_body_indent;

	  current_body_line += og_inst->nb_lines = sl_slist_length(lline);
	  og_inst->width = max_size(lline);
     }
#endif
     return lline;
}

ListLines pretty_print_instruction(int width, Instruction *inst)
{
     switch(inst->type) {
     case IT_SIMPLE:
	  return pretty_print_simple_instruction(width, inst->u.simple_inst);
	  break;
     case IT_COMMENT: {
	  ListLines lline = sl_make_slist();

	  sl_add_to_tail(lline, create_line(inst->u.comment));
#ifndef NO_GRAPHIX
	  current_body_line++;
#endif
	  return lline;
	       }
	  break;
     case IT_BREAK: {
	  ListLines lline = sl_make_slist();

	  sl_add_to_tail(lline, create_line(lower_case_id ? "break" : "BREAK"));
#ifndef NO_GRAPHIX
	  current_body_line++;
#endif
	  return lline;
	       }
	  break;
     case IT_GOTO: {
	  ListLines lline = sl_make_slist();
	  Line *line;
	  
	  line = create_line(lower_case_id ? "goto " : "GOTO ");
	  concat_lines(line,create_line(inst->u.goto_inst));
#ifndef NO_GRAPHIX
	  current_body_line++;
#endif
	  sl_add_to_tail(lline, line);
	  return lline;
	       }
	  break;
     case IT_LABEL: {
	  ListLines lline = sl_make_slist();
	  Line *line;
	  
	  line = create_line(lower_case_id ? "label " : "LABEL ");
	  concat_lines(line,create_line(inst->u.goto_inst));
#ifndef NO_GRAPHIX
	  current_body_line++;
#endif
	  sl_add_to_tail(lline, line);

	  return lline;
	       }
	  break;
     case IT_IF:
	  return pretty_print_if_instruction(width, inst->u.if_inst);
	  break;
     case IT_WHILE:
	  return pretty_print_while_instruction(width, inst->u.while_inst);
	  break;
     case IT_DO:
	  return pretty_print_do_instruction(width, inst->u.do_inst);
	  break;
     case IT_PAR:
	  return pretty_print_par_instruction(width, inst->u.par_inst);
	  break;
     default: 	 
	  oprs_perror("pretty_print_instruction", PE_UNEXPECTED_INSTRUCTION_TYPE);
	  return sl_make_slist();
	  break;
     }
}
