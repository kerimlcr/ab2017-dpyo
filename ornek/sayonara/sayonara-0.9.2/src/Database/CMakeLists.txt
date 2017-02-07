PROJECT(say_db)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(HEADERS
    CDatabaseConnector.h
)

SET(SOURCES

    AbstractDatabase.cpp
    DatabaseHandler.cpp
    LibraryDatabase.cpp
    DatabaseBookmarks.cpp
    DatabaseConnector.cpp
    DatabaseModule.cpp
    DatabaseAlbums.cpp
    DatabaseArtists.cpp
    DatabaseLibrary.cpp
    DatabasePlaylist.cpp
    DatabasePodcasts.cpp
    DatabaseSettings.cpp
    DatabaseStreams.cpp
    DatabaseTracks.cpp
    DatabaseVisStyles.cpp

    SayonaraQuery.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Sql
	say_helper
)

#cotire(${PROJECT_NAME})

