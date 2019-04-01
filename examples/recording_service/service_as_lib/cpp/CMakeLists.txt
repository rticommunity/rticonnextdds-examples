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

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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

# Select the adequate version of the Recording Service library depending on the
# build type
set(recording_lib "rtirecordingservice")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(recording_lib "${recording_lib}d")
endif()

find_library(
    recordingservicelib
    NAMES ${recording_lib} 
    PATHS "${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}")
    
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
    "${recordingservicelib}"
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
