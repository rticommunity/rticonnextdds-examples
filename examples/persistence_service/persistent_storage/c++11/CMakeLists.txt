# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission. Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

cmake_minimum_required(VERSION 3.12)
project(rticonnextdds-examples-persistence-service)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "hello_world"
    LANG "C++11"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/PersistenceServiceConfig.xml"
    "${CMAKE_CURRENT_BINARY_DIR}/PersistenceServiceConfig.xml"
    COPYONLY
)
