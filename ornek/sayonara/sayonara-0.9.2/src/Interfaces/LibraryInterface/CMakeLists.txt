CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_interfaces_libraryinterface)

ADD_SUBDIRECTORY(LibraryContainer)

SET(SOURCES
    LibraryPluginHandler.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_helper
)


