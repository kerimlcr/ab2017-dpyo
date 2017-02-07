CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
PROJECT(say_application)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR},
	${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCES
    application.cpp
    InstanceThread.cpp
)

SET(APPLICATION_LIBRARIES
    say_gui_library
    say_gui_directory_widget

    say_gui_plugin_audio_converter
    say_gui_plugin_bookmarks
    say_gui_plugin_broadcasting
    say_gui_plugin_engine
    say_gui_plugin_equalizer
    say_gui_plugin_playlist_chooser
    say_gui_plugin_speed
    say_gui_plugin_stream
    say_gui_plugin_crossfader

    say_gui_player
    say_gui_preferences

    say_interfaces_libraryinterface
    say_comp_remote_control
)

IF(NOT WIN32 OR NOT DEFINED WIN32)
    SET(APPLICATION_LIBRARIES
	${APPLICATION_LIBRARIES}
	say_comp_dbus
    )
ELSE()
    SET(APPLICATION_LIBRARIES
	    ${APPLICATION_LIBRARIES}
	    sayonara_somafm
	    sayonara_soundcloud
    )
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${APPLICATION_LIBRARIES}
)


