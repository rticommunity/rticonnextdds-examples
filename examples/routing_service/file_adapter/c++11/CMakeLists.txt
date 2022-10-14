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
project(FileAdapterC++11)

if(NOT DEFINED CONNEXTDDS_DIR)
    set(msg 
        "Please set the CMake variable CONNEXTDDS_DIR "
        "to point to your installation of RTI Connext DDS")
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
    "${CMAKE_CURRENT_SOURCE_DIR}/FileAdapter.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/FileConnection.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/FileInputDiscoveryStreamReader.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/FileStreamReader.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/FileStreamWriter.cxx"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(${PROJECT_NAME}
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
)

# To differentiate between debug and release builds
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        DEBUG_POSTFIX "d"
)

# Copy the sample data files into the binary directory
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Input_Circle.csv"
        "${CMAKE_BINARY_DIR}/Input_Circle.csv"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Input_Triangle.csv"
        "${CMAKE_BINARY_DIR}/Input_Triangle.csv"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Input_Square.csv"
        "${CMAKE_BINARY_DIR}/Input_Square.csv"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/ShapeType.xml"
        "${CMAKE_BINARY_DIR}/ShapeType.xml"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/RsFileAdapter.xml"
        "${CMAKE_BINARY_DIR}/RsFileAdapter.xml"
)
