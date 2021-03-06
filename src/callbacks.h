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

void on_main_window_icon_view_key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data);
gboolean on_main_window_icon_view_filter(GtkTreeModel *model, GtkTreeIter  *iter, gpointer data);
void on_main_window_entry_changed(GtkWidget* entry, gpointer user_data);

#endif /* CALLBACKS_H_ */