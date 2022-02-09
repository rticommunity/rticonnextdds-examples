# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-recording-service)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake"
)

include(ConnextDdsAddExamplesSubdirectories)

connextdds_add_examples_subdirectories(
    EXAMPLES
        "pluggable_storage"
        # "service_admin"
        # "service_as_lib"
)
