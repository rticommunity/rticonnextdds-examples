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
project (RoutingServiceRemoteAdmin)

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
        messaging_api
        routing_service
)

# Flags for code generation
set(CODEGEN_FLAGS UNBOUNDED)
set(IDL_GEN_CODE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build/${RTI_ARCH}/generated/")
# Include the CodegenRTIConnextDDS cmake module
include(ConnextDdsCodegen)

connextdds_rtiddsgen_run(
    IDL_FILE
      "$ENV{NDDSHOME}/resource/idl/ServiceCommon.idl"
    LANG
      "C++11"
    OUTPUT_DIRECTORY
      "${IDL_GEN_CODE_OUTPUT_DIR}"
    INCLUDE_DIRS
      "${CMAKE_SOURCE_DIR}"
    ${CODEGEN_FLAGS}
)

connextdds_rtiddsgen_run(
    IDL_FILE
      "$ENV{NDDSHOME}/resource/idl/ServiceAdmin.idl"
    LANG
      "C++11"
    OUTPUT_DIRECTORY
      "${IDL_GEN_CODE_OUTPUT_DIR}"
    INCLUDE_DIRS
      "${CMAKE_SOURCE_DIR}"
    ${CODEGEN_FLAGS}
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/USER_QOS_PROFILES.xml
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/../USER_QOS_PROFILES.xml"
        "${CMAKE_BINARY_DIR}/USER_QOS_PROFILES.xml"
    DEPENDS "${CMAKE_SOURCE_DIR}/../USER_QOS_PROFILES.xml"
)

add_custom_target(
    copy_config_files
    DEPENDS
        "${CMAKE_BINARY_DIR}/USER_QOS_PROFILES.xml"
)

# Main application
add_executable(
    Requester
    "${ServiceCommon_CXX11_SOURCES}"
    "${ServiceAdmin_CXX11_SOURCES}"
    "${CMAKE_SOURCE_DIR}/src/ArgumentsParser.cxx"
    "${CMAKE_SOURCE_DIR}/src/Requester.cxx"
)

target_include_directories(Requester
  PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CONNEXTDDS_INCLUDE_DIRS}"
    "${IDL_GEN_CODE_OUTPUT_DIR}"
)

set_target_properties(
    Requester
    PROPERTIES CXX_STANDARD 11)

add_dependencies(Requester copy_config_files)

target_link_libraries(
    Requester
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::messaging_cpp2_api
    RTIConnextDDS::routing_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
