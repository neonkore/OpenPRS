static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * ope-print.c -- 
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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>

#include "xhelp.h"
#include "xhelp_f.h"

#include "ope-graphic.h"
#include "ope-global.h"
#include "constant.h"
#include "macro.h"

Widget printCommandDialog;
Widget printCommandDialogTxtFldGx;
Widget printCommandDialogTxtFldTxt;

String gx_print_command_template;
String txt_print_command_template;
Widget printQuestion;

int print_op_printer(Draw_Data *dd)
{
     char print_command[BUFSIZ];
     char message[BUFSIZ];

     sprintf(message,LG_STR("Printing OP %s. Please Wait.",
			    "Impression du OP %s. Patience."), current_op->name);
     TimeoutCursors(True, message);

     XFlush(XtDisplay(topLevel));
     XmUpdateDisplay(topLevel);
     sleep(1);
     XmUpdateDisplay(topLevel);

     if (dd->op->graphic) {
	  if (dump_op_sys(dd,file_name_for_print,
			  XWD
	       ) == -1) {
	       fprintf(stderr, LG_STR("dump_op_pxm problem.\n",
				      "Probleme de dump de OP.\n"));
	  } else {
     
	       XmUpdateDisplay(topLevel);

	       sprintf(print_command, gx_print_command_template, file_name_for_print);

	       XmUpdateDisplay(topLevel);

	       system(print_command);
	  }
     } else {
	  FILE *fpo;
	  PBoolean save_pv = print_var_name;
	  PBoolean save_peo = print_english_operator ;

	  if ((fpo = (FILE *) fopen(file_name_for_print, "w")) == NULL) {
	       fprintf(stderr, LG_STR("write_opfile: failed fdopen %s\n",
				      "write_opfile: failed fdopen %s\n"), file_name_for_print);
	       return;
	  }
	  XmUpdateDisplay(topLevel);

	  sprintf(print_command, txt_print_command_template, file_name_for_print);

	  print_var_name = TRUE;
	  print_english_operator = FALSE;

	  write_top(fpo,dd->op);
	  

	  fclose(fpo);
	  
	  print_var_name = save_pv;
	  print_english_operator = save_peo;

	  XmUpdateDisplay(topLevel);
	  
	  system(print_command);
     }

     if (unlink(file_name_for_print) == -1)
	  perror("unlink");
     
     TimeoutCursors(False, NULL);
}

int dump_op_pxm(Draw_Data *dd, PString file)
{
     char message[BUFSIZ];

     sprintf(message,LG_STR("Dumping OP %s pixmap. Please Wait.",
			    "Dumping OP %s pixmap. Please Wait."), current_op->name);
     TimeoutCursors(True, message);

     XFlush(XtDisplay(topLevel));
     XmUpdateDisplay(topLevel);
     sleep(1);
     XmUpdateDisplay(topLevel);
     

     if (dump_op_sys(dd,file,XPM) == -1)
	  fprintf(stderr, LG_STR("dump_op_pxm problem.\n",
				 "dump_op_pxm problem.\n"));

     TimeoutCursors(False, NULL);
}

void find_bounding_box(Draw_Data *dd, int *width,  int *height)
{
     if (dd->op) {
	  OG *og, *og2;

	  sl_loop_through_slist(dd->op->list_og_edge, og, OG *) {
	       *width = MAX (*width, og->x + (int)og->width);
	       *height = MAX (*height, og->y + (int)og->height);
	       sl_loop_through_slist(og->u.gedge->list_knot, og2, OG *) {
		    *width = MAX (*width, og2->x + (int)og2->width);
		    *height = MAX (*height, og2->y + (int)og2->height);
	       }
	  }

	  sl_loop_through_slist(dd->op->list_og_node, og, OG *) {
	       *width = MAX (*width, og->x + (int)og->width);
	       *height = MAX (*height, og->y + (int)og->height);
	  }

	  sl_loop_through_slist(dd->op->list_og_edge_text, og, OG *) {
	       *width = MAX (*width, og->x + (int)og->width);
	       *height = MAX (*height, og->y + (int)og->height);
	  }

	  sl_loop_through_slist(dd->op->list_og_text, og, OG *) {
	       if (og->u.gtext->visible) {
		    *width = MAX (*width, og->x + (int)og->width);
		    *height = MAX (*height, og->y + (int)og->height);
	       }
	  }
	  og =  dd->op->op_title;
	  if (og->u.gtext->visible) {
	       *width = MAX (*width, og->x + (int)og->width);
	       *height = MAX (*height, og->y + (int)og->height);
	  }
     }
}

int dump_op_sys(Draw_Data *dd, PString file, DumpFormat format)
{
    Pixmap pm;
    int width = 1;
    int height = 1 ;
    int x = dd->left;
    int y = dd->top;

    int res;

    find_bounding_box(dd, &width, &height);

    pm = XCreatePixmap(XtDisplay(dd->canvas),
		       dd->window, width, height,
		       DefaultDepthOfScreen(XtScreen(dd->canvas)));

    dd->window = pm;
    dd->left = 0;
    dd->top = 0;
    /*    XClearWindow(XtDisplay(dd->canvas), dd->window); cannot do that in a pixmap... */
    redraw_all_in_pixmap(dd->canvas, dd, width, height);
    dd->window = XtWindow(dd->canvas);
    dd->left = x;
    dd->top = y;
    
    if (format == XWD) {
	 res = WritePixmap(dd->canvas, file, width, height, pm);

	 XFreePixmap(XtDisplay(dd->canvas),pm);
	 return res;
	 
    } else if  (format == XPM) {
	 res = XWriteBitmapFile(XtDisplay(dd->canvas), file, pm, width, height, -1, -1);

	 XFreePixmap(XtDisplay(dd->canvas),pm);

	 if (res != BitmapSuccess) {
	      return -1;
	 } else {
	      return 1;
	 }
    } else {
	 fprintf (stderr, LG_STR("dump_op_sys:  unknown format.\n",
				 "dump_op_sys:  unknown format.\n"));
	 XFreePixmap(XtDisplay(dd->canvas),pm);
	 return -1;
    }
}

void printCommandDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     gx_print_command_template = XmTextGetString(printCommandDialogTxtFldGx);
     txt_print_command_template = XmTextGetString(printCommandDialogTxtFldTxt);
     return;
}

void ReallyPrint(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Op_Structure *op;
     Op_Structure *old_selected_op;


     old_selected_op = current_op;

     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *) {
	  select_op(op, dd);
	  print_op_printer(dd);
     }
     
     select_op(old_selected_op, dd);
}

void printQuestionManage()
{
     Arg args[1];
     char message[BUFSIZ];

     sprintf(message, LG_STR("Do you really want to print all the OPs of the OP file:\n%s,\nwith the command:\n%s (for Graphic OP)\nand:\n%s (for Text OP).",
			     "Do you really want to print all the OPs of the OP file:\n%s,\nwith the command:\n%s (for Graphic OP)\nand:\n%s (for Text OP)."),
	     current_opfile->name, gx_print_command_template, txt_print_command_template);

     XtSetArg(args[0], XmNmessageString, XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(printQuestion, args, 1);
     XtManageChild(printQuestion);
}

void OpPrint(Widget w,  Draw_Data *dd, XtPointer call_data)
{
     if (! current_op) {
	  report_user_error(LG_STR("You have to select a OP, before using this menu item.",
				   "You have to select a OP, before using this menu item."));
	  return;
     }

     print_op_printer(dd);
}

void MiscPrint(Widget w, Draw_Data *dd, XtPointer call_data)
{   
     XmTextSetString(printCommandDialogTxtFldGx, gx_print_command_template);
     XmTextSetString(printCommandDialogTxtFldTxt, txt_print_command_template);
     XtManageChild(printCommandDialog);
     return;
}


void ope_create_print_command_dialog(Widget parent, Draw_Data *dd)
{
     Cardinal n;
     Arg args[MAXARGS];
     Widget printCommandWA;

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;
     printCommandDialog= XmCreatePromptDialog(parent, "printCommandDialog",args,n);
     XtAddCallback(printCommandDialog, XmNokCallback, (XtCallbackProc)printCommandDialogAccept, dd);
     XtAddCallback(printCommandDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Change Print Command"));

     XtUnmanageChild(XmSelectionBoxGetChild(printCommandDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(printCommandDialog, XmDIALOG_TEXT));

     printCommandWA= XmCreateWorkArea(printCommandDialog, "printCommandWA",args,n);
     XtManageChild(printCommandWA);

     XtManageChild(XmCreateLabel(printCommandWA,"printCommandDialogTxtLblTxt",NULL, 0));
     printCommandDialogTxtFldTxt = XmCreateTextField(printCommandWA,"printCommandDialogTxtFldTxt",NULL, 0);
     XtManageChild(printCommandDialogTxtFldTxt);
     XtManageChild(XmCreateLabel(printCommandWA,"printCommandDialogTxtLblGx",NULL, 0));
     printCommandDialogTxtFldGx = XmCreateTextField(printCommandWA,"printCommandDialogTxtFldGx",NULL, 0);
     XtManageChild(printCommandDialogTxtFldGx);

}
