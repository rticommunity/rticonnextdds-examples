# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.11)
project(rtiexamples-cloud-discovery-service-library-api-c)
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

add_executable(CloudDiscoveryServiceLibraryAPIC
    "${CMAKE_CURRENT_SOURCE_DIR}/cloud_discovery_service_library_api.c"
)

# TODO: Refactor these once https://github.com/rticommunity/rticonnextdds-cmake-utils/issues/75
# is resolved.
target_compile_definitions(CloudDiscoveryServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_COMPILE_DEFINITIONS})

set_target_properties(CloudDiscoveryServiceLibraryAPIC
    PROPERTIES
        DEBUG_POSTFIX "d"
)

target_include_directories(CloudDiscoveryServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_INCLUDE_DIRS}
)

target_link_directories(CloudDiscoveryServiceLibraryAPIC
    PUBLIC
        ${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}
)

target_link_libraries(CloudDiscoveryServiceLibraryAPIC
    PUBLIC
        RTIConnextDDS::c_api
        rticlouddiscoveryservice
)
