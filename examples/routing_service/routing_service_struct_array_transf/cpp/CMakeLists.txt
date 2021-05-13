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
project (StructArrayTransformation)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=ON CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

if(CONNEXTDDS_IMPORTED_TARGETS_DEBUG)
    set(msg
        "You have selected to build the library in debug mode. It's "
        "recommended to run the debug version of Routing Service or "
        "otherwise you may run into undefined behavior"
    )
    message(WARNING ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)
find_package(
    RTIConnextDDS "6.1.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

# Generate type plug-in files for conversion types
add_custom_command(
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/SensorData.idl"
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/SensorData.cxx"
        "${CMAKE_CURRENT_BINARY_DIR}/SensorDataPlugin.cxx"
        "${CMAKE_CURRENT_BINARY_DIR}/SensorData.hpp"
        "${CMAKE_CURRENT_BINARY_DIR}/SensorDataPlugin.hpp"
    COMMAND
        ${RTICODEGEN}
            -language C++11
            -stl
            -replace -d ${CMAKE_CURRENT_BINARY_DIR}
            "${CMAKE_CURRENT_SOURCE_DIR}/SensorData.idl"
)

# Define the library that will provide the  Transformation plugin
add_library(structarraytransf SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/StructArrayTransformation.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorData.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorDataPlugin.cxx"
)

target_include_directories(structarraytransf
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(structarraytransf
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX "d"
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

target_link_libraries(structarraytransf
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
)


# Define the Publisher application
add_executable(SensorAttributesCollectionPublisher
    "${CMAKE_CURRENT_SOURCE_DIR}/SensorAttributesCollectionPublisher.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorData.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorDataPlugin.cxx"
)

target_include_directories(SensorAttributesCollectionPublisher
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(SensorAttributesCollectionPublisher
    RTIConnextDDS::cpp2_api
)

set_target_properties(SensorAttributesCollectionPublisher
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX "d"
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

# Define the Subscriber application
add_executable(SensorDataSubscriber
    "${CMAKE_CURRENT_SOURCE_DIR}/SensorDataSubscriber.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorData.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/SensorDataPlugin.cxx"
)

target_include_directories(SensorDataSubscriber
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(SensorDataSubscriber
    RTIConnextDDS::cpp2_api
)

set_target_properties(SensorDataSubscriber
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        DEBUG_POSTFIX "d"
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
