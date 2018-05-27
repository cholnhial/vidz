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

#ifndef VIDZ_MOVIEINFO_H_
#define VIDZ_MOVIEINFO_H_

#include <glib.h>

typedef struct
{
	GString* title;
	GString* year;
	GString* rating;
	GString* genre;
	GString* image_url;
} vidz_movieinfo_t;

typedef struct {
	char* data;
	unsigned int length;
} vidz_movieinfo_resp_t;

/***
 *  vidz_movieinfo_get_info
 *
 *  Returns a pointer with all movie
 *  information Title, Year, Rating ...etc
 *  
 *  
 *  Paramaters:
 *   A string containing the name of the movie
 *
 *  Returns:
 *    NULL on failure, A pointer filled with
 *    all the goodies.
 *    
 **/
vidz_movieinfo_t* vidz_movieinfo_get_info(gchar* movie_name);

/***
 *  vidz_movieinfo_free
 *
 *  Free's memory that was previously allocated by 
 *  vidz_get_movie_info().
	 *  
 *  Paramaters:
 *    A valid vidz_movieinfo_t pointer.
 *
 *  Returns:
 *    none
 *   
 *    
 **/
void vidz_movieinfo_free(vidz_movieinfo_t* movieinfo_p);

/***********************************
 * Helper functions, not part of   *
 * the interface                   *
 * *********************************/
static int json_parse_resp(vidz_movieinfo_t* movieinfo_p, const char* json_data);

#endif /* VIDZ_MOVIEINFO_H_ */
