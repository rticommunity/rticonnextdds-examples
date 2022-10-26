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
project (RecorderRemoteAdmin)

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
        messaging_api
        routing_service
        recording_service
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/recorder_remote_admin.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/recorder_remote_admin.xml"
)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/replay_remote_admin.xml"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/replay_remote_admin.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/replay_remote_admin.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/replay_remote_admin.xml"
)

add_custom_target(
    copy_config_files
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/replay_remote_admin.xml"
)

# Main application
add_executable(
    Requester
    "${CMAKE_CURRENT_SOURCE_DIR}/Requester.cxx"
)

set_target_properties(
    Requester
    PROPERTIES CXX_STANDARD 11)

add_dependencies(Requester copy_config_files)

target_link_libraries(
    Requester
    RTIConnextDDS::messaging_cpp2_api
    RTIConnextDDS::recording_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
