CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(ICONS
	bg-checker.png
	favicon.ico
	logo.png
)

IF(WIN32)
	SET(ICON_TARGET_PATH
		"."
	)
ELSE()
	SET(ICON_TARGET_PATH
		"share/sayonara"
	)
ENDIF()


INSTALL(FILES "sayonara.png" DESTINATION /usr/share/icons)
INSTALL(FILES "sayonara.png" DESTINATION /usr/share/pixmaps)

INSTALL(FILES ${ICONS} DESTINATION ${ICON_TARGET_PATH})


