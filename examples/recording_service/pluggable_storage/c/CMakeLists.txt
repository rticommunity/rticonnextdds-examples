#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

project (PluggableStorage)

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

# Generate type plug-in files for the storage type
add_custom_command(
    OUTPUT 
        "${CMAKE_SOURCE_DIR}/HelloMsg.c" 
        "${CMAKE_SOURCE_DIR}/HelloMsgPlugin.c"
        "${CMAKE_SOURCE_DIR}/HelloMsgSupport.c"
    MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/HelloMsg.idl"
    COMMAND ${RTICODEGEN} -language C -replace -d "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/HelloMsg.idl"
)

# Define the library that will provide the storage writer plugin
add_library(
    FileStorageWriter MODULE
        "${CMAKE_SOURCE_DIR}/HelloMsg.c" 
        "${CMAKE_SOURCE_DIR}/FileStorageWriter.c"
        "${CMAKE_SOURCE_DIR}/FileStorageWriter.h"
)

target_link_libraries(
    FileStorageWriter
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageWriter
    PUBLIC "${CMAKE_BINARY_DIR}"
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}/recordingservice
)

# Define the library that will provide the storage reader plugin
add_library(
    FileStorageReader MODULE
        "${CMAKE_SOURCE_DIR}/HelloMsg.c" 
        "${CMAKE_SOURCE_DIR}/FileStorageReader.c"
        "${CMAKE_SOURCE_DIR}/FileStorageReader.h"
)

target_link_libraries(
    FileStorageReader
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageReader
    PUBLIC "${CMAKE_BINARY_DIR}"
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}/recordingservice
)

# Define the publisher application
add_executable(
    HelloMsg_publisher
    "${CMAKE_SOURCE_DIR}/HelloMsg.c"
    "${CMAKE_SOURCE_DIR}/HelloMsgPlugin.c"
    "${CMAKE_SOURCE_DIR}/HelloMsgSupport.c"
    "${CMAKE_SOURCE_DIR}/HelloMsg_publisher.c"
)

target_include_directories(
    HelloMsg_publisher
    PUBLIC "${CMAKE_BINARY_DIR}"
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}
)

target_compile_definitions(
    HelloMsg_publisher
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS}
)

target_link_libraries(
    HelloMsg_publisher
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

# Define the subscriber application
add_executable(
    HelloMsg_subscriber
    "${CMAKE_SOURCE_DIR}/HelloMsg.c"
    "${CMAKE_SOURCE_DIR}/HelloMsgPlugin.c"
    "${CMAKE_SOURCE_DIR}/HelloMsgSupport.c"
    "${CMAKE_SOURCE_DIR}/HelloMsg_subscriber.c"
)

target_include_directories(
    HelloMsg_subscriber
    PUBLIC "${CMAKE_BINARY_DIR}"
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS}
)

target_compile_definitions(
    HelloMsg_subscriber
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS}
)

target_link_libraries(
    HelloMsg_subscriber
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
