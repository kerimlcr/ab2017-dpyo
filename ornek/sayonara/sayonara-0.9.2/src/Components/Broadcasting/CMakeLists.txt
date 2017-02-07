CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_broadcasting)

SET(HEADERS
	StreamServer.h
	StreamWriter.h
	StreamHttpParser.h
	StreamDataSender.h
)

SET(SOURCES
	StreamServer.cpp
	StreamWriter.cpp
	StreamHttpParser.cpp
	StreamDataSender.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_interfaces_raw_sound_receiver
    say_gui_helper
)

#cotire(${PROJECT_NAME})
