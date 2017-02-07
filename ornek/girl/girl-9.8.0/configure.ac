#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(girl, 9.8.0)
AM_INIT_AUTOMAKE([no-dist-gzip dist-xz])
AC_CONFIG_SRCDIR([src/girl.c])
AC_CONFIG_HEADER([config.h])

# Checks for libraries.

record_msg="no (not requested)"
try_record=:
girl_record=false

AC_ARG_WITH([recording],AS_HELP_STRING([--with-recording],[Support recording with gstreamer]), [
     AC_MSG_CHECKING([for recording])
     if test x$withval = xno; then
           try_record=false
	   record_msg = "no (not requested)"
	   AC_DEFINE([HAVE_GIRL_RECORD],0,[record missing])
	   girl_record=false
     else
	   AC_DEFINE([HAVE_GIRL_RECORD],1,[have record])
	   record_msg="yes (with recording support)"
	   girl_record=true
     fi
     AC_MSG_RESULT([$record_msg])
])
AM_CONDITIONAL([HAVE_GIRL_RECORD], [$girl_record])
AC_SUBST([HAVE_GIRL_RECORD])

AC_CHECK_LIB(m, pow, [], [], [])

PKG_CHECK_MODULES(GIRL,
		  gtk+-2.0 >= 2.0 \
		  glib-2.0 >= 2.40 \
		  libgnome-2.0 >= 2.0 \
		  libgnomeui-2.0 >= 2.0 \
		  libxml-2.0 >= 2.0 \
		  gnome-vfs-2.0 >= 2.0
		  gstreamer-1.0 >= 1.0
		  gstreamer-video-1.0) 2>&5

GIRL_PKG_DEPS="gtk+-2.0, glib-2.0, libgnome-2.0, libgnomeui-2.0, libxml-2.0, gnome-vfs-2.0, gstreamer-1.0 gstreamer-video-1.0"

GTK_DOC_CHECK([1.16], [--flavour no-tmpl])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

# ***********                                                                   
# Translation                                                                   
# ***********                                                                   

AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])

AC_SUBST(GIRL_LIBS)
AC_SUBST(GIRL_CFLAGS)
AC_SUBST(GIRL_PKG_DEPS)

dnl Enable yelp-tools
YELP_HELP_INIT

AC_OUTPUT([
Makefile
data/Makefile
data/girl.desktop
data/pixmaps/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/256x256/Makefile
data/icons/512x512/Makefile
data/icons/1024x1024/Makefile
help/Makefile
man/Makefile
po/Makefile.in
src/Makefile
girl.spec
])
