#
# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form
#  only for use with RTI products.  The Software is provided "as is", with no
#  warranty of any type, including any warranty for fitness for any purpose.
#  RTI is under no obligation to maintain or support the Software.  RTI shall
#  not be liable for any incidental or consequential damages arising out of the
#  use or inability to use the software.


cmake_minimum_required(VERSION 3.12)
project (MsqlAdapter)

if(DEFINED ENV{NDDSHOME})
	# NDDSHOME is set
else()
    set(msg
        "NDDSHOME environment variable must be set")
    message(FATAL_ERROR ${msg})
endif()

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=1 CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

if(NOT CONNEXTDDS_ARCH)
    set(msg
        "CONNEXTDDS_ARCH must be defined. For example, -DCONNEXTDDS_ARCH=x64Win64VS2017")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "$ENV{NDDSHOME}/resource/cmake"
)

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.1"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

# Compiler Definitions 
add_compile_definitions(RTI_RS_USE_LOG) # if defined use logging

# Define the library that will provide the Adapter plugin
add_library(msqladapter
    "${CMAKE_CURRENT_SOURCE_DIR}/../RsAdapterPlugin.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../RsConnection.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../RsStreamWriter.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/../RsStreamReader.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/../RsInputDiscoveryStreamReader.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ConnectorBase.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ConnectorMsqlDb.cxx")

# Set target properties for lang requirement output library name
set_target_properties(msqladapter
    PROPERTIES
        CXX_STANDARD 11
        OUTPUT_NAME_DEBUG msqladapterd
        LIBRARY_OUTPUT_DIRECTORY "${output_dir}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
        ARCHIVE_OUTPUT_DIRECTORY "${output_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
)

target_link_libraries(msqladapter
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::routing_service_infrastructure
	odbc32.lib)

target_include_directories(msqladapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}")