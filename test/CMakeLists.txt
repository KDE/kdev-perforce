# Running the test only makes sense if the perforce command line client
# is present. So check for it before adding the test...
set( perforceplugintest_SRCS
     perforceplugintest.cpp
     ../perforceplugin.cpp
   )

add_definitions( "-DP4_CLIENT_STUB_EXE=\\\"${CMAKE_BINARY_DIR}/p4clientstub/p4clientstub\\\"" )

kde4_add_unit_test( perforceplugintest ${perforceplugintest_SRCS} )
target_link_libraries( perforceplugintest
        ${QT_QTTEST_LIBRARY}
        ${KDEVPLATFORM_TESTS_LIBRARIES}
        ${KDEVPLATFORM_VCS_LIBRARIES}
    )

