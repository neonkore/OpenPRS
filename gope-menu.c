/*                               -*- Mode: C -*-
 * gope-menu.c --
 *
 * Copyright (c) 2011 LAAS/CNRS
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


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "xm2gtk.h"

#include "oprs-type.h"

#include "gope-main_f.h"
#include "op-structure.h"
#include "gope-graphic.h"
#include "gope-global.h"
#include "relevant-op_f.h"
#include "gope-graphic_f.h"
#include "ope-bboard_f.h"
#include "gope-filesel_f.h"
#include "gope-edit_f.h"
#include "gope-op-opf_f.h"
#include "ope-syntax_f.h"
#include "op-structure_f.h"
#include "xhelp.h"
#include "xhelp_f.h"
#include "ope-report_f.h"

#include "ope-print_f.h"
#include "ope-vsblf_f.h"

#include "xm2gtk_f.h"


#ifdef OPE_ADD_MSTATS_BUTTON
void Debug(Widget w, Draw_Data *dd, XtPointer call_data)
{
     mstats(stderr,"Op-Editor Malloc functions.");
}
#endif /* OPE_ADD_MSTATS_BUTTON */

void OpfLoad(GtkWidget *w, Draw_Data *dd)
{
  opeLoadFileSelRun(opeLoadFilesel, dd);
}

void OpfSelect(Widget w, XtPointer client_data, XtPointer call_data)
{
     SelectOpFileDialogManage();
}

/* void OpfPrint(Widget w, XtPointer client_data, XtPointer call_data) */
/* { */
/*      printQuestionManage(); */
/* } */

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
#ifdef IGNORE_GTK
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
#endif
}

void OpfSave(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
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
#endif
}

void OpfWrite(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     XtManageChild(opeWriteFilesel);
#endif
}

void OpfWriteTex(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     XtManageChild(opeWriteTexFilesel);
#endif
}

void OpDestroy(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     if (current_op != NULL)
	  XtManageChild(destroyOpQuestion);
#endif
}

void OpSelectDestroy(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
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

#endif
}

void OpDump(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     if (! current_op) {
	  report_user_error(LG_STR("You have to select a OP, before using this menu item.",
				   "You have to select a OP, before using this menu item."));
	  return;
     }

      XtManageChild(printSaveFile);
#endif
}

void MiscSize(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK   
     char s[LINSIZ];
     
     sprintf(s,"%d", dd->work_width);
     XmTextSetString(drawingSizeDialogTxtFldx,s);

     sprintf(s,"%d", dd->work_height);
     XmTextSetString(drawingSizeDialogTxtFldy,s);

     XtManageChild(drawingSizeDialog);
     return;
#endif
}

void MiscVisibleFields(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK   
     if (! current_op) {
	  report_user_error(LG_STR("You have to select a OP, before using this menu item.",
				   "You have to select a OP, before using this menu item."));
	  return;
     }

     updateVisibleFieldsDialog(dd);
     XtManageChild(VisibleFieldsDialog);
     return;
#endif
}

void MiscSymbolList(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK   
     symbolListDialogManage(symbolListDialog);
     return;
#endif
}

void OpRedraw(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     XClearArea(XtDisplay(dd->canvas), dd->window, 0, 0, 0, 0, True);
#endif
}

void OpCreate(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     Arg arg[1];

     XtSetArg(arg[0], XmNuserData, dd);
     XtSetValues(createOPBBoardStruct->createOPBBoard, arg, 1);

     XtManageChild(createOPBBoardStruct->createOPBBoard);

#endif
}

void OpDuplicate(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(duplicateOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(duplicateOpDialog);
     } else
	  report_user_error(LG_STR("you have to select a OP, before duplicating it.",
				   "you have to select a OP, before duplicating it."));
#endif
}

void OpRename(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(renameOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(renameOpDialog);
     } else
	  report_user_error(LG_STR("you have to select a OP, before renaming it.",
				   "you have to select a OP, before renaming it."));
#endif
}

void OpCopy(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     if (current_op != NULL) {
	  XmTextSetString(XmSelectionBoxGetChild(copyOpDialog,XmDIALOG_TEXT), current_op->name);
	  XtManageChild(copyOpDialog);
     } else
	  report_user_error(LG_STR("You have to select a OP, before copying it.",
				   "You have to select a OP, before copying it."));
#endif
}

void OpPaste(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     pasteOpDialogManage();
#endif
}

void OpClear(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     clear_buffer_opfile();
#endif
}

void OpPrevious(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     select_previous_op_in_same_file(dd);
#endif
}

void OpNext(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     select_next_op_in_same_file(dd);
#endif
}

void OpToggle_selected(Widget w, Draw_Data *dd, XtPointer call_data)
{
#ifdef IGNORE_GTK
     toggle_last_selected_ops(dd);
#endif
}

void OpLast_selected(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef IGNORE_GTK
     last_selectedOpDialogManage();
#endif
}

GtkWidget *opRedraw, *opDestroy, *opDump, *opPrint, *opDuplicate, *opRename, *opCopy;

GtkToolItem *moveObjects, *createNode, *createIfNode, *createEdge, *createKnot, 
  *editObjects, *duplicateObjects, *mergeNode, *convertEnd, *convertStart, 
  *relevantOp, *alignObjects, *alignObjectsVert, *alignObjectsHor, 
  *destroyObjects, *flipJoin, *flipSplit, *openNode;

/* Widget WMmoveObjects, WMcreateNode, WMcreateIfNode, WMcreateEdge, WMcreateKnot, WMeditObjects; */
/* Widget WMduplicateObjects, WMmergeNode, WMconvertEnd, WMconvertStart, WMrelevantOp; */
/* Widget WMalignObjects, WMalignObjectsVert, WMalignObjectsHor, WMdestroyObjects; */
/* Widget WMflipJoin, WMflipSplit, WMopenNode; */

GtkWidget *miscVisibleFields, *miscSymbolList;

void update_select_sensitivity(PBoolean sensible)
{
#ifdef IGNORE
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
#endif
}

GtkWidget *opPaste, *opClear;
PBoolean buffer_sensitivity;

void update_buffer_sensitivity(PBoolean sensible)
{
#ifdef IGNORE
     XtSetSensitive(opPaste, sensible);
     XtSetSensitive(opClear, sensible);
     buffer_sensitivity = sensible;
#endif
}

GtkWidget *opToggle_selected, *opLast_selected;

void update_toggle_sensitivity(PBoolean sensible)
{
#ifdef IGNORE
     XtSetSensitive(opToggle_selected, sensible);
     XtSetSensitive(opLast_selected, sensible);
#endif
}

GtkWidget *opSelectDestroy, *opPrevious, *opNext;

void update_empty_sensitivity(PBoolean sensible)
{
#ifdef IGNORE  
     XtSetSensitive(opSelectDestroy, sensible);
     XtSetSensitive(opPrevious, sensible);
     XtSetSensitive(opNext, sensible);
#endif
}

GtkWidget *opCreate;
GtkWidget *opfWriteTex, *opfAppend, *opfSave, *opfWrite;

void update_file_sensitivity(PBoolean sensible)
{
#ifdef IGNORE
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
#endif
}

GtkWidget *WMPDMenu;
GtkWidget *opPDMenu;

GtkWidget *create_menu_bar(GtkWidget *window, Draw_Data *dd)
{
  GtkWidget *menuBar;
  GtkWidget *filePDMenu, *editPDMenu, *miscPDMenu, *helpPDMenu;
  GtkWidget *fileCButton, *editCButton, *opCButton, *miscCButton, *helpCButton, *WMCButton;
  
  GtkWidget  *opfLoad, *opfNew, *opfPrint, *opfSelect, *opfUnload, *opfQuit;
  
  GtkWidget  *miscSize, *miscPrint;
  GtkWidget  *help;
  //, file_help, edit_help, 
  //    op_help,  misc_help, work_help;
  
  GtkAccelGroup *accel_group = NULL;

  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  menuBar =  gtk_menu_bar_new();

  filePDMenu = gtk_menu_new();
  editPDMenu = gtk_menu_new();
  opPDMenu = gtk_menu_new();
  miscPDMenu = gtk_menu_new();
  /* WMPDMenu = gtk_menu_new(); */
  helpPDMenu = gtk_menu_new();

  fileCButton = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileCButton), filePDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileCButton);


  editCButton = gtk_menu_item_new_with_label("Edit");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(editCButton), editPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), editCButton);


  opCButton = gtk_menu_item_new_with_label("Op");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(opCButton), opPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), opCButton);


  miscCButton = gtk_menu_item_new_with_label("Misc");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(miscCButton), miscPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), miscCButton);


  /* WMCButton = gtk_menu_item_new_with_label("WM"); */
  /* gtk_menu_item_set_submenu(GTK_MENU_ITEM(WMCButton), WMPDMenu); */
  /* gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), WMCButton); */


  helpCButton = gtk_menu_item_new_with_label("Help");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpCButton), helpPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), helpCButton);



  /* Now he menu items... */

  /* File Menu */

  opfLoad = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfLoad);
  g_signal_connect(G_OBJECT(opfLoad), "activate",
		   G_CALLBACK(OpfLoad), dd);

  opfAppend =  gtk_menu_item_new_with_label("Append");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfAppend);
  g_signal_connect(G_OBJECT(opfAppend), "activate",
		   G_CALLBACK(OpfAppend), NULL);

  opfNew =  gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfNew);
  g_signal_connect(G_OBJECT(opfNew), "activate",
		   G_CALLBACK(OpfNew), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  opfSave =  gtk_menu_item_new_with_label("Save");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfSave);
  g_signal_connect(G_OBJECT(opfSave), "activate",
		   G_CALLBACK(OpfSave), NULL);

  opfWrite =  gtk_menu_item_new_with_label("Write");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfWrite);
  g_signal_connect(G_OBJECT(opfWrite), "activate",
		   G_CALLBACK(OpfWrite), NULL);

  opfWriteTex =  gtk_menu_item_new_with_label("WriteTex");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),opfWriteTex);
  g_signal_connect(G_OBJECT(opfWriteTex), "activate",
		   G_CALLBACK(OpfWriteTex), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  opfQuit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
  gtk_widget_add_accelerator(opfQuit, "activate", accel_group, 
			     GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), opfQuit);
  
  g_signal_connect(G_OBJECT(opfQuit), "activate",
		   G_CALLBACK(quitQuestionManage),(gpointer) window);

  /* Edit Menu */

  opfSelect = gtk_menu_item_new_with_label("Select");
  gtk_menu_shell_append(GTK_MENU_SHELL(editPDMenu), opfSelect);
  g_signal_connect(G_OBJECT(opfSelect),"activate",
		   G_CALLBACK(OpfSelect), NULL);

  /* opfPrint = gtk_menu_item_new_with_label("Print"); */
  /* gtk_menu_shell_append(GTK_MENU_SHELL(editPDMenu), opfPrint); */
  /* g_signal_connect(G_OBJECT(opfPrint),"activate", */
  /* 		   G_CALLBACK(OpfPrint), NULL); */

  opfUnload = gtk_menu_item_new_with_label("Unload");
  gtk_menu_shell_append(GTK_MENU_SHELL(editPDMenu), opfUnload);
  g_signal_connect(G_OBJECT(opfUnload),"activate",
		   G_CALLBACK(OpfUnload), NULL);

  /* OP menu */

  opCreate = gtk_menu_item_new_with_label("Create");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opCreate);
  g_signal_connect(G_OBJECT(opCreate),"activate",
		   G_CALLBACK(OpCreate), NULL);

  opDuplicate = gtk_menu_item_new_with_label("Duplicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opDuplicate);
  g_signal_connect(G_OBJECT(opDuplicate),"activate",
		   G_CALLBACK(OpDuplicate), NULL);

  opRename = gtk_menu_item_new_with_label("Rename");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opRename);
  g_signal_connect(G_OBJECT(opRename),"activate",
		   G_CALLBACK(OpRename), NULL);

  opRedraw = gtk_menu_item_new_with_label("Redraw");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opRedraw);
  g_signal_connect(G_OBJECT(opRedraw),"activate",
		   G_CALLBACK(OpRedraw), NULL);
     
  opDestroy = gtk_menu_item_new_with_label("Destroy");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opDestroy);
  g_signal_connect(G_OBJECT(opDestroy),"activate",
		   G_CALLBACK(OpDestroy), NULL);
     
  opSelectDestroy = gtk_menu_item_new_with_label("SelectDestroy");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opSelectDestroy);
  g_signal_connect(G_OBJECT(opSelectDestroy),"activate",
		   G_CALLBACK(OpSelectDestroy), NULL);
     
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), gtk_separator_menu_item_new());

  opDump = gtk_menu_item_new_with_label("Dump");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opDump);
  g_signal_connect(G_OBJECT(opDump),"activate",
		   G_CALLBACK(OpDump), NULL);
     
  /* opPrint = gtk_menu_item_new_with_label("Print"); */
  /* gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opPrint); */
  /* g_signal_connect(G_OBJECT(opPrint),"activate", */
  /* 		   G_CALLBACK(OpPrint), NULL); */

  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), gtk_separator_menu_item_new());

  opPrevious = gtk_menu_item_new_with_label("Previous");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opPrevious);
  g_signal_connect(G_OBJECT(opPrevious),"activate",
		   G_CALLBACK(OpPrevious), NULL);

  opNext = gtk_menu_item_new_with_label("Next");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opNext);
  g_signal_connect(G_OBJECT(opNext),"activate",
		   G_CALLBACK(OpNext), NULL);

  opToggle_selected = gtk_menu_item_new_with_label("Toggle_selected");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opToggle_selected);
  g_signal_connect(G_OBJECT(opToggle_selected),"activate",
		   G_CALLBACK(OpToggle_selected), NULL);

  opLast_selected = gtk_menu_item_new_with_label("Last_selected");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opLast_selected);
  g_signal_connect(G_OBJECT(opLast_selected),"activate",
		   G_CALLBACK(OpLast_selected), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), gtk_separator_menu_item_new());

  opCopy = gtk_menu_item_new_with_label("Copy");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opCopy);
  g_signal_connect(G_OBJECT(opCopy),"activate",
		   G_CALLBACK(OpCopy), NULL);

  opPaste = gtk_menu_item_new_with_label("Paste");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opPaste);
  g_signal_connect(G_OBJECT(opPaste),"activate",
		   G_CALLBACK(OpPaste), NULL);

  opClear = gtk_menu_item_new_with_label("Clear");
  gtk_menu_shell_append(GTK_MENU_SHELL(opPDMenu), opClear);
  g_signal_connect(G_OBJECT(opClear),"activate",
		   G_CALLBACK(OpClear), NULL);

  /* Misc Menu */

  miscSymbolList = gtk_menu_item_new_with_label("SymbolList");
  gtk_menu_shell_append(GTK_MENU_SHELL(miscPDMenu), miscSymbolList);
  g_signal_connect(G_OBJECT(miscSymbolList),"activate",
		   G_CALLBACK(MiscSymbolList), NULL);

  miscSize = gtk_menu_item_new_with_label("Size");
  gtk_menu_shell_append(GTK_MENU_SHELL(miscPDMenu), miscSize);
  g_signal_connect(G_OBJECT(miscSize),"activate",
		   G_CALLBACK(MiscSize), NULL);

  /* miscPrint = gtk_menu_item_new_with_label("Print"); */
  /* gtk_menu_shell_append(GTK_MENU_SHELL(miscPDMenu), miscPrint); */
  /* g_signal_connect(G_OBJECT(miscPrint),"activate", */
  /* 		   G_CALLBACK(MiscPrint), NULL); */

  miscVisibleFields = gtk_menu_item_new_with_label("VisibleFields");
  gtk_menu_shell_append(GTK_MENU_SHELL(miscPDMenu), miscVisibleFields);
  g_signal_connect(G_OBJECT(miscVisibleFields),"activate",
		   G_CALLBACK(MiscVisibleFields), NULL);



  /* Help menu items */
  help = gtk_menu_item_new_with_label("help");
  gtk_menu_shell_append(GTK_MENU_SHELL(helpPDMenu), help);
  g_signal_connect(G_OBJECT(help),"activate",
		   G_CALLBACK(infoHelp), NULL);

  return menuBar;

}

GtkWidget *toggled_button = NULL; 

void toggle_button(GtkWidget *w)
{
#ifdef IGNORE_GTK
     Pixel back,fore;
     Arg args[2];
     Cardinal n;

     if (toggled_button) {
	  GtkWidget *widget = toggled_button;
	  
	  toggled_button = NULL;
	  toggle_button(widget);
     }

     toggled_button = w;


     /* n = 0; */
     /* XtSetArg(args[n], XmNbackground, &back); n++; */
     /* XtSetArg(args[n], XmNforeground, &fore); n++; */
     /* XtGetValues(w,args,n); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNbackground, fore); n++; */
     /* XtSetArg(args[n], XmNforeground, back); n++; */
     /* XtSetValues(w,args,n); */
     
#endif
}

void CreateNode(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(createNode);
  set_draw_mode_from_menu(dd, dd->cgcsp, DRAW_NODE);
}

void CreateIfNode(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(createIfNode);
     set_draw_mode_from_menu(dd, dd->cgcsp, DRAW_IFNODE);
}

void FlipJoin(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(flipJoin);
     set_draw_mode_from_menu(dd, dd->cgcsp, FLIP_JOIN);
}

void OpenNode(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(openNode);
     set_draw_mode_from_menu(dd, dd->cgcsp, OPEN_NODE);
}

void FlipSplit(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(flipSplit);
     set_draw_mode_from_menu(dd, dd->cgcsp, FLIP_SPLIT);
}

void EditObjects(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(editObjects);
     set_draw_mode_from_menu(dd, dd->cgcsp, EDIT_OG);
}

void MoveObjects(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(moveObjects);
     set_draw_mode_from_menu(dd, dd->cgcsp, MOVE_OG);
}

void RelevantOp(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(relevantOp);
     set_draw_mode_from_menu(dd, dd->cgcsp, DM_RELEVANT_OP);
}

void ConvertEnd(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(convertEnd);
     set_draw_mode_from_menu(dd, dd->cgcsp, CONVERT_END);
}

void ConvertStart(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(convertStart);
     set_draw_mode_from_menu(dd, dd->cgcsp, CONVERT_START);
}

void AlignObjects(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(alignObjects);
     set_draw_mode_from_menu(dd, dd->cgcsp, ALIGN_OG);
}

void AlignObjectsVert(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(alignObjectsVert);
     set_draw_mode_from_menu(dd, dd->cgcsp, ALIGN_OG_V);
}

void AlignObjectsHor(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(alignObjectsHor);
     set_draw_mode_from_menu(dd, dd->cgcsp, ALIGN_OG_H);
}

void DestroyObjects(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(destroyObjects);
     set_draw_mode_from_menu(dd, dd->cgcsp, DESTROY_OG);
}

void CreateEdge(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(createEdge);
     set_draw_mode_from_menu(dd, dd->cgcsp, DRAW_EDGE);
}

void CreateKnot(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(createKnot);
     set_draw_mode_from_menu(dd, dd->cgcsp, DRAW_KNOT);
}

void DuplicateObjects(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(duplicateObjects);
     set_draw_mode_from_menu(dd, dd->cgcsp, DUPLICATE_OBJECTS);
}

void MergeNode(GtkWidget *w, Draw_Data *dd, XtPointer call_data)
{
  //     toggle_button(mergeNode);
     set_draw_mode_from_menu(dd, dd->cgcsp, MERGE_NODE);
}

GtkWidget *create_tool_bar(GtkWidget *parent, Draw_Data *dd)
{
  GtkWidget *toolbar;

#ifdef OPE_ADD_MSTATS_BUTTON
  Widget debug;
#endif /* OPE_ADD_MSTATS_BUTTON */
  
  toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  
  gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);

  moveObjects = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "moveObjects", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), moveObjects, -1);
   g_signal_connect(G_OBJECT(moveObjects),"clicked",
		   G_CALLBACK(MoveObjects), dd);
  //  toggle_button(moveObjects);

  createNode = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "createNode", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), createNode, -1);


  createIfNode = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "createIfNode", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), createIfNode, -1);

  openNode = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "openNode", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), openNode, -1);

  createEdge = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "createEdge", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), createEdge, -1);

  createKnot = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "createKnot", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), createKnot, -1);

  flipJoin = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "flipJoin", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), flipJoin, -1);

  flipSplit = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "flipSplit", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), flipSplit, -1);

  editObjects = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "editObjects", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), editObjects, -1);

  duplicateObjects = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "duplicateObjects", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), duplicateObjects, -1);

  mergeNode = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "mergeNode", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), mergeNode, -1);

  convertEnd = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "convertEnd", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), convertEnd, -1);

  convertStart = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "convertStart", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), convertStart, -1);

  alignObjects = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "alignObjects", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), alignObjects, -1);

  alignObjectsVert = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "alignObjectsVert", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), alignObjectsVert, -1);

  alignObjectsHor = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "alignObjectsHor", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), alignObjectsHor, -1);

  destroyObjects = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "destroyObjects", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), destroyObjects, -1);

  relevantOp = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); // "relevantOp", NULL, 0);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), relevantOp, -1);

  return toolbar;
}
