CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_playlist_chooser)

SET(SOURCES
    GUI_PlaylistChooser.cpp
	GUI_TargetPlaylistDialog.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_PlaylistChooser.ui
	GUI_TargetPlaylistDialog.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_playlist_chooser
    say_gui_plugins
)
