#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

gboolean draw_things(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  GdkGC *gc;

  gc=gdk_gc_new(GDK_DRAWABLE(widget->window));
  gdk_gc_set_line_attributes(gc, 5, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  gdk_draw_line(GDK_DRAWABLE(widget->window), gc, 300, 50, 300, 300);

  return TRUE;
}

int main(int argc, char **argv) {
  GtkWidget *window;

  gtk_init(&argc, &argv);

  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(window, 300, 150);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK(draw_things), NULL);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
