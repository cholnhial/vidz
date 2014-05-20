#include "callbacks.h"
#include "threads.h"
#include "vidz_manager.h"
#include "vidz.h"

extern GSList* local_movies_list;
static movie_selected = FALSE;

void on_main_window_add_button_clicked(GtkWidget* button, gpointer user_data)
{
	/* Show the add dialog */

	gui_show_add_dialog ();


}

void on_add_dialog_ok_button_clicked(GtkWidget* button, gpointer user_data)
{ 
	gtk_dialog_response (gui_get_widget (ADD_DIALOG), GTK_RESPONSE_OK);

	g_thread_new("add_movie_thread",  add_movie_thread, NULL);
}

void on_main_window_browse_button_clicked(GtkWidget* button, gpointer user_data)
{

	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Choose Movie File",
	                                      GTK_WINDOW(gui_get_widget (MAIN_WINDOW)),
	                                      GTK_FILE_CHOOSER_ACTION_OPEN,
	                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                                      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GtkEntry* file_entry = GTK_ENTRY(gui_get_widget (ADD_DIALOG_FILE_ENTRY));
		gtk_entry_set_text (file_entry, filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void on_main_window_remove_button_clicked(GtkWidget* button, gpointer user_data)
{

	/* Remove the movie from the the icon view, and also
	 * from the database */
	gui_remove_selected_movie ();

}

void on_main_window_play_button_clicked(GtkWidget* button, gpointer user_data)
{
	GtkIconView* iconview = GTK_ICON_VIEW(gui_get_widget (ICONVIEW));
	GSList* selected_items;
	GtkTreeModel* icon_list_store;
	GtkTreeIter iter;


	icon_list_store = gtk_icon_view_get_model (iconview);
	selected_items = gtk_icon_view_get_selected_items(iconview);

	for(GSList* i = selected_items; i != NULL; i = i->next) {
		GtkTreePath* path = (GtkTreePath*) i->data;
		if(gtk_tree_model_get_iter (icon_list_store, &iter, path)) {
			gchar* movie_name = g_malloc(MAX_MOVIE_NAME);
			gchar* text;
			gtk_tree_model_get(icon_list_store, &iter, COL_MOVIE_INFO, &text, -1);
			parse_string(text, movie_name, "<b>", '<');

			vidz_moviedata_t* moviedata = g_object_get_data (G_OBJECT(iconview), movie_name);
			/* Play the movie */
			vidz_play_movie (moviedata->movie_file->str);
			g_free(movie_name);
			g_free(text);
		}

	}

}

void on_main_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{	
	gui_hide_main_window ();

	/* clean up */
	vidz_manager_free_movie_list (local_movies_list);
	vidz_manager_cleanup ();

	gtk_main_quit();
}

void on_main_window_quit_button_clicked(GtkButton* button, gpointer user_data)
{
	gui_hide_main_window ();

	/* clean up */
	vidz_manager_free_movie_list (local_movies_list);
	vidz_manager_cleanup ();

	gtk_main_quit();	
}

void on_main_window_about_button_clicked(GtkButton* button, gpointer user_data)
{
	const gchar* authors[] = {"Developers:",
		" Chol Nhial <chol.nhial.chol@gmail.com>",
		"", NULL};

	GtkWidget* about_dialog = gtk_about_dialog_new();

	/* Make it a child of the main window */
	gtk_window_set_transient_for(GTK_WINDOW(about_dialog), GTK_WINDOW(gui_get_widget(MAIN_WINDOW)));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(about_dialog), TRUE); 

	/* Make the dialog modal */
	gtk_window_set_modal(GTK_WINDOW(about_dialog), TRUE);

	/* @Note for testing purposes, the logo is set from a file location,
	 it will later be changed.*/
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), "vidz");

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PROGRAM_NAME_DESKTOP);
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), authors);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PROGRAM_VERSION);

	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about_dialog),
	                             "This program is free software; you can redistribute it\n"
	                             "and/or modify it under the terms of the GNU General Public\n"
	                             "License as published by the Free Software Foundation; either\n"
	                             "version 2 of the License, or (at your option) any later\n"
	                             "version.\n"
	                             "\n"
	                             "This program is distributed in the hope that it will be\n"
	                             "useful, but WITHOUT ANY WARRANTY; without even the implied\n"
	                             "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n"
	                             "PURPOSE.  See the GNU General Public License for more details."
	                             );

	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), PROGRAM_COMMENT);

	gtk_widget_show_all(GTK_WIDGET(about_dialog));

	int result = gtk_dialog_run(GTK_DIALOG(about_dialog));
	switch(result)
	{
		case(GTK_RESPONSE_OK):
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));

			break;
		}
		case(GTK_RESPONSE_DELETE_EVENT):
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));
			break;
		}
		default:
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));
		}
	}

	gtk_widget_destroy(about_dialog);
}

void on_main_window_icon_view_selection_changed(GtkIconView* iconview, gpointer user_data)
{
	/* Check if there's anything selected, if so activate
	 * the action button*/
	GSList* selected_items = NULL;
	selected_items = gtk_icon_view_get_selected_items(iconview);
	if(g_slist_length (selected_items) == 0) {
		gui_activate_action_buttons (FALSE);
		movie_selected = FALSE;
	} else {
		gui_activate_action_buttons (TRUE);
		movie_selected = TRUE;
	}
}

void on_main_window_icon_view_key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
	// First check if a movie is selected
	if(movie_selected) {
		if(event->keyval == GDK_KEY_Delete) {
			gui_remove_selected_movie ();
		}
	}

}