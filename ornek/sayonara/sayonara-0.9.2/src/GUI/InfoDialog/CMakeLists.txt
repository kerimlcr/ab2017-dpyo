CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_info_dialog)

SET(SOURCES
    GUI_InfoDialog.cpp
	InfoDialogContainer.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_InfoDialog.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_lyrics
    say_helper_metadata_info
    say_gui_tagedit
    say_gui_covers
)


