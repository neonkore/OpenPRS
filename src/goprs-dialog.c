/*                               -*- Mode: C -*- 
 * goprs-dialog.c -- 
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand, LAAS/CNRS.
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

#include "constant.h"
#include "macro.h"

#include "xhelp.h"
#include "xhelp_f.h"
#include "oprs-type.h"
#include "op-structure.h"
#include "oprs.h"
#include "relevant-op.h"
#include "intention.h"
#include "goprs.h"
#include "gope-graphic.h"
#include "goprs-main.h"
#include "gope-graphic_f.h"
#include "goprs-intention.h"
#include "int-graph_f.h"
#include "oprs-main.h"
#include "oprs-sprint.h"
#include "oprs-sprint_f.h"
#include "goprs-dialog_f.h"

#include "database.h"
#include "database_f.h"

#include "relevant-op_f.h"

#include "xm2gtk_f.h"

Widget addFactGoalDialog;
Widget consultOPDialog;
Widget consultAOPDialog;
Widget consultDBDialog;
Widget concludeDBDialog;
Widget deleteDBDialog;

Widget addfactGoalEntry;
Widget consultOPEntry;
Widget consultAOPEntry;
Widget consultDBEntry;
Widget concludeDBEntry;
Widget deleteDBEntry;

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
Widget xpTraceOPDialog, xpTraceOPList;
/*
  Widget xpGraphicOpDialog;
  Widget xpGraphicOpScrollList;
*/

Widget xpDisplaySelectOpDialog;
Widget xpDeleteSelectOpDialog;
Widget xpOPFUnloadDialog, OPFUnloadOPFList;
Widget xpOPFReloadDialog, OPFReloadOPFList;
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

void resetQuestionManage()
{
  //  XtManageChild(xp_resetQuestion);
}


void xpTraceIDialogUpdate(Widget w, char *string)
{
  //     XmTextInsert(w,XmTextGetLastPosition(w),string);
  //     XmTextShowPosition(w,XmTextGetLastPosition(w));
}

void manage_ut_window(Widget w)
{
}

void delete_ut_window(Widget w)
{
}

void rename_ut_window(Widget w, char *title)
{
}

void unmanage_ut_window(Widget w)
{
}

Widget make_xoprs_ut_window()
{
}


Widget debug_trace_widget_array[MAX_DEBUG];

char *debug_trace_widget_name_array [MAX_DEBUG];

static void init_debug_trace_widget_name_array()
{
  debug_trace_widget_name_array[POST_GOAL] = "Post Goal";
  debug_trace_widget_name_array[POST_FACT] = "Post Fact";
  debug_trace_widget_name_array[RECEIVE_MESSAGE] = "Receive Message";
  debug_trace_widget_name_array[SEND_MESSAGE] = "Send Message";
  debug_trace_widget_name_array[DATABASE_FRAME] = "Database Frame";
  debug_trace_widget_name_array[DATABASE] = "Database";
  debug_trace_widget_name_array[CONCLUDE_FROM_PARSER] = "Conclude From Parser";
  debug_trace_widget_name_array[SUC_FAIL] = "Success/Failure of OP";
  debug_trace_widget_name_array[INTENTION_FAILURE] = "Intention Failure";
  debug_trace_widget_name_array[INT_FAIL_BIND] = "Intention Failue with Binding";
  debug_trace_widget_name_array[INTEND] = "Intend";
  debug_trace_widget_name_array[OP_COMPILER] = "OP Compiler";
  debug_trace_widget_name_array[RELEVANT_OP] = "Relevant OP";
  debug_trace_widget_name_array[SOAK] = "Soak";
  debug_trace_widget_name_array[GRAPHIC_OP] = "Graphic OP";
  debug_trace_widget_name_array[TEXT_OP] = "Text OP";
  debug_trace_widget_name_array[GRAPHIC_INTEND] = "Graphic Intention";
  debug_trace_widget_name_array[THREADING] = "Threading";
#ifdef OPRS_DEBUG
  debug_trace_widget_name_array[DATABASE_KEY] = "Database Key";
#else 
  debug_trace_widget_name_array[DATABASE_KEY] = NULL;
#endif
}

char *meta_option_widget_name_array [MAX_META_OPTION];
Widget meta_option_widget_array [MAX_META_OPTION];

static void init_meta_option_widget_name_array()
{
  meta_option_widget_name_array[META_LEVEL] = "Meta Level";
  meta_option_widget_name_array[SOAK_MF] = "Soak MF";
  meta_option_widget_name_array[FACT_INV] = "Fact Inv";
  meta_option_widget_name_array[GOAL_INV] = "Goal Inv";
  meta_option_widget_name_array[APP_OPS_FACT] = "App OPs Fact";
  meta_option_widget_name_array[APP_OPS_GOAL] = "App OPs Goal";
}

char *compiler_option_widget_name_array [MAX_COMPILER_OPTION];
Widget compiler_option_widget_array [MAX_COMPILER_OPTION];

static void init_compiler_option_widget_name_array()
{
  compiler_option_widget_name_array[CHECK_ACTION] = "Check Action";
  compiler_option_widget_name_array[CHECK_PFR] = "Check Pred/Func";
  compiler_option_widget_name_array[CHECK_SYMBOL] = "Check Symbol";
}

char *run_option_widget_name_array [MAX_RUN_OPTION];
Widget run_option_widget_array [MAX_RUN_OPTION];

static void init_run_option_widget_name_array()
{
  run_option_widget_name_array[EVAL_ON_POST] = "Eval On Post";
  run_option_widget_name_array[PAR_POST] = "Parralel Post";
  run_option_widget_name_array[PAR_INTEND] = "Parralel Intend";
  run_option_widget_name_array[PAR_EXEC] = "Parralel Execution";
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
     
  if (op) display_op_pos(op, dd, dd->cgcsp, 0, 0);
  else {
    clear_dd_window(dd);
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
     
  if (pop) display_op_pos(pop, dd, dd->cgcsp, 0, 0);
  else {
    clear_dd_window(dd);
    XClearWindow(XtDisplay(dd->canvas), dd->window);
    dd->op = NULL;
    XBell(XtDisplay(dd->canvas), 50);
  }
}

Widget create_dialog_with_combo_box_entry(const gchar *title, GtkWindow *parent, Widget *Entry)
{
  Widget dialog = 
    gtk_dialog_new_with_buttons(title,
				GTK_WINDOW(parent),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_OK,
				GTK_RESPONSE_ACCEPT,
				GTK_STOCK_CANCEL,
				GTK_RESPONSE_REJECT,
				NULL);

  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  
  Widget vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  GtkWidget *label = gtk_label_new(title);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  *Entry = gtk_combo_box_entry_new_text();
  //  gtk_entry_set_text(GTK_ENTRY(entry), "");
  /* g_signal_connect (G_OBJECT (*Entry), "key-press-event", */
  /*          G_CALLBACK (on_combo_key_press), dialog); */
   
  gtk_box_pack_start (GTK_BOX (vbox), *Entry, TRUE, TRUE, 0);

  return dialog;
}

void show_dialog_with_combo_box_entry(Widget dialog, Widget entry, const gchar *command)
{  
  gtk_widget_show_all (dialog);
  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  char *to_free;
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT:
      to_free = gtk_combo_box_get_active_text(GTK_COMBO_BOX(entry));
      if (to_free) {
	if (to_free[0] != '\0') {
	  char *s;
	  gtk_combo_box_append_text(GTK_COMBO_BOX(entry),to_free);
	  s = (char *)MALLOC(strlen(to_free) + 20);
	  sprintf(s, command, to_free);
	  send_command_to_parser(s);
	  FREE(s);
	}
	g_free(to_free);
      }
      break;
    default:
      break;
    }
     
  gtk_widget_hide (dialog);
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


enum
  {
    COLUMN_OP_NAME,
    COLUMN_OP_POINTER,
    COLUMN_TEXT_TRACE,
    COLUMN_GRAPHIC_TRACE,
    COLUMN_STEP_TRACE,
    NUM_COLUMNS
  };

enum
  {
    LIST_OPF_NAME = 0
  };

static void
trace_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data, int trace)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreeIter  iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
  gboolean flag;
  Op_Structure *op;

  /* get toggled iter */
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get(model, &iter, 
		     trace, &flag,
		     COLUMN_OP_POINTER, &op,
		     -1);
  /* Flip the value */
  flag ^= 1;

  switch (trace)
    {
    case COLUMN_TEXT_TRACE:
      op->text_traced = flag; 
      break;
    case COLUMN_GRAPHIC_TRACE:
      op->graphic_traced = flag; 
      break;
    case COLUMN_STEP_TRACE:
      op->step_traced = flag; 
      break;
    default:
      break;
    }

  /* set new value */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, trace, flag, -1);

  /* clean up */
  gtk_tree_path_free (path);
}


static void
text_trace_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
  trace_toggled (cell, path_str, data, COLUMN_TEXT_TRACE);
}

static void
graphic_trace_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
  trace_toggled (cell, path_str, data, COLUMN_GRAPHIC_TRACE);
}

static void
step_trace_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
  trace_toggled (cell, path_str, data, COLUMN_STEP_TRACE);
}


GtkWidget *init_oplist()
{
  GtkWidget *opList;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *opStore;

  opList = gtk_tree_view_new();

  opStore = gtk_list_store_new (NUM_COLUMNS,
				G_TYPE_STRING,
				G_TYPE_POINTER,
				G_TYPE_BOOLEAN,
				G_TYPE_BOOLEAN,
				G_TYPE_BOOLEAN);

  gtk_tree_view_set_model(GTK_TREE_VIEW(opList), 
			  GTK_TREE_MODEL(opStore));

  g_object_unref(opStore);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(opList), TRUE);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("OP List",
						    renderer, "text", COLUMN_OP_NAME, NULL);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN (column),
				  GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_OP_NAME);
  gtk_tree_view_append_column(GTK_TREE_VIEW(opList), column); 

  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled",
                    G_CALLBACK (text_trace_toggled), opStore);

  column = gtk_tree_view_column_new_with_attributes ("Text",
                                                     renderer,
                                                     "active", COLUMN_TEXT_TRACE,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
				   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 50);
  gtk_tree_view_append_column (GTK_TREE_VIEW(opList), column);


  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled",
                    G_CALLBACK (graphic_trace_toggled), opStore);

  column = gtk_tree_view_column_new_with_attributes ("Graphic",
                                                     renderer,
                                                     "active", COLUMN_GRAPHIC_TRACE,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
				   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 50);
  gtk_tree_view_append_column (GTK_TREE_VIEW(opList), column);


  renderer = gtk_cell_renderer_toggle_new ();
  g_signal_connect (renderer, "toggled",
                    G_CALLBACK (step_trace_toggled), opStore);

  column = gtk_tree_view_column_new_with_attributes ("Step",
                                                     renderer,
                                                     "active", COLUMN_STEP_TRACE,
						     NULL);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
				   GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 50);
  gtk_tree_view_append_column (GTK_TREE_VIEW(opList), column);

  return opList;
}


void update_oplist(GtkWidget *opList)
{
  GtkListStore *opStore;
  GtkTreeIter iter;
  Op_Structure *op;

  opStore = GTK_LIST_STORE(gtk_tree_view_get_model
			   (GTK_TREE_VIEW(opList)));

  gtk_list_store_clear(opStore);


  sl_sort_slist_func(current_oprs->relevant_op->op_list,sort_op_opf);
  
  sl_loop_through_slist(current_oprs->relevant_op->op_list, op, Op_Structure *) {
    if (!op->xms_name) build_op_xms_name(op);
    gtk_list_store_append(opStore, &iter);
    gtk_list_store_set(opStore, &iter, 
		       COLUMN_OP_NAME, op->xms_name, 
		       COLUMN_OP_POINTER, (gpointer)op,
		       COLUMN_TEXT_TRACE, op->text_traced, 
		       COLUMN_GRAPHIC_TRACE, op->graphic_traced, 
		       COLUMN_STEP_TRACE, op->step_traced, 
		       -1);
  }
}


GtkWidget *init_opflist()
{
  GtkWidget *opfList;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *opfStore;

  opfList = gtk_tree_view_new();
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(opfList), FALSE);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("OP File List",
						    renderer, "text", LIST_OPF_NAME, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(opfList), column);

  opfStore = gtk_list_store_new(1, G_TYPE_STRING);

  gtk_tree_view_set_model(GTK_TREE_VIEW(opfList), 
			  GTK_TREE_MODEL(opfStore));

  g_object_unref(opfStore);

  return opfList;
}

void update_opflist(GtkWidget *opfList)
{
  GtkListStore *opfStore;
  GtkTreeIter iter;
  char *opf;

  opfStore = GTK_LIST_STORE(gtk_tree_view_get_model
			    (GTK_TREE_VIEW(opfList)));
  gtk_list_store_clear(opfStore);

  sl_loop_through_slist(current_oprs->relevant_op->opf_list, opf, char *) {
    gtk_list_store_append(opfStore, &iter);
    gtk_list_store_set(opfStore, &iter, LIST_OPF_NAME, opf, -1);
  }
}


void xp_create_dialogs(Widget parent)
{
  addFactGoalDialog = create_dialog_with_combo_box_entry("Add fact or goal",	GTK_WINDOW(parent), &addfactGoalEntry);
  consultDBDialog = create_dialog_with_combo_box_entry("Consult Database", GTK_WINDOW(parent), &consultDBEntry);
  concludeDBDialog = create_dialog_with_combo_box_entry("Assert in the Database", GTK_WINDOW(parent), &concludeDBEntry);
  deleteDBDialog = create_dialog_with_combo_box_entry("Delete Fact(s) in the Database", GTK_WINDOW(parent), &deleteDBEntry);
  consultOPDialog = create_dialog_with_combo_box_entry("Consult Relevant OP", GTK_WINDOW(parent), &consultOPEntry);
  consultAOPDialog = create_dialog_with_combo_box_entry("Consult Applicable OP", GTK_WINDOW(parent), &consultAOPEntry);

  xpTraceDialog = gtk_dialog_new_with_buttons("OpenPRS Trace",
					      GTK_WINDOW(parent),
					      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_ACCEPT,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_REJECT,
					      NULL);
  
  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpTraceDialog));
  
  GtkWidget *label = gtk_label_new("Open PRS Trace");
  gtk_container_add (GTK_CONTAINER (content_area), label);

  init_debug_trace_widget_name_array();
  
  int i;

  for(i = 0; i < MAX_DEBUG; i++) {
    if (debug_trace_widget_name_array[i] != NULL) { /* We only create  the "named" one */
      debug_trace_widget_array[i] = gtk_check_button_new_with_label(debug_trace_widget_name_array[i]);
      gtk_container_add (GTK_CONTAINER (content_area), debug_trace_widget_array[i]);
    }
  }
   
  xpMetaOptionDialog = gtk_dialog_new_with_buttons("Meta Level Options",
						   GTK_WINDOW(parent),
						   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						   GTK_STOCK_OK,
						   GTK_RESPONSE_ACCEPT,
						   GTK_STOCK_CANCEL,
						   GTK_RESPONSE_REJECT,
						   NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpMetaOptionDialog));
  
  label = gtk_label_new("Meta Level Options");
  gtk_container_add (GTK_CONTAINER (content_area), label);

  init_meta_option_widget_name_array();
  
  for(i = 0; i < MAX_META_OPTION; i++) {
    if (meta_option_widget_name_array[i] != NULL) { /* We only create  the "named" one */
      meta_option_widget_array[i] = gtk_check_button_new_with_label(meta_option_widget_name_array[i]);
      gtk_container_add (GTK_CONTAINER (content_area), meta_option_widget_array[i]);
    }
  }
   
  xpRunOptionDialog = gtk_dialog_new_with_buttons("Run Options",
						  GTK_WINDOW(parent),
						  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						  GTK_STOCK_OK,
						  GTK_RESPONSE_ACCEPT,
						  GTK_STOCK_CANCEL,
						  GTK_RESPONSE_REJECT,
						  NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpRunOptionDialog));
  
  label = gtk_label_new("Run Options");
  gtk_container_add (GTK_CONTAINER (content_area), label);

  init_run_option_widget_name_array();
  
  for(i = 0; i < MAX_RUN_OPTION; i++) {
    if (run_option_widget_name_array[i] != NULL) { /* We only create  the "named" one */
      run_option_widget_array[i] = gtk_check_button_new_with_label(run_option_widget_name_array[i]);
      gtk_container_add (GTK_CONTAINER (content_area), run_option_widget_array[i]);
    }
  }
   
  xpCompilerOptionDialog = gtk_dialog_new_with_buttons("Compiler Options",
						       GTK_WINDOW(parent),
						       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpCompilerOptionDialog));
  
  label = gtk_label_new("Compiler Options");
  gtk_container_add (GTK_CONTAINER (content_area), label);

  init_compiler_option_widget_name_array();

  for(i = 0; i < MAX_COMPILER_OPTION; i++) {
    if (compiler_option_widget_name_array[i] != NULL) { /* We only create  the "named" one */
      compiler_option_widget_array[i] = gtk_check_button_new_with_label(compiler_option_widget_name_array[i]);
      gtk_container_add (GTK_CONTAINER (content_area), compiler_option_widget_array[i]);
    }
  }


  xpOPFUnloadDialog = gtk_dialog_new_with_buttons("Select OP File to unload",
						  GTK_WINDOW(parent),
						  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						  GTK_STOCK_OK,
						  GTK_RESPONSE_ACCEPT,
						  GTK_STOCK_CANCEL,
						  GTK_RESPONSE_REJECT,
						  NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpOPFUnloadDialog));

  Widget vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);
  
  label = gtk_label_new("Select OP File to unload");
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  OPFUnloadOPFList =  init_opflist();
  gtk_box_pack_start (GTK_BOX (vbox), OPFUnloadOPFList, TRUE, TRUE, 0);

  xpOPFReloadDialog = gtk_dialog_new_with_buttons("Select OP File to reload",
						  GTK_WINDOW(parent),
						  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						  GTK_STOCK_OK,
						  GTK_RESPONSE_ACCEPT,
						  GTK_STOCK_CANCEL,
						  GTK_RESPONSE_REJECT,
						  NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpOPFReloadDialog));

  vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);
  
  label = gtk_label_new("Select OP File to reload");
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  OPFReloadOPFList =  init_opflist();
  gtk_container_add (GTK_CONTAINER (content_area), OPFReloadOPFList);

  xpTraceOPDialog = gtk_dialog_new_with_buttons("Select OP Trace",
						GTK_WINDOW(parent),
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_OK,
						GTK_RESPONSE_ACCEPT,
						NULL);
  
  content_area = gtk_dialog_get_content_area(GTK_DIALOG(xpTraceOPDialog));
  
  vbox = gtk_vbox_new (FALSE, 8);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  label = gtk_label_new("Select Trace for OP");
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);


  Widget sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
				       GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				  GTK_POLICY_NEVER,
				  GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(sw, -1 , 250);

  gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

  

  xpTraceOPList =  init_oplist();

  gtk_container_add (GTK_CONTAINER (sw), xpTraceOPList);

}


void xpOPFUnloadDialogShow()
{
  update_opflist(OPFUnloadOPFList);

  gtk_widget_show_all(xpOPFUnloadDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpOPFUnloadDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      GtkTreeIter iter;
      GtkTreeModel *model;
      char *opf_name;
      OPFile *opf;

      if (gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(OPFUnloadOPFList)), &model, &iter)) {
	gtk_tree_model_get(model, &iter, LIST_OPF_NAME, &opf_name,  -1);
	
	if (strcmp(opf_name, "") != 0) {
	  char *s = (char *)MALLOC(strlen(opf_name) + 20);
	  sprintf(s,"delete opf \"%s\"\n", opf_name);
	  send_command_to_parser(s);
	  FREE(s);
	}
	g_free(opf_name);
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpOPFUnloadDialog);
}

void xpOPFReloadDialogShow()
{
  update_opflist(OPFReloadOPFList);

  gtk_widget_show_all(xpOPFReloadDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpOPFReloadDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      GtkTreeIter iter;
      GtkTreeModel *model;
      char *opf_name;
      OPFile *opf;

      if (gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(OPFReloadOPFList)), &model, &iter)) {
	gtk_tree_model_get(model, &iter, LIST_OPF_NAME, &opf_name,  -1);
	
	if (strcmp(opf_name, "") != 0) {
	  char *s = (char *)MALLOC(strlen(opf_name) + 20);
	  sprintf(s,"reload opf \"%s\"\n", opf_name);
	  send_command_to_parser(s);
	  FREE(s);
	}
	g_free(opf_name);
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpOPFReloadDialog);
}
void xpTraceOPDialogShow()
{
  update_oplist(xpTraceOPList);

  gtk_widget_show_all(xpTraceOPDialog);
  
  gtk_dialog_run (GTK_DIALOG (xpTraceOPDialog));
  
  gtk_widget_hide (xpTraceOPDialog);
}

void xpTraceDialogShow()
{
  int i;

  for (i = 0; i < MAX_DEBUG; i++) {
    if (debug_trace_widget_name_array[i])
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(debug_trace_widget_array[i]),debug_trace[i]);
  }

  gtk_widget_show_all(xpTraceDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpTraceDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      if ((! debug_trace[GRAPHIC_INTEND])
	  && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(debug_trace_widget_array[GRAPHIC_INTEND]))
	  && global_int_draw_data) {
	rebuilt_intention_graph_graphic(global_int_draw_data->ig);
      }
      if (debug_trace[GRAPHIC_INTEND]
	  && ( ! (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(debug_trace_widget_array[GRAPHIC_INTEND]))))
	  && global_int_draw_data) {
	// gtk XClearWindow(XtDisplay(global_int_draw_data->canvas), XtWindow(global_int_draw_data->canvas));
      }

      for (i = 0; i < MAX_DEBUG; i++) {
	if (debug_trace_widget_name_array[i])
	  debug_trace[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(debug_trace_widget_array[i]));
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpTraceDialog);
}

void xpMetaOptionDialogShow()
{
  int i;

  for (i = 0; i < MAX_META_OPTION; i++) {
    if (meta_option_widget_name_array[i])
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(meta_option_widget_array[i]),meta_option[i]);
  }

  gtk_widget_show_all(xpMetaOptionDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpMetaOptionDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      for (i = 0; i < MAX_META_OPTION; i++) {
	if (meta_option_widget_name_array[i])
	  meta_option[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(meta_option_widget_array[i]));
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpMetaOptionDialog);
}


void xpRunOptionDialogShow()
{
  int i;

  for (i = 0; i < MAX_RUN_OPTION; i++) {
    if (run_option_widget_name_array[i])
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(run_option_widget_array[i]),run_option[i]);
  }

  gtk_widget_show_all(xpRunOptionDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpRunOptionDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      for (i = 0; i < MAX_RUN_OPTION; i++) {
	if (run_option_widget_name_array[i])
	  run_option[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(run_option_widget_array[i]));
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpRunOptionDialog);
}


void xpCompilerOptionDialogShow()
{
  int i;

  for (i = 0; i < MAX_COMPILER_OPTION; i++) {
    if (compiler_option_widget_name_array[i])
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(compiler_option_widget_array[i]),compiler_option[i]);
  }

  gtk_widget_show_all(xpCompilerOptionDialog);
  
  gint result = gtk_dialog_run (GTK_DIALOG (xpCompilerOptionDialog));
  switch (result)
    {
    case GTK_RESPONSE_ACCEPT: {
      for (i = 0; i < MAX_COMPILER_OPTION; i++) {
	if (compiler_option_widget_name_array[i])
	  compiler_option[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(compiler_option_widget_array[i]));
      }
    }
      break;
    default:
      break;
    }
  
  gtk_widget_hide (xpCompilerOptionDialog);
}


#ifdef IGNORE
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

#endif
