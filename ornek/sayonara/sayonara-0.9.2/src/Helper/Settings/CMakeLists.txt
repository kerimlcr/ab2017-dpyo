PROJECT(say_helper_settings)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
	Setting.cpp
	Settings.cpp
	SettingConverter.cpp
	SettingRegistry.cpp
	AbstrSettingNotifier.cpp
	SayonaraClass.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_db
	say_helper
)
