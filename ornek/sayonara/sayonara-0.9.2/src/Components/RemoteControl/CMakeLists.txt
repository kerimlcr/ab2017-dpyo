CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_remote_control)

SET(SOURCES
    RemoteControl.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Gui

    say_comp_playmanager
    say_comp_playlist
    say_comp_covers
)
