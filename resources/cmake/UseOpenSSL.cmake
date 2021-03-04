# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _use_openssl:

UseOpenSSL
----------

This module contains functions to use OpenSSL like file certificate generation.


.. _connextdds_openssl_generate_selfsigned_ca:
.. _connextdds_openssl_generate_signed_ca:

Generate CA
^^^^^^^^^^^
::

    connextdds_openssl_generate_selfsigned_ca(
        OUTPUT_CERT_FILE file
        OUTPUT_KEY_FILE file
        [TEXT]
        [RSA_KEY_PASSWORD pass]
        [RSA_KEY_ENCRYPTION alg]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [DSAPARAM_NUMBITS numbits]
        [DSAPARAM_OUTPUT_FILE dsa_output]
        [DIGEST digestName]
        [DAYS num]
        [CRL_NUMBER_FILE crlNumberFile]
        [CA_EXTENSION ext]
        CONFIG_FILE file
        WORKING_DIRECTORY path
    )

    connextdds_openssl_generate_signed_ca(
        OUTPUT_CERT_FILE file
        OUTPUT_CERT_REQUEST_FILE file
        OUTPUT_KEY_FILE file
        [RSA_KEY_PASSWORD pass]
        [RSA_KEY_ENCRYPTION alg]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [DSAPARAM_NUMBITS numbits]
        [DSAPARAM_OUTPUT_FILE dsa_output]
        [DIGEST digestName]
        CONFIG_FILE file
        DAYS num
        [CRL_NUMBER_FILE crlNumberFile]
        CA_CONFIG_FILE file
        CA_CERT_FILE file
        CA_KEY_FILE file
        CA_KEY_PASSWORD pass
        [CA_EXTENSION ext]
        WORKING_DIRECTORY path
    )

Generates a Certificate Authority using a configuration file. It can be
self-signed or signed by other CA.

Arguments:

``OUTPUT_CERT_FILE`` (required)
    Output path for the certificate file.

``OUTPUT_CERT_REQUEST_FILE`` (required)
    Output path for the certificate sign request file.

``OUTPUT_KEY_FILE`` (required)
    Output path for the private key file.

``TEXT`` (optional)
    Print the certificate in text format instead of base64.

``RSA_KEY_PASSWORD`` (optional)
    Password for the private RSA key.

``RSA_KEY_ENCRYPTION`` (optional)
    Algorithm to encrypt the password.

``RSA_NUMBITS`` (required with `RSA_KEY_PASSWORD`)
    Size in bits of the private key to generate.

``ECPARAM_OUTPUT_FILE`` (optional)
    Output file with the EC parameters.

``ECPARAM_NAME`` (required with `ECPARAM_OUTPUT_FILE`)
    Short name of the EC parameters.

``DSAPARAM_OUTPUT_FILE`` (optional)
    Output file with the DSA parameters.

``DSAPARAM_NUMBITS`` (required with `DSAPARAM_OUTPUT_FILE`)
    Size in bits of the DSA parameters.

``DIGEST`` (optional)
    Digest algorithm. For instance `SHA256`.

``CONFIG_FILE`` (required)
    Configuration file to generate the CA.

``DAYS`` (optional)
    The number of days the certificate will be valid.

``CRL_NUMBER_FILE`` (optional)
    The file containing the current CRL number.

``CA_CONFIG_FILE`` (required)
    CA configuration file for signing the CA.

``CA_CERT_FILE`` (required)
    CA certificate file for verifying the CA.

``CA_KEY_FILE`` (required)
    CA private key file for signing the CA.

``CA_KEY_PASSWORD`` (required)
    CA private key password.

``CA_EXTENSION`` (optional)
    Name of extension to apply from the CA configuration file.

``WORKING_DIRECTORY`` (required)
    The working directory for the openssl command. This is needed to resolve
    correctly the relative paths in the configuration file.


.. _connextdds_openssl_generate_signed_certificate:

Generate Certificate
^^^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_generate_signed_certificate(
        [OUTPUT_PEM_FILE file]
        OUTPUT_CERT_FILE file
        OUTPUT_CERT_REQUEST_FILE file
        OUTPUT_KEY_FILE file
        [TEXT]
        [RSA_KEY_PASSWORD]
        [RSA_KEY_ENCRYPTION]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [DSAPARAM_NUMBITS numbits]
        [DSAPARAM_OUTPUT_FILE dsa_output]
        [PHRASE_PASSWORD pass]
        [PRIVATE_KEY_PASSWORD pass]
        [PRIVATE_KEY_ENCRYPTION pass_encrypt]
        CONFIG_FILE file
        CA_CONFIG_FILE file
        CA_CERT_FILE file
        CA_KEY_FILE file
        CA_KEY_PASSWORD pass
        DAYS num
        [CA_EXTENSION ext]
        WORKING_DIRECTORY path
    )

Generates a certificate and signs with the CA.

Arguments:

``OUTPUT_PEM_FILE`` (optional)
    The output PEM file combining the private key and the certificate.

``OUTPUT_CERT_FILE`` (required)
    The output path for the signed certificate file.

``OUTPUT_CERT_REQUEST_FILE`` (required)
    The output path for the signing request certificate.

``OUTPUT_KEY_FILE`` (required)
    The output path for the private key.

``TEXT`` (optional)
    Print the certificate in text format in addition to base64.

``RSA_KEY_PASSWORD`` (optional)
    Password for the private RSA key.

``RSA_KEY_ENCRYPTION`` (optional)
    Algorithm to encrypt the password. Possible values are: `DES3`.

``RSA_NUMBITS`` (required with `RSA_KEY_PASSWORD`)
    Size in bits of the private key to generate.

``ECPARAM_OUTPUT_FILE`` (optional)
    Output file with the EC parameters.

``ECPARAM_NAME`` (required with `ECPARAM_OUTPUT_FILE`)
    Short name of the EC parameters.

``DSAPARAM_OUTPUT_FILE`` (optional)
    Output file with the DSA parameters.

``DSAPARAM_NUMBITS`` (required with `DSAPARAM_OUTPUT_FILE`)
    Size in bits of the DSA parameters.

``CONFIG_FILE`` (required)
    The configuration file to generate the certificate.

``CA_CONFIG_FILE`` (required)
    CA configuration file.

``CA_CERT_FILE`` (required)
    CA certificate file for verifying the certificate.

``CA_KEY_FILE`` (required)
    CA private key file for signing the certificate.

``CA_KEY_PASSWORD`` (required)
    CA private key password.

``DAYS`` (required)
    The number of days the certificate will be valid.

``CA_EXTENSION`` (optional)
    Name of extension to apply from the CA configuration file.

``WORKING_DIRECTORY`` (required)
    The working directory for the openssl command. This is needed to resolve
    correctly the relative paths in the configuration file.


.. _connextdds_openssl_revoke_certificate:

Revoke Certificate
^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_revoke_certificate(
        OUTPUT_CRL_FILE file
        CERT_FILE file
        CA_DATABASE_INDEX file
        CA_CONFIG_FILE file
        CA_CERT_FILE file
        [CA_KEY_PASSWORD pass]
    )

Revokes one or more certificates and output the CA CRL file.

``OUTPUT_CRL_FILE`` (required)
    The output path for the CRL containing the revoked certificates.

``CERT_FILE`` (required)
    The certificates to revoke.

``CA_DATABASE_INDEX`` (required)
    The path for the database index file configured in the configuration file.

``CA_CONFIG_FILE`` (required)
    The CA configuration file.

``CA_CERT_FILE`` (required)
    The CA certificate file.

``CA_KEY_PASSWORD`` (optional)
    The private key password of the CA.


.. _connextdds_openssl_generate_dh_params:

Generate DH Parameters
^^^^^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_generate_dh_params(
        OUTPUT file
        NUMBITS num
    )

Generates a file with the parameters to generate a Diffie-Hellman key pair.

Arguments:

``OUTPUT`` (required)
    Output file with the DH parameters.

``NUMBITS`` (required)
    Size of the parameter to generate.


.. _connextdds_openssl_configure_tester:

Configure Tester
^^^^^^^^^^^^^^^^
::

    connextdds_openssl_configure_tester(
        NAME testerName
    )

Configures a target tester that uses the OpenSSL libraries. The function sets
the environment variable to locate the dynamic OpenSSL libraries.

Arguments:

``NAME`` (required)
    The name of the tester target.


.. _connextdds_openssl_generate_simplified_certificate:

Simplify Certificate
^^^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_generate_simplified_certificate(
        INPUT inputCertificate
        OUTPUT outputCertificate
    )

Simplify the input certificate to remove the human-friendly text.

Arguments:

``INPUT`` (required)
  The path to the input certificate.

``OUTPUT`` (required)
  The path to the output certificate without the human-friendly.

#]]

include(FindRTIOpenSSL)

# For this module we only need the openssl executable
if(NOT OPENSSL_EXECUTABLE)
    message("OpenSSL executable not found. Make "
        "sure the variable OPENSSL_EXECUTABLE is properly defined. If you use "
        "an external OpenSSL intallation, make sure you ran "
        "find_package(RTIOpenSSL [version]) before including this file. In "
        "case you are using a builtin openssl shipped with the toolchain, make "
        "sure you have set OPENSSL_EXECUTABLE in the ProjectOptions file for "
        "your architecture."
    )
endif()

# Get the command for openssl
connextdds_get_library_search_path_definition(
    OUTPUT lib_search_def
    DIRECTORIES
        ${OPENSSL_HOST_PATH_ENV}
)
set(OPENSSL_COMMAND
    ${CMAKE_COMMAND} -E env
        ${lib_search_def}
        OPENSSL_CONF=${OPENSSL_DEFAULT_CONFIG_FILE}
    ${OPENSSL_EXECUTABLE}
)

function(connextdds_openssl_generate_rsa_key)
    set(options "")
    set(single_args OUTPUT_KEY_FILE PASSWORD KEY_ENCRYPTION NUMBITS)
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(_OPENSSL_OUTPUT_KEY_FILE _OPENSSL_NUMBITS)

    # Get the directory for creation.
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Optional args
    set(rsa_encryption_arg)
    if(_OPENSSL_KEY_ENCRYPTION)
        string(TOLOWER ${_OPENSSL_KEY_ENCRYPTION} encryption)
        set(rsa_encryption_arg "-${encryption}")
    endif()
    if(_OPENSSL_PASSWORD)
        list(APPEND rsa_encryption_arg "-passout" "pass:${_OPENSSL_PASSWORD}")
    endif()

    add_custom_command(
        COMMENT "Generating RSA key: ${_OPENSSL_OUTPUT_KEY_FILE}"
        OUTPUT "${_OPENSSL_OUTPUT_KEY_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${key_dir}
        COMMAND
            ${OPENSSL_COMMAND} genrsa
                ${rsa_encryption_arg}
                -out ${_OPENSSL_OUTPUT_KEY_FILE}
                ${_OPENSSL_NUMBITS}
        VERBATIM
    )
endfunction()

function(connextdds_openssl_generate_ec_params)
    set(options "")
    set(single_args OUTPUT_FILE NAME)
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(_OPENSSL_OUTPUT_FILE _OPENSSL_NAME)

    # Get the directory for creation.
    get_filename_component(out_dir "${_OPENSSL_OUTPUT_FILE}" DIRECTORY)

    add_custom_command(
        VERBATIM
        COMMENT "Generating EC params: ${_OPENSSL_OUTPUT_FILE}"
        OUTPUT "${_OPENSSL_OUTPUT_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${out_dir}
        COMMAND
            ${OPENSSL_COMMAND} ecparam
                -name ${_OPENSSL_NAME}
                -out ${_OPENSSL_OUTPUT_FILE}
    )
endfunction()

function(connextdds_openssl_generate_dsa_params)
    set(options "")
    set(single_args OUTPUT_FILE NUMBITS)
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(_OPENSSL_OUTPUT_FILE _OPENSSL_NUMBITS)

    # Get the directory for creation.
    get_filename_component(out_dir "${_OPENSSL_OUTPUT_FILE}" DIRECTORY)

    add_custom_command(
        VERBATIM
        COMMENT "Generating DSA params: ${_OPENSSL_OUTPUT_FILE}"
        OUTPUT "${_OPENSSL_OUTPUT_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${out_dir}
        COMMAND
            ${OPENSSL_COMMAND} dsaparam
                -out ${_OPENSSL_OUTPUT_FILE}
                ${_OPENSSL_NUMBITS}
    )
endfunction()

function(connextdds_openssl_generate_private_key)
    set(options "")
    set(single_args
        OUTPUT_KEY_FILE PASSWORD KEY_ENCRYPTION
        ECPARAM_OUTPUT_FILE ECPARAM_NAME
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_KEY_FILE _OPENSSL_PASSWORD _OPENSSL_KEY_ENCRYPTION
    )

    # Get the directory for creation.
    get_filename_component(out_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    set(additional_deps)
    set(additional_args)
    if(_OPENSSL_ECPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_ec_params(
            OUTPUT_FILE ${_OPENSSL_ECPARAM_OUTPUT_FILE}
            NAME ${_OPENSSL_ECPARAM_NAME}
        )

        list(APPEND additional_deps ${_OPENSSL_ECPARAM_OUTPUT_FILE})
        list(APPEND additional_args -paramfile ${_OPENSSL_ECPARAM_OUTPUT_FILE})
    endif()

    string(TOLOWER ${_OPENSSL_KEY_ENCRYPTION} encrypt_arg)
    set(encrypt_arg "-${encrypt_arg}")

    add_custom_command(
        VERBATIM
        COMMENT "Generating private key: ${_OPENSSL_OUTPUT_KEY_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_KEY_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${out_dir}
        COMMAND
            ${OPENSSL_COMMAND} genpkey
                ${encrypt_arg}
                -pass pass:${_OPENSSL_PASSWORD}
                ${additional_args}
                -out ${_OPENSSL_OUTPUT_KEY_FILE}
        DEPENDS
            ${additional_deps}
    )
endfunction()

function(connextdds_openssl_generate_selfsigned_ca)
    set(options TEXT)
    set(single_args
        OUTPUT_KEY_FILE OUTPUT_CERT_FILE CONFIG_FILE DIGEST DAYS CRL_NUMBER_FILE
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        DSAPARAM_NUMBITS DSAPARAM_OUTPUT_FILE
        CA_EXTENSION
        WORKING_DIRECTORY
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_KEY_FILE _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_WORKING_DIRECTORY
    )

    # Get the directories to create them
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Optional args
    set(optional_args)
    if(_OPENSSL_DAYS)
        list(APPEND optional_args -days ${_OPENSSL_DAYS})
    endif()
    if(_OPENSSL_DIGEST)
        string(TOLOWER ${_OPENSSL_DIGEST} digest)
        list(APPEND optional_args "-${digest}")
    endif()
    if(_OPENSSL_TEXT)
        list(APPEND optional_args "-text")
    endif()
    if(_OPENSSL_CA_EXTENSION)
        list(APPEND optional_args -extensions ${_OPENSSL_CA_EXTENSION})
    endif()
    if(NOT _OPENSSL_CRL_NUMBER_FILE)
        set(_OPENSSL_CRL_NUMBER_FILE "${cert_dir}/crlnumber")
    endif()

    if(_OPENSSL_RSA_KEY_PASSWORD)
        connextdds_openssl_generate_rsa_key(
            OUTPUT_KEY_FILE "${_OPENSSL_OUTPUT_KEY_FILE}"
            PASSWORD ${_OPENSSL_RSA_KEY_PASSWORD}
            KEY_ENCRYPTION ${_OPENSSL_RSA_KEY_ENCRYPTION}
            NUMBITS ${_OPENSSL_RSA_NUMBITS}
        )

        # If we generate a RSA key, the key file is a dependency, not an output
        set(ca_key_arg -new -key "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_dep "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_output)
    elseif(_OPENSSL_ECPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_ec_params(
            OUTPUT_FILE "${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            NAME ${_OPENSSL_ECPARAM_NAME}
        )

        # We add the param as a dependency and the key as output
        set(ca_key_arg
            -newkey "ec:${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
        )
        set(ca_key_dep "${_OPENSSL_ECPARAM_OUTPUT_FILE}")
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    elseif(_OPENSSL_DSAPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_dsa_params(
            OUTPUT_FILE "${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            NUMBITS ${_OPENSSL_DSAPARAM_NUMBITS}
        )

        # We add the param as a dependency and the key as output
        set(ca_key_arg
            -newkey "dsa:${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
        )
        set(ca_key_dep "${_OPENSSL_DSAPARAM_OUTPUT_FILE}")
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    else()
        # If we want to autocreate a key, the key file is an output, not a dependency
        set(ca_key_arg -newkey rsa:2048 -keyout "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_dep)
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    endif()

    add_custom_command(
        VERBATIM
        COMMENT "Generating self-signed CA: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${ca_key_output}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${cert_dir} ${key_dir} ${_OPENSSL_WORKING_DIRECTORY}
        COMMAND
            # We can't use the WORKING_DIRECTORY argument since the directory is
            # created in the first command and we don't want to create the dir
            # from CMake since if it's removed, it won't be created unless CMake
            # is called again.
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} req
                -nodes -x509 ${optional_args} ${ca_key_arg}
                -config ${_OPENSSL_CONFIG_FILE}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${cert_dir}/serial
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${_OPENSSL_CRL_NUMBER_FILE}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        DEPENDS
            "${ca_key_dep}"
            "${_OPENSSL_CONFIG_FILE}"
    )
endfunction()

function(connextdds_openssl_generate_signed_ca)
    set(options "")
    set(single_args
        OUTPUT_KEY_FILE OUTPUT_CERT_FILE OUTPUT_CERT_REQUEST_FILE CONFIG_FILE
        DAYS CRL_NUMBER_FILE
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        DSAPARAM_NUMBITS DSAPARAM_OUTPUT_FILE
        CA_CONFIG_FILE CA_KEY_FILE CA_CERT_FILE CA_KEY_PASSWORD CA_EXTENSION
        WORKING_DIRECTORY
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_KEY_FILE _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_OUTPUT_CERT_REQUEST_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_WORKING_DIRECTORY
        _OPENSSL_CA_CONFIG_FILE _OPENSSL_CA_CERT_FILE
        _OPENSSL_CA_KEY_FILE
    )

    # Get the directories to create them
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Due to a bug in OpenSSL, the default SRL file path is created by appending
    # ".srl" after the first dot in the path. In our case this would be the root
    # binary dir since our folders have dots (security.1.0). This would make to
    # use the same serial file for each certificate and there could be problems
    # when creating them in parallel. We specify a custom path.
    get_filename_component(serial_file ${_OPENSSL_OUTPUT_CERT_FILE} NAME_WE)
    string(APPEND serial_file ${serial_file} ".srl")

    # Optional args
    set(optional_args)
    if(_OPENSSL_DAYS)
        list(APPEND optional_args -days ${_OPENSSL_DAYS})
    endif()

    set(ca_extension)
    if(_OPENSSL_CA_EXTENSION)
        set(ca_extension "-extensions" ${_OPENSSL_CA_EXTENSION})
    endif()

    if(NOT _OPENSSL_CRL_NUMBER_FILE)
        set(_OPENSSL_CRL_NUMBER_FILE "${cert_dir}/crlnumber")
    endif()

    set(ca_password_arg)
    if(_OPENSSL_CA_KEY_PASSWORD)
        set(ca_password_arg -passin "pass:${_OPENSSL_CA_KEY_PASSWORD}")
    endif()

    if(_OPENSSL_RSA_KEY_PASSWORD)
        connextdds_openssl_generate_rsa_key(
            OUTPUT_KEY_FILE "${_OPENSSL_OUTPUT_KEY_FILE}"
            PASSWORD ${_OPENSSL_RSA_KEY_PASSWORD}
            KEY_ENCRYPTION ${_OPENSSL_RSA_KEY_ENCRYPTION}
            NUMBITS ${_OPENSSL_RSA_NUMBITS}
        )

        # If we generate a RSA key, the key file is a dependency, not an output
        set(ca_key_arg -key "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_dep "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_output)
    elseif(_OPENSSL_ECPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_ec_params(
            OUTPUT_FILE "${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            NAME ${_OPENSSL_ECPARAM_NAME}
        )

        # We add the param as a dependency and the key as output
        set(ca_key_arg
            -newkey "ec:${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
            -nodes
        )
        set(ca_key_dep "${_OPENSSL_ECPARAM_OUTPUT_FILE}")
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    elseif(_OPENSSL_DSAPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_dsa_params(
            OUTPUT_FILE "${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            NUMBITS ${_OPENSSL_DSAPARAM_NUMBITS}
        )

        # We add the param as a dependency and the key as output
        set(ca_key_arg
            -newkey "dsa:${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
            -nodes
        )
        set(ca_key_dep "${_OPENSSL_DSAPARAM_OUTPUT_FILE}")
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    else()
        # If we want to autocreate a key, the key file is an output, not a dependency
        set(ca_key_arg -newkey rsa:2048 -keyout "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_dep)
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    endif()

    add_custom_command(
        VERBATIM
        COMMENT "Generating signed CA: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
            ${ca_key_output}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${cert_dir} ${key_dir} ${_OPENSSL_WORKING_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} req
                -new ${optional_args} ${ca_key_arg}
                -config ${_OPENSSL_CONFIG_FILE}
                -out ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} x509 -req
                -CAcreateserial -CAserial ${serial_file}
                ${ca_password_arg}
                -extfile ${_OPENSSL_CA_CONFIG_FILE} ${ca_extension}
                -CA ${_OPENSSL_CA_CERT_FILE} -CAkey ${_OPENSSL_CA_KEY_FILE}
                -in ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${cert_dir}/serial
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${_OPENSSL_CRL_NUMBER_FILE}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        DEPENDS
            "${ca_key_dep}"
            "${_OPENSSL_CONFIG_FILE}"
            "${_OPENSSL_CA_CONFIG_FILE}" "${_OPENSSL_CA_CERT_FILE}"
    )
endfunction()

function(connextdds_openssl_generate_signed_certificate)
    set(options INCLUDE_CA_CERT TEXT)
    set(single_value_args
        OUTPUT_PEM_FILE OUTPUT_CERT_FILE OUTPUT_CERT_REQUEST_FILE OUTPUT_KEY_FILE
        CONFIG_FILE DAYS WORKING_DIRECTORY
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        DSAPARAM_NUMBITS DSAPARAM_OUTPUT_FILE
        PHRASE_PASSWORD
        PRIVATE_KEY_PASSWORD PRIVATE_KEY_ENCRYPTION
        CA_KEY_FILE CA_CONFIG_FILE CA_CERT_FILE CA_KEY_PASSWORD CA_EXTENSION
    )
    set(multi_value_args "")
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_OUTPUT_CERT_REQUEST_FILE _OPENSSL_OUTPUT_KEY_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_DAYS _OPENSSL_WORKING_DIRECTORY
        _OPENSSL_CA_KEY_FILE _OPENSSL_CA_CONFIG_FILE _OPENSSL_CA_CERT_FILE
    )

    # Get the directory to create temporal files
    get_filename_component(pem_dir "${_OPENSSL_OUTPUT_PEM_FILE}" DIRECTORY)
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(cert_req_dir "${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Due to a bug in OpenSSL, the default SRL file path is created by appending
    # ".srl" after the first dot in the path. In our case this would be the root
    # binary dir since our folders have dots (security.1.0). This would make to
    # use the same serial file for each certificate and there could be problems
    # when creating them in parallel. We specify a custom path.
    get_filename_component(serial_file ${_OPENSSL_OUTPUT_CERT_FILE} NAME_WE)
    string(APPEND serial_file ${serial_file} ".srl")

    # Optional arguments

    set(text_arg)
    if(_OPENSSL_TEXT)
        set(text_arg -text)
    endif()

    set(ca_extension)
    if(_OPENSSL_CA_EXTENSION)
        set(ca_extension "-extensions" ${_OPENSSL_CA_EXTENSION})
    endif()

    set(ca_password_arg)
    if(_OPENSSL_CA_KEY_PASSWORD)
        set(ca_password_arg -passin "pass:${_OPENSSL_CA_KEY_PASSWORD}")
    endif()

    set(req_password)
    if(_OPENSSL_PHRASE_PASSWORD)
        set(req_password -passout "pass:${_OPENSSL_PHRASE_PASSWORD}")
    else()
        # "nodes" means that openssl won't encrypt the private key
        set(req_password -nodes)
    endif()

    set(append_ca_files)
    if(_OPENSSL_INCLUDE_CA_CERT)
        set(append_ca_files "${_OPENSSL_CA_CERT_FILE}")
    endif()

    # We can create a private key with or without password
    if(_OPENSSL_PRIVATE_KEY_PASSWORD)
        connextdds_openssl_generate_private_key(
            OUTPUT_KEY_FILE "${_OPENSSL_OUTPUT_KEY_FILE}"
            PASSWORD ${_OPENSSL_PRIVATE_KEY_PASSWORD}
            KEY_ENCRYPTION ${_OPENSSL_PRIVATE_KEY_ENCRYPTION}
            ECPARAM_OUTPUT_FILE "${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            ECPARAM_NAME ${_OPENSSL_ECPARAM_NAME}
        )

        # We add the key as a dependency and the key as output
        set(key_arg
            -key "${_OPENSSL_OUTPUT_KEY_FILE}"
            -passin "pass:${_OPENSSL_PRIVATE_KEY_PASSWORD}"
        )
        set(key_dep ${_OPENSSL_OUTPUT_KEY_FILE} ${_OPENSSL_ECPARAM_OUTPUT_FILE})
        set(key_output)
    elseif(_OPENSSL_RSA_KEY_PASSWORD)
        connextdds_openssl_generate_rsa_key(
            OUTPUT_KEY_FILE "${_OPENSSL_OUTPUT_KEY_FILE}"
            PASSWORD "${_OPENSSL_RSA_KEY_PASSWORD}"
            KEY_ENCRYPTION ${_OPENSSL_RSA_KEY_ENCRYPTION}
            NUMBITS ${_OPENSSL_RSA_NUMBITS}
        )

        # If we generate a RSA key, the file is a dependency, not an output
        set(key_arg -passin pass:${_OPENSSL_RSA_KEY_PASSWORD} -key "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(key_dep "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(key_output)
    elseif(_OPENSSL_ECPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_ec_params(
            NAME ${_OPENSSL_ECPARAM_NAME}
            OUTPUT_FILE "${_OPENSSL_ECPARAM_OUTPUT_FILE}"
        )

        # We add the param as a dependency and the key as output
        set(key_arg
            -newkey "ec:${_OPENSSL_ECPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
        )
        set(key_dep "${_OPENSSL_ECPARAM_OUTPUT_FILE}")
        set(key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    elseif(_OPENSSL_DSAPARAM_OUTPUT_FILE)
        connextdds_openssl_generate_dsa_params(
            OUTPUT_FILE "${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            NUMBITS ${_OPENSSL_DSAPARAM_NUMBITS}
        )

        # We add the param as a dependency and the key as output
        set(key_arg
            -newkey "dsa:${_OPENSSL_DSAPARAM_OUTPUT_FILE}"
            -keyout "${_OPENSSL_OUTPUT_KEY_FILE}"
        )
        set(key_dep "${_OPENSSL_DSAPARAM_OUTPUT_FILE}")
        set(key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    else()
        # If we want to autocreate a key, the file is an output, not a dependency
        set(key_arg -newkey rsa:2048 -keyout "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(key_dep)
        set(key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    endif()

    # Create the certificate
    add_custom_command(
        VERBATIM
        COMMENT "Generating certificate: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
            ${key_output}
        WORKING_DIRECTORY "${_OPENSSL_WORKING_DIRECTORY}"
        # Pre-requesites: create folder and database
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${cert_req_dir} ${pem_dir} ${cert_dir} ${key_dir}
        # Create the certificate request
        COMMAND
            ${OPENSSL_COMMAND} req
                ${req_password}
                -new ${key_arg}
                -config ${_OPENSSL_CONFIG_FILE}
                -out ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
        # Certified with the CA
        COMMAND
            ${OPENSSL_COMMAND} x509
                -req -days ${_OPENSSL_DAYS}
                ${text_arg}
                -CAcreateserial -CAserial ${serial_file}
                -CA ${_OPENSSL_CA_CERT_FILE} -CAkey ${_OPENSSL_CA_KEY_FILE}
                -extfile ${_OPENSSL_CA_CONFIG_FILE} ${ca_extension}
                ${ca_password_arg}
                -in ${_OPENSSL_OUTPUT_CERT_REQUEST_FILE}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
        DEPENDS
            "${key_dep}"
            "${_OPENSSL_CONFIG_FILE}"
            "${_OPENSSL_CA_CONFIG_FILE}"
            "${_OPENSSL_CA_CERT_FILE}"
    )

    if(_OPENSSL_OUTPUT_PEM_FILE)
        set(pem_file_input
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${append_ca_files}
            ${_OPENSSL_OUTPUT_KEY_FILE}
        )
        add_custom_command(
            OUTPUT "${_OPENSSL_OUTPUT_PEM_FILE}"
            # Concatenate the key with the certificate
            COMMAND
                ${CMAKE_COMMAND}
                    -DOUTPUT=${_OPENSSL_OUTPUT_PEM_FILE}
                    "-DINPUTS=${pem_file_input}"
                    -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/Concatenate.cmake
            DEPENDS
                "${pem_file_input}"
            VERBATIM
        )
    endif()
endfunction()

function(connextdds_openssl_revoke_certificate)
    set(options "")
    set(single_value_args
        OUTPUT_CRL_FILE
        CA_DATABASE_INDEX CA_CONFIG_FILE CA_CERT_FILE CA_KEY_PASSWORD
        WORKING_DIRECTORY
    )
    set(multi_value_args CERT_FILE)
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_CRL_FILE _OPENSSL_CERT_FILE _OPENSSL_WORKING_DIRECTORY
        _OPENSSL_CA_DATABASE_INDEX _OPENSSL_CA_CONFIG_FILE _OPENSSL_CA_CERT_FILE
    )

    get_filename_component(crl_dir "${_OPENSSL_OUTPUT_CRL_FILE}" DIRECTORY)

    # Optional arg
    set(ca_password_arg)
    if(_OPENSSL_CA_KEY_PASSWORD)
        set(ca_password_arg -passin "pass:${_OPENSSL_CA_KEY_PASSWORD}")
    endif()

    add_custom_command(
        VERBATIM
        COMMENT "Creating database: ${_OPENSSL_CA_DATABASE_INDEX}"
        OUTPUT
            "${_OPENSSL_CA_DATABASE_INDEX}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${crl_dir} ${_OPENSSL_WORKING_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${_OPENSSL_WORKING_DIRECTORY}/ca/database
        # Make a new database index.txt. This file will contain the revoked
        # certificates and from it we will generate the CRL.
        COMMAND
            ${CMAKE_COMMAND} -E remove ${_OPENSSL_CA_DATABASE_INDEX}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${_OPENSSL_CA_DATABASE_INDEX}
        DEPENDS
            "${_OPENSSL_CERT_FILE}"
            "${_OPENSSL_CA_CONFIG_FILE}"
            "${_OPENSSL_CA_CERT_FILE}"
    )

    set(gencrl_dependencies "")
    foreach(certFile ${_OPENSSL_CERT_FILE})
        list(APPEND gencrl_dependencies "${certFile}Revoked")
        add_custom_command(
            VERBATIM
            COMMENT "Revoking certificate: ${certFile}"
            OUTPUT
                "${certFile}Revoked"
            COMMAND
                ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
                ${OPENSSL_COMMAND} ca
                    -batch ${ca_password_arg}
                    -config ${_OPENSSL_CA_CONFIG_FILE}
                    -revoke ${certFile}
            COMMAND
                ${CMAKE_COMMAND} -DCONTENT=1 -DOUTPUT=${certFile}Revoked
                    -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
            DEPENDS
                "${_OPENSSL_CA_DATABASE_INDEX}"
        )
    endforeach()
    add_custom_command(
        VERBATIM
        COMMENT "Generating CRL file: ${_OPENSSL_OUTPUT_CRL_FILE}"
        OUTPUT
            "${_OPENSSL_OUTPUT_CRL_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} ca
                -gencrl -batch ${ca_password_arg}
                -config ${_OPENSSL_CA_CONFIG_FILE}
                -out ${_OPENSSL_OUTPUT_CRL_FILE}
        COMMAND
            # BUILD-2417: Make sure that if we ever need to regenerate the CRL,
            # we also regenerate the database file on which the CRL depends.
            # Otherwise, we will get "already revoked" errors.
            ${CMAKE_COMMAND} -E remove ${_OPENSSL_CA_DATABASE_INDEX}
        DEPENDS
            "${gencrl_dependencies}"
    )
endfunction()

function(connextdds_openssl_generate_dh_params)
    set(options "")
    set(single_value_args OUTPUT NUMBITS)
    set(multi_value_args "")
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN})
    connextdds_check_required_arguments(_OPENSSL_OUTPUT _OPENSSL_NUMBITS)

    get_filename_component(output_dir "${_OPENSSL_OUTPUT}" DIRECTORY)

    add_custom_command(
        VERBATIM
        COMMENT "Generating DH parameters: ${_OPENSSL_OUTPUT}"
        OUTPUT "${_OPENSSL_OUTPUT}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${output_dir}
        COMMAND
            ${OPENSSL_COMMAND} dhparam -out ${_OPENSSL_OUTPUT} ${_OPENSSL_NUMBITS}
    )
endfunction()

function(connextdds_openssl_smime_sign)
    set(options)
    set(single_args INPUT OUTPUT SIGNER_CERTIFICATE PRIVATE_KEY_FILE)
    set(multi_args)
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_args}"
        "${multi_args}"
        ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_INPUT _OPENSSL_OUTPUT
        _OPENSSL_SIGNER_CERTIFICATE _OPENSSL_PRIVATE_KEY_FILE
    )

    add_custom_command(
        VERBATIM
        COMMENT "Signing SMIME: ${_OPENSSL_INPUT}"
        OUTPUT "${_OPENSSL_OUTPUT}"
        COMMAND
            ${OPENSSL_COMMAND} smime -sign -text
                -in ${_OPENSSL_INPUT}
                -out ${_OPENSSL_OUTPUT}
                -signer ${_OPENSSL_SIGNER_CERTIFICATE}
                -inkey ${_OPENSSL_PRIVATE_KEY_FILE}
        DEPENDS
            "${_OPENSSL_INPUT}"
            "${_OPENSSL_SIGNER_CERTIFICATE}" "${_OPENSSL_PRIVATE_KEY_FILE}"
    )
endfunction()


function(connextdds_openssl_generate_simplified_certificate)
    set(options)
    set(single_args INPUT OUTPUT)
    set(multi_args)
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_args}"
        "${multi_args}"
        ${ARGN}
    )

    connextdds_check_required_arguments(
        _OPENSSL_INPUT _OPENSSL_OUTPUT
    )

    add_custom_command(
        VERBATIM
        COMMENT "Creating base64-only version of ${_OPENSSL_INPUT}"
        OUTPUT "${_OPENSSL_OUTPUT}"
        COMMAND
            ${CMAKE_COMMAND}
                "-DOUTPUT=${_OPENSSL_OUTPUT}"
                "-DINPUT=${_OPENSSL_INPUT}"
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/SimplifyCertificate.cmake
        DEPENDS
            "${_OPENSSL_INPUT}"
    )

endfunction()