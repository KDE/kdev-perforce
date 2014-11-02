project(kdevperforce)


set(KDEVPERFORCE_VERSION_MAJOR 1)
set(KDEVPERFORCE_VERSION_MINOR 9)
set(KDEVPERFORCE_VERSION_PATCH 90)

# KDevplatform dependency version
set( KDEVPLATFORM_VERSION "${KDEVPERFORCE_VERSION_MAJOR}.${KDEVPERFORCE_VERSION_MINOR}.${KDEVPERFORCE_VERSION_PATCH}" )

cmake_minimum_required(VERSION 2.8.12)
find_package (ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${kdevperforce_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 REQUIRED Core Test)
find_package(KF5 REQUIRED COMPONENTS KDELibs4Support ThreadWeaver I18n)
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)


add_definitions(-DKDE_DEFAULT_DEBUG_AREA=9025)


add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_BINARY_DIR} 
 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(p4clientstub)
add_subdirectory(test)

set(kdevperforce_PART_SRCS
    perforceplugin.cpp
)

add_library(kdevperforce MODULE ${kdevperforce_PART_SRCS})

target_link_libraries(kdevperforce
    kdevplatformvcs
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    ${KDEVPLATFORM_LANGUAGE_LIBRARIES}
)

install(TARGETS kdevperforce DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############
install( FILES kdevperforce.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES kdevperforce.rc DESTINATION ${DATA_INSTALL_DIR}/kdevperforce)




