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
project (PluggableStorage)

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")

find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

find_package(
    RTICodeGenerator "3.0.0"
    REQUIRED
)

find_library(
    recordingservicelib
    NAMES rtirecordingservice 
    PATHS "${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}")

# Define the subscriber application
add_executable(
    ServiceAsLibExample
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceAsLib.cxx"
)

target_include_directories(
    ServiceAsLibExample
    PUBLIC 
        "${CMAKE_BINARY_DIR}"
        ${CONNEXTDDS_INCLUDE_DIRS}
)

target_link_libraries(
    ServiceAsLibExample
    RTIConnextDDS::cpp2_api
    "{recordingservicelib}"
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
