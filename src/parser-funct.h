/*                               -*- Mode: C -*- 
 * parser-funct.h -- 
 * 
 * Copyright (c) 1991-2012 Francois Felix Ingrand.
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


#ifndef INCLUDE_parser_function
#define INCLUDE_parser_function

#ifdef VXWORKS
#include "vxWorks.h"
#include "stdioLib.h"
#else
#include <stdio.h>
#endif

#include "opaque.h"

typedef enum {PS_FILE, PS_STRING, PS_SOCKET} Parse_Source_Type;

extern PBoolean i_have_the_stdin;
extern PBoolean quit;
extern Parse_Source_Type parse_source;
extern char s_parsed[BUFSIZ];
extern PBoolean ope_parser;
extern PBoolean stop_parsing_from_socket;
extern PBoolean parse_and_print_english_operator;


#define MAX_OPEN_INCLUDE 128

extern int string_line_number;
extern int parser_line_number[MAX_OPEN_INCLUDE];
extern char *parser_file_name[MAX_OPEN_INCLUDE];
extern char parser_indent[MAX_OPEN_INCLUDE];
extern int parser_index;

extern PBoolean closed_file;
extern PBoolean use_dialog_error;

extern char *progname;

extern PString oprs_data_path;
extern PString parser_message;
extern PBoolean too_long_message;

#define MAX_LENGTH_PARSER_MESSAGE 1000

/* Macro to allocate and create a string equal to from (which is double quoted...) pointed by to (without the quote). */
#define NEWQSTR_ENVAR(from, to) do {char * tmp;\
                                    NEWQSTR(from,tmp);\
				    to = replace_env_tring(tmp);\
				    FREE(tmp);\
				   } while (0)


#endif /* INCLUDE_parser_function */
