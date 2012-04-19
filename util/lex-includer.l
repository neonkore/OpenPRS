%{
/*                               -*- Mode: C -*- 
 * lex-includer.l -- 
 * 
 * Copyright (C) 1994-2011 Francois Felix Ingrand
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

/*
 * This little utility allow us to have lex and yacc grammars composed of
 * different submodules which are included in the main grammar file. We
 * could not use cpp nor m4 because of interaction with # and include
 * strings in the grammar. Moreover, this utility will remove C
 * comments in the file, allowing us to put comments in lex and yacc file
 * wherever we want.
 */

#include <string.h>
%}

%option noyywrap

%%
^include[ \t]*\"[^\"\n]*\"$	{output_file_to_stdout(yytext);}
\/\*				{eat_up_c_comment();}
%%
#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include "constant.h"
#include "macro.h"

char * vpath=NULL;

eat_up_c_comment()
{
     register int c;

     for ( ; ; )
     {
	  while ( (c = input()) != '*' &&
		 c != EOF )
	       ;		/* eat up text of comment */

	  if ( c == '*' )
	  {
	       while ( (c = input()) == '*' )
		    ;
	       if ( c == '/' )
		    break;	/* found the end */
	  }

	  if ( c == EOF )
	  {
	       fprintf(stderr, "EOF in comment" );
	       break;
	  }
     }
}

int file_readable(char *path)
{
     return (access(path,R_OK) == 0);
}

char * find_file_path(char *path, char *name)
{
     char *res;
     char *cp;
     int more_path;
     char dir[MAXPATHLEN];
     int found = FALSE;

     dir[0] = '\0';
     
     more_path = (path && path[0]);

     if (found = file_readable(name)) {
	  NEWSTR_STD(name,res);
	  return res;
     } else {
	  while (! found && more_path) {
	       if ((cp = INDEX(path, ':')) != NULL) {
		    strncpy(dir, path, cp - path);
		    dir[cp - path] = '\0';
		    strcat(dir, "/");
		    path = cp + 1;
	       } else {
		    strcpy(dir, path);
		    strcat(dir, "/");
		    more_path = FALSE;
	       } 
	       strcat(dir, name);
	       found = file_readable(dir);
	  }

	  if (! found) {
	       strcpy(dir, name);
	       found = file_readable(dir);
	  }
     }

     if (found)
	  NEWSTR_STD(dir,res);
     else
	  NEWSTR_STD(name,res);

     return res;
}

output_file_to_stdout(char *command)
{
     char file_name[BUFSIZ];
     FILE *file;
     char *start = strchr(command, '"');
     char *end = strrchr(command, '"');

     if (start && end && (start != end)) {
	  strncpy(file_name, start + 1, end - start - 1);
	  file_name[end - start - 1] = '\0';
	  if (file = fopen(find_file_path(vpath,file_name),"r")) {
	       int c;
/* 	       printf("#line 1 \"%s\"\n",file_name); */
	       while ((c = getc(file)) != EOF) {
		    if (c == '/') {
			 if ((c = getc(file)) != EOF) {
			      if (c == '*') {
				   for ( ; ; ) {
					while ((c = getc(file)) != '*' &&  c != EOF )
					     ; /* eat up text of comment */

					if ( c == '*' ) {
					     while ( (c = getc(file)) == '*' )
						  ;
					     if ( c == '/' )
						  break; /* found the end */
					}
			 
					if ( c == EOF ) {
					     fprintf(stderr, "EOF in comment" );
					     break;
					}
				   }
			      } else {
				  putchar('/');
				  putchar(c);
			      }
			 } else {
			     putchar('/');
			     break;
			 }			 
		    } else putchar(c);
	       }
	       fclose(file);
	  } else  fprintf(stderr,"Unable to open file: %s\n", file_name);
     } else fprintf(stderr,"Bad command format: %s\n", command);
}

main(int argc, char **argv)
{

     extern int optind;
     extern char *optarg;
     int c, getoptflg=0;

/*     ++argv, --argc;		/* skip over program name */

     while ((c = getopt(argc, argv, "p:h")) != EOF)
	  switch (c) {
	  case 'p':
	       NEWSTR_STD(optarg,vpath);
	       break;
	  case 'h':
	  default:
	       getoptflg++;
	  } 


     if (getoptflg) {
	  fprintf(stderr, "Wrong argument.");
	  exit(1);
     }

     yyin = stdin;

     yylex();
     
     exit(0);
}
