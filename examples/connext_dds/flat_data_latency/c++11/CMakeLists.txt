# (c) 2021 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-flat-data-latency)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "CameraImage"
    LANG "C++11"
)

target_link_libraries(flat_data_latency_publisher_cpp2
    PRIVATE
        RTIConnextDDS::metp
)

target_link_libraries(flat_data_latency_subscriber_cpp2
    PRIVATE
        RTIConnextDDS::metp
)
