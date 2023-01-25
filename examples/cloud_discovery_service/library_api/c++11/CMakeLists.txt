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

add_executable(CloudDiscoveryServiceLibraryAPICXX11
    "${CMAKE_CURRENT_SOURCE_DIR}/cloud_discovery_service_library_api.cxx"
)

set_property(
    TARGET CloudDiscoveryServiceLibraryAPICXX11
    PROPERTY CXX_STANDARD 11
)

set_property(
    TARGET CloudDiscoveryServiceLibraryAPICXX11
    PROPERTY CXX_STANDARD_REQUIRED ON
)

target_link_libraries(CloudDiscoveryServiceLibraryAPICXX11
    PUBLIC
        RTIConnextDDS::cloud_discovery_service_cpp2
)

# To differentiate between release and debug executables
set_target_properties(CloudDiscoveryServiceLibraryAPICXX11
    PROPERTIES
        DEBUG_POSTFIX "d"
)
