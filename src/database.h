#ifndef DATABASE_H_
#define DATABASE_H_
#include <sqlite3.h>
#include <string.h>
#include <glib.h>

typedef enum {TEXT = 1, BLOB, INT} db_data_t;

/**
 *  Opens the sqlite database, by defualt 
 *  the database is created if it doesn't exist
 *
 *  Paramaters:
 *      filename (string), overwrite_existing( boolean )
 *  returns:
 *      sqlite pointer, NULL on failure
 **/
 sqlite3* database_open(char* dbname, int overwrite_existing);

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
void database_close(sqlite3* db);

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
int database_execute(sqlite3* db, char* query);

/**
 *  Uses a formated list of argurments to be inserted 
 *  into the database
 *
 *  Paramaters:
 *      sqlite db, query (string), number of items, formated list of (TYPE, DATA, COL)
 *
 *  Returns:
 *      -1 on falire, 0 on success
 **/
int database_generic_insert(sqlite3* db, char* query, int nitems, ...);

/**
 *  Uses a formated list of argurments to be read 
 *  from the database.
 *
 *  Paramaters:
 *      sqlite db, query (string), number of items, formated list of (TYPE, DATA, COL)
 *
 *  Returns:
 *      -1 on falire, 0 on success
 **/
int database_generic_get(sqlite3* db, char* query, char* key, int nitems, ...);

/**
 *  Reads a list of strings from one column
 *  from the database.
 *
 *  Paramaters:
 *      sqlite db, query (string), 
 *      
 *
 *  Returns:
 *      -1 on failure, 0 on success
 **/
GSList* database_generic_get_str_list(sqlite3* db, char* query);

/**
 *  Returns a string of the last database error 
 *  
 *  Paramaters:
 *      sqlite database pointer
 *
 *  Returns:
 *      a string
 **/
const char* database_get_last_error(sqlite3* db);

#endif
