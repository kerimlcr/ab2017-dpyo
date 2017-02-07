CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_interfaces_notifications)


SET(SOURCES
	NotificationInterface.cpp
	NotificationHandler.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_helper_settings
)

#cotire(${PROJECT_NAME})
