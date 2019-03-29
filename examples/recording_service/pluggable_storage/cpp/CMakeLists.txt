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

# Create the directory that's going to contain the generated code files
add_custom_target(
    generated_code_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/generated")

set(HELLOMSG_CPP_SOURCES
    "${CMAKE_BINARY_DIR}/generated/HelloMsg.cxx"
    "${CMAKE_BINARY_DIR}/generated/HelloMsgPlugin.cxx"
)

# Generate type plug-in files for the storage type
add_custom_command(
    OUTPUT ${HELLOMSG_CPP_SOURCES}
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg.idl"
    COMMAND ${RTICODEGEN} -language C++03 -update typefiles
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_BINARY_DIR}/generated" 
            "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg.idl"
)

add_custom_target(
    generated_code 
    DEPENDS 
        ${HELLOMSG_CPP_SOURCES}
        generated_code_dir
)

# Define the library that will provide the storage writer plugin
add_library(
    FileStorageWriterCpp MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/FileStorageWriter.cxx"
)

target_link_libraries(
    FileStorageWriterCpp
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageWriterCpp
    PUBLIC 
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

# Define the library that will provide the storage reader plugin
add_library(
    FileStorageReaderCpp MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/FileStorageReader.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/FileStorageReader.hpp"
)

target_link_libraries(
    FileStorageReaderCpp
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

target_include_directories(
    FileStorageReaderCpp
    PUBLIC 
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

# Define the publisher application
add_executable(
    HelloMsg_publisher
    ${HELLOMSG_CPP_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_publisher.cxx"
)

add_dependencies(HelloMsg_publisher generated_code)

target_include_directories(
    HelloMsg_publisher
    PUBLIC 
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(
    HelloMsg_publisher
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

# Define the subscriber application
add_executable(
    HelloMsg_subscriber
    ${HELLOMSG_CPP_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_subscriber.cxx"
)

add_dependencies(HelloMsg_subscriber generated_code)

target_include_directories(
    HelloMsg_subscriber
    PUBLIC 
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated"
)

target_link_libraries(
    HelloMsg_subscriber
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
