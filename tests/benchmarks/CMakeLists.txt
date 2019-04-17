set(kclient_benchmarks_SRCS )

qt5_add_resources(kclient_benchmarks_SRCS benchmarks.qrc)

macro(kclient_benchmarks)
    foreach(_benchmarkname ${ARGN})
        add_executable(${_benchmarkname} ${_benchmarkname}.cpp ${kclient_benchmarks_SRCS})
        target_link_libraries(${_benchmarkname} Qt5::Test KClientPrivate)
        #add_test(NAME kclient-${_benchmarkname} COMMAND ${_benchmarkname})
        #ecm_mark_as_test(${_benchmarkname})
        #set_tests_properties(kclient-${_benchmarkname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_benchmarkname)
endmacro()

kclient_benchmarks(
    #adblockmatchrule
    adblockparserule
)
