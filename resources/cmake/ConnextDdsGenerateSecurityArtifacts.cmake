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
    # Directory where the source files used to generate the security artifacts
    # are. These are the .cnf openssl configuration files that will be used to
    # generate the certificates, and the XML Governance/Permission Documents
    # that will be signed.
    set(SECURITY_RESOURCES_PATH "${CMAKE_MODULE_PATH}/../security")

    # ##########################################################################
    # Create the directory structure for the generated security artifacts.
    # <example/<CMAKE_CURRENT_BINARY_DIR>/security/ecdsa01
    #   |- certs/
    #   |- temporary_files/
    #   |- xml/
    #   |   |- signed/
    #
    # ##########################################################################
    set(security_root_directory "${CMAKE_CURRENT_BINARY_DIR}/security")
    file(MAKE_DIRECTORY ${security_root_directory})
    # ecdsa01. Copied from the repository resources.
    set(openssl_working_directory "${security_root_directory}/ecdsa01")
    file(COPY
        "${SECURITY_RESOURCES_PATH}/ecdsa01"
        DESTINATION ${security_root_directory}
    )
    # xml. Copied from the repository resources.
    file(COPY
        "${SECURITY_RESOURCES_PATH}/xml"
        DESTINATION ${openssl_working_directory}
    )
    # certs. Output OpenSSL files. This must match the value in ca.cnf.
    set(certificates_output_directory "${openssl_working_directory}/certs")
    file(MAKE_DIRECTORY ${certificates_output_directory})
    # xml/signed. This is where the result of signing the xml files will be.
    file(MAKE_DIRECTORY "${openssl_working_directory}/xml/signed")
    # temporary_files. This must match the value in ca.cnf.
    set(openssl_temporary_directory "${openssl_working_directory}/temporary_files")
    file(MAKE_DIRECTORY ${openssl_temporary_directory})

    # ##########################################################################
    # Openssl configuration.
    # ##########################################################################
    # Set input files
    set(ca_config_file "${openssl_working_directory}/ca.cnf")
    set(peer1_config_file "${openssl_working_directory}/peer1.cnf")
    set(peer2_config_file "${openssl_working_directory}/peer2.cnf")

    # Set output files
    set(ca_key_file "${certificates_output_directory}/ca_key.pem")
    set(ca_cert_file "${certificates_output_directory}/ca_cert.pem")
    set(peer1_key_file "${certificates_output_directory}/peer1_key.pem")
    set(peer1_cert_file "${certificates_output_directory}/peer1_cert.pem")
    set(peer2_key_file "${certificates_output_directory}/peer2_key.pem")
    set(peer2_cert_file "${certificates_output_directory}/peer2_cert.pem")


    # Set configuration options for the certificates.
    set(expiration_days 3650)  # Almost 10 years

    # ##########################################################################
    # Generate the certificates.
    # ##########################################################################
    # This must match values in ca.cnf. RootCa is self-signed
    connextdds_openssl_generate_selfsigned_ca(
        OUTPUT_KEY_FILE "${ca_key_file}"
        OUTPUT_CERT_FILE "${ca_cert_file}"
        CRL_NUMBER_FILE "${openssl_temporary_directory}/crlNumber"
        TEXT
        DIGEST SHA256
        DAYS ${expiration_days}
        ECPARAM_NAME prime256v1
        ECPARAM_OUTPUT_FILE "${openssl_temporary_directory}/ecdsaparam"
        CONFIG_FILE "${ca_config_file}"
        WORKING_DIRECTORY "${openssl_working_directory}"
    )

    # RootCa signs Peer01Cert
    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer1_cert_file}"
        OUTPUT_CERT_REQUEST_FILE "${openssl_temporary_directory}/peer1_req_cert.pem"
        OUTPUT_KEY_FILE "${peer1_key_file}"
        TEXT
        ECPARAM_NAME "prime256v1"
        ECPARAM_OUTPUT_FILE "${openssl_temporary_directory}/ecdsaparam1"
        CONFIG_FILE "${peer1_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${expiration_days}
        WORKING_DIRECTORY "${openssl_working_directory}"
    )

    connextdds_openssl_generate_signed_certificate(
        OUTPUT_CERT_FILE "${peer2_cert_file}"
        OUTPUT_CERT_REQUEST_FILE "${openssl_temporary_directory}/peer2_req_cert.pem"
        OUTPUT_KEY_FILE "${peer2_key_file}"
        TEXT
        ECPARAM_NAME "prime256v1"
        ECPARAM_OUTPUT_FILE "${openssl_temporary_directory}/ecdsaparam1"
        CONFIG_FILE "${peer2_config_file}"
        CA_KEY_FILE "${ca_key_file}"
        CA_CONFIG_FILE "${ca_config_file}"
        CA_CERT_FILE "${ca_cert_file}"
        DAYS ${expiration_days}
        WORKING_DIRECTORY "${openssl_working_directory}"
    )

    # ##########################################################################
    # Sign the permissions and governance files.
    # ##########################################################################
    set(signed_xmls_name Governance Permissions1 Permissions2)
    foreach(xml ${signed_xmls_name})
        list(APPEND signed_xmls
            "${openssl_working_directory}/xml/signed/signed_${xml}.p7s"
        )
        connextdds_openssl_smime_sign(
            INPUT "${openssl_working_directory}/xml/${xml}.xml"
            OUTPUT "${openssl_working_directory}/xml/signed/signed_${xml}.p7s"
            SIGNER_CERTIFICATE "${ca_cert_file}"
            PRIVATE_KEY_FILE "${ca_key_file}"
        )
    endforeach()

    # ##########################################################################
    # Add custom target.
    # ##########################################################################
    add_custom_target(securityArtifacts
        ALL
        DEPENDS
            "${ca_key_file}"
            "${ca_cert_file}"
            "${peer1_config_file}"
            "${peer1_key_file}"
            "${peer1_cert_file}"
            "${peer2_key_file}"
            "${peer2_cert_file}"
            ${signed_xmls}
    )
endfunction()
