CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(sayonara_soundcloud)

SET(SOURCES
	src/SoundcloudData.cpp
	src/SoundcloudWebAccess.cpp
	src/SoundcloudJsonParser.cpp
	src/SoundcloudLibrary.cpp
	src/SoundcloudDataFetcher.cpp
	ui/GUI_SoundcloudLibrary.cpp
	ui/GUI_SoundcloudArtistSearch.cpp

	SoundcloudLibraryContainer.cpp
)

SET(FORMS
	ui/GUI_SoundcloudLibrary.ui
	ui/GUI_SoundcloudArtistSearch.ui
)

SET(RESOURCES
	SoundcloudIcons.qrc
)

QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

QT5_WRAP_UI(FORMS_HEADERS ${FORMS})

IF(WIN32)
	ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_HEADERS} ${RESOURCES_RCC})
	SET(SOUNDCLOUD_INSTALL_PATH
		"share"
	)
ELSE()
	ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES} ${FORMS_HEADERS} ${RESOURCES_RCC})
	SET(SOUNDCLOUD_INSTALL_PATH
		"share/sayonara"
	)

ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_gui_helper
	say_gui_library
	say_interfaces_library_container
)

#cotire(${PROJECT_NAME})

IF(NOT WIN32 OR NOT DEFINED WIN32)
	INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib${LIB_SUFFIX}/sayonara)
ENDIF()

INSTALL(FILES "soundcloud.db" DESTINATION ${SOUNDCLOUD_INSTALL_PATH})

