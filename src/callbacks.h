#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "gui.h"

void on_main_window_add_button_clicked(GtkWidget* button, gpointer user_data);

void on_main_window_browse_button_clicked(GtkWidget* button, gpointer user_data);

void on_add_dialog_ok_button_clicked(GtkWidget* button, gpointer user_data);

void on_main_window_remove_button_clicked(GtkWidget* button, gpointer user_data);

void on_main_window_play_button_clicked(GtkWidget* button, gpointer user_data);

void on_main_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data);

void on_main_window_about_button_clicked(GtkButton* button, gpointer user_data);

void on_main_window_quit_button_clicked(GtkButton* button, gpointer user_data);

void on_main_window_icon_view_selection_changed(GtkIconView* iconview, gpointer user_data);
#endif /* CALLBACKS_H_ */