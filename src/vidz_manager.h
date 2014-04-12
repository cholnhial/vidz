#ifndef VIDZ_MANAGER_H_
#define VIDZ_MANAGER_H_


#include "database.h"
#include "vidz_movieinfo.h"

typedef struct 
{   
	guint id;
	GString* title;
	GString* year;
	GString* rating;
	GString* genre;
	GString* movie_file;
	GString* cover_image;
} vidz_moviedata_t;



/***
 *  vidz_manager_init()
 *
 *  Initialises vidz manager 
 *  so that it can be queried for
 *  various movie information.
 *  
 *  
 *  Paramaters:
 *   A string containing the name of the datbase file
 *
 *  Returns:
 *    TRUE on success, FALSE on failure
 *
 *    
 **/
gboolean vidz_manager_init(gchar* dbname, gchar* cover_path);

/***
 *  vidz_manager_create_database()
 *
 *  Creates a database and the table.
 *  
 *  
 *  Paramaters:
 *   A string containing the name of the database file,
 *   a boolean value (TRUE to overwrite, FALSE otherwise)
 *
 *  Returns:
 *    TRUE on success, FALSE on failure
 *
 *    
 **/
gboolean vidz_manager_create_database(gchar* dbname, gboolean overwrite);

/***
 *  vidz_manager_add_movie()
 *
 *  Adds a movie to the database, and also downloads
 *  the movie metadata with cover image.
 *  
 *  Paramaters:
 *    A string containing the name of the movie.
 *   
 *
 *  Returns:
 *    Returns a valid pointer vidz_moviedata_t or NULL on failure
 *
 *    
 **/
vidz_moviedata_t* vidz_manager_add_movie(gchar* movie_name, gchar* movie_file);

/***
 *  vidz_manager_remove_movie()
 *
 *  Removes a movie from the database, along with
 *  its metadata.
 *  
 *  Paramaters:
 *   A string containing the name of the movie,
 *   A string containing the path to the movie file.
 *   
 *
 *  Returns:
 *    TRUE on success, FALSE on failure
 *
 *    
 **/
gboolean vidz_manager_remove_movie(guint id);

/***
 *  vidz_manager_remove_all_movies
 *
 *  Removes all the movies from the database, along with
 *  their metadata.
 *  
 *  Paramaters:
 *	  none
 *   
 *
 *  Returns:
 *    TRUE on success, FALSE on failure
 *
 *    
 **/
gboolean vidz_manager_remove_all_movies();

/***
 *  vidz_manager_check_movie()
 *
 *  Checks if the movie is already in the database,
 *  If so it returns TRUE, else FALSE.
 *  
 *  Paramaters:
 *   A string containing the name of the movie.
 *   
 *
 *  Returns:
 *    TRUE on success and if found, FALSE on failure
 *    
 **/
gboolean vidz_manager_check_movie(gchar* movie_name);

/***
 *  vidz_manager_get_movie_list()
 *
 *  Returns a list contianing structures of
 *  All the movies in the database.
 *  
 *  Paramaters:
 *   none
 *   
 *
 *  Returns:
 *    A valid pointer, or NULL on failure
 *    
 **/
GSList* vidz_manager_get_movie_list();

/***
 *  vidz_manager_movie_list_size()
 *
 *  Returns the number of list of movies
 *  in the database.
 *  
 *  Paramaters:
 *   none
 *   
 *
 *  Returns:
 *   an integer.
 *    
 **/
gint vidz_manager_movie_list_size();

/***
 *  vidz_manager_free_moviedata()
 *
 *  Frees memory that is previous allocated to 
 *  vidz_moviedata_t.
 *  
 *  Paramaters:
 *   none
 *   
 *
 *  Returns:
 *    none
 *    
 **/
void vidz_manager_free_moviedata(vidz_moviedata_t* moviedata_p);

/***
 *  vidz_manager_free_movie_list()
 *
 *  Free memory that was previous allocated to items
 *  in the list.
 *  
 *  
 *  Paramaters:
 *   The list pointer
 *   
 *
 *  Returns:
 *    none
 *    
 **/
void vidz_manager_free_movie_list(GSList* movie_list);

/***
 *  vidz_manager_cleanup()
 *
 *  Clears memory allocated, and
 *  cleans up the database.
 *  
 *  Paramaters:
 *   none
 *   
 *
 *  Returns:
 *    none
 *    
 **/
void vidz_manager_cleanup();

#endif /* VIDZ_MANAGER_H_ */