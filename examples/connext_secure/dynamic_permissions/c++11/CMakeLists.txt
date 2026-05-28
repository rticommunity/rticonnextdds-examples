#
# (c) 2025 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project(rtiexamples-dynamic-permissions)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
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
    IDL "dynamic_permissions"
    LANG "C++11"
)

include (ConnextDdsGenerateSecurityArtifacts)
connextdds_generate_security_artifacts()

# Do a copy of the original subscriber's Permissions Document, but with the
# validity modified so that it expires in 1 minute.
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/Permissions2_expiring.xml"
    COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/Permissions2.xml"
            -DOUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/Permissions2_expiring.xml"
            -P ${CMAKE_SOURCE_DIR}/modify_permissions.cmake
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/Permissions2.xml"
)

# Sign the modified Permissions Document
connextdds_openssl_smime_sign(
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/Permissions2_expiring.xml"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/signed/signed_Permissions2_expiring.p7s"
    SIGNER_CERTIFICATE "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/certs/ca_cert.pem"
    PRIVATE_KEY_FILE "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/certs/ca_key.pem"
)

# Create a Permissions Document that is about to expire
add_custom_target(createExpiringPermissions
    ALL
    DEPENDS
        dynamic_permissions_securityArtifacts
        "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01/xml/signed/signed_Permissions2_expiring.p7s")
