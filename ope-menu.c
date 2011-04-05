static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*-
 * ope-menu.c --
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
/* #include <math.h> */

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>		  /* CascadeButton for menubar  */
#include <Xm/PushB.h>			  /* PushButton Gadget for menu button */
#include <Xm/PushBG.h>			  /* PushButton Gadget for menu button */
#include <Xm/Frame.h>
#include <Xm/Separator.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>

#include "oprs-type.h"

#include "ope-graphic.h"
#include "ope-global.h"
#include "relevant-op_f.h"
#include "ope-graphic_f.h"
#include "ope-bboard_f.h"
#include "ope-filesel_f.h"
#include "ope-edit_f.h"
#include "ope-op-opf_f.h"
#include "ope-syntax_f.h"
#include "op-structure_f.h"
#include "xhelp.h"
#include "xhelp_f.h"
#include "ope-report_f.h"

#include "ope-print_f.h"
#include "ope-vsblf_f.h"


#ifdef OPE_ADD_MSTATS_BUTTON
void Debug(Widget w, Draw_Data *dd, XtPointer call_data)
{
     mstats(stderr,"Op-Editor Malloc functions.");
}
#endif /* OPE_ADD_MSTATS_BUTTON */

void OpfQuit(Widget w, XtPointer client_data, XtPointer call_data)
{
     quitQuestionManage();
}

void OpfLoad(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtManageChild(opeLoadFilesel);
}

void OpfSelect(Widget w, XtPointer client_data, XtPointer call_data)
{
     SelectOpFileDialogManage();
}

void OpfPrint(Widget w, XtPointer client_data, XtPointer call_data)
{
     printQuestionManage();
}

void OpfNew(Widget w, XtPointer client_data, XtPointer call_data)
{
     make_opfile(NULL, ACS_GRAPH);
     add_op_file_name(current_opfile->name, relevant_op);
}

void OpfAppend(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtManageChild(opeAppendFilesel);
}

void OpfUnload(Widget w, XtPointer client_data, XtPointer call_data)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int nb_item = 0, j;
     OPFile *opf;

     item = (XmStringTable)XtCalloc(sl_slist_length(list_opfiles) + 1,sizeof(XmString));

     sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	  item[nb_item] = XmStringCreateLtoR(opf->name, XmSTRING_DEFAULT_CHARSET);
	  nb_item++;
     }
     
     item[nb_item] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, nb_item); n++;
     XtSetValues(opeUnloadFileDialog, args, n);

     for (j = 0 ; j < nb_item; j++) {
	  XmStringFree(item[j]);
     }

     XtFree( (char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_TEXT),"");
     XtManageChild(opeUnloadFileDialog);
}

void OpfSave(Widget w, XtPointer client_data, XtPointer call_data)
{
     if (current_opfile->type != ACS_GRAPH) {
	  report_user_error(LG_STR("This version can only save in the graph format.\n You have to write it first in the graph format\nand then you can reload it and save it as you wish.",
				   "This version can only save in the graph format.\n You have to write it first in the graph format\nand then you can reload it and save it as you wish."));
	  return;
     }
     if (current_opfile->filed && check_write_access(current_opfile->name)) {
	  if (write_opfile(current_opfile->name, current_opfile) == -1)
	       report_user_error(LG_STR("Error while writing the file.",
					"Error while writing the file."));
	  return;
     }
     XtManageChild(opeSaveFilesel);
}

void OpfWrite(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtManageChild(opeWriteFilesel);
}

void OpfWriteTex(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtManageChild(opeWriteTexFilesel);
}

void OpDestroy(Widget w, Draw_Data *dd, XtPointer call_data)
{
     if (current_op != NULL)
	  XtManageChild(destroyOpQuestion);
}

void OpSelectDestroy(Widget w, XtPointer client_data, XtPointer call_data)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int nb_item = 0;
     Op_Structure *op;

     item = (XmStringTable)XtCalloc(sl_slist_length(current_opfile->list_op) + 1,sizeof(XmString));

     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *) {
	  if (op->xms_name == NULL) op->xms_name = XmStringCreateLtoR(op->name, XmSTRING_DEFAULT_CHARSET);
	  item[nb_item] = op->xms_name;
	  nb_item++;
     }

     item[nb_item] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, nb_item); n++;
     XtSetValues(destroyOpDialog, args, n);

     XtFree( (char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_TEXT),"");
     XtManageChild(destroyOpDialog);

}

void OpDump(Widget w, XtPointer client_data, XtPointer call_data)
{
     if (! current_op) {
	  report_user_error(LG_STR("You have to select a OP, before using this menu item.",
				   "You have to select a OP, before using this menu item."));
	  return;
     }

      XtManageChild(printSaveFile);
}

void MiscSize(Widget w, Draw_Data *dd, XtPointer call_data)
{   
     char s[LINSIZ];
     
     sprintf(s,"%d", dd->work_width);
     XmTextSetString(drawingSizeDialogTxtFldx,s);

     sprintf(s,"%d", dd->work_height);
     XmTextSetString(drawingSizeDialogTxtFldy,s);

     XtManageChild(drawingSizeDialog);
     return;
}

void MiscVisibleFields(Widget w, Draw_Data *dd, XtPointer call_data)
{   
     if (! current_op) {
	  report_user_error(LG_STR("You have to select a OP, before using this menu item.",
				   "You have to select a OP, before using this menu item."));
	  return;
     }

     updateVisibleFieldsDialog(dd);
     XtManageChild(VisibleFieldsDialog);
     return;
}

void MiscSymbolList(Widget w, Draw_Data *dd, XtPointer call_data)
{   
     symbolListDialogManage(symbolListDialog);
     return;
}

void OpSelect(Widget w, XtPointer client_data, XtPointer call_data)
{
     selectOpDialogManage();
}

void OpRedraw(Widget w, Draw_Data *dd, XtPointer call_data)
{
     XClearArea(XtDisplay(dd->canvas), dd->window, 0, 0, 0, 0, True);
}

void OpCreate(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Arg arg[1];

     XtSetArg(arg[0], XmNuserData, dd);
     XtSetValues(createOPBBoardStruct->createOPBBoard, arg, 1);

     XtManageChild(createOPBBoardStruct->createOPBBoard);

}

void OpDuplicate(Widget w, XtPointer client_data, XtPointer call_data)
{
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(duplicateOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(duplicateOpDialog);
     } else
	  report_user_error(LG_STR("you have to select a OP, before duplicating it.",
				   "you have to select a OP, before duplicating it."));
}

void OpRename(Widget w, XtPointer client_data, XtPointer call_data)
{
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(renameOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(renameOpDialog);
     } else
	  report_user_error(LG_STR("you have to select a OP, before renaming it.",
				   "you have to select a OP, before renaming it."));
}

void OpCopy(Widget w, XtPointer client_data, XtPointer call_data)
{
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(copyOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(copyOpDialog);
     } else
	  report_user_error(LG_STR("You have to select a OP, before copying it.",
				   "You have to select a OP, before copying it."));
}

void OpPaste(Widget w, XtPointer client_data, XtPointer call_data)
{
     pasteOpDialogManage();
}

void OpClear(Widget w, XtPointer client_data, XtPointer call_data)
{
     clear_buffer_opfile();
}

void OpPrevious(Widget w, Draw_Data *dd, XtPointer call_data)
{
     select_previous_op_in_same_file(dd);
}

void OpNext(Widget w, Draw_Data *dd, XtPointer call_data)
{
     select_next_op_in_same_file(dd);
}

void OpToggle_selected(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_last_selected_ops(dd);
}

void OpLast_selected(Widget w, XtPointer client_data, XtPointer call_data)
{
     last_selectedOpDialogManage();
}

Widget opRedraw, opDestroy, opDump, opPrint, opDuplicate, opRename, opCopy;

Widget moveObjects, createNode, createIfNode, createEdge, createKnot, editObjects;
Widget duplicateObjects, mergeNode, convertEnd, convertStart, relevantOp;
Widget alignObjects, alignObjectsVert, alignObjectsHor, destroyObjects;
Widget flipJoin, flipSplit, openNode;

Widget WMmoveObjects, WMcreateNode, WMcreateIfNode, WMcreateEdge, WMcreateKnot, WMeditObjects;
Widget WMduplicateObjects, WMmergeNode, WMconvertEnd, WMconvertStart, WMrelevantOp;
Widget WMalignObjects, WMalignObjectsVert, WMalignObjectsHor, WMdestroyObjects;
Widget WMflipJoin, WMflipSplit, WMopenNode;

Widget miscVisibleFields, miscSymbolList;

void update_select_sensitivity(PBoolean sensible)
{
     /* To set or unset all functions which needs a selected OP */
     XtSetSensitive(opRedraw, sensible);
     XtSetSensitive(opDestroy, sensible);
     XtSetSensitive(opDump, sensible);
     XtSetSensitive(opPrint, sensible);
     XtSetSensitive(opDuplicate, sensible);
     XtSetSensitive(opRename, sensible);
     XtSetSensitive(opCopy, sensible);

     XtSetSensitive(moveObjects, sensible);
     XtSetSensitive(WMmoveObjects, sensible);
     XtSetSensitive(createNode, sensible);
     XtSetSensitive(WMcreateNode, sensible);
     XtSetSensitive(createIfNode, sensible);
     XtSetSensitive(WMcreateIfNode, sensible);
     XtSetSensitive(flipJoin, sensible);
     XtSetSensitive(WMflipJoin, sensible);
     XtSetSensitive(openNode, sensible);
     XtSetSensitive(WMopenNode, sensible);
     XtSetSensitive(flipSplit, sensible);
     XtSetSensitive(WMflipSplit, sensible);
     XtSetSensitive(createEdge, sensible);
     XtSetSensitive(WMcreateEdge, sensible);
     XtSetSensitive(createKnot, sensible);
     XtSetSensitive(WMcreateKnot, sensible);
     XtSetSensitive(editObjects, sensible);
     XtSetSensitive(WMeditObjects, sensible);
     XtSetSensitive(duplicateObjects, sensible);
     XtSetSensitive(WMduplicateObjects, sensible);
     XtSetSensitive(mergeNode, sensible);
     XtSetSensitive(WMmergeNode, sensible);
     XtSetSensitive(convertEnd, sensible);
     XtSetSensitive(WMconvertEnd, sensible);
     XtSetSensitive(convertStart, sensible);
     XtSetSensitive(WMconvertStart, sensible);
     XtSetSensitive(alignObjects, sensible);
     XtSetSensitive(WMalignObjects, sensible);
     XtSetSensitive(alignObjectsVert, sensible);
     XtSetSensitive(WMalignObjectsVert, sensible);
     XtSetSensitive(alignObjectsHor, sensible);
     XtSetSensitive(WMalignObjectsHor, sensible);
     XtSetSensitive(destroyObjects, sensible);
     XtSetSensitive(WMdestroyObjects, sensible);
     XtSetSensitive(relevantOp, sensible);
     XtSetSensitive(WMrelevantOp, sensible);

     XtSetSensitive(miscVisibleFields, sensible);
     
     updateVisibleFieldsSensitivityIfManaged(sensible);
}

Widget opPaste, opClear;
PBoolean buffer_sensitivity;

void update_buffer_sensitivity(PBoolean sensible)
{
     XtSetSensitive(opPaste, sensible);
     XtSetSensitive(opClear, sensible);
     buffer_sensitivity = sensible;
}

Widget opToggle_selected, opLast_selected;

void update_toggle_sensitivity(PBoolean sensible)
{
     XtSetSensitive(opToggle_selected, sensible);
     XtSetSensitive(opLast_selected, sensible);
}

Widget opSelect, opSelectDestroy, opPrevious, opNext;

void update_empty_sensitivity(PBoolean sensible)
{
     XtSetSensitive(opSelect, sensible);
     XtSetSensitive(opSelectDestroy, sensible);
     XtSetSensitive(opPrevious, sensible);
     XtSetSensitive(opNext, sensible);
}

Widget opPDMenu;
Widget opCreate;
Widget opfWriteTex, opfAppend, opfSave, opfWrite;

void update_file_sensitivity(PBoolean sensible)
{
     if (sensible && buffer_sensitivity )
	  XtSetSensitive(opPaste, True);
     else
	  XtSetSensitive(opPaste, False);
/*
     XtSetSensitive(opPDMenu, sensible);
*/
     XtSetSensitive(opCreate, sensible);
     XtSetSensitive(opfAppend, sensible);
     XtSetSensitive(opfSave, sensible);
     XtSetSensitive(opfWrite, sensible);
     XtSetSensitive(opfWriteTex, sensible);
     XtSetSensitive(miscSymbolList, sensible);
     XtSetSensitive(symbolListDialog, sensible);
}

Widget WMPDMenu;

Widget create_menu_bar(Widget parent, Draw_Data *dd)
{
     Widget menuBar;
     Widget fileCButton, filePDMenu, opfQuit, opfLoad,
     opfNew;
     Widget editCButton, editPDMenu,  opfPrint, opfSelect, opfUnload;
     Widget opCButton;

     Widget miscCButton, miscPDMenu, miscSize, miscPrint;
     Widget helpCButton, helpPDMenu, help, file_help, edit_help, 
     	    op_help,  misc_help, work_help;

     Widget WMCButton;


     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     menuBar = XmCreateMenuBar(parent, "menuBar", args, n);
     XtManageChild(menuBar);

     filePDMenu = XmCreatePulldownMenu(menuBar, "filePDMenu", NULL, 0);

     editPDMenu = XmCreatePulldownMenu(menuBar, "editPDMenu", NULL, 0);

     opPDMenu = XmCreatePulldownMenu(menuBar, "opPDMenu", NULL, 0);

     miscPDMenu = XmCreatePulldownMenu(menuBar, "miscPDMenu", NULL, 0);

     WMPDMenu = XmCreatePulldownMenu(menuBar, "WMWorkMenu", NULL, 0);

     helpPDMenu = XmCreatePulldownMenu(menuBar, "helpPDMenu", NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, filePDMenu); n++;
     fileCButton = XmCreateCascadeButton(menuBar, "fileCButton", args, n);
     XtManageChild(fileCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, editPDMenu); n++;
     editCButton = XmCreateCascadeButton(menuBar, "editCButton", args, n);
     XtManageChild(editCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, opPDMenu); n++;
     opCButton = XmCreateCascadeButton(menuBar, "opCButton", args, n);
     XtManageChild(opCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, miscPDMenu); n++;
     miscCButton = XmCreateCascadeButton(menuBar, "miscCButton", args, n);
     XtManageChild(miscCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, WMPDMenu); n++;
     WMCButton = XmCreateCascadeButton(menuBar, "WMCButton", args, n);
     XtManageChild(WMCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, helpPDMenu); n++;
     helpCButton = XmCreateCascadeButton(menuBar, "helpCButton", args, n);
     XtManageChild(helpCButton);

     /* Help menu items */
     n = 0;
     help = XmCreatePushButtonGadget(helpPDMenu, "help", args, n);
     XtAddCallback(help, XmNactivateCallback, infoHelp, NULL);
     XtManageChild(help);

     XtManageChild(XmCreateSeparator(helpPDMenu, "fileseparator", NULL, 0));

     n = 0;
     file_help = XmCreatePushButtonGadget(helpPDMenu, "file_help", args, n);
     XtAddCallback(file_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "File Menu of the OP Editor"));
     XtManageChild(file_help);

     n = 0;
     edit_help = XmCreatePushButtonGadget(helpPDMenu, "edit_help", args, n);
     XtAddCallback(edit_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Edit Menu of the OP Editor"));
     XtManageChild(edit_help);

     n = 0;
     op_help = XmCreatePushButtonGadget(helpPDMenu, "op_help", args, n);
     XtAddCallback(op_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "OP Menu"));
     XtManageChild(op_help);

     n = 0;
     misc_help = XmCreatePushButtonGadget(helpPDMenu, "misc_help", args, n);
     XtAddCallback(misc_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Misc Menu"));
     XtManageChild(misc_help);

     n = 0;
     work_help = XmCreatePushButtonGadget(helpPDMenu, "work_help", args, n);
     XtAddCallback(work_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Working Menu Items"));
     XtManageChild(work_help);

     n = 0;
     XtSetArg(args[n], XmNmenuHelpWidget, (XtArgVal) helpCButton); n++;
     XtSetValues(menuBar, args, n);

	/* File Menu */

     opfLoad = XmCreatePushButtonGadget(filePDMenu, "opfLoad", NULL, 0);
     XtAddCallback(opfLoad, XmNactivateCallback, OpfLoad, 0);
     XtManageChild(opfLoad);

     opfAppend = XmCreatePushButtonGadget(filePDMenu, "opfAppend", NULL, 0);
     XtAddCallback(opfAppend, XmNactivateCallback, OpfAppend, 0);
     XtManageChild(opfAppend);

     opfNew = XmCreatePushButtonGadget(filePDMenu, "opfNew", NULL, 0);
     XtAddCallback(opfNew, XmNactivateCallback, OpfNew, 0);
     XtManageChild(opfNew);

     XtManageChild(XmCreateSeparator(filePDMenu, "fileseparator", NULL, 0));

     opfSave = XmCreatePushButtonGadget(filePDMenu, "opfSave", NULL, 0);
     XtAddCallback(opfSave, XmNactivateCallback, OpfSave, 0);
     XtManageChild(opfSave);

     opfWrite = XmCreatePushButtonGadget(filePDMenu, "opfWrite", NULL, 0);
     XtAddCallback(opfWrite, XmNactivateCallback, OpfWrite, 0);
     XtManageChild(opfWrite);

     opfWriteTex = XmCreatePushButtonGadget(filePDMenu, "opfWriteTex", NULL, 0);
     XtAddCallback(opfWriteTex, XmNactivateCallback, OpfWriteTex, 0);
     XtManageChild(opfWriteTex);
     XtManageChild(XmCreateSeparator(filePDMenu, "fileseparator", NULL, 0));

     opfQuit = XmCreatePushButtonGadget(filePDMenu, "opfQuit", NULL, 0);
     XtAddCallback(opfQuit, XmNactivateCallback, OpfQuit, 0);
     XtManageChild(opfQuit);

     /* Edit Menu */

     opfSelect = XmCreatePushButtonGadget(editPDMenu, "opfSelect", NULL, 0);
     XtAddCallback(opfSelect, XmNactivateCallback, OpfSelect, 0);
     XtManageChild(opfSelect);

     opfPrint = XmCreatePushButtonGadget(editPDMenu, "opfPrint", NULL, 0);
     XtAddCallback(opfPrint, XmNactivateCallback, OpfPrint, 0);
     XtManageChild(opfPrint);

     opfUnload = XmCreatePushButtonGadget(editPDMenu, "opfUnload", NULL, 0);
     XtAddCallback(opfUnload, XmNactivateCallback, OpfUnload, 0);
     XtManageChild(opfUnload);

     /* OP menu */

     opSelect = XmCreatePushButtonGadget(opPDMenu, "opSelect", NULL, 0);
     XtAddCallback(opSelect, XmNactivateCallback, OpSelect, 0);
     XtManageChild(opSelect);

     opCreate = XmCreatePushButtonGadget(opPDMenu, "opCreate", NULL, 0);
     XtAddCallback(opCreate, XmNactivateCallback, (XtCallbackProc)OpCreate, dd);
     XtManageChild(opCreate);

     opDuplicate = XmCreatePushButtonGadget(opPDMenu, "opDuplicate", NULL, 0);
     XtAddCallback(opDuplicate, XmNactivateCallback, OpDuplicate, 0);
     XtManageChild(opDuplicate);

     opRename = XmCreatePushButtonGadget(opPDMenu, "opRename", NULL, 0);
     XtAddCallback(opRename, XmNactivateCallback, OpRename, 0);
     XtManageChild(opRename);

     opRedraw = XmCreatePushButtonGadget(opPDMenu, "opRedraw", NULL, 0);
     XtAddCallback(opRedraw, XmNactivateCallback, (XtCallbackProc)OpRedraw, dd);
     XtManageChild(opRedraw);
     
     opDestroy = XmCreatePushButtonGadget(opPDMenu, "opDestroy", NULL, 0);
     XtAddCallback(opDestroy, XmNactivateCallback,  (XtCallbackProc)OpDestroy, dd);
     XtManageChild(opDestroy);
     
     opSelectDestroy = XmCreatePushButtonGadget(opPDMenu, "opSelectDestroy", NULL, 0);
     XtAddCallback(opSelectDestroy, XmNactivateCallback, OpSelectDestroy, 0);
     XtManageChild(opSelectDestroy);
     
     XtManageChild(XmCreateSeparator(opPDMenu, "opseparator", NULL, 0));

     opDump = XmCreatePushButtonGadget(opPDMenu, "opDump", NULL, 0);
     XtAddCallback(opDump, XmNactivateCallback, OpDump, 0);
     XtManageChild(opDump);
     
     opPrint = XmCreatePushButtonGadget(opPDMenu, "opPrint", NULL, 0);
     XtAddCallback(opPrint, XmNactivateCallback,(XtCallbackProc)OpPrint, dd);
     XtManageChild(opPrint);

     XtManageChild(XmCreateSeparator(opPDMenu, "opseparator", NULL, 0));

     opPrevious = XmCreatePushButtonGadget(opPDMenu, "opPrevious", NULL, 0);
     XtAddCallback(opPrevious, XmNactivateCallback,  (XtCallbackProc)OpPrevious, dd);
     XtManageChild(opPrevious);

     opNext = XmCreatePushButtonGadget(opPDMenu, "opNext", NULL, 0);
     XtAddCallback(opNext, XmNactivateCallback,  (XtCallbackProc)OpNext, dd);
     XtManageChild(opNext);

     opToggle_selected = XmCreatePushButtonGadget(opPDMenu, "opToggle_selected", NULL, 0);
     XtAddCallback(opToggle_selected, XmNactivateCallback,  (XtCallbackProc)OpToggle_selected, dd);
     XtManageChild(opToggle_selected);

     opLast_selected = XmCreatePushButtonGadget(opPDMenu, "opLast_selected", NULL, 0);
     XtAddCallback(opLast_selected, XmNactivateCallback, OpLast_selected, 0);
     XtManageChild(opLast_selected);

     XtManageChild(XmCreateSeparator(opPDMenu, "opseparator", NULL, 0));

     opCopy = XmCreatePushButtonGadget(opPDMenu, "opCopy", NULL, 0);
     XtAddCallback(opCopy, XmNactivateCallback, OpCopy, 0);
     XtManageChild(opCopy);

     opPaste = XmCreatePushButtonGadget(opPDMenu, "opPaste", NULL, 0);
     XtAddCallback(opPaste, XmNactivateCallback, OpPaste, 0);
     XtManageChild(opPaste);

     opClear = XmCreatePushButtonGadget(opPDMenu, "opClear", NULL, 0);
     XtAddCallback(opClear, XmNactivateCallback, OpClear, 0);
     XtManageChild(opClear);

     /* Misc Menu */

     miscSymbolList = XmCreatePushButtonGadget(miscPDMenu, "miscSymbolList", NULL, 0);
     XtAddCallback(miscSymbolList, XmNactivateCallback, (XtCallbackProc)MiscSymbolList, dd);
     XtManageChild(miscSymbolList);

     miscSize = XmCreatePushButtonGadget(miscPDMenu, "miscSize", NULL, 0);
     XtAddCallback(miscSize, XmNactivateCallback, (XtCallbackProc)MiscSize, dd);
     XtManageChild(miscSize);

     miscPrint = XmCreatePushButtonGadget(miscPDMenu, "miscPrint", NULL, 0);
     XtAddCallback(miscPrint, XmNactivateCallback, (XtCallbackProc)MiscPrint, dd);
     XtManageChild(miscPrint);

     miscVisibleFields = XmCreatePushButtonGadget(miscPDMenu, "miscVisibleFields", NULL, 0);
     XtAddCallback(miscVisibleFields, XmNactivateCallback, (XtCallbackProc)MiscVisibleFields, dd);
     XtManageChild(miscVisibleFields);

     /*
      * resetParser = XmCreatePushButtonGadget(miscPDMenu,"resetParser", NULL,
      * 0); XtAddCallback(resetParser, XmNactivateCallback, ResetParser, 0);
      * XtManageChild(resetParser);
      */

     return menuBar;

}

Widget toggled_button = NULL; 

void toggle_button(Widget w)
{
     Pixel back,fore;
     Arg args[2];
     Cardinal n;

     if (toggled_button) {
	  Widget widget = toggled_button;
	  
	  toggled_button = NULL;
	  toggle_button(widget);
     }

     toggled_button = w;


     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtSetArg(args[n], XmNforeground, &fore); n++;
     XtGetValues(w,args,n);

     n = 0;
     XtSetArg(args[n], XmNbackground, fore); n++;
     XtSetArg(args[n], XmNforeground, back); n++;
     XtSetValues(w,args,n);
     
}

void CreateNode(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(createNode);
     set_draw_mode_from_menu(dd, DRAW_NODE);
}

void CreateIfNode(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(createIfNode);
     set_draw_mode_from_menu(dd, DRAW_IFNODE);
}

void FlipJoin(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(flipJoin);
     set_draw_mode_from_menu(dd, FLIP_JOIN);
}

void OpenNode(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(openNode);
     set_draw_mode_from_menu(dd, OPEN_NODE);
}

void FlipSplit(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(flipSplit);
     set_draw_mode_from_menu(dd, FLIP_SPLIT);
}

void EditObjects(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(editObjects);
     set_draw_mode_from_menu(dd, EDIT_OG);
}

void MoveObjects(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(moveObjects);
     set_draw_mode_from_menu(dd, MOVE_OG);
}

void RelevantOp(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(relevantOp);
     set_draw_mode_from_menu(dd, DM_RELEVANT_OP);
}

void ConvertEnd(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(convertEnd);
     set_draw_mode_from_menu(dd, CONVERT_END);
}

void ConvertStart(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(convertStart);
     set_draw_mode_from_menu(dd, CONVERT_START);
}

void AlignObjects(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(alignObjects);
     set_draw_mode_from_menu(dd, ALIGN_OG);
}

void AlignObjectsVert(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(alignObjectsVert);
     set_draw_mode_from_menu(dd, ALIGN_OG_V);
}

void AlignObjectsHor(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(alignObjectsHor);
     set_draw_mode_from_menu(dd, ALIGN_OG_H);
}

void DestroyObjects(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(destroyObjects);
     set_draw_mode_from_menu(dd, DESTROY_OG);
}

void CreateEdge(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(createEdge);
     set_draw_mode_from_menu(dd, DRAW_EDGE);
}

void CreateKnot(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(createKnot);
     set_draw_mode_from_menu(dd, DRAW_KNOT);
}

void DuplicateObjects(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(duplicateObjects);
     set_draw_mode_from_menu(dd, DUPLICATE_OBJECTS);
}

void MergeNode(Widget w, Draw_Data *dd, XtPointer call_data)
{
     toggle_button(mergeNode);
     set_draw_mode_from_menu(dd, MERGE_NODE);
}

Widget create_menu_pane(Widget parent, Draw_Data *dd)
{
     Widget opeMenuFrame, opeMenu;
#ifdef OPE_ADD_MSTATS_BUTTON
     Widget debug;
#endif /* OPE_ADD_MSTATS_BUTTON */

     opeMenuFrame = XmCreateFrame(parent, "opeMenuFrame", NULL, 0);
     XtManageChild(opeMenuFrame);

     opeMenu = XmCreateWorkArea(opeMenuFrame, "opeMenu", NULL, 0);
     XtManageChild(opeMenu);

     moveObjects = XmCreatePushButton(opeMenu, "moveObjects", NULL, 0);
     XtAddCallback(moveObjects, XmNactivateCallback, (XtCallbackProc)MoveObjects, dd);
     XtManageChild(moveObjects);
     WMmoveObjects = XmCreatePushButton(WMPDMenu, "moveObjects", NULL, 0);
     XtAddCallback(WMmoveObjects, XmNactivateCallback, (XtCallbackProc)MoveObjects, dd);
     XtManageChild(WMmoveObjects);
     toggle_button(moveObjects);

     createNode = XmCreatePushButton(opeMenu, "createNode", NULL, 0);
     XtAddCallback(createNode, XmNactivateCallback, (XtCallbackProc)CreateNode, dd);
     XtManageChild(createNode);
     WMcreateNode = XmCreatePushButton(WMPDMenu, "createNode", NULL, 0);
     XtAddCallback(WMcreateNode, XmNactivateCallback, (XtCallbackProc)CreateNode, dd);
     XtManageChild(WMcreateNode);

     createIfNode = XmCreatePushButton(opeMenu, "createIfNode", NULL, 0);
     XtAddCallback(createIfNode, XmNactivateCallback, (XtCallbackProc)CreateIfNode, dd);
     XtManageChild(createIfNode);
     WMcreateIfNode = XmCreatePushButton(WMPDMenu, "createIfNode", NULL, 0);
     XtAddCallback(WMcreateIfNode, XmNactivateCallback, (XtCallbackProc)CreateIfNode, dd);
     XtManageChild(WMcreateIfNode);

     openNode = XmCreatePushButton(opeMenu, "openNode", NULL, 0);
     XtAddCallback(openNode, XmNactivateCallback, (XtCallbackProc)OpenNode, dd);
     XtManageChild(openNode);
     WMopenNode = XmCreatePushButton(WMPDMenu, "openNode", NULL, 0);
     XtAddCallback(WMopenNode, XmNactivateCallback, (XtCallbackProc)OpenNode, dd);
     XtManageChild(WMopenNode);

     createEdge = XmCreatePushButton(opeMenu, "createEdge", NULL, 0);
     XtAddCallback(createEdge, XmNactivateCallback, (XtCallbackProc)CreateEdge, dd);
     XtManageChild(createEdge);
     WMcreateEdge = XmCreatePushButton(WMPDMenu, "createEdge", NULL, 0);
     XtAddCallback(WMcreateEdge, XmNactivateCallback, (XtCallbackProc)CreateEdge, dd);
     XtManageChild(WMcreateEdge);

     createKnot = XmCreatePushButton(opeMenu, "createKnot", NULL, 0);
     XtAddCallback(createKnot, XmNactivateCallback, (XtCallbackProc)CreateKnot, dd);
     XtManageChild(createKnot);
     WMcreateKnot = XmCreatePushButton(WMPDMenu, "createKnot", NULL, 0);
     XtAddCallback(WMcreateKnot, XmNactivateCallback, (XtCallbackProc)CreateKnot, dd);
     XtManageChild(WMcreateKnot);

     flipJoin = XmCreatePushButton(opeMenu, "flipJoin", NULL, 0);
     XtAddCallback(flipJoin, XmNactivateCallback, (XtCallbackProc)FlipJoin, dd);
     XtManageChild(flipJoin);
     WMflipJoin = XmCreatePushButton(WMPDMenu, "flipJoin", NULL, 0);
     XtAddCallback(WMflipJoin, XmNactivateCallback, (XtCallbackProc)FlipJoin, dd);
     XtManageChild(WMflipJoin);

     flipSplit = XmCreatePushButton(opeMenu, "flipSplit", NULL, 0);
     XtAddCallback(flipSplit, XmNactivateCallback, (XtCallbackProc)FlipSplit, dd);
     XtManageChild(flipSplit);
     WMflipSplit = XmCreatePushButton(WMPDMenu, "flipSplit", NULL, 0);
     XtAddCallback(WMflipSplit, XmNactivateCallback, (XtCallbackProc)FlipSplit, dd);
     XtManageChild(WMflipSplit);

     editObjects = XmCreatePushButton(opeMenu, "editObjects", NULL, 0);
     XtAddCallback(editObjects, XmNactivateCallback, (XtCallbackProc)EditObjects, dd);
     XtManageChild(editObjects);
     WMeditObjects = XmCreatePushButton(WMPDMenu, "editObjects", NULL, 0);
     XtAddCallback(WMeditObjects, XmNactivateCallback, (XtCallbackProc)EditObjects, dd);
     XtManageChild(WMeditObjects);

     duplicateObjects = XmCreatePushButton(opeMenu, "duplicateObjects", NULL, 0);
     XtAddCallback(duplicateObjects, XmNactivateCallback, (XtCallbackProc)DuplicateObjects, dd);
     XtManageChild(duplicateObjects);
     WMduplicateObjects = XmCreatePushButton(WMPDMenu, "duplicateObjects", NULL, 0);
     XtAddCallback(WMduplicateObjects, XmNactivateCallback, (XtCallbackProc)DuplicateObjects, dd);
     XtManageChild(WMduplicateObjects);

     mergeNode = XmCreatePushButton(opeMenu, "mergeNode", NULL, 0);
     XtAddCallback(mergeNode, XmNactivateCallback, (XtCallbackProc)MergeNode, dd);
     XtManageChild(mergeNode);
     WMmergeNode = XmCreatePushButton(WMPDMenu, "mergeNode", NULL, 0);
     XtAddCallback(WMmergeNode, XmNactivateCallback, (XtCallbackProc)MergeNode, dd);
     XtManageChild(WMmergeNode);

     convertEnd = XmCreatePushButton(opeMenu, "convertEnd", NULL, 0);
     XtAddCallback(convertEnd, XmNactivateCallback, (XtCallbackProc)ConvertEnd, dd);
     XtManageChild(convertEnd);
     WMconvertEnd = XmCreatePushButton(WMPDMenu, "convertEnd", NULL, 0);
     XtAddCallback(WMconvertEnd, XmNactivateCallback, (XtCallbackProc)ConvertEnd, dd);
     XtManageChild(WMconvertEnd);

     convertStart = XmCreatePushButton(opeMenu, "convertStart", NULL, 0);
     XtAddCallback(convertStart, XmNactivateCallback, (XtCallbackProc)ConvertStart, dd);
     XtManageChild(convertStart);
     WMconvertStart = XmCreatePushButton(WMPDMenu, "convertStart", NULL, 0);
     XtAddCallback(WMconvertStart, XmNactivateCallback, (XtCallbackProc)ConvertStart, dd);
     XtManageChild(WMconvertStart);

     alignObjects = XmCreatePushButton(opeMenu, "alignObjects", NULL, 0);
     XtAddCallback(alignObjects, XmNactivateCallback, (XtCallbackProc)AlignObjects, dd);
     XtManageChild(alignObjects);
     WMalignObjects = XmCreatePushButton(WMPDMenu, "alignObjects", NULL, 0);
     XtAddCallback(WMalignObjects, XmNactivateCallback, (XtCallbackProc)AlignObjects, dd);
     XtManageChild(WMalignObjects);

     alignObjectsVert = XmCreatePushButton(opeMenu, "alignObjectsVert", NULL, 0);
     XtAddCallback(alignObjectsVert, XmNactivateCallback, (XtCallbackProc)AlignObjectsVert, dd);
     XtManageChild(alignObjectsVert);
     WMalignObjectsVert = XmCreatePushButton(WMPDMenu, "alignObjectsVert", NULL, 0);
     XtAddCallback(WMalignObjectsVert, XmNactivateCallback, (XtCallbackProc)AlignObjectsVert, dd);
     XtManageChild(WMalignObjectsVert);

     alignObjectsHor = XmCreatePushButton(opeMenu, "alignObjectsHor", NULL, 0);
     XtAddCallback(alignObjectsHor, XmNactivateCallback, (XtCallbackProc)AlignObjectsHor, dd);
     XtManageChild(alignObjectsHor);
     WMalignObjectsHor = XmCreatePushButton(WMPDMenu, "alignObjectsHor", NULL, 0);
     XtAddCallback(WMalignObjectsHor, XmNactivateCallback, (XtCallbackProc)AlignObjectsHor, dd);
     XtManageChild(WMalignObjectsHor);

     destroyObjects = XmCreatePushButton(opeMenu, "destroyObjects", NULL, 0);
     XtAddCallback(destroyObjects, XmNactivateCallback, (XtCallbackProc)DestroyObjects, dd);
     XtManageChild(destroyObjects);
     WMdestroyObjects = XmCreatePushButton(WMPDMenu, "destroyObjects", NULL, 0);
     XtAddCallback(WMdestroyObjects, XmNactivateCallback, (XtCallbackProc)DestroyObjects, dd);
     XtManageChild(WMdestroyObjects);

     relevantOp = XmCreatePushButton(opeMenu, "relevantOp", NULL, 0);
     XtAddCallback(relevantOp, XmNactivateCallback, (XtCallbackProc)RelevantOp, dd);
     XtManageChild(relevantOp);
     WMrelevantOp = XmCreatePushButton(WMPDMenu, "relevantOp", NULL, 0);
     XtAddCallback(WMrelevantOp, XmNactivateCallback, (XtCallbackProc)RelevantOp, dd);
     XtManageChild(WMrelevantOp);

#ifdef OPE_ADD_MSTATS_BUTTON
     debug = XmCreatePushButton(opeMenu, "debug", NULL, 0);
     XtAddCallback(debug, XmNactivateCallback, (XtCallbackProc)Debug, 0);
     XtManageChild(debug);
#endif /* OPE_ADD_MSTATS_BUTTON */

     return opeMenuFrame;
}
