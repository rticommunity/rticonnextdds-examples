#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-using-qos-profiles)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "profiles"
    LANG "C"
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

add_custom_target(using_qos_profiles_custom_qos_c
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/my_custom_qos_profiles.xml")

add_dependencies(using_qos_profiles_publisher_c
    using_qos_profiles_custom_qos_c)
add_dependencies(using_qos_profiles_subscriber_c
    using_qos_profiles_custom_qos_c)
