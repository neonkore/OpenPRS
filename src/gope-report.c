
/*                               -*- Mode: C -*- 
 * ope-report.c -- 
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
#include "macro.h"
#include "opaque.h"
#include "op-structure.h"
#include "parser-funct.h"
#include "gope-graphic.h"
#include "gope-global.h"
#include "gope-report_f.h"

#include "xhelp.h"
#include "xhelp_f.h"

#include "xm2gtk_f.h"

void ope_information_report(char *message)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new(GTK_WINDOW(topLevelWindow),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_INFO,
				  GTK_BUTTONS_OK,
				  "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Information");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void report_user_error(PString message)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(topLevelWindow),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_ERROR,
				  GTK_BUTTONS_OK,
				  "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}


void report_syntax_error(PString message)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(topLevelWindow),
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_ERROR,
				  GTK_BUTTONS_OK,
				  "%s%s", message,(parser_message?parser_message:""));
  gtk_window_set_title(GTK_WINDOW(dialog), "parser Error");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}
