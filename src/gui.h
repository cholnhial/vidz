#ifndef GUI_H_
#define GUI_H_

#include <gtk/gtk.h>

#include "vidz_manager.h"

#define UI_FILE "/usr/local/share/vidz/ui/vidz.ui"

#define MAIN_WINDOW "mainwindow"
#define ICONVIEW "iconview"
#define PROGRESS_DIALOG "progress_dialog"
#define ADD_DIALOG_MOVIE_ENTRY "add_dialog_movie_name_entry"
#define ADD_DIALOG_FILE_ENTRY "add_dialog_file_name_entry"
#define ADD_DIALOG "add_dialog"
#define REMOVE_BTN "remove_btn"
#define PLAY_BTN "play_btn"

#define ICONVIEW_MARKUP_COLUMN "<b>%s</b>\n<i>Year: %s</i>\n<i>Rated: %s</i>"

#define ERROR_EMPTY_FIELD "Please make sure both input fields are filled"
#define ERROR_ADDING_MOVIE "Unable to add the movie specified.\nCheck your internet connection and\n make sure the name is spelled correctly"
#define ERROR_ALREADY_IN_LIBRARY "The movie your are trying to add is already in the library"
#define WARNING_REMOVE_MOVIE "Are you sure you want to remove the selected movie(s)?"

#define PROGRAM_COMMENT "A simple, but yet powerful movie library"
#define ICON_FILE "/usr/local/share/pixmaps/vidz.png"
enum
{
	COL_MOVIE_INFO,
	COL_PIXBUF,
	NUM_COLS
};

/**
 *  gui_init()
 *	@Must be called first
 *
 *  Loads the user interface objects from glade file
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean gui_init();

/**
 *  gui_show_main_window()
 *	
 *
 *  Shows the main window
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    GtkWidget pointer to the window object
 **/
GtkWidget* gui_show_main_window();

/**
 *  gui_hide_main_window()
 *	
 *
 *  Hides the main window
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 * */
void gui_close_main_window();

/**
 *  gui_show_progress_dialog()
 *	
 *	shows the progress dialog when a
 *  word is being added
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_show_progress_dialog();

/**
 *  gui_hide_progress_dialog()
 *	
 *	
 *	 hide progress dialog, will usually
 *   be called in a thread
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_hide_progress_dialog();

/**
 *   gui_init_icon_view()
 *	
 *	
 *	 Initializes the movies icon view
 *   
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_init_icon_view();


/**
 *  gui_show_add_dialog()
 *	
 *
 *  Shows the add dialog for 
 *  adding a new movie 
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_show_add_dialog();

/**
 *  gui_hide_add_dialog()
 *	
 *
 *  Hides the add dialog
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_hide_add_dialog();

/**
 *   gui_add_movie_to_view()
 *	
 *	
 *	 Initializes the movies icon view
 *   
 *
 *  Paramaters:
 *	  A vidz_moviedata_t pointer
 *  returns:
 *    none
 **/
void gui_add_movie_to_view(vidz_moviedata_t* moviedata);


/**
 *  gui_activate_action_buttons()
 *	   
 *   Makes the play and remove buttons 
 *   sensitive or insensitive depending
 *   on the boolean value.
 *   
 *
 *  Paramaters:
 *	  a boolean
 *  returns:
 *    none
 **/
void gui_activate_action_buttons(gboolean activate);

/**
 *  gui_show_generic_msg_dialog()
 *	
 *	Shows a generic dialog boxes
 *	given the supplied arguments
 *  
 *  
 *
 *  Paramaters:
 *   the message type, the button type, title, and the message
 *  returns:
 *    the response 
 **/
int gui_show_generic_msg_dialog(GtkMessageType type,  GtkButtonsType buttons, gchar* title, gchar * msg);
GtkWidget* gui_get_widget(gchar* widget_name);

/**
 *  gui_remove_selected_movie()
 *	
 *	Removes selected movies from the icon view
 *	and from the database
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
void gui_remove_selected_movie();

#endif /* GUI_H_ */