
/*                               -*- Mode: C -*- 
 * xhelp.c -- 
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


#define URL_DOC

#include <stdlib.h>

#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "oprs-type.h"

#include "oprs-sprint.h"
#include "oprs-sprint_f.h"

/*
 * Standard Toolkit include files
 */
#include <Xm/Form.h>


/* #include "xinfo/Info.h" */

#include "soak_f.h"
#include "xhelp.h"
#include "xhelp_f.h"

void send_URL_to_netscape(const char *url, PBoolean force_new)
{
     pid_t child;
    
     if ((child = fork()) == 0) {
	  /* child */

	  if (force_new) {
	       if (execlp("netscape", "netscape", url, NULL) == -1) {
		    perror("send_URL_to_netscape: execlp: netscape");
		    exit(0);	/* So the father will not bother... calling it again */
	       }
	  } else {
	       char buf[2048];

	       sprintf(buf, "openURL(%s,new-window)", url);
	       if (execlp("netscape", "netscape", "-remote", buf, NULL) == -1) {
		    perror("send_URL_to_netscape: execlp: netscape");
		    exit(0);	/* So the father will not bother... calling it again */
	       }		    
	  }
     } else {
	  if (child == -1) {
	       perror("send_URL_to_netscape: fork");
	       return;
	  }

	  /* parent */

	  if (!force_new) {

	       int stat_loc, stat_wait;

	       /* wait for the "netscape" child to exit; it will do this quickly
		  either after sending the command or issuing an error if
		  netscape isn't running */
	       while ((stat_wait = waitpid(child, &stat_loc, 0) == -1) &&
		       (errno == EINTR)) ; /* Empty while */
	       if (stat_wait == -1) {
		    perror("send_URL_to_netscape: waitpid");
		    return ;
	       }

	       if (WIFEXITED(stat_loc)) {
		    if (WEXITSTATUS(stat_loc) == EXIT_SUCCESS)
			 return;
		    else
			 if (! force_new) {
			      fprintf(stderr, "Starting netscape.\n");
			      send_URL_to_netscape(url,TRUE);
			      return ;
			 }
		    
	       }
	       fprintf(stderr, "Cannot start browser for the documentation.\n");
	  }
     }
}

#ifndef URL_DOC
Widget helpInfo;
Widget formInfo;
#endif

#ifndef URL_DOC
static void quit_help(Widget w, XtPointer client_data, XtPointer call_data)
{
     XtUnmanageChild((Widget)client_data);

}
#endif

PString oprs_doc_dir = NULL;

#ifndef URL_DOC
void create_help_info(Widget parent)
{
     Arg args[10];
     Cardinal n;
     static XtCallbackRec call[2];

     n = 0;
     XtSetArg(args[n], XmNresizable, TRUE);		n++;
     formInfo = XmCreateFormDialog(parent, "helpFormDialog", args, n);


     call[0].callback = (XtCallbackProc)quit_help;
     call[0].closure = (XtPointer)formInfo;
     call[1].callback =  (XtCallbackProc)NULL;
     call[1].closure = (XtPointer)NULL;

     n = 0;
     XtSetArg(args[n], XmNresizable, TRUE);		n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XtNcallback, call);		n++;
     if ((oprs_doc_dir = getenv("OPRS_DOC_DIR"))) {
	  XtSetArg(args[n], XpNinfoPath, oprs_doc_dir); n++;
     }
     helpInfo = XtCreateManagedWidget("info", infoWidgetClass, formInfo, args, n);
}
#else
void create_help_info(void)
{
     fprintf(stderr, "The documentation cannot be browsed from the application, use your PDF viewer to read it.\n");
}
#endif

/* Uncommnent this if you want to check the existence of all the Doc entries  referenced in the code */
/* You must do the same thing in Info.c too... */
/* #define DOC_DEBUG */

char *make_perl_string(char *string)
{
     char *res, *tmp;

     NEWSTR(string,res);
     
     tmp = res;
     while (*tmp) {
	  if (strchr(" ?,;:/",*tmp))
	       *tmp = '_';
	  tmp++;
     }
     /* fprintf(stderr, "%s -> %s\n", string, res); */

     return res;
}

FileNode *makeFileNode(String file, String node)
{
#ifndef URL_DOC
#ifdef FIND_UNEXISTING_NODE
     Cardinal n;
     Arg args[3];
#endif
#endif
     FileNode *filenode = MAKE_OBJECT(FileNode);
     
     filenode->file = file;
     /* filenode->file = "oprs";       there is only one doc file now... */
#ifdef URL_DOC
     filenode->node = make_perl_string(node);
#else
     filenode->node = node;
#endif

#ifndef URL_DOC
#ifdef FIND_UNEXISTING_NODE
     n = 0;			/* This will check that there is a documetation for each of these nodes... and complain otherwise...*/
     XtSetArg(args[n], XpNinfoFile, file); n++;
     XtSetArg(args[n], XpNinfoNode, node); n++;
     XtSetValues(helpInfo, args, n);
#endif
#endif

     return filenode;


}

#ifndef URL_DOC
void infoHelp(Widget w,  XtPointer fn_par, XtPointer call_data)
{
     Cardinal n;
     Arg args[3];
     
     String file, node;
     FileNode  *fn = (FileNode *) fn_par;

     if (fn) {			/* If null, just pop up the info window... */
	  file = fn->file;	/* Do not ask why this is so.. I used to allocate them... */
	  node = fn->node;

	  n = 0;
	  XtSetArg(args[n], XpNinfoFile, file); n++;
	  XtSetArg(args[n], XpNinfoNode, node); n++;
	  XtSetValues(helpInfo, args, n);
     }
     n = 0;
     XtSetArg(args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
     XtSetValues(XtParent(helpInfo), args, n);
     XtManageChild(XtParent(helpInfo));	/* We manage the dialog box, not the info pane itself. */
}
#else
void infoHelp(Widget w,  XtPointer fn_par, XtPointer call_data)
{
     FileNode  *fn = (FileNode *) fn_par;
     char *doc;
     char url_doc[BUFSIZ];
     char default_doc[BUFSIZ];

     if (! (oprs_doc_dir)) {
	  fprintf(stderr, "OPRS_DOC_DIR undefined.\n");
	  return;
     }

     sprintf(default_doc,"%s/%s", oprs_doc_dir, "oprs.html");
	       
     if (! (fn && fn->node)) {
	  /*  fprintf(stderr, "Empty Help File Node, using default doc.\n"); */
	  doc = default_doc;
     } else {
	  FILE *find_url_popen;
	  char *find_url_com = "find_url_in_doc.pl";
	  char *find_url_arg =  fn->node;
	  static Sprinter *com = NULL;

	  if (com) 
	       reset_sprinter(com);
	  else 
	       com= make_sprinter(0);

	  SPRINT(com, strlen(find_url_arg) + strlen(find_url_com) + strlen(oprs_doc_dir) + 3,
		 sprintf(f,"%s %s %s", find_url_com, oprs_doc_dir, find_url_arg));

	  block_sigalarm();
	  if ((find_url_popen = popen(SPRINTER_STRING(com), "r"))) {
	       if (!(fgets(url_doc, BUFSIZ, find_url_popen))) {
		    fprintf(stderr, "Cannot fgets from: %s\n", SPRINTER_STRING(com));
		    doc = default_doc;
	       } else
		    doc = url_doc;
	       pclose(find_url_popen);
	  } else {
	       fprintf(stderr, "Cannot popen: %s\n", SPRINTER_STRING(com));
	       doc = default_doc;
	  }
	  unblock_sigalarm();
     }
     send_URL_to_netscape(doc, FALSE);
}
#endif


#if 0
int main(unsigned int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s URL\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (fork() == 0)
    send_URL_to_netscape(argv[1]);
    /*
	send_URL_to_InternetExplorer(argv[1]);
    */
  
  return EXIT_SUCCESS;
}
#endif
