PROJECT(say_comp_mtp)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PKG_CHECK_MODULES(LIBMTP libmtp)

IF(${MTP_FOUND})

    INCLUDE_DIRECTORIES(LIBMTP_INCLUDE_DIRS)

    SET(SOURCES
	    MTP.cpp
	    MTP_CopyFiles.cpp
	    MTP_Device.cpp
	    MTP_File.cpp
	    MTP_Folder.cpp
	    MTP_RawDevice.cpp
	    MTP_Storage.cpp
	    MTP_Track.cpp
    )

    ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	mtp
	say_helper_settings
    )

    #cotire(${PROJECT_NAME})

ENDIF()
