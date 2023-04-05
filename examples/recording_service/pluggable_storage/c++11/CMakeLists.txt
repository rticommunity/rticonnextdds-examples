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
project (PluggableStorageC++11)

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_CXX_STANDARD 11)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=ON CMake parameter")
    message(WARNING ${msg})
    return()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(msg
        "Not specifying CMAKE_BUILD_TYPE can lead to undefined behaviour and "
        "is not supported for this example. Please provide a valid value for "
        "the CMAKE_BUILD_TYPE parameter")
    message(WARNING ${msg})
    return()
endif()

if(CONNEXTDDS_IMPORTED_TARGETS_DEBUG)
    set(msg
        "You have selected to build the library in debug mode. It's recommended "
        "to run the debug version of Recording Service or otherwise you may run "
        "into undefined behavior")
    message(WARNING ${msg})
endif()

# Find RTI Connext dependencies
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(
    RTIConnextDDS "7.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

# Run Codegen to generate the source code files for the types
include(ConnextDdsCodegen)
connextdds_rtiddsgen_run(
    IDL_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg.idl"
    OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
    LANG C++11
)

add_custom_target(${PROJECT_NAME}_codegen_sources
    DEPENDS
        ${HelloMsg_CXX11_GENERATED_SOURCES}
)

# Define the library that will provide the storage writer plugin
add_library(
    FileStorageWriterCpp
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
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# Define the library that will provide the storage reader plugin
add_library(
    FileStorageReaderCpp
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
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# Define the publisher application
add_executable(
    ${PROJECT_NAME}_HelloMsg_publisher
    ${HelloMsg_CXX11_GENERATED_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_publisher.cxx"
)

add_dependencies(${PROJECT_NAME}_HelloMsg_publisher ${PROJECT_NAME}_codegen_sources)

set_target_properties(${PROJECT_NAME}_HelloMsg_publisher
    PROPERTIES
        OUTPUT_NAME "HelloMsg_publisher"
)

target_include_directories(
    ${PROJECT_NAME}_HelloMsg_publisher
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(
    ${PROJECT_NAME}_HelloMsg_publisher
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

# Define the subscriber application
add_executable(
    ${PROJECT_NAME}_HelloMsg_subscriber
    ${HelloMsg_CXX11_GENERATED_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloMsg_subscriber.cxx"
)

add_dependencies(${PROJECT_NAME}_HelloMsg_subscriber ${PROJECT_NAME}_codegen_sources)

set_target_properties(${PROJECT_NAME}_HelloMsg_subscriber
    PROPERTIES
        OUTPUT_NAME "HelloMsg_subscriber"
)

target_include_directories(
    ${PROJECT_NAME}_HelloMsg_subscriber
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(
    ${PROJECT_NAME}_HelloMsg_subscriber
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
