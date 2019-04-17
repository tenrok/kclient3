macro(pykclient_tests)
    foreach(_testname ${ARGN})
        add_test(NAME pykclient-${_testname} COMMAND pykclienttestrunner ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.py)
        set_tests_properties(pykclient-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

add_executable(pykclienttestrunner pykclienttestrunner.cpp)
target_link_libraries(pykclienttestrunner KClientPrivate Qt5::Test)

pykclient_tests(
    basictest
)
