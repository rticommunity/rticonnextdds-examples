# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission. Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.12)
project(rticonnextdds-examples-cloud-discovery-service)
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
        cloud_discovery_service
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CloudDiscoveryServiceConfig.xml"
    "${CMAKE_CURRENT_BINARY_DIR}/CloudDiscoveryServiceConfig.xml"
    COPYONLY
)

add_executable(CloudDiscoveryServiceLibraryAPIC
    "${CMAKE_CURRENT_SOURCE_DIR}/cloud_discovery_service_library_api.c"
)

target_link_libraries(CloudDiscoveryServiceLibraryAPIC
    PUBLIC
        RTIConnextDDS::cloud_discovery_service_c
)

# To differentiate between release and debug executables
set_target_properties(CloudDiscoveryServiceLibraryAPIC
    PROPERTIES
        DEBUG_POSTFIX "d"
)
