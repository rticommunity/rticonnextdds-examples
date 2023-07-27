# (c) 2023 Copyright, Real-Time Innovations, Inc. All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.

cmake_minimum_required(VERSION 3.11)
project(rtiexamples-batching)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)

include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(RTIConnextDDS
    "7.0.0"
    REQUIRED
    COMPONENTS
        core
        messaging_api
)

include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "Inventory"
    LANG "C++11"
    SUB_NAME "service"
    PUB_NAME "client"
    DEPENDENCIES RTIConnextDDS::messaging_cpp2_api
)