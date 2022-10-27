# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-crypto-algorithms)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(RTIConnextDDS
    "6.1.1"
    REQUIRED
    COMPONENTS
        core
        security_plugins
)

# Include ConnextDdsAddExample.cmake from resources/cmake
include(ConnextDdsAddExample)

connextdds_add_example(
    IDL "crypto_algorithms"
    LANG "C"
    DEPENDENCIES
        RTIConnextDDS::security_plugins
)

include (ConnextDdsGenerateSecurityArtifacts)
connextdds_generate_security_artifacts(
    CA "p256" "p384"
)