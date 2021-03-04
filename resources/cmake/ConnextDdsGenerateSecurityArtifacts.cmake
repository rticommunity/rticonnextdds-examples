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
    # Directory structure for the generated security artifacts.
    # <example/<CMAKE_CURRENT_BINARY_DIR>/security/ecdsa01
    #   |- certs/
    #   |- temporary_files/
    #   |- xml/
    #   |   |- signed/
    #
    # These variables will be used when generating the security artifacts.
    # ##########################################################################
    set(artifacts_output_dir "${CMAKE_CURRENT_BINARY_DIR}/security")

    set(openssl_working_dir "${artifacts_output_dir}/ecdsa01")
    # certs. Output OpenSSL files. This must match the value in ca.cnf.
    set(certificates_output_dir "${openssl_working_dir}/certs")
    # xml/signed. This is where the result of signing the xml files will be.
    set(signed_xml_output_dir "${openssl_working_dir}/xml/signed")
    # temporary_files. This must match the value in ca.cnf.
    set(openssl_temporary_dir "${openssl_working_dir}/temporary_files")

    # ##########################################################################
    # Copy the source files from the resource folder common for all examples
    # (resources_input_dir) to the binary folder for the particular example.
    # ##########################################################################
    # Directory where the source files used to generate the security artifacts
    # are.
    set(resources_input_dir "${CMAKE_MODULE_PATH}/../security")

    # These are the files that we copied from resources_input_dir.
    # They are the .cnf openssl configuration files that will be used to
    # generate the certificates, and the XML Governance/Permission Documents
    # that will be signed.
    set(ca_config_file "${openssl_working_dir}/ca.cnf")
    set(peer1_config_file "${openssl_working_dir}/peer1.cnf")
    set(peer2_config_file "${openssl_working_dir}/peer2.cnf")
    set(artifacts_input_files
        "${ca_config_file}"
        "${peer1_config_file}"
        "${peer2_config_file}"
    )
    set(xmls_name Governance Permissions1 Permissions2)
    foreach(xml ${xmls_name})
        list(APPEND artifacts_input_files "${xml}.xml")
    endforeach()

    add_custom_command(
        OUTPUT ${artifacts_input_files}
        PRE_BUILD
        COMMENT "Copying security resources to the example"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${artifacts_output_dir}
        COMMAND # ecdsa01. Copy config files from the repository resources.
            ${CMAKE_COMMAND} -E copy_directory
                    "${resources_input_dir}/ecdsa01"
                    "${artifacts_output_dir}/ecdsa01"
        COMMAND # xml. Copy Governance/Permissions from the repository resources.
            ${CMAKE_COMMAND} -E copy_directory
                "${resources_input_dir}/xml"
                "${openssl_working_dir}/xml"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${certificates_output_dir}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${signed_xml_output_dir}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${openssl_temporary_dir}
        DEPENDS
            ${resources_input_dir}
    )

    # ##########################################################################
    # Openssl configuration.
    # ##########################################################################
    # Set output files.
    set(ca_key_file "${certificates_output_dir}/ca_key.pem")
    set(ca_cert_file "${certificates_output_dir}/ca_cert.pem")
    set(peer1_key_file "${certificates_output_dir}/peer1_key.pem")
    set(peer1_cert_file "${certificates_output_dir}/peer1_cert.pem")
    set(peer2_key_file "${certificates_output_dir}/peer2_key.pem")
    set(peer2_cert_file "${certificates_output_dir}/peer2_cert.pem")
    set(artifacts_output_files
        "${ca_key_file}"
        "${ca_cert_file}"
        "${peer1_key_file}"
        "${peer1_cert_file}"
        "${peer2_key_file}"
        "${peer2_cert_file}"
    )

    # Set configuration options for the certificates.
    set(expiration_days 3650)  # Almost 10 years.

    # ##########################################################################
    # Generate the certificates.
    # ##########################################################################
    # This must match values in ca.cnf. RootCa is self-signed.
    connextdds_openssl_generate_selfsigned_ca(
        OUTPUT_KEY_FILE "${ca_key_file}"
        OUTPUT_CERT_FILE "${ca_cert_file}"
        CRL_NUMBER_FILE "${openssl_temporary_dir}/crlNumber"
        TEXT
        DIGEST SHA256
        DAYS ${expiration_days}
        ECPARAM_NAME prime256v1
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam"
        CONFIG_FILE "${ca_config_file}"
        WORKING_DIRECTORY "${openssl_working_dir}"
    )

    # RootCa signs Peer01Cert.
    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer1_cert_file}"
        OUTPUT_CERT_REQUEST_FILE "${openssl_temporary_dir}/peer1_req_cert.pem"
        OUTPUT_KEY_FILE "${peer1_key_file}"
        TEXT
        ECPARAM_NAME "prime256v1"
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam1"
        CONFIG_FILE "${peer1_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${expiration_days}
        WORKING_DIRECTORY "${openssl_working_dir}"
    )

    # RootCa signs Peer02Cert.
    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer2_cert_file}"
        OUTPUT_CERT_REQUEST_FILE "${openssl_temporary_dir}/peer2_req_cert.pem"
        OUTPUT_KEY_FILE "${peer2_key_file}"
        TEXT
        ECPARAM_NAME "prime256v1"
        ECPARAM_OUTPUT_FILE "${openssl_temporary_dir}/ecdsaparam1"
        CONFIG_FILE "${peer2_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${expiration_days}
        WORKING_DIRECTORY "${openssl_working_dir}"
    )

    # ##########################################################################
    # Sign the permissions and governance files.
    # ##########################################################################
    foreach(xml ${xmls_name})
        list(APPEND signed_xmls
            "${signed_xml_output_dir}/signed_${xml}.p7s"
        )
        connextdds_openssl_smime_sign(
            INPUT "${openssl_working_dir}/xml/${xml}.xml"
            OUTPUT "${signed_xml_output_dir}/signed_${xml}.p7s"
            SIGNER_CERTIFICATE "${ca_cert_file}"
            PRIVATE_KEY_FILE "${ca_key_file}"
        )
    endforeach()
    list(APPEND artifacts_output_files ${signed_xmls})

    # ##########################################################################
    # Add custom target.
    # ##########################################################################
    add_custom_target(securityArtifacts
        ALL
        DEPENDS
            ${artifacts_input_files}
            ${artifacts_output_files}
    )
endfunction()
