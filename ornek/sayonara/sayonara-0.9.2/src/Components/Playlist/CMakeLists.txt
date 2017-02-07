PROJECT(say_comp_playlist)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(SOURCES
    Playlist.cpp
    StdPlaylist.cpp
    StreamPlaylist.cpp
    PlaylistHandler.cpp
    PlaylistLoader.cpp
    PlaylistDBInterface.cpp
    PlaylistDBWrapper.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_playmanager
	say_comp_engine
)
#cotire(${PROJECT_NAME})

