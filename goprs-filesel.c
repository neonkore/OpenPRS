/*                               -*- Mode: C -*- 
 * oprs-filesel.c -- 
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

#include "xm2gtk.h"

#include "constant.h"
#include "oprs-type.h"
#include "xoprs.h"
#include "xhelp.h"
#include "parser-funct.h"

#include "xhelp_f.h"
#include "oprs-init_f.h"

#include "xm2gtk_f.h"

Widget xp_includeFilesel, xp_loaddbFilesel, xp_loadopFilesel, xp_loadkrnFilesel, xp_savedbFilesel;
Widget xp_loadddbFilesel, xp_loaddopFilesel, xp_dumpdbFilesel, xp_dumpopFilesel, xp_dumpkrnFilesel;

void xp_includeFileselok(GtkWidget *dialog)
{
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    
    char s[LINSIZ];
    PBoolean res;
        
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
     
    if (strcmp(filename,"") != 0) {
      sprintf(s,"include \"%s\"\n", filename);
      send_command_to_parser(s);
    }    
    g_free (filename);
  }
  gtk_widget_hide(dialog);
}

#ifdef IGNORE
void xp_loaddbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loaddbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadopFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load opf \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     XtFree(selected_file);
     }
}

void xp_savedbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_savedbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"save db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpdbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpdbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpopFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump op  \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_dumpkrnFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_dumpkrnFilesel,XmDIALOG_TEXT));
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"dump kernel \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadddbFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadddbFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump db \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loaddopFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loaddopFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump op \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}

void xp_loadkrnFileselok(Widget w, XtPointer client_data, XtPointer call_data)
{
     char * selected_file;
     char s[LINSIZ];

     XtUnmanageChild(w);
     selected_file = XmTextGetString(XmSelectionBoxGetChild(xp_loadkrnFilesel,XmDIALOG_TEXT));
     
     if (strcmp(selected_file,"") != 0) {
	  sprintf(s,"load dump kernel \"%s\"\n", selected_file);
	  send_command_to_parser(s);
     }
     XtFree(selected_file);
}
#endif

void xp_create_filesel(Widget parent)
{
  GtkFileFilter *filter_inc = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_inc, "*.inc"); 
  gtk_file_filter_set_name(filter_inc, "Inc Commands Files"); 
  
  xp_includeFilesel = gtk_file_chooser_dialog_new ("Load Commands File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_includeFilesel),filter_inc);

  GtkFileFilter *filter_opf = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_opf, "*.opf"); 
  gtk_file_filter_set_name(filter_opf, "OP Files"); 
  
  xp_loadopFilesel = gtk_file_chooser_dialog_new ("Load OP File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_loadopFilesel),filter_opf);


  GtkFileFilter *filter_db = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_db, "*.db"); 
  gtk_file_filter_set_name(filter_db, "Database Files"); 
  
  xp_loaddbFilesel = gtk_file_chooser_dialog_new ("Load Database File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_loaddbFilesel),filter_db);

  g_object_ref_sink(filter_db);
  
  xp_savedbFilesel = gtk_file_chooser_dialog_new ("Save Database File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_SAVE,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_savedbFilesel),filter_db);


     /* xp_savedbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_savedbFilesel",  args, n); */
     /* xp_loadddbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loadddbFilesel",  args, n); */
     /* xp_loaddopFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loaddopFilesel",  args, n); */
     /* xp_loadkrnFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_loadkrnFilesel",  args, n); */
     /* xp_dumpdbFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpdbFilesel",  args, n); */
     /* xp_dumpopFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpopFilesel",  args, n); */
     /* xp_dumpkrnFilesel = (Widget) XmCreateFileSelectionDialog(parent, "xp_dumpkrnFilesel",  args, n); */

}
