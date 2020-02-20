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
project (LevelDbIntegration)

set(CMAKE_DEBUG_POSTFIX "d")

# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")

set(BUILD_SHARED_LIBS "ON")

find_package(
    RTIConnextDDS "6.0.1"
    REQUIRED
    COMPONENTS
        core
        routing_service
)

set(LEVELDB_LIB_NAME "leveldb")

find_library(
    leveldb_lib
    NAMES ${LEVELDB_LIB_NAME}
    PATHS "${LEVELDB_DIR}"
)

# Disable RTTI.
string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

# Enable sanitizers
if(ENABLE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fsanitize=leak -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address  -fsanitize=leak")
    set(CMAKE_EXE_LINKER_FLAGS 
        "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=leak")
    set(CMAKE_MODULE_LINKER_FLAGS 
        "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address -fsanitize=leak")
endif()


# Create the directory that's going to contain the generated code files
add_custom_target(
    generated_code_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(REDUCED_DCPSPUB_CPP_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypes.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypesPlugin.cxx"
)

# Generate type plug-in files for the storage type
add_custom_command(
    OUTPUT ${REDUCED_DCPSPUB_CPP_SOURCES}
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/LevelDb_RecorderTypes.idl"
    COMMAND ${RTICODEGEN} -language C++11 -update typefiles -unboundedSupport
            -platform ${CONNEXTDDS_ARCH} -d "${CMAKE_BINARY_DIR}/generated" 
            "${CMAKE_CURRENT_SOURCE_DIR}/LevelDb_RecorderTypes.idl"
)

add_custom_target(
    generated_code 
    DEPENDS 
        ${REDUCED_DCPSPUB_CPP_SOURCES}
        generated_code_dir
)

# Define the library that will provide the storage writer plugin
add_library(
    LevelDbWriter SHARED
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypes.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypesPlugin.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/LevelDbWriter.cxx"
)

set_target_properties(
    LevelDbWriter 
    PROPERTIES CXX_STANDARD 11
)

target_link_libraries(
    LevelDbWriter
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
    "${leveldb_lib}"
)

target_include_directories(
    LevelDbWriter
    PUBLIC 
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
        "${LEVELDB_INCLUDE}"
)

add_dependencies(LevelDbWriter generated_code)

# Define the library that will provide the storage reader plugin
add_library(
    LevelDbReader SHARED
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypes.cxx"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/LevelDb_RecorderTypesPlugin.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/LevelDbReader.cxx"
)

set_target_properties(
    LevelDbReader 
    PROPERTIES CXX_STANDARD 11
)

target_link_libraries(
    LevelDbReader
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
    "${leveldb_lib}"
)

target_include_directories(
    LevelDbReader
    PUBLIC 
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
        "${LEVELDB_INCLUDE}"
)

add_dependencies(LevelDbReader generated_code)

# Define the test executable
add_executable(
    tester 
    "${CMAKE_CURRENT_SOURCE_DIR}/Tests.cxx")

target_link_libraries(
    tester 
    LevelDbWriter 
    LevelDbReader
    RTIConnextDDS::routing_service_infrastructure
    RTIConnextDDS::cpp2_api
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

