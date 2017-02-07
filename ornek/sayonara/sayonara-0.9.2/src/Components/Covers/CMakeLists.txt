CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_comp_covers)

SET(SOURCES
    CoverHelper.cpp
    CoverLookup.cpp
    CoverLookupAlternative.cpp
    CoverLookupAll.cpp
    CoverFetchThread.cpp
    CoverLocation.cpp
    LocalCoverSearcher.cpp
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_db
)
#cotire(${PROJECT_NAME})
