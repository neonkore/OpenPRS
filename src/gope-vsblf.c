/*                               -*- Mode: C -*- 
 * ope-vsblf.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand, LAAS/CNRS
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


#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "op-structure.h"

#ifdef GTK
#include <gtk/gtk.h>
#include "xm2gtk.h"
#include "gope-graphic.h"
#include "gope-global.h"
#else
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include "ope-graphic.h"
#include "ope-global.h"
#endif

#include "ope-vsblf_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

#ifdef GTK
#include "xm2gtk_f.h"
#endif


Widget VisibleFieldsDialog, VisibleFieldsMenu, VFTitle, VFCtxt, VFCall, VFSet, VFProp, VFEffe, VFDocu, VFNone, VFAll, VFNoneNT;

void VFNoneChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
#ifdef GTK_IGNORE
     if (call_data->set) {
	  XmToggleButtonSetState(VFAll, False, False);
	  XmToggleButtonSetState(VFNoneNT, False, False);
	  XmToggleButtonSetState(VFTitle, False, False);
	  XmToggleButtonSetState(VFCtxt, False, False);
	  XmToggleButtonSetState(VFCall, False, False);
	  XmToggleButtonSetState(VFSet, False, False);
	  XmToggleButtonSetState(VFProp, False, False);
	  XmToggleButtonSetState(VFEffe, False, False);
	  XmToggleButtonSetState(VFDocu, False, False);
     }
#endif
}

void VFAllChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
#ifdef GTK_IGNORE
     if (call_data->set) {
	  XmToggleButtonSetState(VFNone, False, False);
	  XmToggleButtonSetState(VFNoneNT, False, False);
	  XmToggleButtonSetState(VFTitle, True, False);
	  XmToggleButtonSetState(VFCtxt, True, False);
	  XmToggleButtonSetState(VFCall, True, False);
	  XmToggleButtonSetState(VFSet, True, False);
	  XmToggleButtonSetState(VFProp, True, False);
	  XmToggleButtonSetState(VFEffe, True, False);
	  XmToggleButtonSetState(VFDocu, True, False);
     }
#endif
}

void VFNoneNTChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
#ifdef GTK_IGNORE
     if (call_data->set) {
	  XmToggleButtonSetState(VFAll, False, False);
	  XmToggleButtonSetState(VFNone, False, False);
	  XmToggleButtonSetState(VFTitle, True, False);
	  XmToggleButtonSetState(VFCtxt, False, False);
	  XmToggleButtonSetState(VFCall, False, False);
	  XmToggleButtonSetState(VFSet, False, False);
	  XmToggleButtonSetState(VFProp, False, False);
	  XmToggleButtonSetState(VFEffe, False, False);
	  XmToggleButtonSetState(VFDocu, False, False);
     }
#endif
}

void updateVisibleFieldsDialog(Draw_Data *dd)
{
#ifdef GTK_IGNORE
     Op_Structure *op = dd->op;

     XtSetSensitive(VFTitle, True);
     XmToggleButtonSetState(VFTitle,op->op_title->u.gtext->visible,False);

     if (EMPTY_STRING(op->gcontext->u.gtext->string)) {
	  XtSetSensitive(VFCtxt, True);
	  XmToggleButtonSetState(VFCtxt,op->gcontext->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFCtxt,False);
     }

     if (op->gcall && EMPTY_STRING(op->gcall->u.gtext->string)) {
	  XtSetSensitive(VFCall, True);
	  XmToggleButtonSetState(VFCall,op->gcall->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFCall,False);
     }

     if (op->gsetting && EMPTY_STRING(op->gsetting->u.gtext->string)) {
	  XtSetSensitive(VFSet, True);
	  XmToggleButtonSetState(VFSet,op->gsetting->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFSet,False);
     }

     if  (op->geffects && (EMPTY_STRING(op->geffects->u.gtext->string) ||
			   (strcmp(op->geffects->u.gtext->string,"()") == 0))) {
	  XtSetSensitive(VFEffe, True);
	  XmToggleButtonSetState(VFEffe,op->geffects->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFEffe,False);
     }
	  
     if  (((EMPTY_STRING(op->gproperties->u.gtext->string)) ||
	   (strcmp(op->gproperties->u.gtext->string,"()") == 0))) {
	  XtSetSensitive(VFProp, True);
	  XmToggleButtonSetState(VFProp,op->gproperties->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFProp,False);
     }
	  
     if  (((EMPTY_STRING(op->gdocumentation->u.gtext->string)) ||
	   (strcmp(op->gdocumentation->u.gtext->string,"\"\"") == 0))) {
	  XtSetSensitive(VFDocu, True);
	  XmToggleButtonSetState(VFDocu,op->gdocumentation->u.gtext->visible,False);
     } else {
	  XtSetSensitive(VFDocu,False);
     }

     XmToggleButtonSetState(VFNone,False,False);
     XmToggleButtonSetState(VFNoneNT,False,False);
     XmToggleButtonSetState(VFAll,False,False);
#endif
}

void updateVisibleFieldsDialogIfManaged(Draw_Data *dd)
{
#ifdef IGNORE_GTK
     if (XtIsManaged(VisibleFieldsDialog))
	  updateVisibleFieldsDialog(dd);
#endif
}

void updateVisibleFieldsSensitivityIfManaged(PBoolean sensible)
{
#ifdef IGNORE_GTK
     if (XtIsManaged(VisibleFieldsDialog)) {
	  XtSetSensitive(XmSelectionBoxGetChild(VisibleFieldsDialog, XmDIALOG_OK_BUTTON), sensible);
	  XtSetSensitive(XmSelectionBoxGetChild(VisibleFieldsDialog, XmDIALOG_APPLY_BUTTON), sensible);
     }
#endif
}

void update_list_ogs(Op_Structure *op, PBoolean prev, PBoolean now, OG *og)
{
     if (prev && ! now) {
	  sl_delete_slist_node(op->list_movable_og, og);
	  sl_delete_slist_node(op->list_editable_og, og);
     } else if (! prev && now) {
	  sl_add_to_head(op->list_movable_og, og);
	  sl_add_to_head(op->list_editable_og, og);
     }
}

void updateVisibleFields(Draw_Data *dd)
{
     Op_Structure *op = dd->op;
     PBoolean prev;

     if (XtIsSensitive(VFTitle)) {
	  op->op_title->u.gtext->visible = XmToggleButtonGetState(VFTitle);
     }

     if (XtIsSensitive(VFCtxt)) {
	  prev = op->gcontext->u.gtext->visible;
	  op->gcontext->u.gtext->visible = XmToggleButtonGetState(VFCtxt);
	  update_list_ogs(op, prev, op->gcontext->u.gtext->visible, op->gcontext);
     }

     if (XtIsSensitive(VFCall)) {
	  prev = op->gcall->u.gtext->visible;
	  op->gcall->u.gtext->visible = XmToggleButtonGetState(VFCall);
	  update_list_ogs(op, prev, op->gcall->u.gtext->visible, op->gcall);
     }
     
     if (XtIsSensitive(VFSet)) {
	  prev = op->gsetting->u.gtext->visible;
	  op->gsetting->u.gtext->visible = XmToggleButtonGetState(VFSet);
	  update_list_ogs(op, prev, op->gsetting->u.gtext->visible, op->gsetting);
     }

     if (XtIsSensitive(VFProp)) {
	  prev = op->gproperties->u.gtext->visible;
	  op->gproperties->u.gtext->visible = XmToggleButtonGetState(VFProp);
	  update_list_ogs(op, prev, op->gproperties->u.gtext->visible, op->gproperties);
     }
     
     if (XtIsSensitive(VFEffe)) {
	  prev = op->geffects->u.gtext->visible;
	  op->geffects->u.gtext->visible = XmToggleButtonGetState(VFEffe);
	  update_list_ogs(op, prev, op->geffects->u.gtext->visible, op->geffects);
     }
     
     if (XtIsSensitive(VFDocu)) {
	  prev = op->gdocumentation->u.gtext->visible;
	  op->gdocumentation->u.gtext->visible = XmToggleButtonGetState(VFDocu);
	  update_list_ogs(op, prev, op->gdocumentation->u.gtext->visible, op->gdocumentation);
     }
     report_opfile_modification();
}

void VisibleFieldsDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef GTK_IGNORE
     updateVisibleFields(dd);
     XClearArea(XtDisplay(dd->canvas), dd->window, 0, 0, 0, 0, True);
#endif
}

void ope_create_vf_dialog(Widget parent, Draw_Data *dd)
{
#ifdef GTK_IGNORE
     Cardinal n;
     Arg args[MAXARGS];

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;
     VisibleFieldsDialog = XmCreatePromptDialog(parent, "VisibleFieldsDialog",args,n);
     XtManageChild(XmSelectionBoxGetChild(VisibleFieldsDialog, XmDIALOG_APPLY_BUTTON));
     XtAddCallback(VisibleFieldsDialog, XmNokCallback, (XtCallbackProc)VisibleFieldsDialogAccept, dd);
     XtAddCallback(VisibleFieldsDialog, XmNapplyCallback, (XtCallbackProc)VisibleFieldsDialogAccept, dd);
     XtAddCallback(VisibleFieldsDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Change Visible Fields"));

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(VisibleFieldsDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(VisibleFieldsDialog, XmDIALOG_TEXT));

     VisibleFieldsMenu = XmCreateRowColumn(VisibleFieldsDialog,"VisibleFieldsMenu",NULL,0);
     XtManageChild(VisibleFieldsMenu);

     VFTitle = XmCreateToggleButton(VisibleFieldsMenu,"VFTitle", NULL, 0);
     XtManageChild(VFTitle);
     VFCtxt = XmCreateToggleButton(VisibleFieldsMenu,"VFCtxt", NULL, 0);
     XtManageChild(VFCtxt);
     VFCall = XmCreateToggleButton(VisibleFieldsMenu,"VFCall", NULL, 0);
     XtManageChild(VFCall);
     VFSet = XmCreateToggleButton(VisibleFieldsMenu,"VFSet", NULL, 0);
     XtManageChild(VFSet);
     VFEffe = XmCreateToggleButton(VisibleFieldsMenu,"VFEffe", NULL, 0);
     XtManageChild(VFEffe);
     VFProp = XmCreateToggleButton(VisibleFieldsMenu,"VFProp", NULL, 0);
     XtManageChild(VFProp);
     VFDocu = XmCreateToggleButton(VisibleFieldsMenu,"VFDocu", NULL, 0);
     XtManageChild(VFDocu);
     
     XtManageChild(XmCreateSeparator(VisibleFieldsMenu, "VFseparator", NULL, 0));

     VFNone = XmCreateToggleButton(VisibleFieldsMenu,"VFNone", NULL, 0);
     VFNoneNT = XmCreateToggleButton(VisibleFieldsMenu,"VFNoneNT", NULL, 0);
     VFAll = XmCreateToggleButton(VisibleFieldsMenu,"VFAll", NULL, 0);
     XtAddCallback(VFNone, XmNvalueChangedCallback, (XtCallbackProc)VFNoneChanged, NULL);
     XtAddCallback(VFNoneNT, XmNvalueChangedCallback, (XtCallbackProc)VFNoneNTChanged, NULL);
     XtAddCallback(VFAll, XmNvalueChangedCallback, (XtCallbackProc)VFAllChanged, NULL);
     XtManageChild(VFNone);
     XtManageChild(VFNoneNT);
     XtManageChild(VFAll);
#endif
}
