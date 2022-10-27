# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rticonnextdds-examples-recording-service)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

include(ConnextDdsAddExamplesSubdirectories)

if(NOT DEFINED CONNEXTDDS_RECORDING_SERVICE_EXAMPLES)
    set(CONNEXTDDS_RECORDING_SERVICE_EXAMPLES
        "pluggable_storage"
        "service_admin"
        "service_as_lib"
    )
endif()

connextdds_add_examples_subdirectories(
    EXAMPLES
        ${CONNEXTDDS_RECORDING_SERVICE_EXAMPLES}
)
