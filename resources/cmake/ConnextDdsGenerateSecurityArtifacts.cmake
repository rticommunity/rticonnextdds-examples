# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _connextdds_generate_security_artifacts:

ConnextDdsGenerateSecurityArtifacts
-----------------------------------

Generate the artifacts needed for secure examples.

``connextdds_generate_security_artifacts()``

It is based on:
 - The OpenSSL configuration files in the `resources/security/<ca>` directory.
 - The Governance and Permissions files in the `resources/xml` directory.

So far this means:

 - One ECDSA CA and two peers.
 - RTPS and metadata protection kind set to ENCRYPT.
 - Publishing and subscribing to any domain and topic is allowed, only for those
   peers.

#]]

include(UseOpenSSL)

function(connextdds_generate_security_artifacts)
    # ##########################################################################
    # Parse the function arguments.
    # ##########################################################################
    set(options)
    set(single_args)
    set(multi_args CA)

    cmake_parse_arguments(
        ARGS
        "${options}"
        "${single_args}"
        "${multi_args}"
        ${ARGN}
    )

    if (NOT ARGS_CA)
        set(ARGS_CA "p256")
    endif()

    # ##########################################################################
    # Set up initial variables.
    # ##########################################################################
    # CONNEXTDDS_RESOURCE_DIR: Directory where the source files used to generate
    # the security artifacts are.
    #
    # These lines rely on the fact that every example sets
    # CMAKE_CURRENT_SOURCE_DIR so that its last item points to the cmake
    # resource directory.
    # CONNEXTDDS_RESOURCE_DIR must be set to run OpenSSL functions.
    list(GET CMAKE_MODULE_PATH -1 CONNEXTDDS_RESOURCE_DIR)
    set(CONNEXTDDS_RESOURCE_DIR "${CONNEXTDDS_RESOURCE_DIR}/..")

    # We will store the ouput artifacts in this folder.
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/security")

    # This variable will be populated when generating the CAs.
    set(dependencies)

    # Set configuration options for the certificates.
    set(expiration_days 3650)  # Almost 10 years.

    # ##########################################################################
    # ECDSA P256.
    # ##########################################################################
    if("p256" IN_LIST ARGS_CA)
        _connextdds_generate_security_artifacts_for_ecdsa(
            CA_DIRECTORY "ecdsa01_p256"
            RESOURCE_DIRECTORY "${CONNEXTDDS_RESOURCE_DIR}/security"
            OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa01_p256"
            ECPARAM_NAME "prime256v1"
            DIGEST "SHA256"
            DAYS ${expiration_days}
        )
    endif()

    if("p384" IN_LIST ARGS_CA)
        _connextdds_generate_security_artifacts_for_ecdsa(
            CA_DIRECTORY "ecdsa02_p384"
            RESOURCE_DIRECTORY "${CONNEXTDDS_RESOURCE_DIR}/security"
            OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/security/ecdsa02_p384"
            ECPARAM_NAME "secp384r1"
            DIGEST "SHA384"
            DAYS ${expiration_days}
        )
    endif()

    # ##########################################################################
    # Add custom target.
    # ##########################################################################
    # We prefix the target with the name of the example because it must be
    # unique.
    get_filename_component(
        folder_name
        "${CMAKE_CURRENT_SOURCE_DIR}"
        DIRECTORY)
    get_filename_component(
        folder_name
        "${folder_name}"
        NAME)
    add_custom_target("${folder_name}_securityArtifacts"
        ALL
        DEPENDS
            ${dependencies}
    )
endfunction()

function(_connextdds_generate_security_artifacts_for_ecdsa)
    # ##########################################################################
    # Parse the function arguments.
    # ##########################################################################
    set(options)
    set(single_args
        CA_DIRECTORY
        RESOURCE_DIRECTORY
        OUTPUT_DIRECTORY
        ECPARAM_NAME
        DIGEST
        DAYS
    )
    set(multi_args)

    cmake_parse_arguments(
        ARGS
        "${options}"
        "${single_args}"
        "${multi_args}"
        ${ARGN}
    )

    # ##########################################################################
    # Directory structure for the generated security artifacts.
    # <example/<CMAKE_CURRENT_BINARY_DIR>/security/<ca>
    #   |- certs/
    #   |- temporary_files/
    #   |- xml/
    #   |   |- signed/
    #
    # These variables will be used when generating the security artifacts.
    # ##########################################################################
    # certs. Output OpenSSL files. This must match the value in ca.cnf.
    set(certificates_output_dir "${ARGS_OUTPUT_DIRECTORY}/certs")
    # xml/signed. This is where the result of signing the xml files will be.
    set(signed_xml_output_dir "${ARGS_OUTPUT_DIRECTORY}/xml/signed")
    # temporary_files. This must match the value in ca.cnf.
    set(openssl_temporary_dir "${ARGS_OUTPUT_DIRECTORY}/temporary_files")

    # These are the files that we copied from ARGS_RESOURCE_DIRECTORY.
    # They are the .cnf openssl configuration files that will be used to
    # generate the certificates, and the XML Governance/Permission Documents
    # that will be signed.
    set(ca_config_file "${ARGS_OUTPUT_DIRECTORY}/ca.cnf")
    set(peer1_config_file "${ARGS_OUTPUT_DIRECTORY}/peer01.cnf")
    set(peer2_config_file "${ARGS_OUTPUT_DIRECTORY}/peer02.cnf")
    set(xmls_name Governance Permissions1 Permissions2)

    set(artifacts_input_files)
    list(APPEND artifacts_input_files
        "${ca_config_file}"
        "${peer1_config_file}"
        "${peer2_config_file}"
    )
    foreach(xml ${xmls_name})
        list(APPEND artifacts_input_files
            "${ARGS_OUTPUT_DIRECTORY}/xml/${xml}.xml"
        )
    endforeach()
    set(dependencies ${dependencies} ${artifacts_input_files} PARENT_SCOPE)

    # ##########################################################################
    # Copy the source files from the resource folder common for all examples
    # (ARGS_RESOURCE_DIRECTORY) to the binary folder for the particular example.
    # ##########################################################################
    add_custom_command(
        OUTPUT ${artifacts_input_files}
        PRE_BUILD
        COMMENT "Copying security resources to the example's binary directory"
        COMMAND # Copy config files from the repository resources.
            ${CMAKE_COMMAND} -E copy_directory
                    "${ARGS_RESOURCE_DIRECTORY}/${ARGS_CA_DIRECTORY}"
                    "${ARGS_OUTPUT_DIRECTORY}"
        COMMAND # xml. Copy Governance/Permissions from the repository resources.
            ${CMAKE_COMMAND} -E copy_directory
                "${ARGS_RESOURCE_DIRECTORY}/xml"
                "${ARGS_OUTPUT_DIRECTORY}/xml"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${signed_xml_output_dir}
        DEPENDS
            ${ARGS_RESOURCE_DIRECTORY}
    )

    # ##########################################################################
    # Openssl configuration.
    # ##########################################################################
    # Set output files.
    set(ca_key_file "${certificates_output_dir}/caKey.pem")
    set(ca_cert_file "${certificates_output_dir}/caCert.pem")
    set(peer1_key_file "${certificates_output_dir}/peer01Key.pem")
    set(peer1_cert_file "${certificates_output_dir}/peer01Cert.pem")
    set(peer2_key_file "${certificates_output_dir}/peer02Key.pem")
    set(peer2_cert_file "${certificates_output_dir}/peer02Cert.pem")
    list(APPEND artifacts_output_files
        "${ca_key_file}"
        "${ca_cert_file}"
        "${peer1_key_file}"
        "${peer1_cert_file}"
        "${peer2_key_file}"
        "${peer2_cert_file}"
    )

    # ##########################################################################
    # Generate the certificates.
    # ##########################################################################
    # This must match values in ca.cnf. RootCa is self-signed.
    connextdds_openssl_generate_selfsigned_ca(
        OUTPUT_KEY_FILE "${ca_key_file}"
        OUTPUT_CERT_FILE "${ca_cert_file}"
        CRL_NUMBER_FILE "${openssl_temporary_dir}/crlNumber"
        TEXT
        DAYS ${ARGS_DAYS}
        ECPARAM_NAME ${ARGS_ECPARAM_NAME}
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam"
        DIGEST ${ARGS_DIGEST}
        CONFIG_FILE "${ca_config_file}"
        WORKING_DIRECTORY "${ARGS_OUTPUT_DIRECTORY}"
    )

    # RootCa signs Peer01Cert.
    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer1_cert_file}"
        OUTPUT_KEY_FILE "${peer1_key_file}"
        TEXT
        ECPARAM_NAME ${ARGS_ECPARAM_NAME}
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam1"
        CONFIG_FILE "${peer1_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${ARGS_DAYS}
        DIGEST ${ARGS_DIGEST}
        WORKING_DIRECTORY "${ARGS_OUTPUT_DIRECTORY}"
    )

    # RootCa signs Peer02Cert.
    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer2_cert_file}"
        OUTPUT_KEY_FILE "${peer2_key_file}"
        TEXT
        ECPARAM_NAME ${ARGS_ECPARAM_NAME}
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam1"
        CONFIG_FILE "${peer2_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${ARGS_DAYS}
        DIGEST ${ARGS_DIGEST}
        WORKING_DIRECTORY "${ARGS_OUTPUT_DIRECTORY}"
    )

    # ##########################################################################
    # Sign the permissions and governance files.
    # ##########################################################################
    foreach(xml ${xmls_name})
        list(APPEND signed_xmls
            "${signed_xml_output_dir}/signed_${xml}.p7s"
        )
        connextdds_openssl_smime_sign(
            INPUT "${ARGS_OUTPUT_DIRECTORY}/xml/${xml}.xml"
            OUTPUT "${signed_xml_output_dir}/signed_${xml}.p7s"
            SIGNER_CERTIFICATE "${ca_cert_file}"
            PRIVATE_KEY_FILE "${ca_key_file}"
        )
    endforeach()
    list(APPEND artifacts_output_files ${signed_xmls})

    set(dependencies ${dependencies} ${artifacts_output_files} PARENT_SCOPE)
endfunction()
