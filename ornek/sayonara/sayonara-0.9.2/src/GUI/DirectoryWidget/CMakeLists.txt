CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_directory_widget)

SET(SOURCES
	GUI_DirectoryWidget.cpp
	DirectoryIconProvider.cpp
	DirectoryDelegate.cpp
	FileListView.cpp
	DirectoryTreeView.cpp
)

QT5_WRAP_UI(UI_HEADERS
	GUI_DirectoryWidget.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${UI_HEADERS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_info_dialog
	say_gui_helper
)

#cotire(${PROJECT_NAME})


