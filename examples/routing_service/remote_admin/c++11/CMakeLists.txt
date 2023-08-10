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
project (RoutingServiceRemoteAdminC++11)

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
        messaging_api
        routing_service
)

# Flags for code generation
set(CODEGEN_FLAGS UNBOUNDED)
# Include the CodegenRTIConnextDDS cmake module
include(ConnextDdsCodegen)
# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_rtiddsgen_run(
    IDL_FILE
      "${CONNEXTDDS_DIR}/resource/idl/ServiceCommon.idl"
    LANG
      "C++11"
    OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}/generated"
    ${CODEGEN_FLAGS}
)

connextdds_rtiddsgen_run(
    IDL_FILE
      "${CONNEXTDDS_DIR}/resource/idl/ServiceAdmin.idl"
    LANG
      "C++11"
    OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}/generated"
    ${CODEGEN_FLAGS}
)

# Main application
add_executable(${PROJECT_NAME}
    "${ServiceCommon_CXX11_SOURCES}"
    "${ServiceAdmin_CXX11_SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ArgumentsParser.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Requester.cxx"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CONNEXTDDS_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES CXX_STANDARD 11)

target_link_libraries(${PROJECT_NAME}
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::messaging_cpp2_api
    RTIConnextDDS::routing_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_QOS_"
    DEPENDANT_TARGET ${PROJECT_NAME}
    FILENAME "../USER_QOS_PROFILES.xml"
)
