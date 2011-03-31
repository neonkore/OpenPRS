/*                               -*- Mode: C -*- 
 * gope-global.h -- 
 * 
 * $Id$
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

#ifndef INCLUDE_ope_global
#define INCLUDE_ope_global

#include "op-default.h"

typedef struct {
     Widget createOPBBoard;
     Widget act_bb;
     Widget cbb_graph_radiobutton;
     Widget cbb_name_field, cbb_ip_field, cbb_ctxt_field, cbb_set_field,
	  cbb_call_field,
            cbb_doc_field, cbb_act_field, cbb_eff_field, cbb_prop_field;
}      CreateOPBBoardStruct;

extern CreateOPBBoardStruct *createOPBBoardStruct;

typedef struct {
     Widget createEdgeForm;
     Widget text;
}      CreateEdgeStruct;

extern CreateEdgeStruct *createEdgeStruct;

typedef struct {
     Widget editObjectForm;
     Widget text;
     Widget width, fill_true, fill_false;
}      EditObjectStruct;

extern ListOPFile list_opfiles;

extern EditObjectStruct *editObjectStruct;

extern Widget topLevel;
extern Widget ope_information;
extern Widget printQuestion;
extern Widget op_information;
extern GtkWidget *opeLoadFilesel;
extern Widget opeAppendFilesel;
extern Widget opeSaveFilesel;
extern Widget printSaveFile;
extern Widget opeWriteFilesel;
extern Widget opeWriteTexFilesel;
extern Widget opeUnloadFileDialog;
extern Widget opeSelectFileDialog;

extern Widget selectOpDialog;
extern Widget renameOpDialog;
extern Widget symbolListDialog;
extern Widget duplicateOpDialog;
extern Widget destroyOpQuestion;
extern Widget destroyOpDialog;
extern Widget copyOpDialog;
extern Widget pasteOpDialog;
extern Widget editedOpDialog;
extern Widget drawingSizeDialog, drawingSizeDialogTxtFldx, drawingSizeDialogTxtFldy;

extern Widget name_errorDialog;

/* extern Widget working_dialog; */
extern Widget syntax_error;
extern Widget user_error;

extern Widget printCommandDialog;
extern Widget VisibleFieldsDialog;

extern Widget editNodeNameDialog;

extern OPFile *current_opfile;

extern int pretty_width;
extern char *s_pretty_parsed;
/* extern char s_parsed[BUFSIZ]; */

extern char *file_name_for_copy;
extern char *file_name_for_print;
extern String gx_print_command_template;
extern String txt_print_command_template;

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

#endif
