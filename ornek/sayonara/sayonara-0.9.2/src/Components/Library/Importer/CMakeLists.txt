PROJECT( say_comp_library_importer  )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


SET(SOURCES
    ./ImportCache.cpp
    ./CachingThread.cpp
    ./CopyThread.cpp
    ./LibraryImporter.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_gui_helper
)
