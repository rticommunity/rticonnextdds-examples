#
# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project(rtiexamples-custom-flow-controller)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

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
