# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission. Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.12)
project(rticonnextdds-examples-web-integration-service)
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
        web_integration_service
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/WebIntegrationServiceConfig.xml"
    "${CMAKE_CURRENT_BINARY_DIR}/WebIntegrationServiceConfig.xml"
    COPYONLY
)

add_executable(WebIntegrationServiceLibraryAPICXX11
    "${CMAKE_CURRENT_SOURCE_DIR}/web_integration_service_library_api.cxx"
)

set_property(
    TARGET WebIntegrationServiceLibraryAPICXX11
    PROPERTY CXX_STANDARD 11
)
set_property(
    TARGET WebIntegrationServiceLibraryAPICXX11
    PROPERTY CXX_STANDARD_REQUIRED ON
)

target_link_libraries(WebIntegrationServiceLibraryAPICXX11
    PUBLIC
        RTIConnextDDS::web_integration_service_cpp2
)
