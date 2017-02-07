CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_streamplugins_streams)

SET(SOURCES
	AbstractStreamHandler.cpp
	StreamHandlerPodcasts.cpp
	StreamHandlerStreams.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_playlist
)

#cotire(${PROJECT_NAME})
