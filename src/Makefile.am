# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = toolbareditor \
	  dialogs \
	  charmap 

INCLUDES = \
	-I$(top_srcdir)						\
	-I$(includedir)						\
	-I$(top_srcdir)/src/dialogs				\
	-I$(top_srcdir)/src/toolbareditor			\
	-I$(top_srcdir)/src/charmap				\
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"	\
	-DPREFIX=\""$(prefix)"\"                                \
	-DSYSCONFDIR=\""$(sysconfdir)"\"                        \
	-DDATADIR=\""$(pkgdatadir)"\"                              \
	-DLIBDIR=\""$(libdir)"\"                                \
	$(GTRANSLATOR_CFLAGS)                                   \
	$(GTKSPELL_CFLAGS)                                      \
	$(SPELLCHECK_CFLAGS)					\
	$(DEPRECATED_FLAGS)                                     \
	-DG_LOG_DOMAIN=\""$(PACKAGE)"\"                         \
	-DSCHEMESDIR=\""$(pkgdatadir)"/colorschemes\"	\
	-DSCRIPTSDIR=\""$(pkgdatadir)"/scripts\"	\
	-DWINDOW_ICON=\""$(datadir)"/pixmaps/gtranslator.png\"

bin_PROGRAMS = \
	gtranslator
#	gtranslator-auto-translate \
#	gtranslator-auto-learn \
#	gtranslator-export-learn-buffer

gtranslator_SOURCES = \
	application.c application.h \
	compile.h \
	dnd.h \
	draw-spaces.c draw-spaces.h \
	find.h \
	header_stuff.h \
	languages.c languages.h \
	main.c \
	message-area.c message-area.h \
	nautilus-string.c nautilus-string.h \
	notebook.c notebook.h \
	panel.c panel.h \
	runtime-config.c runtime-config.h \
	tab.c tab.h \
	translator.h \
	vfs-handle.h \
	view.c view.h \
	window.c window.h \
	po.c \
	po.h \
	translator.c \
	utils_gui.c \
	utils_gui.h \
	utils.c \
	utils.h \
	msg.c \
	msg.h \
	actions-file.c \
	actions.h \
	actions-go.c \
	actions-edit.c \
	save.c \
	save.h \
	comment-panel.c \
	comment-panel.h \
	io-error-message-area.c \
	io-error-message-area.h \
	prefs-manager.c \
	prefs-manager.h \
	prefs-manager-private.h \
	actions-help.c \
	prefs-manager-app.c \
	prefs-manager-app.h

gtranslator_LDADD = \
	$(top_srcdir)/src/dialogs/libdialogs.la \
	$(top_srcdir)/src/toolbareditor/libtoolbareditor.la \
	$(top_srcdir)/src/charmap/libcharmap.la \
	$(INTLLIBS) \
	$(GTRANSLATOR_LIBS) \
	$(GTKSPELL_LIBS)
	
gtranslator_LDFLAGS = \
	-Wall

#gtranslator_auto_learn_SOURCES = \
#	auto-learn.c
#	
#gtranslator_auto_translate_SOURCES = \
#	auto-translate.c
#	
#gtranslator_export_learn_buffer_SOURCES = \
#	export-learn-buffer.c
#	

gladedir = $(pkgdatadir)

glade_DATA =  \
       header_dialog.glade 

EXTRA_DIST = ChangeLog $(glade_DATA)


