#include "gui.h"
#include "threads.h"
#include "vidz.h"
#include "callbacks.h"

/*globals */
static GtkBuilder* builder;
static GtkWidget* mainwindow;
static GtkIconView* iconview;
static GtkDialog* progress_dialog;
static GtkDialog* add_dialog;
static GtkEntry* add_dialog_movie_entry;
static GtkEntry* add_dialog_movie_file_entry;
static GtkListStore *icon_list_store;
static GtkEntry* movie_search_entry;
static GtkWidget* play_btn;
static GtkWidget* remove_btn;
GSList* local_movies_list = NULL;
static GtkTreeModel * filter;


/**
 *  gui_init()
 *	@Must be called first
 *
 *  Loads the user interface objects from glade file
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean gui_init()
{ 
	builder = gtk_builder_new();
	GError* err = NULL;
	if(!gtk_builder_add_from_file(builder, UI_FILE, &err)) {
		g_critical ("Couldn't load builder file: %s", err->message);
		g_error_free(err);
		return FALSE;
	}

	mainwindow = gui_get_widget (MAIN_WINDOW);

	iconview = GTK_ICON_VIEW(gui_get_widget (ICONVIEW));

	progress_dialog = GTK_DIALOG(gui_get_widget (PROGRESS_DIALOG));

	add_dialog = GTK_DIALOG(gui_get_widget (ADD_DIALOG));

	add_dialog_movie_entry = GTK_ENTRY(gui_get_widget (ADD_DIALOG_MOVIE_ENTRY));

	add_dialog_movie_file_entry = GTK_ENTRY(gui_get_widget (ADD_DIALOG_FILE_ENTRY));

	play_btn =  gui_get_widget (PLAY_BTN);

	remove_btn = gui_get_widget (REMOVE_BTN);
	movie_search_entry = gui_get_widget (MOVIE_SEARCH_ENTRY);
	
	/* Connect the signal handlers */
	gtk_builder_connect_signals(builder, NULL);

	/* Connect extra signals */
	g_signal_connect(G_OBJECT(movie_search_entry), "changed", on_main_window_entry_changed, iconview);

	/* Show main window */
	gui_show_main_window ();

	/*No movie is selected at the start */
	gui_activate_action_buttons (FALSE);
	/* Initialize the icon view */
	gui_init_icon_view();


}

/**
 *  gui_show_main_window()
 *	
 *
 *  Loads the main window using GtkBuilder
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    GtkWidget pointer to the window object
 **/
GtkWidget* gui_show_main_window()
{   
	/* Set the icon */
	gtk_window_set_icon_from_file (mainwindow, ICON_FILE, NULL);

	gtk_widget_show_all(mainwindow);
}

/**
 *  gui_hide_main_window()
 *	
 *
 *  Hides the main window
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 * */
void gui_hide_main_window()
{
	gtk_widget_hide(mainwindow);	
}

/**
 *  gui_show_progress_dialog()
 *	
 *	shows the progress dialog when a
 *  word is being added
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_show_progress_dialog()
{
	gtk_window_set_default_size(GTK_WINDOW(progress_dialog), 250, 100);
	gtk_widget_show_all(GTK_WIDGET(progress_dialog));
}
/**
 *  gui_hide_progress_dialog()
 *	
 *	
 *	 hide progress dialog, will usually
 *   be called in a thread
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_hide_progress_dialog()
{
	gtk_widget_hide(GTK_WIDGET(progress_dialog));
}

/**
 *   gui_init_icon_view()
 *	
 *	
 *	 Initializes the movies icon view
 *   and also loads the movies
 *   
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_init_icon_view()
{   

	icon_list_store = gtk_list_store_new(NUM_COLS, 
	                                     G_TYPE_STRING, GDK_TYPE_PIXBUF);

	/* init GtkIconView */

	gtk_icon_view_set_model(iconview, icon_list_store);

	gtk_icon_view_set_markup_column(iconview, COL_MOVIE_INFO);
	gtk_icon_view_set_pixbuf_column(iconview, COL_PIXBUF);
	gtk_icon_view_set_selection_mode(iconview, GTK_SELECTION_MULTIPLE);

	gtk_widget_show_all(GTK_WIDGET(iconview));
	
	/* Set up filter callback */
	 filter =  gtk_tree_model_filter_new(GTK_TREE_MODEL(icon_list_store), NULL);
	 gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER(filter),
     on_main_window_icon_view_filter, iconview, NULL);
	 gtk_icon_view_set_model (iconview, filter);

	/* Load contents from the database */
	g_thread_new("load_movie_list_thread",  load_movie_list_thread, NULL);
}

/**
 *   gui_add_movie_to_view()
 *	
 *	
 *	 Initializes the movies icon view
 *   
 *
 *  Paramaters:
 *	  A vidz_moviedata_t pointer
 *  returns:
 *    none
 **/
void gui_add_movie_to_view(vidz_moviedata_t* moviedata)
{
	GtkTreeIter iter;

	gtk_list_store_append(icon_list_store, &iter);

	/* Format the text for the view */
	GString* text = g_string_new("");
	g_string_sprintf(text, ICONVIEW_MARKUP_COLUMN, moviedata->title->str,
	                 moviedata->year->str,
	                 moviedata->rating->str);

	/* Associate the movie data pointer with iconview*/
	g_object_set_data (G_OBJECT(iconview), moviedata->title->str, moviedata);

	/* Create Pixbuf for movie cover */
	GError* gerror = NULL;
	GdkPixbuf* cover_image = gdk_pixbuf_new_from_file(moviedata->cover_image->str, &gerror);
	if(cover_image == NULL) {
		g_critical("Error loading cover image: %s\n", gerror->message);
		g_error_free(gerror);
	}
	/* Scale image */
	GdkPixbuf* scaled_cover_image = gdk_pixbuf_scale_simple (cover_image, 123, 190, GDK_INTERP_HYPER);

	/* Add to local list */
	local_movies_list = g_slist_append(local_movies_list, moviedata);

	/* Now add to the view finaly */
	gtk_list_store_set(icon_list_store, &iter, COL_MOVIE_INFO, 
	                   text->str, COL_PIXBUF, scaled_cover_image, -1);

	//g_object_unref(G_OBJECT(cover_image));
	g_string_free(text, TRUE);

}


/**
 *  gui_show_add_dialog()
 *	
 *
 *  Shows the add dialog for 
 *  adding a new movie 
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_show_add_dialog()
{
	int result = gtk_dialog_run(GTK_DIALOG(add_dialog));
	switch(result)
	{
		case(GTK_RESPONSE_OK):
		{
			gtk_widget_hide(GTK_WIDGET(add_dialog));
			break;
		}
		case(GTK_RESPONSE_DELETE_EVENT):
		{
			gtk_widget_hide(GTK_WIDGET(add_dialog));
			break;
		}
		default:
		{
			gtk_widget_hide(GTK_WIDGET(add_dialog));
		}
	}
}

/**
 *  gui_hide_add_dialog()
 *	
 *
 *  Hides the add dialog
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_hide_add_dialog()
{
	gtk_widget_hide(GTK_WIDGET(add_dialog));
}

/**
 *  gui_activate_action_buttons()
 *	   
 *   Makes the play and remove buttons 
 *   sensitive or insensitive depending
 *   on the boolean value.
 *   
 *
 *  Paramaters:
 *	  a boolean
 *  returns:
 *    none
 **/
void gui_activate_action_buttons(gboolean activate)
{
	gtk_widget_set_sensitive(remove_btn, activate);
	gtk_widget_set_sensitive(play_btn, activate);
}

/**
 *  gui_show_generic_msg_dialog()
 *	
 *	Shows a generic dialog boxes
 *	given the supplied arguments
 *  
 *  
 *
 *  Paramaters:
 *   the message type, the button type, title, and the message
 *  returns:
 *    the response 
 **/
int gui_show_generic_msg_dialog(GtkMessageType type,  GtkButtonsType buttons, gchar* title, gchar * msg)
{
	GtkWidget* dialog = gtk_message_dialog_new (gui_get_widget(MAIN_WINDOW),
	                                            GTK_DIALOG_DESTROY_WITH_PARENT,
	                                            type,
	                                            buttons,
	                                            title
	                                            );

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), msg);

	int response = gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return response;
}

/* Widget getter */
GtkWidget* gui_get_widget(gchar* widget_name)
{   
	GtkWidget* widget = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
	if(widget == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", widget, UI_FILE);
		return NULL;
	}

	return widget;
}

/**
 *  gui_remove_selected_movie()
 *	
 *	Removes selected movies from the icon view
 *	and from the database
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
void gui_remove_selected_movie()
{
	GtkIconView* iconview = GTK_ICON_VIEW(gui_get_widget (ICONVIEW));
	GSList* selected_items;
	GtkTreeIter iter;

	selected_items = gtk_icon_view_get_selected_items(iconview);

	/* Issue a warning before removing the word */
	int response = gui_show_generic_msg_dialog(
	                                           GTK_MESSAGE_WARNING,
	                                           GTK_BUTTONS_YES_NO,
	                                           "Remove Movie(s)?",
	                                           WARNING_REMOVE_MOVIE
	                                           );

	switch(response) {
		case GTK_RESPONSE_YES:
		{
			/* proceed */
			break;
		}

		case GTK_RESPONSE_NO:
		{
			return;
		}

	}


	for(GSList* i = selected_items; i != NULL; i = i->next) {
		GtkTreePath* path = (GtkTreePath*) i->data;

		if(gtk_tree_model_get_iter (icon_list_store, &iter, path)) {
			//GdkPixbuf* cover_image;
			gchar* movie_name = g_malloc(MAX_MOVIE_NAME);
			gchar* text;
			gtk_tree_model_get(icon_list_store, &iter, COL_MOVIE_INFO, &text, -1);
			parse_string(text, movie_name, "<b>", '<');

			vidz_moviedata_t* moviedata = g_object_get_data (G_OBJECT(iconview), movie_name);

			/* remove from the view */
			gtk_list_store_remove(icon_list_store, &iter);
			/* remove from database */
			if(!vidz_manager_remove_movie (moviedata->id)) {
				g_critical("Unable to remove movie from the database");
			}

			/* remove from the local list */
			local_movies_list = g_slist_remove(local_movies_list, moviedata);

			vidz_manager_free_moviedata (moviedata);
			g_free(movie_name);
			g_free(text);
		}

	}

	/* check if the list is empty */
	if(!gtk_tree_model_get_iter_first(icon_list_store, &iter)) {
		/* make some buttons insensitive until, a movie is added */
		gui_activate_action_buttons (FALSE);
	}
}

/**
 *  gui_icon_view_get_list_model_filter()
 *	
 *	Returns the tree model filter
 *	for the iconView icons.
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   a pointer to GtkTreeModelFilter
 **/
GtkTreeModelFilter* gui_icon_view_get_list_model_filter()
{   
	return filter;
}

/**
 *  gui_get_movie_search_entry()
 *	
 *	Returns the search entry pointer
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   a pointer to GtkEntry
 **/
GtkEntry* gui_get_movie_search_entry()
{
	return movie_search_entry;
}