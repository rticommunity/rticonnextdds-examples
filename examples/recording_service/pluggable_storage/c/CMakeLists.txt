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

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_NOCASE)

if(CMAKE_BUILD_TYPE_NOCASE STREQUAL "debug")
    set(msg
        "You have selected to build the library in debug mode. It's recommended "
        "to run the debug version of Recording Service or otherwise you may run "
        "into undefined behavior")
    message(WARNING ${msg})
endif()

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

# Create the directory that's going to contain the generated code files
add_custom_target(
    generated_code_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/generated")

set(HELLOMSG_C_SOURCES
    "${CMAKE_BINARY_DIR}/generated/HelloMsg.c"
    "${CMAKE_BINARY_DIR}/generated/HelloMsgPlugin.c"
    "${CMAKE_BINARY_DIR}/generated/HelloMsgSupport.c"
)

# Generate type plug-in files for the storage type
add_custom_command(
    OUTPUT ${HELLOMSG_C_SOURCES}
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg.idl"
    COMMAND ${RTICODEGEN} -language C -update typefiles
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_BINARY_DIR}/generated"
            "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg.idl"
)

add_custom_target(
    generated_code
    DEPENDS
        generated_code_dir
        ${HELLOMSG_C_SOURCES}
)

# Define the library that will provide the storage writer plugin
add_library(
    FileStorageWriter MODULE
        "${CMAKE_BINARY_DIR}/generated/HelloMsg.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileStorageWriter.c"
)

add_dependencies(FileStorageWriter generated_code)

target_link_libraries(
    FileStorageWriter
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageWriter
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
        "${CONNEXTDDS_INCLUDE_DIRS}/recordingservice"
)

# Define the library that will provide the storage reader plugin
add_library(
    FileStorageReader MODULE
        "${CMAKE_BINARY_DIR}/generated/HelloMsg.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileStorageReader.c"
)

add_dependencies(FileStorageReader generated_code)

target_link_libraries(
    FileStorageReader
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageReader
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
        "${CONNEXTDDS_INCLUDE_DIRS}/recordingservice"
)

# Define the publisher application
add_executable(
    HelloMsg_publisher
    ${HELLOMSG_C_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_publisher.c"
)

add_dependencies(HelloMsg_publisher generated_code)

target_include_directories(
    HelloMsg_publisher
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(
    HelloMsg_publisher
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

# Define the subscriber application
add_executable(
    HelloMsg_subscriber
    ${HELLOMSG_C_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_subscriber.c"
)

add_dependencies(HelloMsg_subscriber generated_code)

target_include_directories(
    HelloMsg_subscriber
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(
    HelloMsg_subscriber
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
