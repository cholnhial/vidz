/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * vidz.h
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

#ifndef _VIDZ_H_
#define _VIDZ_H_


#include <gtk/gtk.h>

#define PROGRAM_NAME "vidz" /* executable name too*/
#define PROGRAM_NAME_DESKTOP "Vidz"
#define PROGRAM_VERSION "0.2"
#define MOVIES_DB_FILE "library.db"
#define COVERS_DIR_NAME "covers"
#define VIDZ_MOVIE_TABLE "Movies"
#define MAX_MOVIE_NAME 256
#define DEFAULT_MOVIE_PLAYER "mplayer"
#define DEFAULT_COVER_IMAGE "/usr/share/local/pixmaps/default.png"
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
gboolean vidz_init();

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
gboolean vidz_is_first_run();

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
gboolean vidz_create_config_directory();

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
gchar* vidz_get_movie_covers_directory();

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
gchar* vidz_get_movie_covers_directory();

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
gchar* vidz_get_movie_db_file();

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
void vidz_play_movie(gchar* filename);

/***************************************
 *									   *										
 *  Utility Functions				   *
 *									   *
 * *************************************/

typedef struct {
	char* data;
	unsigned int length;
} vidz_image_resp_t;

#define MAX_IMAGE_SIZE 65535

gboolean vidz_download_movie_cover(gchar* url, gchar* outfilename);

void parse_string(const char* src, char* dst, const char* start, char end);
#endif /*VIDZ_H_ */

