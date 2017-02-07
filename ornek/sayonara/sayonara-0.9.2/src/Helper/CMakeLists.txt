PROJECT(say_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(MetaDataInfo)
ADD_SUBDIRECTORY(Settings)
ADD_SUBDIRECTORY(Compressor)

IF(NOT WIN32 OR NOT DEFINED WIN32)
    ADD_SUBDIRECTORY(Shutdown)
ENDIF()


SET(SOURCES

    EqualizerPresets.cpp
    FileHelper.cpp
    Helper.cpp
    UrlHelper.cpp
    Set.cpp
    Tree.cpp
    LibrarySearchMode.cpp

    Playlist/CustomPlaylist.cpp

    MetaData/LibraryItem.cpp
    MetaData/MetaData.cpp
    MetaData/MetaDataList.cpp
    MetaData/Album.cpp
    MetaData/Artist.cpp

    Logger/Logger.cpp
    Random/RandomGenerator.cpp
    SmartCompare/SmartCompare.cpp
    WebAccess/AsyncWebAccess.cpp

    Parser/AbstractPlaylistParser.cpp

    Parser/M3UParser.cpp
    Parser/PLSParser.cpp
    Parser/ASXParser.cpp

    Parser/PlaylistParser.cpp
    Parser/PodcastParser.cpp
    Parser/StreamParser.cpp

    Parser/CommandLineParser.cpp

    Tagging/Tagging.cpp

    Tagging/Frames/AbstractFrame.cpp
    Tagging/Frames/Discnumber.cpp
    Tagging/Frames/Popularimeter.cpp
    Tagging/Frames/Cover.cpp

    DirectoryReader/DirectoryReader.cpp
)


ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Qt5::Network
    Qt5::Xml
    Qt5::DBus
    Qt5::Gui
    tag
)
#cotire(${PROJECT_NAME})
