/*                               -*- Mode: C -*- 
 * xps-textwin.c -- 
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

/* 
 * Standard Toolkit include files
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/*
 * Public include files for widgets used in this file.
 */
#include <Xm/Text.h>

#include "constant.h"

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "ope-graphic.h"
#include "xoprs-main.h"
#include "xoprs-textwin.h"
#include "xoprs-textwin_f.h"



static XmTextPosition  MaxTextSize = 200000; /* Max size (in characters) of the textwindow */
#define MinTextSize 2000 /* minimum size */
static XmTextPosition  ScrollTextSize = 50000; /* Number of characters to delete when we reach the maxsize */
#define CLEARMESSAGE LG_STR("The previous part of the text has been cleaned up\n","The previous part of the text has been cleaned up\n")

static PBoolean cleaning = FALSE;

XmTextPosition TextGetLastPos(Widget w)
{
	Arg	args[MAXARGS];
	XmTextPosition cursorPos;

	XtSetArg(args[0], XmNcursorPosition, &cursorPos);
	XtGetValues(w, args, 1);
	return(cursorPos);	
}

void AppendTextWindow(Widget textWindow, char *s,PBoolean big)
{
     XmTextPosition  StartPos;      	/* starting position of input text */
     XmTextPosition       lastPos;
     XmTextBlockRec        textblock;


    if (!s || (strcmp(s, "") == 0)) return;

    textblock.length   = strlen(s);
    textblock.ptr      = s;

    lastPos = XmTextGetLastPosition(textWindow);

    if (big) XtUnmanageChild(textWindow);
    XmTextInsert(textWindow, lastPos, textblock.ptr);
    if (big) XtManageChild(textWindow);
    StartPos = TextGetLastPos(textWindow);
    XmTextSetInsertionPosition(textWindow, 
			       XmTextGetLastPosition(textWindow));
}

void ClearTextWindow(Widget textWindow)
{
     XmTextSetString(textWindow,"");
     XmTextSetInsertionPosition(textWindow,0);
}

void VerifySizeTextWindow(Widget textWindow, XtPointer client_data, XtPointer call_data)
{
     XmTextPosition  StartPos;      	/* starting position of input text */
     if (!cleaning){ /* we must check if the callback as not be activated by itself */

	  StartPos = TextGetLastPos(textWindow);
	  if (StartPos > MaxTextSize){
	       String string;
	       Arg args[1];

	       cleaning = TRUE;
	       
	       XtSetArg(args[0], XmNvalue, &string); 
	       XtGetValues(textWindow,args,1);
	       
	       XtSetArg(args[0], XmNvalue, string + (int)strlen(string)/2); 
	       XtSetValues(textWindow,args,1);

	       XtFree(string);

	       StartPos = TextGetLastPos(textWindow);
	       cleaning = FALSE;
	  }
     }
}
	  
void setSizeTextWindow(int newsize)
{
     if (newsize < MinTextSize) 
	  MaxTextSize = MinTextSize;
     else
	  MaxTextSize = newsize;

     ScrollTextSize = MaxTextSize /2;
}

	  
int getSizeTextWindow()
{
     return (MaxTextSize);
}
