#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h> 
#include <sys/types.h>
#include <assert.h>

#include "database.h"

/* Not part of the interface */
static int callback_generic_get_list(void* dst, int argc,
                                     char** argv,
                                     char** azColName);

static GSList* str_list;

/**
 *  Opens the sqlite database, by defualt 
 *  the database is created if it doesn't exist
 *
 *  Paramaters:
 *      filename (string), overwrite_existing( boolean )
 *  returns:
 *      sqlite pointer, NULL on failure
 **/
sqlite3* database_open(char* dbname, int overwrite_existing)
{
	sqlite3* db;

	if(overwrite_existing) {
		remove(dbname);
	}

	sqlite3_open(dbname, &db);

	return db;
}

/**
 *  Closes the database. WARNING! after this the pointer
 *  is not usable.
 *  
 *  Paramaters: 
 *      sqlite pointer
 *  
 *  Returns:
 *      none
 **/
void database_close(sqlite3* db)
{
	assert(db != NULL);

	sqlite3_close(db);
}

/**
 *  Executes a query, WARNING! use only when the db is opened.
 *
 *  Paramaters:
 *      sqlite db pointer, query (string)
 *  
 *  Returns:
 *      on a succesful query 0 is returned, on failure -1 is returned 
 *  
 ***/
int database_execute(sqlite3* db, char* query)
{
	assert(db != NULL);

	int res = sqlite3_exec(db, query, 0, 0, 0);
	if(res != SQLITE_OK) {
		return -1;
	}

	return 0;
}

/**
 *  Uses a formated list of argurments to be inserted 
 *  into the database
 *
 *  Paramaters:
 *      sqlite db, query (string), number of items, formated list of (TYPE, DATA, COL, SIZEOF DATA)
 *
 *  Returns:
 *      -1 on falire, 0 on success
 **/
int database_generic_insert(sqlite3* db, char* query, int nitems, ...)
{
	assert(db != NULL);

	int res;
	sqlite3_stmt *stmt;
	va_list vl;
	int done = 0;

	va_start(vl, nitems*4);

	/* start the do-while loop */
	/* loop through the number of items */
	/* get the va_list */
	/* in the switch variable */
	/* case the ACTION is BLOB, INT, TEXT, do something */
	/* after having added all the data. run sqlite3_step(), and sqlite_finalize */

	do {

		res = sqlite3_prepare(db, query, -1, &stmt, 0); 
		if(res != SQLITE_OK) {
			return -1;    
		}

		for (int i = 0; i < nitems && !done; i++) {

			db_data_t db_data_type = va_arg(vl,  db_data_t);

			switch (db_data_type) {

				case TEXT:
				{
					char* text;
					int column;

					text = va_arg(vl, char*);
					column = va_arg(vl, int);

					sqlite3_bind_text(stmt, column, text, -1, SQLITE_STATIC);
					break;
				}

				case BLOB: 
				{
					void* blob_data;
					int column;
					size_t sizeof_data;

					blob_data = va_arg(vl, void*);
					column = va_arg(vl, int);
					sizeof_data = va_arg(vl, size_t);
					sqlite3_bind_blob(stmt, column, blob_data, sizeof_data, SQLITE_STATIC);

					break;
				}

				case INT:
				{
					int integer;
					int column;

					integer = va_arg(vl, int);
					column = va_arg(vl, int);

					sqlite3_bind_int(stmt,  column, integer);
					break;
				}

			}

			if(i == nitems - 1) {
				done = 1;
				va_end(vl);
			}
		}


		res = sqlite3_step(stmt);

		res = sqlite3_finalize(stmt);

	} while (res == SQLITE_SCHEMA);

}

/**
 *  Uses a formated list of argurments to be read 
 *  from the database.
 *
 *  Paramaters:
 *      sqlite db, query (string), number of items, formated list of (TYPE, DATA, COL) or for BLOB data 
	 *      (TYPE, DATA, SIZE a address of an integer, COL)
	 *
 *  Returns:
 *      -1 on falire, 0 on success
 **/
int database_generic_get(sqlite3* db, char* query, char* key, int nitems, ...)
{

	assert(db != NULL);

	int res;
	sqlite3_stmt *stmt;
	va_list vl;
	int done = 0;

	va_start(vl, nitems*3);

	do {
		res = sqlite3_prepare(db, query, -1, &stmt, 0);
		if(res != SQLITE_OK) {
			return -1;
		}

		sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);


		res = sqlite3_step(stmt);
		if(res == SQLITE_ROW) {

			for(int i = 0; i < nitems && !done; i++) {

				db_data_t db_data_type = va_arg(vl, db_data_t);

				switch(db_data_type) {

					case INT: 
					{
						int* integer;
						int column;

						integer = va_arg(vl, int*);
						column = va_arg(vl, int);

						*integer = sqlite3_column_int(stmt, column);
						break;
					}

					case BLOB:
					{
						void** blob_data;
						int* psize;
						int column;
						int sizeof_data;

						blob_data = va_arg(vl, void**);
						psize = va_arg(vl, int*);
						column = va_arg(vl, int);
						sizeof_data = sqlite3_column_bytes(stmt, column);
						*blob_data = malloc(sizeof_data);
						if(*blob_data == NULL) {
							return -1;
						}
						*psize = sizeof_data;
						memcpy(*blob_data, sqlite3_column_blob(stmt, column), sizeof_data); 

						break;
					}

					case TEXT: 
					{
						char** text;
						int column;

						text = va_arg(vl, char**);
						column = va_arg(vl, int);

						*text = strdup(sqlite3_column_text(stmt, column));
						if(*text == NULL) {
							return -1;
						}
						break;
					}
				}

				if(i == nitems - 1) {
					done = 1;
				}
			}

		}


		res = sqlite3_finalize(stmt);

	} while ( res == SQLITE_SCHEMA);

}
/**
 *  Reads a list of strings from one column
 *  from the database.
 *
 *  Paramaters:
 *      sqlite db, query (string), 
 *      
 *
 *  Returns:
 *      NULL on failure, or a valid pointer
 **/
GSList* database_generic_get_str_list(sqlite3* db, char* query)
{

	str_list = NULL;
	int res = sqlite3_exec(db, query,
	                       callback_generic_get_list,
	                       NULL,
	                       NULL);
	if(res != SQLITE_OK) {
		return NULL;
	}

	GSList* list = NULL;

	for(GSList* iter = str_list; iter != NULL; iter = iter->next) {
		list = g_slist_append(list, g_strdup((char*) iter->data));
		g_free(iter->data);
	}

	g_slist_free(str_list);

	return list;
}

/**
 *  Returns a string of the last database error 
 *  
 *  Paramaters:
 *      sqlite database pointer
 *
 *  Returns:
 *      a string
 **/
const char* database_get_last_error(sqlite3* db)
{   
	assert(db != NULL);

	return g_strdup(sqlite3_errmsg(db));
}


/* Not a part of the interface */
static int callback_generic_get_list(void* dst, int argc,
                                     char** argv,
                                     char** azColName)
{


	for (int i = 0; i < argc; i++) {
		str_list = g_slist_append(str_list, g_strdup(argv[i]));
	}

	return 0;
}


