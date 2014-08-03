/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * vidz.c
 * Copyright (C) 2014 Chol Nhial <chol.nhial.chol@gmail>
	 * 
 * vidz is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 * 
 * vidz is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "vidz.h"

#include <curl/curl.h>

#include <glib/gi18n.h>

/* Globals */
static gboolean first_run = FALSE;

/**
 *  vidz_init()
 *	
 *
 *  Initialises vidz_manager
 *  if the program is been run for the first time
 *  then it will create the defaults.
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if it's the first time FALSE otherwise.
 **/
gboolean vidz_init()
{
	first_run = vidz_is_first_run();
	if(first_run) {
		vidz_create_config_directory();
		vidz_manager_create_database (vidz_get_movie_db_file(), FALSE);
	}

	if(vidz_manager_init(vidz_get_movie_db_file(), vidz_get_movie_covers_directory()) == FALSE) {
		g_critical("Unable to initialise movie manager!\n");
		return FALSE;
	}


	return TRUE;

}

/**
 *  vidz_is_first_run()
 *	
 *
 *  Checks if this is the first time the
 *  program has been ran.
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if it's the first time FALSE otherwise.
 **/
gboolean vidz_is_first_run()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	if(g_file_test(conf_path,  G_FILE_TEST_IS_DIR)) {
		g_free(conf_path);
		return FALSE;
	} 

	g_free(conf_path);
	return TRUE;
}

/**
 *  vidz_create_config_directory()
 *	
 *
 *  creates the configuration directory
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if successful, FALSE otherwise
 **/
gboolean vidz_create_config_directory()
{
	int mode = 0700;
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	if(g_mkdir(conf_path, mode) == 0) {
		g_free(conf_path);
	} else {
		return FALSE;
	}

	conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, G_DIR_SEPARATOR_S, COVERS_DIR_NAME, NULL);
	if(g_mkdir(conf_path, mode) == 0) {
		g_free(conf_path);
		return TRUE;
	}

	g_free(conf_path);
	return FALSE;	
}

/**
 *  vidz_get_config_directory()
 *	
 * 	Returns a full path to the configuration
 *  directory
 *  
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vidz_get_config_directory()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	return conf_path;
}

/**
 *  vidz_get_movie_covers_directory()
 *	
 * 	Returns a full path to the movies
 *  cover director
 *  
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vidz_get_movie_covers_directory()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, G_DIR_SEPARATOR_S, COVERS_DIR_NAME, NULL);
	return conf_path;
}

/**
 *  vidz_get_movie_db_file()
 *	
 * 	Returns a full path to the vocabulary 
 *  database file 
 *  
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vidz_get_movie_db_file()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, G_DIR_SEPARATOR_S, MOVIES_DB_FILE, NULL);
	return conf_path;
}

/**
 *  vidz_play_movie()
 *	
 * 	Spawns a process that plays
 *  a movie
 *  
 * 
 *
 *  Paramaters:
 *	a string containing the name of the movie 
 *  returns:
 *    none
 **/
void vidz_play_movie(gchar* filename)
{
	GError* gerror;

	gchar* argv[2];

	argv[0] = DEFAULT_MOVIE_PLAYER;
	argv[1] = filename;
	argv[2] = NULL;

	#ifdef G_SPAWN_SEARCH_PATH_FROM_ENVP
	if(!g_spawn_async (NULL, argv, NULL,
	                   G_SPAWN_SEARCH_PATH_FROM_ENVP, NULL, NULL, NULL, &gerror)) {

		g_critical("Error spawning process: %s\n", gerror->message);
		//g_error_free(gerror);
	}
	#elif G_SPAWN_SEARCH_PATH
	if(!g_spawn_async (NULL, argv, NULL,
		G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &gerror)) {

	g_critical("Error spawning process: %s\n", gerror->message);
	//g_error_free(gerror);
	#endif
	
	
}



/***************************************
 *									   *										
 *  Utility Functions				   *
 *									   *
 * *************************************/

static size_t http_callback(char* ptr,
                            size_t count, 
                            size_t blocksize,
                            void* userdata)
{
	vidz_image_resp_t* response = userdata;
	size_t size = count * blocksize;
	memcpy(response->data + response->length, ptr, size);
	response->length += size;

	return size;
}

gboolean vidz_download_movie_cover(gchar* url, gchar* outfilename)
{

	/* Allocate some memory to hold the image */


	/* Response stuff */
	vidz_image_resp_t* resp =  g_malloc(sizeof(vidz_image_resp_t));
	if (!resp) {
		g_critical("Unable to allocate memory for HTTP response");
		return FALSE;
	}

	resp->data = (char*) g_malloc(MAX_IMAGE_SIZE);
	if (!resp->data) {
		g_free(resp);
		g_critical("Unable to allocate memory for HTTP response");
		return FALSE;
	}
	resp->length = 0;


	/* Initialize curl */
	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		g_free(resp->data);
		g_free(resp);
		g_critical("Error initializing CuRL globally");
		return FALSE;
	}

	CURL *curl_hndl = curl_easy_init();
	if (!curl_hndl) {
		g_free(resp->data);
		g_free(resp);
		g_critical("Error getting CuRL handle");
		return FALSE;
	}


	curl_easy_setopt(curl_hndl, CURLOPT_URL, url);
	curl_easy_setopt(curl_hndl, CURLOPT_WRITEFUNCTION, http_callback);
	curl_easy_setopt(curl_hndl, CURLOPT_WRITEDATA, resp);

	int res = curl_easy_perform(curl_hndl);
	if (res != CURLE_OK) {
		g_free(resp->data);
		g_free(resp);
		curl_easy_cleanup(curl_hndl);
		return FALSE;
	}

	GError* gerror = NULL;

	/* Write image data to file */
	if(!g_file_set_contents (outfilename, (gchar*) resp->data, resp->length, &gerror))
	{
		g_printf("Error saving image: %s\n", gerror->message);
		g_free(resp);
		g_error_free(gerror);
		return FALSE;
	}

	/* clean up */
	g_free(resp->data);
	g_free(resp);
	curl_easy_cleanup(curl_hndl);

	return TRUE;
}

void parse_string(const char* src, char* dst, const char* start, char end)
{
	char* ptr_start;

	/* Find the start */
	ptr_start = strstr(src, start);
	if(ptr_start == NULL)
	{
		dst = NULL;
		return;
	}

	/* Skip start string */
	ptr_start += strlen(start);

	/* Copy the string */
	while(*ptr_start && *ptr_start != end)
	{
		*dst++ = *ptr_start++;
	}
	*dst++ = '\0';
}
