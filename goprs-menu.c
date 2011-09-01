/*                               -*- Mode: C -*- 
 * goprs-menu.c -- 
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
#include <gdk/gdkkeysyms.h>

#include "xm2gtk.h"

#include "constant.h"
#include "lang.h"
#include "oprs-type.h"
#include "op-structure.h"

#include "gope-graphic.h"

#include "xhelp.h"
#include "xhelp_f.h"
#include "oprs.h"
#include "top-lev.h"
#include "gtop-lev_f.h"
#include "goprs.h"
#include "oprs-main.h"
#include "goprs-main.h"
#include "goprs-intention.h"

#include "xm2gtk_f.h"

/*
 * quit button callback function
 */
void Quit(Widget w, XtPointer client_data, XtPointer call_data)
{ 
      quitQuestionManage();
}

void Xp_resetdb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("empty fact db\n");
}

void Xp_resetop(Widget w, Draw_Data *dd)
{ 
  gdk_window_clear_area_e(dd->window, dd->left, dd->top , dd->work_width, dd->work_height);
  dd->op = NULL;
  send_command_to_parser("empty op db\n");
}

void Xp_addFactGoal(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(addFactGoalDialog, addfactGoalEntry, "add %s\n");
}

void Xp_consultDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(consultDBDialog, consultDBEntry, "consult %s\n");
}

void Xp_concludeDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(concludeDBDialog, concludeDBEntry, "conclude %s\n");
}

void Xp_deleteDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(deleteDBDialog, deleteDBEntry, "delete %s\n");
}

void Xp_consultOP(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(consultOPDialog, consultOPEntry, "consult relevant op %s\n");
}

void Xp_consultAOP(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  show_dialog_with_combo_box_entry(consultAOPDialog, consultAOPEntry, "consult applicable op %s\n");
}

void Xp_showDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  send_command_to_parser("show db\n");
  //gtk xpShowDBDialogManage();

}

void Xp_showGV(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("show variable\n");
}

void Xp_showSI(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  send_command_to_parser("show intention\n");
  //gtk xpShowIGDialogManage();
}

void Xp_showSC(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  send_command_to_parser("show condition\n");
  //gtk xpShowCondDialogManage();
}

void Xp_listALL(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list all\n");
}

void Xp_listPredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list predicate\n");
}

void Xp_listEvaluablePredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list evaluable predicate\n");
}

void Xp_listCWPredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list cwp\n");
}

void Xp_listFFPredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list ff\n");
}

void Xp_listBEPredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list be\n");
}

void Xp_listOPPredicate(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list op_predicate\n");
}

void Xp_listEvaluableFunction(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list evaluable function\n");
}

void Xp_listFunction(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list function\n");
}

void Xp_listAction(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list action\n");
}

void Xp_statDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("stat db\n");
}

void Xp_statID(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("stat id\n");
}

void Xp_statALL(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("stat all\n");
}

void Xp_loaddb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_loaddbFileselok(xp_loaddbFilesel);
}

void Xp_dumpdb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_dumpdbFileselok(xp_dumpdbFilesel);
}

void Xp_dumpop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_dumpopFileselok(xp_dumpopFilesel);
}

void Xp_dumpAllOpf(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("dump all opf\n");
}

void Xp_dumpkrn(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_dumpkrnFileselok(xp_dumpkrnFilesel);
}

void Xp_savedb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_savedbFileselok(xp_savedbFilesel);
}

void Xp_include(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_includeFileselok(xp_includeFilesel);
}

void Xp_loadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_loadopFileselok(xp_loadopFilesel);
}

void Xp_loadkrn(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_loadkrnFileselok(xp_loadkrnFilesel);
}

void Xp_loaddop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_loaddopFileselok(xp_loaddopFilesel);
}

void Xp_loadddb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xp_loadddbFileselok(xp_loadddbFilesel);
}

void Xp_unloadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpUnloadOpaDialogManage();     
}

void Xp_reloadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpReloadOpDialogManage();     
}

void Xp_listopfs(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list opf\n");
}

void Xp_listops(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list op\n");
}

void Xp_listvops(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("list variable op\n");
}

/*
 * Reset_Oprs button callback function
 */
void Reset_Oprs(Widget w, XtPointer client_data, XtPointer call_data)
{ 
/*      send_command_to_selected_oprs("\n.\n"); */
}

/*
 * oprstrace button callback function
 */
void Oprstrace(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  xpTraceDialogShow(); 
}

/*
 * Optrace button callback function
 */
void Optrace(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpTraceOpDialogManage();
}

/*
 * metaoption button callback function
 */
void Meta_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpMetaOptionDialogManage();
}

/*
 * metaoption button callback function
 */
void User_trace(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpUserTraceDialogManage();
}

#ifdef OPRS_PROFILING
/*
 * profilingoption button callback function
 */
void Profiling_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpProfilingOptionDialogManage();
}
#endif

/*
 * compileroption button callback function
 */
void Compiler_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpCompilerOptionDialogManage();
}

/*
 * run_option button callback function
 */
void Run_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk xpRunOptionDialogManage();
}

/*
 * Opoption button callback function
 */
void Opoption(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     printf(LG_STR("OP option...\n",
		   "OP option...\n"));
}

/*
 * cleartextdisplay button callback function
 */
void Cleartextdisplay(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk ClearTextWindow(textWindow);
}

void Showmem(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("show memory\n");
}

#ifdef OPRS_PROFILING
void ShowProfiling(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("show profiling");
}

void ResetProfiling(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("reset profiling");
}
#endif

/*
 * changemaxtextsize button callback function
 */
void Changemaxtextsize(Widget w, XtPointer client_data, XtPointer call_data)
{ 
  //gtk changeMaxSizeDialogManage ();
}

/*
 * Clearopdisplay button callback function
 */
void Clearopdisplay(Widget w, Draw_Data *dd)
{ 
     clear_dd_window(dd);
     dd->op = NULL;
}

/*
 * Clearigdisplay button callback function
 */
void Clearigdisplay(Widget w, Int_Draw_Data *idd)
{ 
  gdk_window_clear_area_e(idd->window, idd->left, idd->top , idd->work_width, idd->work_height);
}

/*
 * Displayop button callback function
 */
void Displayop(Widget w,  XtPointer ignore, XtPointer call_data)
{ 
  //gtk xpDisplaySelectOpDialogManage();     
}

void DisplayNextOp(Widget w, Draw_Data *dd, XtPointer call_data)
{ 
  xpDisplayNextOp(dd);
}

void DisplayPreviousOp(Widget w, Draw_Data *dd, XtPointer call_data)
{ 
  xpDisplayPreviousOp(dd);
}

/*
 * DeleteOP button callback function
 */
void DeleteOP(Widget w,  XtPointer ignore, XtPointer call_data)
{ 
  //gtk xpDeleteSelectOpDialogManage();     
}

GtkWidget *goprs_create_menu_bar(GtkWidget *window, Draw_Data *dd, Int_Draw_Data *idd)
{

  Widget fileCButton, filePDMenu, quit, include;
  Widget oprsCButton, oprsPDMenu;
  Widget inspectCButton, inspectPDMenu;
  Widget listCButton, listPDMenu;
  Widget traceCButton, tracePDMenu;
  Widget optionCButton, optionPDMenu;
  Widget displayCButton, displayPDMenu;
  Widget helpCButton, helpPDMenu;
  Widget menuBar;

  Widget loaddb,loadddb, savedb, dumpdb, dumpAllOpf, dumpop, dumpkrn, loadop, loadkrn, loaddop, unloadop, 
    reloadop, listops, listvops, listopfs;
  Widget resetdb, resetop, addFactGoal, consultDB, deleteOP, showDB, showSI, showSC;
  Widget concludeDB, deleteDB, showMem;
  Widget listALL, statDB, statALL, statID, showGV;
  Widget listPredicate, listEvaluablePredicate, listCWPredicate, listFFPredicate,
    listOPPredicate, listBEPredicate;
  Widget listFunction, listEvaluableFunction, listAction;
  Widget consultAOP, consultOP;
  Widget oprstrace, optrace;
  /* , opgtrace; */
  Widget compiler_option, run_option, meta_option, user_trace;
#ifdef OPRS_PROFILING
  Widget profiling_option, showProfiling, resetProfiling;
#endif
  Widget cleartextdisplay, clearopdisplay, clearigdisplay, displayop, displayNextOp, displayPreviousOp;
  Widget help, file_help, oprs_help, inspect_help, trace_help, option_help, display_help;
  Widget  changemaxtextsize;
     
  GtkAccelGroup *accel_group = NULL;
     
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  menuBar =  gtk_menu_bar_new();

  filePDMenu = gtk_menu_new();
  oprsPDMenu = gtk_menu_new();
  inspectPDMenu = gtk_menu_new();
  listPDMenu = gtk_menu_new();
  tracePDMenu = gtk_menu_new();
  optionPDMenu = gtk_menu_new();
  displayPDMenu = gtk_menu_new();
  helpPDMenu = gtk_menu_new();

  fileCButton = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileCButton), filePDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileCButton);


  oprsCButton = gtk_menu_item_new_with_label("Oprs");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(oprsCButton), oprsPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), oprsCButton);

  inspectCButton = gtk_menu_item_new_with_label("Inspect");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(inspectCButton), inspectPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), inspectCButton);

  traceCButton = gtk_menu_item_new_with_label("Trace");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(traceCButton), tracePDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), traceCButton);

  optionCButton = gtk_menu_item_new_with_label("Option");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(optionCButton), optionPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), optionCButton);

  displayCButton = gtk_menu_item_new_with_label("Display");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(displayCButton), displayPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), displayCButton);

  helpCButton = gtk_menu_item_new_with_label("Help");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpCButton), helpPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), helpCButton);

  /* File menu items */
  include = gtk_menu_item_new_with_label("Load Commands");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),include);
  g_signal_connect(G_OBJECT(include), "activate", G_CALLBACK(Xp_include), NULL);

  loaddb = gtk_menu_item_new_with_label("load Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),loaddb);
  g_signal_connect(G_OBJECT(loaddb), "activate", G_CALLBACK(Xp_loaddb), NULL);

  if (dev_env) {
    loadop = gtk_menu_item_new_with_label("Load OP File");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),loadop);
    g_signal_connect(G_OBJECT(loadop), "activate", G_CALLBACK(Xp_loadop), NULL);
  }

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  loadddb = gtk_menu_item_new_with_label("Load Dumped Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),loadddb);
  g_signal_connect(G_OBJECT(loadddb), "activate", G_CALLBACK(Xp_loadddb), NULL);

  loaddop = gtk_menu_item_new_with_label("Load Dumped OP File");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),loaddop);
  g_signal_connect(G_OBJECT(loaddop), "activate", G_CALLBACK(Xp_loaddop), NULL);

  loadkrn = gtk_menu_item_new_with_label("Load Dumped Kernel");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),loadkrn);
  g_signal_connect(G_OBJECT(loadkrn), "activate", G_CALLBACK(Xp_loadkrn), NULL);

  if (dev_env) {
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

    listopfs = gtk_menu_item_new_with_label("List OP Files");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),listopfs);
    g_signal_connect(G_OBJECT(listopfs), "activate", G_CALLBACK(Xp_listopfs), NULL);

    reloadop = gtk_menu_item_new_with_label("Reload OP File");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),reloadop);
    g_signal_connect(G_OBJECT(reloadop), "activate", G_CALLBACK(Xp_reloadop), NULL);

    unloadop = gtk_menu_item_new_with_label("Unload OP File");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),unloadop);
    g_signal_connect(G_OBJECT(unloadop), "activate", G_CALLBACK(Xp_unloadop), NULL);
  }

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  savedb = gtk_menu_item_new_with_label("Save Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),savedb);
  g_signal_connect(G_OBJECT(savedb), "activate", G_CALLBACK(Xp_savedb), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  if (dev_env) {
    dumpdb = gtk_menu_item_new_with_label("Dump Database");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),dumpdb);
    g_signal_connect(G_OBJECT(dumpdb), "activate", G_CALLBACK(Xp_dumpdb), NULL);

    dumpAllOpf = gtk_menu_item_new_with_label("Dump All OPs");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),dumpAllOpf);
    g_signal_connect(G_OBJECT(dumpAllOpf), "activate", G_CALLBACK(Xp_dumpAllOpf), NULL);

    dumpop = gtk_menu_item_new_with_label("Dump One OP File");
    gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),dumpop);
    g_signal_connect(G_OBJECT(dumpop), "activate", G_CALLBACK(Xp_dumpop), NULL);
  }

  dumpkrn = gtk_menu_item_new_with_label("Dump the Kernel");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),dumpkrn);
  g_signal_connect(G_OBJECT(dumpkrn), "activate", G_CALLBACK(Xp_dumpkrn), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu), gtk_separator_menu_item_new());

  quit = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(filePDMenu),quit);
  g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(Quit), NULL);
  gtk_widget_add_accelerator (quit, "activate", accel_group,
			      GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
     
  /* Oprs menu items */

  addFactGoal = gtk_menu_item_new_with_label("Add a Fact or a Goal");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), addFactGoal);
  g_signal_connect(G_OBJECT(addFactGoal), "activate", G_CALLBACK(Xp_addFactGoal), NULL);
  gtk_widget_add_accelerator (addFactGoal, "activate", accel_group,
			      GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  concludeDB = gtk_menu_item_new_with_label("Assert in the Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), concludeDB);
  g_signal_connect(G_OBJECT(concludeDB), "activate", G_CALLBACK(Xp_concludeDB), NULL);

  deleteDB = gtk_menu_item_new_with_label("Delete Fact(s) in the Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), deleteDB);
  g_signal_connect(G_OBJECT(deleteDB), "activate", G_CALLBACK(Xp_deleteDB), NULL);

  deleteOP = gtk_menu_item_new_with_label("Delete an OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), deleteOP);
  g_signal_connect(G_OBJECT(deleteOP), "activate", G_CALLBACK(DeleteOP), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), gtk_separator_menu_item_new());

  resetdb = gtk_menu_item_new_with_label("Empty Fact Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), resetdb);
  g_signal_connect(G_OBJECT(resetdb), "activate", G_CALLBACK(Xp_resetdb), NULL);

  resetop = gtk_menu_item_new_with_label("Empty OP Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(oprsPDMenu), resetop);
  g_signal_connect(G_OBJECT(resetop), "activate", G_CALLBACK(Xp_resetop), dd);

  /* Inspect Menu Item */

  showDB = gtk_menu_item_new_with_label("Show Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),showDB);
  g_signal_connect(G_OBJECT(showDB), "activate", G_CALLBACK(Xp_showDB), NULL);

  showGV = gtk_menu_item_new_with_label("Show Global Varaibles");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),showGV);
  g_signal_connect(G_OBJECT(showGV), "activate", G_CALLBACK(Xp_showGV), NULL);

  showSI = gtk_menu_item_new_with_label("Show Intention Graph");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),showSI);
  g_signal_connect(G_OBJECT(showSI), "activate", G_CALLBACK(Xp_showSI), NULL);

  showSC = gtk_menu_item_new_with_label("Show Conditions");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),showSC);
  g_signal_connect(G_OBJECT(showSC), "activate", G_CALLBACK(Xp_showSC), NULL);

  showMem = gtk_menu_item_new_with_label("Show Memory Usage");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),showMem);
  g_signal_connect(G_OBJECT(showMem), "activate", G_CALLBACK(Showmem), NULL);

  consultDB = gtk_menu_item_new_with_label("Consult Database");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),consultDB);
  g_signal_connect(G_OBJECT(consultDB), "activate", G_CALLBACK(Xp_consultDB), NULL);

  consultOP = gtk_menu_item_new_with_label("Consult Relevant OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),consultOP);
  g_signal_connect(G_OBJECT(consultOP), "activate", G_CALLBACK(Xp_consultOP), NULL);

  consultAOP = gtk_menu_item_new_with_label("Consult Applicable OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),consultAOP);
  g_signal_connect(G_OBJECT(consultAOP), "activate", G_CALLBACK(Xp_consultAOP), NULL);

  listALL = gtk_menu_item_new_with_label("List ALL");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),listALL);
  g_signal_connect(G_OBJECT(listALL), "activate", G_CALLBACK(Xp_listALL), NULL);


  listCButton = gtk_menu_item_new_with_label("List...");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(listCButton), listPDMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu), listCButton);
     
  listops = gtk_menu_item_new_with_label("List OPs");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),listops);
  g_signal_connect(G_OBJECT(listops), "activate", G_CALLBACK(Xp_listops), NULL);

  listvops = gtk_menu_item_new_with_label("List Variable OPs");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),listvops);
  g_signal_connect(G_OBJECT(listvops), "activate", G_CALLBACK(Xp_listvops), NULL);

  statDB = gtk_menu_item_new_with_label("Stat Database Hashtable");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),statDB);
  g_signal_connect(G_OBJECT(statDB), "activate", G_CALLBACK(Xp_statDB), NULL);

  statID = gtk_menu_item_new_with_label("Stat Id Hashtable");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),statID);
  g_signal_connect(G_OBJECT(statID), "activate", G_CALLBACK(Xp_statID), NULL);

  statALL = gtk_menu_item_new_with_label("Stat All Hashtables");
  gtk_menu_shell_append(GTK_MENU_SHELL(inspectPDMenu),statALL);
  g_signal_connect(G_OBJECT(statALL), "activate", G_CALLBACK(Xp_statALL), NULL);

  /* list submenus */

  listPredicate = gtk_menu_item_new_with_label("List Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listPredicate);
  g_signal_connect(G_OBJECT(listPredicate), "activate", G_CALLBACK(Xp_listPredicate), NULL);

  listEvaluablePredicate = gtk_menu_item_new_with_label("List Evaluable Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listEvaluablePredicate);
  g_signal_connect(G_OBJECT(listEvaluablePredicate), "activate", G_CALLBACK(Xp_listEvaluablePredicate), NULL);

  listCWPredicate = gtk_menu_item_new_with_label("List Closed World Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listCWPredicate);
  g_signal_connect(G_OBJECT(listCWPredicate), "activate", G_CALLBACK(Xp_listCWPredicate), NULL);

  listFFPredicate = gtk_menu_item_new_with_label("List Functional Fact Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listFFPredicate);
  g_signal_connect(G_OBJECT(listFFPredicate), "activate", G_CALLBACK(Xp_listFFPredicate), NULL);

  listBEPredicate = gtk_menu_item_new_with_label("List Basic Event Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listBEPredicate);
  g_signal_connect(G_OBJECT(listBEPredicate), "activate", G_CALLBACK(Xp_listBEPredicate), NULL);

  listOPPredicate = gtk_menu_item_new_with_label("List OP Predicate");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listOPPredicate);
  g_signal_connect(G_OBJECT(listOPPredicate), "activate", G_CALLBACK(Xp_listOPPredicate), NULL);

  listFunction = gtk_menu_item_new_with_label("List Function");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listFunction);
  g_signal_connect(G_OBJECT(listFunction), "activate", G_CALLBACK(Xp_listFunction), NULL);

  listEvaluableFunction = gtk_menu_item_new_with_label("List Evaluable Function");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listEvaluableFunction);
  g_signal_connect(G_OBJECT(listEvaluableFunction), "activate", G_CALLBACK(Xp_listEvaluableFunction), NULL);

  listAction = gtk_menu_item_new_with_label("List Action");
  gtk_menu_shell_append(GTK_MENU_SHELL(listPDMenu),listAction);
  g_signal_connect(G_OBJECT(listAction), "activate", G_CALLBACK(Xp_listAction), NULL);

  /* Trace menu items */

  oprstrace = gtk_menu_item_new_with_label("oprstrace");
  gtk_menu_shell_append(GTK_MENU_SHELL(tracePDMenu),oprstrace);
  g_signal_connect(G_OBJECT(oprstrace), "activate", G_CALLBACK(Oprstrace), NULL);

  optrace = gtk_menu_item_new_with_label("optrace");
  gtk_menu_shell_append(GTK_MENU_SHELL(tracePDMenu),optrace);
  g_signal_connect(G_OBJECT(optrace), "activate", G_CALLBACK(Optrace), NULL);

  if (install_user_trace) {
    user_trace = gtk_menu_item_new_with_label("user_trace");
    gtk_menu_shell_append(GTK_MENU_SHELL(tracePDMenu),user_trace);
    g_signal_connect(G_OBJECT(user_trace), "activate", G_CALLBACK(User_trace), NULL);
  }

  /* Option menu items */

  run_option = gtk_menu_item_new_with_label("Run Option");
  gtk_menu_shell_append(GTK_MENU_SHELL(optionPDMenu),run_option);
  g_signal_connect(G_OBJECT(run_option), "activate", G_CALLBACK(Run_option), NULL);

  compiler_option = gtk_menu_item_new_with_label("Ompiler Option");
  gtk_menu_shell_append(GTK_MENU_SHELL(optionPDMenu),compiler_option);
  g_signal_connect(G_OBJECT(compiler_option), "activate", G_CALLBACK(Compiler_option), NULL);

  meta_option = gtk_menu_item_new_with_label("Meta Option");
  gtk_menu_shell_append(GTK_MENU_SHELL(optionPDMenu),meta_option);
  g_signal_connect(G_OBJECT(meta_option), "activate", G_CALLBACK(Meta_option), NULL);

#ifdef OPRS_PROFILING
  profiling = gtk_menu_item_new_with_label("profiling_option");
  gtk_menu_shell_append(GTK_MENU_SHELL(optionPDMenu),profiling);
  g_signal_connect(G_OBJECT(profiling), "activate", G_CALLBACK(Profiling_option), NULL);
#endif

  /*
    XtSetArg(args[0],XmNsensitive, False);
    opoption = XmCreatePushButtonGadget(optionPDMenu,"opoption", args, 1);
    XtAddCallback(opoption, XmNactivateCallback, Opoption, 0);
    XtManageChild(opoption);
  */   
  /* Display menu items */

  displayop = gtk_menu_item_new_with_label("Display OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),displayop);
  g_signal_connect(G_OBJECT(displayop), "activate", G_CALLBACK(Displayop), NULL);

  displayPreviousOp = gtk_menu_item_new_with_label("Display Previous OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),displayPreviousOp);
  g_signal_connect(G_OBJECT(displayPreviousOp), "activate", G_CALLBACK(DisplayPreviousOp), dd);
  gtk_widget_add_accelerator (displayPreviousOp, "activate", accel_group,
			      GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);


  displayNextOp = gtk_menu_item_new_with_label("Display Next OP");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),displayNextOp);
  g_signal_connect(G_OBJECT(displayNextOp), "activate", G_CALLBACK(DisplayNextOp), dd);
  gtk_widget_add_accelerator (displayNextOp, "activate", accel_group,
			      GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  cleartextdisplay = gtk_menu_item_new_with_label("Clear Text Display");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),cleartextdisplay);
  g_signal_connect(G_OBJECT(cleartextdisplay), "activate", G_CALLBACK(Cleartextdisplay),NULL);

  clearopdisplay = gtk_menu_item_new_with_label("Clear OP Display");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),clearopdisplay);
  g_signal_connect(G_OBJECT(clearopdisplay), "activate", G_CALLBACK(Clearopdisplay), dd);

  clearigdisplay = gtk_menu_item_new_with_label("Clear IG Display");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),clearigdisplay);
  g_signal_connect(G_OBJECT(clearigdisplay), "activate", G_CALLBACK(Clearigdisplay), idd);

  changemaxtextsize = gtk_menu_item_new_with_label("Change Max Text size");
  gtk_menu_shell_append(GTK_MENU_SHELL(displayPDMenu),changemaxtextsize);
  g_signal_connect(G_OBJECT(changemaxtextsize), "activate", G_CALLBACK(Changemaxtextsize), NULL);
  /* Help menu items */
  help = gtk_menu_item_new_with_label("Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(helpPDMenu),help);
  g_signal_connect(G_OBJECT(help), "activate", G_CALLBACK(infoHelp), NULL);

  return menuBar;

}


void set_oprs_active_mode(PBoolean mode)
{
     if (mode) {
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsActiveDButton), "Active");
       unset_button(oprsIdleDButton);
       set_button(oprsActiveDButton);
     } else {
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsActiveDButton), "Idle");
       set_button(oprsIdleDButton);
       unset_button(oprsActiveDButton);
     }
}

void xset_oprs_run_mode(Oprs_Run_Type mode)
{
     switch(mode) {
     case RUN:
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsStoppedDButton), "Runnable");
	  register_main_loop(current_oprs, FALSE);
	  break;
     case STEP_NEXT:
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsStoppedDButton), "Runnable");
       //	  unset_button(oprsStoppedDButton);
	  register_main_loop(current_oprs, FALSE);
	  break;
     case STEP_HALT:
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsStoppedDButton), "Stopped");
       //	  set_button(oprsStoppedDButton);
	  break;
     case HALT:
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsStoppedDButton), "Stopped");
       //  set_button(oprsStoppedDButton);
	  break;
     case STEP:
       gtk_tool_button_set_label(GTK_TOOL_BUTTON(oprsStoppedDButton), "Runnable");
       //	  unset_button(oprsStoppedDButton);
	  break;
     default:
	  fprintf(stderr, LG_STR("xset_oprs_run_mode: unknown run_mode.\n",
				 "xset_oprs_run_mode: unknown run_mode.\n"));
     }
}

void OprsResetButton(Widget w, XtPointer oprs, XtPointer call_data)
{ 
/*     reset_oprs_kernel((Oprs *)oprs); */
     resetQuestionManage();
}

void OprsHaltButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(HALT);
}

void OprsStepButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(STEP);
}

void OprsStepNextButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(STEP_NEXT);
}

void OprsRunButton(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     set_oprs_run_mode(RUN);
}

GtkToolItem *oprsIdleDButton, *oprsStoppedDButton, *oprsActiveDButton;
GtkToolItem *oprsRunButton, *oprsHaltButton, *oprsResetButton, *oprsStepButton, *oprsStepNextButton;

GtkWidget *create_tool_bar(GtkWidget *parent, Draw_Data *dd)
{
  GtkWidget *toolbar;
  
  toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_orientation(GTK_TOOLBAR(toolbar),GTK_ORIENTATION_VERTICAL);
  gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);

  oprsActiveDButton = gtk_tool_button_new(NULL, "Idle");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),oprsActiveDButton, -1);

  /* oprsIdleDButton = gtk_tool_button_new(NULL, "Idle"); */
  /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar),oprsIdleDButton, -1); */

   oprsStoppedDButton = gtk_tool_button_new(NULL, "Runnable"); 
   gtk_toolbar_insert(GTK_TOOLBAR(toolbar),oprsStoppedDButton, -1); 

  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),gtk_separator_tool_item_new(), -1);

  oprsRunButton = gtk_tool_button_new(NULL, "Run");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), oprsRunButton, -1);
  g_signal_connect(G_OBJECT(oprsRunButton),"clicked", G_CALLBACK(OprsRunButton),NULL);

  oprsStepButton = gtk_tool_button_new(NULL, "Step");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), oprsStepButton, -1);
  g_signal_connect(G_OBJECT(oprsStepButton),"clicked", G_CALLBACK(OprsStepButton), NULL);

  oprsStepNextButton = gtk_tool_button_new(NULL, "Next");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), oprsStepNextButton, -1);
  g_signal_connect(G_OBJECT(oprsStepNextButton),"clicked", G_CALLBACK(OprsStepNextButton), NULL);

  oprsHaltButton = gtk_tool_button_new(NULL, "Halt");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), oprsHaltButton, -1);
  g_signal_connect(G_OBJECT(oprsHaltButton),"clicked", G_CALLBACK(OprsHaltButton), NULL);

  gtk_toolbar_insert(GTK_TOOLBAR(toolbar),gtk_separator_tool_item_new(), -1);

  oprsResetButton = gtk_tool_button_new(NULL, "Reset");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), oprsResetButton, -1);
  g_signal_connect(G_OBJECT(oprsResetButton),"clicked", G_CALLBACK(OprsResetButton), NULL);

  return toolbar;

}
