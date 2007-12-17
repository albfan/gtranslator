INCLUDES =                                                      \
        -I$(top_srcdir)                                         \
        -I$(top_builddir)                                       \
        -I$(top_srcdir)/src                                     \
        -I$(top_builddir)/src                                   \
	-DDATADIR=\""$(pkgdatadir)"\"                           \
        $(GTRANSLATOR_CFLAGS)                                   \
        $(WARN_CFLAGS)                                          \
        $(DISABLE_DEPRECATED_CFLAGS)

gladedir = $(pkgdatadir)

glade_DATA =  \
		comment-dialog.glade

EXTRA_DIST = \
		$(glade_DATA)

## File created by the gnome-build tools

