# SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)

set(CROUTONS_VERSION 1.0)
set(CROUTONS_SOVERSION 1)

project(Croutons VERSION ${CROUTONS_VERSION})

include(FeatureSummary)
find_package(ECM 5.85.0 NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Croutons")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/CroutonsConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CroutonsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

add_subdirectory(lib)

install(EXPORT CroutonsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE CroutonsTargets.cmake )
