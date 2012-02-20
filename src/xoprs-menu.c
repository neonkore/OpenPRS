/*                               -*- Mode: C -*- 
 * xoprs-menu.c -- 
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
#include <Xm/CascadeB.h>	/* CascadeButton for menubar  */
#include <Xm/PushBG.h>		/* PushButton Gadget for menu button */
#include <Xm/Frame.h>
#include <Xm/Separator.h>

#include "constant.h"
#include "lang.h"
#include "oprs-type.h"

#include "ope-graphic.h"

#include "xhelp.h"
#include "xhelp_f.h"
#include "oprs.h"
#include "xoprs.h"
#include "oprs-main.h"
#include "xoprs-main.h"
#include "xoprs-intention.h"

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

void Xp_resetop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
/*
     XClearWindow(XtDisplay(dd->canvas), dd->window);
     dd->op = NULL;
*/
     send_command_to_parser("empty op db\n");
}

void Xp_addFactGoal(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(addFactGoalDialog);
}

void Xp_consultDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(consultDBDialog);
}

void Xp_concludeDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(concludeDBDialog);
}

void Xp_deleteDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(deleteDBDialog);
}

void Xp_showDB(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     /* send_command_to_parser("show db\n"); */
     xpShowDBDialogManage();

}

void Xp_showGV(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("show variable\n");
}

void Xp_showSI(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     /* send_command_to_parser("show intention\n"); */
     xpShowIGDialogManage();
}

void Xp_showSC(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     /* send_command_to_parser("show condition\n"); */
     xpShowCondDialogManage();
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

void Xp_consultOP(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(consultOPDialog);
}

void Xp_consultAOP(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(consultAOPDialog);
}

void Xp_loaddb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_loaddbFilesel);
}

void Xp_dumpdb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_dumpdbFilesel);
}

void Xp_dumpop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_dumpopFilesel);
}

void Xp_dumpAllOpf(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     send_command_to_parser("dump all opf\n");
}

void Xp_dumpkrn(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_dumpkrnFilesel);
}

void Xp_savedb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_savedbFilesel);
}

void Xp_include(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_includeFilesel);
}

void Xp_loadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_loadopFilesel);
}

void Xp_loadkrn(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_loadkrnFilesel);
}

void Xp_loaddop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_loaddopFilesel);
}

void Xp_loadddb(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     XtManageChild(xp_loadddbFilesel);
}

void Xp_unloadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpUnloadOpDialogManage();     
}

void Xp_reloadop(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpReloadOpDialogManage();     
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
     xpTraceDialogManage(); 
}

/*
 * Optrace button callback function
 */
void Optrace(Widget w, XtPointer client_data, XtPointer call_data)
{ 
      xpTraceOpDialogManage();
}

/*
 * metaoption button callback function
 */
void Meta_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpMetaOptionDialogManage();
}

/*
 * metaoption button callback function
 */
void User_trace(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpUserTraceDialogManage();
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
     xpCompilerOptionDialogManage();
}

/*
 * run_option button callback function
 */
void Run_option(Widget w, XtPointer client_data, XtPointer call_data)
{ 
     xpRunOptionDialogManage();
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
     ClearTextWindow(textWindow);
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
     changeMaxSizeDialogManage ();
}

/*
 * Clearopdisplay button callback function
 */
void Clearopdisplay(Widget w, Draw_Data *dd, XtPointer call_data)
{ 
     XClearWindow(XtDisplay(dd->canvas), dd->window);
     dd->op = NULL;
}

/*
 * Clearigdisplay button callback function
 */
void Clearigdisplay(Widget w, Int_Draw_Data *idd, XtPointer call_data)
{ 
     XClearWindow(XtDisplay(idd->canvas), XtWindow(idd->canvas));

}

/*
 * Displayop button callback function
 */
void Displayop(Widget w,  XtPointer ignore, XtPointer call_data)
{ 
     xpDisplaySelectOpDialogManage();     
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
     xpDeleteSelectOpDialogManage();     
}

Widget xp_create_menu_bar(Widget parent, Draw_Data *dd, Int_Draw_Data *idd)
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

     Arg args[MAXARGS];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     menuBar = XmCreateMenuBar(parent, "menuBar", args, n);
     XtManageChild(menuBar);


     filePDMenu = XmCreatePulldownMenu(menuBar, "filePDMenu", NULL, 0);

     oprsPDMenu = XmCreatePulldownMenu(menuBar, "oprsPDMenu", NULL, 0);

     inspectPDMenu = XmCreatePulldownMenu(menuBar, "inspectPDMenu", NULL, 0);

     listPDMenu = XmCreatePulldownMenu(inspectPDMenu, "listPDMenu", NULL, 0);

     tracePDMenu = XmCreatePulldownMenu(menuBar, "tracePDMenu",NULL, 0);

     optionPDMenu = XmCreatePulldownMenu(menuBar, "optionPDMenu",NULL, 0);

     displayPDMenu = XmCreatePulldownMenu(menuBar, "displayPDMenu",NULL, 0);

     helpPDMenu = XmCreatePulldownMenu(menuBar, "helpPDMenu",NULL, 0);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, filePDMenu); n++;
     fileCButton = XmCreateCascadeButton(menuBar,"fileCButton", args, n);
     XtManageChild(fileCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, oprsPDMenu); n++;
     oprsCButton = XmCreateCascadeButton(menuBar,"oprsCButton", args, n);
     XtManageChild(oprsCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, inspectPDMenu); n++;
     inspectCButton = XmCreateCascadeButton(menuBar,"inspectCButton", args, n);
     XtManageChild(inspectCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, tracePDMenu); n++;
     traceCButton = XmCreateCascadeButton(menuBar,"traceCButton", args, n);
     XtManageChild(traceCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, optionPDMenu); n++;
     optionCButton = XmCreateCascadeButton(menuBar,"optionCButton", args, n);
     XtManageChild(optionCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, displayPDMenu); n++;
     displayCButton = XmCreateCascadeButton(menuBar,"displayCButton", args, n);
     XtManageChild(displayCButton);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, helpPDMenu); n++;
     helpCButton = XmCreateCascadeButton(menuBar,"helpCButton", args, n);
     XtManageChild(helpCButton);

     n = 0;
     XtSetArg(args[n], XmNmenuHelpWidget,(XtArgVal)helpCButton); n++;
     XtSetValues(menuBar,args,n);

     /* Help menu items */
     n = 0;
     help = XmCreatePushButtonGadget(helpPDMenu,"help", args, n);
     XtAddCallback(help, XmNactivateCallback, infoHelp, NULL);
     XtManageChild(help);

     XtManageChild(XmCreateSeparator(helpPDMenu, "fileseparator", NULL, 0));

     n = 0;
     file_help = XmCreatePushButtonGadget(helpPDMenu, "file_help", args, n);
     XtAddCallback(file_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "File Menu"));
     XtManageChild(file_help);

     n = 0;
     oprs_help = XmCreatePushButtonGadget(helpPDMenu, "oprs_help", args, n);
     XtAddCallback(oprs_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "OPRS Menu"));
     XtManageChild(oprs_help);

     n = 0;
     inspect_help = XmCreatePushButtonGadget(helpPDMenu, "inspect_help", args, n);
     XtAddCallback(inspect_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Inspect Menu"));
     XtManageChild(inspect_help);

     n = 0;
     trace_help = XmCreatePushButtonGadget(helpPDMenu, "trace_help", args, n);
     XtAddCallback(trace_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Trace Menu"));
     XtManageChild(trace_help);

     n = 0;
     option_help = XmCreatePushButtonGadget(helpPDMenu, "option_help", args, n);
     XtAddCallback(option_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Option Menu"));
     XtManageChild(option_help);

     n = 0;
     display_help = XmCreatePushButtonGadget(helpPDMenu, "display_help", args, n);
     XtAddCallback(display_help, XmNactivateCallback, infoHelp,
                   makeFileNode("oprs", "Display Menu"));
     XtManageChild(display_help);
     
     /* File menu items */
     include = XmCreatePushButtonGadget(filePDMenu,"include", NULL, 0);
     XtAddCallback(include, XmNactivateCallback, Xp_include, 0);
     XtManageChild(include);

     loaddb = XmCreatePushButtonGadget(filePDMenu,"loaddb", NULL, 0);
     XtAddCallback(loaddb, XmNactivateCallback, Xp_loaddb, 0);
     XtManageChild(loaddb);

     if (dev_env) {
	  loadop = XmCreatePushButtonGadget(filePDMenu,"loadop", NULL, 0);
	  XtAddCallback(loadop, XmNactivateCallback, Xp_loadop, 0);
	  XtManageChild(loadop);
     }

     XtManageChild(XmCreateSeparator(filePDMenu,"fileseparator",NULL,0));

     loadddb = XmCreatePushButtonGadget(filePDMenu,"loadddb", NULL, 0);
     XtAddCallback(loadddb, XmNactivateCallback, Xp_loadddb, 0);
     XtManageChild(loadddb);

     loaddop = XmCreatePushButtonGadget(filePDMenu,"loaddop", NULL, 0);
     XtAddCallback(loaddop, XmNactivateCallback, Xp_loaddop, 0);
     XtManageChild(loaddop);

     loadkrn = XmCreatePushButtonGadget(filePDMenu,"loadkrn", NULL, 0);
     XtAddCallback(loadkrn, XmNactivateCallback, Xp_loadkrn, 0);
     XtManageChild(loadkrn);

     if (dev_env) {
	  XtManageChild(XmCreateSeparator(filePDMenu,"fileseparator",NULL,0));

	  listopfs = XmCreatePushButtonGadget(filePDMenu,"listopfs", NULL, 0);
	  XtAddCallback(listopfs, XmNactivateCallback, Xp_listopfs, 0);
	  XtManageChild(listopfs);

	  reloadop = XmCreatePushButtonGadget(filePDMenu,"reloadop", NULL, 0);
	  XtAddCallback(reloadop, XmNactivateCallback, Xp_reloadop, 0);
	  XtManageChild(reloadop);

	  unloadop = XmCreatePushButtonGadget(filePDMenu,"unloadop", NULL, 0);
	  XtAddCallback(unloadop, XmNactivateCallback, Xp_unloadop, 0);
	  XtManageChild(unloadop);
     }

     XtManageChild(XmCreateSeparator(filePDMenu,"fileseparator",NULL,0));

     savedb = XmCreatePushButtonGadget(filePDMenu,"savedb", NULL, 0);
     XtAddCallback(savedb, XmNactivateCallback, Xp_savedb, 0);
     XtManageChild(savedb);

     XtManageChild(XmCreateSeparator(filePDMenu,"fileseparator",NULL,0));

     if (dev_env) {
	  dumpdb = XmCreatePushButtonGadget(filePDMenu,"dumpdb", NULL, 0);
	  XtAddCallback(dumpdb, XmNactivateCallback, Xp_dumpdb, 0);
	  XtManageChild(dumpdb);

	  dumpAllOpf = XmCreatePushButtonGadget(filePDMenu,"dumpAllOpf", NULL, 0);
	  XtAddCallback(dumpAllOpf, XmNactivateCallback, Xp_dumpAllOpf, 0);
	  XtManageChild(dumpAllOpf);

	  dumpop = XmCreatePushButtonGadget(filePDMenu,"dumpop", NULL, 0);
	  XtAddCallback(dumpop, XmNactivateCallback, Xp_dumpop, 0);
	  XtManageChild(dumpop);
     }

     dumpkrn = XmCreatePushButtonGadget(filePDMenu,"dumpkrn", NULL, 0);
     XtAddCallback(dumpkrn, XmNactivateCallback, Xp_dumpkrn, 0);
     XtManageChild(dumpkrn);

     XtManageChild(XmCreateSeparator(filePDMenu,"fileseparator",NULL,0));

     quit = XmCreatePushButtonGadget(filePDMenu,"quit", NULL, 0);
     XtAddCallback(quit, XmNactivateCallback, Quit, 0);
     XtManageChild(quit);

     
     /* OPRS menu items */

     addFactGoal = XmCreatePushButtonGadget(oprsPDMenu,"addFactGoal", NULL, 0);
     XtAddCallback(addFactGoal, XmNactivateCallback, Xp_addFactGoal, 0);
     XtManageChild(addFactGoal);

     concludeDB = XmCreatePushButtonGadget(oprsPDMenu,"concludeDB", NULL, 0);
     XtAddCallback(concludeDB, XmNactivateCallback, Xp_concludeDB, 0);
     XtManageChild(concludeDB);

     XtManageChild(XmCreateSeparator(oprsPDMenu,"fileseparator",NULL,0));

     deleteDB = XmCreatePushButtonGadget(oprsPDMenu,"deleteDB", NULL, 0);
     XtAddCallback(deleteDB, XmNactivateCallback, Xp_deleteDB, 0);
     XtManageChild(deleteDB);

     deleteOP = XmCreatePushButtonGadget(oprsPDMenu,"deleteOP", NULL, 0);
     XtAddCallback(deleteOP, XmNactivateCallback, (XtCallbackProc)DeleteOP, dd);
     XtManageChild(deleteOP);

     XtManageChild(XmCreateSeparator(oprsPDMenu,"fileseparator",NULL,0));

     resetdb = XmCreatePushButtonGadget(oprsPDMenu,"resetdb", NULL, 0);
     XtAddCallback(resetdb, XmNactivateCallback, Xp_resetdb, 0);
     XtManageChild(resetdb);

     resetop = XmCreatePushButtonGadget(oprsPDMenu,"resetop", NULL, 0);
     XtAddCallback(resetop, XmNactivateCallback, Xp_resetop, dd);
     XtManageChild(resetop);

     /* Inspect Menu Item */

     showDB = XmCreatePushButtonGadget(inspectPDMenu,"showDB", NULL, 0);
     XtAddCallback(showDB, XmNactivateCallback, Xp_showDB, 0);
     XtManageChild(showDB);

     showGV = XmCreatePushButtonGadget(inspectPDMenu,"showGV", NULL, 0);
     XtAddCallback(showGV, XmNactivateCallback, Xp_showGV, 0);
     XtManageChild(showGV);

     showSI = XmCreatePushButtonGadget(inspectPDMenu,"showSI", NULL, 0);
     XtAddCallback(showSI, XmNactivateCallback, Xp_showSI, 0);
     XtManageChild(showSI);

     showSC = XmCreatePushButtonGadget(inspectPDMenu,"showSC", NULL, 0);
     XtAddCallback(showSC, XmNactivateCallback, Xp_showSC, 0);
     XtManageChild(showSC);

     showMem = XmCreatePushButtonGadget(inspectPDMenu,"showMem", NULL, 0);
     XtAddCallback(showMem, XmNactivateCallback, Showmem, 0);
     XtManageChild(showMem);

     XtManageChild(XmCreateSeparator(inspectPDMenu,"fileseparator",NULL,0));

     consultDB = XmCreatePushButtonGadget(inspectPDMenu,"consultDB", NULL, 0);
     XtAddCallback(consultDB, XmNactivateCallback, Xp_consultDB, 0);
     XtManageChild(consultDB);

     consultOP = XmCreatePushButtonGadget(inspectPDMenu,"consultOP", NULL, 0);
     XtAddCallback(consultOP, XmNactivateCallback, Xp_consultOP, 0);
     XtManageChild(consultOP);

     consultAOP = XmCreatePushButtonGadget(inspectPDMenu,"consultAOP", NULL, 0);
     XtAddCallback(consultAOP, XmNactivateCallback, Xp_consultAOP, 0);
     XtManageChild(consultAOP);

     XtManageChild(XmCreateSeparator(inspectPDMenu,"fileseparator",NULL,0));

     listALL = XmCreatePushButtonGadget(inspectPDMenu,"listALL", NULL, 0);
     XtAddCallback(listALL, XmNactivateCallback, Xp_listALL, 0);
     XtManageChild(listALL);

     n = 0;
     XtSetArg(args[n], XmNsubMenuId, listPDMenu); n++;
     listCButton = XmCreateCascadeButton(inspectPDMenu,"listCButton", args, n);
     XtManageChild(listCButton);

     listops = XmCreatePushButtonGadget(inspectPDMenu,"listops", NULL, 0);
     XtAddCallback(listops, XmNactivateCallback, Xp_listops, 0);
     XtManageChild(listops);

     listvops = XmCreatePushButtonGadget(inspectPDMenu,"listvops", NULL, 0);
     XtAddCallback(listvops, XmNactivateCallback, Xp_listvops, 0);
     XtManageChild(listvops);

     XtManageChild(XmCreateSeparator(inspectPDMenu,"fileseparator",NULL,0));

     statDB = XmCreatePushButtonGadget(inspectPDMenu,"statDB", NULL, 0);
     XtAddCallback(statDB, XmNactivateCallback, Xp_statDB, 0);
     XtManageChild(statDB);

     statID = XmCreatePushButtonGadget(inspectPDMenu,"statID", NULL, 0);
     XtAddCallback(statID, XmNactivateCallback, Xp_statID, 0);
     XtManageChild(statID);

     statALL = XmCreatePushButtonGadget(inspectPDMenu,"statALL", NULL, 0);
     XtAddCallback(statALL, XmNactivateCallback, Xp_statALL, 0);
     XtManageChild(statALL);

     /* list submenus */

     listPredicate = XmCreatePushButtonGadget(listPDMenu,"listPredicate", NULL, 0);
     XtAddCallback(listPredicate, XmNactivateCallback, Xp_listPredicate, 0);
     XtManageChild(listPredicate);

     listEvaluablePredicate = XmCreatePushButtonGadget(listPDMenu,"listEvaluablePredicate", NULL, 0);
     XtAddCallback(listEvaluablePredicate, XmNactivateCallback, Xp_listEvaluablePredicate, 0);
     XtManageChild(listEvaluablePredicate);

     listCWPredicate = XmCreatePushButtonGadget(listPDMenu,"listCWPredicate", NULL, 0);
     XtAddCallback(listCWPredicate, XmNactivateCallback, Xp_listCWPredicate, 0);
     XtManageChild(listCWPredicate);

     listFFPredicate = XmCreatePushButtonGadget(listPDMenu,"listFFPredicate", NULL, 0);
     XtAddCallback(listFFPredicate, XmNactivateCallback, Xp_listFFPredicate, 0);
     XtManageChild(listFFPredicate);

     listBEPredicate = XmCreatePushButtonGadget(listPDMenu,"listBEPredicate", NULL, 0);
     XtAddCallback(listBEPredicate, XmNactivateCallback, Xp_listBEPredicate, 0);
     XtManageChild(listBEPredicate);

     listOPPredicate = XmCreatePushButtonGadget(listPDMenu,"listOPPredicate", NULL, 0);
     XtAddCallback(listOPPredicate, XmNactivateCallback, Xp_listOPPredicate, 0);
     XtManageChild(listOPPredicate);

     listFunction = XmCreatePushButtonGadget(listPDMenu,"listFunction", NULL, 0);
     XtAddCallback(listFunction, XmNactivateCallback, Xp_listFunction, 0);
     XtManageChild(listFunction);

     listEvaluableFunction = XmCreatePushButtonGadget(listPDMenu,"listEvaluableFunction", NULL, 0);
     XtAddCallback(listEvaluableFunction, XmNactivateCallback, Xp_listEvaluableFunction, 0);
     XtManageChild(listEvaluableFunction);

     listAction = XmCreatePushButtonGadget(listPDMenu,"listAction", NULL, 0);
     XtAddCallback(listAction, XmNactivateCallback, Xp_listAction, 0);
     XtManageChild(listAction);

     /* Trace menu items */

     oprstrace = XmCreatePushButtonGadget(tracePDMenu,"oprstrace", NULL, 0);
     XtAddCallback(oprstrace, XmNactivateCallback, Oprstrace, 0);
     XtManageChild(oprstrace);

     optrace = XmCreatePushButtonGadget(tracePDMenu,"optrace", NULL, 0);
     XtAddCallback(optrace, XmNactivateCallback, Optrace, 0);
     XtManageChild(optrace);

     if (install_user_trace) {
	  user_trace = XmCreatePushButtonGadget(tracePDMenu,"user_trace", NULL, 0);
	  XtAddCallback(user_trace, XmNactivateCallback, User_trace, 0);
	  XtManageChild(user_trace);
     }

     /* Option menu items */

     run_option = XmCreatePushButtonGadget(optionPDMenu,"run_option", NULL, 0);
     XtAddCallback(run_option, XmNactivateCallback, Run_option, 0);
     XtManageChild(run_option);

     compiler_option = XmCreatePushButtonGadget(optionPDMenu,"compiler_option", NULL, 0);
     XtAddCallback(compiler_option, XmNactivateCallback, Compiler_option, 0);
     XtManageChild(compiler_option);

     meta_option = XmCreatePushButtonGadget(optionPDMenu,"meta_option", NULL, 0);
     XtAddCallback(meta_option, XmNactivateCallback, Meta_option, 0);
     XtManageChild(meta_option);

#ifdef OPRS_PROFILING
     profiling_option = XmCreatePushButtonGadget(optionPDMenu,"profiling_option", NULL, 0);
     XtAddCallback(profiling_option, XmNactivateCallback, Profiling_option, 0);
     XtManageChild(profiling_option);
#endif

/*
     XtSetArg(args[0],XmNsensitive, False);
     opoption = XmCreatePushButtonGadget(optionPDMenu,"opoption", args, 1);
     XtAddCallback(opoption, XmNactivateCallback, Opoption, 0);
     XtManageChild(opoption);
*/   
     /* Display menu items */

     displayop = XmCreatePushButtonGadget(displayPDMenu,"displayop", NULL, 0);
     XtAddCallback(displayop, XmNactivateCallback, (XtCallbackProc)Displayop, dd);
     XtManageChild(displayop);

     displayPreviousOp = XmCreatePushButtonGadget(displayPDMenu,"displayPreviousOp", NULL, 0);
     XtAddCallback(displayPreviousOp, XmNactivateCallback, (XtCallbackProc)DisplayPreviousOp, dd);
     XtManageChild(displayPreviousOp);

     displayNextOp = XmCreatePushButtonGadget(displayPDMenu,"displayNextOp", NULL, 0);
     XtAddCallback(displayNextOp, XmNactivateCallback, (XtCallbackProc)DisplayNextOp, dd);
     XtManageChild(displayNextOp);

     XtManageChild(XmCreateSeparator(displayPDMenu,"fileseparator",NULL,0));

     cleartextdisplay = XmCreatePushButtonGadget(displayPDMenu,"cleartextdisplay", NULL, 0);
     XtAddCallback(cleartextdisplay, XmNactivateCallback, Cleartextdisplay, NULL);
     XtManageChild(cleartextdisplay);

     clearopdisplay = XmCreatePushButtonGadget(displayPDMenu,"clearopdisplay", NULL, 0);
     XtAddCallback(clearopdisplay, XmNactivateCallback, (XtCallbackProc)Clearopdisplay, dd);
     XtManageChild(clearopdisplay);

     clearigdisplay = XmCreatePushButtonGadget(displayPDMenu,"clearigdisplay", NULL, 0);
     XtAddCallback(clearigdisplay, XmNactivateCallback, (XtCallbackProc)Clearigdisplay, idd);
     XtManageChild(clearigdisplay);

     changemaxtextsize = XmCreatePushButtonGadget(displayPDMenu,"changemaxtextsize", NULL, 0);
     XtAddCallback(changemaxtextsize, XmNactivateCallback, Changemaxtextsize, 0);
     XtManageChild(changemaxtextsize);

     return menuBar;

}
