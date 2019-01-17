# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.5)
project(rtiexamples-zero-copy)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "zero_copy"
    LANG "C++"
    NO_REQUIRE_QOS
)

target_include_directories(zero_copy_publisher_cpp
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

target_include_directories(zero_copy_publisher_cpp
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)