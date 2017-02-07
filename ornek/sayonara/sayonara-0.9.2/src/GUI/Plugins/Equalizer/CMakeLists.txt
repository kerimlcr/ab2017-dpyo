CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_equalizer)

SET(SOURCES
	EqSlider.cpp
	GUI_Equalizer.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_Equalizer.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_engine
    say_gui_plugins
)
