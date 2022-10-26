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
project(ServiceAsLib)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=ON CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(msg
        "Not specifying CMAKE_BUILD_TYPE can lead to undefined behaviour and "
        "is not supported for this example. Please provide a valid value for "
        "the CMAKE_BUILD_TYPE parameter")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

find_package(
    RTIConnextDDS "6.1.2"
    REQUIRED
    COMPONENTS
        core
        routing_service
        recording_service
)

# Copy recorder and replay configuration files to binary directory
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/recorder_config.xml
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/recorder_config.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_config.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/recorder_config.xml"
)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/replay_config.xml"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/replay_config.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/replay_config.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/replay_config.xml"
)

add_custom_target(
    copy_config_files
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_config.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/replay_config.xml"
)

# Define the subscriber application
add_executable(
    ServiceAsLibExample
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceAsLibExample.cxx"
)

set_target_properties(
    ServiceAsLibExample
    PROPERTIES CXX_STANDARD 11)

add_dependencies(ServiceAsLibExample copy_config_files)

target_include_directories(
    ServiceAsLibExample
    PUBLIC
        ${CONNEXTDDS_INCLUDE_DIRS}
)

target_link_libraries(
    ServiceAsLibExample
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::recording_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
