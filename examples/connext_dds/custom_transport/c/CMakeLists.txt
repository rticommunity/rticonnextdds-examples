# (c) 2018 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-custom-flow-controller)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

if(WIN32)
    message(WARNING "This example is not supported in Windows.")
    return()
endif()

if(NOT BUILD_SHARED_LIBS)
    message(WARNING "Static linking is not supported for this example")
    return()
endif()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "custom_transport"
    LANG "C"
    CODEGEN_ARGS
        GENERATE_EXAMPLE
)

# This library must be built dynamically always
add_library(rti_custom_transports SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/FileTransport.c"
)

target_link_libraries(rti_custom_transports
    PUBLIC
        RTIConnextDDS::c_api)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(suffix "")
else()
    set(suffix "d")
endif()

set_target_properties(rti_custom_transports
    PROPERTIES
        LIBRARY_OUTPUT_NAME "rti_custom_transports${suffix}"
)
