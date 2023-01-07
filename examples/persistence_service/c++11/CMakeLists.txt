# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.11)
project(rtiexamples-persistence-service)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "hello_world"
    LANG "C++11"
)

find_package(
    RTIConnextDDS "7.0.0"
    REQUIRED
    COMPONENTS
        core
)

add_executable(PersistenceServiceLibraryAPI
    "${CMAKE_CURRENT_SOURCE_DIR}/persistence_service_library_api.c"
)

# TODO: Refactor these once https://github.com/rticommunity/rticonnextdds-cmake-utils/issues/75
# is resolved.
target_compile_definitions(PersistenceServiceLibraryAPI
    PUBLIC
        ${CONNEXTDDS_COMPILE_DEFINITIONS})

set_target_properties(PersistenceServiceLibraryAPI
    PROPERTIES
        DEBUG_POSTFIX "d"
)

target_include_directories(PersistenceServiceLibraryAPI
    PUBLIC
        ${CONNEXTDDS_INCLUDE_DIRS}
        ${CONNEXTDDS_DIR}/include/persistence/
)

target_link_directories(PersistenceServiceLibraryAPI
    PUBLIC
        ${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}
)

target_link_libraries(PersistenceServiceLibraryAPI
    PUBLIC
        RTIConnextDDS::c_api
        rtipersistenceservice
)
