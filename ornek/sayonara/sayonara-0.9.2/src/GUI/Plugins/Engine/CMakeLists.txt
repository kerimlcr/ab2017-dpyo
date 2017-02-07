CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_plugin_engine)

SET(SOURCES
	EngineColorStyleChooser.cpp
	EnginePlugin.cpp
	GUI_LevelPainter.cpp
	GUI_Spectrum.cpp
	GUI_StyleSettings.cpp
)

QT5_WRAP_UI(FORMS_UIC
	GUI_LevelPainter.ui
	GUI_Spectrum.ui
	GUI_Style.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_engine
    say_gui_plugins
)
