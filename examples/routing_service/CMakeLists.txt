#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project(rticonnextdds-examples-routing-service)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

include(ConnextDdsAddExamplesSubdirectories)

# Mongo DB C++ Driver may not be installed, so we don't build it by default
option(CONNEXTDDS_BUILD_MONGODB_EXAMPLE
    "Build the Mongo DB C++ Routing Service example"
    OFF
)
set(routing_service_mongo_db_example_dir)
if(CONNEXTDDS_BUILD_MONGODB_EXAMPLE)
    set(routing_service_mongo_db_example_dir "mongo_db")
endif()

if(NOT DEFINED CONNEXTDDS_ROUTING_SERVICE_EXAMPLES)
    set(CONNEXTDDS_ROUTING_SERVICE_EXAMPLES
        "file_adapter"
        "file_to_dds_configuration"
        "${routing_service_mongo_db_example_dir}"
        "monitoring"
        "remote_admin"
        "shapes_processor"
        "struct_array_transf"
    )
endif()

connextdds_add_examples_subdirectories(
    EXAMPLES
        ${CONNEXTDDS_ROUTING_SERVICE_EXAMPLES}
)
