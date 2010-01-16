static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*- 
 * ope-rop.c -- 
 * 
 * Copyright (c) 1991-2010 Francois Felix Ingrand.
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
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>

#include "macro.h"
#include "oprs-type.h"
#include "ope-graphic.h"
#include "ope-global.h"
#include "relevant-op.h"
#include "op-structure.h"

#include "relevant-op_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

Widget rOpDialog;
ListLastSelectedOP list_rops;

void select_op_from_op(Op_Structure *op, Draw_Data *dd)
{
     OPFile *opf;
     char *op_name = op->name;
     char *file_name = op->file_name;

     if ((current_opfile == NULL ) || (strcmp(current_opfile->name,file_name) != 0))
	  /* Select the Op File */
	  sl_loop_through_slist(list_opfiles, opf, OPFile *)
	       if (strcmp(opf->name, file_name) == 0) {
		    select_opfile(opf);
		    update_empty_sensitivity(True); /* we know that this file is not empty */
		    break;
	       }
     if ((current_op == NULL ) || (strcmp(current_op->name,op_name) != 0))
	  /* Select the Op File */
	  
	  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, op_name) == 0) {
		    select_op(op, dd);
		    break;
	       }
}     

void rOpDialogManage()
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int j, i = 0;
     char *full_name;
     Op_Structure *op;

     item = (XmStringTable)XtCalloc(sl_slist_length(list_rops) + 1,sizeof(XmString));

     sl_loop_through_slist(list_rops, op, Op_Structure *) {

	  full_name = (char *)MALLOC (strlen(op->name) + strlen(op->file_name) + 5);

	  full_name[0] = '\0';
	  strcat(full_name, op->name);
	  strcat(full_name, "  [");
	  strcat(full_name, op->file_name);
	  strcat(full_name, "]");
	  item[i] = XmStringCreateLtoR(full_name, XmSTRING_DEFAULT_CHARSET);
	  FREE(full_name);	/* There was a leak here... */
	  i++;
     }

     item[i] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, i); n++;
     XtSetValues(rOpDialog, args, n);

     for (j = 0 ; j < i ; j++) {
	  XmStringFree(item[j]);
     }

     XtFree((char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(rOpDialog, XmDIALOG_TEXT),"");
     XtManageChild(rOpDialog);
}

void rOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     OPFullNameStruct *last_selected_op;
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
	  select_op_from_op(op,dd);
     }
     XtFree((char *)pos_list);
}

void ope_find_rel_ops_gtexpr(Expression *gtexpr)
{
     FREE_SLIST(list_rops);

     list_rops = find_rel_ops_expr(gtexpr);

     if (sl_slist_empty(list_rops))
	  ope_information_report(LG_STR("No relevant OP found for this gtexpr.",
					"No relevant OP found for this gtexpr."));
     else
	  rOpDialogManage();
}


void ope_find_rel_ops(Edge *edge)
{
     if (edge->expr) {
	  ope_find_rel_ops_gtexpr(edge->expr);
     } else {
	  fprintf(stderr, LG_STR("ope_find_rel_ops: no gtexpr associated to this edge.\n",
				 "ope_find_rel_ops: no gtexpr associated to this edge.\n"));
     }
}

void ope_find_rel_ops_inst(Instruction *inst)
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
	  ope_find_rel_ops_gtexpr(gtexpr);
     } else {
	  fprintf(stderr, LG_STR("ope_find_rel_ops: no gtexpr associated to this instruction.\n",
				 "ope_find_rel_ops: no gtexpr associated to this instruction.\n"));
     }
}

/* void ope_find_rel_ops_list_gtexpr(ExprList lgtexpr) */
/* { */
/*      FREE_SLIST(list_rops); */

/*      list_rops = find_rel_ops_list_expr(lgtexpr); */

/*      if (sl_slist_empty(list_rops)) */
/* 	  ope_information_report(LG_STR("No relevant OP found for this list of gtexpr.", */
/* 					"No relevant OP found for this list of gtexpr.")); */
/*      else */
/* 	  rOpDialogManage(); */
/* } */

void ope_find_rel_ops_gmexpr_except_me(Expression *gmexpr, Op_Structure *op)
{
     FREE_SLIST(list_rops);

     list_rops = find_rel_ops_expr(gmexpr);

     sl_delete_slist_node(list_rops, op);

     if (sl_slist_empty(list_rops))
	  ope_information_report(LG_STR("No relevant OP found for this gmexpr.",
					"No relevant OP found for this gmexpr."));
     else
	  rOpDialogManage();
}

void ope_find_rel_ops_gmexpr(Expression *gmexpr)
{
     FREE_SLIST(list_rops);

     list_rops = find_rel_ops_expr(gmexpr);

     if (sl_slist_empty(list_rops))
	  ope_information_report(LG_STR("No relevant OP found for this gmexpr.",
					"No relevant OP found for this gmexpr."));
     else
	  rOpDialogManage();
}


void ope_create_rop_dialog(Widget parent, Draw_Data *dd)
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
