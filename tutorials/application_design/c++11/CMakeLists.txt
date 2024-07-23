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
project(connext-tutorial-application-design)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_call_codegen(
    TYPE_PATH "../VehicleModeling.xml"
    LANG "C++11"
    PREFIX "VehicleModeling"
)

connextdds_add_application(
    TARGET "publisher"
    LANG "C++11"
    OUTPUT_NAME "publisher"
    SOURCES
        $<TARGET_OBJECTS:VehicleModeling_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/publisher.cxx"
    DEPENDENCIES
    ${_CONNEXT_DEPENDENCIES}
)

connextdds_add_application(
    TARGET "subscriber"
    LANG "C++11"
    OUTPUT_NAME "subscriber"
    SOURCES
        $<TARGET_OBJECTS:VehicleModeling_CXX11_obj>
        "${CMAKE_CURRENT_SOURCE_DIR}/subscriber.cxx"
    DEPENDENCIES
    ${_CONNEXT_DEPENDENCIES}
)
