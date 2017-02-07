PROJECT(say_helper_shutdown)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
    Shutdown.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_playmanager
    say_interfaces_notifications
)
