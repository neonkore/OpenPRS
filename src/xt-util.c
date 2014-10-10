/*                               -*- Mode: C -*- 
 * xt-util.c -- 
 * 
 * Copyright (c) 1991-2014 Francois Felix Ingrand.
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
1 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
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


#include <stdlib.h>
#include <stdio.h>
#include "ope-gope.h"

#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>

#include "opaque.h"
#include "xt-util_f.h"

extern PBoolean flushing_xt_events;
extern XtAppContext app_context;

void show_widget_hierarchy(Widget  w, String  hierarchy_name)
{
     CompositeWidget cw;
     Widget  kid;
     unsigned int     i;
     String  new_hier_name;

     fprintf(stderr, "%s%s (%s)\n", hierarchy_name, XtName(w), (XtClass(w))->core_class.class_name);

     new_hier_name = (String)malloc( strlen(hierarchy_name) +
				    strlen((XtName(w))) +
				    sizeof('.') + 1);

     new_hier_name[0] = '\0';
     strcat(new_hier_name, hierarchy_name);
     strcat(new_hier_name, XtName(w));
     strcat(new_hier_name, ".");

     if (XtIsWidget(w) &&  XtIsComposite(w)) {
	  cw = (CompositeWidget)w;

	  /*
	   *      Print path for nonterminal nodes.
	   */

	  for (i=0; i < cw->composite.num_children; i++) {
	       kid = cw->composite.children[i];


	       show_widget_hierarchy(kid, new_hier_name);
	  }
     }
	  
     /*
      *      Test every widget for popup children.
      */

     if (XtIsWidget(w) && w->core.num_popups > 0) {

	  for (i=0; i < w->core.num_popups; i++) {
	       Widget wp = w->core.popup_list[i];

	       show_widget_hierarchy(wp, new_hier_name);

	  }
     }
     free(new_hier_name);
}

void show_top_widget_hierarchy(Widget  w)
{
     show_widget_hierarchy(w,"");
}

void print_child(Widget w, unsigned int indent)
{  
     /* WidgetClass class; */
     unsigned i;

     /* class = XtClass(w); */

     for (i = 0 ; i < indent; i++)
	  fprintf(stderr,"   |");

     fprintf(stderr,"%s, (%s)%c\n",
	    XtName(w),
	    (XtClass(w))->core_class.class_name,
	    (XtIsWidget(w) && (XtIsComposite(w) || w->core.num_popups >0)) ? ':' : '.' );

     indent++;
     
     if (XtIsWidget(w) && XtIsComposite(w)) {
	  CompositeWidget cw;
	  Widget wc;
	  
	  cw = (CompositeWidget)w;

	  for (i = 0 ; i < cw->composite.num_children ; i++) {
	       wc = cw->composite.children[i];
	       print_child(wc, indent);
	  }
     }

     if (XtIsWidget(w) && w->core.num_popups > 0) {
	
	  for (i=0; i < w->core.num_popups; i++) {
	       Widget wp = w->core.popup_list[i];

	       print_child(wp, indent);
	  }
     }
}

void print_top_widget(Widget  w)
{
     print_child(w,0);
}

void process_xt_events()
{
  if (!gtk) {
     XtInputMask im;
     PBoolean tmp = flushing_xt_events;

     flushing_xt_events = TRUE;

     while ((im = XtAppPending(app_context)))
	  /* && 
	   * (im & (XtIMXEvent | XtIMAlternateInput))
	   */
     {
	  /*
	   * XEvent event;
	   * XtAppNextEvent(app_context, &event); This function does not get Timer events..... Arg
	   * XtDispatchEvent(&event); So we could get stuck in the middle.
	   */
	  
	  XtAppProcessEvent(app_context,XtIMAll); /* This one does it all. */
     }
     flushing_xt_events = tmp;
}
}
