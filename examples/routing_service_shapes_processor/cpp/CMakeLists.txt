#
# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form
#  only for use with RTI products.  The Software is provided "as is", with no
#  warranty of any type, including any warranty for fitness for any purpose.
#  RTI is under no obligation to maintain or support the Software.  RTI shall
#  not be liable for any incidental or consequential damages arising out of the
#  use or inability to use the software.

cmake_minimum_required(VERSION 3.11)
project(ShapesProcessor)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=1 CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{NDDSHOME}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

# Define the library that will provide the  Processor plugin
add_library(shapesprocessor
    "${CMAKE_CURRENT_SOURCE_DIR}/ShapesProcessor.cxx")

target_link_libraries(shapesprocessor
    ${CONNEXTDDS_EXTERNAL_LIBS}
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::routing_service_infrastructure)

target_include_directories(shapesprocessor
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(${target_name}
    PROPERTIES
        CXX_STANDARD 11
)