# (c) 2021 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-real-time-wan-transport)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "real_time_wan_transport"
    LANG "C++"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CLOUD_DISCOVERY_SERVICE.xml"
    "${CMAKE_BINARY_DIR}/real_time_wan_transport/c++/CLOUD_DISCOVERY_SERVICE.xml"
    COPYONLY)
    
include (ConnextDdsGenerateSecurityArtifacts)
connextdds_generate_security_artifacts()