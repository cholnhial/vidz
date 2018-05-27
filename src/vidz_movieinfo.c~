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

#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <json/json.h>

#include "vidz_movieinfo.h"

#define DATA_BUF_SIZE 4096
#define URLTEMPLATE "http://www.omdbapi.com/?t=%s"

static size_t http_callback(char* ptr,
                            size_t count, 
                            size_t blocksize,
                            void* userdata)
{
	vidz_movieinfo_resp_t* response = userdata;
	size_t size = count * blocksize;
	memcpy(response->data + response->length, ptr, size);
	response->length += size;

	return size;
}


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
vidz_movieinfo_t* vidz_movieinfo_get_info(gchar* movie_name)
{
	/* Allocate memory for movieinfo container */

	vidz_movieinfo_t* vidz_movieinfo = g_malloc(sizeof(vidz_movieinfo_t));

	/* allocate memory for GString */
	vidz_movieinfo->title = g_string_new ("");
	vidz_movieinfo->year = g_string_new("");
	vidz_movieinfo->rating = g_string_new ("");
	vidz_movieinfo->genre = g_string_new ("");
	vidz_movieinfo->image_url = g_string_new ("");


	/* Response stuff */
	vidz_movieinfo_resp_t* resp =  g_malloc(sizeof(vidz_movieinfo_resp_t));
	if (!resp) {
		g_critical("Unable to allocate memory for HTTP response");
		vidz_movieinfo_free(vidz_movieinfo);
		return NULL;
	}

	resp->data = (char*) g_malloc(DATA_BUF_SIZE);
	if (!resp->data) {
		g_free(resp);
		g_critical("Unable to allocate memory for HTTP response");
		vidz_movieinfo_free(vidz_movieinfo);
		return NULL;
	}
	resp->length = 0;


	/* Initialize curl */
	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		g_free(resp->data);
		g_free(resp);
		vidz_movieinfo_free(vidz_movieinfo);
		g_critical("Error initializing CuRL globally");
		return NULL;
	}

	CURL *curl_hndl = curl_easy_init();
	if (!curl_hndl) {
		g_free(resp->data);
		g_free(resp);
		vidz_movieinfo_free(vidz_movieinfo);
		g_critical("Error getting CuRL handle");
		return NULL;
	}

	/* Construct complete URL for request */

	GString* url = g_string_new("");
	g_string_sprintf(url, URLTEMPLATE, 
	                 curl_easy_escape(curl_hndl, movie_name, strlen(movie_name)));

	curl_easy_setopt(curl_hndl, CURLOPT_URL, url->str);
	curl_easy_setopt(curl_hndl, CURLOPT_WRITEFUNCTION, http_callback);
	curl_easy_setopt(curl_hndl, CURLOPT_WRITEDATA, resp);
    //curl_easy_setopt(curl_hndl, TIMEOUT

	int res = curl_easy_perform(curl_hndl);
	if (res != CURLE_OK) {
		g_free(resp->data);
		g_free(resp);
		g_string_free(url, TRUE);
		vidz_movieinfo_free(vidz_movieinfo);
		curl_easy_cleanup(curl_hndl);
		g_critical("Unable to retrieve movie information");
		return NULL;
	}

	//#ifdef DEBUG
	g_printf("%s\n", resp->data);
	// #endif

	if (json_parse_resp(vidz_movieinfo, resp->data) == -1) {
		g_free(resp->data);
		g_free(resp);
		g_string_free(url, TRUE);
		vidz_movieinfo_free(vidz_movieinfo);
		curl_easy_cleanup(curl_hndl);
		g_critical("Unable to parse the movie information retreived");
		return NULL;
	}

	/* clean up */
	g_free(resp->data);
	g_free(resp);
	g_string_free(url, TRUE);
	curl_easy_cleanup(curl_hndl);

	return vidz_movieinfo;

}

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
void vidz_movieinfo_free(vidz_movieinfo_t* movieinfo_p)
{

	g_string_free(movieinfo_p->title, TRUE);
	g_string_free(movieinfo_p->year, TRUE);
	g_string_free(movieinfo_p->rating, TRUE);
	g_string_free(movieinfo_p->genre, TRUE);
	g_string_free(movieinfo_p->image_url, TRUE);

	g_free(movieinfo_p);
}

/**********************************
 *  Helper functions definitions  *
 *                                *
 *  *******************************/

static int json_parse_resp(vidz_movieinfo_t* movieinfo_p, const char* json_data)
{
	if (!movieinfo_p && !json_data) {
		return -1;
	}

	json_object* jobj;
	json_object* jobj_extract;

	jobj = json_tokener_parse(json_data);
	if (!jobj) {
		return -1;
	} 

	/** check if error, might be wrong hostname/ip **/
	jobj_extract = json_object_object_get(jobj, "Error");
	if (json_object_get_string(jobj_extract)) {
		g_printf("Error!\n");
		return -1;
	}

	/* Get movie information */
	jobj_extract = json_object_object_get(jobj, "Title");
	g_string_assign(movieinfo_p->title, json_object_get_string(jobj_extract));    
	jobj_extract = json_object_object_get(jobj, "Year");
	g_string_assign(movieinfo_p->year, json_object_get_string(jobj_extract)); 
	jobj_extract = json_object_object_get(jobj, "Rated");
	g_string_assign(movieinfo_p->rating, json_object_get_string(jobj_extract)); 
	jobj_extract = json_object_object_get(jobj, "Genre");
	g_string_assign(movieinfo_p->genre, json_object_get_string(jobj_extract)); 
	jobj_extract = json_object_object_get(jobj, "Poster");
	g_string_assign(movieinfo_p->image_url, json_object_get_string(jobj_extract)); 


	return 0;
}



/*
 * 
 int main(int argc, char** argv)
 {
	 vidz_movieinfo_t* movieinfo = vidz_movieinfo_get_info(argv[1]);
	 if(movieinfo == NULL) {
		 g_printf("Error getting movie information\n");
		 exit(EXIT_FAILURE);
		 }

		 g_printf("Title: %s\n", movieinfo->title->str);
		 g_printf("Year: %s\n", movieinfo->year->str);
		 g_printf("Rated: %s\n", movieinfo->rating->str);
		 g_printf("Genre: %s\n", movieinfo->genre->str);
		 g_printf("Cover: %s\n", movieinfo->image_url->str);



		 exit(EXIT_SUCCESS);
		 }
		 */
