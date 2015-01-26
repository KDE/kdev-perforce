project(kdevperforce)

cmake_minimum_required(VERSION 2.8.12)
find_package (ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMAddTests)



find_package(Qt5 REQUIRED Core Test)
find_package(KF5 REQUIRED COMPONENTS KDELibs4Support TextEditor ThreadWeaver)
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)


add_definitions(-DKDE_DEFAULT_DEBUG_AREA=9025)


add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)

#include_directories(
#${CMAKE_CURRENT_SOURCE_DIR} 
#${CMAKE_CURRENT_BINARY_DIR} 
#${KDEVPLATFORM_INCLUDE_DIR}
#${KDEVPLATFORM_INCLUDE_DIR}/vcs
# )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(p4clientstub)
add_subdirectory(test)

set(kdevperforce_PART_SRCS
    perforceplugin.cpp
)

add_library(kdevperforce MODULE ${kdevperforce_PART_SRCS})

configure_file(kdevperforce.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdevperforce.desktop)
kcoreaddons_desktop_to_json(kdevperforce ${CMAKE_CURRENT_BINARY_DIR}/kdevperforce.desktop)

target_link_libraries(kdevperforce
   KDev::Interfaces
   KDev::Vcs
)

########### install files ###############
install(TARGETS kdevperforce DESTINATION ${PLUGIN_INSTALL_DIR} )


# install( FILES kdevperforce.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
# install( FILES kdevperforce.rc DESTINATION ${DATA_INSTALL_DIR}/kdevperforce)




