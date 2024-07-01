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
project(connext-tutorial-last-value-cache)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_call_codegen(
    IDL "home_automation"
    LANG "C++11"
    PREFIX "home_automation"
)

connextdds_add_application(
    TARGET "publisher"
    LANG "C++11"
    OUTPUT_NAME "home_automation_publisher"
    SOURCES
        $<TARGET_OBJECTS:home_automation_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/home_automation_publisher.cxx"
    DEPENDENCIES
    ${_CONNEXT_DEPENDENCIES}
)

connextdds_add_application(
    TARGET "subscriber"
    LANG "C++11"
    OUTPUT_NAME "home_automation_subscriber"
    SOURCES
        $<TARGET_OBJECTS:home_automation_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/home_automation_subscriber.cxx"
    DEPENDENCIES
    ${_CONNEXT_DEPENDENCIES}
)

connextdds_add_application(
    TARGET "audit"
    LANG "C++11"
    OUTPUT_NAME "home_automation_audit"
    SOURCES
        $<TARGET_OBJECTS:home_automation_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/home_automation_audit.cxx"
    DEPENDENCIES
    ${_CONNEXT_DEPENDENCIES}
)
