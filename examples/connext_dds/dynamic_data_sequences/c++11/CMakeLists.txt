# (c) 2018 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-dynamic-data-sequences)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Find the RTI Connext DDS libraries
if(NOT RTIConnextDDS_FOUND)
    find_package(RTIConnextDDS
        "6.1.1"
        REQUIRED
        COMPONENTS
            core
    )
endif()

add_executable(dynamic_data_sequences_cxx2
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_sequences.cxx"
)

target_link_libraries(dynamic_data_sequences_cxx2
    PUBLIC
        RTIConnextDDS::cpp2_api
)

target_include_directories(dynamic_data_sequences_cxx2
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set_target_properties(dynamic_data_sequences_cxx2
    PROPERTIES
        OUTPUT_NAME "dynamic_data_sequences")

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set_target_properties(dynamic_data_sequences_cxx2
        PROPERTIES
            LINK_FLAGS -Wl,--no-as-needed)
endif()
