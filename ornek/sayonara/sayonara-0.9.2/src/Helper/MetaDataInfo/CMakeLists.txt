PROJECT( say_helper_metadata_info )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./AlbumInfo.cpp
    ./MetaDataInfo.cpp
    ./ArtistInfo.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	say_helper_settings
	say_comp_covers
	say_helper_compressor
)
