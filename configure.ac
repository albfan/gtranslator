dnl -------------------------------------------------------------------
dnl Process this file with autoconf to produce a configure script.
dnl -------------------------------------------------------------------
dnl (C) 2000-2004 Fatih Demir <kabalak@kabalak.net>
dnl -------------------------------------------------------------------
AC_PREREQ(2.52)
AC_INIT(gtranslator, 1.9.2,
	http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator)

AC_CONFIG_SRCDIR(src/main.c)
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_MAINTAINER_MODE

AC_PROG_INSTALL

dnl -------------------------------------------------------------------
dnl Use intltools.
dnl -------------------------------------------------------------------
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=gtranslator
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE(GETTEXT_PACKAGE,[gtranslator],[Package name for use by gettext library])
AM_GLIB_GNU_GETTEXT
AC_CHECK_FUNC(bind_textdomain_codeset,,[AC_CHECK_LIB(intl,bind_textdomain_codeset)])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])

dnl -------------------------------------------------------------------
dnl Standard bla-bla checks.
dnl -------------------------------------------------------------------
AC_PROG_CC
AC_ISC_POSIX
AM_PROG_LIBTOOL
AC_TYPE_SIGNAL

dnl -------------------------------------------------------------------
dnl The standard checks for the compiler.
dnl -------------------------------------------------------------------
AC_HEADER_STDC

dnl -------------------------------------------------------------------
dnl The standard extended GNOME compile warning options ...
dnl -------------------------------------------------------------------
GNOME_COMPILE_WARNINGS(maximum)

AC_ARG_ENABLE(deprecations,
              [AC_HELP_STRING([--enable-deprecations],
                              [warn about deprecated usages [default=no]])],,
              [enable_deprecations=no])

if test "x$enable_deprecations" = "xyes"; then
   DISABLE_DEPRECATED_CFLAGS="\
-DG_DISABLE_DEPRECATED \
-DGDK_DISABLE_DEPRECATED \
-DGTK_DISABLE_DEPRECATED \
-DGDK_PIXBUF_DISABLE_DEPRECATED \
-DGNOME_DISABLE_DEPRECATED"
   AC_SUBST(DISABLE_DEPRECATED_CFLAGS)
fi

dnl ==========================================================================
dnl
dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!
dnl
dnl ==========================================================================

GTK_REQUIRED=2.11.6
LIBGNOME_REQUIRED=1.105.0
LIBGNOMEUI_REQUIRED=1.105.0
GNOME_VFS_REQUIRED=1.9.4
LIBXML_REQUIRED=2.4.12
LIBGLADE_REQUIRED=2.6.0
SOURCEVIEW_REQUIRED=1.8.0
LIBGUCHARMAP_REQUIRED=1.6.0
GDICT_REQUIRED=0.10.8
GTKSPELL_OPTIONAL=2.0.2

AC_SUBST(GTK_REQUIRED)
AC_SUBST(LIBGNOME_REQUIRED)
AC_SUBST(LIBGNOMEUI_REQUIRED)
AC_SUBST(GNOME_BUILD_REQUIRED)
AC_SUBST(GNOME_VFS_REQUIRED)
AC_SUBST(LIBXML_REQUIRED)
AC_SUBST(LIBGLADE_REQUIRED)
AC_SUBST(SOURCEVIEW_REQUIRED)
AC_SUBST(LIBGUCHARMAP_REQUIRED)
AC_SUBST(GDICT_REQUIRED)
AC_SUBST(GTKSPELL_OPTIONAL)

PKG_CHECK_MODULES(GTRANSLATOR, [
libgnome-2.0 >= $LIBGNOME_REQUIRED 
libgnomeui-2.0 >= $LIBGNOMEUI_REQUIRED 
gnome-vfs-2.0 >= $GNOME_VFS_REQUIRED 
libxml-2.0 >= $LIBXML_REQUIRED 
libglade-2.0 >= $LIBGLADE_REQUIRED
gtksourceview-1.0 >= $SOURCEVIEW_REQUIRED
gucharmap >= $LIBGUCHARMAP_REQUIRED
gdict-1.0 >= $GDICT_REQUIRED
])

AC_SUBST(GTRANSLATOR_CFLAGS)
AC_SUBST(GTRANSLATOR_LIBS)

GLIB_GENMARSHAL=`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`
AC_SUBST(GLIB_GENMARSHAL)

GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

dnl -------------------------------------------------------------------
dnl Whether we use the broken, unsupported gtk+ functions
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(gtk_enable_broken, [  --enable-gtk-broken     Use broken GTK functions ],[
	gtk_enable_broken=yes
	AC_DEFINE(GTK_ENABLE_BROKEN, [yes], [Define this to use broken GTK functions])
])

dnl -------------------------------------------------------------------
dnl Check for Scrollkeeper - it must be present now...
dnl -------------------------------------------------------------------
AC_CHECK_PROG(sc_present, scrollkeeper-config, yes, no)
if test "z$sc_present" = "zyes" ; then
	AC_MSG_CHECKING([for scrollkeeper])
	_VERSION=`scrollkeeper-config --version|sed -e 's/\.//g'`
	if test $_VERSION -ge 0 -o $_VERSION -ge 02; then
		AC_MSG_RESULT(yes)
		SC_STATEDIR=`scrollkeeper-config --pkglocalstatedir`
		SC_OMFDIR=`scrollkeeper-config --omfdir`
	else
		AC_MSG_ERROR(no bad bad...)
	fi
fi
AC_SUBST(SC_OMFDIR)
AC_SUBST(SC_STATEDIR)

dnl -------------------------------------------------------------------
dnl Check for gettext >= 0.14.2 - uses it's parser/writer functions
dnl -------------------------------------------------------------------
AC_CHECK_HEADER([gettext-po.h])
AC_CHECK_LIB([gettextpo], [po_message_create], [
	LDFLAGS="$LDFLAGS -lgettextpo"
],[
	AC_MSG_ERROR(Missing or inadequate gettext library. Need at least version 0.14.2)
])

dnl -------------------------------------------------------------------
dnl Check for gtkspell >= 2.0 and use it if found
dnl -------------------------------------------------------------------
AC_MSG_CHECKING([for a gtkspell >= $GTKSPELL_OPTIONAL])
PKG_CHECK_EXISTS([gtkspell-2.0 >= $GTKSPELL_OPTIONAL], [
	PKG_CHECK_MODULES(GTKSPELL, [gtkspell-2.0 >= $GTKSPELL_OPTIONAL])
	AC_DEFINE(HAVE_GTKSPELL, [true], [A usable GtkSpell library was found])
],[
	echo "not found - spellchecking disabled."
])
AC_SUBST(GTKSPELL_CFLAGS)
AC_SUBST(GTKSPELL_LIBS)

dnl -------------------------------------------------------------------
dnl Enable debug builds.
dnl -------------------------------------------------------------------
AC_ARG_ENABLE(debug,
	[  --enable-debug              Turn on debugging flags (default=yes)],
	,enable_debug=yes)
AC_MSG_CHECKING([if a debug friendly version should be build])
if test "x$enable_debug" = "xyes" ; then
	CFLAGS="$CFLAGS -ggdb "
else
	CFLAGS="`echo $CFLAGS|sed -e s/-g.//g -e s/-ggdb//g`"
fi
AC_MSG_RESULT($enable_debug)

dnl -------------------------------------------------------------------
dnl Get the date for the man-page and substitute it there and anywhere.
dnl -------------------------------------------------------------------
MY_DATE=`date +%Y-%m-%d`
PIXMAP_DIR=${prefix}/share/pixmaps

AC_SUBST(MY_DATE)
AC_SUBST(PIXMAP_DIR)

dnl -------------------------------------------------------------------
dnl Put out the configured files.
dnl -------------------------------------------------------------------
AC_OUTPUT([
Makefile
gtranslator.spec
data/Makefile
data/colorschemes/Makefile
data/desktop/Makefile
data/desktop/gtranslator.desktop.in
data/mime/Makefile
data/mime/gtranslator.keys_template
data/pixmaps/Makefile
data/scripts/Makefile
doc/Makefile
doc/UMTF/Makefile
help/Makefile
help/C/Makefile
man/Makefile
man/gtranslator.1
po/Makefile.in
src/Makefile
src/dialogs/Makefile
src/toolbareditor/Makefile
src/charmap/Makefile
src/dictionary/Makefile
])

dnl ------------------------------------------------------------------
dnl Print an information screen at the end of the process
dnl ------------------------------------------------------------------
echo "

------------------------------------------------------------------
-- gtranslator compilation options

gtranslator version	  :	${VERSION}

debug friendly build	  :	${enable_debug:-no}
build with deprecation 	  :	${enable_deprecated:-no}
compilation flags      	  :	${CFLAGS}

------------------------------------------------------------------
-- For suggestions, problems & bug reports for gtranslator please
    use http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator


Thanks for your attention and braveness in \"compiling\" gtranslator!
-- The website for gtranslator is http://gtranslator.sourceforge.net

------------------------------------------------------------------
"