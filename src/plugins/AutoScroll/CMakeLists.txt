set( AutoScroll_SRCS
	autoscrollplugin.cpp
	autoscroller.cpp
	framescroller.cpp
	autoscrollsettings.cpp
	)

ecm_create_qm_loader( AutoScroll_SRCS kclient_autoscroll_qt )

set( AutoScroll_UIS
	autoscrollsettings.ui
	)
qt5_wrap_ui(UIS ${AutoScroll_UIS})

set( AutoScroll_RSCS
	autoscroll.qrc
	)
qt5_add_resources(RSCS ${AutoScroll_RSCS})

add_library(AutoScroll MODULE ${AutoScroll_SRCS} ${UIS} ${RSCS})
install(TARGETS AutoScroll DESTINATION ${KCLIENT_INSTALL_PLUGINDIR})
target_link_libraries(AutoScroll KClientPrivate)

