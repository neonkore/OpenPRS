static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * xoprs-filesel.c -- 
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>

#include "constant.h"
#include "xoprs.h"
#include "xhelp.h"
#include "ope-graphic.h"
#include "xoprs-main.h"
#include "parser-funct.h"

#include "xhelp_f.h"
#include "oprs-init_f.h"

Widget xp_includeFilesel, xp_loaddbFilesel, xp_loadopFilesel, xp_loadkrnFilesel, xp_savedbFilesel;
Widget xp_loadddbFilesel, xp_loaddopFilesel, xp_dumpdbFilesel, xp_dumpopFilesel, xp_dumpkrnFilesel;

void xp_includeFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_includeFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"include \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loaddbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loaddbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadopFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load opf \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     XtFree(selected_file);
     }
}

void xp_savedbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_savedbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"save db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpdbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpdbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpopFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump op  \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpkrnFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpkrnFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump kernel \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadddbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadddbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loaddopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loaddopFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump op \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadkrnFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadkrnFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump kernel \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_create_filesel(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;

     xp_includeFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_includeFilesel",  args, n);
     xp_loaddbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loaddbFilesel",  args, n);
     xp_loadopFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loadopFilesel",  args, n);
     xp_savedbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_savedbFilesel",  args, n);
     xp_loadddbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loadddbFilesel",  args, n);
     xp_loaddopFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loaddopFilesel",  args, n);
     xp_loadkrnFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loadkrnFilesel",  args, n);
     xp_dumpdbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpdbFilesel",  args, n);
     xp_dumpopFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpopFilesel",  args, n);
     xp_dumpkrnFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpkrnFilesel",  args, n);

     XtAddCallback(xp_includeFilesel, XmNokCallback, xp_includeFileselok, 0);
     XtAddCallback(xp_includeFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Include"));
     XtAddCallback(xp_loaddbFilesel, XmNokCallback, xp_loaddbFileselok, 0);
     XtAddCallback(xp_loaddbFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Load Database"));
     XtAddCallback(xp_loadopFilesel, XmNokCallback, xp_loadopFileselok, 0);
     XtAddCallback(xp_loadopFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Load OP"));

     XtAddCallback(xp_dumpdbFilesel, XmNokCallback, xp_dumpdbFileselok, 0);
     XtAddCallback(xp_dumpdbFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Dump Database"));
     XtAddCallback(xp_dumpopFilesel, XmNokCallback, xp_dumpopFileselok, 0);
     XtAddCallback(xp_dumpopFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Dump OP"));
     XtAddCallback(xp_dumpkrnFilesel, XmNokCallback, xp_dumpkrnFileselok, 0);
     XtAddCallback(xp_dumpkrnFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Dump Kernel"));

     XtAddCallback(xp_loadddbFilesel, XmNokCallback, xp_loadddbFileselok, 0);
     XtAddCallback(xp_loadddbFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Load Dump Database"));
     XtAddCallback(xp_loaddopFilesel, XmNokCallback, xp_loaddopFileselok, 0);
     XtAddCallback(xp_loaddopFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Load Dump OP"));

     XtAddCallback(xp_loadkrnFilesel, XmNokCallback, xp_loadkrnFileselok, 0);
     XtAddCallback(xp_loadkrnFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Load Dump Kernel"));

     XtAddCallback(xp_savedbFilesel, XmNokCallback, xp_savedbFileselok, 0);
     XtAddCallback(xp_savedbFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Save Database"));
}
