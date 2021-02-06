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
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

add_library(
    fileadapter
        "${CMAKE_CURRENT_SOURCE_DIR}/FileAdapter.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileConnection.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileInputDiscoveryStreamReader.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileStreamReader.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileStreamWriter.cxx"
)

set_property(TARGET fileadapter PROPERTY CXX_STANDARD 11)
set_property(TARGET fileadapter PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(
    fileadapter
        RTIConnextDDS::routing_service_infrastructure
        RTIConnextDDS::cpp2_api
        ${CONNEXTDDS_EXTERNAL_LIBS}
)

# To differentiate between debug and release builds
set_target_properties(fileadapter
    PROPERTIES
        OUTPUT_NAME_DEBUG fileadapterd
)

# Copy the sample data files into the binary directory
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Input_Circle.csv"
    "${CMAKE_BINARY_DIR}/Input_Circle.csv"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Input_Square.csv"
    "${CMAKE_BINARY_DIR}/Input_Square.csv"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Input_Triangle.csv"
    "${CMAKE_BINARY_DIR}/Input_Triangle.csv"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ShapeType.xml"
    "${CMAKE_BINARY_DIR}/ShapeType.xml"
    COPYONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/RsFileAdapter.xml"
    "${CMAKE_BINARY_DIR}/RsFileAdapter.xml"
    COPYONLY
)
