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
#

cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 11)
project (ShapesProcessor)


# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

# Define the library that will provide the  Processor plugin
add_library(shapesprocessor SHARED ${CMAKE_SOURCE_DIR}/ShapesProcessor.cxx)

target_include_directories(shapesprocessor
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS})

target_compile_definitions(shapesprocessor
    PUBLIC
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS})

target_link_libraries(shapesprocessor
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS})
