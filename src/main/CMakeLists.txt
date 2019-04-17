include_directories(
  ../lib/3rdparty
  ../lib/app
  ../lib/session
  ../lib/webengine
  ../lib/webtab
)
set(SRCS main.cpp)
if (WIN32)
    set(SRCS ${SRCS} version.rc)
    configure_file(kclientversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kclientversion.h)
endif()
ecm_create_qm_loader(SRCS kclient_qt)
add_executable(kclient ${SRCS})

target_link_libraries(kclient KClientPrivate)

if (APPLE)
    set_target_properties(kclient PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set_target_properties(kclient PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.kclient")
    set_target_properties(kclient PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KClient")
    install(FILES lib/data/icons/exeicons/kclient.icns DESTINATION ${BUNDLE_INSTALL_DIR}/kclient.app/Contents/Resources)
    #install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/locale DESTINATION ${BUNDLE_INSTALL_DIR}/kclient.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${BUNDLE_INSTALL_DIR}/kclient.app/Contents/Resources)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins DESTINATION ${BUNDLE_INSTALL_DIR}/kclient.app/Contents/Resources)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES FreeBSD OR CMAKE_SYSTEM_NAME MATCHES OpenBSD)
    target_link_libraries(kclient execinfo)
endif()

if (UNIX AND NOT APPLE)
    install(TARGETS kclient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/themes DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/kclient)
    if (KF5I18n_FOUND)
        ki18n_install(${CMAKE_SOURCE_DIR}/po)
    endif()
    ecm_install_po_files_as_qm(${CMAKE_SOURCE_DIR}/poqm)

    install(PROGRAMS ../../linux/applications/org.kde.kclient.desktop DESTINATION ${KDE_INSTALL_APPDIR})

    install(FILES ../../linux/pixmaps/kclient.png DESTINATION ${KDE_INSTALL_DATADIR}/pixmaps)

    ecm_install_icons(ICONS
        ../../linux/hicolor/16-apps-kclient.png
        ../../linux/hicolor/32-apps-kclient.png
        ../../linux/hicolor/48-apps-kclient.png
        ../../linux/hicolor/64-apps-kclient.png
        ../../linux/hicolor/128-apps-kclient.png
        ../../linux/hicolor/256-apps-kclient.png
        ../../linux/hicolor/sc-apps-kclient.svg
        DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)

    install(FILES ../../linux/completion/kclient DESTINATION ${KDE_INSTALL_DATADIR}/bash-completion/completions)
    install(FILES ../../linux/appdata/org.kde.kclient.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
endif()

