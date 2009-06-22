static const char* const rcsid = "$Id$";
/*                               -*- Mode: C -*- 
 * opc-main.c -- 
 * 
 * Copyright (c) 1991-2009 Francois Felix Ingrand.
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

#if defined(WIN95)
#include "winsock.h"
#else
#include "netdb.h"
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

/* 
 * Standard Toolkit include files
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>


#include "slistPack.h"
#include "slistPack_f.h"

#include "constant.h"
#include "macro.h"

#include "oprs-type.h"
#include "parser-funct.h"
#include "op-structure.h"
#include "op-default.h"
#include "ope-graphic.h"
#include "oprs.h"
#include "oprs-sprint.h"

#include "opc.h"

#include "oprs-type_f.h"
#include "oprs-pred-func_f.h"
#include "relevant-op_f.h"
#include "help_f.h"
#include "oprs-util_f.h"
#include "parser-funct_f.h"
#include "oprs-sprint_f.h"

Oprs *current_oprs;

#define OPC_ARG_ERR_MESSAGE LG_STR(\
"Usage: opc [-v] [-o output-dopf] [-i command-file] [-l input-opf] [-d imputdopf]  [-p oprs-data-file]\n\
\top-file*\n",\
"Utilisation: opc [-v] [-o output-dopf] [-i command-file] [-l input-opf] [-d imputdopf]  [-p oprs-data-file]\n\
\top-file*\n")

XtAppContext app_context;

struct _resrcs {
     XmRenderTable renderTable;
     int work_width, work_height;	  /* default bitmap size */
     Boolean  printEnglishOperator;
     char *language;
     int  ip_x;
     int  ip_y;
     int  ip_width;
     int  call_x;
     int  call_y;
     int  call_width;
     int  ctxt_x;
     int  ctxt_y;
     int  ctxt_width;
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
     {"callX", "CallX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_x), XmRImmediate, (char *)CALL_X},
     {"callY", "CallY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_y), XmRImmediate, (char *)CALL_Y},
     {"callWidth", "CallWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, call_width), XmRImmediate, (char *)CALL_WIDTH},
     {"ctxtX", "CtxtX", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_x), XmRImmediate, (char *)CTXT_X},
     {"ctxtY", "CtxtY", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_y), XmRImmediate, (char *)CTXT_Y},
     {"ctxtWidth", "CtxtWidth", XmRInt, sizeof(int), 
      XtOffsetOf(struct _resrcs, ctxt_width), XmRImmediate, (char *)CTXT_WIDTH},
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
     {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     XtOffsetOf(struct _resrcs, renderTable), XmRImmediate, NULL},
     {"printEnglishOperator", "PrintEnglishOperator", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printEnglishOperator), XmRImmediate, (XtPointer)FALSE},
};

String fallback_resources[] = {
#include "XOprs.ad.h"
NULL
};

char *progname;

Relevant_Op *relevant_op;
Widget topLevel;
Draw_Data dd;

Slist *opc_init_arg(int argc,char **argv)
{
     char *command, *filename = NULL;
/*      char *id_case_option = NULL, *lang_str = NULL; */

     Slist *list_of_commands = sl_make_slist();
     Sprinter *sp = make_sprinter(0);
     PBoolean tflag = FALSE, Xflag = FALSE, hflag = FALSE;

     int c;

     extern int optind;
     extern char *optarg;

     while ((c = getopt(argc, argv, "vtXed:l:o:i:p:h")) != EOF)
	  switch (c)
	  {
	  case 'o':
	       filename = optarg;
	       reset_sprinter(sp);
	       SPRINT(sp, 16 + strlen(filename), sprintf(f,"dump op \"%s\"\n", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'l':
	       filename = optarg;
	       reset_sprinter(sp);
	       SPRINT(sp, 16 + strlen(filename), sprintf(f,"load opf \"%s\"\n", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'd':
	       filename = optarg;
	       reset_sprinter(sp);
	       SPRINT(sp, 16 + strlen(filename), sprintf(f,"load dump op \"%s\"\n", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'i':
	       filename = optarg;
	       reset_sprinter(sp);
	       SPRINT(sp, 16 + strlen(filename), sprintf(f,"include \"%s\"\n", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'p':
	       filename = optarg;
	       reset_sprinter(sp);
	       SPRINT(sp, 24 + strlen(filename), sprintf(f,"set oprs_data_path \"%s\"\n", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),command);
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 'e':
	       reset_sprinter(sp);
	       SPRINT(sp, 12 + strlen(filename), sprintf(f,"empty op db\n")); 
	       NEWSTR(SPRINTER_STRING(sp),command); 
	       sl_add_to_tail(list_of_commands, command);
	       break;
	  case 't':
	       tflag = TRUE;
	       break;
	  case 'X':
	       Xflag = TRUE;
	       break;
	  case 'v':
	       be_quiet = FALSE;
	       break;
	  case 'h':
	  default:
	       hflag = TRUE;
 	  }

     if (hflag) {
	  fprintf(stderr, OPC_ARG_ERR_MESSAGE);
	  exit(1);
     }

     if (tflag && Xflag) {
	  fprintf(stderr, LG_STR("Cannot request both graphique [-X] and non graphique [-t] compilation.\n",
				 "Ne peut honorer une compilation graphique [-X] et non graphique [-t].\n"));
	  exit(1);
     }
     compile_graphix = ! tflag;

     for (; optind < argc; optind++) { /* The others files */
	  char *pos;
	  filename = argv[optind];
	  reset_sprinter(sp);
	  SPRINT(sp, 12 + strlen(filename), sprintf(f,"empty op db\n")); 
	  NEWSTR(SPRINTER_STRING(sp),command);
	  sl_add_to_tail(list_of_commands, command);
	  reset_sprinter(sp);
	  SPRINT(sp, 16 + strlen(filename), sprintf(f,"load opf \"%s\"\n", filename)); 
	  NEWSTR(SPRINTER_STRING(sp),command);
	  sl_add_to_tail(list_of_commands, command);
	  reset_sprinter(sp);
	  
	  if ((pos = strrchr(filename,'.'))  && (strcasecmp(".opf",pos) == 0))
	       pos[0] = '\0';
	  
	  SPRINT(sp, 24 + strlen(filename), sprintf(f,"dump op \"%s.dopf\"\n", filename)); 
	  NEWSTR(SPRINTER_STRING(sp),command);
	  sl_add_to_tail(list_of_commands, command);  
     }

     free_sprinter(sp);

     return(list_of_commands);
}

int oprs_yyparse_caller(void)
{
     closed_file = FALSE;
     yy_begin_COMMAND();
     return oprs_yyparse();
}

void opc_parse_one_or_more(void)
{
     oprs_yyparse_caller();

     while (parser_index > 0) { /* We are parsing a file... */
	  oprs_yyparse_caller();
     }
}

void opc_yyparse_string(PString s)
{
     Parse_Source_Type string_saved_parse_source;
     int string_saved_line_number;
     PBoolean res;

     string_saved_parse_source = parse_source;
     string_saved_line_number = string_line_number;
     parse_source = PS_STRING;  
     string_line_number = 1;
     oprs_yy_scan_string(s);	   
     opc_parse_one_or_more();

     parse_source = string_saved_parse_source;
     string_line_number = string_saved_line_number;
}

int main(int argc, char **argv, char **envp)
{
     PBoolean ignore;
     Slist *list_of_commands;
     char *command;
     char *language_str;

     disable_slist_compaction();

     progname = argv[0];
     
     be_quiet = TRUE;

     oprs_data_path = getenv("OPRS_DATA_PATH");
     
     list_of_commands = opc_init_arg( argc, argv);
     
     if (compile_graphix) {
	  
	  global_draw_data = &dd;

	  topLevel =  XtVaAppInitialize(
	       &app_context, /* Application context */
	       "XOprs", /* application class name */
	       NULL, 0,  /* command line option list */
	       &argc, argv, /* command line args */
	       fallback_resources,	/* for missing app-defaults file */
	       NULL); /* terminate varargs list */
	  
	  XtGetApplicationResources(topLevel, &Resrcs, resources, XtNumber(resources), NULL, 0);

	  print_english_operator = Resrcs.printEnglishOperator;
	  language_str = Resrcs.language;
	  ip_x = Resrcs.ip_x;
	  ip_y = Resrcs.ip_y;
	  ip_width = Resrcs.ip_width;
	  call_x = Resrcs.call_x;
	  call_y = Resrcs.call_y;
	  call_width = Resrcs.call_width;
	  ctxt_x = Resrcs.ctxt_x;
	  ctxt_y = Resrcs.ctxt_y;
	  ctxt_width = Resrcs.ctxt_width;
	  set_x = Resrcs.set_x;
	  set_y = Resrcs.set_y;
	  set_width = Resrcs.set_width;
	  eff_x = Resrcs.eff_x;
	  eff_y = Resrcs.eff_y;
	  eff_width = Resrcs.eff_width;
	  prop_x = Resrcs.prop_x;
	  prop_y = Resrcs.prop_y;
	  prop_width = Resrcs.prop_width;
	  doc_x = Resrcs.doc_x;
	  doc_y = Resrcs.doc_y;
	  doc_width = Resrcs.doc_width;
	  act_x = Resrcs.act_x;
	  act_y = Resrcs.act_y;
	  act_width = Resrcs.act_width;
	  bd_x = Resrcs.bd_x;
	  bd_y = Resrcs.bd_y;
	  bd_width = Resrcs.bd_width;
	  edge_width = Resrcs.edge_width;
	  dd.rendertable = Resrcs.renderTable;
	  dd.just_compiling = TRUE;


	  select_language(language_str, TRUE);
     }
     
     yy_begin_COMMAND();		/* On demarre le lex parser dans cet etat */

     SRANDOM(getpid());		/* we should use the date or something else... */

     init_hash_size_id(0);
     init_hash_size_pred_func(0);

     make_id_hash();		/* Make the symbol hash table */
     init_id_hash();

     make_pred_func_hash();
     make_global_var_list();

     relevant_op = (Relevant_Op *)make_relevant_op();

     parse_source = PS_STRING;

     i_have_the_stdin = TRUE;

     setbuf(stdin, NULL);	/* In case we are connected to a file, we want to be unbuffered Otherwise we have
				 * trouble when we fork because the buffer of the child stdin is full of the father
				 * command... */

     command = (char *) sl_get_from_head(list_of_commands);
     while (command != NULL) {
	  opc_yyparse_string(command); 
	  FREE (command);
	  command = (char *) sl_get_from_head(list_of_commands);
     }
     FREE_SLIST(list_of_commands);

     if (compile_graphix) XtCloseDisplay(XtDisplay(topLevel));

     socket_cleanup_and_exit(0);

}


void process_xt_events() {}
