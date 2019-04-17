include(ECMMarkAsTest)

set(kclient_autotests_SRCS )

qt5_add_resources(kclient_autotests_SRCS autotests.qrc)

macro(kclient_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp ${kclient_autotests_SRCS})
        target_link_libraries(${_testname} Qt5::Test KClientPrivate)
        add_test(NAME kclient-${_testname} COMMAND ${_testname})
        ecm_mark_as_test(${_testname})
        set_tests_properties(kclient-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

kclient_tests(
    qztoolstest
    cookiestest
    adblocktest
    updatertest
    locationbartest
    webviewtest
    webtabtest
    sqldatabasetest
)

set(kclient_autotests_SRCS ${CMAKE_SOURCE_DIR}/tests/modeltest/modeltest.cpp)
include_directories(${CMAKE_SOURCE_DIR}/tests/modeltest)
kclient_tests(
    tabmodeltest
)

set(kclient_autotests_SRCS passwordbackendtest.cpp)
include_directories(${OPENSSL_INCLUDE_DIR})
kclient_tests(
    databasepasswordbackendtest
    databaseencryptedpasswordbackendtest
)
