#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#

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
