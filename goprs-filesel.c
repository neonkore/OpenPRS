/*                               -*- Mode: C -*- 
 * goprs-filesel.c -- 
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
#include "goprs.h"
#include "xhelp.h"
#include "parser-funct.h"

#include "xhelp_f.h"
#include "oprs-init_f.h"

#include "xm2gtk_f.h"

Widget xp_includeFilesel, xp_loaddbFilesel, xp_loadopFilesel, xp_loadkrnFilesel, xp_savedbFilesel;
Widget xp_loadddbFilesel, xp_loaddopFilesel, xp_dumpdbFilesel, xp_dumpopFilesel, xp_dumpkrnFilesel;

void gtk_Fileselok(GtkWidget *dialog, char* command)
{
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    
    char s[LINSIZ];
    PBoolean res;
        
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
     
    if (strcmp(filename,"") != 0) {
      sprintf(s,command, filename);
      send_command_to_parser(s);
    }    
    g_free (filename);
  }
  gtk_widget_hide(dialog);
}

void xp_includeFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog,"include \"%s\"\n");
}

void xp_loadopFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "load opf \"%s\"\n");
}

void xp_savedbFileselok(GtkWidget *dialog)
{
    gtk_Fileselok(dialog, "save db \"%s\"\n");
}

void xp_loaddbFileselok(GtkWidget *dialog)
{
    gtk_Fileselok(dialog, "load db \"%s\"\n");
}

void xp_dumpdbFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "dump db \"%s\"\n");
}

void xp_dumpopFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "dump op  \"%s\"\n");
}

void xp_dumpkrnFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "dump kernel \"%s\"\n");
}

void xp_loadddbFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "load dump db \"%s\"\n");
}

void xp_loaddopFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "load dump op \"%s\"\n");
}

void xp_loadkrnFileselok(GtkWidget *dialog)
{
  gtk_Fileselok(dialog, "load dump kernel \"%s\"\n");
}

void xp_create_filesel(Widget parent)
{
  /* I am not sure this is the GTK best practice, but I create all file
     chooser dialogs and then show them when needed. */

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


  GtkFileFilter *filter_ddb = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_ddb, "*.ddb"); 
  gtk_file_filter_set_name(filter_ddb, "Dump Database Files"); 
  
  xp_loadddbFilesel = gtk_file_chooser_dialog_new ("Load Dumped Database File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_loadddbFilesel),filter_ddb);

  g_object_ref_sink(filter_ddb);
  
  xp_dumpdbFilesel = gtk_file_chooser_dialog_new ("Dump Database File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_SAVE,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_dumpdbFilesel),filter_ddb);


  GtkFileFilter *filter_dop = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_dop, "*.dop"); 
  gtk_file_filter_set_name(filter_dop, "Dump OP Files"); 
  
  xp_loaddopFilesel = gtk_file_chooser_dialog_new ("Load Dumped OP File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_loaddopFilesel),filter_dop);

  g_object_ref_sink(filter_dop);
  
  xp_dumpopFilesel = gtk_file_chooser_dialog_new ("Dump OP File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_SAVE,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_dumpopFilesel),filter_dop);


  GtkFileFilter *filter_krn = gtk_file_filter_new ();
  gtk_file_filter_add_pattern(filter_krn, "*.krn"); 
  gtk_file_filter_set_name(filter_krn, "OPrs Kenel Files"); 
  
  xp_loadkrnFilesel = gtk_file_chooser_dialog_new ("Load Kernel File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_OPEN,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_loadkrnFilesel),filter_krn);

  g_object_ref_sink(filter_krn);
  
  xp_dumpkrnFilesel = gtk_file_chooser_dialog_new ("Dump Kernel File", GTK_WINDOW(parent),
						   GTK_FILE_CHOOSER_ACTION_SAVE,
						   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						   NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(xp_dumpkrnFilesel),filter_krn);
}
