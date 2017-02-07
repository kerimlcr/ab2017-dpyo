SUBDIRS = pixmaps icons

@INTLTOOL_DESKTOP_RULE@

DESKTOP_IN_FILES= girl.desktop.in.in
DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications
desktop_DATA = $(DESKTOP_FILES)

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/appdata
appdata_in_files = girl.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

# gsettings_ENUM_NAMESPACE = org.gnome.girl
# gsettings_ENUM_FILES = $(top_srcdir)/src/eog-scroll-view.h	\
#                        $(top_srcdir)/src/eog-window.h
# gsettings_SCHEMAS = org.gnome.girl.gschema.xml

# @GSETTINGS_RULES@

# gsettingsconvertdir = $(datadir)/GConf/gsettings
# gsettingsconvert_DATA = eog.convert

# uidir = $(pkgdatadir)
# ui_DATA = \
# 	eog-toolbar.xml

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = girl.pc

EXTRA_DIST =			\
	$(DESKTOP_IN_FILES)	\
	$(appdata_in_files)	\
	timezone_-1.png 	\
	timezone_-10.png 	\
	timezone_-10_dim.png 	\
	timezone_-11.png 	\
	timezone_-11_dim.png 	\
	timezone_-1_dim.png 	\
	timezone_-2.png 	\
	timezone_-2_dim.png 	\
	timezone_-3.5.png 	\
	timezone_-3.5_dim.png 	\
	timezone_-3.png 	\
	timezone_-3_dim.png 	\
	timezone_-4.5.png 	\
	timezone_-4.5_dim.png 	\
	timezone_-4.png 	\
	timezone_-4_dim.png 	\
	timezone_-5.5.png 	\
	timezone_-5.5_dim.png 	\
	timezone_-5.png 	\
	timezone_-5_dim.png 	\
	timezone_-6.png 	\
	timezone_-6_dim.png 	\
	timezone_-7.png 	\
	timezone_-7_dim.png 	\
	timezone_-8.png 	\
	timezone_-8_dim.png 	\
	timezone_-9.5.png 	\
	timezone_-9.5_dim.png 	\
	timezone_-9.png 	\
	timezone_-9_dim.png 	\
	timezone_0.png 		\
	timezone_0_dim.png 	\
	timezone_1.png 		\
	timezone_10.5.png 	\
	timezone_10.5_dim.png 	\
	timezone_10.png 	\
	timezone_10_dim.png 	\
	timezone_11.5.png 	\
	timezone_11.5_dim.png 	\
	timezone_11.png 	\
	timezone_11_dim.png 	\
	timezone_12.75.png 	\
	timezone_12.75_dim.png 	\
	timezone_12.png 	\
	timezone_12_dim.png 	\
	timezone_13.png 	\
	timezone_13_dim.png 	\
	timezone_14.png 	\
	timezone_14_dim.png 	\
	timezone_1_dim.png 	\
	timezone_2.png 		\
	timezone_2_dim.png 	\
	timezone_3.5.png 	\
	timezone_3.5_dim.png 	\
	timezone_3.png 		\
	timezone_3_dim.png 	\
	timezone_4.5.png 	\
	timezone_4.5_dim.png 	\
	timezone_4.png 		\
	timezone_4_dim.png 	\
	timezone_5.5.png 	\
	timezone_5.5_dim.png 	\
	timezone_5.75.png 	\
	timezone_5.75_dim.png 	\
	timezone_5.png 		\
	timezone_5_dim.png 	\
	timezone_6.5.png 	\
	timezone_6.5_dim.png 	\
	timezone_6.png 		\
	timezone_6_dim.png 	\
	timezone_7.png 		\
	timezone_7_dim.png 	\
	timezone_8.75.png 	\
	timezone_8.75_dim.png 	\
	timezone_8.png 		\
	timezone_8_dim.png 	\
	timezone_9.5.png 	\
	timezone_9.5_dim.png 	\
	timezone_9.png 		\
	timezone_9_dim.png

CLEANFILES = 			\
	$(appdata_DATA)

DISTCLEANFILES = 		\
        $(DESKTOP_FILES)
#        $(gsettings_SCHEMAS)

-include $(top_srcdir)/git.mk
