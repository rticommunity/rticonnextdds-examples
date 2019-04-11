# (c) 2018 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.7)
project(rtiexamples-content-filtered-topic-str)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "cft"
    LANG "C++03"
    CODEGEN_ARGS
        LEGACY_PLUGIN
)
