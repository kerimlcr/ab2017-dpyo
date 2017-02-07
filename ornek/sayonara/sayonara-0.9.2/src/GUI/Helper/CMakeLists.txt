PROJECT(say_gui_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SOURCES

	CustomMimeData.cpp
	GUI_Helper.cpp

	ContextMenu/ContextMenu.cpp
	ContextMenu/LibraryContextMenu.cpp

	Delegates/ComboBoxDelegate.cpp
	Delegates/TreeDelegate.cpp
	Delegates/ListDelegate.cpp

	IconLoader/IconLoader.cpp

	Message/Message.cpp
	Message/GlobalMessage.cpp
	Message/GlobalMessageReceiverInterface.cpp

	MenuTool/MenuButton.cpp
	MenuTool/MenuTool.cpp

	RatingLabel/RatingLabel.cpp

	SayonaraWidget/SayonaraWidgetTemplate.cpp
	SayonaraWidget/SayonaraWidget.cpp
	SayonaraWidget/SayonaraDialog.cpp
	SayonaraWidget/SayonaraSlider.cpp
	SayonaraWidget/SayonaraLoadingBar.cpp

	SearchableWidget/SearchableListView.cpp
	SearchableWidget/SearchableTableView.cpp
	SearchableWidget/SearchableFileTreeView.cpp
	SearchableWidget/SayonaraSelectionView.cpp
	SearchableWidget/MiniSearcher.cpp

	Shortcuts/Shortcut.cpp
	Shortcuts/ShortcutHandler.cpp
	Shortcuts/ShortcutWidget.cpp

	Style/Style.cpp

)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	Qt5::Widgets
	say_helper_settings
)

#cotire(${PROJECT_NAME})
