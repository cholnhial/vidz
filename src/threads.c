#include "threads.h"
#include "gui.h"

/**
 *  add_movie_thread()
 *	
 *	Adds a new movie to the 
 *  database and has it displayed
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *   gpointer
 **/
gpointer add_movie_thread(gpointer user_data)
{   
	guint* res = g_malloc(sizeof(guint));

	gui_show_progress_dialog ();

	/* Do some error checking */
	GtkEntry* filename_entry = GTK_ENTRY(gui_get_widget (ADD_DIALOG_FILE_ENTRY));
	GtkEntry* movie_name_entry = GTK_ENTRY(gui_get_widget (ADD_DIALOG_MOVIE_ENTRY));
	gchar* movie_name = gtk_entry_get_text(movie_name_entry);
	gchar* filename =  gtk_entry_get_text(filename_entry);

	if(!g_strcmp0(movie_name, "") || !g_strcmp0(filename, "")) {
		*res = 1;
		g_idle_add_full(G_PRIORITY_HIGH_IDLE,  add_movie_thread_show_msg_idle, res, NULL);
		return NULL;
	}


	/* Check if the movie is already in the database */
	if(vidz_manager_check_movie(movie_name)) {
		*res = 2;
		g_idle_add_full(G_PRIORITY_HIGH_IDLE,  add_movie_thread_show_msg_idle, res, NULL);
		return NULL;
	}

	/* Add movie to the database */
	vidz_moviedata_t* moviedata = vidz_manager_add_movie (movie_name, filename);

	/* Do some error checking before continuing */
	if(moviedata == NULL) {
		*res = 3;
		g_idle_add_full(G_PRIORITY_HIGH_IDLE,  add_movie_thread_show_msg_idle, res, NULL);
		return NULL;
	} 


	gui_activate_action_buttons (TRUE);
	gui_hide_progress_dialog ();
	/* Add movie to view */
	g_idle_add_full(G_PRIORITY_HIGH_IDLE, add_movie_thread_add_to_view_idle, moviedata, NULL);
	g_free(res);

	return NULL;
}


gboolean add_movie_thread_show_msg_idle(gpointer user_data)
{

	guint res = *((guint*) user_data);

	if(res == 1) {
		gui_hide_progress_dialog();
		gui_show_generic_msg_dialog(GTK_MESSAGE_INFO,
		                            GTK_BUTTONS_CLOSE,
		                            "Can't add movie",
		                            ERROR_EMPTY_FIELD
		                            );
		g_free(user_data);
		return FALSE;
	}


	if(res == 2) {
		gui_hide_progress_dialog();
		gui_show_generic_msg_dialog(GTK_MESSAGE_INFO,
		                            GTK_BUTTONS_CLOSE,
		                            "Can't add movie",
		                            ERROR_ALREADY_IN_LIBRARY
		                            );
		g_free(user_data);
		return FALSE;
	}

	if(res == 3) {
		gui_hide_progress_dialog();
		gui_show_generic_msg_dialog(GTK_MESSAGE_ERROR,
		                            GTK_BUTTONS_CLOSE,
		                            "Can't add movie",
		                            ERROR_ADDING_MOVIE
		                            );

		g_free(user_data);
		return FALSE;
	}



	/* Add movie to view */
	//gui_add_movie_to_view (moviedata);

	//gui_hide_progress_dialog ();

	return FALSE;
}

gboolean add_movie_thread_add_to_view_idle(gpointer user_data)
{

	vidz_moviedata_t* moviedata = (vidz_moviedata_t*) user_data;

	gui_add_movie_to_view (moviedata);

}

/**
 *  load_movie_list_thread()
 *	
 *	Loads the movies from the database
 *  and displays them sequentially
 *  
 * 
 *  Paramaters:
 *   none
 *  returns:
 *   gpointer
 **/
gpointer load_movie_list_thread(gpointer user_data)
{   gui_show_progress_dialog();

	/* Fill the ListStore */
	GSList* movie_list =   vidz_manager_get_movie_list ();
	if(movie_list == NULL) {
		g_critical("Error getting movie list, maybe it's empty");
		/* make delete button insensitive until, a movie is added */
		gui_activate_action_buttons (FALSE);
		gui_hide_progress_dialog();
		return NULL;
	}


	for(GSList* iter = movie_list; iter; iter = iter->next) {
		g_idle_add_full(G_PRIORITY_HIGH_IDLE, add_movie_thread_add_to_view_idle, iter->data, NULL);
	}

	gui_hide_progress_dialog();
}
