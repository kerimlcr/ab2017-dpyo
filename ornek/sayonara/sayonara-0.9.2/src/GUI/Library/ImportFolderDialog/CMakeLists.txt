PROJECT(say_gui_library_import_folder_dialog)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES
    ./GUI_ImportFolder.cpp
)

QT5_WRAP_UI(UI_HEADERS
    ./GUI_ImportFolder.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_HEADERS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_tagedit
    say_gui_helper

)
