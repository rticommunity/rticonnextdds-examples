# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
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
