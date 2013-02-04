# Running the test only makes sense if the perforce command line client
# is present. So check for it before adding the test...
set( perforceplugintest_SRCS
     perforceplugintest.cpp
     ../perforceplugin.cpp
   )

add_definitions( "-DPROJECTS_SOURCE_DIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}/projects\\\"" )

kde4_add_unit_test( perforceplugintest ${perforceplugintest_SRCS} )
target_link_libraries( perforceplugintest
        ${QT_QTTEST_LIBRARY}
        ${KDEVPLATFORM_TESTS_LIBRARIES}
        ${KDEVPLATFORM_VCS_LIBRARIES}
    )

