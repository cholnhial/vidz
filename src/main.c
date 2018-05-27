/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
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

#include <config.h>
#include <X11/Xlib.h>  
#include <gtk/gtk.h>
#include "vidz.h"
#include "gui.h"


int
main (int argc, char *argv[])
{
	XInitThreads();
	gtk_init(&argc, &argv);

	if(!vidz_init()) {
		g_critical("Unable to initialise vidz!\n");
		return -1;
	}

	gui_init();
	gtk_main();

	return 0;	
}

