set( TestPlugin_SRCS
	testplugin.cpp
	testplugin_sidebar.cpp
	)


set( TestPlugin_RSCS
    testplugin.qrc
)
qt5_add_resources(RSCS ${TestPlugin_RSCS})

ecm_create_qm_loader( TestPlugin_SRCS kclient_testplugin_qt )

add_library(TestPlugin MODULE ${TestPlugin_SRCS} ${RSCS})
install(TARGETS TestPlugin DESTINATION ${KCLIENT_INSTALL_PLUGINDIR})
target_link_libraries(TestPlugin KClientPrivate)
