
AM_CPPFLAGS = $(GIRL_CFLAGS) \
	$(NULL)

AM_CFLAGS = $(GIRL_CFLAGS) \
	-DGIRL_DATADIR="\"$(pkgdatadir)\"" \
	$(NULL)

AM_LDFLAGS= $(GIRL_LIBS)

noinst_PROGRAMS = test-stations-gfx test-endianess test-stations

test_stations_SOURCES = test-stations.c girl-stations-map.h girl-stations-map.c girl-tz.c girl-tz.h
test_stations_LDADD = $(GIRL_LIBS) 
test_stations_CFLAGS = $(GIRL_CFLAGS) -DGIRL_DATADIR="\"$(pkgdatadir)\""

test_stations_gfx_SOURCES = test-stations-gfx.c girl-tz.c girl-tz.h
test_stations_gfx_LDADD = $(GIRL_LIBS)
test_stations_gfx_CFLAGS = $(GIRL_CFLAGS) -DGIRL_DATADIR="\"$(pkgdatadir)\""

test_endianess_SOURCES = test-endianess.c date-endian.c date-endian.h
test_endianess_LDADD = $(GIRL_LIBS)
test_endianess_CFLAGS = $(GIRL_CFLAGS) -DGIRL_DATADIR="\"$(pkgdatadir)\""

bin_PROGRAMS = girl

girl_SOURCES = \
	       girl-gui.c \
	       girl-listener.c \
	       girl-program.c \
	       girl-player.c \
	       girl-player-backend.c \
	       girl-player-frontend.c \
	       girl-record.c \
               girl-record-backend.c \
	       girl-record-frontend.c \
               girl-runners.c \
	       girl-station.c \
	       girl-stations-map.c \
	       girl-streams.c \
	       girl-tz.c \
	       girl.c

girl_CFLAGS  = $(GIRL_CFLAGS) \
	       -DGIRL_DATADIR=\"$(datadir)/girl\" \
	       -DDATADIR=\"$(datadir)\" \
	       -DGIRL_DEBUG=1 \
	       -DGIRL_CFG \
	       -DGNOMELOCALEDIR=\"$(datadir)/locale\"

girl_LDADD = $(GIRL_LIBS)

girldir = $(datadir)/girl

girl_DATA = girl-9.7.dtd girl-logo.png girl.png girl.xml girl.xsl

EXTRA_DIST = girl.h girl-station.h girl-stations-map.h girl-tz.h girl-gui.h girl-listener.h girl-player.h girl-player-globals.h girl-player-frontend.h girl-player-backend.h girl-record.h girl-record-backend.h girl-record-globals.h girl-record-frontend.h girl-program.h girl-runners.h girl-streams.h girl-9.7.dtd girl-map.png girl-logo.png girl.png girl.xml girl.xsl

all-local: check-local

check-local: test-stations-gfx test-endianess test-stations
#	$(builddir)/test-stations-gfx ../data
#	$(builddir)/test-endianess
#	$(builddir)/test-stations
