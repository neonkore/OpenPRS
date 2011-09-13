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

  /* cairo_move_to(cr, 800,  30); */
  /* cairo_show_text(CGCs.cr_basic, "CGCs.cr_basic"); */
  /* cairo_move_to(CGCs.cr_title, 800, 60); */
  /* cairo_show_text(CGCs.cr_title, "CGCs.cr_title"); */

  /* cairo_move_to(CGCs.cr_edge, 800, 120); */
  /* cairo_show_text(CGCs.cr_edge, "CGCs.cr_edge"); */

  /* cairo_move_to(CGCs.cr_node, 800, 150); */
  /* cairo_show_text(CGCs.cr_node, "CGCs.cr_node"); */
  /* cairo_move_to(CGCs.cr_text, 800, 180); */
  /* cairo_show_text(CGCs.cr_text, "CGCs.cr_text"); */

  handle_exposures(widget, global_draw_data, event, &CGCs);

  destroy_cgcs(&CGCs);

  return FALSE;
}

gboolean motion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  goprs_canvas_mouse_motion(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean released(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  goprs_canvas_mouse_release(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_draw_data->window);

  goprs_canvas_mouse_press(widget, global_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

static gboolean
ion_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_int_draw_data->window);
  

  cairo_t *cr = CGCs.cr_basic;

  /* cairo_move_to(cr, 300,  30); */
  /* cairo_show_text(CGCs.cr_basic, "CGCs.cr_basic"); */
  /* cairo_move_to(CGCs.cr_title, 300, 60); */
  /* cairo_show_text(CGCs.cr_title, "CGCs.cr_title"); */

  /* cairo_move_to(CGCs.cr_edge, 300, 120); */
  /* cairo_show_text(CGCs.cr_edge, "CGCs.cr_edge"); */

  /* cairo_move_to(CGCs.cr_node, 300, 150); */
  /* cairo_show_text(CGCs.cr_node, "CGCs.cr_node"); */
  /* cairo_move_to(CGCs.cr_text, 300, 180); */
  /* cairo_show_text(CGCs.cr_text, "CGCs.cr_text"); */

  idd_handle_exposures(widget, global_int_draw_data, event, &CGCs);

  destroy_cgcs(&CGCs);

  return FALSE;
}

gboolean imotion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_int_draw_data->window);

  idd_canvas_mouse_motion(widget, global_int_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean ireleased(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_int_draw_data->window);

  idd_canvas_mouse_release(widget, global_int_draw_data, &CGCs, event);

  destroy_cgcs(&CGCs);

  return TRUE;
}

gboolean iclicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  CairoGCs CGCs;

  /* This will create graphic context specific to this expose thread. */
  create_cgcs(&CGCs, global_int_draw_data->window);

  idd_canvas_mouse_press(widget, global_int_draw_data, &CGCs, event);

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

gint update_active_idle(gpointer oprs_par)
{
  Oprs *oprs = (Oprs *)oprs_par;

  set_oprs_active_mode(! (sl_slist_empty(oprs->new_facts) &&
			  sl_slist_empty(oprs->new_goals) && /* We could be more specific... here */
			  sl_slist_empty(oprs->intention_graph->current_intentions)));
  return TRUE;
}

gint register_update_active_idle(gpointer oprs)
{
  g_timeout_add(100,update_active_idle,oprs);
}


Draw_Data *global_draw_data;
Draw_Data dd;

Int_Draw_Data idd;
CairoGCs opCGCs;		/* this will be the one for the main */
CairoGCs *opCGCsp;		/* this will be the one for the main */



CairoGCs inCGCs;		/* this will be the one for the main */
CairoGCs *inCGCsp;		/* this will be the one for the main */


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

  Pixmap icon_pixmap;
  Cardinal n;
  char *name;
  char title[LINSIZ];
  char welcome[LINSIZ];
  Oprs *oprs;
  char *log_file = NULL;
  char *language_str = NULL;

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

  /* under X11, these variables' value were grabbed from a resource file... I am not
     sure this was so useful, so I did not port it to GTK. */
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
  gtk_box_pack_start(GTK_BOX(vbox), vpaned, TRUE, TRUE, 1); /* add a vertical paned to the main vbox */


  hbox = gtk_hbox_new(FALSE, 0);
  gtk_paned_pack1 (GTK_PANED (vpaned), hbox, TRUE, FALSE);

  opeDrawWin = gtk_scrolled_window_new(NULL, NULL); /* create a large scrolled window in this vbox */
  gtk_paned_pack2 (GTK_PANED (vpaned), opeDrawWin, TRUE, FALSE);

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
  dd.node_selected = NULL;
  dd.sensitive_og = NULL;
  dd.edge_selected = NULL;
  dd.og_selected_on_press = NULL;
  dd.og_align = NULL;
  dd.og_aligning = NULL;
  dd.expose_region = NULL;
  dd.just_compiling = FALSE;

  //  init_draw_mode_help();

  oprsMenu = create_tool_bar(topLevelWindow, &dd);
  gtk_box_pack_start(GTK_BOX(hbox), oprsMenu, FALSE, FALSE, 1); /* add a oprsMenu at the left  */

  
  GtkWidget *hpaned = gtk_hpaned_new ();
  gtk_box_pack_start(GTK_BOX(hbox), hpaned, TRUE, TRUE, 1); /* add an vertical horizontal pane to the main hbox */

  textSWindow = gtk_scrolled_window_new(NULL, NULL); /* create the text  window in this hbox */
  gtk_paned_pack1 (GTK_PANED (hpaned), textSWindow, TRUE, FALSE);
  textview = gtk_text_view_new ();
  gtk_container_add (GTK_CONTAINER (textSWindow), textview);

  intDrawWin = gtk_scrolled_window_new(NULL, NULL); /* create the int graph window in this hbox */
  gtk_paned_pack2 (GTK_PANED (hpaned), intDrawWin, TRUE, FALSE);

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

  xp_create_filesel(topLevelWindow);
  xp_create_dialogs(topLevelWindow);

  gtk_widget_show_all(topLevelWindow);

  dd.window = GTK_LAYOUT(dd.canvas)->bin_window;
  idd.window = GTK_LAYOUT(idd.canvas)->bin_window;

  opCGCsp = &opCGCs;
  dd.cgcsp = opCGCsp;
  
  create_cgcs(&opCGCs, dd.window); /* this will create all the cairo context for the main loop... */

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

  inCGCsp = &inCGCs;
  idd.cgcsp = inCGCsp;
  create_cgcs(&inCGCs, idd.window); /* this will create all the cairo context for the main loop... */
   
  gtk_widget_add_events (idd.canvas, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events (idd.canvas, GDK_BUTTON_RELEASE_MASK);
  gtk_widget_add_events (idd.canvas, GDK_POINTER_MOTION_MASK);

  g_signal_connect(idd.canvas, "expose-event",
		   G_CALLBACK(ion_expose_event), NULL);
  g_signal_connect(idd.canvas, "button-press-event", 
		   G_CALLBACK(iclicked), NULL);
  g_signal_connect(idd.canvas, "button-release-event", 
		   G_CALLBACK(ireleased), NULL);
  g_signal_connect(idd.canvas, "motion-notify-event", 
		   G_CALLBACK(imotion), NULL);

  call_oprs_cat(log_file,textview);
  
  start_kernel_user_hook(name);

  run_initial_commands();

  register_main_loop(oprs,FALSE);   /* this will register the main loop as an idle func.  */
  
  register_update_active_idle(oprs);

  gtk_main();

  return 0;

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

