#
# (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project(connext-tutorial-publish-subscribe)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

find_package(RTIConnextDDS
    "7.3.0"
    REQUIRED
    COMPONENTS
        core
        messaging_api
)

connextdds_call_codegen(
    IDL "robot"
    LANG "C++11"
    PREFIX "robot"
)

connextdds_add_application(
    TARGET "service"
    LANG "C++11"
    OUTPUT_NAME "robot_service"
    NO_REQUIRE_QOS
    SOURCES
        $<TARGET_OBJECTS:robot_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/robot_service.cxx"
    DEPENDENCIES
        ${_CONNEXT_DEPENDENCIES}
        RTIConnextDDS::messaging_cpp2_api
)

connextdds_add_application(
    TARGET "client"
    LANG "C++11"
    OUTPUT_NAME "robot_client"
    NO_REQUIRE_QOS
    SOURCES
        $<TARGET_OBJECTS:robot_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/robot_client.cxx"
    DEPENDENCIES
        ${_CONNEXT_DEPENDENCIES}
        RTIConnextDDS::messaging_cpp2_api
)
