static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * ope-report.c -- 
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

#include <stdio.h>

/*
 * Standard Toolkit include files
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/MessageB.h>

#include "constant.h"
#include "macro.h"
#include "opaque.h"
#include "parser-funct.h"
#include "ope-report_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

Widget ope_information;

void ope_information_create(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     XtSetArg(args[n], XmNdialogTitle, XmStringCreateLtoR("Information", XmSTRING_DEFAULT_CHARSET)); n++;
     ope_information = XmCreateInformationDialog(parent, "ope_information", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(ope_information, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(ope_information, XmDIALOG_CANCEL_BUTTON));
}

void ope_information_report(char *message)
{
     Arg args[1];
     XmString xmres;

     XtSetArg(args[0], XmNmessageString, xmres = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(ope_information, args, 1);
     XmStringFree(xmres);
     XtManageChild(ope_information);
}

Widget user_error;

void ope_create_user_error(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     user_error = XmCreateErrorDialog(parent, "user_error", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(user_error, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(user_error, XmDIALOG_CANCEL_BUTTON));
}

void report_user_error(PString message)
{
     XmString to_free;
     Arg args[1];

     XtSetArg(args[0], XmNmessageString, to_free = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(user_error, args, 1);
     XmStringFree(to_free);
     XtManageChild(user_error);
}

Widget syntax_error;

void create_syntax_error_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     syntax_error = XmCreateErrorDialog(parent, "syntax_error", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(syntax_error, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(syntax_error, XmDIALOG_CANCEL_BUTTON));
}

void report_syntax_error(PString message)
{
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
}
