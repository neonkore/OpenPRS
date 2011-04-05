/*                               -*- Mode: C -*-
 * ope-op-opf_f.h --
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

void update_last_selected_list(char *file_name, char *op_name);
void remove_last_selected_list(char *file_name, char *op_name);
void remove_file_last_selected_list(char *file_name);
void rename_file_last_selected_list(char *old_file_name, char *new_file_name);

void toggle_last_selected_ops(Draw_Data *dd);
void select_previous_op_in_same_file(Draw_Data *dd);
void select_previous_op_in_same_file(Draw_Data *dd);

void unselect_current_op(Draw_Data *dd, PBoolean update_sensitivity);
void select_op(Op_Structure * op, Draw_Data *dd);
void destroy_op(Op_Structure * op, Draw_Data *dd);
void destroy_opfile(OPFile *opf);
void select_opfile(OPFile *opf);
void make_opfile(PString ext_name, Opf_Type type);

void updateOpList(void);
void SelectOpFileDialogManage(void);

OPFile *make_buffer_opfile(void);
void clear_buffer_opfile(void);

/*
void open_working_dialog(PString message);
void close_working_dialog();
void report_working_dialog(PString message);
*/
void report_opfile_modification();
void report_opfile_saved();
PBoolean sort_opf(OPFile *opf1, OPFile *opf2);

void on_changed_oplist(GtkWidget *widget, gpointer label) ;
