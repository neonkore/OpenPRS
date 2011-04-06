/*                               -*- Mode: C -*-
 * gope-main.c -- GTK version of ope-main.c
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
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#include <gtk/gtk.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "op-structure.h"

#include "gope-graphic.h"
#include "gope-global.h"
#include "gope-main_f.h"
#include "gope-menu_f.h"
#include "gope-graphic_f.h"
#include "ope-edit_f.h"
#include "ope-bboard_f.h"
#include "gope-filesel_f.h"
#include "gope-op-opf_f.h"
#include "op-structure_f.h"
#include "parser-funct.h"
#include "ope-print_f.h"
#include "ope-rop_f.h"
#include "relevant-op_f.h"
#include "oprs-type_f.h"
#include "oprs-socket_f.h"

#include "version.h"
#include "xhelp.h"
#include "xhelp_f.h"

#include "ope-icon.bit"

#define OPE_ARG_ERR_MESSAGE LG_STR("Usage: ope [-l upper|lower|none ] [-F opfile]\n\
\t[-D files-directory] [opfile]*\n","Usage: ope [-l upper|lower|none ] [-F opfile]\n\
\t[-D files-directory] [opfile]*\n")

void UpdateTitleWindow();
void UpdateMessageWindow(char *string);
void make_opfile(PString ext_name, Opf_Type type);


Slist *ope_init_arg(int argc,char **argv);
void init_draw_mode_help();

Widget messageWindow, titleWindow, quitQuestion;

ListOPFile list_opfiles;
OPFile *current_opfile = NULL;
Relevant_Op *relevant_op;

ListLastSelectedOP list_last_selected_ops;

#define NAME_FOR_COPY "/tmp/Oprs_file_op_copy"
#define NAME_FOR_PRINT "/tmp/Oprs_file_op_print"

char *file_name_for_copy;
char *file_name_for_print;
OPFile *buffer_opfile;

PBoolean flushing_xt_events;
PBoolean no_window = FALSE;
int gtk = 1;

char *mp_hostname;
int mp_port;

Slist *ope_init_arg(int argc,char **argv)
{
     Slist *list_of_commands = sl_make_slist();
     char *command;

     char *id_case_option = NULL, *lang_str = NULL;
     char *selected_file, *selected_dir;
     char s[LINSIZ];
     int c, getoptflg = 0;
     int Dir_flg = 0;
     int lci_flg = 0, lang_flg =0;

     extern int optind;
     extern char *optarg;

     selected_dir = NULL; 		/* To avoid GCC warning... */

     while ((c = getopt(argc, argv, 
			"D:F:t:c:l:L:h"
			)) != EOF)
	  switch (c)
	  {
	  case 'D':
	       Dir_flg++; 
	       selected_dir = optarg;
	       break;
	  case 'F':
	       selected_file = optarg;
	       if (Dir_flg)
		    sprintf(s, "compile_ops \"%s/%s.opf\"", selected_dir, selected_file);
	       else
		    sprintf(s, "compile_ops \"%s\"", selected_file); 
	       NEWSTR(s, command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */

	       sprintf(s, "op-file: %s\n",selected_file);   /* the error message */
	       NEWSTR(s, command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 't':
	       optind--;	/* Back up one file name. */
	       no_window = TRUE;
	       for (; optind < argc; optind++) { /* The others files */
		    char * tex_file;

		    selected_file = argv[optind];
		    optind++;
		    tex_file= argv[optind];
		    sprintf(s, "convert_text_ops \"%s\" \"%s\"", selected_file, tex_file);
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);   /* the command line */

		    sprintf(s, "convert-text-file: %s in %s\n",selected_file, tex_file);   /* the error message */
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);
	       }
	       NEWSTR("exit", command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */
	       NEWSTR("error in exit", command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'c':
	       optind--;	/* Back up one file name. */
	       for (; optind < argc; optind++) { /* The others files */

		    selected_file = argv[optind];
		    sprintf(s, "convert_ops \"%s\" \"%s.bak\"", selected_file, selected_file);
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);   /* the command line */

		    sprintf(s, "convert-file: %s\n",selected_file);   /* the error message */
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);
	       }
	       NEWSTR("exit", command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */
	       NEWSTR("error in exit", command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'l':
	       lci_flg++;
	       id_case_option = optarg;
	       break;
	  case 'L':
	       lang_flg++;
	       lang_str = optarg;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
 
	  }
     if (getoptflg) {
	  fprintf(stderr, OPE_ARG_ERR_MESSAGE );
	  exit(1);
     }

     if (lang_flg) {
	  select_language(lang_str, FALSE);
     }
     if ( lci_flg ) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the arguments .\n",
				      "Invalid case option, check the arguments .\n"));
	  }
     } else if ((id_case_option = getenv("OPRS_ID_CASE")) != NULL) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the OPRS_ID_CASE environment variable.\n",
				      "Invalid case option, check the OPRS_ID_CASE environment variable.\n"));
	  }
     } 

     for (; optind < argc; optind++) { /* The others files */
	  selected_file = argv[optind];
	  if (Dir_flg)
	       sprintf(s, "compile_ops \"%s/%s.opf\"", selected_dir, selected_file);
	  else
	       sprintf(s, "compile_ops \"%s\"", selected_file);
	  NEWSTR(s, command);
	  sl_add_to_tail(list_of_commands, command);   /* the command line */

	  sprintf(s, "op-file: %s\n",selected_file);   /* the error message */
	  NEWSTR(s, command);
	  sl_add_to_tail(list_of_commands, command);
     }
     return(list_of_commands);

}

Slist *ope_init_nw_arg(int argc,char **argv)
{
     Slist *list_of_commands = sl_make_slist();
     char *command;

     char *id_case_option = NULL, *lang_str = NULL;
     char *selected_file, *selected_dir;
     char s[LINSIZ];
     int c, getoptflg = 0;
     int Dir_flg = 0;
     int lci_flg = 0, lang_flg =0;

     extern int optind;
     extern char *optarg;

     selected_dir = NULL; 		/* To avoid GCC warning... */

     while ((c = getopt(argc, argv, 
			"D:F:t:c:l:L:h"
			)) != EOF)
	  switch (c)
	  {
	  case 'D':
	       Dir_flg++; 
	       selected_dir = optarg;
	       break;
	  case 'F':
	       selected_file = optarg;
	       no_window = TRUE;
	       if (Dir_flg)
		    sprintf(s, "compile_ops \"%s/%s.opf\"", selected_dir, selected_file);
	       else
		    sprintf(s, "compile_ops \"%s\"", selected_file); 
	       NEWSTR(s, command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */

	       sprintf(s, "op-file: %s\n",selected_file);   /* the error message */
	       NEWSTR(s, command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 't':
	       optind--;	/* Back up one file name. */
	       no_window = TRUE;
	       for (; optind < argc; optind++) { /* The others files */
		    char * tex_file;

		    selected_file = argv[optind];
		    optind++;
		    tex_file= argv[optind];
		    sprintf(s, "convert_text_ops \"%s\" \"%s\"", selected_file, tex_file);
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);   /* the command line */

		    sprintf(s, "convert-text-file: %s in %s\n",selected_file, tex_file);   /* the error message */
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);
	       }
	       NEWSTR("exit", command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */
	       NEWSTR("error in exit", command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'c':
	       optind--;	/* Back up one file name. */
	       no_window = TRUE;
	       for (; optind < argc; optind++) { /* The others files */

		    selected_file = argv[optind];
		    sprintf(s, "convert_ops \"%s\" \"%s.bak\"", selected_file, selected_file);
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);   /* the command line */

		    sprintf(s, "convert-file: %s\n",selected_file);   /* the error message */
		    NEWSTR(s, command);
		    sl_add_to_tail(list_of_commands, command);
	       }
	       NEWSTR("exit", command);
	       sl_add_to_tail(list_of_commands, command);   /* the command line */
	       NEWSTR("error in exit", command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'l':
	       lci_flg++;
	       id_case_option = optarg;
	       break;
	  case 'L':
	       lang_flg++;
	       lang_str = optarg;
	       break;
	  case 'h':
	  default:
	       getoptflg++;
	  }

     if (lang_flg) {
	  select_language(lang_str, FALSE);
     }
     if ( lci_flg ) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the arguments .\n",
				      "Invalid case option, check the arguments .\n"));
	  }
     } else if ((id_case_option = getenv("OPRS_ID_CASE")) != NULL) {
	  if (! check_and_set_id_case_option(id_case_option)) {
	       fprintf(stderr, LG_STR("Invalid case option, check the OPRS_ID_CASE environment variable.\n",
				      "Invalid case option, check the OPRS_ID_CASE environment variable.\n"));
	  }
     } 

     for (; optind < argc; optind++) { /* The others files */
	  selected_file = argv[optind];
	  if (Dir_flg)
	       sprintf(s, "compile_ops \"%s/%s.opf\"", selected_dir, selected_file);
	  else
	       sprintf(s, "compile_ops \"%s\"", selected_file);
	  NEWSTR(s, command);
	  sl_add_to_tail(list_of_commands, command);   /* the command line */

	  sprintf(s, "op-file: %s\n",selected_file);   /* the error message */
	  NEWSTR(s, command);
	  sl_add_to_tail(list_of_commands, command);
     }
     return(list_of_commands);

}

void ReallyQuit()
{
  gtk_main_quit ();
  
  fprintf(stderr, LG_STR("May the OP Gtk Editor be with you...\n",
			 "Que le OP Gtk Editor soit avec vous...\n"));
  exit(0);
}

void quitQuestionManage(GtkWidget *w, gpointer window)
{
  GtkWidget *dialog;

  OPFile *opf;

  PBoolean unsaved = FALSE;
  char title[BUFSIZ];
  char *message = title;

  message += NUM_CHAR_SPRINT(sprintf(message, LG_STR("The following files are unsaved:\n",
						     "The following files are unsaved:\n")));

  sl_loop_through_slist(list_opfiles, opf, OPFile *) {
    if (opf->modified) {
      unsaved = TRUE;
      message += NUM_CHAR_SPRINT(sprintf(message, "%s\n", opf->name));
    }
  }
  if (unsaved) {
    message += NUM_CHAR_SPRINT(sprintf(message, LG_STR("\nDo you really want to quit?",
						       "\nDo you really want to quit?")));
    message = title;
  } else {
    message = "Do you really want to quit?";
  }
  
  dialog = gtk_message_dialog_new(window,
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  GTK_MESSAGE_QUESTION,
				  GTK_BUTTONS_YES_NO,
				  "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Question");

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (result)
    {
    case GTK_RESPONSE_YES:
      ReallyQuit();
      break;
    default:
      break;
    }
  gtk_widget_destroy (dialog);
}

void reset_dd(Draw_Data *dd)
{
     return;
}

struct _resrcs {
     XmFontList fontList;		  /* font to use for bitmap labels */
     int work_width, work_height;	  /* default bitmap size */
     String gxPrintCmd;
     String txtPrintCmd;
     Boolean  printWidgetTree;
     Boolean  printEnglishOperator;
     char *language;
     int  ip_x;
     int  ip_y;
     int  ip_width;
     int  ctxt_x;
     int  ctxt_y;
     int  ctxt_width;
     int  call_x;
     int  call_y;
     int  call_width;
     int  set_x;
     int  set_y;
     int  set_width;
     int  eff_x;
     int  eff_y;
     int  eff_width;
     int  prop_x;
     int  prop_y;
     int  prop_width;
     int  doc_x;
     int  doc_y;
     int  doc_width;
     int  act_x;
     int  act_y;
     int  act_width;
     int  bd_x;
     int  bd_y;
     int  bd_width;
     int  edge_width;
}       Resrcs;

#define DEFAULT_PRINT_COMMAND "xwdtopnm < %s | pnmcrop | pnmflip -ccw | pgmtops -r | lpr"

static XtResource resources[] = {
     {"workWidth", "WorkWidth", XmRInt, sizeof(int),
     XtOffsetOf(struct _resrcs, work_width), XmRImmediate, (char *) WORK_WIDTH},
     {"workHeight", "WorkHeight", XmRInt, sizeof(int),
     XtOffsetOf(struct _resrcs, work_height), XmRImmediate, (char *) WORK_HEIGHT},
     {"language", "Language", XmRString, sizeof(String),
     XtOffsetOf(struct _resrcs, language), XmRImmediate, NULL},

     {"ipX", "IpX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_x), XmRImmediate, (char *)IP_X},
     {"ipY", "IpY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_y), XmRImmediate, (char *)IP_Y},
     {"ipWidth", "IpWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ip_width), XmRImmediate, (char *)IP_WIDTH},
     {"ctxtX", "CtxtX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_x), XmRImmediate, (char *)CTXT_X},
     {"ctxtY", "CtxtY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_y), XmRImmediate, (char *)CTXT_Y},
     {"ctxtWidth", "CtxtWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_width), XmRImmediate, (char *)CTXT_WIDTH},
     {"callX", "CallX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_x), XmRImmediate, (char *)CALL_X},
     {"callY", "CallY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_y), XmRImmediate, (char *)CALL_Y},
     {"callWidth", "CallWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_width), XmRImmediate, (char *)CALL_WIDTH},
     {"setX", "SetX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_x), XmRImmediate, (char *)SET_X},
     {"setY", "SetY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_y), XmRImmediate, (char *)SET_Y},
     {"setWidth", "SetWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, set_width), XmRImmediate, (char *)SET_WIDTH},
     {"effX", "EffX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_x), XmRImmediate, (char *)EFF_X},
     {"effY", "EffY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_y), XmRImmediate, (char *)EFF_Y},
     {"effWidth", "EffWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, eff_width), XmRImmediate, (char *)EFF_WIDTH},
     {"propX", "PropX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_x), XmRImmediate, (char *)PROP_X},
     {"propY", "PropY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_y), XmRImmediate, (char *)PROP_Y},
     {"propWidth", "PropWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, prop_width), XmRImmediate, (char *)PROP_WIDTH},
     {"docX", "DocX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_x), XmRImmediate, (char *)DOC_X},
     {"docY", "DocY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_y), XmRImmediate, (char *)DOC_Y},
     {"docWidth", "DocWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, doc_width), XmRImmediate, (char *)DOC_WIDTH},
     {"actX", "ActX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_x), XmRImmediate, (char *)ACT_X},
     {"actY", "ActY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_y), XmRImmediate, (char *)ACT_Y},
     {"actWidth", "ActWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, act_width), XmRImmediate, (char *)ACT_WIDTH},
     {"bdX", "BdX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_x), XmRImmediate, (char *)BD_X},
     {"bdY", "BdY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_y), XmRImmediate, (char *)BD_Y},
     {"bdWidth", "BdWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, bd_width), XmRImmediate, (char *)BD_WIDTH},
     {"edgeWidth", "EdgeWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, edge_width), XmRImmediate, (char *)EDGE_WIDTH},

     {"printCommand", "PrintCommand", XtRString, sizeof(String),
     XtOffsetOf(struct _resrcs, gxPrintCmd), XtRString, (String)DEFAULT_PRINT_COMMAND},
     {"txtPrintCommand", "TxtPrintCommand", XtRString, sizeof(String),
     XtOffsetOf(struct _resrcs, txtPrintCmd), XtRString, (String)DEFAULT_PRINT_COMMAND},
     {"gxPrintCommand", "GxPrintCommand", XtRString, sizeof(String),
     XtOffsetOf(struct _resrcs, gxPrintCmd), XtRString, (String)DEFAULT_PRINT_COMMAND},
     {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     XtOffsetOf(struct _resrcs, fontList), XmRImmediate, NULL},
     {"printWidgetTree", "PrintWidgetTree", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printWidgetTree), XmRImmediate, (XtPointer)FALSE},
     {"printEnglishOperator", "PrintEnglishOperator", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printEnglishOperator), XmRImmediate, (XtPointer)FALSE},
};


Draw_Data *global_draw_data;
Draw_Data dd;
CairoGCs mainCGCs;		/* this will be the one for the main */
CairoGCs *mainCGCsp;		/* this will be the one for the main */


/* this is to allow our parsing of these options. */
static XrmOptionDescRec options[] = {
   {"-D",	"",	XrmoptionSkipArg,	NULL},
   {"-F",	"",	XrmoptionSkipArg,	NULL},
   {"-S",	"",	XrmoptionSkipArg,	NULL},
   {"-G",	"",	XrmoptionSkipArg,	NULL},
   {"-l",	"",	XrmoptionSkipArg,	NULL},
   {"-L",	"",	XrmoptionSkipArg,	NULL},
   {"-h",	"",	XrmoptionSkipNArgs,	0},
   {"-t",	"",	XrmoptionSkipLine,	NULL},
   {"-c",	"",	XrmoptionSkipLine,	NULL},
   {"-pwt",	"*printWidgetTree",	XrmoptionNoArg,	"True"},
   {"-peo",	"*printEnglishOperator",	XrmoptionNoArg,	"True"},
     };

/* String fallback_resources[] = {NULL}; */

String fallback_resources[] = {
#include "Ope.ad.h"
NULL
};

XtAppContext app_context;

extern PBoolean use_dialog_error;

/* #ifdef YY_NO_CONST */
/* extern FILE *oprs_yyin, *oprs_yyout; */
/* #endif */

GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}


double coordx[100];
double coordy[100];

int count = 0;

static gboolean
on_expose_event(GtkWidget *widget,
    GdkEventExpose *event,
    gpointer data)
{
  CairoGCs CGCs;

  create_cgcs(&CGCs, global_draw_data->window);
  

  cairo_t *cr = CGCs.cr_basic;

  cairo_move_to(cr, 800,  30);
  cairo_show_text(CGCs.cr_basic, "CGCs.cr_basic");
  cairo_move_to(CGCs.cr_title, 800, 60);
  cairo_show_text(CGCs.cr_title, "CGCs.cr_title");

  cairo_move_to(CGCs.cr_edge, 800, 120);
  cairo_show_text(CGCs.cr_edge, "CGCs.cr_edge");

  cairo_move_to(CGCs.cr_node, 800, 150);
  cairo_show_text(CGCs.cr_node, "CGCs.cr_node");
  cairo_move_to(CGCs.cr_text, 800, 180);
  cairo_show_text(CGCs.cr_text, "CGCs.cr_text");

  int i, j;
  for ( i = 0; i <= count - 1; i++ ) {
      for ( j  = 0; j <= count -1; j++ ) {
          cairo_move_to(cr, coordx[i], coordy[i]);
          cairo_line_to(cr, coordx[j], coordy[j]);
      }
  }

  // count = 0;
  cairo_stroke(cr);
  handle_exposures(widget, global_draw_data, &CGCs);

  destroy_cgcs(&CGCs);

  return FALSE;
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
    if (event->button == 1) {
        coordx[count] = event->x;
        coordy[count++] = event->y;
    }

    if (event->button == 3) {
        gtk_widget_queue_draw(widget);
    }

    return TRUE;
}

GtkWidget *opList;
GtkWidget *topLevel;

int main(int argc, char **argv, char **envp)
{
  GtkWidget *topLevelWindow;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *sw;
  GtkWidget *label;

  GtkWidget *menubar;
  GtkWidget *toolbar;
  GtkWidget *opeDrawWin;
  GtkWidget *filemenu;
  GtkWidget *file;
  GtkWidget *quit;

  GtkTreeSelection *selection; 

  char title[LINSIZ];

  OG *init_og;
  Pixmap icon_pixmap;
  char *language_str;
  
  Cardinal n;
  Arg args[MAXARGS];
  
  Slist *list_of_commands;
  char *command, *error;
  char error_message[BUFSIZ];
  
  PBoolean loadedfiles = 0, res, errors = 0, ignore;
  char welcome_message[3000];
  char iconname[] = "OP Editor";
  int pid=getpid();
  
     gx_print_command_template = DEFAULT_PRINT_COMMAND;
     txt_print_command_template = DEFAULT_PRINT_COMMAND;
     ip_x = IP_X;
     ip_y = IP_Y;
     ip_width = IP_WIDTH;
     ctxt_x = CTXT_X;
     ctxt_y = CTXT_Y;
     ctxt_width = CTXT_WIDTH;
     call_x = CALL_X;
     call_y = CALL_Y;
     call_width = CALL_WIDTH;
     set_x = SET_X;
     set_y = SET_Y;
     set_width = SET_WIDTH;
     eff_x = EFF_X;
     eff_y = EFF_Y;
     eff_width = EFF_WIDTH;
     prop_x = PROP_X;
     prop_y = PROP_Y;
     prop_width = PROP_WIDTH;
     doc_x = DOC_X;
     doc_y = DOC_Y;
     doc_width = DOC_WIDTH;
     act_x = ACT_X;
     act_y = ACT_Y;
     act_width = ACT_WIDTH;
     bd_x = BD_X;
     bd_y = BD_Y;
     bd_width = BD_WIDTH;
     edge_width = EDGE_WIDTH;

  disable_slist_compaction();
  
  list_of_commands = ope_init_nw_arg(argc, argv);
  init_hash_size_id(0);
  make_id_hash();		/* Make the symbol hash table */
  init_id_hash();
  
  init_hash_size_pred_func(0);
  make_pred_func_hash();		/* Make the predicat hash table */
  
  make_global_var_list();
  
  relevant_op = (Relevant_Op *)make_relevant_op();
  list_opfiles = sl_make_slist();
  list_last_selected_ops = sl_make_slist();
  
  ope_parser = TRUE;
  parse_source = PS_STRING;
  
  if (no_window) {
    strcpy (error_message,  "The following files have not been loaded:\n");
    
    command = (char *) sl_get_from_head(list_of_commands);
    while (command != NULL) {
      res = yyparse_one_command_string(command); 
      error = (char *) sl_get_from_head(list_of_commands);
      
      if (!res){
	strcat(error_message, error);
	errors++;
      } else
	loadedfiles++;
      
      FREE (command);
      FREE (error);
      command = (char *) sl_get_from_head(list_of_commands);
    }
    FREE_SLIST(list_of_commands);
    exit(0);
  }
  
  sprintf(title, LG_STR("OP Editor (GTK) %s",
			"OP Editeur (GTK) %s"), package_version);


  select_language(language_str, TRUE);
  use_dialog_error = TRUE;
  really_build_node = FALSE;
  file_name_for_copy = (char *) MALLOC(sizeof(NAME_FOR_COPY) + 10); /* An integer is not biger than 10 char... */
  sprintf(file_name_for_copy, "%s%d",NAME_FOR_COPY, pid);
  
  file_name_for_print = (char *) MALLOC(sizeof(NAME_FOR_PRINT) + 10);
  sprintf(file_name_for_print, "%s%d",NAME_FOR_PRINT, pid);
  global_draw_data = &dd;

  gtk_init(&argc, &argv);

  topLevelWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  dd.topLevelWindow = topLevelWindow;
  gtk_window_set_title(GTK_WINDOW(topLevelWindow), title);
  gtk_window_set_default_size(GTK_WINDOW(topLevelWindow), 1400, 800);
  gtk_window_set_position(GTK_WINDOW(topLevelWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_icon(GTK_WINDOW(topLevelWindow), create_pixbuf("ope-icon.png"));
  g_signal_connect(G_OBJECT(topLevelWindow), "destroy",
      G_CALLBACK(quitQuestionManage), topLevelWindow);


  vbox = gtk_vbox_new(FALSE, 0); /* The main vbox */
  gtk_container_add(GTK_CONTAINER(topLevelWindow), vbox); /* in the topLevelWindow */
  
  menubar = create_menu_bar(topLevelWindow, &dd);
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 1); /* add a menubar to the main vbox */

  toolbar = create_tool_bar(topLevelWindow, &dd);
  gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 1); /* add a toolbar just bellow the menubar */

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(vbox), hbox); /* create a hbox below the toolbar */

  opeDrawWin = gtk_scrolled_window_new(NULL, NULL); /* create a large scrolled window in this hbox */
  gtk_box_pack_start(GTK_BOX(hbox), opeDrawWin, TRUE, TRUE, 1);

  vbox = gtk_vbox_new(FALSE, 0); /* pack anothe vbox on the right */
  gtk_box_pack_end(GTK_BOX(hbox), vbox, FALSE, FALSE, 1);

  label = gtk_label_new("OP Name");	/* put a title on this vbox */
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);

  sw = gtk_scrolled_window_new(NULL, NULL); /* crea */
  gtk_box_pack_end(GTK_BOX(vbox), sw, TRUE, TRUE, 1);

  opList = gtk_tree_view_new();
  gtk_widget_set_size_request(opList, 200, -1);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(opList), FALSE);

  gtk_container_add(GTK_CONTAINER(sw), opList);


  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(opList));

  g_signal_connect(selection, "changed", 
      G_CALLBACK(on_changed_oplist), label);


  dd.canvas = gtk_layout_new(NULL,NULL);
  gtk_widget_set_app_paintable(dd.canvas, TRUE);
  gtk_container_add(GTK_CONTAINER(opeDrawWin),dd.canvas);

  gtk_layout_get_size(GTK_LAYOUT(dd.canvas),&dd.canvas_width, &dd.canvas_height);
  dd.work_height = MAX(WORK_HEIGHT, dd.canvas_height);
  dd.work_width = MAX(WORK_WIDTH, dd.canvas_width);
  gtk_layout_set_size(GTK_LAYOUT(dd.canvas), dd.work_width, dd.work_height); 

  dd.top = 0;
  dd.left = 0;

  ope_create_filesel(topLevelWindow, &dd);

  gtk_widget_show_all(topLevelWindow);

  dd.window = GTK_LAYOUT(dd.canvas)->bin_window;

  mainCGCsp = &mainCGCs;
  dd.cgcsp = mainCGCsp;
  
  create_cgcs(&mainCGCs, dd.window); /* this will create all the cairo context for the main loop... */
   
  gtk_widget_add_events (dd.canvas, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(dd.canvas, "expose-event",
		   G_CALLBACK(on_expose_event), NULL);
  g_signal_connect(dd.canvas, "button-press-event", 
		   G_CALLBACK(clicked), NULL);


  gtk_main();

  return 0;
#ifdef IGNORE
/*      XFontStruct  	*font; */

#ifdef XEDITRESCHECKMESSAGES_AVAILABLE
     extern void _XEditResCheckMessages();
#endif

/*      fprintf(stderr, "Locale set from env to %s.\n",setlocale(LC_CTYPE,"")); */

/*      topLevel = XtVaAppInitialize( */
/* 				  &app_context,	/\* Application context *\/ */
/* 				  "Ope",	/\* application class name *\/ */
/* 				   options, XtNumber(options),  /\* command line option list *\/ */
/* 				  & argc, argv,	/\* command line args *\/ */
/* 				  fallback_resources,	/\* for missing app-defaults file *\/ */
/* 				  NULL);	/\* terminate varargs list *\/ */

/* #ifdef XEDITRESCHECKMESSAGES_AVAILABLE */
/*      XtAddEventHandler(topLevel, (EventMask)0, True, _XEditResCheckMessages, NULL); */
/* #endif */

/*      XtGetApplicationResources(topLevel, &Resrcs, resources, XtNumber(resources), NULL, 0); */

/* #ifdef YY_NO_CONST */
/*      oprs_yyin = stdin; */
/*      oprs_yyout = stdout; */
/* #endif */
     


     list_of_commands = ope_init_arg(argc, argv);


     /* icon_pixmap = XCreateBitmapFromData(XtDisplay(topLevel), */
     /* 					 RootWindowOfScreen(XtScreen(topLevel)), */
     /* 					 ope_icon_bits, */
     /* 					 ope_icon_width, ope_icon_height); */

     /* XtVaSetValues(topLevel, */
     /* 		   XmNiconPixmap, icon_pixmap, */
     /* 		   XmNtitle, title, */
     /* 		   XmNiconName, iconname, */
     /* 		   XmNborderWidth, 1, */
     /* 		   NULL); */

     create_help_info(topLevel);

     /* topForm = XmCreateForm(topLevel, "topForm", NULL, 0); */
     /* XtManageChild(topForm); */

     /* opeDrawWinFrame = XmCreateFrame(topForm, "opeDrawWinFrame", NULL, 0); */
     /* XtManageChild(opeDrawWinFrame); */

     /* opeDrawWin = XmCreateScrolledWindow(opeDrawWinFrame, "opeDrawWin", NULL, 0); */
     /* XtManageChild(opeDrawWin); */

     /* menuBar = create_menu_bar(topForm, &dd); */

     /* opeMenuFrame = create_menu_pane(topForm, &dd); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++; */
     /* XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++; */
     /* XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++; */
     /* XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++; */
     /* XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET)); n++; */
     /* messageWindow = XtCreateManagedWidget("messageWindow", xmLabelWidgetClass, topForm, args, n); */


     /* n = 0; */
     /* XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++; */
     /* XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++; */
     /* XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNtopWidget, menuBar); n++; */
     /* XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++; */
     /* XtSetArg(args[n], XmNlabelString, XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET)); n++; */
     /* titleWindow = XtCreateManagedWidget("titleWindow", xmLabelWidgetClass, topForm, args, n); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNtopWidget, titleWindow); n++; */
     /* XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNbottomWidget, messageWindow); n++; */
     /* XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNleftWidget, opeMenuFrame); n++; */
     /* XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++; */
     /* XtSetValues(opeDrawWinFrame, args, n); */

     /* n = 0; */
     /* XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNtopWidget, titleWindow); n++; */
     /* XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++; */
     /* XtSetArg(args[n], XmNbottomWidget, messageWindow); n++; */
     /* XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++; */
     /* XtSetValues(opeMenuFrame, args, n); */

     make_create_bboard(topLevel);

     make_edge_bboard(topLevel);

     edit_edge_bboard(topLevel);
     make_edit_node_dialog(topLevel, &dd);

     ope_create_filesel(topLevel, &dd);
     ope_create_rop_dialog(topLevel, &dd);
     ope_create_dialogs(topLevel, &dd);
     ope_create_print_command_dialog(topLevel, &dd);
     ope_create_vf_dialog(topLevel, &dd);

     ope_information_create(topLevel);
     create_syntax_error_dialog(topLevel);
/*      ope_create_working_dialog(topLevel);*/
     ope_create_user_error(topLevel);
     ope_create_name_error(topLevel);

     /* dd.canvas = XmCreateDrawingArea(opeDrawWin, "opeCanvas", NULL, 0); */
     /* XtManageChild(dd.canvas); */

     /* XtSetArg(args[0], XmNheight, &dd.canvas_height); */
     /* XtSetArg(args[1], XmNwidth, &dd.canvas_width); */
     /* XtGetValues(dd.canvas, args, 2); */
     
     init_draw_mode_help();

     reset_dd(&dd);
     dd.copy_area_index_queue = make_queue();
     dd.node_selected = NULL;
     dd.sensitive_og = NULL;
     dd.edge_selected = NULL;
     dd.og_selected_on_press = NULL;
     dd.og_align = NULL;
     dd.og_aligning = NULL;
     /* dd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT); */
/*     dd.fontlist = XmFontListCreate(dd.font,"opeCanvas_Charset"); */
     dd.fontlist = Resrcs.fontList;
     dd.expose_region = NULL;
     dd.just_compiling = FALSE;

     /* XtAddCallback(dd.canvas, XmNexposeCallback, (XtCallbackProc)handle_exposures, &dd); */
     /* XtAddCallback(dd.canvas, XmNresizeCallback, (XtCallbackProc)resize, &dd); */
     /* XtAddEventHandler(dd.canvas, 0, TRUE, (XtEventHandler)handle_g_exposures, &dd); */
     /* XtAddEventHandler(dd.canvas, ButtonPressMask, FALSE, (XtEventHandler)canvas_mouse_press, &dd); */
     /* XtAddEventHandler(dd.canvas, ButtonMotionMask, FALSE, (XtEventHandler)canvas_mouse_motion, &dd); */
     /* XtAddEventHandler(dd.canvas, ButtonReleaseMask, FALSE, (XtEventHandler)canvas_mouse_release, &dd); */


     //     create_scrollbars(opeDrawWin, &dd);

     /* n = 0; */
     /* XtSetArg(args[n], XmNverticalScrollBar, dd.vscrollbar); n++; */
     /* XtSetArg(args[n], XmNhorizontalScrollBar, dd.hscrollbar); n++; */
     /* XtSetArg(args[n], XmNworkWindow, dd.canvas); n++; */
     /* XtSetValues(opeDrawWin, args, n); */

     /* quitQuestion = XmCreateQuestionDialog(topLevel, "quitQuestion", NULL, 0); */
     /* XtAddCallback(quitQuestion, XmNokCallback, ReallyQuit, NULL); */
     /* XtAddCallback(quitQuestion, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Quit")); */

     /* printQuestion = XmCreateQuestionDialog(topLevel, "printQuestion", NULL, 0); */
     /* XtAddCallback(printQuestion, XmNokCallback, (XtCallbackProc)ReallyPrint, &dd); */
     /* XtAddCallback(printQuestion, XmNhelpCallback, infoHelp, makeFileNode("oprs", "Print OP File")); */

     /* XtRealizeWidget(topLevel); */

     /* dd.window = XtWindow(dd.canvas); */

     /* set_draw_mode(&dd, MOVE_OG); */

     /*
      * You cannot do that, this idiot does not have a fontlist... n = 0;
      * XtSetArg(args[n], XmNfontList, &dd.fontlist);n++;
      * XtGetValues(dd.canvas, args, n);
      */

     create_gc(&dd);

     /* all is created, we can initialize the sensitivities */
     update_select_sensitivity(False);
     update_buffer_sensitivity(False);
     update_toggle_sensitivity(False);
     update_empty_sensitivity(False);
     update_file_sensitivity(False);

     sprintf(title, LG_STR("Welcome to the OP Editor %s %s",
			   "Bienvenue dans le OP Editor %s %s"), package_version, COPYRIGHT_STRING);
     UpdateMessageWindow(title);

     sprintf(welcome_message, "\n\
This is OP Editor Version (%s).\n\
\n\
%s\n\
\n\
Version     : %s\n\
Compiled on : %s\n\
Date        : %s\n\
\n\
 All rights reserved.\n\
\n\
 Redistribution and use in source and binary forms, with or without\n\
 modification, are permitted provided that the following conditions\n\
 are met:\n\
\n\
    - Redistributions of source code must retain the above copyright\n\
      notice, this list of conditions and the following disclaimer.\n\
    - Redistributions in binary form must reproduce the above\n\
      copyright notice, this list of conditions and the following\n\
      disclaimer in the documentation and/or other materials provided\n\
      with the distribution.\n\
\n\
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n\
 \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n\
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n\
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n\
 COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n\
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n\
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n\
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n\
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n\
 ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
 POSSIBILITY OF SUCH DAMAGE.\n\
",
	     package_version, COPYRIGHT_STRING, package_version, host, date);

     dd.op = make_op();		/* Just for the drawing of this string */

     make_opfile(NULL, ACS_GRAPH);
     add_op_file_name(current_opfile->name, relevant_op);


     //init_og = create_text(dd.canvas, 80, 20 &dd, TT_TEXT_NONE, welcome_message, 0, FALSE);

     /* Execute all the commands given in argument. */

     strcpy (error_message,  "The following files have not been loaded:\n");

     command = (char *) sl_get_from_head(list_of_commands);
     while (command != NULL) {
	  res = yyparse_one_command_string(command); 
	  error = (char *) sl_get_from_head(list_of_commands);

	  if (!res){
	       strcat(error_message, error);
	       errors++;
	  } else
	       loadedfiles++;
	
	  FREE (command);
	  FREE (error);
	  command = (char *) sl_get_from_head(list_of_commands);
     }
     FREE_SLIST(list_of_commands);
     current_op = NULL;

     if (errors)
	  report_syntax_error(error_message);

     if ( loadedfiles ) {
	  if (!sl_slist_empty(current_opfile->list_op)) {
	       update_empty_sensitivity(True);
	       selectOpDialogManage();
	  }
     }

     buffer_opfile = make_buffer_opfile();

     /* XtAppMainLoop(app_context); */

     return (1);
#endif
}

void init_draw_mode_help()
{
     mode_help[DRAW_NODE] = "L: Click to create a node. M: Move. R: Edit.";
     mode_help[DRAW_IFNODE] = "L: Click to create an if-then-else node. M: Move. R: Edit.";
     mode_help[DRAW_EDGE] = "L: Click on the first node. M: Move. R: Edit.";
     mode_help[DRAW_KNOT] = "L: Click on the edge to which you want to add knots. M: Move. R: Edit.";
     mode_help[EDGE_SELECTED] = "L: Click where you want to add a knot or right click to abort. M: Move. R: Abort.";
     mode_help[NODE_SELECTED] = "L: Click anywhere to create a knot or on a node to terminate or right click to abort. M: Move. R: Abort.";
     mode_help[EDIT_OG] = "L: Click on an object to edit. M: Move. R: Edit.";
     mode_help[DESTROY_OG] = "L: Click on an object to destroy. M: Move. R: Edit.";
     mode_help[MOVE_OG] = "L: Click on an object to move it or on the background to move the whole OP. M: Move. R: Edit.";
     mode_help[MOVING_OG] = "L: Move the object to a new position and release the mouse. M: Move. R: Edit.";
     mode_help[MOVING_CANVAS] =  "L: Move the OP to a new position and release the mouse. M: Move. R: Edit.";
     mode_help[CONVERT_END] = "L: Click on a node (without outgoing edge) to convert to end node. M: Move. R: Edit.";
     mode_help[CONVERT_START] = "L: Click on a node (without ingoing edge) to convert start node. M: Move. R: Edit.";
     mode_help[ALIGN_OG] = "L: Click on an object to align with. M: Move. R: Edit.";
     mode_help[ALIGN_OG_V] =  "L: Click on an object to  verticaly align with. M: Move. R: Edit.";
     mode_help[ALIGN_OG_H] = "L: Click on an object to horizontaly align with. M: Move. R: Edit.";
     mode_help[ALIGNING_OG] = "L: Click on the object you want to align with the selected object, right click to abort. M: Move. R: Abort.";
     mode_help[ALIGNING_OG_V] =  "L: Click on the object you want to align verticaly with the selected object, right click to abort. M: Move. R: Abort.";
     mode_help[ALIGNING_OG_H] =  "L: Click on the object you want to align horizontaly with the selected object, right click to abort. M: Move. R: Abort.";
     mode_help[DUPLICATE_OBJECTS] = "L: Select a node or an edge to duplicate. M: Move. R: Edit.";
     mode_help[DUPLICATING_NODE] = "L: Select a position to create the duplicate, right click to abort. M: Move. R: Abort.";
     mode_help[DUPLICATING_EDGE] = "L: Select the first node, right click to abort. M: Move. R: Abort.";
     mode_help[DUPLICATING_EDGE2] = "L: Click anywhere to create a knot or on a node to terminate, right click to abort. M: Move. R: Abort.";
     mode_help[MERGE_NODE] = "L: Click on the first node. M: Move. R: Edit.";
     mode_help[MERGING_NODE] = "L: Click on the second node, right click to abort. M: Move. R: Abort.";
     mode_help[FLIP_SPLIT] = "L: Click on a node to flip its split status. M: Move. R: Edit.";
     mode_help[FLIP_JOIN] = "L: Click on a node to flip its join status. M: Move. R: Edit.";
     mode_help[OPEN_NODE] = "L: Click on a node you want to split/open. M: Move. R: Edit.";
     mode_help[DM_RELEVANT_OP] = "L: Click on an edge to find out relevant OPs. M: Move. R: Edit.";
     
}

void ClearMessageWindow()
{
     Arg args[1];
     XmString xmres;

     XtSetArg(args[0], XmNlabelString, xmres = XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET));
     XtSetValues(messageWindow, args, 1);
     XmStringFree(xmres);
}

void UpdateMessageWindow(char *string)
{
#ifdef IGNORE
     Arg args[1];
     XmString xmres;
     XmString x1,x2;

     xmres = XmStringCreate(" ", XmSTRING_DEFAULT_CHARSET);
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET));
     XmStringFree(x1);
     XmStringFree(x2);

     XtSetArg(args[0], XmNlabelString, xmres);
     XtSetValues(messageWindow, args, 1);
     XmStringFree(xmres);
#endif
}

void ClearTitleWindow()
{
#ifdef IGNORE
     XmString to_free;
     Arg args[1];

     XtSetArg(args[0], XmNlabelString, to_free = XmStringCreate("", XmSTRING_DEFAULT_CHARSET));
     XtSetValues(titleWindow, args, 1);
     XmStringFree(to_free);
#endif
}

void UpdateTitleWindow()
{
#ifdef IGNORE
     char title[LINSIZ];
     Arg args[1];
     XmString xmres;
     XmString x1,x2;

     xmres = XmStringCreate(LG_STR("Selected File: ",
				   "Fichier Sélectionné: "), "fixed_cs");
     sprintf(title, "%s", (current_opfile ? current_opfile->name :
			   LG_STR("None",
				  "None")));
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreate(title, "variable_cs"));
     XmStringFree(x1);
     XmStringFree(x2);
     sprintf(title, " %c", ((current_opfile && current_opfile->modified) ? '*' : ' '));
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreate(title, "variable_cs"));
     XmStringFree(x1);
     XmStringFree(x2);
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreate(LG_STR("    Selected OP: ",
								   "    OP Sélectionné: "), "fixed_cs"));
     XmStringFree(x1);
     XmStringFree(x2);
     sprintf(title, "%s", (current_op ? current_op->name : LG_STR("None",
			   "None")));
     xmres = XmStringConcat(x1 = xmres, x2 = XmStringCreate(title, "variable_cs"));
     XmStringFree(x1);
     XmStringFree(x2);

     XtSetArg(args[0], XmNlabelString, xmres);
     XtSetValues(titleWindow, args, 1);
     XmStringFree(xmres);
#endif
}

#ifdef DLSYMOPENCLOSE_UNDEFINED
dlsym()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

dlopen()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

dlclose()
{
     fprintf(stderr,LG_STR("Fatal error: Sorry, but you should not be here...",
			   "Fatal error: Sorry, but you should not be here..."));
     exit(1);	  
}

#endif
 
void on_window_destroy (GtkObject *object, gpointer user_data)
{
     gtk_main_quit ();
}

void on_quit_item_activate (GtkObject *object, gpointer user_data)
{
     gtk_main_quit ();
}
