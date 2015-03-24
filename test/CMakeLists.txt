set( perforceplugintest_SRCS
     perforceplugintest.cpp
     ../perforceplugin.cpp
   )

add_definitions( "-DP4_CLIENT_STUB_EXE=\"${CMAKE_BINARY_DIR}/p4clientstub/p4clientstub\"" )

ecm_add_test(${perforceplugintest_SRCS}
        TEST_NAME test_kdevperforce
        LINK_LIBRARIES kdevperforce Qt5::Test KDev::Vcs KDev::Util KDev::Tests )

