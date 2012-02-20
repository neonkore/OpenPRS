
/*                               -*- Mode: C -*- 
 * xoprs-rop.c -- 
 * 
 * Copyright (c) 1991-2011 Francois Felix Ingrand, LAAS/CNRS.
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

#include "macro.h"
#include "oprs-type.h"
#include "op-structure.h"
#include "gope-graphic.h"
#include "ope-global.h"
#include "relevant-op.h"
#include "op-structure.h"

#include "relevant-op_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

#include "xm2gtk_f.h"

#ifdef IGNORE
PBoolean sort_op_opf(Op_Structure *op1, Op_Structure *op2);

ListLastSelectedOP list_rops;
Widget rOpDialog;

void rOpDialogManage()
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     Op_Structure *op;


     sl_sort_slist_func(list_rops,sort_op_opf);

     item = (XmStringTable)XtCalloc(sl_slist_length(list_rops) + 1,sizeof(XmString));

     sl_loop_through_slist(list_rops, op, Op_Structure *) {

	  if (!op->xms_name) build_op_xms_name(op);
	  item[i] = op->xms_name;
	  i++;
     }

     item[i] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, i); n++;
     XtSetValues(rOpDialog, args, n);

     XtFree((char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_TEXT),"");

     XtManageChild(rOpDialog);
}

void rOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Op_Structure *op;
     int items;
     int *pos_list, pos_count;
     Arg args[MAXARGS];
     PBoolean res;

     XtSetArg(args[0], XmNselectedItemCount, &items);
     XtGetValues(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_LIST), args, 1);

     res = XmListGetSelectedPos(XmSelectionBoxGetChild(rOpDialog,
						       XmDIALOG_LIST),
				&pos_list, &pos_count);

     XmListDeselectAllItems(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_LIST));

     if (!res) 
	  return;

     if (pos_count > 1) {
	  fprintf(stderr, LG_STR("More than one OP slected in ropDialogAccept.\n",
				 "More than one OP slected in ropDialogAccept.\n"));

     } else {
	  op = (Op_Structure *)sl_get_slist_pos(list_rops, pos_list[0]);
     
	  if (!op) {
	       fprintf(stderr, LG_STR("No OP found in ropDialogAccept\n.",
				      "No OP found in ropDialogAccept\n."));
	       return;
	  }
	  display_op_pos(op,dd, 0,0);
     }
     XtFree((char *)pos_list);
}

void xp_find_rel_ops_expr(Expression *gtexpr)
{
     FLUSH_SLIST(list_rops);
     list_rops = find_rel_ops_expr(gtexpr);

     if (sl_slist_empty(list_rops))
	  xp_information_report(LG_STR("No relevant OP found for this gtexpr.",
				       "No relevant OP found for this gtexpr."));
     else
	  rOpDialogManage();

}

void xp_find_rel_ops(Edge *edge)
{
     if (edge->expr) {
	  xp_find_rel_ops_expr(edge->expr);
     } else {
	  fprintf(stderr, LG_STR("xp_find_rel_ops: no gtexpr associated to this edge.\n",
				 "xp_find_rel_ops: no gtexpr associated to this edge.\n"));
     }
}

void xp_find_rel_ops_inst(Instruction *inst)
{
     Expression *gtexpr = NULL;

     switch (inst->type) {
     case IT_SIMPLE:
	  gtexpr = inst->u.simple_inst->expr;
	  break;
     case IT_IF:
	  gtexpr = inst->u.if_inst->condition;
	  break;
     case IT_WHILE:
	  gtexpr = inst->u.while_inst->condition;
	  break;
     case IT_DO:
	  gtexpr = inst->u.do_inst->condition;
	  break;
     default:
	  break;
     }
     if (gtexpr) {
	  xp_find_rel_ops_expr(gtexpr);
     } else {
	  fprintf(stderr, LG_STR("xp_find_rel_ops: no gtexpr associated to this instruction.\n",
				 "xp_find_rel_ops: no gtexpr associated to this instruction.\n"));
     }
}

void xp_find_rel_ops_expr_except_me(Expression *gmexpr, Op_Structure *op)
{
     FREE_SLIST(list_rops);

     list_rops = find_rel_ops_expr(gmexpr);

     sl_delete_slist_node(list_rops, op);

     if (sl_slist_empty(list_rops))
	  xp_information_report(LG_STR("No relevant OP found for this gmexpr.",
				       "No relevant OP found for this gmexpr."));
     else
	  rOpDialogManage();
}

void xp_create_rop_dialog(Widget parent, Draw_Data *dd)
{
     Cardinal n;
     Arg args[MAXARGS];

     list_rops = sl_make_slist();

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;

     rOpDialog = XmCreateSelectionDialog(parent, "rOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_LIST_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_TEXT));
     XtAddCallback(rOpDialog, XmNokCallback,  (XtCallbackProc)rOpDialogAccept, dd);
     XtAddCallback(rOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Relevant OP"));
}

#endif
