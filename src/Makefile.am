# --------------------------------------------------------
# Welcome to the gtranslator-automake-autoconf Wonderland!
# --------------------------------------------------------

SUBDIRS = toolbareditor \
	  dialogs \
	  charmap \
	  messagestable \
	  comment

INCLUDES = \
	-I$(top_srcdir)						\
	-I$(includedir)						\
	-I$(top_srcdir)/src/dialogs				\
	-I$(top_srcdir)/src/toolbareditor			\
	-I$(top_srcdir)/src/charmap				\
	-I$(top_srcdir)/src/messagestable			\
	-I$(top_srcdir)/src/comment				\
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
	-DSCRIPTSDIR=\""$(pkgdatadir)"/scripts\"	\
	-DWINDOW_ICON=\""$(datadir)"/pixmaps/gtranslator.png\"

bin_PROGRAMS = \
	gtranslator
#	gtranslator-auto-translate \
#	gtranslator-auto-learn \
#	gtranslator-export-learn-buffer

BUILT_SOURCES =                         \
        gtranslator-marshal.h                 \
        gtranslator-marshal.c

gtranslator_SOURCES = \
	gtranslator-marshal.h \
	gtranslator-marshal.c \
	application.c application.h \
	compile.h \
	draw-spaces.c draw-spaces.h \
	languages.c languages.h \
	main.c \
	message-area.c message-area.h \
	nautilus-string.c nautilus-string.h \
	notebook.c notebook.h \
	panel.c panel.h \
	tab.c tab.h \
	translator.h \
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
	actions-view.c \
	io-error-message-area.c \
	io-error-message-area.h \
	prefs-manager.c \
	prefs-manager.h \
	prefs-manager-private.h \
	actions-help.c \
	prefs-manager-app.c \
	prefs-manager-app.h \
	history-entry.c \
	history-entry.h \
	actions-search.c \
	header.h \
	header.c

gtranslator_LDADD = \
	$(top_srcdir)/src/dialogs/libdialogs.la \
	$(top_srcdir)/src/toolbareditor/libtoolbareditor.la \
	$(top_srcdir)/src/charmap/libcharmap.la \
	$(top_srcdir)/src/messagestable/libmessagestable.la \
	$(top_srcdir)/src/comment/libcomment.la \
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

gtranslator-marshal.h: gtranslator-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=gtranslator_marshal > $@

gtranslator-marshal.c: gtranslator-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --body --header --prefix=gtranslator_marshal > $@

gladedir = $(pkgdatadir)

glade_DATA =  \
       header_dialog.glade 

EXTRA_DIST = ChangeLog $(glade_DATA) gtranslator-marshal.list


CLEANFILES = $(BUILT_SOURCES)
