#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project(FileAdapterC)

if(NOT DEFINED CONNEXTDDS_DIR)
    set(msg
        "Please set the CMake variable CONNEXTDDS_DIR "
        "to point to your installation of RTI Connext DDS")
    message(FATAL_ERROR ${msg})
endif()

if (NOT UNIX)
    set(msg
        "This example uses the pthreads library "
        "which is only available on UNIX-like systems")
    message(FATAL_ERROR ${msg})
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

add_library(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/directory_reading.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/line_conversion.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/file_adapter.c"
)

target_link_libraries(${PROJECT_NAME}
    RTIConnextDDS::routing_service_infrastructure
)

# To differentiate between debug and release builds
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "d"
)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/file_bridge.xml"
        "${CMAKE_BINARY_DIR}/file_bridge.xml"
)
