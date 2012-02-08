static const char* const rcsid = "$Id$";

/*                               -*- Mode: C -*-
 * ope-filesel.c --
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

#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <X11/cursorfont.h>

#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "oprs-type_f.h"
#include "parser-funct_f.h"
#include "ope-graphic.h"
#include "ope-global.h"
#include "ope-filesel_f.h"
#include "ope-bboard_f.h"
#include "ope-save_f.h"
#include "lisp-list_f.h"
#include "ope-pxmw_f.h"
#include "ope-print_f.h"
#include "ope-external_f.h"
#include "ope-vsblf_f.h"
#include "ope-graphic_f.h"
#include "ope-report_f.h"
#include "relevant-op_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

Widget drawingSizeDialog, drawingSizeWA, drawingSizeDialogTxtFldx, drawingSizeDialogTxtLblx, drawingSizeDialogTxtLbly, drawingSizeDialogTxtFldy;

static int nb_modify = 0;

void modify_op_name (Op_Structure *op, Draw_Data *dd)
{
     char *name, *newname;
     int length, total_length;

     nb_modify++;

     name = op->name;
     length = strlen(name);
     total_length = length + sizeof (int) + 1;
     newname = (char *) MALLOC(total_length);
     if ( name[0] == '|'){ /* The name is an "idbar" */
	  strcpy(newname,name);
	  newname[length-1]='\0';
	  sprintf ( newname,"%s.%d|",newname,nb_modify);
	  DECLARE_ID(newname,op->name);

     } else {
	  sprintf ( newname,"%s.%d",name,nb_modify);
	  DECLARE_ID(newname,op->name);

     }
     FREE(newname);

     if (op->xms_name) XmStringFree(op->xms_name);
     op->xms_name = XmStringCreateLtoR(op->name, XmSTRING_DEFAULT_CHARSET); /* Just the name */

     free_op_title(op->op_title);
     op->op_title = make_op_title(dd, op->name);
}


PBoolean check_duplicated_names(Draw_Data *dd)
{
     void *ptr1, *ptr2;
     char *name1, *name2;
     Op_Structure *op1, *op2;
     PBoolean no_duplicated = TRUE;
     Arg args[1];
     int total_length;

     char *previous_message, *message;
     char *duplicated_str_1 =  "The names of some OP were already used.\n\nThe following OP(s) have been renamed :\n";
     char *duplicated_str_2 =  "    in    ";


     total_length = strlen (duplicated_str_1) + 1;
     previous_message = message = (char *) MALLOC(total_length); 
     strcpy(message, duplicated_str_1);

     ptr1 = sl_get_slist_next(current_opfile->list_op, NULL);
     op1 = (Op_Structure *) ptr1;
     name1 = op1->name;

     while ( (ptr1 = sl_get_slist_next(current_opfile->list_op, ptr1)) != NULL){
	  ptr2 = ptr1;
	  while (ptr2 != NULL) {
	       op2 = (Op_Structure *) ptr2;
	       name2 = op2->name;	
	       if (strcmp(name1, name2) == 0) {
		    modify_op_name (op2, dd);

		    total_length += strlen(name1) + strlen (op2->name) + strlen (duplicated_str_2) + 1;
		    message = (char *) MALLOC(total_length); 
		    sprintf(message, "%s%s%s%s\n", previous_message, name1, duplicated_str_2, op2->name);
		    FREE(previous_message);
		    previous_message = message;
		    no_duplicated = FALSE;
	       }
	       ptr2 = sl_get_slist_next(current_opfile->list_op, ptr2);
	  }
	  op1 = (Op_Structure *) ptr1;
	  name1 = op1->name;	  
     }

     if (!no_duplicated){
	  XtSetArg(args[0], XmNmessageString, XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET));
	  XtSetValues(name_errorDialog, args, 1);
	  XtManageChild(name_errorDialog);
     }
     FREE(message);
     return (no_duplicated);
}


Widget name_errorDialog;

void ope_create_name_error(Widget parent)
{
     name_errorDialog =  XmCreateWarningDialog(parent, "name_errorDialog", NULL, 0);
     XtUnmanageChild(XmMessageBoxGetChild(name_errorDialog, XmDIALOG_HELP_BUTTON));
     XtUnmanageChild(XmMessageBoxGetChild(name_errorDialog, XmDIALOG_CANCEL_BUTTON));
}

PBoolean check_ep_in_ip(Op_Structure *op, Symbol predicat)
{
     return TRUE;
}

Widget opeUnloadFileDialog;

void opeUnloadFileDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *res;
     OPFile *opf;
     PBoolean select_another_file = FALSE;

     res = XmTextGetString(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_LIST));

     if (strcmp(res, "") != 0) {
	  sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	       if (strcmp(opf->name, res) == 0) {
		    if (! opf->modified ||
			(AskUser(w,"This OP File has been modified.\nDo you really want to Unload the file\nand loose the modifications done?"))) {

			 if ((current_opfile == opf)
			     || (current_opfile == NULL))
			      select_another_file = TRUE;

			 destroy_opfile(opf);
			 if (select_another_file) {
			      update_empty_sensitivity(False);
			      update_file_sensitivity(False);

			      if (sl_slist_empty(list_opfiles)) {
				   make_opfile(NULL, ACS_GRAPH);
				   add_op_file_name(current_opfile->name, relevant_op);
			      } else {
				   SelectOpFileDialogManage();
			      }
			      
			      UpdateTitleWindow();
			 }
		    }
		    break;
	       }
	  }
     }
     XtFree(res);
}

Widget opeSelectFileDialog;

void opeSelectFileDialogAccept(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *res;
     OPFile *opf;

     res = XmTextGetString(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_LIST));

     if (strcmp(res, "") != 0) {
	  int length_cmp = strlen(res);
	  if (res[length_cmp - 1] == '*'){
	       length_cmp -= 2;
	  }
	  sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	       if (strncmp(opf->name, res, length_cmp) == 0) {
		    select_opfile(opf);
		    UpdateTitleWindow();
		    if (sl_slist_empty(current_opfile->list_op)) {
			 update_empty_sensitivity(False);
		    } else {
			 update_empty_sensitivity(True);
			 selectOpDialogManage();
		    }
		    break;
	       }
	  }
     }
     XtFree(res);
}

Widget symbolListDialog;

PBoolean sort_string(PString a1, PString a2)
{
     return (strcmp(a1,a2) < 0);
}

void symbolListDialogUpdate(Widget w)
{
     
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int nb_item = 0;
     PString sym;

     item = (XmStringTable)XtCalloc((current_opfile ? sl_slist_length(current_opfile->list_sym) : 0) + 1,
				    sizeof(XmString));

     if (current_opfile) {
	  sl_sort_slist_func(current_opfile->list_sym, sort_string);

	  sl_loop_through_slist(current_opfile->list_sym, sym, PString) {
	       item[nb_item] = XmStringCreateLtoR(sym, XmSTRING_DEFAULT_CHARSET);
	       nb_item++;
	  }
     }

     item[nb_item] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, nb_item); n++;
     XtSetValues(symbolListDialog, args, n);

     XtFree( (char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(symbolListDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(symbolListDialog, XmDIALOG_TEXT),"");

}

void symbolListDialogManage(Widget w)
{
     symbolListDialogUpdate(w);
     XtManageChild(symbolListDialog);
}

void symbolListDialogAdd(Widget w, Draw_Data *dd, XtPointer call_data)
{
     PString sres;
     PString *res;

     res = &sres;

     sres = XmTextGetString(XmSelectionBoxGetChild(symbolListDialog, XmDIALOG_TEXT));
     
     if (!check_name(res)) {
	  return;
     }
     
     if ((strcmp(sres, "") != 0) && 
	 (sl_search_slist(current_opfile->list_sym, sres, equal_string) == NULL)) {
	  sl_add_to_tail(current_opfile->list_sym, sres);
	  symbolListDialogUpdate(w);
	  report_opfile_modification();
     }
}

void symbolListDialogDelete(Widget w, Draw_Data *dd, XtPointer call_data)
{
     PString res,elt;

     res = XmTextGetString(XmSelectionBoxGetChild(symbolListDialog, XmDIALOG_TEXT));
     if ((strcmp(res, "") != 0) && 
	 ((elt = (PString)sl_search_slist(current_opfile->list_sym, res, equal_string)) != NULL)) {
	  sl_delete_slist_node(current_opfile->list_sym, elt);
	  symbolListDialogUpdate(w);
	  report_opfile_modification();
     }
}

void symbolListDialogDismiss(Widget w, Draw_Data *dd, XtPointer call_data)
{
     XtUnmanageChild(w);
}

Widget selectOpDialog;

void selectOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *res;
     Op_Structure *op;

     res = XmTextGetString(XmSelectionBoxGetChild(selectOpDialog, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(selectOpDialog, XmDIALOG_LIST));

     if (strcmp(res, "") != 0)
	  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, res) == 0) {
		    select_op(op, dd);
		    break;
	       }
     XtFree(res);
}

/* Not used anymore 
void selectOpDialogCancel(Widget w, Draw_Data *dd, XtPointer call_data)
{
     reset_draw_data_no_redraw(dd);
     reset_draw_mode(dd);
}
*/

Widget renameOpDialog;

void renameOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *sname;
     Op_Structure *op;
     String to_free;

     sname = to_free = XmTextGetString(XmSelectionBoxGetChild(renameOpDialog, XmDIALOG_TEXT));

     if (!check_op_name(&sname)) {
	  XtFree(to_free);
	  return;
     }
     XtFree(to_free);

     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	  if (strcmp(op->name, sname) == 0) {
	       if (strcmp(current_op->name, sname) != 0)
		    report_syntax_error(LG_STR("This name is already used by another OP.",
					       "This name is already used by another OP."));
	       FREE(sname);
	       return;
	  }

     remove_last_selected_list(current_opfile->name, current_op->name);
     current_op->name = sname;
     update_last_selected_list(current_opfile->name, current_op->name);

     erase_og(dd->canvas, dd, current_op->op_title);
     free_op_title(current_op->op_title);

     if (current_op->xms_name) XmStringFree(current_op->xms_name);
     current_op->xms_name = XmStringCreateLtoR(current_op->name, XmSTRING_DEFAULT_CHARSET); /* Just the name */

     current_op->op_title = make_op_title(dd, current_op->name);
     draw_og(dd->canvas, dd, current_op->op_title);
     UpdateTitleWindow();
     report_opfile_modification();

     sl_sort_slist_func(current_opfile->list_op,sort_op);
}

Widget duplicateOpDialog;

void duplicateOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *sname;
     Op_Structure *op, *current_op_save = current_op;
     String to_free;

     sname = to_free = XmTextGetString(XmSelectionBoxGetChild(duplicateOpDialog, XmDIALOG_TEXT));

     if (!check_op_name(&sname)) {
	  XtFree(to_free);
	  return;
     }
     XtFree(to_free);

     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	  if (strcmp(op->name, sname) == 0) {
	       report_syntax_error(LG_STR("This name is already used by another OP.",
					  "This name is already used by another OP."));
	       FREE(sname);
	       return;
	  }

     copy_op (current_op, sname);
     
     current_op = current_op_save; /* copy_op use current_op , we need to save it. */
     report_opfile_modification();

     /* select the duplicated op */
     sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	  if (strcmp(op->name, sname) == 0) {
	       select_op(op, dd);
	       break;
	  }

     FREE(sname);
}

Widget destroyOpQuestion;

void destroyOpQuestionAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     if (current_op != NULL)
	  destroy_op(current_op, dd);
     else
	  fprintf(stderr,LG_STR("you cannot destroy the Current Op, if it does not exist \n",
				"you cannot destroy the Current Op, if it does not exist \n"));

}

Widget destroyOpDialog;

void destroyOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *res;
     Op_Structure *op;

     res = XmTextGetString(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_LIST));

     if (strcmp(res, "") != 0) {
	  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, res) == 0) {
		    destroy_op(op, dd);
		    break;
	       }
     }
     XtFree(res);
}

Widget copyOpDialog;

void copyOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *sname;
     Op_Structure *op, *current_op_save = current_op;
     OPFile *current_opfile_save = current_opfile;
     String to_free;

     sname = to_free = XmTextGetString(XmSelectionBoxGetChild(copyOpDialog, XmDIALOG_TEXT));

     if (!check_op_name(&sname)) {
	  XtFree(to_free);
	  return;
     }
     XtFree(to_free);

     sl_loop_through_slist(buffer_opfile->list_op, op, Op_Structure *)
	  if (strcmp(op->name, sname) == 0) {
	       report_syntax_error(LG_STR("This name is already used by another OP in the buffer File",
					  "This name is already used by another OP in the buffer File"));
	       FREE(sname);
	       return;
	  }
     
     current_opfile = buffer_opfile;
     copy_op (current_op, sname);
     FREE(sname);
     update_buffer_sensitivity(True);

     current_op = current_op_save;
     current_opfile = current_opfile_save;
}

Widget pasteOpDialog;
Widget pasteOpScrollList;

void pasteOpDialogManage()
{
     Arg args[3];
     Op_Structure *op;
     XmStringTable op_xmstring;
     int nop;
     
     nop = 0;

     op_xmstring =  (XmStringTable)XtCalloc(sl_slist_length(buffer_opfile->list_op) + 1,sizeof(XmString));

     sl_loop_through_slist(buffer_opfile->list_op, op, Op_Structure *) {
	  if (!op->xms_name) op->xms_name = XmStringCreateLtoR(op->name, XmSTRING_DEFAULT_CHARSET); /* Just the name */
	  op_xmstring[nop] = op->xms_name;
	  nop++;
     }

     XtSetArg(args[0],  XmNitems, op_xmstring);
     XtSetArg(args[1],  XmNitemCount, nop);
     XtSetValues(pasteOpScrollList, args, 2);

     XmListDeselectAllItems(pasteOpScrollList);

     XtFree((char *)op_xmstring);

     XtManageChild(pasteOpDialog);
}

void pasteOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     Arg args[2];
     char *sname;
     Op_Structure *op, *current_op_save = current_op, *last_pasted_op = NULL;
     XmStringTable op_sxmstring;
     String to_free;
     int i, snop;

     XtSetArg(args[0],  XmNselectedItems, &op_sxmstring);
     XtSetArg(args[1],  XmNselectedItemCount, &snop);
     XtGetValues(pasteOpScrollList, args, 2);

/*     XmListDeselectAllItems(pasteOpScrollList); */
     for (i=0; i < snop; i++) {
	  XmStringGetLtoR(op_sxmstring[i], XmSTRING_DEFAULT_CHARSET,&sname);
	  to_free = sname;
	  if (!check_op_name(&sname)) {
	       XtFree(to_free);
	       XmListDeselectAllItems(pasteOpScrollList); 
	       return;
	  }
	  XtFree(to_free);
	  sl_loop_through_slist(current_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, sname) == 0) {
		    report_syntax_error(LG_STR("This name is already used by another OP.",
					       "This name is already used by another OP."));
		    FREE(sname);
		    XmListDeselectAllItems(pasteOpScrollList); 
		    return;
	       }
	  sl_loop_through_slist(buffer_opfile->list_op, op, Op_Structure *)
	       if (strcmp(op->name, sname) == 0){
		    copy_op (op, sname);
		    report_opfile_modification();
		    break;
	       }
	  last_pasted_op = current_op;
	  FREE(sname);
     }
     XmListDeselectAllItems(pasteOpScrollList); 
     current_op = current_op_save;
	  

     /* select the last "pasted" op */
     if (last_pasted_op != NULL) {
	  update_empty_sensitivity(True);
	  select_op(last_pasted_op, dd);
     }
}

Widget last_selectedOpDialog;

void last_selectedOpDialogManage(void)
{
     Cardinal n;
     Arg args[MAXARGS];
     XmStringTable item;
     int i = 0;
     OPFullNameStruct *tmp;


     item = (XmStringTable)XtCalloc(sl_slist_length(list_last_selected_ops) + 1,sizeof(XmString));
     
     sl_loop_through_slist(list_last_selected_ops, tmp, OPFullNameStruct *) {
	  item[i] =tmp->xms_full_name;
	  i++;
     }

     item[i] = NULL;

     n = 0;
     XtSetArg(args[n], XmNlistItems, item); n++;
     XtSetArg(args[n], XmNlistItemCount, i); n++;
     XtSetValues(last_selectedOpDialog, args, n);

     XtFree((char *)item);

     XmListDeselectAllItems(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_LIST));
     XmTextSetString(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_TEXT),"");
     XtManageChild(last_selectedOpDialog);
}

void last_selectedOpDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     OPFullNameStruct *tmp;
     char *res, *file_name = NULL, *op_name = NULL;
     OPFile *opf;
     Op_Structure *op;

     res = XmTextGetString(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_LIST));

     if (strcmp(res, "") != 0) {
	  sl_loop_through_slist(list_last_selected_ops, tmp, OPFullNameStruct *)
	       if (strcmp(tmp->full_name, res) == 0) {
		    op_name = tmp->op_name;
		    file_name = tmp->file_name;
		    break;
	       }
	  if (op_name == NULL) {
	       fprintf(stderr, LG_STR("do not find OP in last_selectedOpDialogAccept \"%s\"\n",
				      "do not find OP in last_selectedOpDialogAccept \"%s\"\n"), res);
	       return;
	  }
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
     XtFree(res);
}

Widget opeLoadFilesel;

void opeLoadFileselok(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *selected_file;
     char s[LINSIZ];
     PBoolean res;
     OPFile *opf;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(opeLoadFilesel, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeLoadFilesel, XmDIALOG_LIST));

     sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	  if (strcmp(opf->name, selected_file) == 0) {
	       if (opf->modified && 
		   (! AskUser(w,"There is already a file under this name, which has been modified.\nDo you want to load the disk version and discard your modifications?"))) {
		    XtUnmanageChild(w);
		    XtFree(selected_file);
		    return;
		    }
	       destroy_opfile(opf);
	       break;
	  }
     }
     XtUnmanageChild(w);

     unselect_current_op(dd, TRUE);

     UpdateMessageWindow("Loading Op File.");
     sprintf(s, "compile_ops \"%s\"", selected_file);
     res = yyparse_one_command_string(s);

     UpdateMessageWindow("");
     current_op = NULL;   

     if (!res) {
	  report_syntax_error(LG_STR("Invalid Graph, or Invalid OP",
				     "Invalid Graph, or Invalid OP"));

     } else {
	  if (sl_slist_empty(current_opfile->list_op)) {
	       update_empty_sensitivity(False);
	  } else {
	       update_empty_sensitivity(True);
	       selectOpDialogManage();
	  }

	  symbolListDialogUpdate(symbolListDialog);
     }
     XtFree(selected_file);
}

Widget opeAppendFilesel;

void opeAppendFileselok(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char *selected_file;
     char s[LINSIZ];
     PBoolean res;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(opeAppendFilesel, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeAppendFilesel, XmDIALOG_LIST));

     if (strcmp(selected_file, "") != 0) {
	  fprintf(stderr, "opeAppend \"%s\"\n", selected_file);
     }

     XtUnmanageChild(w);

     unselect_current_op(dd, TRUE);
     UpdateTitleWindow();

     UpdateMessageWindow("Append Op File.");
     sprintf(s, "append_ops \"%s\"", selected_file);
     res = yyparse_one_command_string(s);

     current_op = NULL;   
     if (!res) {
	  UpdateMessageWindow("Append Op File : Failed.");
	  report_syntax_error(LG_STR("Invalid Grasper Graph, or Invalid OP",
				     "Invalid Grasper Graph, or Invalid OP"));
     } else {
	  UpdateMessageWindow("Append Op File : Done.");
	  if (!check_duplicated_names(dd)) {
	       printf (LG_STR("duplicated names \n",
			      "duplicated names \n"));
	  }
     }

     if (sl_slist_empty(current_opfile->list_op)) {
	  update_empty_sensitivity(False);
     } else {
	  update_empty_sensitivity(True);
	  selectOpDialogManage();
     }
     symbolListDialogUpdate(symbolListDialog);
     XtFree(selected_file);
}

Widget opeSaveFilesel;

void opeSaveFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *selected_file;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(opeSaveFilesel, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeSaveFilesel, XmDIALOG_LIST));

     if (strcmp(selected_file, "") != 0) {
	  if (! check_write_access(selected_file)) {
	       report_user_error(LG_STR("You do not have write access on this file.",
					"You do not have write access on this file."));
	       XtFree(selected_file);
	       return;
	  }
	  if (check_exist_access(selected_file) && 
	      strcmp(selected_file, current_opfile->name) != 0 &&
	      !(AskUser(w, "This file already exist,\ndo you really want to overwrite it?"))) {
	       XtFree(selected_file);
	       return;
	  }
	  rename_file_last_selected_list(current_opfile->name, selected_file);
	  change_op_file_name(current_opfile->name, selected_file, relevant_op);
	  current_opfile->name = selected_file; 
	  UpdateTitleWindow();
	  if (write_opfile(selected_file, current_opfile) == -1)
	        report_user_error(LG_STR("Error while writing the file.",
					 "Error while writing the file."));
	  current_opfile->filed = TRUE;
     } else
	  XtFree(selected_file);
}

Widget printSaveFile;

void printSaveFileok(Widget w,  Draw_Data *dd, XtPointer call_data)
{
     char *selected_file;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(printSaveFile, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(printSaveFile, XmDIALOG_LIST));

     XtUnmanageChild(printSaveFile);

     if (strcmp(selected_file, "") != 0) {
	  if (! check_write_access(selected_file)) {
	       report_user_error(LG_STR("You do not have write access on this file.",
					"You do not have write access on this file."));
	       XtFree(selected_file);
	       return;
	  }
	  if (check_exist_access(selected_file) && 
	      strcmp(selected_file, current_opfile->name) != 0 &&
	      !(AskUser(w, "This file already exist,\ndo you really want to overwrite it?"))) {
	       XtFree(selected_file);
	       return;
	  }
	  if (dump_op_pxm(dd, selected_file) == -1)
	        report_user_error(LG_STR("Error while writing the file.",
					 "Error while writing the file."));
     }
     XtFree(selected_file);
}


static Widget dialog = NULL;
static int locked = 0;

void TimeoutCursorsOK(Widget w, XtPointer client_data, XtPointer call_data)
{
     XSetWindowAttributes attrs;

     XtPopdown(XtParent(dialog));
     locked = 0;
     attrs.cursor = None;
     XChangeWindowAttributes(XtDisplay(topLevel), XtWindow(topLevel), CWCursor, &attrs);
}

void TimeoutCursors(PBoolean on, PString message)
{
    static Cursor cursor;
    
    XSetWindowAttributes attrs;
    Display *dpy = XtDisplay(topLevel);
    XEvent event;
    Arg args[1];
    XmString str;

    /* "locked" keeps track if we've already called the function.
     * This allows recursion and is necessary for most situations.
     */

    if (locked == 0 && ! on)	/* we are not locked, but we are asked to unlock... Probably  TimeoutCursorsOK has been called. */
	 return;

    on? locked++ : locked--;
    if (locked > 1 || (locked == 1 && on == 0))
        return; /* already locked and we're not unlocking */

    if (!cursor) /* make sure the timeout cursor is initialized */
        cursor = XCreateFontCursor(dpy, XC_watch);

    /* if "on" is true, then turn on watch cursor, otherwise, return
     * the shell's cursor to normal.
     */
    attrs.cursor = on? cursor : None;

    /* change the main application shell's cursor to be the timeout
     * cursor (or to reset it to normal).  If other shells exist in
     * this application, they will have to be listed here in order
     * for them to have timeout cursors too.
     */
    XChangeWindowAttributes(dpy, XtWindow(topLevel), CWCursor, &attrs);

    XFlush(dpy);

    if (on) {
	 if (! dialog) {
	      dialog = XmCreateWorkingDialog(topLevel, "Busy", NULL, 0);
	      XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)TimeoutCursorsOK, NULL);
	      XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	      XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
	      XtManageChild(dialog);
	 }

	 str = XmStringCreateSimple(message);
	 XtSetArg(args[0], XmNmessageString, str);
	 XtSetValues(dialog, args, 1);
	 XmStringFree(str);
	 XtPopup(XtParent(dialog),XtGrabNone);
	 XFlush(dpy);	
    } else {
        /* get rid of all button and keyboard events that occured
         * during the time out.  The user shouldn't have done anything
         * during this time, so flush for button and keypress events.
         * KeyRelease events are not discarded because accelerators
         * require the corresponding release event before normal input
         * can continue.
         */
        while (XCheckMaskEvent(dpy,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask
                | PointerMotionMask | KeyPressMask, &event)) {
            /* do nothing */;
        }
	XtPopdown(XtParent(dialog));
   }
}

Widget opeWriteFilesel;

void opeWriteFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *selected_file;
     OPFile *opf;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(opeWriteFilesel, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeWriteFilesel, XmDIALOG_LIST));

     if (strcmp(selected_file, "") != 0) {
	  /* fprintf(stderr, "opeWrite \"%s\"\n", selected_file); */
	  if (! check_write_access(selected_file)) {
	       report_user_error(LG_STR("You do not have write access on this file.",
					"You do not have write access on this file."));
	       XtFree(selected_file);
	       return;
	  }
	  if (check_exist_access(selected_file) && 
	      strcmp(selected_file, current_opfile->name) != 0 &&
	      !(AskUser(w, "This file already exist,\ndo you really want to overwrite it?"))) {
	       XtFree(selected_file);
	       return;
	  }
	  sl_loop_through_slist(list_opfiles, opf, OPFile *) {
	       if ((strcmp(opf->name, selected_file) == 0) &&
		   (opf != current_opfile)) {
		    if (!(AskUser(w, "There is already a loaded file under this name\ndo you really want to unload it?"))) {
			 XtFree(selected_file);
			 break;
		    } else
			 destroy_opfile(opf);
	       }
	  }
	  rename_file_last_selected_list(current_opfile->name, selected_file);
	  change_op_file_name(current_opfile->name, selected_file, relevant_op);
	  current_opfile->name = selected_file; 
	  UpdateTitleWindow();
	  if (write_opfile(selected_file, current_opfile) == -1)
	       report_user_error(LG_STR("Error while writing the file.",
					"Error while writing the file."));
	  current_opfile->filed = TRUE;
     } else {
	  XtFree(selected_file);
     }

}

Widget opeWriteTexFilesel;

void opeWriteTexFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char *selected_file;

     selected_file = XmTextGetString(XmSelectionBoxGetChild(opeWriteTexFilesel, XmDIALOG_TEXT));
     XmListDeselectAllItems(XmSelectionBoxGetChild(opeWriteTexFilesel, XmDIALOG_LIST));

     if (strcmp(selected_file, "") != 0) {
	  /* fprintf(stderr, "opeWriteTex \"%s\"\n", selected_file); */
	  if (! check_write_access(selected_file)) {
	       report_user_error(LG_STR("You do not have write access on this file.",
					"You do not have write access on this file."));
	       XtFree(selected_file);
	       return;
	  }
	  if (check_exist_access(selected_file) && 
	      strcmp(selected_file, current_opfile->name) != 0 &&
	      !(AskUser(w, "This file already exist,\ndo you really want to overwrite it?"))) {
	       XtFree(selected_file);
	       return;
	  }
	  if (write_opfile_tex(selected_file, current_opfile) == -1)
	       report_user_error(LG_STR("Error while writing the file.",
					"Error while writing the file."));
     }
     XtFree(selected_file);
}

/*
Widget      working_dialog;

void ope_create_working_dialog(Widget parent)
{
     Cardinal n;
     Arg args[MAXARGS];

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;
     working_dialog = XmCreateWorkingDialog(parent, "working_dialog",  args, n);
}
*/

void ope_create_filesel(Widget parent, Draw_Data *dd)
{
     Cardinal n;
     Arg args[MAXARGS];

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;

     opeLoadFilesel = (Widget) XmCreateFileSelectionDialog(parent, "opeLoadFilesel", args, n);
     XtAddCallback(opeLoadFilesel, XmNokCallback, (XtCallbackProc)opeLoadFileselok, dd);
     XtAddCallback(opeLoadFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "X-OPRS Load OP File"));

     opeAppendFilesel = (Widget) XmCreateFileSelectionDialog(parent, "opeAppendFilesel", args, n);
     XtAddCallback(opeAppendFilesel, XmNokCallback,  (XtCallbackProc)opeAppendFileselok, dd);
     XtAddCallback(opeAppendFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Append OP File"));

     opeSaveFilesel = (Widget) XmCreateFileSelectionDialog(parent, "opeSaveFilesel", args, n);
     XtAddCallback(opeSaveFilesel, XmNokCallback, opeSaveFileselok, 0);
     XtAddCallback(opeSaveFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Save OP File"));

     printSaveFile = (Widget) XmCreateFileSelectionDialog(parent, "printSaveFile", args, n);
     XtAddCallback(printSaveFile, XmNokCallback, (XtCallbackProc)printSaveFileok, dd);
     XtAddCallback(printSaveFile, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Dump OP Pixmap to File"));

     opeWriteFilesel = (Widget) XmCreateFileSelectionDialog(parent, "opeWriteFilesel", args, n);
     XtAddCallback(opeWriteFilesel, XmNokCallback, opeWriteFileselok, 0);
     XtAddCallback(opeWriteFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Write OP File"));

     opeWriteTexFilesel = (Widget) XmCreateFileSelectionDialog(parent, "opeWriteTexFilesel", args, n);
     XtAddCallback(opeWriteTexFilesel, XmNokCallback, opeWriteTexFileselok, 0);
     XtAddCallback(opeWriteTexFilesel, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Write TeX Doc File"));

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_MODELESS); n++;
     XtSetArg(args[n],  XmNautoUnmanage, False); n++;
     symbolListDialog = XmCreateSelectionDialog(parent, "symbolListDialog", args, n);
     XtAddCallback(symbolListDialog, XmNokCallback, (XtCallbackProc)symbolListDialogAdd, dd);
     XtAddCallback(symbolListDialog, XmNapplyCallback, (XtCallbackProc)symbolListDialogDismiss, dd);
     XtAddCallback(symbolListDialog, XmNcancelCallback, (XtCallbackProc)symbolListDialogDelete, dd);
     XtAddCallback(symbolListDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Symbols List"));

     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n], XmNautoUnmanage, True); n++;
     opeSelectFileDialog = XmCreateSelectionDialog(parent, "opeSelectFileDialog", args, n);
     XtUnmanageChild(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(opeSelectFileDialog, XmDIALOG_TEXT));
     XtAddCallback(opeSelectFileDialog, XmNokCallback, opeSelectFileDialogAccept, 0);
     XtAddCallback(opeSelectFileDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Select OP File"));

     opeUnloadFileDialog = XmCreateSelectionDialog(parent, "opeUnloadFileDialog", args, n);
     XtUnmanageChild(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(opeUnloadFileDialog, XmDIALOG_TEXT));
     XtAddCallback(opeUnloadFileDialog, XmNokCallback, opeUnloadFileDialogAccept, 0);
     XtAddCallback(opeUnloadFileDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Unload OP File"));

     selectOpDialog = XmCreateSelectionDialog(parent, "selectOpDialog", args, n);
     XtUnmanageChild(XmSelectionBoxGetChild(selectOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(selectOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(selectOpDialog, XmDIALOG_TEXT));
     XtAddCallback(selectOpDialog, XmNokCallback, (XtCallbackProc)selectOpDialogAccept, dd);
/*
     XtAddCallback(selectOpDialog, XmNcancelCallback, (XtCallbackProc)selectOpDialogCancel, dd);
*/
     XtAddCallback(selectOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Select OP"));

     renameOpDialog = XmCreatePromptDialog(parent, "renameOpDialog", args, n);
     XtAddCallback(renameOpDialog, XmNokCallback, (XtCallbackProc)renameOpDialogAccept, dd);
     XtAddCallback(renameOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Rename OP"));

     duplicateOpDialog = XmCreatePromptDialog(parent, "duplicateOpDialog", args, n);
     XtAddCallback(duplicateOpDialog, XmNokCallback, (XtCallbackProc)duplicateOpDialogAccept, dd);
     XtAddCallback(duplicateOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Duplicate OP"));

     destroyOpDialog = XmCreateSelectionDialog(parent, "destroyOpDialog", args, n);
     XtUnmanageChild(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(destroyOpDialog, XmDIALOG_TEXT));
     XtAddCallback(destroyOpDialog, XmNokCallback, (XtCallbackProc)destroyOpDialogAccept, dd);
     XtAddCallback(destroyOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Destroy A OP"));

     copyOpDialog = XmCreatePromptDialog(parent, "copyOpDialog", args, n);
     XtAddCallback(copyOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Copy OP in Buffer"));
     XtAddCallback(copyOpDialog, XmNokCallback, (XtCallbackProc)copyOpDialogAccept, dd);

     last_selectedOpDialog = XmCreateSelectionDialog(parent, "last_selectedOpDialog",args,n);
     XtUnmanageChild(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_APPLY_BUTTON));
     XtUnmanageChild(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_LIST_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(last_selectedOpDialog, XmDIALOG_TEXT));
     XtAddCallback(last_selectedOpDialog, XmNokCallback,  (XtCallbackProc)last_selectedOpDialogAccept, dd);
     XtAddCallback(last_selectedOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Last Selected Ops"));

     destroyOpQuestion = XmCreateQuestionDialog(parent, "destroyOpQuestion", NULL, 0);
     XtAddCallback( destroyOpQuestion, XmNokCallback, (XtCallbackProc)destroyOpQuestionAccept, dd );
     XtAddCallback( destroyOpQuestion, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Destroy Current Op"));
  
}

void drawingSizeDialogAccept(Widget w, Draw_Data *dd, XtPointer call_data)
{
     char s[LINSIZ];
     int xnewsize, ynewsize;
     String to_free;

     sprintf(s,"%s\n", to_free = XmTextGetString(drawingSizeDialogTxtFldx));
     XtFree(to_free);
     if (!sscanf ( s,"%d", &xnewsize ))
	  fprintf ( stderr, LG_STR("The X size must be an integer.\n",
				   "The X size must be an integer.\n"));
     else {
	  sprintf(s,"%s\n", to_free = XmTextGetString(drawingSizeDialogTxtFldy));
	  XtFree(to_free);
	  if (!sscanf ( s,"%d", &ynewsize ))
	       fprintf ( stderr, LG_STR("The Y size must be an integer.\n",
					"The Y size must be an integer.\n"));
	  change_canvas_size(dd, xnewsize, ynewsize);
     }

     return;
}

void ope_create_dialogs(Widget parent, Draw_Data *dd)
{
     Cardinal n;
     Arg args[MAXARGS];

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;
     drawingSizeDialog= XmCreatePromptDialog(parent, "drawingSizeDialog",args,n);
     XtAddCallback(drawingSizeDialog, XmNokCallback, (XtCallbackProc)drawingSizeDialogAccept, dd);
     XtAddCallback(drawingSizeDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Change Drawing Size"));

     XtUnmanageChild(XmSelectionBoxGetChild(drawingSizeDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(drawingSizeDialog, XmDIALOG_TEXT));

     drawingSizeWA= XmCreateWorkArea(drawingSizeDialog, "drawingSizeWA",args,n);
     XtManageChild(drawingSizeWA);

     XtManageChild(XmCreateLabel(drawingSizeWA,"drawingSizeDialogTxtLblx",NULL, 0));
     drawingSizeDialogTxtFldx = XmCreateTextField(drawingSizeWA,"drawingSizeDialogTxtFldx",NULL, 0);
     XtManageChild(drawingSizeDialogTxtFldx);
     XtManageChild(XmCreateLabel(drawingSizeWA,"drawingSizeDialogTxtLbly",NULL, 0));
     drawingSizeDialogTxtFldy = XmCreateTextField(drawingSizeWA,"drawingSizeDialogTxtFldy",NULL, 0);
     XtManageChild(drawingSizeDialogTxtFldy);

     n=0;
     XtSetArg(args[n],  XmNdialogStyle,XmDIALOG_APPLICATION_MODAL); n++;
     XtSetArg(args[n],  XmNautoUnmanage, True); n++;
     pasteOpDialog = XmCreatePromptDialog(parent, "pasteOpDialog",args,n);
     XtAddCallback(pasteOpDialog, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Paste OP from Buffer"));
     XtAddCallback(pasteOpDialog, XmNokCallback,  (XtCallbackProc)pasteOpDialogAccept, dd);

     /* Now get rid of the things we don't want */
     XtUnmanageChild(XmSelectionBoxGetChild(pasteOpDialog, XmDIALOG_SELECTION_LABEL));
     XtUnmanageChild(XmSelectionBoxGetChild(pasteOpDialog, XmDIALOG_TEXT));
 
     n = 0;
     XtSetArg(args[n],  XmNselectionPolicy,XmMULTIPLE_SELECT); n++;
     XtSetArg(args[n],  XmNvisibleItemCount,12); n++;
     pasteOpScrollList = XmCreateScrolledList(pasteOpDialog,"pasteOpScrollList",args,n);
     XtManageChild(pasteOpScrollList);

}

#define YES 1
#define NO  2

extern XtAppContext app_context;

void response_yes(Widget w, int *answer, XtPointer call_data)
{
     *answer = YES;
}

void response_no(Widget w, int *answer, XtPointer call_data)
{
     *answer = NO;
}

PBoolean AskUser(Widget parent, char *question)
{
    static Widget dialog;
    XmString text, yes, no;
    static int answer = 0;

    if (!dialog) {
        dialog = XmCreateQuestionDialog(parent, "dialog", NULL, 0);
        yes = XmStringCreateSimple("Yes");
        no = XmStringCreateSimple("No");
        XtVaSetValues(dialog,
            XmNdialogStyle,        XmDIALOG_SYSTEM_MODAL,
            XmNokLabelString,      yes,
            XmNcancelLabelString,  no,
            NULL);

	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

        XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)response_yes, &answer);
        XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc)response_no, &answer);
    }
    text = XmStringCreateLtoR(question, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues(dialog,
        XmNmessageString,      text,
        NULL);
    XmStringFree(text);
    XtManageChild(dialog);
    answer = 0;
    XtPopup(XtParent(dialog), XtGrabNone);

    /* while the user hasn't provided an answer, simulate XtMainLoop.
     * The answer changes as soon as the user selects one of the
     * buttons and the callback routine changes its value.  Don't
     * break loop until XtPending() also returns False to assure
     * widget destruction.
     */
    while (answer == 0 || XtAppPending(app_context))
        XtAppProcessEvent(app_context, XtIMAll);

    return (answer == YES);
}

PBoolean AskUserHelp(Widget parent, char *question, FileNode *helpnode)
{
    static Widget dialog;
    XmString text, yes, no;
    static int answer = 0;

    if (!dialog) {
        dialog = XmCreateQuestionDialog(parent, "dialog", NULL, 0);
        yes = XmStringCreateSimple("Yes");
        no = XmStringCreateSimple("No");
        XtVaSetValues(dialog,
            XmNdialogStyle,        XmDIALOG_SYSTEM_MODAL,
            XmNokLabelString,      yes,
            XmNcancelLabelString,  no,
            NULL);
	if (helpnode != NULL){
	     XtSetSensitive(
			    XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), True);
	     XtAddCallback(dialog, XmNhelpCallback, infoHelp, helpnode);

	} else
	     XtSetSensitive(
			    XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), False);
        XtAddCallback(dialog, XmNokCallback, (XtCallbackProc)response_yes, &answer);
        XtAddCallback(dialog, XmNcancelCallback, (XtCallbackProc)response_no, &answer);
    }
    text = XmStringCreateLtoR(question, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues(dialog,
        XmNmessageString,      text,
        NULL);
    XmStringFree(text);
    XtManageChild(dialog);
    answer = 0;
    XtPopup(XtParent(dialog), XtGrabNone);

    /* while the user hasn't provided an answer, simulate XtMainLoop.
     * The answer changes as soon as the user selects one of the
     * buttons and the callback routine changes its value.  Don't
     * break loop until XtPending() also returns False to assure
     * widget destruction.
     */
    while (answer == 0 || XtAppPending(app_context))
        XtAppProcessEvent(app_context, XtIMAll);

    return (answer == YES);
}
