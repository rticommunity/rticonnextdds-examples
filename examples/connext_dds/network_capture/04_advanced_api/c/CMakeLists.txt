#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project(rtiexamples-builtin-topics)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(RTIConnextDDS
    "7.0.0"
    REQUIRED
    COMPONENTS
        security_plugins
)

if(NOT TARGET RTIConnextDDS::security_plugins)
    message(WARNING "RTIConnextDDS::security_plugins component is missing. Skipping example")
    return()
endif()

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "network_capture"
    LANG "C"
    DEPENDENCIES
        RTIConnextDDS::security_plugins
)

include (ConnextDdsGenerateSecurityArtifacts)
connextdds_generate_security_artifacts()
