/*                               -*- Mode: C -*- 
 * gope-global.h -- 
 * 
 * Copyright (c) 1991-2013 Francois Felix Ingrand, LAAS/CNRS.
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

#ifndef INCLUDE_ope_global
#define INCLUDE_ope_global

#include "op-default.h"

typedef struct {
     GtkWidget *createOPBBoard;
     GtkWidget *act_bb;
     GtkWidget *cbb_graph_radiobutton;
     GtkWidget *cbb_name_field, *cbb_ip_field, *cbb_ctxt_field, *cbb_set_field,
	  *cbb_call_field, *cbb_doc_field, *cbb_act_field, *cbb_eff_field, *cbb_prop_field;
}      CreateOPBBoardStruct;

extern CreateOPBBoardStruct *createOPBBoardStruct;

typedef struct {
     GtkWidget *createEdgeForm;
     GtkWidget *text;
}      CreateEdgeStruct;

extern CreateEdgeStruct *createEdgeStruct;

typedef struct {
     GtkWidget *editObjectForm;
     GtkWidget *text;
     GtkWidget *width, *fill_true, *fill_false;
}      EditObjectStruct;

extern ListOPFile list_opfiles;

extern EditObjectStruct *editObjectStruct;

extern GtkWidget *topLevelWindow;
extern GtkWidget *ope_information;
extern GtkWidget *printQuestion;
extern GtkWidget *op_information;
extern GtkWidget *opeLoadFilesel;
extern GtkWidget *opeAppendFilesel;
extern GtkWidget *opeSaveFilesel;
extern GtkWidget *printSaveFile;
extern GtkWidget *opeWriteFilesel;
extern GtkWidget *opeWriteTexFilesel;
extern GtkWidget *opeUnloadFileDialog;
extern GtkWidget *opeSelectFileDialog;

extern GtkWidget *selectOpDialog;
extern GtkWidget *renameOpDialog;
extern GtkWidget *symbolListDialog;
extern GtkWidget *duplicateOpDialog;
extern GtkWidget *destroyOpQuestion;
extern GtkWidget *destroyOpDialog;
extern GtkWidget *copyOpDialog;
extern GtkWidget *pasteOpDialog;
extern GtkWidget *editedOpDialog;
extern GtkWidget *drawingSizeDialog, *drawingSizeDialogTxtFldx, *drawingSizeDialogTxtFldy;

extern GtkWidget *name_errorDialog;

/* extern GtkWidget *working_dialog; */
extern GtkWidget *syntax_error;
extern GtkWidget *user_error;

extern GtkWidget *printCommandDialog;
extern GtkWidget *VisibleFieldsDialog;

extern OPFile *current_opfile;

extern int pretty_width;
extern char *s_pretty_parsed;
/* extern char s_parsed[BUFSIZ]; */

extern char *file_name_for_copy;
extern char *file_name_for_print;
extern char *gx_print_command_template;
extern char *txt_print_command_template;

extern OPFile *buffer_opfile;

typedef struct {
     char *full_name;
     XmString xms_full_name;
     char *file_name;
     char *op_name;
} OPFullNameStruct;

extern ListLastSelectedOP list_last_selected_ops;
extern ListOP list_rops;

extern Relevant_Op *relevant_op;
extern PBoolean no_window;

extern GtkWidget *opList;
extern GtkWidget *opfList;

#endif
