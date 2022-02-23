# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rticonnextdds-examples-routing-service)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake"
)

include(ConnextDdsAddExamplesSubdirectories)

# Mongo DB C++ Driver may not be installed, so we don't build it by default
option(CONNEXTDDS_BUILD_MONGODB_EXAMPLE
    "Build the Mongo DB C++ Routing Service example"
    OFF
)
if(CONNEXTDDS_BUILD_MONGODB_EXAMPLE)
    set(routing_service_mongo_db_example_dir "mongo_db")
else()
    set(routing_service_mongo_db_example_dir)
endif()

connextdds_add_examples_subdirectories(
    EXAMPLES
        "file_adapter"
        "file_to_dds_configuration"
        ${routing_service_mongo_db_example_dir}
        "monitoring"
        "shapes_processor"
        "struct_array_transf"
)
