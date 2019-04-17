set(KDEFrameworksIntegration_SRCS
    kdeframeworksintegrationplugin.cpp
    kwalletpasswordbackend.cpp
    kioschemehandler.cpp
)

ecm_create_qm_loader(KDEFrameworksIntegration_SRCS kclient_kdeframeworksintegration_qt)

set(KDEFrameworksIntegration_RSCS
    kdeframeworksintegration.qrc
)
qt5_add_resources(RSCS ${KDEFrameworksIntegration_RSCS})

add_library(KDEFrameworksIntegration MODULE ${KDEFrameworksIntegration_SRCS} ${RSCS})
install(TARGETS KDEFrameworksIntegration DESTINATION ${KCLIENT_INSTALL_PLUGINDIR})
target_link_libraries(KDEFrameworksIntegration
    KClientPrivate
    KF5::Wallet
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Crash
    KF5::CoreAddons
    KF5::PurposeWidgets
)
