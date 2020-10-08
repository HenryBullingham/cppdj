# Copyright(C) 2020 Henry Bullingham
# This file is subject to the license terms in the LICENSE file
# found in the top - level directory of this distribution.


cmake_minimum_required(VERSION 3.1)

project(cppdj)

set(header_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cppdj/cppdj.hpp)

add_library(cppdj INTERFACE)
target_sources(cppdj INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(cppdj INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>")
target_include_directories(cppdj SYSTEM INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>")


# Setup package config
include( CMakePackageConfigHelpers )
set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/cppdj)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cppdj-config-version.cmake
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion)

# Install target
install(DIRECTORY include/cppdj DESTINATION include)

enable_testing()
install(TARGETS cppdj
    EXPORT cppdj-targets
    DESTINATION lib)

install( EXPORT cppdj-targets
    DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR})

install( FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cppdj-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cppdj-config-version.cmake
    DESTINATION
    ${CONFIG_PACKAGE_INSTALL_DIR} )
    
option(CPPDJ_BUILD_TEST "build tests" ON)
if(${CPPDJ_BUILD_TEST})
    add_subdirectory(test/)
endif()

option(CPPDJ_BUILD_EXAMPLES "build examples" ON)
if(${CPPDJ_BUILD_EXAMPLES})
    add_subdirectory(examples/)
endif()
