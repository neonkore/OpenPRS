/*                               -*- Mode: C -*- 
 * parser-funct.c -- 
 * 
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


#include <sys/param.h>
#include <sys/stat.h>

#include "constant.h"
#include "oprs-type.h"
#include "oprs-sprint.h"
#include "parser-funct.h"

#include "mallocPack_f.h"
#include "parser-funct_f.h"
#include "oprs-lex.h"
#include "oprs-sprint_f.h"

PString oprs_data_path = NULL;

void close_file_for_yyparse(void);


PBoolean i_have_the_stdin = TRUE; /* True when we are talking to the stdin. */
PBoolean quit = FALSE;
Parse_Source_Type parse_source = PS_FILE;
PBoolean use_dialog_error = FALSE; /* Should we use dialog box to report the error */

int pretty_width;
char char_to_parse;
char *s_pretty_parsed;
char s_parsed[BUFSIZ];	/* It is not that big... but usually enough */

PBoolean file_readable(char *path)
{
#ifdef VXWORKS
     int fd;

     if ((fd = open(path,READ,0)) != ERROR) {
	  close(fd);
	  return TRUE;
     } else 
	  return FALSE;
#else
     return (access(path,R_OK) == 0);
#endif
}
PString find_file_path(PString path, PString name)
{
     PString res;
     PString cp;
     PBoolean more_path;
     char dir[MAXPATHLEN];
     int found = FALSE;

     dir[0] = '\0';
     
     more_path = (path && path[0]);

     /* absolute pathname? */
     if (name[0] == '/') {
	  if ((found = file_readable(name))) {
	       NEWSTR(name,res);
	       return res;
	  }
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
	  NEWSTR(dir,res);
     else
	  NEWSTR(name,res);

     return res;
}

PString canonicalize_file_name(PString file_name)
{
     char resolved_path[PATH_MAX];
     PString found_file_name;
     PString res = "";
     PString i;
     int j;

     found_file_name = find_file_path(oprs_data_path,file_name);
     FREE(file_name);

     for(j=PATH_MAX;j;resolved_path[--j]='\0');	/* to please purify which seems to have pb with realpath... */


     PROTECT_NULL_SYSCALL_FROM_EINTR(i, realpath(found_file_name, resolved_path));
     if (!i) {
	  if (errno != ENOENT
#ifdef VXWORKS
	      && (errno != S_nfsLib_NFSERR_NOENT)
	      && (errno !=  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
	       ) {
	       perror("canonicalize_file_name: realpath");
	       fprintf(stderr,LG_STR("\tfor file \"%s\".\n",
				     "\tpour le fichier \"%s\".\n"),
		       found_file_name);
	  }
     }
     FREE(found_file_name);
     NEWSTR(resolved_path,res);

     return res;
}

PString canonicalize_read_file_name(PString file_name)
{
     char resolved_path[PATH_MAX];
     PString found_file_name;
     PString res;
     PString  i;
     int j;

     for(j=PATH_MAX;j;resolved_path[--j]='\0');	/* to please purify which seems to have pb with realpath... */

     found_file_name = find_file_path(oprs_data_path,file_name);
     FREE(file_name);


     PROTECT_NULL_SYSCALL_FROM_EINTR(i,realpath(found_file_name, resolved_path));

     if (!i) {
	  perror("canonicalize_file_name: realpath");
	  fprintf(stderr,LG_STR("\tfor file \"%s\".\n",
				"\tpour le fichier \"%s\".\n"), found_file_name);
	  return found_file_name;
     }
     FREE(found_file_name);
     NEWSTR(i,res);

     return res;
}

PBoolean closed_file = FALSE;

extern char *oprs_yytext;

PString parser_message = NULL;
PBoolean too_long_message = FALSE;

void report_parser_error(PString message)
{
	  PString tmp;
	 if ( too_long_message) return;
	 if (parser_message != NULL) {
	  PString prev_message = parser_message;

	  int total_length;

	  total_length =  strlen(message) + strlen(prev_message) + 2; /* add a \n and the end of string char */

	  if (total_length > MAX_LENGTH_PARSER_MESSAGE) {
		   too_long_message = TRUE;
		   parser_message = tmp = (char *) MALLOC (total_length + 6);
		   tmp += NUM_CHAR_SPRINT(sprintf (tmp, "%s", prev_message));
		   tmp += NUM_CHAR_SPRINT(sprintf (tmp, "%s\n...\n", message));
	  } else {
		   parser_message = tmp = (char *) MALLOC (total_length);
		   tmp += NUM_CHAR_SPRINT(sprintf (tmp, "%s", prev_message));
		   tmp += NUM_CHAR_SPRINT(sprintf (tmp, "%s\n", message));
	  }
	  FREE(prev_message);
	 } else {
	  int total_length;

	  total_length =  strlen(message) + 3;
	  parser_message = (char *) MALLOC ( total_length);
	  NUM_CHAR_SPRINT(sprintf (parser_message, "\n%s\n", message));
	 }
}

char *progname;

PBoolean ope_parser = FALSE;

void warning(char *s)
{
     if (use_dialog_error) {
	  static Sprinter *message = NULL;
	  int size = strlen((parse_source == PS_STRING ? "internal string" : parser_file_name[parser_index])) +
	       MAX_PRINTED_INT_SIZE + strlen(s) + 6 + strlen(oprs_yytext);

	  if (! message) 
	       message = make_sprinter(BUFSIZ);
	  else
	       reset_sprinter(message);
	
	  SPRINT(message, size,
		 sprintf (f,"%s:%d: %s near %s",
			  (parse_source == PS_STRING ?
			   "internal string" : parser_file_name[parser_index]),
			  (parse_source == PS_STRING ? 
			   string_line_number : parser_line_number[parser_index]),
			  s, (strlen(oprs_yytext) ? oprs_yytext : "the end...")));
	  report_parser_error(SPRINTER_STRING(message));
     } else {

	  fprintf(stderr, "%s:%s%s%s:%d: %s%s%s \n", progname, (oprs_prompt?oprs_prompt:""),
		  (oprs_prompt?":":""),
		  (parse_source == PS_STRING ? "internal string" : parser_file_name[parser_index]),
		  (parse_source == PS_STRING ? string_line_number : parser_line_number[parser_index]),
		  s, (oprs_yytext[0] ? " near ":""), oprs_yytext);
     }
}

PBoolean yyparse_one_command_string(PString s)
{
     Parse_Source_Type string_saved_parse_source;
     int string_saved_line_number;
     PBoolean res;

     string_saved_parse_source = parse_source;
     string_saved_line_number = string_line_number;
     parse_source = PS_STRING;  
     string_line_number = 1;
     oprs_yy_scan_string(s);	   
     yy_begin_COMMAND();		/* On demarre le lex parser dans cet etat */

     if (oprs_yyparse() == 0) {
	  res = TRUE;
     } else {
 	  while (parse_source == PS_FILE) {/* We screw up, and we left a file open... bad bad... */
	       close_file_for_yyparse();
	  }
	  res = FALSE;
     }
     parse_source = string_saved_parse_source;
     string_line_number = string_saved_line_number;

     return res;
}

void print_oprs_prompt(void)
{
     if ( i_have_the_stdin )
	  printf("\n%s> ",oprs_prompt);
}

PBoolean debug_leak = FALSE;

void pmstats(void)
{
     pr_mstats(stderr,"OPRS Malloc functions.");
}

#ifdef VXWORKS

#define R_OK    4       /* Test for Read permission */
#define W_OK    2       /* Test for Write permission */
#define X_OK    1       /* Test for eXecute permission */
#define F_OK    0       /* Test for existence of File */

int access(char *selected_file,int mode)
{     
     int res;
     struct stat sf;

     PROTECT_SYSCALL_FROM_EINTR(res,stat(selected_file, &sf));

     if  (res == -1)
	  return res;
     else {
	  int st_mode = sf.st_mode;

	  switch (mode) {
	  case R_OK:		/* Test for Read permission */
	       if ((S_IRUSR | S_IRGRP | S_IROTH) & st_mode) return 0;
	       else return -1;
	       break;
	  case W_OK:		/* Test for Write permission */
	       if ((S_IWUSR | S_IWGRP | S_IWOTH) & st_mode) return 0;
	       else return -1;
	       break;
	  case X_OK:		/* Test for eXecute permission */
	       if ((S_IXUSR | S_IXGRP | S_IXOTH) & st_mode) return 0;
	       else return -1;
	       break;
	  case F_OK:		/* Test for existence of File */
	       return 0;
	       break;
	  default:
	       fprintf(stderr,"ERROR: Unknown access mode.\n");
	       return -1;
	       break;
	  }
     }
}		    
#endif

PBoolean check_exist_access(char *selected_file)
{
     return (access(selected_file,F_OK) != -1 || (errno != ENOENT
#ifdef VXWORKS
			&& (errno != S_nfsLib_NFSERR_NOENT)
			&& (errno !=  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
	));
}

PBoolean check_read_access(char *selected_file)
{
     return (access(selected_file,R_OK) != -1 || errno == ENOENT
#ifdef VXWORKS
			|| (errno == S_nfsLib_NFSERR_NOENT)
			|| (errno ==  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
);
}

PBoolean check_write_access(char *selected_file)
{
     return (access(selected_file,W_OK) == 0 || ( errno == ENOENT)
#ifdef VXWORKS
			|| (errno == S_nfsLib_NFSERR_NOENT)
			|| (errno ==  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
);
}

char *compilable_opf(char *filename)
/*  Return the filename of a .opf file with a .dopf extentions if the .opf is newer or the .dopf does not exist. */
{
     char *pos, *dopf_filename;

     if ((pos = strrchr(filename,'.'))) {
	  if (strcasecmp(".dopf",pos) == 0) {
	       return NULL;	/* we where given a dopf filename... */
	  } else if (strcasecmp(".opf",pos) == 0) {
	       struct stat dopf, opf;
	       Sprinter *sp;

	       if (stat(filename, &opf) < 0) {
		    perror("compilable_opf:stat:filename:");
		    return NULL;
	       }

	       sp = make_sprinter(0);
	       pos[0] = '\0';
	  
	       SPRINT(sp, 5 + strlen(filename), sprintf(f,"%s.dopf", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),dopf_filename);
	       free_sprinter(sp);

	       pos[0] = '.';

	       if (stat(dopf_filename, &dopf) < 0){   
		    if ((errno != ENOENT)
#ifdef VXWORKS
			&& (errno != S_nfsLib_NFSERR_NOENT)
			&& (errno !=  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
			 )
			 perror("compilable_opf:stat:dopf_filename:");
		    return dopf_filename; /* Does not exist... */
	       }
	       
	       if (!check_write_access(dopf_filename)) {
		    fprintf(stderr,LG_STR("ERROR: No permission to write: %s.\n",
					  "ERREUR: Pas le droit d'écrire le fichier: %s.\n"), dopf_filename);
		    FREE(dopf_filename);
		    return NULL;
	       } else if (opf.st_mtime > dopf.st_mtime)
		    return dopf_filename; /* dopf is older. */
	       else {
		    FREE(dopf_filename);
		    return NULL;
	       }
	  }
     }
     fprintf(stderr,LG_STR("ERROR: Filename has no .opf extension: %s.\n",
			   "ERREUR: Fichier n'a pas de sufixe .opf: %s.\n"), filename);
     return NULL;		/* Cannot figure out extension.... */
}

char *read_file_name_newer_dopf_p(char *filename)
{
     char *pos, *dopf_filename;

     if ((pos = strrchr(filename,'.'))) {
	  if (strcasecmp(".dopf",pos) == 0) {
	       DUP(filename);	/* It's gonna be free twice... */
	       return filename;	/* we where given a dopf filename... */
	  } else if (strcasecmp(".opf",pos) == 0) {
	       struct stat dopf, opf;
	       Sprinter *sp;

	       if (stat(filename, &opf) < 0) {
		    perror("read_file_name_newer_dopf_p:stat:filename:");
		    return NULL;
	       }

	       sp = make_sprinter(0);
	       pos[0] = '\0';
	  
	       SPRINT(sp, 5 + strlen(filename), sprintf(f,"%s.dopf", filename)); 
	       NEWSTR(SPRINTER_STRING(sp),dopf_filename);
	       free_sprinter(sp);

	       pos[0] = '.';

	       if (stat(dopf_filename, &dopf) < 0){   
		    if ((errno != ENOENT)
#ifdef VXWORKS
			&& (errno != S_nfsLib_NFSERR_NOENT)
			&& (errno !=  S_netDrv_NO_SUCH_FILE_OR_DIR)
#endif
			 )
			 perror("read_file_name_newer_dopf_p:stat:dopf_filename:");
		    FREE(dopf_filename);
		    return NULL;
	       }
     
	       if (!check_read_access(dopf_filename)) {
		    fprintf(stderr,LG_STR("ERROR: No permission to read: %s.\n",
					  "ERREUR: Pas le droit de lire le fichier: %s.\n"), dopf_filename);
		    FREE(dopf_filename);
		    return NULL;
	       } else if (opf.st_mtime < dopf.st_mtime) {
		    return dopf_filename;
	       } else {
		    FREE(dopf_filename);
		    return NULL;
	       }
	  }
     }
     return NULL;
}

PString replace_env_string(PString s)
{
     /* The string is mine, I can play with it. */
     static Sprinter *sp = NULL;
     char *index;

     /* fprintf(stderr, "\"%s\" transformed in",s); */

     if (!sp) sp = make_sprinter(0);
     else reset_sprinter(sp);

     while ((index = strchr(s,'$'))) {
	  char *findex;

 	  if (index[1] == '{' && (findex = strchr(index+1,'}'))) {
	       char *env;

	       index[0] = findex[0]='\0'; /* replace the $ with a null, and terminate the envar */
	       SPRINT(sp,strlen(s),sprintf(f,"%s",s));
	       env = getenv(index+2); /* this is the envar */
	       if (env) SPRINT(sp,strlen(env),sprintf(f,"%s",env));
	       s = findex +1;
	  } else {		/* we only found a $ */
	       index[0]='\0';
	       SPRINT(sp,strlen(s)+1,sprintf(f,"%s$",s));
	       s = index+1;			/* print and skip the $ */
	  }
     }
     SPRINT(sp,strlen(s),sprintf(f,"%s",s));
     NEWSTR(SPRINTER_STRING(sp),index);
     
     /* fprintf(stderr, " \"%s\".\n",index); */
     
     return index;
}
