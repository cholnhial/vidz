## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = vidz.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(VIDZ_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -std=gnu99 \
	 -lX11\
	 -O2
	

bin_PROGRAMS = vidz

vidz_SOURCES = \
	main.c \
	vidz.h \
	vidz.c \
	database.c \
	database.h \
	vidz_movieinfo.c \
	vidz_movieinfo.h \
	vidz_manager.h \
	vidz_manager.c \
	gui.c \
	gui.h \
	callbacks.c \
	callbacks.h \
	threads.c \
	threads.h	

vidz_LDFLAGS = \
	-Wl,--export-dynamic

vidz_LDADD = $(VIDZ_LIBS)


EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

