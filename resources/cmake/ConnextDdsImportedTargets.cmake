# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _connextdds_create_imported_target:

Imported targets
----------------
::

    connextdds_create_imported_target(
        TARGET name
        BASENAME name
        STATIC|SHARED|UNKNOWN|INTERFACE
        LANGUAGES lang1 [...]
        [INCLUDE_DIRS ...]
        [COMPILE_DEFINITIONS ...]
    )

Create a virtual imported target. By Linking against these targets,
it will use its include directories and compile definitions. This is recommended
instead of using variables defined in find modules.

Arguments:

``TARGET`` (mandatory)
    Name of the target to create.

``BASENAME`` (mandatory)
    Prefix of the variables containing the libraries. The variables
    to search are: ``{BASENAME}_LIBRARY`` with the debug and/or release version
    of the libraries, ``{BASENAME}_LIBRARY_DEBUG`` with the debug version and
    ``{BASENAME}_LIBRARY_RELEASE`` for the release version.

``STATIC``, ``SHARED``, ``UNKNOWN``, ``INTERFACE`` (mandatory)
    Specify the configuration of the target libraries. Use ``UNKNOWN`` if you
    don't know if it's static or dynamic and ``INTERFACE`` for header-only
    targets.

``LANGUAGES`` (mandatory)
    List of languages supporting this library. Usually this is just C.

``INCLUDE_DIRS`` (optional)
    List of additional include directories to use with this target.

``COMPILE_DEFINITIONS`` (optional)
    List of additional compile definitions to use with this target.
#]]

include(CMakeParseArguments)
include(ConnextDdsArgumentChecks)

function(connextdds_create_imported_target)
    set(options STATIC SHARED UNKNOWN INTERFACE)
    set(single_value_args TARGET BASENAME)
    set(multi_value_args LANGUAGES INCLUDE_DIRS COMPILE_DEFINITIONS)
    cmake_parse_arguments(
        _IMPORTED
        "${options}" "${single_value_args}" "${multi_value_args}"
        ${ARGN}
    )

    connextdds_check_required_arguments(
        _IMPORTED_TARGET _IMPORTED_BASENAME _IMPORTED_LANGUAGES
    )

    if(NOT _IMPORTED_STATIC AND NOT _IMPORTED_SHARED AND NOT _IMPORTED_UNKNOWN
            AND NOT _IMPORTED_INTERFACE)
        message("Missing library type: static / shared / unknown / interface")
    endif()

    if(NOT ${_IMPORTED_BASENAME}_LIBRARY AND NOT _IMPORTED_INCLUDE_DIRS)
        message("Missing libraries/headers for imported target ${_IMPORTED_TARGET}")
    endif()

    # If it already exists, nothing to do.
    if(TARGET ${_IMPORTED_TARGET})
        return()
    endif()

    if(_IMPORTED_INTERFACE)
        # If there aren't libraries, it's a header-only so we need INTERFACE,
        # otherwise it will complain about missing libraries.
        set(library_type INTERFACE)
    elseif(_IMPORTED_STATIC)
        set(library_type STATIC)
    elseif(_IMPORTED_SHARED)
        set(library_type SHARED)
    elseif(_IMPORTED_UNKNOWN)
        # UNKNOWN reprents that we don't know if it's static or shared.
        set(library_type UNKNOWN)
    else()
        message("Missing library type for '${_IMPORTED_TARGET}'. "
            "Valid types are: STATIC, SHARED, UNKNOWN or INTERFACE"
        )
    endif()

    # Create the library
    add_library(${_IMPORTED_TARGET} ${library_type} IMPORTED)

    # Set the include directory and the default library
    set_target_properties(${_IMPORTED_TARGET} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_IMPORTED_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS "${_IMPORTED_COMPILE_DEFINITIONS}"
    )

    if(WIN32 AND _IMPORTED_SHARED)
        # For Windows shared libraries, IMPORTED_LOCATION must contain the
        # path to the DLL file. Since for linking we only need the .lib file
        # which is the one found by add_library, we only need to set
        # IMPORTED_IMPLIB.
        set(location_property IMPORTED_IMPLIB)
    else()
        set(location_property IMPORTED_LOCATION)
    endif()

    # Set the library
    if(EXISTS "${${_IMPORTED_BASENAME}_LIBRARY}")
        set_target_properties(${_IMPORTED_TARGET} PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "${_IMPORTED_LANGUAGES}"
            ${location_property} "${${_IMPORTED_BASENAME}_LIBRARY}"
        )
    endif()

    # Set the debug library
    if(EXISTS ${${_IMPORTED_BASENAME}_LIBRARY_DEBUG})
        set_property(TARGET ${_IMPORTED_TARGET} APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG
        )
        set_target_properties(${_IMPORTED_TARGET} PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "${_IMPORTED_LANGUAGES}"
            ${location_property}_DEBUG "${${_IMPORTED_BASENAME}_LIBRARY_DEBUG}"
        )
    endif()

    # Set the release library
    if(EXISTS ${${_IMPORTED_BASENAME}_LIBRARY_RELEASE})
        set_property(TARGET ${_IMPORTED_TARGET} APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE
        )
        set_target_properties(${_IMPORTED_TARGET} PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "${_IMPORTED_LANGUAGES}"
            ${location_property}_RELEASE "${${_IMPORTED_BASENAME}_LIBRARY_RELEASE}"
        )
    endif()
endfunction()
