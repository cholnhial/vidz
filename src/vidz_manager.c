#include "vidz_manager.h"

#include "vidz_movieinfo.h"
#include "vidz.h"

/********************************
 *							    *
 *  Local private data members  *
 * ******************************/

static gboolean initialised = FALSE;
static sqlite3* db = NULL;
static GMutex db_access_mutex;
static gchar* movies_cover_path;

/* Functions Not apart of the interface */
static int is_initialised();
static void wait_for_db_access();
static void  lock_db_access();
static void  unlock_db_access();


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
gboolean vidz_manager_init(gchar* dbname, gchar* cover_path)
{
	db = database_open(dbname, FALSE);
	if(db == NULL) {
		return FALSE;
	}

	movies_cover_path = cover_path;
	g_mutex_init(&db_access_mutex);

	initialised = TRUE;

	return TRUE;
}

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
gboolean vidz_manager_create_database(gchar* dbname, gboolean overwrite)
{
	if(overwrite == TRUE) {
		db = database_open(dbname, TRUE);
		if(db == NULL) {
			return FALSE;
		}
	} 
	else {
		db = database_open(dbname, FALSE);
		if(db == NULL) {
			return FALSE;
		}
	}

	char* query = "CREATE TABLE " VIDZ_MOVIE_TABLE "(title TEXT PRIMARY KEY,year TEXT, rating TEXT,genre TEXT,movie_file TEXT,cover_image TEXT)";
	if(database_execute(db, query) == -1) {
		g_printf("ERROR: %s\n", database_get_last_error (db));
		return FALSE;
	}

	return TRUE;
}

/***
 *  vidz_manager_add_movie()
 *
 *  Adds a movie to the database, and also downloads
 *  the movie metadata with cover image.
 *  
 *  Paramaters:
 *   A string containing the name of the movie.
 *   
 *
 *  Returns:
 *    Returns a valid pointer vidz_moviedata_t or NULL on failure
 *
 *    
 **/
vidz_moviedata_t* vidz_manager_add_movie(gchar* movie_name, gchar* movie_file)
{   
	if (!is_initialised()) {
		return NULL;
	}

	/* First meta data for this movie */
	vidz_moviedata_t* moviedata = g_malloc(sizeof(vidz_moviedata_t));
	moviedata->title = g_string_new("");
	moviedata->year = g_string_new("");
	moviedata->rating = g_string_new("");
	moviedata->genre = g_string_new("");
	moviedata->cover_image = g_string_new("");
	moviedata->movie_file = g_string_new("");

	/* Get information from the internet */
	vidz_movieinfo_t* movieinfo = vidz_movieinfo_get_info (movie_name);
	if(!movieinfo) {
		vidz_manager_free_moviedata(moviedata);
		return NULL;
	}

	/* Set ID */
	int id = vidz_manager_movie_list_size ();
	moviedata->id = id + 1;

	/* Download cover image or leave default if no cover */

	/* Construct the image filename */
	if(!g_strcmp0 (movieinfo->image_url->str, "N/A")) {
		g_string_sprintf(moviedata->cover_image, "%s", DEFAULT_COVER_IMAGE); 
	} else {
		g_string_sprintf(moviedata->cover_image, "%s/%s.jpg", movies_cover_path, movieinfo->title->str); 
		if(!vidz_download_movie_cover(movieinfo->image_url->str, moviedata->cover_image->str)) {
			vidz_movieinfo_free (movieinfo);
			vidz_manager_free_moviedata(moviedata);
			return NULL;
		}
	}


	/* Prep to write the contents to the database */

	g_string_assign(moviedata->title, movieinfo->title->str);
	g_string_assign(moviedata->year, movieinfo->year->str);
	g_string_assign(moviedata->rating, movieinfo->rating->str);
	g_string_assign(moviedata->genre, movieinfo->genre->str);
	g_string_assign(moviedata->movie_file, movie_file);


	/* Begin writing to database */

	wait_for_db_access();
	lock_db_access();

	GString* query = g_string_new("");

	g_string_sprintf(query, "INSERT INTO %s(title, year, rating, genre, movie_file, cover_image) VALUES('%s', '%s', '%s', '%s', '%s', '%s')",
	                 VIDZ_MOVIE_TABLE,
	                 moviedata->title->str,
	                 moviedata->year->str,
	                 moviedata->rating->str,
	                 moviedata->genre->str,
	                 moviedata->movie_file->str,
	                 moviedata->cover_image->str
	                 );
	/* execute the query */
	if(database_execute(db, query->str) == -1) {
		unlock_db_access();
		vidz_movieinfo_free (movieinfo);
		vidz_manager_free_moviedata(moviedata);
		g_string_free(query, TRUE);
		return NULL;
	}

	g_string_free(query, TRUE);

	/* Cleanup */
	vidz_movieinfo_free (movieinfo);

	unlock_db_access();

	return moviedata;
}


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
gboolean vidz_manager_remove_movie(guint id)
{
	if (!is_initialised()) {
		return FALSE;
	}

	wait_for_db_access();
	lock_db_access();

	GString* query = g_string_new("");

	g_string_sprintf(query, "DELETE FROM %s WHERE oid = '%d'", VIDZ_MOVIE_TABLE, id);
	/* execute the query */
	if(database_execute(db, query->str) == -1) {
		unlock_db_access();
		g_printf("%s\n", database_get_last_error (db));
		g_string_free(query, TRUE);
		return FALSE;
	}

	g_string_free(query, TRUE);
	unlock_db_access();

	return TRUE;
}

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
gboolean vidz_manager_remove_all_movies()
{
	g_printf("Here");
	return TRUE;
}

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
gboolean vidz_manager_check_movie(gchar* movie_name)
{
	if (!is_initialised()) {
		return FALSE;
	}

	wait_for_db_access();
	lock_db_access();
	int oid = -1;
	GString* query = g_string_new("");
	g_string_sprintf(query, "SELECT oid FROM %s WHERE title='%s'", VIDZ_MOVIE_TABLE, movie_name);
	/* execute the query */
	if(database_generic_get(db, query->str, movie_name, 1, INT, &oid, 0) == -1) {
		unlock_db_access();
		g_string_free(query, TRUE);
		return FALSE;
	}


	/* checks if it's a valid movie in the database*/
	if(oid > -1) {
		unlock_db_access();
		g_string_free(query, TRUE);
		return TRUE;
	}

	g_string_free(query, TRUE);
	unlock_db_access();

	return FALSE;
}

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
GSList* vidz_manager_get_movie_list()
{   
	if (!is_initialised()) {
		return FALSE;
	}

	wait_for_db_access();
	lock_db_access();

	GSList* movies_list = NULL;

	vidz_moviedata_t* moviedata = NULL;

	/* Unlock db access mutex, to prevent deadlock */
	unlock_db_access ();

	gint nmovies = vidz_manager_movie_list_size();

	/* It's okay to lock it now */
	wait_for_db_access();
	lock_db_access();

	if(nmovies == -1) {
		unlock_db_access();
		return NULL;
	}

	GString* query = g_string_new("");
	int res;

	for(int i = 1; i <= nmovies; i++) {
		moviedata = g_malloc(sizeof(vidz_moviedata_t));
		moviedata->title = g_string_new("");
		moviedata->year = g_string_new("");
		moviedata->rating = g_string_new("");
		moviedata->genre = g_string_new("");
		moviedata->movie_file = g_string_new("");
		moviedata->cover_image = g_string_new("");

		g_free(moviedata->title->str);
		g_free(moviedata->year->str);
		g_free(moviedata->rating->str);
		g_free(moviedata->genre->str);
		g_free(moviedata->movie_file->str);
		g_free(moviedata->cover_image->str);

		/* Construct query */
		g_string_sprintf(query, "SELECT * FROM %s WHERE oid='%d'", VIDZ_MOVIE_TABLE, i);

		/* Execute the query to get content */
		res = database_generic_get (db, query->str, NULL, 6, TEXT, &moviedata->title->str, 0,
		                            TEXT, &moviedata->year->str, 1,
		                            TEXT, &moviedata->rating->str, 2,
		                            TEXT, &moviedata->genre->str, 3,
		                            TEXT, &moviedata->movie_file->str, 4,
		                            TEXT, &moviedata->cover_image->str, 5);
		if(res == -1) {
			/* Handle error here */

			/* Clean up and return NULL */
		}

		moviedata->id = i;

		movies_list = g_slist_append(movies_list, moviedata);


	}

	unlock_db_access();

	//g_string_free(query, TRUE);

	return movies_list;
}

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
 *   an integer, -1 on failure.
 *    
 **/
gint vidz_manager_movie_list_size()
{
	if (!is_initialised()) {
		return FALSE;
	}
	wait_for_db_access();
	lock_db_access();
	char* query =  "SELECT count(*) FROM " VIDZ_MOVIE_TABLE;

	guint table_size;
	int res;
	res = database_generic_get(db, query, NULL, 1, INT, &table_size, 0);
	if(res == -1) {
		unlock_db_access();
		return -1;
	}

	unlock_db_access();


	return table_size;
}

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
void vidz_manager_free_moviedata(vidz_moviedata_t* moviedata_p)
{
	g_string_free(moviedata_p->title, TRUE);
	g_string_free(moviedata_p->year, TRUE);
	g_string_free(moviedata_p->rating, TRUE);
	g_string_free(moviedata_p->genre, TRUE);
	g_string_free(moviedata_p->cover_image, TRUE);
	g_string_free(moviedata_p->movie_file, TRUE);

	g_free(moviedata_p);
}

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
void vidz_manager_free_movie_list(GSList* movie_list)
{
	for(GSList* iter = movie_list; iter; iter = iter->next) {
		vidz_moviedata_t* moviedata = iter->data;
		vidz_manager_free_moviedata(moviedata);

	}
}


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
void vidz_manager_cleanup()
{
	if(!is_initialised()) {
		sqlite3_close(db);
		initialised = FALSE;
		return;
	}
	wait_for_db_access();
	lock_db_access();
	sqlite3_close(db);
	g_mutex_clear(&db_access_mutex);
	initialised = FALSE;
}


/* Not apart of the interface */
int is_initialised()
{
	return initialised;
}

void  lock_db_access() {

	g_mutex_lock(&db_access_mutex);
}

void  unlock_db_access() {

	g_mutex_unlock(&db_access_mutex);
}

void wait_for_db_access() {
	/* Wait until the mutex is unlocked */
	while(!g_mutex_trylock(&db_access_mutex));
	g_mutex_unlock(&db_access_mutex);
}
