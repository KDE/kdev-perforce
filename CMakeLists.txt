project(kdevperforce)

#add_definitions(-DTRANSLATION_DOMAIN=\"kdevperforce\")

cmake_minimum_required(VERSION 2.8.12)
find_package (ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMAddTests)



find_package(Qt5 REQUIRED Core Test)
find_package(KF5 REQUIRED COMPONENTS TextEditor ThreadWeaver ItemModels)
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)

add_subdirectory(p4clientstub)
add_subdirectory(test)

set(kdevperforce_PART_SRCS
    perforceplugin.cpp
    perforcepluginmetadata.cpp
)

kdevplatform_add_plugin(kdevperforce JSON kdevperforce.json SOURCES ${kdevperforce_PART_SRCS})

target_link_libraries(kdevperforce
   KDev::Interfaces
   KDev::Vcs
)

########### install files ###############
install(TARGETS kdevperforce DESTINATION ${PLUGIN_INSTALL_DIR} )


install( FILES kdevperforce.rc DESTINATION ${DATA_INSTALL_DIR}/kdevperforce)




