# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-using-qos-profiles)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "profiles"
    LANG "C++"
)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/my_custom_qos_profiles.xml"
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/my_custom_qos_profiles.xml"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Copying my_custom_qos_profiles.xml"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/my_custom_qos_profiles.xml"
    VERBATIM)

add_custom_target(using_qos_profiles_custom_qos_cpp
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/my_custom_qos_profiles.xml")

add_dependencies(using_qos_profiles_publisher_cpp
    using_qos_profiles_custom_qos_cpp)
add_dependencies(using_qos_profiles_subscriber_cpp
    using_qos_profiles_custom_qos_cpp)
