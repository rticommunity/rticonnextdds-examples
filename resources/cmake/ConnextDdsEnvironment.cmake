# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _connextdds_environment:

Environment
-----------

This module defines the following CMake global variables depending on the
host architecture:

``PLATFORM_LIBRARY_SEARCH_PATH_NAME``:
    name of the environment variable to set the list of search directories
    with dynamic libraries.

``PLATFORM_LIBRARY_SEARCH_PATH_SEP``:
    character separator between directories.

``PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP``:
    same as ``PLATFORM_LIBRARY_SEARCH_PATH_SEP`` but in a format that can be
    set as the ``ENVIRONMENT`` property of CTest tests.

``PLATFORM_LIBRARY_SEARCH_PATH_VAR``:
    the environment variable in its variable format to be use from a terminal.
    Used to retrieve its current content at runtime.


Get library search path
^^^^^^^^^^^^^^^^^^^^^^^
::

    connextdds_get_library_search_path_definition(
        OUTPUT output_var
        [CTEST_FORMAT]
        [DIRECTORIES dir1 dir2 ...]
    )

Prepend a list of directories to the dynamic libraries environment variable
(ex: LD_LIBRARY_PATH). It will return the new environment variable
in a variable.
If CTest format is not requested, the output will contain a reference to the
original environment variable instead of its current content. This will ensure
that the content is expanded at compile time instead of configuration time.
For the CTest format this is not possible since CTest won't expand it.

.. note:: If the variable is used as part of a ``add_custom_command`` or
    ``add_custom_target`` you shouldn't use the ``VERBATIM`` parameter since
    it would break the path separator and variable format.


Arguments:

``OUTPUT`` (mandatory):
    The name of the CMake variable to set with the output.

``CTEST_FORMAT`` (optional)
    Set the output variable for the ENVIRONMENT property of a CTest test.

``DIRECTORIES`` (optional):
    List of directories to prepend.
#]]

# Define the library search environment variable and their separator.
# We use this variable for running programs like OpenSSL, rtiddsgen server
# and to run unit tests. Since all these process will run in the host machine
# we check the architecture of the host. For that reason we are not setting
# the fields in the Platform/ directory, because they are included per target
# platform.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_LIBRARY_SEARCH_PATH_NAME "PATH")
    # In Windows, we cannot use the ";" character directly. The problem
    # is that COMMAND in add_custom_command receives a list, and therefore
    # the ";" symbol is considereed a list item delimiter and will be substituted
    # with a space (see BUILD-1020).
    set(PLATFORM_LIBRARY_SEARCH_PATH_SEP "$<SEMICOLON>")

    # ... but for CTest and the automation we can't use the expression generator
    # above because in the final file we will just have a ";" and multiple
    # envrionment variables are also separated with a semicolon. For CTest the
    # solution is to use \;
    set(PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP "\;")

    set(PLATFORM_LIBRARY_SEARCH_PATH_VAR "%${PLATFORM_LIBRARY_SEARCH_PATH_NAME}%")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux"
        OR CMAKE_SYSTEM_NAME STREQUAL "QNX")
    set(PLATFORM_LIBRARY_SEARCH_PATH_NAME "LD_LIBRARY_PATH")
    set(PLATFORM_LIBRARY_SEARCH_PATH_SEP ":")
    set(PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP "${PLATFORM_LIBRARY_SEARCH_PATH_SEP}")
    set(PLATFORM_LIBRARY_SEARCH_PATH_VAR "$$${PLATFORM_LIBRARY_SEARCH_PATH_NAME}")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_LIBRARY_SEARCH_PATH_NAME "DYLD_LIBRARY_PATH")
    set(PLATFORM_LIBRARY_SEARCH_PATH_SEP ":")
    set(PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP "${PLATFORM_LIBRARY_SEARCH_PATH_SEP}")
    set(PLATFORM_LIBRARY_SEARCH_PATH_VAR "$$${PLATFORM_LIBRARY_SEARCH_PATH_NAME}")
else()
    message("Missing library search path config for the platform")
endif()

function(connextdds_get_library_search_path_definition)
    set(options CTEST_FORMAT)
    set(single_value_args OUTPUT)
    set(multi_value_args DIRECTORIES)
    cmake_parse_arguments(_ARGS
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    # For CTest we need to use a different separator because it's stored in
    # CMake style format
    if(_ARGS_CTEST_FORMAT)
        set(sep "${PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP}")
        set(path_string "$ENV{${PLATFORM_LIBRARY_SEARCH_PATH_NAME}}")

        # Escape the semi-colon separator (Windows case)
        if(PLATFORM_LIBRARY_SEARCH_PATH_CTEST_SEP STREQUAL "\;")
            string(REPLACE "\\;" ";" path_string "${path_string}")
            string(REPLACE ";" "\\;" path_string "${path_string}")

            # We don't need quotes in the paths since we quote the full env var
            string(REPLACE "\\\"" "" path_string "${path_string}")
            string(REPLACE "\"" "" path_string "${path_string}")
        endif()

        set(env_var "${path_string}")
    else()
        set(sep "${PLATFORM_LIBRARY_SEARCH_PATH_SEP}")
        set(env_var "${PLATFORM_LIBRARY_SEARCH_PATH_VAR}")
    endif()

    # Remove duplicates
    if(_ARGS_DIRECTORIES)
        list(REMOVE_DUPLICATES _ARGS_DIRECTORIES)
    endif()

    # Prepend the new paths
    set(path_list "${env_var}")
    foreach(path ${_ARGS_DIRECTORIES})
        # We get the native path with a generator expression for the case
        # when "path" is already a generator expression too.
        set(path_list "$<SHELL_PATH:${path}>${sep}${path_list}")
    endforeach()

    set(${_ARGS_OUTPUT}
        "${PLATFORM_LIBRARY_SEARCH_PATH_NAME}=${path_list}"
        PARENT_SCOPE
    )
endfunction()
