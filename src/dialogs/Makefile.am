INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
	-DDATADIR=\""$(pkgdatadir)"\"				\
	-DPIXMAPSDIR=\""$(datadir)"/pixmaps/gtranslator\"	\
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libdialogs.la

libdialogs_la_SOURCES = \
	file-dialogs.c \
	file-dialogs.h \
	preferences-dialog.c \
	preferences-dialog.h

gladedir = $(pkgdatadir)

glade_DATA =  \
		preferences-dialog.glade \
		search-dialog.glade

EXTRA_DIST = \
		$(glade_DATA)
