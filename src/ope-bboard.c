
/*                               -*- Mode: C -*-
 * ope-bboard.c --
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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/BulletinB.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushB.h>

#include <Xm/Separator.h>

#include "macro.h"
#include "constant.h"
#include "opaque.h"

#include "oprs-type.h"
#include "ope-graphic.h"
#include "ope-global.h"
#include "ope-syntax.h"
#include "relevant-op.h"

#include "relevant-op_f.h"
#include "ope-bboard_f.h"
#include "ope-graphic_f.h"
#include "ope-edit_f.h"
#include "ope-syntax_f.h"
#include "op-structure_f.h"
#include "ope-external_f.h"
#include "ope-vsblf_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

CreateOPBBoardStruct *createOPBBoardStruct;
CreateEdgeStruct *createEdgeStruct;
EditObjectStruct *editObjectStruct;
Widget cbb_body_radiobutton, cbb_graph_radiobutton, cbb_action_radiobutton, cbb_act_label;

void update_body_action_string(PString string, Widget act_bb)
{
     XmString xmstring;
     Arg args[1];
     
     if (!XtIsManaged(act_bb))
	  XtManageChild(act_bb);
     XtSetArg(args[0], XmNlabelString, xmstring = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(cbb_act_label, args, 1);
     XmStringFree(xmstring);
}

void action_button_changed(Widget w, CreateOPBBoardStruct * cOPBBoard, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_body_action_string("Action:", cOPBBoard->act_bb);
}

void body_button_changed(Widget w, CreateOPBBoardStruct * cOPBBoard, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_body_action_string("Body:", cOPBBoard->act_bb);
}

void graph_button_changed(Widget w, CreateOPBBoardStruct * cOPBBoard, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set && XtIsManaged(cOPBBoard->act_bb))
	  XtUnmanageChild(cOPBBoard->act_bb);
}

void Cbb_Create(Widget w, CreateOPBBoardStruct * cOPBBoard, XtPointer call_data)
{
     PString sprop, seff, sact, sdoc, sset, scall, sctxt, sip, sname;
     PString *prop, *eff, *act, *doc, *set, *call, *ctxt, *ip, *name;
     Draw_Data *dd;
     Arg arg[1];
     PBoolean res;
     Op_Structure *op;
     PBoolean save_cr;

     prop = &sprop;
     eff = &seff;
     act = &sact;
     doc = &sdoc;
     set = &sset;
     call = &scall;
     ctxt = &sctxt;
     ip = &sip;
     name = &sname;

     sname = XmTextGetString(cOPBBoard->cbb_name_field);
     if (!check_op_name(name)) {
	  return;
     }
     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	  if (strcmp(op->name, sname) == 0) {
	  report_syntax_error(LG_STR("This name is already used by another OP.",
				     "This name is already used by another OP."));
	  return;
     }
     sip = XmTextGetString(cOPBBoard->cbb_ip_field);
     pretty_width = ip_width;
     if (!check_ip(ip))
	  return;

     scall = XmTextGetString(cOPBBoard->cbb_call_field);
     pretty_width = call_width;
     if (!check_call(call))
	  return;

     sctxt = XmTextGetString(cOPBBoard->cbb_ctxt_field);
     pretty_width = ctxt_width;
     if (!check_ctxt(ctxt))
	  return;

     sset = XmTextGetString(cOPBBoard->cbb_set_field);
     pretty_width = set_width;
     if (!check_set(set))
	  return;

     sdoc = XmTextGetString(cOPBBoard->cbb_doc_field);
     pretty_width = doc_width;
	       save_cr = replace_cr;
	       replace_cr = FALSE;
     if (!check_doc(doc)) {
	  replace_cr = save_cr;
	  return;
     }
     replace_cr = save_cr;

     seff = XmTextGetString(cOPBBoard->cbb_eff_field);
     pretty_width = eff_width;
     if (!check_eff(eff))
	  return;

     sprop = XmTextGetString(cOPBBoard->cbb_prop_field);
     pretty_width = prop_width;
     if (!check_prop(prop))
	  return;

     if (!XmToggleButtonGetState(cbb_graph_radiobutton))
	  if (XmToggleButtonGetState(cbb_body_radiobutton)) {
	       sact = XmTextGetString(cOPBBoard->cbb_act_field);
	       pretty_width = bd_width;
	       current_body_indent = 0;
	       current_body_line = 0;
	       current_list_og_inst = sl_make_slist();
	       res = check_body(act); 
	       if (!res) {
		    free_list_og_inst(current_list_og_inst);
		    return;
	       }
	  } else {
	       sact = XmTextGetString(cOPBBoard->cbb_act_field);
	       pretty_width = act_width;
	       if (!check_act(act)) return;
	  }
     
     XtSetArg(arg[0], XmNuserData, &dd);
     XtGetValues(cOPBBoard->createOPBBoard, arg, 1);
     XtUnmanageChild(cOPBBoard->createOPBBoard);

     op = make_op();
     op->name = *name;
     NEWSTR(current_opfile->name,op->file_name);
     sl_add_to_tail(current_opfile->list_op,op);
     sl_sort_slist_func(current_opfile->list_op,sort_op);
     update_empty_sensitivity(True);

     /* Do not used this function : it call some function (like updateVisibleFieldsDialogIfManaged)
      * which needs an already built OP.
      *
      * select_op(op, dd);
      */
     unselect_current_op(dd, TRUE);
     current_op = op;
     dd->op = op;

     XClearArea(XtDisplay(dd->canvas), dd->window, 0, 0, 0, 0, True);
     set_canvas_view(dd, op->last_view_x, op->last_view_y);

     UpdateTitleWindow();
     update_last_selected_list(current_opfile->name, current_op->name);

     current_op->op_title = make_op_title(dd, *name);
     draw_og(dd->canvas, dd, current_op->op_title);

     current_op->ginvocation = create_text(dd->canvas, ip_x, ip_y, dd, TT_INVOCATION, *ip, ip_width,FALSE);
     current_op->invocation = parsed_expr;
     current_op->gcall = create_text(dd->canvas, call_x, call_y, dd, TT_CALL, *call, call_width,FALSE);
     current_op->call = parsed_expr;
     current_op->gcontext = create_text(dd->canvas, ctxt_x, ctxt_y, dd, TT_CONTEXT, *ctxt, ctxt_width,FALSE);
     current_op->gsetting = create_text(dd->canvas, set_x, set_y, dd, TT_SETTING, *set, set_width,FALSE);
     current_op->gdocumentation = create_text(dd->canvas, doc_x, doc_y, dd, TT_DOCUMENTATION, *doc, doc_width,FALSE);
     current_op->geffects = create_text(dd->canvas, eff_x, eff_y, dd, TT_EFFECTS, *eff, eff_width,FALSE);
     current_op->gproperties = create_text(dd->canvas, prop_x, prop_y, dd, TT_PROPERTIES, *prop, prop_width,FALSE);

     if (XmToggleButtonGetState(cbb_graph_radiobutton)) {
	  make_start_node(dd->canvas, dd);
     } else 
	  if (XmToggleButtonGetState(cbb_body_radiobutton)) {
	       current_op->list_og_inst = current_list_og_inst;
	       current_op->graphic = FALSE;
	       current_op->body = parsed_body;
	       current_op->gbody = create_text(dd->canvas, bd_x, bd_y, dd, TT_BODY, *act, bd_width,FALSE);
	       update_list_og_inst(dd, current_op, current_op->gbody);

	  } else {
	       current_op->action = parsed_action;
	       current_op->gaction = create_text(dd->canvas, act_x, act_y, dd, TT_ACTION, *act, act_width,FALSE);
	  }
     
     add_op_to_relevant_op_internal(current_op,relevant_op);

     report_opfile_modification();

     /* Now we can do it ! */

     update_select_sensitivity(True);

     updateVisibleFieldsDialogIfManaged(dd);
}

void Cbb_Cancel(Widget w, Widget bb, XtPointer call_data)
{
     XtUnmanageChild(bb);
}

void make_create_bboard(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     Widget cbb_name_label, cbb_ip_label, cbb_act_choice_label, cbb_ctxt_label,
            cbb_set_label, cbb_doc_label, cbb_eff_label, cbb_prop_label, cbb_call_label; 
     Widget cbb_name_field, cbb_ip_field, cbb_ctxt_field, cbb_call_field, cbb_set_field,
            cbb_doc_field, cbb_act_field, cbb_eff_field, cbb_prop_field;
     Widget cbb_ip_bb, cbb_ctxt_bb, cbb_call_bb, cbb_set_bb, cbb_doc_bb, cbb_act_bb, cbb_act_choice_bb,
            cbb_eff_bb, cbb_prop_bb;
     Widget cbb_act_radiobox;
     Widget cbb_menu, cbb_create, cbb_cancel;
     Widget createOPBBoard;

     createOPBBoardStruct = MAKE_OBJECT(CreateOPBBoardStruct);

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL);
     n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     createOPBBoard = XmCreateBulletinBoardDialog(parent, "createOPBBoard", args, n);
     createOPBBoardStruct->createOPBBoard = createOPBBoard;

     n = 0;
     cbb_name_label = XmCreateLabel(createOPBBoard, "cbb_name_label", args, n);
     XtManageChild(cbb_name_label);

     cbb_name_field = XmCreateTextField(createOPBBoard, "cbb_name_field", args, n);
     XtManageChild(cbb_name_field);
     createOPBBoardStruct->cbb_name_field = cbb_name_field;

     cbb_ip_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_ip_bb", args, n);
     XtManageChild(cbb_ip_bb);
     cbb_call_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_call_bb", args, n);
     XtManageChild(cbb_call_bb);
     cbb_ctxt_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_ctxt_bb", args, n);
     XtManageChild(cbb_ctxt_bb);
     cbb_set_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_set_bb", args, n);
     XtManageChild(cbb_set_bb);
     cbb_doc_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_doc_bb", args, n);
     XtManageChild(cbb_doc_bb);
     cbb_act_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_act_bb", args, n);
     XtManageChild(cbb_act_bb);

     createOPBBoardStruct->act_bb = cbb_act_bb;

     cbb_act_choice_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_act_choice_bb", args, n);
     XtManageChild(cbb_act_choice_bb);
     cbb_eff_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_eff_bb", args, n);
     XtManageChild(cbb_eff_bb);
     cbb_prop_bb = XmCreateBulletinBoard(createOPBBoard, "cbb_prop_bb", args, n);
     XtManageChild(cbb_prop_bb);

     cbb_act_choice_label = XmCreateLabel(cbb_act_choice_bb, "cbb_act_choice_label", args, n);
     XtManageChild(cbb_act_choice_label);
     cbb_ip_label = XmCreateLabel(cbb_ip_bb, "cbb_ip_label", args, n);
     XtManageChild(cbb_ip_label);
     cbb_call_label = XmCreateLabel(cbb_call_bb, "cbb_call_label", args, n);
     XtManageChild(cbb_call_label);
     cbb_ctxt_label = XmCreateLabel(cbb_ctxt_bb, "cbb_ctxt_label", args, n);
     XtManageChild(cbb_ctxt_label);
     cbb_set_label = XmCreateLabel(cbb_set_bb, "cbb_set_label", args, n);
     XtManageChild(cbb_set_label);
     cbb_doc_label = XmCreateLabel(cbb_doc_bb, "cbb_doc_label", args, n);
     XtManageChild(cbb_doc_label);
     cbb_act_label = XmCreateLabel(cbb_act_bb, "cbb_act_label", args, n);
     XtManageChild(cbb_act_label);
     cbb_eff_label = XmCreateLabel(cbb_eff_bb, "cbb_eff_label", args, n);
     XtManageChild(cbb_eff_label);
     cbb_prop_label = XmCreateLabel(cbb_prop_bb, "cbb_prop_label", args, n);
     XtManageChild(cbb_prop_label);

     n = 0;
     XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, True); n++;
     cbb_ip_field = XmCreateScrolledText(cbb_ip_bb, "cbb_ip_field", args, n);
     XtManageChild(cbb_ip_field);
     createOPBBoardStruct->cbb_ip_field = cbb_ip_field;

     cbb_call_field = XmCreateScrolledText(cbb_call_bb, "cbb_call_field", args, n);
     XtManageChild(cbb_call_field);
     createOPBBoardStruct->cbb_call_field = cbb_call_field;

     cbb_ctxt_field = XmCreateScrolledText(cbb_ctxt_bb, "cbb_ctxt_field", args, n);
     XtManageChild(cbb_ctxt_field);
     createOPBBoardStruct->cbb_ctxt_field = cbb_ctxt_field;

     cbb_set_field = XmCreateScrolledText(cbb_set_bb, "cbb_set_field", args, n);
     XtManageChild(cbb_set_field);
     createOPBBoardStruct->cbb_set_field = cbb_set_field;

     cbb_doc_field = XmCreateScrolledText(cbb_doc_bb, "cbb_doc_field", args, n);
     XtManageChild(cbb_doc_field);
     createOPBBoardStruct->cbb_doc_field = cbb_doc_field;

     cbb_act_field = XmCreateScrolledText(cbb_act_bb, "cbb_act_field", args, n);
     XtManageChild(cbb_act_field);
     createOPBBoardStruct->cbb_act_field = cbb_act_field;

     cbb_eff_field = XmCreateScrolledText(cbb_eff_bb, "cbb_eff_field", args, n);
     XtManageChild(cbb_eff_field);
     createOPBBoardStruct->cbb_eff_field = cbb_eff_field;

     cbb_prop_field = XmCreateScrolledText(cbb_prop_bb, "cbb_prop_field", args, n);
     XtManageChild(cbb_prop_field);
     createOPBBoardStruct->cbb_prop_field = cbb_prop_field;


     n = 0;
     cbb_act_radiobox = XmCreateRadioBox(cbb_act_choice_bb, "cbb_act_radiobox", args, n);
     XtManageChild(cbb_act_radiobox);

     cbb_action_radiobutton = XmCreateToggleButton(cbb_act_radiobox, "cbb_act_radiobutton", args, n);
     XtManageChild(cbb_action_radiobutton);
     XtAddCallback(cbb_action_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)action_button_changed, createOPBBoardStruct);

     cbb_body_radiobutton = XmCreateToggleButton(cbb_act_radiobox, "cbb_body_radiobutton", args, n);
     XtManageChild(cbb_body_radiobutton);
     XtAddCallback(cbb_body_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)body_button_changed, createOPBBoardStruct);

     cbb_graph_radiobutton= XmCreateToggleButton(cbb_act_radiobox, "cbb_graph_radiobutton", args, n);
     XtManageChild(cbb_graph_radiobutton);
     XtAddCallback(cbb_graph_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)graph_button_changed, createOPBBoardStruct);
     XmToggleButtonSetState(cbb_graph_radiobutton, True, True);

     n = 0;
     cbb_menu = XmCreateRowColumn(createOPBBoard, "cbb_menu", args, n);
     XtManageChild(cbb_menu);

     n = 0;
     cbb_create = XmCreatePushButton(cbb_menu, "cbb_create", args, n);
     XtAddCallback(cbb_create, XmNactivateCallback, (XtCallbackProc)Cbb_Create, createOPBBoardStruct);
     XtManageChild(cbb_create);
     cbb_cancel = XmCreatePushButton(cbb_menu, "cbb_cancel", args, n);
     XtAddCallback(cbb_cancel, XmNactivateCallback, (XtCallbackProc)Cbb_Cancel, createOPBBoard);
     XtManageChild(cbb_cancel);
}

void Cebb_Create(Widget w, CreateEdgeStruct *cEdgeBBoard, XtPointer call_data)
{
     Arg arg[1];
     Draw_Data *dd;
     PString *edge;
     PString sedge;

     edge = &sedge;
     *edge = XmTextGetString(cEdgeBBoard->text);
     pretty_width = edge_width;
     if (!check_edge(edge))
	  return;

     report_opfile_modification();

     XtUnmanageChild(cEdgeBBoard->createEdgeForm);
     XtSetArg(arg[0], XmNuserData, &dd);
     XtGetValues(cEdgeBBoard->createEdgeForm, arg, 1);
     if (dd->second_node_selected->type ==  DT_IF_NODE) 
	  create_edge(dd->canvas, dd, dd->node_selected, dd->second_node_selected,
		      *edge, edge_width, FALSE, DT_EDGE, ET_IF, dd->list_knot, parsed_expr);
     else
	  create_edge(dd->canvas, dd, dd->node_selected, dd->second_node_selected,
		      *edge, edge_width, FALSE, DT_EDGE, ET_GOAL, dd->list_knot, parsed_expr);
     dd->node_selected = dd->second_node_selected = NULL;
}

void Cebb_Cancel(Widget w,  CreateEdgeStruct *cEdgeBBoard, XtPointer call_data)
{
     Arg arg[1];
     Draw_Data *dd;
     
     XtSetArg(arg[0], XmNuserData, &dd);
     XtGetValues(cEdgeBBoard->createEdgeForm, arg, 1);

     dd->node_selected = dd->second_node_selected = NULL;
     XtUnmanageChild(cEdgeBBoard->createEdgeForm);
}

void make_edge_bboard(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     Widget cebb_name_label;
     Widget cebb_text_field;
     Widget cebb_menu_help, cebb_syntax;
     Widget cebb_menu, cebb_create, cebb_cancel;
     Widget createEdgeForm;


     createEdgeStruct = MAKE_OBJECT(CreateEdgeStruct);

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     XtSetArg(args[n], XmNverticalSpacing, 5); n++;
     XtSetArg(args[n], XmNhorizontalSpacing, 5); n++;
     createEdgeForm = XmCreateFormDialog(parent, "createEdgeForm", args, n);

     createEdgeStruct->createEdgeForm = createEdgeForm;

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     cebb_name_label = XmCreateLabel(createEdgeForm, "cebb_name_label", args, n);
     XtManageChild(cebb_name_label);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     cebb_menu_help = XmCreateForm(createEdgeForm, "cebb_menu_help", args, n);
     XtManageChild(cebb_menu_help);

     n = 0;
     cebb_syntax = XmCreatePushButton(cebb_menu_help, "cebb_syntax", args, n);
     XtAddCallback(cebb_syntax, XmNactivateCallback, infoHelp, makeFileNode("oprs", "Syntax and Semantic Used in the OPRS Development Environment"));
     XtManageChild(cebb_syntax);


     n = 0;
     XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, True); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
/*
     XtSetArg(args[n], XmNtopWidget, cebb_name_label); n++;
*/
     XtSetArg(args[n], XmNtopWidget, cebb_menu_help); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 78); n++;
     cebb_text_field = XmCreateScrolledText(createEdgeForm, "cebb_text_field", args, n);
     XtManageChild(cebb_text_field);

     createEdgeStruct->text = cebb_text_field;

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 80); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtManageChild(XmCreateSeparator(createEdgeForm, "ceSeparator", args, n));

     n = 0;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 82); n++;
     XtSetArg(args[n], XmNfractionBase, 5); n++;
     cebb_menu = XmCreateForm(createEdgeForm, "cebb_menu", args, n);
     XtManageChild(cebb_menu);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 1); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 4); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 2); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 1); n++;
     cebb_create = XmCreatePushButton(cebb_menu, "cebb_create", args, n);
     XtAddCallback(cebb_create, XmNactivateCallback, (XtCallbackProc)Cebb_Create, createEdgeStruct);
     XtManageChild(cebb_create);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 1); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 4); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 4); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 3); n++;
     cebb_cancel = XmCreatePushButton(cebb_menu, "cebb_cancel", args, n);
     XtAddCallback(cebb_cancel, XmNactivateCallback, (XtCallbackProc)Cebb_Cancel, createEdgeStruct);
     XtManageChild(cebb_cancel);
}

void Eobb_Edit(Widget w, EditObjectStruct * eObjectBBoard, XtPointer call_data)
{
     Arg arg[1];
     Draw_Data *dd;
     PString *string;
     PString sstring, tmp_string;
     PBoolean text_modified = FALSE;
     PBoolean res;
     PBoolean save_cr;

     /* char s[LINSIZ]; */
     int new_width = 0;
     char *tmp_str;

     tmp_str = XmTextGetString(eObjectBBoard->width);
     /* sprintf(s,"%s\n", (tmp_str = XmTextGetString(eObjectBBoard->width))); */
     if (!sscanf ( tmp_str, "%d", &new_width )) 
	 fprintf ( stderr, LG_STR("The width must be an integer.\n",
				  "The width must be an integer.\n"));
     XtFree (tmp_str);

     fill_lines = XmToggleButtonGetState(eObjectBBoard->fill_true);

     sstring = XmTextGetString(eObjectBBoard->text);
     string = &sstring;
     tmp_string = sstring;

     XtSetArg(arg[0], XmNuserData, &dd);
     XtGetValues(eObjectBBoard->editObjectForm, arg, 1);

     switch (dd->edited_og->type) {
     case DT_EDGE_TEXT:
	  if (new_width <= 0)
	       pretty_width = edge_width;
	  else
	       pretty_width = new_width;
	  if (!check_edge(string))
	       return;
	  dd->edited_og->u.gedge_text->edge->u.gedge->edge->expr = parsed_expr;
	  break;
     case DT_TEXT:
	  text_modified = TRUE;
	  switch (dd->edited_og->u.gtext->text_type) {
	  case TT_INVOCATION:
	       if (new_width <= 0)
		    pretty_width = ip_width;
	       else
		    pretty_width = new_width;
	       if (!check_ip(string))
		    return;
	       delete_op_from_rop(current_op,relevant_op,FALSE,FALSE,FALSE);
	       current_op->invocation = parsed_expr;
	       add_op_to_relevant_op_internal(current_op,relevant_op);

	       break;
	  case TT_CALL:
	       if (new_width <= 0)
		    pretty_width = call_width;
	       else
		    pretty_width = new_width;
	       if (!check_call(string))
		    return;
	       break;
	  case TT_CONTEXT:
	       if (new_width <= 0)
		    pretty_width = ctxt_width;
	       else
		    pretty_width = new_width;
	       if (!check_ctxt(string))
		    return;
	       break;
	  case TT_SETTING:
	       if (new_width <= 0)
		    pretty_width = set_width;
	       else
		    pretty_width = new_width;
	       if (!check_set(string))
		    return;
	       break;
	  case TT_PROPERTIES:
	       if (new_width <= 0)
		    pretty_width = prop_width;
	       else
		    pretty_width = new_width;
	       if (!check_prop(string))
		    return;
	       break;
	  case TT_DOCUMENTATION:
	       save_cr = replace_cr;
	       replace_cr = FALSE;
	       if (new_width <= 0)
		    pretty_width = doc_width;
	       else
		    pretty_width = new_width;
	       if (!check_doc(string)) {
		    replace_cr = save_cr;
		    return;
	       }
	       replace_cr = save_cr;
	       break;
	  case TT_EFFECTS:
	       if (new_width <= 0)
		    pretty_width = eff_width;
	       else
		    pretty_width = new_width;
	       if (!check_eff(string))
		    return;
	       break;
	  case TT_ACTION:
	       if (new_width <= 0)
		    pretty_width = act_width;
	       else
		    pretty_width = new_width;
	       if (!check_act(string))
		    return;
	       break;
	  case TT_BODY:
	       current_list_og_inst = sl_make_slist();
	       if (new_width <= 0)
		    pretty_width = bd_width;
	       else
		    pretty_width = new_width;
	       current_body_indent = 0;
	       current_body_line = 0;
	       res = check_body(string);

	       if (!res) {
		    free_list_og_inst(current_list_og_inst);
		    return;
	       } else {
		    free_list_og_inst(current_op->list_og_inst);
		    current_op->list_og_inst = current_list_og_inst;
	       }
	       break;
	  default:
	       fprintf(stderr, LG_STR("Eobb_Edit: unknwon editable text type...\n",
				      "Eobb_Edit: unknwon editable text type...\n"));
	  }
	  break;
     default:
	  fprintf(stderr, LG_STR("Eobb_Edit: unknwon editable type...\n",
				 "Eobb_Edit: unknwon editable type...\n"));
	  break;
     }
     XtFree(tmp_string);

     XtUnmanageChild(eObjectBBoard->editObjectForm);
     edit_og(dd->canvas, dd, dd->edited_og, *string);

     set_editable_og_width(dd->edited_og, pretty_width);
     set_editable_og_fill_lines(dd->edited_og, fill_lines);

     if (text_modified)		/* No need to update if we modified edges... */
	  updateVisibleFieldsDialogIfManaged(dd);
}

void Eobb_Delete(Widget w, EditObjectStruct * eObjectBBoard, XtPointer call_data)
{
      XmTextSetString(eObjectBBoard->text, "");
}

void Eobb_Cancel(Widget w, Widget bb, XtPointer call_data)
{
     XtUnmanageChild(bb);
}

/* not used
void fill_button_changed(Widget w, EditObjectStruct * eObjectBBoard, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set) {
	  ;
     } else {
	  ;
     }
}
*/

void Eobb_Apply(Widget w, EditObjectStruct * eObjectBBoard, XtPointer call_data)
{
     Arg arg[1];
     Draw_Data *dd;
     PString *string;
     PString sstring, tmp_string;
     PBoolean text_modified = FALSE;
     PBoolean res;
     PBoolean save_cr;

     int new_width = 0;
     char *tmp_str;

     tmp_str = XmTextGetString(eObjectBBoard->width);

     if (!sscanf ( tmp_str,"%d", &new_width )) 
	 fprintf ( stderr, LG_STR("The width must be an integer.\n",
				  "The width must be an integer.\n"));
     XtFree (tmp_str);
 
     fill_lines = XmToggleButtonGetState(eObjectBBoard->fill_true);

     sstring = XmTextGetString(eObjectBBoard->text);
     string = &sstring;
     tmp_string = sstring;
     
     XtSetArg(arg[0], XmNuserData, &dd);
     XtGetValues(eObjectBBoard->editObjectForm, arg, 1);

     switch (dd->edited_og->type) {
     case DT_EDGE_TEXT:
	  if (new_width <= 0)
	       pretty_width = edge_width;
	  else
	       pretty_width = new_width;
	  if (!check_edge(string))
	       return;
	  break;
     case DT_TEXT:
	  text_modified = TRUE;
	  switch (dd->edited_og->u.gtext->text_type) {
	  case TT_INVOCATION:
	       if (new_width <= 0)
		    pretty_width = ip_width;
	       else
		    pretty_width = new_width;
	       if (!check_ip(string))
		    return;
	       break;
	  case TT_CONTEXT:
	       if (new_width <= 0)
		    pretty_width = ctxt_width;
	       else
		    pretty_width = new_width;
	       if (!check_ctxt(string))
		    return;
	       break;
	  case TT_CALL:
	       if (new_width <= 0)
		    pretty_width = call_width;
	       else
		    pretty_width = new_width;
	       if (!check_call(string))
		    return;
	       break;
	  case TT_SETTING:
	       if (new_width <= 0)
		    pretty_width = set_width;
	       else
		    pretty_width = new_width;
	       if (!check_set(string))
		    return;
	       break;
	  case TT_PROPERTIES:
	       if (new_width <= 0)
		    pretty_width = prop_width;
	       else
		    pretty_width = new_width;
	       if (!check_prop(string))
		    return;
	       break;
	  case TT_DOCUMENTATION:
	       save_cr = replace_cr;
	       replace_cr = FALSE;
	       if (new_width <= 0)
		    pretty_width = doc_width;
	       else
		    pretty_width = new_width;
	       if (!check_doc(string)) {
		    replace_cr = save_cr;
		    return;
	       }
	       replace_cr = save_cr;
	       break;
	  case TT_EFFECTS:
	       if (new_width <= 0)
		    pretty_width = eff_width;
	       else
		    pretty_width = new_width;
	       if (!check_eff(string))
		    return;
	       break;
	  case TT_ACTION:
	       if (new_width <= 0)
		    pretty_width = act_width;
	       else
		    pretty_width = new_width;
	       if (!check_act(string))
		    return;
	       break;
	  case TT_BODY:
	       current_body_indent = 0;
	       current_body_line = 0;
	       current_list_og_inst = sl_make_slist();
	       if (new_width <= 0)
		    pretty_width = bd_width;
	       else
		    pretty_width = new_width;
	       res = check_body(string);

	       free_list_og_inst(current_list_og_inst);
	       if (!res)
		    return;
	       break;
	  default:
	       fprintf(stderr, LG_STR("Eobb_Apply: unknwon editable text type...\n",
				      "Eobb_Apply: unknwon editable text type...\n"));
	  }
	  break;
     default:
	  fprintf(stderr, LG_STR("Eobb_Apply: unknwon editable type...\n",
				 "Eobb_Apply: unknwon editable type...\n"));
	  break;
     }
     XmTextSetString(eObjectBBoard->text, *string);
     XtFree (tmp_string);

     FREE(sstring);

     if (text_modified)		/* No need to update if we modified edges... */
	  updateVisibleFieldsDialogIfManaged(dd);

}

void edit_edge_bboard(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     Widget eobb_name_label;
     Widget eobb_text_sep, eobb_pretty_sep;
     Widget eobb_text_field;
     Widget eobb_menu_help, eobb_syntax;
     Widget eobb_menu, eobb_edit, eobb_delete, eobb_cancel;

     Widget eobb_pretty_menu, eobb_pretty_widthFld, eobb_pretty_widthLbl;
     Widget eobb_filling_label, eobb_apply;
     Widget eobb_filling_radiobox, eobb_filling_true_radiobutton, eobb_filling_false_radiobutton;

     Widget editObjectForm;


     editObjectStruct = MAKE_OBJECT(EditObjectStruct);

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     XtSetArg(args[n], XmNverticalSpacing, 5); n++;
     XtSetArg(args[n], XmNhorizontalSpacing, 5); n++;

     editObjectForm = XmCreateFormDialog(parent, "editObjectForm", args, n);
     editObjectStruct->editObjectForm = editObjectForm;

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     eobb_name_label = XmCreateLabel(editObjectForm, "eobb_name_label", args, n);
     XtManageChild(eobb_name_label);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     eobb_menu_help = XmCreateForm(editObjectForm, "eobb_menu_help", args, n);
     XtManageChild(eobb_menu_help);

     n = 0;
     eobb_syntax = XmCreatePushButton(eobb_menu_help, "eobb_syntax", args, n);
     XtAddCallback(eobb_syntax, XmNactivateCallback, infoHelp, makeFileNode("oprs", "Syntax and Semantic Used in the OPRS Development Environment"));
     XtManageChild(eobb_syntax);


     n = 0;

     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNfractionBase, 7); n++;
     eobb_pretty_menu= XmCreateForm(editObjectForm, "eobb_pretty_menu",args,n);
     XtManageChild(eobb_pretty_menu);

     n = 0;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, eobb_pretty_menu); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     eobb_pretty_sep = XmCreateSeparator(editObjectForm, "eobb_pretty_sep", args, n);
     XtManageChild(eobb_pretty_sep);

     n = 0;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, eobb_pretty_sep); n++;
     XtSetArg(args[n], XmNfractionBase, 7); n++;
     eobb_menu = XmCreateForm(editObjectForm, "eobb_menu", args, n);
     XtManageChild(eobb_menu);

     n = 0;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, eobb_menu); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     eobb_text_sep = XmCreateSeparator(editObjectForm, "eobb_text_sep", args, n);
     XtManageChild(eobb_text_sep);

     n = 0;
     XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNrows, 10); n++;
     XtSetArg(args[n], XmNcolumns, 40); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, True); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
/*
     XtSetArg(args[n], XmNtopWidget, eobb_name_label); n++;
*/
     XtSetArg(args[n], XmNtopWidget, eobb_menu_help); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNbottomWidget, eobb_text_sep); n++;
     eobb_text_field = XmCreateScrolledText(editObjectForm, "eobb_text_field", args, n);
     XtManageChild(eobb_text_field);

     editObjectStruct->text = eobb_text_field;

     n = 0;
/*
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 180); n++;
*/
/*
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     eobb_pretty_sep2 = XmCreateSeparator(editObjectForm, "eobb_pretty_sep2", args, n);
     XtManageChild(eobb_pretty_sep2);
*/
/*
     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 130); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtManageChild(XmCreateSeparator(editObjectForm, "eofSeparator", args, n));
*/

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 2); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 5); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 2); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 1); n++;
     eobb_edit = XmCreatePushButton(eobb_menu, "eobb_edit", args, n);
     XtAddCallback(eobb_edit, XmNactivateCallback, (XtCallbackProc)Eobb_Edit, editObjectStruct);
     XtManageChild(eobb_edit);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 2); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 5); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 4); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 3); n++;
     eobb_delete = XmCreatePushButton(eobb_menu, "eobb_delete", args, n);
     XtAddCallback(eobb_delete, XmNactivateCallback,(XtCallbackProc)Eobb_Delete, editObjectStruct);
     XtManageChild(eobb_delete);

     n= 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 2); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 5); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 6); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 5); n++;
     eobb_cancel = XmCreatePushButton(eobb_menu, "eobb_cancel", args, n);
     XtAddCallback(eobb_cancel, XmNactivateCallback,(XtCallbackProc)Eobb_Cancel, editObjectForm);
     XtManageChild(eobb_cancel);

     n= 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 0); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 3); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 0); n++;
     eobb_pretty_widthLbl = XmCreateLabel(eobb_pretty_menu,"eobb_pretty_widthLbl", args, n);
     XtManageChild(eobb_pretty_widthLbl);

     n= 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 0); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 3); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 3); n++;
     XtSetArg(args[n], XmNcolumns, 7); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, True); n++;
     eobb_pretty_widthFld = XmCreateTextField(eobb_pretty_menu,"eobb_pretty_widthFld", args, n);
     XtManageChild(eobb_pretty_widthFld);

     editObjectStruct->width = eobb_pretty_widthFld;

     n= 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 4); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 7); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 0); n++;
     eobb_filling_label = XmCreateLabel(eobb_pretty_menu, "eobb_filling_label", args, n);
     XtManageChild(eobb_filling_label);

     n= 0;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 4); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 7); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 2); n++;
     eobb_filling_radiobox = XmCreateRadioBox(eobb_pretty_menu, "eobb_filling_radiobox", args, n);
     XtManageChild(eobb_filling_radiobox);

     n= 0;
     eobb_filling_true_radiobutton = XmCreateToggleButton(eobb_filling_radiobox, 
							  "eobb_filling_true_radiobutton", args, n);
     XtManageChild(eobb_filling_true_radiobutton);

     n= 0;
     eobb_filling_false_radiobutton = XmCreateToggleButton(eobb_filling_radiobox, 
							   "eobb_filling_false_radiobutton", args, n);

/*
     XtAddCallback(eobb_filling_false_radiobutton, XmNvalueChangedCallback, 
		   (XtCallbackProc)fill_button_changed, editObjectStruct);
*/
     XtManageChild(eobb_filling_false_radiobutton);

     editObjectStruct->fill_true = eobb_filling_true_radiobutton;
     editObjectStruct->fill_false = eobb_filling_false_radiobutton;


     n= 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNtopPosition, 2); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNbottomPosition, 5); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNleftPosition, 5); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
     XtSetArg(args[n], XmNrightPosition, 6); n++;
     eobb_apply = XmCreatePushButton(eobb_pretty_menu, "eobb_apply", args, n);
     XtAddCallback(eobb_apply, XmNactivateCallback,(XtCallbackProc)Eobb_Apply, editObjectStruct);
     XtManageChild(eobb_apply);
}

Widget editNodeNameDialog;

void editNodeNameDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *sname;
     String to_free;
     OG *og = dd->edited_og;
     Node *node, *renamed_node;

     if (og->type == DT_NODE) {
	  renamed_node = og->u.gnode->node;
	  
	  if ((renamed_node->type == NT_START)) {
	       fprintf(stderr,LG_STR("Incompatible Node type: %d in editNodeName.\n",
				     "Incompatible Node type: %d in editNodeName.\n"),renamed_node->type);
	       return;
	  }
	  
	  sname = to_free = XmTextGetString(XmSelectionBoxGetChild(editNodeNameDialog, XmDIALOG_TEXT));

	  if (!check_node_name(&sname)) {
	       XtFree(to_free);
	       return;
	  }
	  XtFree(to_free);

	  sl_loop_through_slist(current_op->node_list, node, Node *)
	       if ((strcmp(sname, node->name) == 0) &&
		   (node != renamed_node)) {
		    report_user_error(LG_STR("This name is already used by another Node.",
					     "This name is already used by another Node."));
		    FREE(sname);
		    return;
	       }

	  rename_and_redraw_node(w,dd,og,sname);

	  FREE(sname);
     } else if (og->type == DT_IF_NODE) {
	  Node *renamed_then_node, *renamed_else_node;
	  PString then_name = NULL;
	  PString else_name = NULL;
	  PString *then_name_p, *else_name_p;

	  then_name_p = &then_name;
	  else_name_p = &else_name;

	  renamed_node = og->u.gnode->node;
	  renamed_then_node = then_node_from_if_node(renamed_node);
	  renamed_else_node = else_node_from_if_node(renamed_node);

	  sname = to_free = XmTextGetString(XmSelectionBoxGetChild(editNodeNameDialog, XmDIALOG_TEXT));

	  if (!check_node_name(&sname)) {
	       XtFree(to_free);
	       return;
	  }
	  XtFree(to_free);

	  new_then_else_node_name_from_if_name(sname, then_name_p, else_name_p);

	  sl_loop_through_slist(current_op->node_list, node, Node *) {
	       if ((strcmp(sname, node->name) == 0) &&
		   (node != renamed_node)) {
		    report_user_error(LG_STR("This name is already used by another Node.",
					     "This name is already used by another Node."));
		    FREE(sname);
		    FREE(then_name);
		    FREE(else_name);
		    return;
	       } else if ((strcmp(then_name, node->name) == 0) &&
		   (node != renamed_then_node)) {
		    report_user_error(LG_STR("The default then name is already used by another Node.",
					     "The default then name is already used by another Node."));
		    FREE(sname);
		    FREE(then_name);
		    FREE(else_name);
		    return;
	       } else if ((strcmp(else_name, node->name) == 0) &&
		   (node != renamed_else_node)) {
		    report_user_error(LG_STR("The default else name is already used by another Node.",
					     "The default else name is already used by another Node."));
		    FREE(sname);
		    FREE(then_name);
		    FREE(else_name);
		    return;
	       }
	  }

	  rename_and_redraw_if_node(w,dd,og,sname, then_name, else_name);

	  FREE(sname);
	  FREE(then_name);
	  FREE(else_name);
     } else {
	  fprintf(stderr,LG_STR("Incompatible Og type: %d in editNodeName.\n",
				"Incompatible Og type: %d in editNodeName.\n"),og->type);
	  return;
     }
     report_opfile_modification();
}

void make_edit_node_dialog(Widget parent, Draw_Data *dd)
{
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     editNodeNameDialog = XmCreatePromptDialog(parent, "editNodeNameDialog", args, n);
     XtAddCallback(editNodeNameDialog, XmNokCallback, (XtCallbackProc)editNodeNameDialogAccept, dd);
     XtAddCallback(editNodeNameDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Edit Object"));
}
