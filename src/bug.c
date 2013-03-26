#include <stdio.h>

#include <gtk/gtk.h>

int main(int argc, char **argv, char **envp)
{

     GtkWidget *topLevelWindow, *vbox, *hpaned, *intDrawWin, *canvas, *oprsMenu;
     
     GdkDrawable *window;

     gtk_init(&argc, &argv);


     topLevelWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     gtk_window_set_default_size(GTK_WINDOW(topLevelWindow), 400, 400);
     gtk_window_set_position(GTK_WINDOW(topLevelWindow), GTK_WIN_POS_CENTER);


     vbox = gtk_vbox_new(FALSE, 0);
     gtk_container_add(GTK_CONTAINER(topLevelWindow), vbox); 
  
     hpaned = gtk_hpaned_new();
     gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 1);

     oprsMenu = gtk_toolbar_new();
     gtk_paned_pack1(GTK_PANED(hpaned), oprsMenu, TRUE,FALSE); 

     intDrawWin = gtk_scrolled_window_new(NULL, NULL);
     gtk_paned_pack2 (GTK_PANED(hpaned), intDrawWin, TRUE, FALSE);

     canvas = gtk_layout_new(NULL,NULL);
     gtk_container_add(GTK_CONTAINER(intDrawWin),canvas);

     gtk_layout_set_size(GTK_LAYOUT(canvas), 400, 400);

     gtk_widget_show_all(topLevelWindow);

     window = GTK_LAYOUT(canvas)->bin_window;

     fprintf(stderr,"Window = %p\n",window);

     gtk_main();

     return 0;

}

