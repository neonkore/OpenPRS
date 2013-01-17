/*                               -*- Mode: C -*- 
 * yy-funct.c -- 
 *
 */

/*
 * Copyright (c) 1991-2013 Francois Felix Ingrand.
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

#ifdef VXWORKS
#include "vxWorks.h"
#include "ioLib.h"
#include "stdioLib.h"
#include "nfsLib.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <fcntl.h>
#ifdef WIN95
#include <io.h>
#define R_OK 04
#define MAXPATHLEN 1024
#else
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "opaque.h"
#include "constant.h"
#include "macro.h"
#include "oprs-type.h"
#include "oprs.h"
#include "parser-funct.h"

#include "yy-funct_f.h"
#include "parser-funct_f.h"
#include "oprs-type_f.h"
#include "op-instance_f.h"

void close_file_for_yyparse(void);

int parser_index = 0;

static YY_BUFFER_STATE parser_state_save[MAX_OPEN_INCLUDE];
static FILE *FILE_save[MAX_OPEN_INCLUDE];
Parse_Source_Type saved_parse_source[MAX_OPEN_INCLUDE];
char parser_indent[MAX_OPEN_INCLUDE] = ""; /* at most MAX_OPEN_INCLUDES white spaces */
int parser_line_number[MAX_OPEN_INCLUDE] = {1};
char *parser_file_name[MAX_OPEN_INCLUDE] = {"stdin"};
int string_line_number = 1;
PBoolean stop_parsing_from_socket;

void oprs_yyerror(char *s)
{
/*      if (!closed_file) */
	  warning(s);

/*      closed_file = FALSE; */
}


int oprs_yywrap()
{
     switch (parse_source) {
     case PS_FILE:		/* File or stdin... */
	  if (parser_index > 0)
	       close_file_for_yyparse();
	  if (!ope_parser && (parser_index == 0)) return 1; /* stdin, stop parsing*/
	  else return 0;	/* Keep parsing */
	  break;
     case PS_STRING:		/* end of string... */
	  return 1;
	  break;
     case PS_SOCKET:		/* The socket is now empty...  */
	  stop_parsing_from_socket = TRUE;
	  return 1;		/* Done with parsing... */
	  break;
     default:
	  fprintf(stderr,"oprs_ywrap: unknown parse source.\n");
	  return -1;

     }
}

#ifdef IGNORE
     if (parser_index > 0 && (parse_source == PS_FILE)) {
	  closed_file = TRUE;
	  close_file_for_yyparse();
	  /* clearerr(stdin); */		/* it was in an eof state... */
	  if (!ope_parser && (parser_index == 0)) return 1;
	  else return 0;	/* Keep parsing */
     } else
	  return 1;		/* Done with parsing */
#endif


int open_file_for_yyparse(char * path)
{
     struct stat buf;
     FILE *file_stream;
     int res;

/*      if (parser_index == 0) saved_parse_source = parse_source; */

     if (parser_index == MAX_OPEN_INCLUDE) {
	  fprintf(stderr,LG_STR("Too many includes files opened.\n",
				"Trop de fichiers include ouverts.\n"));
	  return -1;
     }

     PROTECT_SYSCALL_FROM_EINTR(res, stat(path,&buf));
     if (res == -1) {
	  perror("open_file_for_yyparse: stat");
	  fprintf(stderr,LG_STR("\tfor file \"%s\".\n",
				"\tpour le fichier \"%s\".\n"), path);
	  return -1;
     }

     if (! S_ISREG(buf.st_mode)) {
	  if (use_dialog_error) {
	       char message[LINSIZ];
	       sprintf (message,LG_STR("File \"%s\" is not a regular file.",
				       "Le fichier \"%s\" n'est pas un fichier normal."), path);
	       report_parser_error(message);
	  } else {
	       fprintf(stderr,LG_STR("File \"%s\" is not a regular file.\n",
				     "Le fichier \"%s\" n'est pas un fichier normal.\n"), path);
	  }
	  return -1;
     }

     PROTECT_NULL_SYSCALL_FROM_EINTR(file_stream, fopen(path,"r"));
     
     if (!file_stream) {
	  if (use_dialog_error) {
	       extern int errno;
	       char message[LINSIZ];
	       
	       sprintf (message,LG_STR("Error in opening file \"%s\": %s",
				       "Erreur en ouvrant le fichier \"%s\": %s"), path, strerror(errno));
	       report_parser_error(message);
	  } else {
	       perror("open_file_for_yyparse: open");
	       fprintf(stderr, LG_STR("ERROR: open_file_for_yyparse: could not open: %s\n",
				      "ERREUR: open_file_for_yyparse: ne peut ouvrir: %s\n"), path);
	  }
	  return -1;
     }
     
     sprintf(parser_indent,"%*s",parser_index,""); /* parser_index white spaces. */

     if (!be_quiet) fprintf(stdout,LG_STR("%sOpening file \"%s\" for parsing.\n",
					  "%sOuverture fichier \"%s\" pour parsing.\n"),
			    parser_indent, path);

     parser_state_save[parser_index] = YY_CURRENT_BUFFER;
     saved_parse_source[parser_index] = parse_source;

     oprs_yy_switch_to_buffer(oprs_yy_create_buffer(file_stream,YY_BUF_SIZE));

     parser_index++;
     parser_line_number[parser_index] = 1;
     parser_file_name[parser_index] = DUP(path); /* */
     FILE_save[parser_index] = file_stream;

     parse_source = PS_FILE;

     enable_slist_compaction();	/* usually we are going to build lists... */

     return 0;
}

void yy_delete_buffer_no_arg(void)
{
	  oprs_yy_delete_buffer(YY_CURRENT_BUFFER);
}

void close_file_for_yyparse(void)
{
     if (parse_source != PS_FILE) return; /*  */

     sprintf(parser_indent,"%*s",parser_index-1,""); /* parser_index white spaces. */

     if (!be_quiet) fprintf(stdout,LG_STR("%sClosing file \"%s\" after parsing.\n",
					  "%sFermeture fichier \"%s\" après parsing.\n"), 
			    parser_indent,parser_file_name[parser_index]);

     /* the file name was duplicated, when opened */
     FREE(parser_file_name[parser_index]);
     parser_file_name[parser_index] = NULL;
     parser_line_number[parser_index] = 0;
     fclose(FILE_save[parser_index]);

     oprs_yy_delete_buffer(YY_CURRENT_BUFFER);

/*      if (parser_index == 0) { */
/* 	  parse_source = saved_parse_source; */
/*      } */

     parser_index--;

     oprs_yy_switch_to_buffer(parser_state_save[parser_index]);
     parse_source = saved_parse_source[parser_index];

     compile_dynamic_slist();
}

