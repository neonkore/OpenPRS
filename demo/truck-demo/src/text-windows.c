/*                               -*- Mode: C -*- 
 * text-windows.c -- 
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
#include "constant.h"
#include "demo.h"

#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawnB.h>


Widget messageWindowFrame, messageTextWindow;
Widget warningWindowFrame, warningTextWindow;
Widget oprsWindowFrame, oprsTextWindow;

void clear_messageWindow(void);
void clear_warningWindow(void);
void clear_oprsWindow(void);

void make_text_windows(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     Widget messageWindowForm, messageWindowLabel, messageWindow;
     Widget messageWindow_Menu, messageWindow_clearDButton;
     Widget warningWindowForm, warningWindowLabel, warningWindow;
     Widget warningWindow_Menu, warningWindow_clearDButton;
     Widget oprsWindowForm, oprsWindowLabel, oprsWindow;
     Widget oprsWindow_Menu, oprsWindow_clearDButton;

/*
 * Message Window
 */

     n = 0;
     messageWindowFrame =  XmCreateFrame(parent, "messageWindowFrame", args, n);
     XtManageChild(messageWindowFrame);
     n = 0;
     messageWindowForm =  XmCreateForm(messageWindowFrame, "messageWindowForm", args, n);
     XtManageChild(messageWindowForm);

     n = 0;
     messageTextWindow = XmCreateScrolledText (messageWindowForm, "messageTextWindow", args, n);
     XtManageChild (messageTextWindow);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     messageWindow_Menu = XmCreateWorkArea(messageWindowForm,"messageWindow_Menu",args,n);
     XtManageChild(messageWindow_Menu);

     n = 0;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     messageWindow_clearDButton = XmCreateDrawnButton(messageWindow_Menu,"messageWindow_clearDButton", args, n);
     XtAddCallback(messageWindow_clearDButton, XmNactivateCallback, (XtCallbackProc)clear_messageWindow, 0);
     XtManageChild(messageWindow_clearDButton);

     messageWindow = XtParent(messageTextWindow);
     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, messageWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(messageWindow , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, messageWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, messageWindow); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(LG_STR("Message Window",
								 "Fenêtre Messages"), XmSTRING_DEFAULT_CHARSET)); n++;
     messageWindowLabel = XtCreateManagedWidget("messageWindowLabel", xmLabelWidgetClass,messageWindowForm, args, n);

/*
 * Warning Window
 */
     n = 0;
     warningWindowFrame =  XmCreateFrame(parent, "warningWindowFrame", args, n);
     XtManageChild(warningWindowFrame);
     n = 0;
     warningWindowForm =  XmCreateForm(warningWindowFrame, "warningWindowForm", args, n);
     XtManageChild(warningWindowForm);

     n = 0;
     warningTextWindow = XmCreateScrolledText (warningWindowForm, "warningTextWindow", args, n);
     XtManageChild (warningTextWindow);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     warningWindow_Menu = XmCreateWorkArea(warningWindowForm,"warningWindow_Menu",args,n);
     XtManageChild(warningWindow_Menu);

     n = 0;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     warningWindow_clearDButton = XmCreateDrawnButton(warningWindow_Menu,"warningWindow_clearDButton", args, n);
     XtAddCallback(warningWindow_clearDButton, XmNactivateCallback, (XtCallbackProc)clear_warningWindow, 0);
     XtManageChild(warningWindow_clearDButton);

     warningWindow = XtParent(warningTextWindow);
     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, warningWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(warningWindow , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, warningWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, warningWindow); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(LG_STR("Warning Window",
								 "Fenêtre Problèmes"), XmSTRING_DEFAULT_CHARSET)); n++;
     warningWindowLabel = XtCreateManagedWidget("warningWindowLabel", xmLabelWidgetClass,warningWindowForm, args, n);

/*
 * Oprs Messages Window
 */
     n = 0;
     oprsWindowFrame =  XmCreateFrame(parent, "oprsWindowFrame", args, n);
     XtManageChild(oprsWindowFrame);

     n = 0;
     oprsWindowForm =  XmCreateForm(oprsWindowFrame, "oprsWindowForm", args, n);
     XtManageChild(oprsWindowForm);

     n = 0;
     oprsTextWindow = XmCreateScrolledText (oprsWindowForm, "oprsTextWindow", args, n);
     XtManageChild (oprsTextWindow);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     oprsWindow_Menu = XmCreateWorkArea(oprsWindowForm,"oprsWindow_Menu",args,n);
     XtManageChild(oprsWindow_Menu);

     n = 0;
     XtSetArg(args[n], XmNisAligned, TRUE); n++;
     XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
     oprsWindow_clearDButton = XmCreateDrawnButton(oprsWindow_Menu,"oprsWindow_clearDButton", args, n);
     XtAddCallback(oprsWindow_clearDButton, XmNactivateCallback, (XtCallbackProc)clear_oprsWindow, 0);
     XtManageChild(oprsWindow_clearDButton);

     oprsWindow = XtParent(oprsTextWindow);
     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, oprsWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetValues(oprsWindow , args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNrightWidget, oprsWindow_Menu); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, oprsWindow); n++;
     XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
     XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR(LG_STR("Oprs Window",
								 "Fenêtre OPRS"), XmSTRING_DEFAULT_CHARSET)); n++;
     oprsWindowLabel = XtCreateManagedWidget("oprsWindowLabel", xmLabelWidgetClass,oprsWindowForm, args, n);

}

void appendTextWindow(Widget textWindow, char *s)
{
     XmTextPosition       lastPos;
     XmTextBlockRec        textblock;


     if (!s || !strcmp(s, "")) return;
     
     textblock.length   = strlen(s);
     textblock.ptr      = s;

     lastPos = XmTextGetLastPosition(textWindow);

     XmTextInsert(textWindow, lastPos, textblock.ptr);
     XmTextSetInsertionPosition(textWindow, 
				XmTextGetLastPosition(textWindow));
}

void clear_messageWindow()
{
     XmTextSetString(messageTextWindow,"");
     XmTextSetInsertionPosition(messageTextWindow,0);

}

void clear_warningWindow()
{
     XmTextSetString(warningTextWindow,"");
     XmTextSetInsertionPosition(warningTextWindow,0);

}

void clear_oprsWindow()
{
     XmTextSetString(oprsTextWindow,"");
     XmTextSetInsertionPosition(oprsTextWindow,0);

}

void clear_all_textWindows()
{
     clear_messageWindow();
     clear_warningWindow();
     clear_oprsWindow();
}
