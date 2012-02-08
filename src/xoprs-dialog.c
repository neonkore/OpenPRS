static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * xoprs-dialog.c -- 
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


#include <stdio.h>

/* 
 * Standard Toolkit include files
 */
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/MessageB.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>

#include "constant.h"
#include "macro.h"

#include "xhelp.h"
#include "xhelp_f.h"
#include "oprs-type.h"
#include "xoprs.h"
#include "xoprs-main.h"
#include "op-structure.h"
#include "oprs.h"
#include "relevant-op.h"
#include "intention.h"
#include "ope-graphic.h"
#include "ope-graphic_f.h"
#include "xoprs-intention.h"
#include "int-graph_f.h"
#include "oprs-main.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"

#include "database.h"
#include "database_f.h"

#include "relevant-op_f.h"

Widget addFactGoalDialog;
Widget consultOPDialog;
Widget consultAOPDialog;
Widget consultDBDialog;
Widget concludeDBDialog;
Widget deleteDBDialog;
Widget xp_quitQuestion;
Widget xp_resetQuestion;
Widget xpTraceDialog;
Widget xpMetaOptionDialog;
Widget xpUserTraceDialog;
#ifdef OPRS_PROFILING
Widget xpProfilingOptionDialog;
#endif
Widget xpCompilerOptionDialog;
Widget xpRunOptionDialog;
/* Widget xpTextOpMenu; */
Widget xpTraceOpDialog;
Widget xpTraceOpScrollList;
/*
Widget xpGraphicOpDialog;
Widget xpGraphicOpScrollList;
*/

Widget xpDisplaySelectOpDialog;
Widget xpDeleteSelectOpDialog;
Widget xpUnloadOpDialog;
Widget xpReloadOpDialog;
Widget trace_radiobox, text_radiobutton, graphic_radiobutton, both_radiobutton, all_radiobutton, step_radiobutton;

void updateSensitiveMetaOptionButtons(PBoolean meta_level_is_on);
void updateSensitiveUserTraceButtons(PBoolean meta_level_is_on);
#ifdef OPRS_PROFILING
void updateSensitiveProfilingOptionButtons(PBoolean profiling);
#endif
void ReallyQuit(Widget w, XtPointer client_data, XtPointer call_data);

Widget changeMaxSizeDialog;

void setSizeTextWindow(int size);
int getSizeTextWindow();

void changeMaxSizeDialogManage(void)
{
     char s[LINSIZ];
     int oldsize = getSizeTextWindow();

     sprintf (s,"%d", oldsize);
     XmTextSetString(XmSelectionBoxGetChild(changeMaxSizeDialog,XmDIALOG_TEXT), s);
     XtManageChild(changeMaxSizeDialog);
}

void changeMaxSizeDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char s[LINSIZ];
     int newsize;
     char *tmp_str;

     sprintf(s,"%s\n", (tmp_str = XmTextGetString(XmSelectionBoxGetChild(changeMaxSizeDialog,
									 XmDIALOG_TEXT))));
     if (!sscanf ( s,"%d", &newsize ))
	 fprintf ( stderr, LG_STR("The size must be an integer.\n",
				  "The size must be an integer.\n"));
       else
	    setSizeTextWindow(newsize);
     XtFree (tmp_str);
}

char *file_basename(char *file_name)
{
    char *index;

    index = strrchr(file_name,'/'); /* search for / backward. */
    if (!index) 
	 index = file_name; /* Cannot find one... bad news... take the whole thing. */
    else 
	 index = index+1;	/* To remove the / */

    return index;
}

PBoolean sort_op_opf(Op_Structure *op1, Op_Structure *op2)
{
     int file_cmp;
     char *file_name1 = file_basename(op1->file_name);
     char *file_name2 = file_basename(op2->file_name);

     if ((file_cmp = strcmp(file_name1,file_name2)) == 0) 
	  return (strcmp(op1->name,op2->name) < 0);
     else
	  return (file_cmp < 0);
}

void build_op_xms_name(Op_Structure *op) /* in xoprs xms contains the name of the file and the name of the OP */
{
    static Sprinter *sp = NULL;
    char *index;
    
    if (!sp) sp = make_sprinter(0);
    else reset_sprinter(sp);

    
    index = file_basename(op->file_name);
    SPRINT(sp,strlen(index) + strlen(op->name) + 4, sprintf(f,"[%s] %s", index, op->name));
    op->xms_name = XmStringCreateSimple(SPRINTER_STRING(sp));
}

void xpDisplaySelectOpDialogManage(void)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     Op_Structure *op;

     sl_sort_slist_func(current_oprs->relevant_op->op_list,sort_op_opf);

     item = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->op_list) + 1,sizeof(XmString));

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if (!op->xms_name) build_op_xms_name(op);
	  item[i] = op->xms_name;
	  i++;
     }
     
     item[i] = NULL;

     n = 0;
     XtSetArg(args[n],  XmNlistItems,item); n++;
     XtSetArg(args[n],  XmNlistItemCount,i); n++;
     XtSetValues(xpDisplaySelectOpDialog,args,n);

     XtFree((char *)item);

     XtManageChild(xpDisplaySelectOpDialog);
}

void xpDisplaySelectOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Arg args[2];
     Op_Structure *op;
     XmStringTable op_sxmstring;
     int snop;

     XtSetArg(args[0],  XmNselectedItems, &op_sxmstring);
     XtSetArg(args[1],  XmNselectedItemCount, &snop);
     XtGetValues(XmSelectionBoxGetChild(xpDisplaySelectOpDialog,XmDIALOG_LIST), args, 2);

     if (snop > 1) 
	  fprintf(stderr, LG_STR("How did you select more than one item?\n",
				 "How did you select more than one item?\n"));

     if (snop == 0) return;

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if (XmStringCompare(op->xms_name,op_sxmstring[0])) {
		    display_op_pos(op,dd, 0,0);
		    break;
	  }
     }
}

void xpDisplayNextOp(Draw_Data *dd)
{
     PBoolean next = FALSE;
     Op_Structure *op  = NULL;

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if ((! dd->op) || next) {
	       break;
	  }
	  
	  next = (op == dd->op);
     }
     
     if (op) display_op_pos(op, dd, 0, 0);
     else {
	  XClearWindow(XtDisplay(dd->canvas), dd->window);
	  dd->op = NULL;
	  XBell(XtDisplay(dd->canvas), 50);
     }
}

void xpDisplayPreviousOp(Draw_Data *dd)
{
     Op_Structure *pop  = NULL;
     Op_Structure *op;

     if (! dd->op) {
	  pop = (Op_Structure *)sl_get_slist_tail(current_oprs->relevant_op->op_list);
     } else {
	  sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	       if (op == dd->op) break;
	       pop = op;
	  }
     }
     
     if (pop) display_op_pos(pop,dd, 0,0);
     else {
	  XClearWindow(XtDisplay(dd->canvas), dd->window);
	  dd->op = NULL;
	  XBell(XtDisplay(dd->canvas), 50);
     }
}

void xpDeleteSelectOpDialogManage(void)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     Op_Structure *op;

     sl_sort_slist_func(current_oprs->relevant_op->op_list,sort_op_opf);

     item = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->op_list) + 1,sizeof(XmString));

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if (!op->xms_name) build_op_xms_name(op);
	  item[i] = op->xms_name;
	  i++;
     }
     
     item[i] = NULL;

     n = 0;
     XtSetArg(args[n],  XmNlistItems,item); n++;
     XtSetArg(args[n],  XmNlistItemCount,i); n++;
     XtSetValues(xpDeleteSelectOpDialog,args,n);

     XtFree((char *)item);

     XtManageChild(xpDeleteSelectOpDialog);
}

void xpDeleteSelectOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Arg args[2];
     Op_Structure *op;
     XmStringTable op_sxmstring;
     int snop;

     XtSetArg(args[0],  XmNselectedItems, &op_sxmstring);
     XtSetArg(args[1],  XmNselectedItemCount, &snop);
     XtGetValues(XmSelectionBoxGetChild(xpDeleteSelectOpDialog,XmDIALOG_LIST), args, 2);

     if (snop > 1) 
	  fprintf(stderr, LG_STR("How did you select more than one item?\n",
				 "How did you select more than one item?\n"));

     if (snop == 0) return;

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if (XmStringCompare(op->xms_name,op_sxmstring[0])) {
	       delete_op_from_rop(op, current_oprs->relevant_op,TRUE, TRUE, TRUE);
	       break;
	  }
     }
}

void xpUnloadOpDialogManage(void)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     char *opf;

     item = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->opf_list) + 1,sizeof(XmString));

     sl_loop_through_slist(current_oprs->relevant_op->opf_list, opf, char *) {
	  item[i] = XmStringCreateSimple(opf);
	  i++;
     }
     
     item[i] = NULL;

     n = 0;
     XtSetArg(args[n],  XmNlistItems,item); n++;
     XtSetArg(args[n],  XmNlistItemCount,i); n++;
     XtSetValues(xpUnloadOpDialog,args,n);

     XtFree((char *)item);

     XtManageChild(xpUnloadOpDialog);
}

void xpUnloadOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *res;

     res = XmTextGetString(XmSelectionBoxGetChild(xpUnloadOpDialog,XmDIALOG_TEXT));
     if (strcmp(res,"") != 0) {
	  char *s = (char *)MALLOC(strlen(res) + 20);
	  sprintf(s,"delete opf \"%s\"\n", res);
	  send_command_to_parser(s);
	  FREE(s);
     }
     XtFree(res);
}

void xpReloadOpDialogManage(void)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     char *opf;

     item = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->opf_list) + 1,sizeof(XmString));

     sl_loop_through_slist(current_oprs->relevant_op->opf_list, opf, char *) {
	  item[i] = XmStringCreateSimple(opf);
	  i++;
     }
     
     item[i] = NULL;

     n = 0;
     XtSetArg(args[n],  XmNlistItems,item); n++;
     XtSetArg(args[n],  XmNlistItemCount,i); n++;
     XtSetValues(xpReloadOpDialog,args,n);

     XtFree((char *)item);

     XtManageChild(xpReloadOpDialog);
}

void xpReloadOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *res;

     res = XmTextGetString(XmSelectionBoxGetChild(xpReloadOpDialog,XmDIALOG_TEXT));
     if (strcmp(res,"") != 0) {
	  char *s = (char *)MALLOC(strlen(res) + 30);

	  sprintf(s,"reload opf \"%s\"\n",res);
	  send_command_to_parser(s);

	  FREE(s);
     }
     XtFree(res);
}

void addFactGoalDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     XmString xm_to_free;
     String to_free;
     Widget list;

     to_free = XmTextGetString(XmSelectionBoxGetChild(addFactGoalDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(addFactGoalDialog,XmDIALOG_LIST);

     if (XmListItemExists(list,xm_to_free))
	  XmListDeleteItem(list,xm_to_free);
     
     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);
     
     s = (char *)MALLOC(strlen(to_free) + 20);
     sprintf(s,"add %s\n", to_free);
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void consultOPDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     String to_free;
     XmString xm_to_free;
     Widget list;

     to_free =  XmTextGetString(XmSelectionBoxGetChild(consultOPDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(consultOPDialog,XmDIALOG_LIST);

     if (XmListItemExists(list, xm_to_free))
	  XmListDeleteItem(list, xm_to_free);

     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);

     s = (char *)MALLOC(strlen(to_free) + 32);
     sprintf(s,"consult relevant op %s\n",  to_free);       
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void consultAOPDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     String to_free;
     XmString xm_to_free;
     Widget list;

     to_free = XmTextGetString(XmSelectionBoxGetChild(consultAOPDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(consultAOPDialog,XmDIALOG_LIST);

     if (XmListItemExists(list, xm_to_free))
	  XmListDeleteItem(list, xm_to_free);

     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);

     s = (char *)MALLOC(strlen(to_free) + 32);
     sprintf(s,"consult applicable op %s\n", to_free);       
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void consultDBDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     String to_free;
     XmString xm_to_free;
     Widget list;

     to_free = XmTextGetString(XmSelectionBoxGetChild(consultDBDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(consultDBDialog,XmDIALOG_LIST);

     if (XmListItemExists(list, xm_to_free))
	  XmListDeleteItem(list, xm_to_free);

     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);

     s = (char *)MALLOC(strlen(to_free) + 20);
     sprintf(s,"consult %s\n", to_free);       
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void deleteDBDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     String to_free;
     XmString xm_to_free;
     Widget list;

     to_free = XmTextGetString(XmSelectionBoxGetChild(deleteDBDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(deleteDBDialog,XmDIALOG_LIST);

     if (XmListItemExists(list, xm_to_free))
	  XmListDeleteItem(list, xm_to_free);

     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);

     s = (char *)MALLOC(strlen(to_free) + 20);
     sprintf(s,"delete %s\n", to_free);       
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void concludeDBDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *s;
     String to_free;
     XmString xm_to_free;
     Widget list;

     to_free = XmTextGetString(XmSelectionBoxGetChild(concludeDBDialog,XmDIALOG_TEXT));
     xm_to_free = XmStringCreateSimple(to_free);
     list = XmSelectionBoxGetChild(concludeDBDialog,XmDIALOG_LIST);

     if (XmListItemExists(list, xm_to_free))
	  XmListDeleteItem(list, xm_to_free);

     XmListAddItem(list, xm_to_free, 0);
     XmListSetBottomPos(list, 0);

     s = (char *)MALLOC(strlen(to_free) + 20);
     sprintf(s,"conclude %s\n", to_free);       
     send_command_to_parser(s);
     FREE(s);
     XtFree(to_free);
     XmStringFree(xm_to_free);
}

void quitQuestionManage()
{
  XtManageChild(xp_quitQuestion);
}

void ReallyReset(Widget w, XtPointer oprs, XtPointer call_data)
{
     reset_oprs_kernel((Oprs *)oprs);
}

void resetQuestionManage()
{
  XtManageChild(xp_resetQuestion);
}

Widget debug_trace_widget_array[MAX_DEBUG];

char *debug_trace_widget_name_array [MAX_DEBUG];

static void init_debug_trace_widget_name_array()
{
     debug_trace_widget_name_array[POST_GOAL] = "tracePostGoal";
     debug_trace_widget_name_array[POST_FACT] = "tracePostFact";
     debug_trace_widget_name_array[RECEIVE_MESSAGE] = "traceReceiveMessage";
     debug_trace_widget_name_array[SEND_MESSAGE] = "traceSendMessage";
     debug_trace_widget_name_array[DATABASE_FRAME] = "traceDatabaseFrame";
     debug_trace_widget_name_array[DATABASE] = "traceDatabase";
     debug_trace_widget_name_array[CONCLUDE_FROM_PARSER] = "traceConcludeFromParser";
     debug_trace_widget_name_array[SUC_FAIL] = "traceSucFail";
     debug_trace_widget_name_array[INTENTION_FAILURE] = "traceInFail";
     debug_trace_widget_name_array[INT_FAIL_BIND] = "traceInFailBind";
     debug_trace_widget_name_array[INTEND] = "traceIntend";
     debug_trace_widget_name_array[OP_COMPILER] = "traceOpCompiler";
     debug_trace_widget_name_array[RELEVANT_OP] = "traceRelevantOp";
     debug_trace_widget_name_array[SOAK] = "traceSoak";
     debug_trace_widget_name_array[GRAPHIC_OP] = "traceGraphicOp";
     debug_trace_widget_name_array[TEXT_OP] = "traceTextOp";
     debug_trace_widget_name_array[GRAPHIC_INTEND] = "traceGraphicIntend";
     debug_trace_widget_name_array[THREADING] = "traceThreading";
#ifdef OPRS_DEBUG
     debug_trace_widget_name_array[DATABASE_KEY] = "traceDatabaseKey";
#else 
     debug_trace_widget_name_array[DATABASE_KEY] = NULL;
#endif
}

#ifdef OPRS_PROFILING
char *profiling_option_widget_name_array [MAX_PROFILING_OPTION];
Widget profiling_option_widget_array [MAX_PROFILING_OPTION];

static void init_profiling_option_widget_name_array()
{
     profiling_option_widget_name_array[PROFILING] = "profilingOptionProfiling";
     profiling_option_widget_name_array[PROF_DB] = "profilingDb";
     profiling_option_widget_name_array[PROF_ACTION_OP] = "profilingActionOp";
     profiling_option_widget_name_array[PROF_OP] = "profilingOp";
     profiling_option_widget_name_array[PROF_EP] = "profilingEP";
     profiling_option_widget_name_array[PROF_EF] = "profilingEF";
     profiling_option_widget_name_array[PROF_AC] = "profilingAc";
}
#endif

char *meta_option_widget_name_array [MAX_META_OPTION];
Widget meta_option_widget_array [MAX_META_OPTION];

static void init_meta_option_widget_name_array()
{
     meta_option_widget_name_array[META_LEVEL] = "metaOptionMetaLevel";
     meta_option_widget_name_array[SOAK_MF] = "metaOptionSoakMF";
     meta_option_widget_name_array[FACT_INV] = "metaOptionFactInv";
     meta_option_widget_name_array[GOAL_INV] = "metaOptionGoalInv";
     meta_option_widget_name_array[APP_OPS_FACT] = "metaOptionAppOpsFact";
     meta_option_widget_name_array[APP_OPS_GOAL] = "metaOptionAppOpsGoal";
}

char *user_trace_widget_name_array [MAX_USER_TRACE];
Widget user_trace_widget_array [MAX_USER_TRACE];

static void init_user_trace_widget_name_array()
{
     user_trace_widget_name_array[USER_TRACE] = "userTraceUserTrace";
     user_trace_widget_name_array[UT_POST_GOAL] = "userTraceUtPostGoal";
     user_trace_widget_name_array[UT_POST_FACT] = "userTraceUtPostFact";
     user_trace_widget_name_array[UT_RECEIVE_MESSAGE] = "userTraceUtReceiveMessage";
     user_trace_widget_name_array[UT_SEND_MESSAGE] = "userTraceUtSendMessage";
/*     user_trace_widget_name_array[UT_DATABASE] = "userTraceUtDatabase";
     user_trace_widget_name_array[UT_INTENTION_FAILURE] = "userTraceUtIntentionFailure"; */
     user_trace_widget_name_array[UT_SUC_FAIL] = "userTraceUtSucFail";
     user_trace_widget_name_array[UT_INTEND] = "userTraceUtIntend";
     user_trace_widget_name_array[UT_RELEVANT_OP] = "userTraceUtRelevantOp";
     user_trace_widget_name_array[UT_SOAK] = "userTraceUtSoak";
     user_trace_widget_name_array[UT_OP] = "userTraceUtOp";
     user_trace_widget_name_array[UT_THREADING] = "userTraceUtThreading";
}

char *compiler_option_widget_name_array [MAX_COMPILER_OPTION];
Widget compiler_option_widget_array [MAX_COMPILER_OPTION];

static void init_compiler_option_widget_name_array()
{
     compiler_option_widget_name_array[CHECK_ACTION] = "compilerOptionCheckAction";
     compiler_option_widget_name_array[CHECK_PFR] = "compilerOptionCheckPredFunc";
     compiler_option_widget_name_array[CHECK_SYMBOL] = "compilerOptionCheckSymbol";
}

char *run_option_widget_name_array [MAX_RUN_OPTION];
Widget run_option_widget_array [MAX_RUN_OPTION];

static void init_run_option_widget_name_array()
{
     run_option_widget_name_array[EVAL_ON_POST] = "runOptionEvalOnPost";
     run_option_widget_name_array[PAR_POST] = "runOptionParPost";
     run_option_widget_name_array[PAR_INTEND] = "runOptionParIntend";
     run_option_widget_name_array[PAR_EXEC] = "runOptionParExec";
}

void xpTraceDialogManage()
{
     int i;

     for (i = 0; i < MAX_DEBUG; i++) {
	  XmToggleButtonSetState(debug_trace_widget_array[i],debug_trace[i],False);
     }
     XtManageChild(xpTraceDialog);
}

void xpTraceDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     int i;

     if ((! debug_trace[GRAPHIC_INTEND])
	 && XmToggleButtonGetState(debug_trace_widget_array[GRAPHIC_INTEND])
	 && global_int_draw_data) {
	  rebuilt_intention_graph_graphic(global_int_draw_data->ig);
     }
     if (debug_trace[GRAPHIC_INTEND]
	 && ( ! (XmToggleButtonGetState(debug_trace_widget_array[GRAPHIC_INTEND])))
	 && global_int_draw_data) {
	  XClearWindow(XtDisplay(global_int_draw_data->canvas), XtWindow(global_int_draw_data->canvas));
     }

     for (i = 0; i < MAX_DEBUG; i++) {
	  debug_trace[i] = XmToggleButtonGetState(debug_trace_widget_array[i]);
     }
}

#ifdef OPRS_PROFILING
void xpProfilingOptionDialogManage()
{
     int i;

     for (i = 0; i < MAX_PROFILING_OPTION; i++) {
	  XmToggleButtonSetState(profiling_option_widget_array[i], profiling_option[i],False);
     }
     updateSensitiveProfilingOptionButtons(profiling_option[PROFILING]);

     XtManageChild(xpProfilingOptionDialog);
}

void xpProfilingOptionDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     int i;

     for (i = 0; i < MAX_PROFILING_OPTION; i++) {
	       profiling_option[i] = XmToggleButtonGetState(profiling_option_widget_array[i]);
     }
}
#endif

void xpMetaOptionDialogManage()
{
     int i;

     for (i = 0; i < MAX_META_OPTION; i++) {
	  XmToggleButtonSetState(meta_option_widget_array[i], meta_option[i],False);
     }
     updateSensitiveMetaOptionButtons(meta_option[META_LEVEL]);

     XtManageChild(xpMetaOptionDialog);
}

void xpMetaOptionDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     PBoolean meta_fact_changed = FALSE;
     int i;

     for (i = 0; i < MAX_META_OPTION; i++) {
	  if ( meta_option[i] != XmToggleButtonGetState(meta_option_widget_array[i]) ) {
	       meta_fact_changed = TRUE;
	       meta_option[i] = XmToggleButtonGetState(meta_option_widget_array[i]);
	  }
     }

     if (meta_fact_changed) report_meta_option_array();
}

void xpUserTraceDialogManage()
{
     int i;

     for (i = 0; i < MAX_USER_TRACE; i++) {
	  XmToggleButtonSetState(user_trace_widget_array[i], user_trace[i],False);
     }
     updateSensitiveUserTraceButtons(user_trace[USER_TRACE]);

     XtManageChild(xpUserTraceDialog);
}

void xpUserTraceDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     int i;

     for (i = 0; i < MAX_USER_TRACE; i++) {
	  user_trace[i] = XmToggleButtonGetState(user_trace_widget_array[i]);
     }
}

void xpRunOptionDialogManage()
{
     int i;

     for(i = 0; i < MAX_RUN_OPTION; i++) {
	  XmToggleButtonSetState(run_option_widget_array[i], run_option[i],False);
     }
     XtManageChild(xpRunOptionDialog);
}

void xpRunOptionDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     int i;

     for(i = 0; i < MAX_RUN_OPTION; i++)
	  run_option[i] = XmToggleButtonGetState(run_option_widget_array[i]);
}

void xpCompilerOptionDialogManage()
{
     int i;

     for(i = 0; i < MAX_COMPILER_OPTION; i++)
	  XmToggleButtonSetState(compiler_option_widget_array[i], compiler_option[i],False);

     XtManageChild(xpCompilerOptionDialog);
}

void xpCompilerOptionDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     int i;

     for(i = 0; i < MAX_COMPILER_OPTION; i++)
	  compiler_option[i] = XmToggleButtonGetState(compiler_option_widget_array[i]);
}

typedef enum {TEXT_TRACE, GRAPHIC_TRACE, BOTH_TRACE, STEP_TRACE, ALL_TRACE } Trace_Type;

text_button_changed(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_trace_op_list(TEXT_TRACE);
}

graphic_button_changed(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_trace_op_list(GRAPHIC_TRACE);
}

both_button_changed(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_trace_op_list(BOTH_TRACE);
}

step_button_changed(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_trace_op_list(STEP_TRACE);
}

all_button_changed(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct * call_data)
{
     if (call_data->set)
	  update_trace_op_list(ALL_TRACE);
}

update_trace_op_list(Trace_Type tt)
{
     Arg args[4];
     Op_Structure *op;
     XmStringTable op_xmstring;
     XmStringTable op_sxmstring;
     int nop, snop;
 
     nop = 0;
     snop = 0;

     op_xmstring = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->op_list) + 1,
					   sizeof(XmString));
     op_sxmstring = (XmStringTable)XtCalloc(sl_slist_length(current_oprs->relevant_op->op_list) + 1,
					    sizeof(XmString));

     sl_sort_slist_func(current_oprs->relevant_op->op_list,sort_op_opf);

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  if (!op->xms_name) build_op_xms_name(op);
	  op_xmstring[nop++] =  op->xms_name;
	  switch (tt) {
	  case TEXT_TRACE:
	       if (op->text_traced)  op_sxmstring[snop++] =  op->xms_name;
	       break;
	  case GRAPHIC_TRACE:
	       if (op->graphic_traced)  op_sxmstring[snop++] =  op->xms_name;
	       break;
	  case BOTH_TRACE:
	       if (op->graphic_traced && op->text_traced)  op_sxmstring[snop++] =  op->xms_name;
	       break;
	  case ALL_TRACE:
	       if (op->graphic_traced && op->text_traced && op->step_traced) 
		    op_sxmstring[snop++] =  op->xms_name;
	       break;
	  case STEP_TRACE:
	       if (op->step_traced)  op_sxmstring[snop++] =  op->xms_name;
	       break;
	  }
     }

     XtSetArg(args[0],  XmNitems, op_xmstring);
     XtSetArg(args[1],  XmNitemCount, nop);
     XtSetArg(args[2],  XmNselectedItems, op_sxmstring);
     XtSetArg(args[3],  XmNselectedItemCount, snop);
     XtSetValues(xpTraceOpScrollList, args, 4);
	
     XtFree((char *)op_xmstring);
     XtFree((char *)op_sxmstring);
}


void xpTraceOpDialogManage()
{
     if (XmToggleButtonGetState(text_radiobutton))
	  update_trace_op_list(TEXT_TRACE);
	  
     if (XmToggleButtonGetState(graphic_radiobutton))
	  update_trace_op_list(GRAPHIC_TRACE);
	  
     if (XmToggleButtonGetState(both_radiobutton))
	  update_trace_op_list(BOTH_TRACE);
	  
     if (XmToggleButtonGetState(all_radiobutton))
	  update_trace_op_list(ALL_TRACE);

     if (XmToggleButtonGetState(step_radiobutton))
	  update_trace_op_list(STEP_TRACE);

     XtManageChild(xpTraceOpDialog);

}

void xpTraceOpDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     Arg args[2];
     PBoolean value;
     XmString op_xms;
     Op_Structure *op;
     int i;
     XmStringTable op_sxmstring;
     int snop;
     Trace_Type tt = BOTH_TRACE;

     if (XmToggleButtonGetState(text_radiobutton))
	  tt = TEXT_TRACE;
	  
     if (XmToggleButtonGetState(graphic_radiobutton))
	  tt = GRAPHIC_TRACE;
	  
     if (XmToggleButtonGetState(both_radiobutton))
	  tt = BOTH_TRACE;
	  
     if (XmToggleButtonGetState(all_radiobutton))
	  tt = ALL_TRACE;
	  
     if (XmToggleButtonGetState(step_radiobutton))
	  tt = STEP_TRACE;

     XtSetArg(args[0],  XmNselectedItems, &op_sxmstring);
     XtSetArg(args[1],  XmNselectedItemCount, &snop);
     XtGetValues(xpTraceOpScrollList, args, 2);

     sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
	  value = FALSE;
	  op_xms = op->xms_name;
	  for (i=0; i < snop; i++) {
	       if (XmStringCompare(op_xms,op_sxmstring[i])) {
		    value = TRUE;
		    break;
	       }
	  }
	  switch (tt) {
	  case TEXT_TRACE:
	       op->text_traced = value;
	       break;
	  case GRAPHIC_TRACE:
	       op->graphic_traced = value;
	       break;
	  case STEP_TRACE:
	       op->step_traced = value;
	       break;
	  case BOTH_TRACE:
	       op->graphic_traced = value;
	       op->text_traced = value;
	       break;
	  case ALL_TRACE:
	       op->graphic_traced = value;
	       op->text_traced = value;
	       op->step_traced = value;
	       break;
	  }
     }
}

void updateSensitiveMetaOptionButtons(PBoolean meta_level_is_on)
{
     int i;

     for(i = 0 ; i < MAX_META_OPTION; i++) {
	  if (i != META_LEVEL)
	       XtSetSensitive(meta_option_widget_array[i], meta_level_is_on);
     }
}

void OptionMLChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
     updateSensitiveMetaOptionButtons(call_data->set);
}

void updateSensitiveUserTraceButtons(PBoolean meta_level_is_on)
{
     int i;

     for(i = 0 ; i < MAX_USER_TRACE; i++) {
	  if (i != USER_TRACE)
	       XtSetSensitive(user_trace_widget_array[i], meta_level_is_on);
     }
}

void OptionUTChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
     updateSensitiveUserTraceButtons(call_data->set);
}

#ifdef OPRS_PROFILING
void updateSensitiveProfilingOptionButtons(PBoolean profiling)
{
     int i;

     for(i = 0 ; i < MAX_PROFILING_OPTION; i++) {
	  if (i != PROFILING)
	       XtSetSensitive(profiling_option_widget_array[i], profiling);
     }
}

void OptionProfilingChanged(Widget w, XtPointer client_data, XmToggleButtonCallbackStruct *call_data)
{
     updateSensitiveProfilingOptionButtons(call_data->set);
}
#endif

Widget xpShowDBScrlTxt, xpShowDBDialog;

void xpShowDBDialogUpdate()
{
     Arg args[1];
     static Sprinter *show_db_string = NULL;

     if (! show_db_string)
	  show_db_string = make_sprinter(0);
     else
	  reset_sprinter(show_db_string);

     sprint_database(show_db_string, current_oprs->database);

     XmTextSetString(xpShowDBScrlTxt,SPRINTER_STRING(show_db_string));

/*      XtUnmanageChild(xpShowDBScrlTxt); */
/*      XtSetArg(args[0], XmNvalue,SPRINTER_STRING(show_db_string)); */
/*      XtSetValues(xpShowDBScrlTxt,args,1); */
/*      XtManageChild(xpShowDBScrlTxt); */
}

void XpShowDBDialogUpdate(Widget w, XtPointer client_data, XtPointer call_data)
{
     xpShowDBDialogUpdate();
}

void xpShowDBDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild(w);
}

void xpShowDBDialogManage()
{
     xpShowDBDialogUpdate();
     XtManageChild(xpShowDBDialog);
}

void xp_create_showdb_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     
     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpShowDBDialog =  XmCreateTemplateDialog(parent, "xpShowDBDialog",args,n);
#else
     xpShowDBDialog =  XmCreatePromptDialog(parent, "xpShowDBDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowDBDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowDBDialog, XmDIALOG_TEXT));
#endif
     XtAddCallback(xpShowDBDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Show Database")); 
     XtAddCallback(xpShowDBDialog, XmNokCallback, XpShowDBDialogUpdate, 0);
     XtAddCallback(xpShowDBDialog, XmNcancelCallback, xpShowDBDialogCancel, 0);


     n = 0;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpShowDBScrlTxt = XmCreateScrolledText(xpShowDBDialog,"xpShowDBScrlTxt",args,n);
     XtManageChild(xpShowDBScrlTxt);
}

Widget xpShowCondScrlTxt, xpShowCondDialog;

void xpShowCondDialogUpdate()
{
     Arg args[1];
     static Sprinter *show_cond_string = NULL;

     if (! show_cond_string)
	  show_cond_string = make_sprinter(0);
     else
	  reset_sprinter(show_cond_string);

     sprint_conditions(show_cond_string, current_oprs->conditions_list);
     
     XmTextSetString(xpShowCondScrlTxt,SPRINTER_STRING(show_cond_string));

/*      XtSetArg(args[0], XmNvalue,SPRINTER_STRING(show_cond_string)); */
/*      XtSetValues(xpShowCondScrlTxt,args,1); */
}

void XpShowCondDialogUpdate(Widget w, XtPointer client_data, XtPointer call_data)
{
     xpShowCondDialogUpdate();
}

void xpShowCondDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild(w);
}

void xpShowCondDialogManage()
{
     xpShowCondDialogUpdate();
     XtManageChild(xpShowCondDialog);
}

void xp_create_show_cond_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     
     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpShowCondDialog =  XmCreateTemplateDialog(parent, "xpShowCondDialog",args,n);
#else
     xpShowCondDialog =  XmCreatePromptDialog(parent, "xpShowCondDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowCondDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowCondDialog, XmDIALOG_TEXT));
#endif
     XtAddCallback(xpShowCondDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Show Conditions")); 
     XtAddCallback(xpShowCondDialog, XmNokCallback, XpShowCondDialogUpdate, 0);
     XtAddCallback(xpShowCondDialog, XmNcancelCallback, xpShowCondDialogCancel, 0);

     n = 0;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpShowCondScrlTxt = XmCreateScrolledText(xpShowCondDialog,"xpShowCondScrlTxt",args,n);
     XtManageChild(xpShowCondScrlTxt);
}

Widget xpShowIGScrlTxt, xpShowIGDialog;

void xpShowIGDialogUpdate()
{
     Arg args[1];
     static Sprinter *show_ig_string = NULL;

     if (! show_ig_string)
	  show_ig_string = make_sprinter(0);
     else
	  reset_sprinter(show_ig_string);

     sprint_intention_graph(show_ig_string, current_oprs->intention_graph);
     
     XmTextSetString(xpShowIGScrlTxt,SPRINTER_STRING(show_ig_string));

/*      XtSetArg(args[0], XmNvalue,SPRINTER_STRING(show_ig_string)); */
/*      XtSetValues(xpShowIGScrlTxt,args,1); */
}

void XpShowIGDialogUpdate(Widget w, XtPointer client_data, XtPointer call_data)
{
     xpShowIGDialogUpdate();
}

void xpShowIGDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild(w);
}

void xpShowIGDialogManage()
{
     xpShowIGDialogUpdate();
     XtManageChild(xpShowIGDialog);
}

void xp_create_showig_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     
     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpShowIGDialog =  XmCreateTemplateDialog(parent, "xpShowIGDialog",args,n);
#else
     xpShowIGDialog =  XmCreatePromptDialog(parent, "xpShowIGDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowIGDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowIGDialog, XmDIALOG_TEXT));
#endif
     XtAddCallback(xpShowIGDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Show Intentions")); 
     XtAddCallback(xpShowIGDialog, XmNokCallback, XpShowIGDialogUpdate, 0);
     XtAddCallback(xpShowIGDialog, XmNcancelCallback, xpShowIGDialogCancel, 0);

     n = 0;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpShowIGScrlTxt = XmCreateScrolledText(xpShowIGDialog,"xpShowIGScrlTxt",args,n);
     XtManageChild(xpShowIGScrlTxt);
}

Widget xpShowIScrlTxt, xpShowIDialog;

void xpShowIDialogUpdate()
{
     Arg args[1];
     static Sprinter *show_i_string = NULL;
     Intention *intention;

     if (! show_i_string)
	  show_i_string = make_sprinter(0);
     else
	  reset_sprinter(show_i_string);

     XtSetArg(args[0], XmNuserData,&intention);
     XtGetValues(xpShowIScrlTxt,args,1);

     if (valid_intention(current_oprs->intention_graph, intention)) {
	  sprint_show_intention(show_i_string, intention, current_oprs->intention_graph);
	  
	  XmTextSetString(xpShowIScrlTxt,SPRINTER_STRING(show_i_string));
/* 	  XtSetArg(args[0], XmNvalue,SPRINTER_STRING(show_i_string)); */
     } else
	  XmTextSetString(xpShowIScrlTxt,"This intention does not exist anymore...");
/* 	  XtSetArg(args[0], XmNvalue,"This intention does not exist anymore..."); */
     
/*      XtSetValues(xpShowIScrlTxt,args,1); */
}

void XpShowIDialogUpdate(Widget w, XtPointer client_data, XtPointer call_data)
{
     xpShowIDialogUpdate();
}

void xpShowIDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild(w);
}

void xpShowIDialogManage(Intention *in)
{
     Arg args[1];

     XtSetArg(args[0],  XmNuserData, in);
     XtSetValues(xpShowIScrlTxt,args,1);

     xpShowIDialogUpdate();
     XtManageChild(xpShowIDialog);
}

void xp_create_show_intention_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     
     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpShowIDialog =  XmCreateTemplateDialog(parent, "xpShowIDialog",args,n);
#else
     xpShowIDialog =  XmCreatePromptDialog(parent, "xpShowIDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowIDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowIDialog, XmDIALOG_TEXT));
#endif
     XtAddCallback(xpShowIDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Graphic Intention Pane")); 
     XtAddCallback(xpShowIDialog, XmNokCallback, XpShowIDialogUpdate, 0);
     XtAddCallback(xpShowIDialog, XmNcancelCallback, xpShowIDialogCancel, 0);

     n = 0;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpShowIScrlTxt = XmCreateScrolledText(xpShowIDialog,"xpShowIScrlTxt",args,n);
     XtManageChild(xpShowIScrlTxt);
}

void xpTraceIDialogUpdate(Widget w, char *string)
{
     XmTextInsert(w,XmTextGetLastPosition(w),string);
     XmTextShowPosition(w,XmTextGetLastPosition(w));
}

void xpTraceIDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     Arg args[1];
     Intention *intention;

     XtSetArg(args[0], XmNuserData,&intention);
     XtGetValues(w,args,1);

     intention->trace_dialog = NULL;
     intention->trace_scrl_txt = NULL;

     XtUnmanageChild(w);

     free_intention(intention);
}

void xp_create_trace_intention_dialog(Widget parent, Intention *in, char *title)
{
     Widget xpTraceIDialog, xpTraceIScrlTxt;
     Cardinal n;
     Arg args[MAXARGS];
     XmString to_free;

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n], XmNautoUnmanage, False); n++;
     XtSetArg(args[n], XmNdialogTitle, to_free = XmStringCreateLtoR(title, XmSTRING_DEFAULT_CHARSET)); n++;
     XtSetArg(args[n], XmNuserData, in); n++;
     dup_intention(in);
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpTraceIDialog = in->trace_dialog =  XmCreateTemplateDialog(parent, "xpTraceIDialog",args,n);
#else
     xpTraceIDialog = in->trace_dialog =  XmCreatePromptDialog(parent, "xpTraceIDialog",args,n);
     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceIDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceIDialog, XmDIALOG_TEXT));
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceIDialog, XmDIALOG_OK_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceIDialog, XmDIALOG_HELP_BUTTON));
#endif

     XmStringFree(to_free);

     XtAddCallback(xpTraceIDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Trace Intention")); 
     XtAddCallback(xpTraceIDialog, XmNcancelCallback, xpTraceIDialogCancel, 0);

     n = 0;
     XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpTraceIScrlTxt = in->trace_scrl_txt = XmCreateScrolledText(xpTraceIDialog,"xpTraceIScrlTxt",args,n);

     XtManageChild(xpTraceIScrlTxt);
     XtManageChild(xpTraceIDialog);
}

Widget xp_information;

void xp_information_create(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmString xmres;

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     XtSetArg(args[n], XmNdialogTitle, xmres = XmStringCreateLtoR("Information", XmSTRING_DEFAULT_CHARSET)); n++;
     xp_information = XmCreateInformationDialog(parent, "xp_information", args, n);
     XtUnmanageChild(XmMessageBoxGetChild(xp_information, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(xp_information, XmDIALOG_CANCEL_BUTTON));
     XmStringFree(xmres);
}

void xp_information_report(char *message)
{
     Arg args[1];
     XmString xmres;

     XtSetArg(args[0], XmNmessageString, xmres = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
     XtSetValues(xp_information, args, 1);
     XmStringFree(xmres);
     XtManageChild(xp_information);
}

void xpcreate_dialogs(Widget parent, Draw_Data *dd)
{
     int i;
     Cardinal n;
     Arg args[MAXARGS];

     Widget xpTraceMenu;
     Widget xpMetaOptionMenu;
     Widget xpUserTraceMenu;
#ifdef OPRS_PROFILING
     Widget xpProfilingOptionMenu;
#endif
     Widget xpRunOptionMenu;
     Widget xpCompilerOptionMenu;

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;

     xpTraceOpDialog = XmCreateSelectionDialog(parent, "xpTraceOpDialog",args,n);
     XtManageChild(XmSelectionBoxGetChild(xpTraceOpDialog, XmDIALOG_APPLY_BUTTON));
     XtAddCallback(xpTraceOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OP Text Trace"));
     XtAddCallback(xpTraceOpDialog, XmNokCallback, xpTraceOpDialogAccept, 0);
     XtAddCallback(xpTraceOpDialog, XmNapplyCallback, xpTraceOpDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceOpDialog, XmDIALOG_TEXT));

     n = 0;
     XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
     trace_radiobox = XmCreateRadioBox(xpTraceOpDialog, "trace_radiobox", args, n);
     XtManageChild(trace_radiobox);

     n = 0;
     XtSetArg(args[n], XmNset, False); n++;
     text_radiobutton = XmCreateToggleButton(trace_radiobox, "text_trace_radiobutton", args, n);
     XtManageChild(text_radiobutton);
     XtAddCallback(text_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)text_button_changed, NULL);

     graphic_radiobutton = XmCreateToggleButton(trace_radiobox, "graphic_trace_radiobutton", args, n);
     XtManageChild(graphic_radiobutton);
     XtAddCallback(graphic_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)graphic_button_changed, NULL);

     n= 0;
     XtSetArg(args[n], XmNset, True); n++;
     both_radiobutton= XmCreateToggleButton(trace_radiobox, "both_trace_radiobutton", args, n);
     XtManageChild(both_radiobutton);
     XtAddCallback(both_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)both_button_changed, NULL);

     n= 0;
     XtSetArg(args[n], XmNset, False); n++;
     step_radiobutton= XmCreateToggleButton(trace_radiobox, "step_trace_radiobutton", args, n);
     XtManageChild(step_radiobutton);
     XtAddCallback(step_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)step_button_changed, NULL);

     n= 0;
     XtSetArg(args[n], XmNset, False); n++;
     all_radiobutton= XmCreateToggleButton(trace_radiobox, "all_trace_radiobutton", args, n);
     XtManageChild(all_radiobutton);
     XtAddCallback(all_radiobutton, XmNvalueChangedCallback, (XtCallbackProc)all_button_changed, NULL);

     n=0;
     XtSetArg(args[n], XmNselectionPolicy,XmMULTIPLE_SELECT); n++;
     xpTraceOpScrollList = XmSelectionBoxGetChild(xpTraceOpDialog, XmDIALOG_LIST);
     XtSetValues(xpTraceOpScrollList, args, n);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpTraceDialog = XmCreatePromptDialog(parent, "xpTraceDialog",args,n);
     XtManageChild(XmSelectionBoxGetChild(xpTraceDialog, XmDIALOG_APPLY_BUTTON));
     XtAddCallback(xpTraceDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Trace"));
     XtAddCallback(xpTraceDialog, XmNokCallback, xpTraceDialogAccept, 0);
     XtAddCallback(xpTraceDialog, XmNapplyCallback, xpTraceDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpTraceDialog, XmDIALOG_TEXT));

     xpTraceMenu = XmCreateRowColumn(xpTraceDialog,"xpTraceMenu",NULL,0);
     XtManageChild(xpTraceMenu);

     init_debug_trace_widget_name_array();

     for(i = 0; i < MAX_DEBUG; i++) {
	  debug_trace_widget_array[i] = XmCreateToggleButton(xpTraceMenu,debug_trace_widget_name_array[i], NULL, 0);
	  if (debug_trace_widget_name_array[i] != NULL) { /* We only manage the "named" one */
	       XtManageChild(debug_trace_widget_array[i]);
	  }
     }

#ifdef OPRS_PROFILING
     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpProfilingOptionDialog = XmCreatePromptDialog(parent, "xpProfilingOptionDialog",args,n);
     XtAddCallback(xpProfilingOptionDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Profiling Option"));
     XtAddCallback(xpProfilingOptionDialog, XmNokCallback, xpProfilingOptionDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpProfilingOptionDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpProfilingOptionDialog, XmDIALOG_TEXT));

     xpProfilingOptionMenu = XmCreateRowColumn(xpProfilingOptionDialog,"xpProfilingOptionMenu",NULL,0);
     XtManageChild(xpProfilingOptionMenu);

     init_profiling_option_widget_name_array();

     for(i = 0; i < MAX_PROFILING_OPTION; i++) {
	  profiling_option_widget_array[i] = XmCreateToggleButton(xpProfilingOptionMenu,
							     profiling_option_widget_name_array[i], NULL, 0);
	  if (i == PROFILING) {
	       XtAddCallback(profiling_option_widget_array[i],
			     XmNvalueChangedCallback, (XtCallbackProc) OptionProfilingChanged, NULL);
	  }
	  XtManageChild(profiling_option_widget_array[i]);
     } 
#endif

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpMetaOptionDialog = XmCreatePromptDialog(parent, "xpMetaOptionDialog",args,n);
     XtAddCallback(xpMetaOptionDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Meta Level Option"));
     XtAddCallback(xpMetaOptionDialog, XmNokCallback, xpMetaOptionDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpMetaOptionDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpMetaOptionDialog, XmDIALOG_TEXT));

     xpMetaOptionMenu = XmCreateRowColumn(xpMetaOptionDialog,"xpMetaOptionMenu",NULL,0);
     XtManageChild(xpMetaOptionMenu);

     init_meta_option_widget_name_array();

     for(i = 0; i < MAX_META_OPTION; i++) {
	  meta_option_widget_array[i] = XmCreateToggleButton(xpMetaOptionMenu,
							     meta_option_widget_name_array[i], NULL, 0);
	  if (i == META_LEVEL) {
	       XtAddCallback(meta_option_widget_array[i],
			     XmNvalueChangedCallback, (XtCallbackProc) OptionMLChanged, NULL);
	  }
	  XtManageChild(meta_option_widget_array[i]);
     } 

     if (install_user_trace) {

	  n=0;
	  XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
	  XtSetArg(args[n], XmNautoUnmanage, True); n++;
	  xpUserTraceDialog = XmCreatePromptDialog(parent, "xpUserTraceDialog",args,n);
	  XtAddCallback(xpUserTraceDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Meta Level Option"));
	  XtAddCallback(xpUserTraceDialog, XmNokCallback, xpUserTraceDialogAccept, 0);

	  /* Now get rid of the things we don't want */
	  XtUnmanageChild(XmSelectionBoxGetChild(xpUserTraceDialog, XmDIALOG_SELECTION_LABEL));
	  XtUnmanageChild(XmSelectionBoxGetChild(xpUserTraceDialog, XmDIALOG_TEXT));

	  xpUserTraceMenu = XmCreateRowColumn(xpUserTraceDialog,"xpUserTraceMenu",NULL,0);
	  XtManageChild(xpUserTraceMenu);

	  init_user_trace_widget_name_array();

	  for(i = 0; i < MAX_USER_TRACE; i++) {
	       user_trace_widget_array[i] = XmCreateToggleButton(xpUserTraceMenu,
								 user_trace_widget_name_array[i], NULL, 0);
	       if (i == USER_TRACE) {
		    XtAddCallback(user_trace_widget_array[i],
				  XmNvalueChangedCallback, (XtCallbackProc) OptionUTChanged, NULL);
	       }
	       XtManageChild(user_trace_widget_array[i]);
	  } 
     }

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpRunOptionDialog = XmCreatePromptDialog(parent, "xpRunOptionDialog",args,n);
     XtAddCallback(xpRunOptionDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Run Option"));
     XtAddCallback(xpRunOptionDialog, XmNokCallback, xpRunOptionDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpRunOptionDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpRunOptionDialog, XmDIALOG_TEXT));

     xpRunOptionMenu = XmCreateRowColumn(xpRunOptionDialog,"xpRunOptionMenu",NULL,0);
     XtManageChild(xpRunOptionMenu);

     init_run_option_widget_name_array();

     for(i = 0; i < MAX_RUN_OPTION; i++) {
	  run_option_widget_array[i] = XmCreateToggleButton(xpRunOptionMenu,
							    run_option_widget_name_array[i], NULL, 0);
	  XtManageChild(run_option_widget_array[i]);
     }

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpCompilerOptionDialog = XmCreatePromptDialog(parent, "xpCompilerOptionDialog",args,n);
     XtAddCallback(xpCompilerOptionDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "OPRS Compiler/Parser Option"));
     XtAddCallback(xpCompilerOptionDialog, XmNokCallback, xpCompilerOptionDialogAccept, 0);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(xpCompilerOptionDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpCompilerOptionDialog, XmDIALOG_TEXT));

     xpCompilerOptionMenu = XmCreateRowColumn(xpCompilerOptionDialog,"xpCompilerOptionMenu",NULL,0);
     XtManageChild(xpCompilerOptionMenu);

     init_compiler_option_widget_name_array();

     for(i = 0; i < MAX_COMPILER_OPTION; i++) {
	  compiler_option_widget_array[i] = XmCreateToggleButton(xpCompilerOptionMenu,
								 compiler_option_widget_name_array[i], NULL, 0);
	  XtManageChild(compiler_option_widget_array[i]);
     }

     xp_quitQuestion = XmCreateQuestionDialog(parent, "xp_quitQuestion", NULL, 0);
     XtAddCallback(xp_quitQuestion, XmNhelpCallback, infoHelp, makeFileNode("oprs", "X-OPRS Quit"));
     XtAddCallback(xp_quitQuestion, XmNokCallback, ReallyQuit, NULL);

     xp_resetQuestion = XmCreateQuestionDialog(parent, "xp_resetQuestion", NULL, 0);
     XtAddCallback(xp_resetQuestion, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Control Button Menu"));
     XtAddCallback(xp_resetQuestion, XmNokCallback, ReallyReset, current_oprs);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     addFactGoalDialog = XmCreateSelectionDialog(parent, "addFactGoalDialog",args,n);
     XtAddCallback(addFactGoalDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Add Fact or Goal"));
     XtAddCallback(addFactGoalDialog, XmNokCallback, addFactGoalDialogAccept, 0);
     XtAddCallback(addFactGoalDialog, XmNapplyCallback, addFactGoalDialogAccept, 0);

     consultOPDialog = XmCreateSelectionDialog(parent, "consultOPDialog",args,n);
     XtAddCallback(consultOPDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Consult Relevant OP"));
     XtAddCallback(consultOPDialog, XmNokCallback, consultOPDialogAccept, 0);
     XtAddCallback(consultOPDialog, XmNapplyCallback, consultOPDialogAccept, 0);

     consultAOPDialog = XmCreateSelectionDialog(parent, "consultAOPDialog",args,n);
     XtAddCallback(consultAOPDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Consult Applicable OP"));
     XtAddCallback(consultAOPDialog, XmNokCallback, consultAOPDialogAccept, 0);
     XtAddCallback(consultAOPDialog, XmNapplyCallback, consultAOPDialogAccept, 0);

     consultDBDialog = XmCreateSelectionDialog(parent, "consultDBDialog",args,n);
     XtAddCallback(consultDBDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Consult Fact Database"));
     XtAddCallback(consultDBDialog, XmNokCallback, consultDBDialogAccept, 0);
     XtAddCallback(consultDBDialog, XmNapplyCallback, consultDBDialogAccept, 0);

     deleteDBDialog = XmCreateSelectionDialog(parent, "deleteDBDialog",args,n);
     XtAddCallback(deleteDBDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Delete Fact Database"));
     XtAddCallback(deleteDBDialog, XmNokCallback, deleteDBDialogAccept, 0);
     XtAddCallback(deleteDBDialog, XmNapplyCallback, deleteDBDialogAccept, 0);

     concludeDBDialog = XmCreateSelectionDialog(parent, "concludeDBDialog",args,n);
     XtAddCallback(concludeDBDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Conclude Fact Database"));
     XtAddCallback(concludeDBDialog, XmNokCallback, concludeDBDialogAccept, 0);
     XtAddCallback(concludeDBDialog, XmNapplyCallback, concludeDBDialogAccept, 0);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpDisplaySelectOpDialog = XmCreateSelectionDialog(parent, "xpDisplaySelectOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpDisplaySelectOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(xpDisplaySelectOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpDisplaySelectOpDialog, XmDIALOG_TEXT));
     XtAddCallback(xpDisplaySelectOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Display a particular OP"));
     XtAddCallback(xpDisplaySelectOpDialog, XmNokCallback, (XtCallbackProc)xpDisplaySelectOpDialogAccept, dd);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpDeleteSelectOpDialog = XmCreateSelectionDialog(parent, "xpDeleteSelectOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpDeleteSelectOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(xpDeleteSelectOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpDeleteSelectOpDialog, XmDIALOG_TEXT));
     XtAddCallback(xpDeleteSelectOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Delete a OP"));
     XtAddCallback(xpDeleteSelectOpDialog, XmNokCallback, (XtCallbackProc)xpDeleteSelectOpDialogAccept, dd);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpUnloadOpDialog = XmCreateSelectionDialog(parent, "xpUnloadOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpUnloadOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(xpUnloadOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpUnloadOpDialog, XmDIALOG_TEXT));
     XtAddCallback(xpUnloadOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "X-OPRS Unload OP File"));
     XtAddCallback(xpUnloadOpDialog, XmNokCallback, (XtCallbackProc)xpUnloadOpDialogAccept, dd);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     xpReloadOpDialog = XmCreateSelectionDialog(parent, "xpReloadOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpReloadOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(xpReloadOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpReloadOpDialog, XmDIALOG_TEXT));
     XtAddCallback(xpReloadOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Reload OP File"));
     XtAddCallback(xpReloadOpDialog, XmNokCallback, (XtCallbackProc)xpReloadOpDialogAccept, dd);

     n=0;
     XtSetArg(args[n], XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     changeMaxSizeDialog = XmCreatePromptDialog(parent, "changeMaxSizeDialog",args,n);
     XtAddCallback(changeMaxSizeDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Change Size Text pane"));
     XtAddCallback(changeMaxSizeDialog, XmNokCallback, changeMaxSizeDialogAccept, 0);

}

void XpShowUTDialogUpdate(Widget w, XtPointer client_data, XtPointer call_data)
{
     ClearTextWindow((Widget)client_data);
}

void xpShowUTDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild(w);
}


Widget make_xoprs_ut_window()
{
     Widget parent = x_oprs_top_level_widget;

     Widget xpShowUTDialog, xpShowUTScrlTxt;
     
     Cardinal n;
     Arg args[MAXARGS];
     
     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
#ifdef MOTIF_HAS_TEMPLATE_DIALOG
     xpShowUTDialog =  XmCreateTemplateDialog(parent, "xpShowUTDialog",args,n);
#else
     xpShowUTDialog =  XmCreatePromptDialog(parent, "xpShowUTDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowUTDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(xpShowUTDialog, XmDIALOG_TEXT));
#endif
/*      XtUnmanageChild(XmSelectionBoxGetChild(xpShowUTDialog, XmDIALOG_HELP_BUTTON)); */
     XtAddCallback(xpShowUTDialog, XmNcancelCallback, xpShowUTDialogCancel, 0);


     n = 0;
     XtSetArg(args[n], XmNeditMode,  XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNautoShowCursorPosition, False); n++;
     XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
     XtSetArg(args[n], XmNeditable, False); n++;
     XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
     xpShowUTScrlTxt = XmCreateScrolledText(xpShowUTDialog,"xpShowUTScrlTxt",args,n);

     XtAddCallback(xpShowUTDialog, XmNokCallback, XpShowUTDialogUpdate, xpShowUTScrlTxt);

     XtManageChild(xpShowUTScrlTxt);
     XtManageChild(xpShowUTDialog);

     return xpShowUTScrlTxt;
}

void rename_ut_window(Widget w, char *title)
{
     if (w) {
	  Arg args[1];
	  XmString to_free;

	  XtSetArg(args[0], XmNdialogTitle, to_free = XmStringCreateLtoR(title, XmSTRING_DEFAULT_CHARSET));
	  XtSetValues(XtParent(XtParent(w)), args, 1);
	  XmStringFree(to_free);
     }
}

void manage_ut_window(Widget w)
{
     if (w) XtManageChild(XtParent(XtParent(w)));
}

void unmanage_ut_window(Widget w)
{
     if (w) XtUnmanageChild(XtParent(XtParent(w)));
}


void delete_ut_window(Widget w)
{
     if (w) XtDestroyWidget(XtParent(XtParent(w)));
}
