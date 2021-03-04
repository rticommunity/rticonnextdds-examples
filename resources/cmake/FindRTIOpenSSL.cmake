# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _find_rtiopenssl:

FindRTIOpenSSL
--------------

Find the OpenSSL libraries and executables.
In Android platforms it will search for our custom library ``rtisslsupport``.
In the rest of platforms, since we use the standard OpenSSL libraries,
it will fallback to the CMake FindOpenSSL module.

The list of paths to search the libraries are:

- ``OPENSSL_ROOT_DIR``
- Environment variable ``OPENSSL_ROOT_DIR``
- ``PLATFORM_TOOLCHAIN_PATH/openssl/<version>``

For executables, it searchs in the host system paths. In the case of native
builds, it searchs in the same list of paths of the libraries first. To set
a specific path you can set ``OPENSSL_EXECUTABLE``.

CMake doesn't support chars in the version number (e.g.: '1.0.2g').
To make sure you are detecting the expected version you need to specify
the version in the variable ``OPENSSL_FULL_VERSION`` and the find module will do
a string comparison.

Output variables related to libraries:

- ``OPENSSL_SSL_LIBRARY``: The SSL library.
- ``OPENSSL_CRYPTO_LIBRARY``: The Crypto library.
- ``OPENSSL_LIBRARIES``: All the OpenSSL related libraries.
- ``OPENSSL_VERSION``: The version of the OpenSSL libraries.
- ``OpenSSL_SSL``: imported target for SSL library.
- ``OpenSSL_Crypto``: imported target for Crypto library.

Output variables related to executables:

- ``OPENSSL_EXECUTABLE``: The 'openssl' executable.
- ``OPENSSL_EXECUTABLE_VERSION``: The version of the 'openssl' executable.
- ``OPENSSL_HOST_PATH_ENV``: The list of paths to append to the PATH or similar
  environment variable to run the 'openssl' executable.
- ``OPENSSL_PATH_ENV``: The list of paths to append to the PATH or similar
  environment variable to run executables in the target machine.
- ``OPENSSL_DEFAULT_CONFIG_FILE``: The default openssl configuration file.

#]]

include(FindPackageHandleStandardArgs)
include(ConnextDdsImportedTargets)
include(ConnextDdsEnvironment)

##########################################################################
# Helper functions                                                       #
##########################################################################

#----------------------------------------------------------------------------#
# Helper function to convert an hexadecimal number into decimal
#----------------------------------------------------------------------------#
function(_hexstring2dec output_var hex)
    set(num 0)

    string(LENGTH ${hex} length)
    math(EXPR length "${length} - 1")
    foreach(i RANGE ${length})
        string(SUBSTRING ${hex} ${i} 1 hex_char)
        _hex2dec(dec ${hex_char})
        math(EXPR num "(${num} << 4) | ${dec}")
    endforeach()

    set(${output_var} ${num} PARENT_SCOPE)
endfunction()



#----------------------------------------------------------------------------#
# Helper function to convert an hexadecimal digit into decimal
#----------------------------------------------------------------------------#
function(_hex2dec output_var hex)
    string(TOLOWER ${hex} hex)
    if(hex STREQUAL "a")
        set(num 10)
    elseif(hex STREQUAL "b")
        set(num 11)
    elseif(hex STREQUAL "c")
        set(num 12)
    elseif(hex STREQUAL "d")
        set(num 13)
    elseif(hex STREQUAL "e")
        set(num 14)
    elseif(hex STREQUAL "f")
        set(num 15)
    elseif(hex MATCHES "[0-9]")
        set(num ${hex})
    else()
        message("Invalid hexadecimal char")
    endif()

    set(${output_var} ${num} PARENT_SCOPE)
endfunction()


#----------------------------------------------------------------------------#
# Helper function to extract the shared library ID
#----------------------------------------------------------------------------#
function(_rtiopenssl_get_shared_library_id library outvar)

    if(BUILD_SHARED_LIBS)
        if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            # We use `otool` for this task.
            # otool: object file displaying tool.
            find_program(OTOOL
                NAMES
                    "otool"
                DOC "The otool program"
            )
            if(NOT OTOOL)
                message("otool not found")
            endif()

            execute_process(
                COMMAND
                    ${OTOOL} -D ${library} -X
                OUTPUT_VARIABLE otool_output
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE otool_retcode
            )

            # If everything goes properly, the return code is 0
            if(otool_retcode)
                message("Failed to run otool")
            endif()

            string(REPLACE "\n" "" shared_library_id ${otool_output})
            # Get the path to the real library
            set(${outvar} "${shared_library_id}" PARENT_SCOPE)
        elseif(NOT WIN32)
            # Get the path to the real library
            get_filename_component(
                resolved_library_full ${library} REALPATH
            )
            set(${outvar} ${resolved_library_full} PARENT_SCOPE)
        else()
            message("This functionality is not supported in Windows")
        endif()
    else()
        message("This functionality is only supported for shared libs")
    endif()
endfunction()



#----------------------------------------------------------------------------#
# Search the Android libraries.
# For Android, we have a customized library that is called "rtisslsupport".
#----------------------------------------------------------------------------#
macro(_rtiopenssl_find_android)
    ######### Basename for the variable where the library will be stored
    set(ssl_basename_var "RTISSLSUPPORT")
    set(crypto_basename_var "RTISSLSUPPORT")

    ######### Search the library
    find_library(${ssl_basename_var}_LIBRARY
        NAMES
            "rtisslsupport"
        HINTS
            "${OPENSSL_ROOT_DIR}/lib"
            "$ENV{OPENSSL_ROOT_DIR}/lib/"
    )

    set(${crypto_basename_var}_LIBRARY "${${ssl_basename_var}_LIBRARY}")

    ######### Search for the include directories
    # Since we have to look for other library, we can not use the builtin
    # CMake Find Package for OpenSSL. As a consequence, we need to search
    # the include directories
    find_path(OPENSSL_INCLUDE_DIR
        NAMES
            "openssl/ssl.h"
        HINTS
            "${OPENSSL_ROOT_DIR}/include"
            "$ENV{OPENSSL_ROOT_DIR}/include/"
    )

    ######### Get the OpenSSL found version
    # As with the include directories, we also have to search the version of
    # the found libraries
    if(OPENSSL_INCLUDE_DIR)
        # The version is encoded in hexadecimal
        set(hex "[0-9a-fA-F]")

        # Search the version string
        file(STRINGS
            "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h"
            version_header
            REGEX
            "OPENSSL_VERSION_NUMBER[\t ]+0x${hex}+"
        )

        # If we found the header with the version number, we decode it
        if(version_header)
            string(REGEX MATCH
                "0x(${hex})(${hex}${hex})(${hex}${hex})(${hex}${hex})"
                match
                ${version_header}
            )

            if(match)
                _hexstring2dec(major ${CMAKE_MATCH_1})
                _hexstring2dec(minor ${CMAKE_MATCH_2})
                _hexstring2dec(fix ${CMAKE_MATCH_3})
                if(CMAKE_MATCH_4)
                    # Convert the patch number from hex to char. Patch version
                    # can be there or not
                    _hexstring2dec(patch ${CMAKE_MATCH_4})
                    math(EXPR patch "${patch} + 96")
                    string(ASCII ${patch} patch)
                else()
                    set(patch)
                endif()

                set(OPENSSL_VERSION "${major}.${minor}.${fix}${patch}")
            endif()
        endif()
    endif()
endmacro()



#----------------------------------------------------------------------------#
# In the case of the "standard" OpenSSL, we rely on the CMake builtin
# find package module. So, in this macro, we just configure the call to
# find_package(OpenSSL) with the needed arguments
#----------------------------------------------------------------------------#
macro(_rtiopenssl_find_standard)
    ######### Basename for the variable where the SSL library will be stored
    set(ssl_basename_var "OPENSSL_SSL")
    set(crypto_basename_var "OPENSSL_CRYPTO")

    ######### Configure the arguments to pass to the builtin OpenSSL CMake
    # find package
    set(optional_args)

    # Required
    if(RTIOpenSSL_FIND_REQUIRED)
        list(APPEND optional_args REQUIRED)
    endif()

    # Full version matching done later
    if(NOT OPENSSL_FULL_VERSION AND RTIOpenSSL_FIND_VERSION)
        list(APPEND optional_args ${RTIOpenSSL_FIND_VERSION})
    endif()

    if(NOT BUILD_SHARED_LIBS)
        # This variable is an input variable for the builtin OpenSSL find
        # package CMake script. When this variable is set to `TRUE`, the
        # CMake script will look for static libraries
        set(OPENSSL_USE_STATIC_LIBS TRUE)
        if(WIN32)
            # This variable is an input variable for the builtin OpenSSL find
            # package CMake script. When this variable is set to `TRUE`, the
            # CMake script will look for the MT version of the libraries
            set(OPENSSL_MSVC_STATIC_RT TRUE)
        endif()
    endif()

    ######### Search the libraries
    # Call the builtin CMake find package with the requested parameters
    find_package(OpenSSL ${optional_args})
endmacro()



#----------------------------------------------------------------------------#
# Find the OpenSSL program
#----------------------------------------------------------------------------#
macro(_rtiopenssl_find_program)
    ######### Define search paths
    # If we are not cross-compiling we can search the binary in the root dir.
    # Otherwise, the executable may not run in the host platform so CMake
    # will search in the host default paths (like PATH environment variable).
    set(search_paths)
    if(NOT CMAKE_CROSSCOMPILING)
        set(search_paths
            "${OPENSSL_ROOT_DIR}/bin"
            "$ENV{OPENSSL_ROOT_DIR}/bin"
        )
    endif()

    ######### Search the executable
    find_program(OPENSSL_EXECUTABLE
        NAMES
            "openssl"
        HINTS
            ${search_paths}
        )

    if(OPENSSL_EXECUTABLE)
        message("Found OpenSSL program: ${OPENSSL_EXECUTABLE}")
    endif()

    # Get the path to the dynamic libraries to which the OpenSSL program may
    # be linked if compiled in dynamic mode. This is needed in Windows since
    # it does not support the rpath mechanism. The libraries are usually in
    # the `bin` dir. We use the `lib` and the `bin` directory.
    #
    # IMPORTANT: We can't use the libraries found previously because they are
    # for the target architecture, not for the host machine and those
    # libraries can be not compatible with the host machine.
    get_filename_component(bin_dir ${OPENSSL_EXECUTABLE} DIRECTORY)
    get_filename_component(parent_dir ${bin_dir} DIRECTORY)
    set(OPENSSL_HOST_PATH_ENV "${parent_dir}/lib" ${bin_dir})

    ######### Search the openssl.cnf file
    # Get the default OpenSSL config file to set in the
    # `OPENSSL_CONF` environment variable
    # This file is needed to run OpenSSL
    find_file(OPENSSL_DEFAULT_CONFIG_FILE
        NAMES
            "openssl.cnf"
        PATHS
            "${bin_dir}/../ssl"
            "${OPENSSL_ROOT_DIR}/ssl/"
            "$ENV{OPENSSL_ROOT_DIR}/ssl/"
            "${bin_dir}/../"
            "${OPENSSL_ROOT_DIR}"
            "$ENV{OPENSSL_ROOT_DIR}"
    )

    if(NOT OPENSSL_DEFAULT_CONFIG_FILE)
        message("Cannot find the default OpenSSL config file")
    endif()

    ######### Get the version

    # Get the environment variable paths to set for running OpenSSL (where
    # the libraries are placed just in case the OpenSSL executable was built
    # dynamically)
    connextdds_get_library_search_path_definition(
        OUTPUT lib_search_def
        DIRECTORIES
            ${OPENSSL_HOST_PATH_ENV}
    )

    # Execute OpenSSL to get the version
    execute_process(
        COMMAND
            ${CMAKE_COMMAND} -E env
                "${lib_search_def}"
                OPENSSL_CONF=${OPENSSL_DEFAULT_CONFIG_FILE}
                ${OPENSSL_EXECUTABLE} version
        OUTPUT_VARIABLE program_version
    )

    # Parse the version
    if(program_version)
        string(REGEX MATCH
            "OpenSSL ([0-9]+)\.([0-9]+)\.([0-9]+)([a-z]?)"
            match
            ${program_version}
        )
        if(match)
            set(OPENSSL_EXECUTABLE_VERSION
                "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}${CMAKE_MATCH_4}"
            )
            if(NOT OPENSSL_EXECUTABLE_VERSION STREQUAL OPENSSL_VERSION)
                message("OpenSSL executable version "
                    "(${OPENSSL_EXECUTABLE_VERSION}) is different to the "
                    "library version (${OPENSSL_VERSION})"
                )
            endif()
        endif()
    else()
        message("Cannot run OpenSSL: '${OPENSSL_EXECUTABLE}'")
    endif()
endmacro()



##########################################################################
# Check the environment                                                  #
##########################################################################

######### Check the OpenSSL version to search
if(OPENSSL_FULL_VERSION)
    # If we specified the full, we use it
    set(version_dir ${OPENSSL_FULL_VERSION})
else()
    # Otherwise, we use the version provided in the call to the CMake builtin
    # macro `find_package`
    set(version_dir ${RTIOpenSSL_FIND_VERSION})
endif()

######### Path to search the OpenSSL stuff
# The find package for OpenSSL will try to find it using the
# `OPENSSL_ROOT_DIR` CMake variable. If it was not set, we set it to a path
# under the PLATFORM_TOOLCHAIN_PATH to check if it is there as last resource
if(NOT OPENSSL_ROOT_DIR)
    set(OPENSSL_ROOT_DIR
        "${PLATFORM_TOOLCHAIN_PATH}/openssl/${version_dir}"
        CACHE PATH "OpenSSL root directory"
    )
endif()

# We append to FIND_ROOT_PATH to allow searching in cross-compiling scenarios
if(CMAKE_CROSSCOMPILING)
    # We insert it the first one to ensure our folder is the first to be read
    list(INSERT CMAKE_FIND_ROOT_PATH 0 ${OPENSSL_ROOT_DIR})
endif()

if(NOT BUILD_SHARED_LIBS)
    # For static builds, we search the static libraries. Use the "z" suffix to
    # search the libraries.
    #
    # We store the original value of `CMAKE_FIND_LIBRARY_SUFFIXES` in a
    # temporary variable to restore it after finding the libraries.
    # The reason to do this is:
    # The OpenSSL convention is to create static libraries whose suffix is
    # "z.a" (Unix like systems) and "z.lib" (Windows). Other third-party will
    # use standard suffixes. So we need to recover the original value in the
    # `CMAKE_FIND_LIBRARY_SUFFIXES` CMake variable to ensure we can find the
    # other third-party libraries
    set(original_find_library_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES "z.a" "z.lib")
endif()

##########################################################################
# Search the libraries                                                   #
##########################################################################
if(ANDROID)
    # In Android we don't follow the standard lib names so we can't use
    # the built-in CMake find module for OpenSSL
    _rtiopenssl_find_android()
else()
    # For the rest of platforms, we use the built-in CMake FindOpenSSL.
    # This way we don't duplicate the logic to find the libraries
    # by looking the registry or default paths
    _rtiopenssl_find_standard()
endif()

# If this variable is set, it means the libraries were found properly and
# can continue looking for the rest of libraries and executables.
# If not, the `find_package_handle_standard_args` CMake variable will take
# care of showing one error later with some help
if(OPENSSL_INCLUDE_DIR)

    ######### Check if the found library is the one we want to find
    if(OPENSSL_FULL_VERSION)
        if(NOT OPENSSL_VERSION STREQUAL OPENSSL_FULL_VERSION)
            message("Invalid OpenSSL version '${OPENSSL_VERSION}' detected. "
                "Expecting version '${OPENSSL_FULL_VERSION}'. "
                "Try to download it from Conan or set the -DOPENSSL_ROOT_DIR "
                "CMake parameter to the correct version.")
        endif()
    endif()

    ######### Find the OpenSSL program
    # We need the OpenSSL libraries for things like the security modules but
    # also the OpenSSL executable to be run in the build machine. We use the
    # OpenSSl program to generate different certificates, sign them...
    _rtiopenssl_find_program()

    # Get the path to the dynamic libraries to which the test programs may be
    # linked if compiled in dynamic mode. This is needed in Windows since it
    # does not support the rpath mechanism. The libraries are tipically located
    # in the bin dir. We use the lib and the bin directory.
    get_filename_component(lib_dir ${${ssl_basename_var}_LIBRARY} DIRECTORY)
    get_filename_component(parent_dir ${lib_dir} DIRECTORY)
    set(OPENSSL_PATH_ENV)
    if (WIN32 AND BUILD_SHARED_LIBS)
        set(OPENSSL_BIN "${parent_dir}/bin")
        list(APPEND OPENSSL_PATH_ENV ${OPENSSL_BIN})
        file(GLOB openssl_dll "${OPENSSL_BIN}/*.dll")
        set(OPENSSL_RUNTIME_LIBRARIES ${openssl_dll} CACHE PATH "")
    endif()
    list(APPEND OPENSSL_PATH_ENV ${lib_dir})
endif()

# Restore the library suffixes to not affect finding other third parties
if(NOT BUILD_SHARED_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${original_find_library_suffixes})
endif()


##########################################################################
# Assert the required element from the find package are set              #
##########################################################################
set(fail_msg "It was not possible to find OpenSSL. Try to download it using \
Conan or set the -DOPENSSL_ROOT_DIR CMake parameter. For 'openssl' program, \
make sure it's in your PATH or set the -DOPENSSL_EXECUTABLE CMake parameter")
find_package_handle_standard_args(
    "RTIOpenSSL"
    REQUIRED_VARS
        OPENSSL_INCLUDE_DIR
        OPENSSL_EXECUTABLE
    VERSION_VAR OPENSSL_VERSION
    FAIL_MESSAGE
        "${fail_msg}"
)

##########################################################################
# Create the imported targets                                            #
##########################################################################
if(RTIOPENSSL_FOUND)

    ######### The library type to import, depending on the linking
    if(BUILD_SHARED_LIBS)
        set(lib_type SHARED)
    else()
        set(lib_type STATIC)
    endif()

    # Create the imported target for the SSL library
    connextdds_create_imported_target(
        TARGET OpenSSL_SSL
        ${lib_type}
        BASENAME ${ssl_basename_var}
        INCLUDE_DIRS
            ${OPENSSL_INCLUDE_DIR}
        LANGUAGES "C"
    )

    # Create the imported target for the Crypto library
    connextdds_create_imported_target(
        TARGET OpenSSL_Crypto
        ${lib_type}
        BASENAME ${crypto_basename_var}
        INCLUDE_DIRS
            ${OPENSSL_INCLUDE_DIR}
        LANGUAGES "C"
    )

    if(ANDROID)
        # For Android, we use the `OpenSSL_SSL` and `OpenSSL_Crypto` imported
        # targets for importing the "rtisslsupport" library
        set_target_properties(OpenSSL_SSL OpenSSL_Crypto
            PROPERTIES
                # In CMake, when we link against an imported target, CMake
                # will try to get the soname of the library if the built-in
                # CMake variable `CMAKE_PLATFORM_USES_PATH_WHEN_NO_SONAME`
                # is equal to 1 (or `TRUE`) and will link against the library
                # using `-l<libraryname>`. If the library has not a soname,
                # CMake will use the complete path to the library in order to
                # link against it.
                # Setting the `IMPORTED_SONAME` target library doesn't change
                # the result (at least until CMake 3.18).
                # The `rtisslsupport` library has not a soname and, for that
                # reason, CMake is adding the complete path to the library.
                # This is a problem because the path to the `rtisslsupport`
                # will be hardcoded in the linked library/executable and it
                # will not be a real "dynamic" binary.
                # Setting the `IMPORTED_NO_SONAME` to `TRUE` solves this issue
                IMPORTED_NO_SONAME TRUE
                IMPORTED_SONAME "rtisslsupport"
        )
    endif()


    if(WIN32)
        if(BUILD_SHARED_LIBS)
            # OpenSSL is loaded at runtime from some tests.
            # For this reason, we need to set the `IMPORTED_LOCATION` CMake
            # target property.

            ######### Set the imported location for SSL
            # Find the DLLs for the SSL library
            find_file(OPENSSL_SSL_DLL
                NAMES
                    # OpenSSL 1.1.x
                    libssl-1_1-x64.dll
                    libssl-1_1.dll

                    # OpenSSL 1.0.x
                    ssleay32.dll
                HINTS
                    # OpenSSL stores the dll under the `bin` directory.
                    ${OPENSSL_BIN}
                NO_DEFAULT_PATH
            )
            # Set the found DLL in the target
            set_target_properties(OpenSSL_SSL
                PROPERTIES
                    IMPORTED_LOCATION ${OPENSSL_SSL_DLL}
            )

            ######### Set the imported location for Crypto
            # Find the DLLs for the Crypto library
            find_file(OPENSSL_CRYPTO_DLL
                NAMES
                    # OpenSSL 1.1.x
                    libcrypto-1_1-x64.dll
                    libcrypto-1_1.dll

                    # OpenSSL 1.0.x
                    libeay32.dll
                HINTS
                    ${OPENSSL_BIN}
                NO_DEFAULT_PATH
            )

            # Set the found DLL in the target
            set_target_properties(OpenSSL_Crypto
                PROPERTIES
                    IMPORTED_LOCATION ${OPENSSL_CRYPTO_DLL}
            )

        else()
            # We need to link also against `crypt32` when linking against
            # the Crypto library
            set_property(TARGET OpenSSL_Crypto
                APPEND
                    PROPERTY
                        INTERFACE_LINK_LIBRARIES crypt32.lib
            )
        endif()
    elseif(BUILD_SHARED_LIBS)
        # In UNIX, shared libraries found by the find_library are usually
        # a symbolic link to a library with ABI versioning information.
        # When using test bundles, we need both libraries to be included in
        # the bundle. See BUILD-1837
        _rtiopenssl_get_shared_library_id(
            ${${ssl_basename_var}_LIBRARY}
            ssl_shared_library_id
        )
        if(ssl_shared_library_id)
            get_filename_component(
                ssl_soname ${ssl_shared_library_id} NAME
            )
            set_target_properties(OpenSSL_SSL
                PROPERTIES
                    IMPORTED_NO_SONAME TRUE
                    IMPORTED_SONAME ${ssl_soname}
            )
        else()
            message("It was not possible to determine ssl shared library id")
        endif()

        # In UNIX, shared libraries found by the find_library are usually
        # a symbolic link to a library with ABI versioning information.
        # When using test bundles, we need both libraries to be included
        # in the bundle. See BUILD-1837
        _rtiopenssl_get_shared_library_id(
            ${${crypto_basename_var}_LIBRARY}
            crypto_shared_library_id
        )

        if(crypto_shared_library_id)
            # Get the soname
            get_filename_component(
                crypto_soname ${crypto_shared_library_id} NAME
            )

            # Set the imported soname
            set_target_properties(OpenSSL_Crypto
                PROPERTIES
                    IMPORTED_SONAME ${crypto_soname}
            )
        else()
            message("It was not possible to determine crypto shared library ID")

            if(crypto_shared_library_id)
                # Get the soname
                get_filename_component(
                    crypto_soname ${crypto_shared_library_id} NAME
                )

                # Set the imported soname
                set_target_properties(
                    OpenSSL_Crypto
                    PROPERTIES
                    IMPORTED_SONAME ${crypto_soname}
                )
            else()
                message("It was not possible to determine crypto shared library ID")
            endif()
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT BUILD_SHARED_LIBS)
        # TODO: BUILD-1157
        # OpenSSL 1.1.1 crypto has a dependency on the pthreads library. Until
        # we move to CMake 3.16 or greater, we need to add this dependency
        # manually.
        string(
            REGEX MATCH
            "1.1.1[a-z]?"
            match_version
            ${OPENSSL_VERSION}
        )
        if(match_version)
            find_package(Threads)
            set_property(
                TARGET
                OpenSSL_Crypto
                    APPEND
                        PROPERTY
                        INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
            )
        endif()
    endif()

endif()