PROJECT(sayonara_soundcloud)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)


SET(HEADERS
	SoundcloudData.h
	SoundcloudHelper.h
	SoundcloudLibrary.h
)

SET(SOURCES
	SoundcloudData.cpp
	SoundcloudHelper.cpp
	SoundcloudLibrary.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Core
	Qt5::Xml
	say_comp_library
)



