## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po pixmaps


desktop_in_files = vidz.desktop.in
desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

vidzdocdir = ${prefix}/doc/vidz
vidzdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(vidzdoc_DATA) \
	$(INTLTOOL_FILES)   \
	$(desktop_in_files) \
	$(desktop_DATA) 

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache \
	vidz.desktop


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(vidzdocdir)
