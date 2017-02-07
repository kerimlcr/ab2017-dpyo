CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_covers)

SET(SOURCES
	CoverButton.cpp
	AlternativeCoverItemDelegate.cpp
	AlternativeCoverItemModel.cpp
	GUI_AlternativeCovers.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_AlternativeCovers.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_covers
    say_gui_helper
)

#cotire(${PROJECT_NAME})

