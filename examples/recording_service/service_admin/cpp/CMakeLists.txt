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
project (RecorderRemoteAdmin)

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

# Create the directory that's going to contain the generated code files
add_custom_target(
    generated_code_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml
    COMMAND ${CMAKE_COMMAND} -E copy 
        "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
    COMMAND ${CMAKE_COMMAND} -E copy 
        "${CMAKE_CURRENT_SOURCE_DIR}/recorder_remote_admin.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/recorder_remote_admin.xml"
)

add_custom_target(
    copy_config_files
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/recorder_remote_admin.xml"
)

set(SERVICE_COMMON_CPP_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ServiceCommon.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ServiceCommonPlugin.cxx"
)

set(SERVICE_ADMIN_CPP_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ServiceAdmin.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/ServiceAdminPlugin.cxx"
)

set(RECORDER_TYPES_CPP_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/RecordingServiceTypes.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/RecordingServiceTypesPlugin.cxx"
)

# Generate type plug-in files for the remote service administration
add_custom_command(
    OUTPUT ${SERVICE_COMMON_CPP_SOURCES}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/ServiceCommon.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_BINARY_DIR}/generated" 
            "${CONNEXTDDS_DIR}/resource/idl/ServiceCommon.idl"
)

add_custom_command(
    OUTPUT ${SERVICE_ADMIN_CPP_SOURCES}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/ServiceAdmin.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_BINARY_DIR}/generated" 
            "${CONNEXTDDS_DIR}/resource/idl/ServiceAdmin.idl"
)

add_custom_command(
    OUTPUT ${RECORDER_TYPES_CPP_SOURCES}
    MAIN_DEPENDENCY "${CONNEXTDDS_DIR}/resource/idl/RecordingServiceTypes.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_CURRENT_BINARY_DIR}/generated" 
            "${CONNEXTDDS_DIR}/resource/idl/RecordingServiceTypes.idl"
)

add_custom_target(
    generated_code 
    DEPENDS ${SERVICE_COMMON_CPP_SOURCES} 
            ${SERVICE_ADMIN_CPP_SOURCES}
            ${RECORDER_TYPES_CPP_SOURCES}
            generated_code_dir
)

# Main application
add_executable(
    Requester
    "${CMAKE_CURRENT_SOURCE_DIR}/Requester.cxx"
    ${SERVICE_COMMON_CPP_SOURCES}
    ${SERVICE_ADMIN_CPP_SOURCES}
    ${RECORDER_TYPES_CPP_SOURCES}
)

set_target_properties(
    Requester 
    PROPERTIES CXX_STANDARD 11)

add_dependencies(Requester copy_config_files generated_code)

target_include_directories(
    Requester
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(
    Requester
    RTIConnextDDS::messaging_cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)
