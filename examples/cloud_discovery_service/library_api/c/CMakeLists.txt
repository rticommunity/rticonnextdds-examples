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
