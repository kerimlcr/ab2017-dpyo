CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_tagedit)

SET(SOURCES
	GUI_TagEdit.cpp
	TagLineEdit.cpp
	TagTextInput.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_TagEdit.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_comp_tagedit
	say_gui_helper
	say_gui_covers
)

#cotire(${PROJECT_NAME})

