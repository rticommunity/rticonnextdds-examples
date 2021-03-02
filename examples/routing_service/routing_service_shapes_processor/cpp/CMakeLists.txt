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


cmake_minimum_required(VERSION 3.11)
project (ShapesProcessor)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=ON CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

if(CONNEXTDDS_IMPORTED_TARGETS_DEBUG)
    set(msg
        "You have selected to build the library in debug mode. It's recommended "
        "to run the debug version of Routing Service or otherwise you may run "
        "into undefined behavior")
    message(WARNING ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

# Define the library that will provide the Aggregator Processor plugin
set(AGGREGATOR_LIB "shapesaggregator")
add_library(${AGGREGATOR_LIB}
    "${CMAKE_CURRENT_SOURCE_DIR}/ShapesAggregator.cxx")

set(SPLITTER_LIB "shapessplitter")
add_library(${SPLITTER_LIB}
    "${CMAKE_CURRENT_SOURCE_DIR}/ShapesSplitter.cxx")

set(PROCESSORS ${AGGREGATOR_LIB} ${SPLITTER_LIB})

foreach (proc ${PROCESSORS})

    # Set target properties for lang requirement output library name
    set_target_properties(
        ${proc}
        PROPERTIES
            CXX_STANDARD 11
            OUTPUT_NAME_DEBUG ${proc}d
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
        
    target_link_libraries(${proc}
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::routing_service_infrastructure
        RTIConnextDDS::routing_service)
        
    target_include_directories(${proc}
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}")

endforeach()

