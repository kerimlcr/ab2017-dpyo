CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(say_gui_library)

SET(SOURCES
    GUI_AbstractLibrary.cpp
    GUI_LocalLibrary.cpp
    LocalLibraryContainer.cpp

    Models/LibraryItemModelAlbums.cpp
    Models/LibraryItemModelArtists.cpp
    Models/LibraryItemModel.cpp
    Models/LibraryItemModelTracks.cpp

    Delegates/LibraryItemDelegateAlbums.cpp
    Delegates/LibraryItemDelegateArtists.cpp
    Delegates/LibraryItemDelegateTracks.cpp
    Delegates/LibraryRatingDelegate.cpp

    Views/LibraryGenreView.cpp
    Views/LibraryViewAlbum.cpp
    Views/LibraryView.cpp
    Views/LibraryViewEvents.cpp
    Views/LibraryViewHeader.cpp
    Views/HeaderView.cpp

    Helper/DiscPopupMenu.cpp
    Helper/LocalLibraryMenu.cpp
    Helper/ColumnHeader.cpp

    InfoBox/GUI_LibraryInfoBox.cpp
    ImportFolderDialog/GUI_ImportFolder.cpp
)

QT5_WRAP_UI(FORMS_UIC
    GUI_LocalLibrary.ui
    InfoBox/GUI_LibraryInfoBox.ui
    ImportFolderDialog/GUI_ImportFolder.ui
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SOURCES} ${FORMS_UIC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    say_comp_library
    say_comp_tagedit

    say_gui_helper
    say_gui_info_dialog

    say_interfaces_library_container

    say_comp_library_importer
)

#cotire(${PROJECT_NAME})
