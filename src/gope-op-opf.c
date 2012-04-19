/*                               -*- Mode: C -*-
 * ope-op-opf.c --
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

#include <gtk/gtk.h>

#include "xm2gtk.h"
#include "macro.h"
#include "constant.h"
#include "opaque.h"
#include "op-structure.h"

#include "gope-graphic.h"
#include "gope-global.h"
#include "oprs-type.h"
#include "gope-op-opf_f.h"
#include "gope-graphic_f.h"
#include "xm2gtk_f.h"

void updateOpfList(OPFile *opf);

void update_last_selected_list(char *file_name, char *op_name)
{
     OPFullNameStruct *last_selected_op, *tmp;
     char *full_name;

     last_selected_op = NULL;
     sl_loop_through_slist(list_last_selected_ops, tmp, OPFullNameStruct *) {
	  if ((strcmp(op_name, tmp->op_name) == 0)
	      && (strcmp(file_name, tmp->file_name) == 0)) {
	       last_selected_op = tmp;
	       break;
	  }
     }
     if (last_selected_op != NULL)
	  sl_delete_slist_node(list_last_selected_ops, last_selected_op);
     else {
	  last_selected_op = MAKE_OBJECT(OPFullNameStruct);
	  last_selected_op->file_name = file_name;
	  last_selected_op->op_name = op_name;
	  full_name= (char *)MALLOC (strlen(op_name) + strlen(file_name) + 5);

	  full_name[0] = '\0';
	  strcat(full_name, op_name);
	  strcat(full_name, "  [");
	  strcat(full_name, file_name);
	  strcat(full_name, "]");
	  last_selected_op->full_name = full_name;
	  last_selected_op->xms_full_name = XmStringCreateLtoR(full_name, XmSTRING_DEFAULT_CHARSET);
     }
     sl_add_to_head(list_last_selected_ops, last_selected_op);
     if (sl_slist_length(list_last_selected_ops) >= 1) 
	  update_toggle_sensitivity(TRUE);

}

void remove_last_selected_list(char *file_name, char *op_name)
{
     OPFullNameStruct *last_selected_op, *tmp;

     last_selected_op = NULL;
     sl_loop_through_slist(list_last_selected_ops, tmp, OPFullNameStruct *) {
	  if ((strcmp(op_name, tmp->op_name) == 0)
	      && (strcmp(file_name, tmp->file_name) == 0)) {
	       last_selected_op = tmp;
	       break;
	  }
     }
     if (last_selected_op != NULL) {
	  sl_delete_slist_node(list_last_selected_ops, last_selected_op);

	  XmStringFree(last_selected_op->xms_full_name);
	  FREE(last_selected_op->full_name);
	  FREE(last_selected_op);
     }
     if (sl_slist_length(list_last_selected_ops) <1) 
	  update_toggle_sensitivity(FALSE);
}

void remove_file_last_selected_list(char *file_name)
{
     OPFullNameStruct *last_selected_op, *tmp;
     ListLastSelectedOP to_del = sl_make_slist();

     last_selected_op = NULL;
     sl_loop_through_slist(list_last_selected_ops, tmp, OPFullNameStruct *) {
	  if (strcmp(file_name, tmp->file_name) == 0) {
	       sl_add_to_head(to_del, tmp);
	  }
     }
     sl_loop_through_slist(to_del, tmp, OPFullNameStruct *) {
	  sl_delete_slist_node(list_last_selected_ops, tmp);

	  XmStringFree(tmp->xms_full_name);
	  FREE(tmp->full_name);
	  FREE(tmp);
     }
     FREE_SLIST(to_del);
     if (sl_slist_length(list_last_selected_ops) <1) 
	  update_toggle_sensitivity(FALSE);
}

void rename_file_last_selected_list(char *old_file_name, char *new_file_name)
{
     OPFullNameStruct *last_selected_op;
     char *full_name;

     sl_loop_through_slist(list_last_selected_ops, last_selected_op, OPFullNameStruct *) {
	  if (strcmp(old_file_name, last_selected_op->file_name) == 0) {
	       last_selected_op->file_name = new_file_name;
	       XmStringFree(last_selected_op->xms_full_name);
	       FREE(last_selected_op->full_name);

	       full_name = (char *)MALLOC (strlen(last_selected_op->op_name) + strlen(new_file_name) + 5);
	       
	       full_name[0] = '\0';
	       strcat(full_name, last_selected_op->op_name);
	       strcat(full_name, "  [");
	       strcat(full_name, new_file_name);
	       strcat(full_name, "]");
	       last_selected_op->full_name = full_name;
	       last_selected_op->xms_full_name = XmStringCreateLtoR(full_name, XmSTRING_DEFAULT_CHARSET);
	  }
     }
}

void toggle_last_selected_ops(Draw_Data *dd)
{
     OPFullNameStruct *last_selected_op = NULL;
     char *file_name, *op_name;
     OPFile *opf;
     Op_Structure *op;

     if (current_op == NULL ) 	  /* in this case we take the first */
	  last_selected_op = (OPFullNameStruct *) sl_get_slist_pos(list_last_selected_ops, 1);
     else
	  last_selected_op = (OPFullNameStruct *) sl_get_slist_pos(list_last_selected_ops, 2);
	  
     if (last_selected_op  != NULL) {
	  op_name = last_selected_op->op_name;
	  file_name = last_selected_op->file_name;
	  
	  if ((current_opfile == NULL ) || (strcmp(current_opfile->name,file_name) != 0))
	       /* Select the Op File */
	       sl_loop_through_slist(list_opfiles, opf, OPFile *)
		    if (strcmp(opf->name, file_name) == 0) {
			 select_opfile(opf);
			 update_empty_sensitivity(TRUE); /* we know that this file is not empty */
			 break;
		    }
	  /* Select the Op */

	  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, op_name) == 0) {
		    select_op(op, dd);
		    break;
	       }
	  
	  
     } else if ((sl_slist_length(list_last_selected_ops) == 1) && (current_op != NULL )) 
	  return; /* nothing to do */
     else
     {
	  fprintf(stderr,LG_STR("Can't find the previous selected OP\n",
				"Can't find the previous selected OP\n"));

     }
}

void select_previous_op_in_same_file(Draw_Data *dd) 
{
     Op_Structure *op = NULL, *tmp;

     if (current_opfile == NULL) return;

     if (current_op == NULL) {
	  op = ( Op_Structure *) sl_get_slist_tail(current_opfile->list_op);
     } else
	  sl_loop_through_slist(current_opfile->list_op, tmp, Op_Structure *)
	  {
	       if (current_op == tmp)
		    break;
	       op = tmp;
	  }
	  
     if (op) select_op(op,dd);
     else {
	  unselect_current_op(dd, TRUE);
	  // XBell(XtDisplay(dd->canvas), 50);
     }
}

void select_next_op_in_same_file(Draw_Data *dd) 
{
     Op_Structure *op = NULL, *tmp;
     PBoolean get_next = FALSE;

     if (current_opfile == NULL) return;

     if (current_op == NULL) {
	  op = ( Op_Structure *) sl_get_slist_head(current_opfile->list_op);
     } else
	  sl_loop_through_slist(current_opfile->list_op, tmp, Op_Structure *)
	  {
	       if (get_next) {
		    op = tmp;
		    break;
	       } 
	        get_next = (current_op == tmp);
	  }
	  
     if (op) select_op(op,dd);
     else {
	  unselect_current_op(dd, TRUE);
	  XBell(XtDisplay(dd->canvas), 50);
     }
}

void unselect_current_op(Draw_Data *dd, PBoolean update_sensitivity)
{
     if (current_op != NULL) {
	  current_op->last_view_x = dd->left;
	  current_op->last_view_y = dd->top;
     }
     current_op = NULL;
     dd->op = NULL;
     
     reset_draw_data_no_redraw(dd);
     reset_draw_mode(dd);

     clear_dd_window(dd);

     if (update_sensitivity) update_select_sensitivity(FALSE);
}

void select_op(Op_Structure * op, Draw_Data *dd)
{
     if (dd->op != op) {
	  unselect_current_op(dd, FALSE);

	  current_op = op;
	  dd->op = op;
	  update_select_sensitivity(TRUE);

	  set_canvas_view(dd, op->last_view_x, op->last_view_y);
	  //	  XClearArea(XtDisplay(dd->canvas), dd->window, 0, 0, 0, 0, TRUE);

	  updateVisibleFieldsDialogIfManaged(dd);
	  UpdateTitleWindow();
	  update_last_selected_list(current_opfile->name, op->name);
	  gtk_widget_queue_draw(dd->canvas);
     }
}

void destroy_op(Op_Structure * op, Draw_Data *dd)
{
     remove_last_selected_list(current_opfile->name, op->name);
     sl_delete_slist_node(current_opfile->list_op, op);
     if (dd->op == op) {
	  unselect_current_op(dd, TRUE);

	  UpdateTitleWindow();
	  if (sl_slist_empty(current_opfile->list_op)) {
	       update_empty_sensitivity(FALSE);
	  } else {
	       update_empty_sensitivity(TRUE);
	       updateOpList();
	  }
     }
     delete_op_from_rop(op, relevant_op, TRUE, FALSE, TRUE);
     /* free_op(op); This is done by the call above */
     report_opfile_modification();
}

void destroy_opfile(OPFile *opf)
{
     remove_file_last_selected_list(opf->name);
     sl_delete_slist_node(list_opfiles, opf);

     if (current_opfile == opf) {
	  unselect_current_op(global_draw_data, TRUE);

	  current_opfile = NULL;
     
	  symbolListDialogUpdate(symbolListDialog);
     }
     delete_opf_from_rop(opf->name, relevant_op, TRUE, FALSE);
     FREE(opf->name);
/* Done by the above
     sl_loop_through_slist(opf->list_op, op, Op_Structure *) {
	  free_op(op);
     }
*/
     FREE_SLIST(opf->list_op);

     FREE(opf);
}

void select_opfile(OPFile *opf)
{
     if (current_opfile == opf) return;

     if (current_op != NULL) 
	  unselect_current_op(global_draw_data, TRUE);

     current_opfile = opf;
     if (! no_window) {
          symbolListDialogUpdate(symbolListDialog);

	  //       XClearWindow(XtDisplay(global_draw_data->canvas), global_draw_data->window);

       UpdateTitleWindow();
       update_file_sensitivity(TRUE);
     }
}

PBoolean sort_opf(OPFile *opf1, OPFile *opf2)
{
     return (strcmp(opf1->name,opf2->name) < 0);

}

PBoolean equal_opf(PString name, OPFile *opf)
{
     return (strcmp(name, opf->name) == 0);
}

void make_opfile(PString ext_name, Opf_Type type)
{
     static int i = 1;
     OPFile *opf_tmp, *opf = MAKE_OBJECT(OPFile);

     if (ext_name) {
       char *f;
       int d;
       NEWSTR(ext_name, opf->name);
       f = strrchr(ext_name,'/');
       if (f) {
	 NEWSTR(f+1,opf->fname);
	 NEWSTR(ext_name, opf->dname);
	 opf->dname[strlen(ext_name)-strlen(opf->fname)]='\0';
       } else {
	 NEWSTR(ext_name,opf->fname);
	 NEWSTR("", opf->dname);
       }
       opf->filed = TRUE;
     } else {
	  char name[30];

	  sprintf(name, LG_STR("Untitled-%d",
			       "Untitled-%d"), i++);
	  NEWSTR(name, opf->name);
	  NEWSTR(name, opf->fname);
	  NEWSTR("", opf->dname);
	  opf->filed = FALSE;
     }
     opf->type = type;
     opf->list_op = sl_make_slist();
     opf->list_sym = sl_make_slist();
     opf->modified = FALSE;
     opf->backed_up = FALSE;
     if ((opf_tmp = (OPFile *) sl_search_slist(list_opfiles, opf->name, equal_opf)) != NULL)
	  fprintf(stderr,LG_STR("already existing file name\n",
				"already existing file name\n"));
/*	  sl_delete_slist_node(list_opfiles, opf_tmp); */
     sl_add_to_tail(list_opfiles, opf);
     select_opfile(opf);
     updateOpfList(opf);
     if (! no_window) {
       update_empty_sensitivity(FALSE);
     }
}

OPFile *make_buffer_opfile()
{
     char name[30];
     OPFile *opf = MAKE_OBJECT(OPFile);


     sprintf(name, LG_STR("BufferFile",
			  "BufferFile"));
     NEWSTR(name, opf->name);
     opf->filed = FALSE;
     
     opf->type = ACS_GRAPH;
     opf->list_op = sl_make_slist();
     opf->list_sym = sl_make_slist();
     opf->backed_up = FALSE;
     opf->modified = FALSE;
     return (opf);
}

void clear_buffer_opfile(void)
{
     Op_Structure *op;

     sl_loop_through_slist(buffer_opfile->list_op, op, Op_Structure *) {
	  free_op(op);
     }
     FREE_SLIST(buffer_opfile->list_op);

     buffer_opfile->list_op = sl_make_slist();
     update_buffer_sensitivity(FALSE);

}

enum
{
  LIST_ITEM = 0,
  LIST_POINTER
};

void init_oplist(GtkWidget *list)
{

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *opStore;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("List Items",
          renderer, "text", LIST_ITEM, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

  opStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_tree_view_set_model(GTK_TREE_VIEW(list), 
      GTK_TREE_MODEL(opStore));

  g_object_unref(opStore);
}

void init_opflist(GtkWidget *list)
{

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *opfStore;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("List Items",
          renderer, "text", LIST_ITEM, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

  opfStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_tree_view_set_model(GTK_TREE_VIEW(list), 
      GTK_TREE_MODEL(opfStore));

  g_object_unref(opfStore);
}


static void
reset_oplist(GtkWidget *list)
{
  GtkListStore *opStore;

  opStore = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));
  gtk_list_store_clear(opStore);
}

static void
add_to_oplist(GtkWidget *list, const gchar *str, Op_Structure *op)
{
  GtkListStore *opStore;
  GtkTreeIter iter;

  opStore = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(opStore, &iter);
  gtk_list_store_set(opStore, &iter, LIST_ITEM, str, LIST_POINTER, (gpointer)op, -1);
}


static void
add_to_opflist(GtkWidget *list, const gchar *str, OPFile *opf)
{
  GtkListStore *opfStore;
  GtkTreeIter iter;

  opfStore = GTK_LIST_STORE(gtk_tree_view_get_model
			    (GTK_TREE_VIEW(list)));

  gtk_list_store_append(opfStore, &iter);
  gtk_list_store_set(opfStore, &iter, LIST_ITEM, str, LIST_POINTER, (gpointer)opf, -1);
}


void on_changed_opflist(GtkWidget *widget, gpointer label) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *opf_name;
  OPFile *opf;

  if (gtk_tree_selection_get_selected(
				      GTK_TREE_SELECTION(widget), &model, &iter)) {
    
    gtk_tree_model_get(model, &iter, LIST_ITEM, &opf_name, LIST_POINTER, &opf,  -1);

    //    gtk_label_set_text(GTK_LABEL(label), opf_name);
    
    /* OPFile *opf; */

    /* if (strcmp(opf_name, "") != 0) { */
    /*   int length_cmp = strlen(opf_name); */
    /*   if (opf_name[length_cmp - 1] == '*'){ */
    /* 	length_cmp -= 2; */
    /*   } */
    /*   sl_loop_through_slist(list_opfiles, opf, OPFile *) { */
    /* 	if (strncmp(opf->name, opf_name, length_cmp) == 0) { */
    select_opfile(opf);
    UpdateTitleWindow();
    if (sl_slist_empty(current_opfile->list_op)) {
      update_empty_sensitivity(FALSE);
    } else {
      update_empty_sensitivity(TRUE);
      updateOpList();
    }
    g_free(opf_name);
  }
}

void on_changed_oplist(GtkWidget *widget, gpointer label) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *ope_name;
  Op_Structure *op;


  if (gtk_tree_selection_get_selected(
				      GTK_TREE_SELECTION(widget), &model, &iter)) {

    gtk_tree_model_get(model, &iter, LIST_ITEM, &ope_name,  LIST_POINTER, &op,  -1);
    //    gtk_label_set_text(GTK_LABEL(label), ope_name);
    
    select_op(op, global_draw_data);
    g_free(ope_name);
  }
}


void updateOpfList(OPFile *opf)
{
       XmString fname, dname, mod, name2, name;
       fname = XmStringCreateLtoR(opf->fname, XmSTRING_DEFAULT_CHARSET);
       if (opf->modified) {
	 mod =  XmStringCreateLtoR(" * ", XmSTRING_DEFAULT_CHARSET);
       } else {
	 mod =  XmStringCreateLtoR(" ", XmSTRING_DEFAULT_CHARSET);
       }
       dname = XmStringCreateLtoR(opf->dname, XmSTRING_DEFAULT_CHARSET);
       name2 = XmStringConcat(fname, mod);
       name = XmStringConcat(name2, dname);
       /* should free this mess. */
       add_to_opflist(opfList, name, opf);
}

void updateOpList(void)
{
  int i = 0;
  Op_Structure *op;

  reset_oplist(opList);

  sl_sort_slist_func(current_opfile->list_op,sort_op);

  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *) {
    if (!op->xms_name) op->xms_name = XmStringCreateLtoR(op->name, XmSTRING_DEFAULT_CHARSET);
    add_to_oplist(opList, op->xms_name, op);
  }

}

void SelectOpFileDialogManage()
{
#ifdef IGNORE_GTK
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int nb_item = 0, j;
     OPFile *opf;

     item = (XmStringTable)XtCalloc(sl_slist_length(list_opfiles) + 1,sizeof(XmString));

     sl_sort_slist_func(list_opfiles, sort_opf);

     sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	  item[nb_item] = XmStringCreateLtoR(opf->name, XmSTRING_DEFAULT_CHARSET);
	  if (opf->modified) {
	       XmString x1,x2;
	       
	       x1 = item[nb_item];
	       x2 =  XmStringCreateLtoR(" *", XmSTRING_DEFAULT_CHARSET);
	       item[nb_item] = XmStringConcat(x1, x2);
	       XmStringFree(x1);
	       XmStringFree(x2);
	  }
	  nb_item++;
     }
     
     item[nb_item] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, nb_item); n++;
     XtSetValues(opeSelectFileDialog, args, n);

     for (j = 0 ; j < nb_item; j++) {
	  XmStringFree(item[j]);
     }

     XtFree((char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_TEXT),"");
     XtManageChild(opeSelectFileDialog);

#endif
}

void report_modification()
{
     current_opfile->modified = TRUE;
}

void report_opfile_modification()
{
     if (!(current_opfile->modified)) {
	  current_opfile->modified = TRUE;
	  UpdateTitleWindow();
     }
}

void report_opfile_saved()
{
     if (current_opfile->modified) {
	  current_opfile->modified = FALSE;
	  UpdateTitleWindow();
     }
}
