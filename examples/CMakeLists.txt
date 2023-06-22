#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project(rticonnextdds-examples)

option(CONNEXTDDS_BUILD_PERSISTENCE_SERVICE_EXAMPLES
    "Build Persistence Service examples"
    OFF
)
option(CONNEXTDDS_BUILD_RECORDING_SERVICE_EXAMPLES
    "Build Recording Service examples"
    OFF
)
option(CONNEXTDDS_BUILD_ROUTING_SERVICE_EXAMPLES
    "Build Routing Service examples"
    OFF
)
option(CONNEXTDDS_BUILD_CLOUD_DISCOVERY_SERVICE_EXAMPLES
    "Build Cloud Discovery Service examples"
    OFF
)

option(CONNEXTDDS_BUILD_WEB_INTEGRATION_SERVICE_EXAMPLES
    "Build Web Integration Service examples"
    OFF
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/connext_dds")

if(CONNEXTDDS_BUILD_PERSISTENCE_SERVICE_EXAMPLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/persistence_service")
endif()
if(CONNEXTDDS_BUILD_RECORDING_SERVICE_EXAMPLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/recording_service")
endif()
if(CONNEXTDDS_BUILD_ROUTING_SERVICE_EXAMPLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/routing_service")
endif()
if(CONNEXTDDS_BUILD_WEB_INTEGRATION_SERVICE_EXAMPLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/web_integration_service")
endif()
if(CONNEXTDDS_BUILD_CLOUD_DISCOVERY_SERVICE_EXAMPLES)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cloud_discovery_service")
endif()
