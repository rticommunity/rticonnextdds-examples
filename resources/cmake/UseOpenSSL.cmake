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
        CRL_NUMBER_FILE crlNumberFile
        [TEXT]
        [RSA_KEY_PASSWORD pass]
        [RSA_KEY_ENCRYPTION alg]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [EDPARAM_NAME ed_name]
        [DIGEST digestName]
        [DAYS num]
        [CA_EXTENSION ext]
        CONFIG_FILE file
        WORKING_DIRECTORY path
    )

    connextdds_openssl_generate_signed_ca(
        OUTPUT_CERT_FILE file
        OUTPUT_KEY_FILE file
        CRL_NUMBER_FILE crlNumberFile
        [RSA_KEY_PASSWORD pass]
        [RSA_KEY_ENCRYPTION alg]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [DIGEST digestName]
        CONFIG_FILE file
        DAYS num
        CA_CONFIG_FILE file
        CA_CERT_FILE file
        CA_KEY_FILE file
        CA_KEY_PASSWORD pass
        [CA_EXTENSION ext]
        WORKING_DIRECTORY path
    )

    connextdds_openssl_generate_expired_ca(
        OUTPUT_CERT_FILE file
        OUTPUT_KEY_FILE file
        CRL_NUMBER_FILE crlNumberFile
        ECPARAM_OUTPUT_FILE ec_output
        CONFIG_FILE file
        CA_DATABASE_INDEX file
        CA_CERT_FILE file
        CA_KEY_FILE file
        WORKING_DIRECTORY path
    )

Generates a Certificate Authority using a configuration file. It can be
self-signed or signed by other CA.

Arguments:

``OUTPUT_CERT_FILE`` (required)
    Output path for the certificate file.

``OUTPUT_KEY_FILE`` (required)
    Output path for the private key file.

``CRL_NUMBER_FILE`` (required)
    The file containing the current CRL number. Must match the crlnumber from
    the cnf file.

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

``EDPARAM_NAME`` (optional)
    Short name of the EDDSA used.

``DIGEST`` (optional)
    Digest algorithm. For instance `SHA256`.

``CONFIG_FILE`` (required)
    Configuration file to generate the CA.

``DAYS`` (optional)
    The number of days the certificate will be valid.

``CA_CONFIG_FILE`` (required)
    CA configuration file for signing the CA.

``CA_CERT_FILE`` (required)
    CA certificate file for verifying the CA.

    This argument is optional for connextdds_openssl_generate_expired_ca.
    If neither CA_CERT_FILE nor CA_KEY_FILE are passed as arguments, the expired
    CA generated will be self-signed.

``CA_KEY_FILE`` (required)
    CA private key file for signing the CA.

    This argument is optional for connextdds_openssl_generate_expired_ca.
    If neither CA_CERT_FILE nor CA_KEY_FILE are passed as arguments, the expired
    CA generated will be self-signed.

``CA_KEY_PASSWORD`` (required)
    CA private key password.

``CA_EXTENSION`` (optional)
    Name of extension to apply from the CA configuration file.

``CA_DATABASE_INDEX`` (required - generate_expired_ca)
    OpenSSL index file (full path).
    It is used by the `openssl ca` command as a database, containing one line
    of information per certificate.
    The file is an output of the function. It does not have to exist previously
    (and if it does, it will be removed).
    It should match the value of the database field in the CA configuration
    file.

``WORKING_DIRECTORY`` (required)
    The working directory for the openssl command. This is needed to resolve
    correctly the relative paths in the configuration file. It has to be set to
    the Certificate Authority's directory, i.e. the one containing the "ca"
    and "identities" folders. Examples are: "ecdsa01", or "rsa01".


.. _connextdds_openssl_generate_signed_certificate:

Generate Certificate
^^^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_generate_signed_certificate(
        [OUTPUT_PEM_FILE file]
        OUTPUT_CERT_FILE file
        OUTPUT_KEY_FILE file
        [TEXT]
        [RSA_KEY_PASSWORD]
        [RSA_KEY_ENCRYPTION]
        [RSA_NUMBITS]
        [ECPARAM_NAME ec_name]
        [ECPARAM_OUTPUT_FILE ec_output]
        [EDPARAM_NAME ed_name]
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

``EDPARAM_NAME`` (optional)
    Short name of the EDDSA used.

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
    correctly the relative paths in the configuration file. It has to be set to
    the Certificate Authority's directory, i.e. the one containing the "ca"
    and "identities" folders. Examples are: "ecdsa01", or "rsa01".


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


Generate Trusted Certificate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
::

    connextdds_openssl_generate_trusted_certificate(
        INPUT_CERT_FILE inputCertificate
        OUTPUT_TRUSTED_CERT_FILE outputTrustedCertificate
        ADD_TRUST signType
    )

Generate the trusted certificate to be trusted for signType use (for eg : OCSPSigning)
Note that this certificate can be used to sign OCSP responses for multiple CA.

Arguments:

``INPUT_CERT_FILE`` (required)
  The path to the input certificate.

``OUTPUT_TRUSTED_CERT_FILE`` (required)
  The path to the output certificate trusted for signType use

``ADD_TRUST`` (required)
  Adds a trusted certificate use. E.g., OCSPSigning

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


function(connextdds_openssl_generate_trusted_certificate)
    set(options "")
    set(single_args INPUT_CERT_FILE OUTPUT_TRUSTED_CERT_FILE ADD_TRUST)
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(_OPENSSL_INPUT_CERT_FILE _OPENSSL_OUTPUT_TRUSTED_CERT_FILE _OPENSSL_ADD_TRUST)

    # Get the directory for creation.
    get_filename_component(out_dir "${_OPENSSL_OUTPUT_TRUSTED_CERT_FILE}" DIRECTORY)

    add_custom_command(
        COMMENT "Generating trusted cert: ${_OPENSSL_OUTPUT_TRUSTED_CERT_FILE}"
        OUTPUT "${_OPENSSL_OUTPUT_TRUSTED_CERT_FILE}"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${out_dir}
        COMMAND
            ${OPENSSL_COMMAND} x509
                -in "${_OPENSSL_INPUT_CERT_FILE}"
                -addtrust ${_OPENSSL_ADD_TRUST}
                -out ${_OPENSSL_OUTPUT_TRUSTED_CERT_FILE}
        DEPENDS
            "${_OPENSSL_INPUT_CERT_FILE}"
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

# This macro returns a 20Bytes hexadecimal serial number that can be used with
# OpenSSL.
macro(get_serial_number)
    set(options "")
    set(single_args BASE_NAME)
    set(multi_args "")
    cmake_parse_arguments(SERIAL
        "${options}"
        "${single_args}"
        "${multi_args}" ${ARGN}
    )
    connextdds_check_required_arguments(SERIAL_BASE_NAME)

    string(RANDOM
        LENGTH 40
        ALPHABET 0123456789ABCDEF
        _${SERIAL_BASE_NAME}_serial_number)
endmacro()

function(connextdds_openssl_generate_selfsigned_ca)
    set(options TEXT)
    set(single_args
        OUTPUT_KEY_FILE OUTPUT_CERT_FILE CONFIG_FILE DIGEST DAYS CRL_NUMBER_FILE
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        EDPARAM_NAME
        CA_EXTENSION
        WORKING_DIRECTORY
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_KEY_FILE _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_CRL_NUMBER_FILE
        _OPENSSL_WORKING_DIRECTORY
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
    elseif(_OPENSSL_EDPARAM_NAME)
        set(ca_key_arg -newkey "${_OPENSSL_EDPARAM_NAME}" -keyout "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(ca_key_dep)
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
                ${_OPENSSL_WORKING_DIRECTORY}/ca/database
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
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${_OPENSSL_CRL_NUMBER_FILE}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        DEPENDS
            ${ca_key_dep}
            ${_OPENSSL_CONFIG_FILE}
    )
endfunction()

function(connextdds_openssl_generate_signed_ca)
    set(options RSA_PSS_PADDING)
    set(single_args
        OUTPUT_KEY_FILE OUTPUT_CERT_FILE CONFIG_FILE
        DAYS CRL_NUMBER_FILE
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        CA_CONFIG_FILE CA_KEY_FILE CA_CERT_FILE CA_KEY_PASSWORD CA_EXTENSION
        WORKING_DIRECTORY
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_KEY_FILE _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_WORKING_DIRECTORY
        _OPENSSL_CA_CONFIG_FILE _OPENSSL_CA_CERT_FILE
        _OPENSSL_CA_KEY_FILE _OPENSSL_CRL_NUMBER_FILE
    )

    # The cert request file is just the cert file with "Cert" replaced by "Req".
    set(certRequestFile)
    string(REPLACE "Cert" "Req" certRequestFile "${_OPENSSL_OUTPUT_CERT_FILE}")

    # Get the directories to create them
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Optional args
    set(optional_args)
    if(_OPENSSL_DAYS)
        list(APPEND optional_args -days ${_OPENSSL_DAYS})
    endif()

    set(ca_extension)
    if(_OPENSSL_CA_EXTENSION)
        set(ca_extension "-extensions" ${_OPENSSL_CA_EXTENSION})
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
    else()
        # If we want to autocreate a key, the key file is an output, not a
        # dependency
        # - nodes: This rsa key is not protected by a password.
        if(_OPENSSL_RSA_PSS_PADDING)
            set(ca_key_arg
                    -newkey rsa-pss -pkeyopt rsa_keygen_bits:2048
                    -keyout "${_OPENSSL_OUTPUT_KEY_FILE}" -nodes)
        else()
            set(ca_key_arg
                -newkey rsa:2048 -keyout "${_OPENSSL_OUTPUT_KEY_FILE}" -nodes)
        endif()
        set(ca_key_dep)
        set(ca_key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    endif()

    get_serial_number(BASE_NAME ${_OPENSSL_OUTPUT_CERT_FILE})

    add_custom_command(
        VERBATIM
        COMMENT "Generating signed CA: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${ca_key_output}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${cert_dir} ${key_dir} ${_OPENSSL_WORKING_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} req
                -new ${optional_args} ${ca_key_arg}
                -config ${_OPENSSL_CONFIG_FILE}
                -out ${certRequestFile}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} x509 -req
                ${optional_args}
                ${ca_password_arg}
                -extfile ${_OPENSSL_CA_CONFIG_FILE} ${ca_extension}
                -CA ${_OPENSSL_CA_CERT_FILE} -CAkey ${_OPENSSL_CA_KEY_FILE}
                -set_serial "0x${_${_OPENSSL_OUTPUT_CERT_FILE}_serial_number}"
                -in ${certRequestFile}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${_OPENSSL_CRL_NUMBER_FILE}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        # The certificate request is just a temporary file, so we remove it.
        COMMAND
            ${CMAKE_COMMAND} -E remove ${certRequestFile}
        DEPENDS
            ${ca_key_dep}
            ${_OPENSSL_CONFIG_FILE}
            ${_OPENSSL_CA_CONFIG_FILE}
            ${_OPENSSL_CA_CERT_FILE}
            ${_OPENSSL_CA_KEY_FILE}
    )
endfunction()

function(connextdds_openssl_generate_expired_ca)
    set(single_args
        OUTPUT_KEY_FILE OUTPUT_CERT_FILE CONFIG_FILE CRL_NUMBER_FILE
        ECPARAM_OUTPUT_FILE CA_DATABASE_INDEX
        CA_CERT_FILE CA_KEY_FILE WORKING_DIRECTORY
    )
    set(multi_args "")
    cmake_parse_arguments(_OPENSSL "${options}" "${single_args}" "${multi_args}" ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_OUTPUT_KEY_FILE
        _OPENSSL_CRL_NUMBER_FILE
        _OPENSSL_ECPARAM_OUTPUT_FILE
        _OPENSSL_CONFIG_FILE
        _OPENSSL_CA_DATABASE_INDEX
        _OPENSSL_WORKING_DIRECTORY
    )

    # The cert request file is just the cert file with "Cert" replaced by "Req".
    set(certRequestFile)
    string(REPLACE "Cert" "Req" certRequestFile "${_OPENSSL_OUTPUT_CERT_FILE}")

    # Get the directories to create them
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # We assign the current date + 2 minutes to the end date of the certificate.
    # This allows us to sign the peers and documents before expiring.
    # Some tests will fail if we run them before the allocated 2 minutes.
    # Manipulate the epoch because we want to support rolling over the hour
    # when the minute changes from 58 or 59.
    string(TIMESTAMP _openssl_current_epoch "%s" UTC)
    MATH(EXPR _openssl_invalid_epoch "(${_openssl_current_epoch} + 120)")

    # SOURCE_DATE_EPOCH allows the date and time to be set externally by an
    # exported environment variable. If the SOURCE_DATE_EPOCH environment
    # variable is set, the string(TIMESTAMP [...]) cmake command will return
    # its value instead of the current time.
    if (DEFINED ENV{SOURCE_DATE_EPOCH})
        set(_old_source_date_epoch ENV{SOURCE_DATE_EPOCH})
    endif()
    set(ENV{SOURCE_DATE_EPOCH} ${_openssl_invalid_epoch})
    string(TIMESTAMP _openssl_invalid_date "%y%m%d%H%M%S" UTC)
    if (DEFINED ${_old_source_date_epoch})
        set(ENV{SOURCE_DATE_EPOCH} ${_old_source_date_epoch})
    else()
        unset(ENV{SOURCE_DATE_EPOCH})
    endif()

    # At this point, _openssl_invalid_date already has the timestamp that we
    # want (current date + 2 minutes). Now we are appending the Z character,
    # which OpenSSL's -enddate requires when indicating UTC time (ASN1 UTCTime
    # structure).
    string(CONCAT _openssl_invalid_date
        ${_openssl_invalid_date}
        "Z"
    )

    connextdds_openssl_generate_ec_params(
        OUTPUT_FILE "${_OPENSSL_ECPARAM_OUTPUT_FILE}"
        NAME prime256v1
    )

    if(_OPENSSL_CA_CERT_FILE AND _OPENSSL_CA_KEY_FILE)
        set(_ca_arg "-keyfile" "${_OPENSSL_CA_KEY_FILE}")
        list(APPEND _ca_arg "-cert" "${_OPENSSL_CA_CERT_FILE}")
    else()
        set(_ca_arg "-selfsign")
        list(APPEND _ca_arg "-keyfile" "${_OPENSSL_OUTPUT_KEY_FILE}")
    endif()

    # We rely on OpenSSL's "ca" command (because it supports -enddate) instead
    # of the "x509" command.
    # The "ca" command does not support the "-set_serial" flag. It always
    # handles serial numbers as files. The issue is that we may run into race
    # conditions when generating+signing certificates.
    get_filename_component(_cert_file_name ${_OPENSSL_OUTPUT_CERT_FILE} NAME)
    string(REPLACE
        "Cert.pem"
        "Serial"
        _serial_file_name
        "${cert_dir}/database/${_cert_file_name}")
    get_serial_number(BASE_NAME ${_OPENSSL_OUTPUT_CERT_FILE})

    add_custom_command(
        VERBATIM
        COMMENT
            "Generating expired CA: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_KEY_FILE}
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${_serial_file_name}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${cert_dir} ${key_dir} ${_OPENSSL_WORKING_DIRECTORY}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${_OPENSSL_WORKING_DIRECTORY}/ca/database
        COMMAND
            ${CMAKE_COMMAND}
                -DCONTENT=${_${_OPENSSL_OUTPUT_CERT_FILE}_serial_number}
                -DOUTPUT=${_serial_file_name}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        COMMAND
            ${CMAKE_COMMAND} -E remove ${_OPENSSL_CA_DATABASE_INDEX}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} req
                -nodes
                -new
                -newkey "ec:${_OPENSSL_ECPARAM_OUTPUT_FILE}"
                -keyout ${_OPENSSL_OUTPUT_KEY_FILE}
                -out ${certRequestFile}
                -config ${_OPENSSL_CONFIG_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E touch ${_OPENSSL_CA_DATABASE_INDEX}
        COMMAND
            ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
            ${OPENSSL_COMMAND} ca
                -batch
                -config ${_OPENSSL_CONFIG_FILE}
                ${_ca_arg}
                -enddate ${_openssl_invalid_date}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
                -in ${certRequestFile}
        COMMAND
            ${CMAKE_COMMAND} -DCONTENT=01 -DOUTPUT=${_OPENSSL_CRL_NUMBER_FILE}
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
        # The certificate request is just a temporary file, so we remove it.
        COMMAND
            ${CMAKE_COMMAND} -E remove ${certRequestFile}
        DEPENDS
            ${_OPENSSL_ECPARAM_OUTPUT_FILE}
            ${_OPENSSL_CONFIG_FILE}
            ${_OPENSSL_CA_CERT_FILE}
            ${_OPENSSL_CA_KEY_FILE}
    )
endfunction()

function(connextdds_openssl_generate_signed_certificate)
    set(options INCLUDE_CA_CERT TEXT RSA_PSS_PADDING)
    set(single_value_args
        OUTPUT_PEM_FILE OUTPUT_CERT_FILE OUTPUT_KEY_FILE
        CONFIG_FILE DAYS WORKING_DIRECTORY
        RSA_KEY_PASSWORD RSA_KEY_ENCRYPTION RSA_NUMBITS
        ECPARAM_NAME ECPARAM_OUTPUT_FILE
        EDPARAM_NAME
        PHRASE_PASSWORD
        PRIVATE_KEY_PASSWORD PRIVATE_KEY_ENCRYPTION
        CA_KEY_FILE CA_CONFIG_FILE CA_CERT_FILE CA_KEY_PASSWORD CA_EXTENSION
        DIGEST
    )
    set(multi_value_args "")
    cmake_parse_arguments(_OPENSSL
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN})
    connextdds_check_required_arguments(
        _OPENSSL_OUTPUT_CERT_FILE
        _OPENSSL_OUTPUT_KEY_FILE
        _OPENSSL_CONFIG_FILE _OPENSSL_DAYS _OPENSSL_WORKING_DIRECTORY
        _OPENSSL_CA_KEY_FILE _OPENSSL_CA_CONFIG_FILE _OPENSSL_CA_CERT_FILE
    )

    # The cert request file is just the cert file with "Cert" replaced by "Req".
    set(certRequestFile)
    string(REPLACE "Cert" "Req" certRequestFile "${_OPENSSL_OUTPUT_CERT_FILE}")

    # Get the directory to create temporal files
    get_filename_component(pem_dir "${_OPENSSL_OUTPUT_PEM_FILE}" DIRECTORY)
    get_filename_component(cert_dir "${_OPENSSL_OUTPUT_CERT_FILE}" DIRECTORY)
    get_filename_component(key_dir "${_OPENSSL_OUTPUT_KEY_FILE}" DIRECTORY)

    # Optional arguments

    set(text_arg)
    if(_OPENSSL_TEXT)
        set(text_arg -text)
    endif()

    set(digest_arg)
    if(_OPENSSL_DIGEST)
        set(digest_arg "-${_OPENSSL_DIGEST}")
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
    elseif(_OPENSSL_EDPARAM_NAME)
        set(key_arg -newkey "${_OPENSSL_EDPARAM_NAME}" -keyout "${_OPENSSL_OUTPUT_KEY_FILE}")
        set(key_dep)
        set(key_output "${_OPENSSL_OUTPUT_KEY_FILE}")
    else()
        # If we want to autocreate a key, the file is an output, not a dependency
        set(key_dep)
        set(key_output "${_OPENSSL_OUTPUT_KEY_FILE}")

        if(_OPENSSL_RSA_PSS_PADDING)
            set(key_arg -newkey rsa-pss -pkeyopt rsa_keygen_bits:2048
                    -keyout ${key_output})
        else()
            set(key_arg -newkey rsa:2048 -keyout ${key_output})
        endif()
    endif()

    get_serial_number(BASE_NAME ${_OPENSSL_OUTPUT_CERT_FILE})

    # Create the certificate
    add_custom_command(
        VERBATIM
        COMMENT "Generating certificate: ${_OPENSSL_OUTPUT_CERT_FILE}"
        OUTPUT
            ${_OPENSSL_OUTPUT_CERT_FILE}
            ${key_output}
        WORKING_DIRECTORY "${_OPENSSL_WORKING_DIRECTORY}"
        # Pre-requesites: create folder and database
        COMMAND
            ${CMAKE_COMMAND} -E make_directory
                ${pem_dir} ${cert_dir} ${key_dir}
        # Create the certificate request
        COMMAND
            ${OPENSSL_COMMAND} req
                ${req_password}
                ${pkeyopt}
                -new ${key_arg}
                -config ${_OPENSSL_CONFIG_FILE}
                -out ${certRequestFile}
        # Certified with the CA
        COMMAND
            ${OPENSSL_COMMAND} x509
                -req -days ${_OPENSSL_DAYS}
                ${text_arg} ${digest_arg}
                -CA ${_OPENSSL_CA_CERT_FILE} -CAkey ${_OPENSSL_CA_KEY_FILE}
                -set_serial "0x${_${_OPENSSL_OUTPUT_CERT_FILE}_serial_number}"
                -extfile ${_OPENSSL_CA_CONFIG_FILE} ${ca_extension}
                ${ca_password_arg}
                -in ${certRequestFile}
                -out ${_OPENSSL_OUTPUT_CERT_FILE}
        # The certificate request is just a temporary file, so we remove it.
        COMMAND
            ${CMAKE_COMMAND} -E remove ${certRequestFile}
        DEPENDS
            ${key_dep}
            ${_OPENSSL_CONFIG_FILE}
            ${_OPENSSL_CA_CONFIG_FILE}
            ${_OPENSSL_CA_CERT_FILE}
            ${_OPENSSL_CA_KEY_FILE}
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
        CA_DATABASE_INDEX CA_CONFIG_FILE CA_CERT_FILE CA_KEY_PASSWORD DAYS
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

    # This file is used to make sure we revoke the certificates only after we
    # have a new _OPENSSL_CA_DATABASE_INDEX database.
    # We can't depend on _OPENSSL_CA_DATABASE_INDEX because we already have that
    # file at the beginning of the function call: we could end up revoking the
    # certificates first, and then removing/touching _OPENSSL_CA_DATABASE_INDEX.
    set(file_prevent_race_condition
        "${_OPENSSL_CA_DATABASE_INDEX}_timestamp.cmake")
    add_custom_command(
        VERBATIM
        COMMENT "Creating database: ${_OPENSSL_CA_DATABASE_INDEX}"
        OUTPUT
            "${_OPENSSL_CA_DATABASE_INDEX}"
            "${file_prevent_race_condition}"
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
        COMMAND
            ${CMAKE_COMMAND} -E touch ${file_prevent_race_condition}
        DEPENDS
            ${_OPENSSL_CERT_FILE}
            ${_OPENSSL_CA_CONFIG_FILE}
            ${_OPENSSL_CA_CERT_FILE}
    )

    set(gencrl_dependencies
        "${file_prevent_race_condition}")
    foreach(certFile ${_OPENSSL_CERT_FILE})
        add_custom_command(
            VERBATIM
            COMMENT "Revoking certificate: ${certFile}"
            OUTPUT
                "${certFile}Revoked"
            COMMAND
                ${CMAKE_COMMAND} -E chdir ${_OPENSSL_WORKING_DIRECTORY}
                ${CMAKE_COMMAND}
                    "-DOPENSSL_COMMAND=${OPENSSL_EXECUTABLE}"
                    "-DCA_CONFIG_FILE=${_OPENSSL_CA_CONFIG_FILE}"
                    "-DCERT_FILE=${certFile}"
                    "-DCA_KEY_PASSWORD=${ca_password_arg}"
                    -P "${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/RevokeCertificate.cmake"
            COMMAND
                ${CMAKE_COMMAND} -DCONTENT=1 -DOUTPUT=${certFile}Revoked
                    -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/WriteFile.cmake
            DEPENDS
                ${gencrl_dependencies}
        )
        # Each custom command in the for loop depends on the previous
        # certificates being revoked.
        # This is to prevent more than one simultaneous call to openssl ca
        # (the command is not safe to call concurrently, see WARNINGS section
        # of the man pages).
        list(APPEND gencrl_dependencies "${certFile}Revoked")
    endforeach()

    set(optional_args)
    if(_OPENSSL_DAYS)
        list(APPEND optional_args -crldays ${_OPENSSL_DAYS})
    endif()
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
                ${optional_args}
        DEPENDS
            ${gencrl_dependencies}
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
    set(multi_args DEPENDS)
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
            ${_OPENSSL_DEPENDS}
            ${_OPENSSL_INPUT}
            ${_OPENSSL_SIGNER_CERTIFICATE}
            ${_OPENSSL_PRIVATE_KEY_FILE}
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
        OUTPUT ${_OPENSSL_OUTPUT}
        COMMAND
            ${CMAKE_COMMAND}
                "-DOUTPUT=${_OPENSSL_OUTPUT}"
                "-DINPUT=${_OPENSSL_INPUT}"
                -P ${CONNEXTDDS_RESOURCE_DIR}/cmake/Scripts/SimplifyCertificate.cmake
        DEPENDS
            ${_OPENSSL_INPUT}
    )

endfunction()
