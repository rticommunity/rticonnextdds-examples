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
project(ServiceAsLibC++11)

if(NOT BUILD_SHARED_LIBS)
    set(msg
        "Static linking is not supported for this example. You can build it "
        "using the -DBUILD_SHARED_LIBS=ON CMake parameter")
    message(FATAL_ERROR ${msg})
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(msg
        "Not specifying CMAKE_BUILD_TYPE can lead to undefined behaviour and "
        "is not supported for this example. Please provide a valid value for "
        "the CMAKE_BUILD_TYPE parameter")
    message(FATAL_ERROR ${msg})
endif()

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

include(ConnextDdsAddExample)

find_package(
    RTIConnextDDS "6.1.1"
    REQUIRED
    COMPONENTS
        core
        routing_service
        recording_service
)

# Define the subscriber application
add_executable(
    ServiceAsLibExample
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceAsLibExample.cxx"
)

target_compile_features(ServiceAsLibExample PRIVATE cxx_std_11)

target_include_directories(
    ServiceAsLibExample
    PUBLIC
        ${CONNEXTDDS_INCLUDE_DIRS}
)

target_link_libraries(
    ServiceAsLibExample
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::recording_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_RecorderXml_"
    DEPENDANT_TARGET ServiceAsLibExample
    FILENAME "recorder_config.xml"
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_ReplayXml_"
    DEPENDANT_TARGET ServiceAsLibExample
    FILENAME "replay_config.xml"
)
