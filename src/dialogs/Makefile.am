INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

noinst_LTLIBRARIES = \
	libdialogs.la

libdialogs_la_SOURCES = \
	file-dialogs.c \
	file-dialogs.h

gladedir = $(pkgdatadir)

glade_DATA =  \
		preferences.glade

EXTRA_DIST = \
		$(glade_DATA)
