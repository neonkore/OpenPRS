static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * ope-report.c -- 
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

#include <stdio.h>


#include <gtk/gtk.h>

#include "xm2gtk.h"

#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "op-structure.h"
#include "parser-funct.h"
#include "gope-graphic.h"
#include "gope-global.h"
#include "gope-report_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

#include "xm2gtk_f.h"

Widget ope_information;

void ope_information_create(Widget parent)
{
#ifdef IGNORE_GTK
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     XtSetArg(args[n], XmNdialogTitle, XmStringCreateLtoR("Information", XmSTRING_DEFAULT_CHARSET)); n++;
     ope_information = XmCreateInformationDialog(parent, "ope_information", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(ope_information, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(ope_information, XmDIALOG_CANCEL_BUTTON));
#endif
}

void ope_information_report(char *message)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new(GTK_WINDOW(topLevel),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_INFO,
				  GTK_BUTTONS_OK,
				  "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Information");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

Widget user_error;

void ope_create_user_error(Widget parent)
{
#ifdef IGNORE_GTK
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     user_error = XmCreateErrorDialog(parent, "user_error", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(user_error, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(user_error, XmDIALOG_CANCEL_BUTTON));
#endif
}

void report_user_error(PString message)
{
#ifdef IGNORE_GTK
     XmString to_free;
     Arg args[1];

     XtSetArg(args[0], XmNmessageString, to_free = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(user_error, args, 1);
     XmStringFree(to_free);
     XtManageChild(user_error);
#endif
}

Widget syntax_error;

void create_syntax_error_dialog(Widget parent)
{
#ifdef IGNORE_GTK
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     syntax_error = XmCreateErrorDialog(parent, "syntax_error", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(syntax_error, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(syntax_error, XmDIALOG_CANCEL_BUTTON));
#endif
}

void report_syntax_error(PString message)
{
#ifdef IGNORE_GTK
     XmString to_free, str1, str2;
     Arg args[1];

     if (parser_message != NULL) {
	  str1 = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
	  str2 = XmStringCreateLtoR(parser_message, XmSTRING_DEFAULT_CHARSET);

	  to_free = XmStringConcat(str1, str2);
	  XmStringFree(str1);
	  XmStringFree(str2);
	  
	  FREE(parser_message);
	  parser_message = NULL;
	  too_long_message = FALSE;
     } else {
	  to_free = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
     }
     XtSetArg(args[0], XmNmessageString, to_free);
     XtSetValues(syntax_error, args, 1);
     XmStringFree(to_free);
     XtManageChild(syntax_error);
#endif
}
