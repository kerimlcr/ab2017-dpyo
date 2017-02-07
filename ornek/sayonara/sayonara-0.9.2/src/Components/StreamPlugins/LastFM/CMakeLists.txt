CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_streamplugins_lastfm)

SET(SOURCES

	ArtistMatch.cpp
    LastFM.cpp

    LFMLoginThread.cpp
	LFMSimArtistsParser.cpp
    LFMTrackChangedThread.cpp
    LFMWebAccess.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_helper
	say_helper_compressor
)

#cotire(${PROJECT_NAME})
