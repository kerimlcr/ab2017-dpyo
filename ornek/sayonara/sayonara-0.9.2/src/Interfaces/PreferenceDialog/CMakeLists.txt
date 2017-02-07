CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_interfaces_preference_dialog)

SET(SOURCES
	PreferenceInterface.cpp
	PreferenceDialogInterface.cpp
	PreferenceWidgetInterface.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_helper
)

#cotire(${PROJECT_NAME})
