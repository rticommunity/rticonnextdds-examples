#
# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-dynamic-data-sequences)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Find the RTI Connext DDS libraries
if(NOT RTIConnextDDS_FOUND)
    find_package(RTIConnextDDS
        "7.0.0"
        REQUIRED
        COMPONENTS
            core
    )
endif()

add_executable(dynamic_data_sequences_c
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_sequences.c"
)

target_link_libraries(dynamic_data_sequences_c
    PRIVATE
        RTIConnextDDS::c_api
)

target_include_directories(dynamic_data_sequences_c
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set_target_properties(dynamic_data_sequences_c
    PROPERTIES
        OUTPUT_NAME "dynamic_data_sequences")

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU")
    set_target_properties(dynamic_data_sequences_c
        PROPERTIES
            LINK_FLAGS -Wl,--no-as-needed)
endif()
