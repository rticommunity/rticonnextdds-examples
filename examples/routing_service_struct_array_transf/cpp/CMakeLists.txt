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

cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 11)
project (StructArrayTransformation)


# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

find_package(
    RTICodeGenerator "3.0.0"
    REQUIRED
    )

# Generate type plug-in files for conversion types
add_custom_command(OUTPUT
    SensorData.idl
    SensorData.cxx
    SensorDataPlugin.cxx
    COMMAND ${RTICODEGEN} -language C++11 -stl -replace -d ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/SensorData.idl)

# Define the library that will provide the  Transformation plugin
add_library(structarraytransf SHARED
    ${CMAKE_SOURCE_DIR}/StructArrayTransformation.cxx
    ${CMAKE_BINARY_DIR}/SensorData.cxx
    ${CMAKE_BINARY_DIR}/SensorDataPlugin.cxx)

target_include_directories(structarraytransf
    PUBLIC ${CMAKE_BINARY_DIR}
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS})

target_compile_definitions(structarraytransf
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS})

target_link_libraries(structarraytransf
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS})


# Define the Publisher application
add_executable(SensorAttributesCollectionPublisher
    ${CMAKE_SOURCE_DIR}/SensorAttributesCollectionPublisher.cxx
    ${CMAKE_BINARY_DIR}/SensorData.cxx
    ${CMAKE_BINARY_DIR}/SensorDataPlugin.cxx)

target_include_directories(SensorAttributesCollectionPublisher
    PUBLIC ${CMAKE_BINARY_DIR}
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS})

target_compile_definitions(SensorAttributesCollectionPublisher
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS})

target_link_libraries(SensorAttributesCollectionPublisher
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS})


# Define the Subscriber application
add_executable(SensorDataSubscriber
    ${CMAKE_SOURCE_DIR}/SensorDataSubscriber.cxx
    ${CMAKE_BINARY_DIR}/SensorData.cxx
    ${CMAKE_BINARY_DIR}/SensorDataPlugin.cxx)

target_include_directories(SensorDataSubscriber
    PUBLIC ${CMAKE_BINARY_DIR}
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS})

target_compile_definitions(SensorDataSubscriber
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS})

target_link_libraries(SensorDataSubscriber
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS})