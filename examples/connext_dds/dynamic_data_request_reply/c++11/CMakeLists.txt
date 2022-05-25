# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-dynamic-data-request_reply)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Find the RTI Connext DDS libraries
find_package(RTIConnextDDS
    "6.1.1"
    REQUIRED
    COMPONENTS
        core
        messaging_api
)

add_executable(prime_number_requester_cxx2
    "${CMAKE_CURRENT_SOURCE_DIR}/PrimeNumberRequester.cxx"
)

add_executable(prime_number_replier_cxx2
    "${CMAKE_CURRENT_SOURCE_DIR}/PrimeNumberReplier.cxx"
)

target_link_libraries(prime_number_requester_cxx2
    PRIVATE
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::messaging_cpp2_api
)

target_link_libraries(prime_number_replier_cxx2
    PRIVATE
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::messaging_cpp2_api
)

target_include_directories(prime_number_requester_cxx2
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

target_include_directories(prime_number_replier_cxx2
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set_target_properties(prime_number_requester_cxx2
    PROPERTIES
        OUTPUT_NAME "PrimeNumberRequester")

set_target_properties(prime_number_replier_cxx2
    PROPERTIES
        OUTPUT_NAME "PrimeNumberReplier")

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set_target_properties(prime_number_requester_cxx2
        PROPERTIES
            LINK_FLAGS -Wl,--no-as-needed)
    set_target_properties(prime_number_replier_cxx2
        PROPERTIES
            LINK_FLAGS -Wl,--no-as-needed)
endif()
