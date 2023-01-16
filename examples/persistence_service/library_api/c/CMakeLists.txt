# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission. Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.12)
project(rticonnextdds-examples-persistence-service)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(
    RTIConnextDDS "7.0.0"
    REQUIRED
    COMPONENTS
        core
)

add_executable(PersistenceServiceLibraryAPIC
    "${CMAKE_CURRENT_SOURCE_DIR}/persistence_service_library_api.c"
)

# TODO: Refactor these once https://github.com/rticommunity/rticonnextdds-cmake-utils/issues/75
# is resolved.
target_compile_definitions(PersistenceServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_COMPILE_DEFINITIONS})

set_target_properties(PersistenceServiceLibraryAPIC
    PROPERTIES
        DEBUG_POSTFIX "d"
)

target_include_directories(PersistenceServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_INCLUDE_DIRS}
        ${CONNEXTDDS_DIR}/include/persistence/
)

target_link_directories(PersistenceServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}
)

target_link_libraries(PersistenceServiceLibraryAPIC
    PUBLIC
        RTIConnextDDS::c_api
        rtipersistenceservice
)
