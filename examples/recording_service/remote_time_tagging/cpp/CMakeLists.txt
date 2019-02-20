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
project (RecorderRemoteTimeTagging)

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")

find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        messaging_api
)

find_package(
    RTICodeGenerator "3.0.0"
    REQUIRED
)

set(SERVICE_COMMON_GENERATED_CODE
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceCommon.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceCommonPlugin.cxx"
)

set(SERVICE_ADMIN_GENERATED_CODE
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceAdmin.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/ServiceAdminPlugin.cxx"
)

set(RECORDER_TYPES_GENERATED_CODE
    "${CMAKE_CURRENT_SOURCE_DIR}/RecordingServiceTypes.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/RecordingServiceTypesPlugin.cxx"
)

# Generate type plug-in files for the remote service administration
add_custom_command(
    OUTPUT ${SERVICE_COMMON_GENERATED_CODE}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/ServiceCommon.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_SOURCE_DIR}" 
            "${CONNEXTDDS_DIR}/resource/idl/ServiceCommon.idl"
)

add_custom_command(
    OUTPUT ${SERVICE_ADMIN_GENERATED_CODE}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/ServiceAdmin.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_SOURCE_DIR}" 
            "${CONNEXTDDS_DIR}/resource/idl/ServiceAdmin.idl"
)

add_custom_command(
    OUTPUT ${RECORDER_TYPES_GENERATED_CODE}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/RecordingServiceTypes.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_SOURCE_DIR}" 
            "${CONNEXTDDS_DIR}/resource/idl/RecordingServiceTypes.idl"
)

add_custom_target(
    generated_code 
    DEPENDS ${SERVICE_COMMON_GENERATED_CODE} 
            ${SERVICE_ADMIN_GENERATED_CODE}
            ${RECORDER_TYPES_GENERATED_CODE}
)

# Main application
add_executable(
    Requester
    "${CMAKE_CURRENT_SOURCE_DIR}/Requester.cxx"
    ${SERVICE_COMMON_GENERATED_CODE}
    ${SERVICE_ADMIN_GENERATED_CODE}
    ${RECORDER_TYPES_GENERATED_CODE}
)

add_dependencies(Requester generated_code)

target_link_libraries(
    Requester
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::messaging_cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
