/*                               -*- Mode: C -*- 
 * goprs-main.c -- Top level file for the GTK interface for OpenPRS.
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#include <string.h>

#include <gtk/gtk.h>

#include "xm2gtk.h"

#include "xhelp.h"
#include "xhelp_f.h"

#include "constant.h"
#include "macro.h"
#include "version.h"

#include "oprs-type.h"
#include "op-structure.h"
#include "oprs.h"
#include "parser-funct.h"
#include "oprs-main.h"
#include "int-graph.h"
#include "top-lev.h"
#include "op-default.h"

#include "gope-graphic.h"
#include "goprs-intention.h"
#include "goprs-textwin_f.h"

#include "mp-register.h"
#include "goprs-main.h"

#include "gope-graphic_f.h"
#include "goprs-intention_f.h"
#include "gtop-lev_f.h"
#include "goprs-menu_f.h"
#include "oprs-type_f.h"
#include "oprs-init_f.h"

#include "xm2gtk_f.h"


gboolean wait_other_events(gpointer oprs);
void reset_oprs_kernel(Oprs *oprs);
gint update_active_idle(gpointer oprs_par);
void UpdateMessageWindow(char *string);
void ClearMessageWindow(void);

void set_button(Widget w);
void unset_button(Widget w); 

Widget x_oprs_top_level_widget;

Widget topLevelWindow, messageWindow;

extern PBoolean use_dialog_error;

PBoolean main_loop_registered = FALSE;
PBoolean socket_registered = FALSE;

#include <pthread.h>

pthread_mutex_t loop_registering_mutex = PTHREAD_MUTEX_INITIALIZER;

gint input_ps, input_mp;

void register_main_loop_from_socket(gpointer data, gint ignore1, GdkInputCondition ignore2);

void deregister_other_inputs(Oprs *oprs)
{
  // fprintf(stderr, "deregister_other_inputs.\n");
  if (register_to_server) {
    gdk_input_remove(input_ps);
  }
  if (register_to_mp) {
    gdk_input_remove(input_mp);
  }
}

void register_other_inputs(Oprs *oprs)
{
  //  fprintf(stderr, "register_other_inputs.\n");
  if (register_to_server) {
    input_ps = gdk_input_add(ps_socket, GDK_INPUT_READ, &register_main_loop_from_socket, oprs);
  }
  if (register_to_mp) {
    input_mp = gdk_input_add(mp_socket, GDK_INPUT_READ, &register_main_loop_from_socket, oprs);
  }
}

void register_main_loop(gpointer data, gboolean from_socket)
{
  Oprs *oprs = data;
  if (pthread_mutex_lock(&loop_registering_mutex) < 0)
    perror("goprs: register_main_loop: pthread_mutex_lock");
  if (from_socket) {
    deregister_other_inputs(oprs);
    socket_registered = FALSE;
  }
  if (! main_loop_registered) {
    //fprintf(stderr, "registering_main_loop as idle.\n");
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, &goprs_top_level_loop,oprs, NULL); /* this will register the main loop */
    main_loop_registered = TRUE;
  }
  if (pthread_mutex_unlock(&loop_registering_mutex) < 0)
    perror("goprs: register_main_loop: pthread_mutex_unlock");

  return;
}

gboolean register_main_loop_from_timer(gpointer data)
{
  //  fprintf(stderr, "register_main_loop_from_timer.\n");
  register_main_loop(data, FALSE);
  return FALSE; /* This will deregister this very timeout function. */
}

void register_main_loop_from_socket(gpointer data, gint ignore1, GdkInputCondition ignore2)
{
  //  fprintf(stderr, "register_main_loop_from_socket.\n");
  register_main_loop(data, TRUE);
}

void deregister_main_loop(Oprs *oprs)
{
  if (main_loop_registered) {
    //fprintf(stderr, "deregister_main_loop.\n");
    if (pthread_mutex_lock(&loop_registering_mutex) < 0)
      perror("goprs: deregister_main_loop: pthread_mutex_lock");
    if (main_loop_registered) {
      //      fprintf(stderr, "deregistering_main_loop.\n");
      main_loop_registered = FALSE;
      g_timeout_add((main_loop_pool_sec * 1000) +  (main_loop_pool_usec / 1000),
		    &register_main_loop_from_timer, oprs); /* This guy will re register us. */
      if (!socket_registered) {
	socket_registered = TRUE;
	register_other_inputs(oprs);
      }
    }
    if (pthread_mutex_unlock(&loop_registering_mutex) < 0)
      perror("goprs: deregister_main_loop: pthread_mutex_unlock");
  } 
}

guint sb_cid;
GtkWidget *messageWindow;

void UpdateMessageWindow(char *string)
{
  gtk_statusbar_push(GTK_STATUSBAR(messageWindow), sb_cid, string);
}


//XtAppContext app_context;

#include "xoprs-icon.bit"

/* This should go in a library as it is shared with gope */
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

static gboolean
on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
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

  handle_exposures(widget, global_draw_data, event, &CGCs);

  destroy_cgcs(&CGCs);

  return FALSE;
}

gboolean motion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  //canvas_mouse_motion(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean released(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  //canvas_mouse_release(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  //  canvas_mouse_press(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

void ReallyQuit()
{
     wrap_up();
     gtk_main_quit ();
     
     exit(0);
}

void quitQuestionManage(GtkWidget *w, gpointer window)
{
  GtkWidget *dialog;

  char *message = "Do you really want to quit?";
    
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


Draw_Data *global_draw_data;
Draw_Data dd;
CairoGCs mainCGCs;		/* this will be the one for the main */
CairoGCs *mainCGCsp;		/* this will be the one for the main */


Widget textSWindow;
Widget textview;
int gtk = 1;

#ifdef C_PLUS_PLUS_RELOCATABLE
int oprs_main(int argc,char **argv, char ** envp)
#else
int main(int argc, char **argv, char **envp)
#endif
{
     char *server_hostn;
     int server_port;

     char *mp_hostn;
     int mp_port;

#ifdef XEDITRESCHECKMESSAGES_AVAILABLE
     extern void _XEditResCheckMessages();
#endif
     Pixmap icon_pixmap;
     Cardinal n;
     //     Arg args[MAXARGS];
     char *name;
     char title[LINSIZ];
     char welcome[LINSIZ];
     Draw_Data dd;
     Int_Draw_Data idd;
     Oprs *oprs;
     char *log_file;
     char *language_str;

  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *menubar;
  //  GtkWidget *toolbar;
  GtkWidget *opeDrawWin;
  GtkWidget *intDrawWin;

     Widget topForm, oprsDrawWinFrame, oprsDrawWin, oprsIntDrawWinFrame, oprsIntDrawWin;
     Widget oprsMenuFrame, oprsMenu;

     progname = argv[0];

     use_dialog_error = TRUE;

     global_draw_data = &dd;
     global_int_draw_data = &idd;



     //     print_english_operator = Resrcs.printEnglishOperator;
     //     log_file = Resrcs.log_file;
     //     language_str = Resrcs.language;
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

     select_language(language_str, TRUE);

     oprs_init_env();

  gtk_init(&argc, &argv);

     name = oprs_init_arg(argc, argv, &mp_hostn, &mp_port, &server_hostn, &server_port);

#if SETBUF_UNDEFINED
     setbuffer(stdout,NULL,0);
#else
     setbuf(stdout,NULL);
#endif

     oprs = oprs_init_main(name, TRUE, mp_hostn, mp_port, server_hostn, server_port);

     x_oprs_top_level_widget = topLevelWindow; /*  This is the external name . */

     sprintf(title,"GOprs %s --- %s",  package_version, name);

  topLevelWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  dd.topLevelWindow = topLevelWindow;
  idd.topLevelWindow = topLevelWindow;
  gtk_window_set_title(GTK_WINDOW(topLevelWindow), title);
  gtk_window_set_default_size(GTK_WINDOW(topLevelWindow), 1400, 800);
  gtk_window_set_position(GTK_WINDOW(topLevelWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_icon(GTK_WINDOW(topLevelWindow), create_pixbuf("xoprs-icon.png"));
  g_signal_connect(G_OBJECT(topLevelWindow), "destroy",
  		   G_CALLBACK(quitQuestionManage), topLevelWindow);


  vbox = gtk_vbox_new(FALSE, 0); /* The main vbox */
  gtk_container_add(GTK_CONTAINER(topLevelWindow), vbox); /* in the topLevelWindow */
  
  menubar = goprs_create_menu_bar(topLevelWindow, &dd, &idd);
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 1); /* add a menubar to the main vbox */

  
  GtkWidget *vpaned = gtk_vpaned_new ();
  gtk_box_pack_start(GTK_BOX(vbox), vpaned, TRUE, TRUE, 1); /* add a menubar to the main vbox */


  hbox = gtk_hbox_new(FALSE, 0);
  //gtk_widget_set_size_request (vpaned, 200 + GTK_PANED (vpaned)->gutter_size, -1);
  gtk_paned_pack1 (GTK_PANED (vpaned), hbox, TRUE, FALSE);
  //gtk_widget_set_size_request (frame1, 50, -1);

  opeDrawWin = gtk_scrolled_window_new(NULL, NULL); /* create a large scrolled window in this vbox */
  gtk_paned_pack2 (GTK_PANED (vpaned), opeDrawWin, TRUE, FALSE);
  //gtk_widget_set_size_request (frame2, 50, -1);

  dd.canvas = gtk_layout_new(NULL,NULL);
  gtk_widget_set_app_paintable(dd.canvas, TRUE);
  gtk_container_add(GTK_CONTAINER(opeDrawWin),dd.canvas);

  gtk_layout_get_size(GTK_LAYOUT(dd.canvas),&dd.canvas_width, &dd.canvas_height);
  dd.work_height = MAX(WORK_HEIGHT, dd.canvas_height);
  dd.work_width = MAX(WORK_WIDTH, dd.canvas_width);
  gtk_layout_set_size(GTK_LAYOUT(dd.canvas), dd.work_width, dd.work_height); 

  dd.op = NULL;
  dd.mode = MOVING_CANVAS;
  dd.top = 0;
  dd.left = 0;
  // dd.copy_area_index_queue = make_queue();
  dd.node_selected = NULL;
  dd.sensitive_og = NULL;
  dd.edge_selected = NULL;
  dd.og_selected_on_press = NULL;
  dd.og_align = NULL;
  dd.og_aligning = NULL;
  /* dd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT); */
  /*     dd.fontlist = XmFontListCreate(dd.font,"opeCanvas_Charset"); */
  // dd.fontlist = Resrcs.fontList;
  dd.expose_region = NULL;
  dd.just_compiling = FALSE;

  //  init_draw_mode_help();


  oprsMenu = create_tool_bar(topLevelWindow, &dd);
  gtk_box_pack_start(GTK_BOX(hbox), oprsMenu, FALSE, FALSE, 1); /* add a oprsMenu at the left  */

  textSWindow = gtk_scrolled_window_new(NULL, NULL); /* create the text  window in this hbox */
  gtk_box_pack_start(GTK_BOX(hbox), textSWindow, TRUE, TRUE, 1);
  textview = gtk_text_view_new ();
  gtk_container_add (GTK_CONTAINER (textSWindow), textview);

  intDrawWin = gtk_scrolled_window_new(NULL, NULL); /* create the int graph window in this hbox */
  gtk_box_pack_start(GTK_BOX(hbox), intDrawWin, TRUE, TRUE, 1);

  idd.canvas = gtk_layout_new(NULL,NULL);
  gtk_widget_set_app_paintable(idd.canvas, TRUE);
  gtk_container_add(GTK_CONTAINER(intDrawWin),idd.canvas);

  gtk_layout_get_size(GTK_LAYOUT(idd.canvas),&idd.canvas_width, &idd.canvas_height);
  idd.work_height = MAX(WORK_HEIGHT, idd.canvas_height);
  idd.work_width = MAX(WORK_WIDTH, idd.canvas_width);
  gtk_layout_set_size(GTK_LAYOUT(idd.canvas), idd.work_width, idd.work_height); 

  idd.top = 0;
  idd.left = 0;
  idd.ig = NULL;
  idd.expose_region = NULL;
  idd.reposition_all = TRUE;

  
  messageWindow = gtk_statusbar_new ();
  gtk_box_pack_end(GTK_BOX(vbox), messageWindow, FALSE, FALSE, 1); /* add a  statusbar at the bottom. */
  sb_cid = gtk_statusbar_get_context_id(GTK_STATUSBAR(messageWindow),"Default");
  sprintf(title, LG_STR("Welcome to the GTK OpenPRS %s %s",
			"Bienvenue dans le GTK OpenPRS %s %s"), package_version, COPYRIGHT_STRING);
  UpdateMessageWindow(title);

  sprintf(welcome, LG_STR("\n\t\tThis is GTK-OpenPRS (%s).\n\t%s\n\n",
			  "\n\t\tThis is GTK-OpenPRS (%s).\n\t%s\n\n"),
	  package_version, COPYRIGHT_STRING);

  AppendTextWindow(GTK_TEXT_VIEW(textview),welcome,FALSE);
  AppendTextWindow(GTK_TEXT_VIEW(textview), "GTK-OpenPRS comes with ABSOLUTELY NO WARRANTY.\n\n",FALSE);

  gtk_widget_show_all(topLevelWindow);

  dd.window = GTK_LAYOUT(dd.canvas)->bin_window;

  mainCGCsp = &mainCGCs;
  dd.cgcsp = mainCGCsp;
  
  create_cgcs(&mainCGCs, dd.window); /* this will create all the cairo context for the main loop... */
   
  gtk_widget_add_events (dd.canvas, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events (dd.canvas, GDK_BUTTON_RELEASE_MASK);
  gtk_widget_add_events (dd.canvas, GDK_POINTER_MOTION_MASK);

  g_signal_connect(dd.canvas, "expose-event",
		   G_CALLBACK(on_expose_event), NULL);
  g_signal_connect(dd.canvas, "button-press-event", 
		   G_CALLBACK(clicked), NULL);
  g_signal_connect(dd.canvas, "button-release-event", 
		   G_CALLBACK(released), NULL);
  g_signal_connect(dd.canvas, "motion-notify-event", 
		   G_CALLBACK(motion), NULL);

  idd.ig = oprs->intention_graph;

  call_oprs_cat(log_file,textview);
  
  start_kernel_user_hook(name);

  run_initial_commands();

  register_main_loop(oprs,FALSE);   /* this will register the main loop as an idle func.  */

  gtk_main();

  return 0;

#ifdef IGNORE
 
     XtSetArg(args[0], XmNheight, &idd.canvas_height);
     XtSetArg(args[1], XmNwidth, &idd.canvas_width);
     XtGetValues(idd.canvas, args, 2);

     idd.copy_area_index_queue = make_queue();
     idd.work_height = MAX(IDD_WORK_HEIGHT, idd.canvas_height);
     idd.work_width = MAX(IDD_WORK_WIDTH, idd.canvas_width);
     idd.top = 0;
     idd.left = 0;
     idd.ig = NULL;
     idd.expose_region = NULL;
     idd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT);
     idd.fontlist = Resrcs.fontList;
     idd.reposition_all = TRUE;

     XtAddCallback(idd.canvas, XmNexposeCallback, (XtCallbackProc)idd_handle_exposures, &idd);
     XtAddCallback(idd.canvas, XmNresizeCallback, (XtCallbackProc)idd_resize, &idd);
     XtAddEventHandler(idd.canvas, 0, TRUE, (XtEventHandler)idd_handle_g_exposures, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonPressMask, FALSE, (XtEventHandler)idd_canvas_mouse_press, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonMotionMask, FALSE, (XtEventHandler)idd_canvas_mouse_motion, (XtPointer)&idd);
     XtAddEventHandler(idd.canvas, ButtonReleaseMask, FALSE, (XtEventHandler)idd_canvas_mouse_release, (XtPointer)&idd);

     idd_create_scrollbars(oprsIntDrawWin, &idd);

     n = 0;
     XtSetArg(args[n], XmNverticalScrollBar, idd.vscrollbar); n++;
     XtSetArg(args[n], XmNhorizontalScrollBar, idd.hscrollbar); n++;
     XtSetArg(args[n], XmNworkWindow, idd.canvas); n++;
     XtSetValues(oprsIntDrawWin, args, n);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNtopWidget, menuBar); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;

     XtSetValues(oprsMenuFrame, args, n);

     oprsDrawWin = XmCreateScrolledWindow(oprsDrawWinFrame, "oprsDrawWin", NULL, 0);
     XtManageChild(oprsDrawWin);

     dd.canvas = XmCreateDrawingArea(oprsDrawWin, "oprsCanvas", NULL, 0);
     XtManageChild(dd.canvas);

     XtSetArg(args[0], XmNheight, &dd.canvas_height);
     XtSetArg(args[1], XmNwidth, &dd.canvas_width);
     XtGetValues(dd.canvas, args, 2);

     dd.copy_area_index_queue = make_queue();
     dd.mode = MOVING_CANVAS;
     dd.og_selected_on_press = NULL;
     dd.work_height = MAX3(WORK_HEIGHT, Resrcs.work_height, (int)dd.canvas_height);
     dd.work_width = MAX3(WORK_WIDTH, Resrcs.work_width, (int)dd.canvas_width);
     dd.top = 0;
     dd.left = 0;
     dd.font = XLoadQueryFont(XtDisplay(topLevel), WORK_FONT);
     dd.fontlist = Resrcs.fontList;
     dd.expose_region = NULL;
     dd.just_compiling = FALSE;
     dd.op = NULL;

     XtAddCallback(dd.canvas, XmNexposeCallback, (XtCallbackProc)handle_exposures, &dd);
     XtAddCallback(dd.canvas, XmNresizeCallback, (XtCallbackProc)resize, &dd);
     XtAddEventHandler(dd.canvas, 0, TRUE, (XtEventHandler)handle_g_exposures, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonPressMask, FALSE, (XtEventHandler)oprs_canvas_mouse_press, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonMotionMask, FALSE, (XtEventHandler)oprs_canvas_mouse_motion, (XtPointer)&dd);
     XtAddEventHandler(dd.canvas, ButtonReleaseMask, FALSE, (XtEventHandler)oprs_canvas_mouse_release, (XtPointer)&dd);

     create_scrollbars(oprsDrawWin, &dd);

     n = 0;
     XtSetArg(args[n], XmNverticalScrollBar, dd.vscrollbar); n++;
     XtSetArg(args[n], XmNhorizontalScrollBar, dd.hscrollbar); n++;
     XtSetArg(args[n], XmNworkWindow, dd.canvas); n++;
     XtSetValues(oprsDrawWin, args, n);

     xpcreate_dialogs(topLevel,&dd);
     xp_create_showdb_dialog(topLevel);
     xp_create_showig_dialog(topLevel);
     xp_create_show_cond_dialog(topLevel);
     xp_create_show_intention_dialog(topLevel);
     xp_create_filesel(topLevel);
     xp_create_rop_dialog(topLevel, &dd);
     xp_information_create(topLevel);
     create_syntax_error_dialog(topLevel);

     XtRealizeWidget(topLevel);

     dd.window = XtWindow(dd.canvas);

     n = 0;
     XtSetArg(args[n], XmNforeground, &globalFore); n++;
     XtSetArg(args[n], XmNbackground, &globalBack); n++;
     XtGetValues(oprsActiveDButton, args, n);

     create_gc(&dd);

     idd_create_gc(&idd);
     UpdateMessageWindow(title);

     idd.ig = oprs->intention_graph;

     update_active_idle((XtPointer)oprs);

     start_kernel_user_hook(name);

     run_initial_commands();

     sprintf(title, "X-OPRS %s", package_version);
     UpdateMessageWindow(title);

     XtAppMainLoop(app_context);
     
     return(1);
#endif

}

gint update_active_idle(gpointer oprs_par)
{
 
     Oprs *oprs = (Oprs *)oprs_par;

     set_oprs_active_mode(! (sl_slist_empty(oprs->new_facts) &&
			    sl_slist_empty(oprs->new_goals) && /* We could be more specific... here */
			    sl_slist_empty(oprs->intention_graph->current_intentions)));
     g_timeout_add(100,update_active_idle,oprs);
     
     return FALSE;
}

void set_button(Widget w)
{   

#ifdef IGNORE
     Pixel back;
     Arg args[3];
     Cardinal n; 
     XmString xmstring;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtSetArg(args[n],  XmNlabelString, &xmstring); n++;
     XtGetValues(w,args,n);
     
     if (back != globalFore) {	/* not inversed */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, globalFore); n++;
	  XtSetArg(args[n], XmNforeground, globalBack); n++;
	  XtSetArg(args[n], XmNlabelString, xmstring); n++;
	  XtSetValues(w,args,n);
     }

#endif

}

void unset_button(Widget w)
{

#ifdef IGNORE
     Pixel back;
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtGetValues(w,args,n);
     
     if (back != globalBack) {	/* not inversed */
	  n = 0;
	  XtSetArg(args[n], XmNbackground, globalBack); n++;
	  XtSetArg(args[n], XmNforeground, globalFore); n++;
	  XtSetValues(w,args,n);
     }
#endif
}

void toggle_button(Widget w)
{

#ifdef IGNORE
     Pixel back,fore;
     Arg args[2];
     Cardinal n;

     n = 0;
     XtSetArg(args[n], XmNbackground, &back); n++;
     XtSetArg(args[n], XmNforeground, &fore); n++;
     XtGetValues(w,args,n);

     n = 0;
     XtSetArg(args[n], XmNbackground, fore); n++;
     XtSetArg(args[n], XmNforeground, back); n++;
     XtSetValues(w,args,n);
#endif
}

#ifdef IGNORE


void ClearMessageWindow(void)
{
     Arg args[1];
     XmString xmres;

     XtSetArg(args[0], XmNlabelString, xmres = XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET));
     XtSetValues(messageWindow, args, 1);
     XmStringFree(xmres);
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



//Pixel globalBack, globalFore;

struct _resrcs {
     XmFontList fontList;		  /* font to use for bitmap labels */
     int work_width, work_height;	  /* default bitmap size */
     PBoolean  printWidgetTree;
     PBoolean  printEnglishOperator;
     char *log_file;
     char *language;
     int  ip_x;
     int  ip_y;
     int  ip_width;
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
     {"logFile", "LogFile", XmRString, sizeof(String),
     XtOffsetOf(struct _resrcs, log_file), XmRImmediate, NULL},
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

     {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     XtOffsetOf(struct _resrcs, fontList), XmRImmediate, NULL},
     {"printWidgetTree", "PrintWidgetTree", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printWidgetTree), XmRImmediate, (XtPointer)FALSE},
     {"printEnglishOperator", "PrintEnglishOperator", XtRBoolean, sizeof(Boolean),
     XtOffsetOf(struct _resrcs, printEnglishOperator), XmRImmediate, (XtPointer)FALSE},
};

/* this is to allow our parsing of these options. */
static XrmOptionDescRec options[] = {
   {"-s",	"",	XrmoptionSkipArg,	NULL},
   {"-i",	"",	XrmoptionSkipArg,	NULL},
   {"-m",	"",	XrmoptionSkipArg,	NULL},
   {"-j",	"",	XrmoptionSkipArg,	NULL},
   {"-n",	"",	XrmoptionSkipArg,	NULL},
   {"-I",	"",	XrmoptionSkipArg,	NULL},
   {"-D",	"",	XrmoptionSkipArg,	NULL},
   {"-P",	"",	XrmoptionSkipArg,	NULL},
   {"-E",	"",	XrmoptionSkipArg,	NULL},
   {"-F",	"",	XrmoptionSkipArg,	NULL},
   {"-l",	"",	XrmoptionSkipArg,	NULL},
   {"-L",	"",	XrmoptionSkipArg,	NULL},
   {"-h",	"",	XrmoptionSkipNArgs,	0},
   {"-q",	"",	XrmoptionSkipNArgs,	0},
   {"-t",	"",	XrmoptionSkipNArgs,	0},
   {"-a",	"",	XrmoptionSkipNArgs,	0},
   {"-A",	"",	XrmoptionSkipNArgs,	0},
   {"-p",	"",	XrmoptionSkipNArgs,	0},
   {"-c",	"",	XrmoptionSkipArg,	NULL},
   {"-x",	"",	XrmoptionSkipArg,	NULL},
   {"-d",	"",	XrmoptionSkipArg,	NULL},
   {"-pwt",	"*printWidgetTree",	XrmoptionNoArg,	"True"},
   {"-log",	"*logFile",	XrmoptionSepArg,	NULL},
   {"-peo",	"*printEnglishOperator",	XrmoptionNoArg,	"True"},
     };

String fallback_resources[] = {
#include "XOprs.ad.h"
NULL
};
     topLevel =  XtVaAppInitialize(
				   &app_context, /* Application context */
				   "XOprs", /* application class name */
				   options, XtNumber(options),  /* command line option list */
				   &argc, argv, /* command line args */
				   fallback_resources,	/* for missing
							 * app-defaults file */
				   NULL); /* terminate varargs list */

#ifdef XEDITRESCHECKMESSAGES_AVAILABLE
     XtAddEventHandler(topLevel, (EventMask)0, True, _XEditResCheckMessages, NULL);
#endif

#endif
