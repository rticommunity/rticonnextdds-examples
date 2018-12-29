# (c) 2018 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.5)
project(rtiexamples-custom-content-filter)

find_package(Git REQUIRED)

# Add the resources folder to the CMAKE_MODULE_PATH
if(NOT RTI_CMAKE_RESOURCES)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
        OUTPUT_VARIABLE repository_root
        ERROR_VARIABLE command_error
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE return_value
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT return_value EQUAL 0)
        set(error
            "It was not possible to find the CMake resources folder needed to "
            "run this script. The folder is provided with the RTI Connext "
            "examples repository. You can point to it providing the "
            "-DRTI_CMAKE_RESOURCES= options to CMake")
        message(FATAL_ERROR ${error})
    endif()

    set(RTI_CMAKE_RESOURCES "${repository_root}/resources/cmake")

endif()

if(NOT EXISTS "${RTI_CMAKE_RESOURCES}")
    set(error
        "The directory ${RTI_CMAKE_RESOURCES} doesn't exists. Please, check "
        "that the repository was properly cloned or provide the path to the "
        "resources/cmake folder")
    message(FATAL_ERROR ${error})
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${RTI_CMAKE_RESOURCES}")

# Find the RTI Connext DDS libraries
find_package(RTIConnextDDS
    "5.3.0"
    REQUIRED
    COMPONENTS
        core)

# Generate source code from the IDL file
include(ConnextDdsCodegen)

connextdds_rtiddsgen_run(
    IDL_FILE
        "${CMAKE_SOURCE_DIR}/ccf.idl"
    OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/src"
    LANG C++)

# Add the target for the publisher
add_executable(ccf_publisher
    "${CMAKE_SOURCE_DIR}/ccf_publisher.cxx"
    ${ccf_CXX_SOURCES})

target_link_libraries(ccf_publisher
    PUBLIC
        RTIConnextDDS::cpp_api)

target_include_directories(ccf_publisher
    PRIVATE
        "${CMAKE_BINARY_DIR}/src")

# Add the target for the subscriber
add_executable(ccf_subscriber
    "${CMAKE_SOURCE_DIR}/ccf_subscriber.cxx"
    ${ccf_CXX_SOURCES})

target_link_libraries(ccf_subscriber
    PUBLIC
        RTIConnextDDS::cpp_api)

target_include_directories(ccf_subscriber
    PRIVATE
        "${CMAKE_BINARY_DIR}/src")
