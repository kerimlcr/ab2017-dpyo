CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
PROJECT(say_gui_player)

SET(SOURCES
    GUI_Player.cpp
    GUI_PlayerEvents.cpp
    GUI_PlayerPlugins.cpp
    GUI_PlayerMenubar.cpp
    GUI_PlayerControls.cpp
    GUI_PlayerCover.cpp
    GUI_PlayerConnections.cpp
    GUI_PlayerMessages.cpp
    GUI_TrayIcon.cpp
    SearchSlider.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_Player.ui
)

SET(PLAYER_LIBRARIES

    say_gui_playlist

    say_interfaces_player_plugin
    say_interfaces_notifications
    say_interfaces_preference_dialog
    say_gui_plugins
)

IF(${MTP_FOUND})
SET(PLAYER_LIBRARIES
    ${PLAYER_LIBRARIES}
    say_gui_mtp
)
ENDIF()


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${PLAYER_LIBRARIES}
)

#cotire(${PROJECT_NAME})
