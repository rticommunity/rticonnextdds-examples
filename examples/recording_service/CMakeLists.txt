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
