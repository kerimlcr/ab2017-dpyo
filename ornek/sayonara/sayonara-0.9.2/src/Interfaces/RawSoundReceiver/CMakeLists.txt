CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_interfaces_raw_sound_receiver)

SET(SOURCES
	RawSoundReceiverInterface.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
