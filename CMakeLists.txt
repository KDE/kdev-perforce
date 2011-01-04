project(kdevperforce)

set(KDE4_BUILD_TESTS "ON" CACHE "BOOL" "Enable building of tests" FORCE )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${kdevperforce_SOURCE_DIR}/cmake)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=9025)

find_package(KDE4 4.5.1 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
find_package(KDevPlatform 1.1.60 REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${KDEVPLATFORM_INCLUDE_DIR} )

add_subdirectory(test)

set(kdevperforce_PART_SRCS
    perforceplugin.cpp
)

#kde4_add_ui_files( kdevperforce_PART_SRCS myform.ui )

kde4_add_plugin(kdevperforce ${kdevperforce_PART_SRCS})

target_link_libraries(kdevperforce
    ${KDE4_KDEUI_LIBS}
    kdevplatformvcs
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    ${KDEVPLATFORM_LANGUAGE_LIBRARIES}
)

install(TARGETS kdevperforce DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############
install( FILES kdevperforce.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES kdevperforce.rc DESTINATION ${DATA_INSTALL_DIR}/kdevperforce)



