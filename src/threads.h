#ifndef THREADS_H_
#define THREADS_H_

#include <gtk/gtk.h>

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
gpointer add_movie_thread(gpointer user_data);


gboolean add_movie_thread_show_msg_idle(gpointer user_data);
gboolean add_movie_thread_add_to_view_idle(gpointer user_data);

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
gpointer load_movie_list_thread(gpointer user_data);
#endif /* THREADS_H_ */