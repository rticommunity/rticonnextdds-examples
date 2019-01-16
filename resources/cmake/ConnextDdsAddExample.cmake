# (c) 2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. connextdds_add_example:

ConnextDdsAddExample
--------------------

Function to build the examples.

::

    _connextdds_add_example(
        NAME name
        LANG language
        [DISABLE_SUBSCRIBER]
        [CODEGEN_ARGS ...]
    )

Add a new example to be built. This method will call Codegen, create the
CMake targets and set the dependencies.

``IDL`` (required):
    Name of the IDL file (without extension).
``LANG`` (required):
    Example language. Valid values are ``C``, ``C++``, ``C++03`` and ``C++11``.
``PREFIX``:
    Prefix name for the targets.
``DISABLE_SUBSCRIBER``:
    Disable the subscriber build.
``DEPENDENCIES``:
    Other target dependencies.
``CODEGEN_ARGS":
    Extra arguments for Codegen.

Output targets:
``folder name><_publisher_<lang>``
    Target for the publisher application.
``<folder name>_subscriber_<lang>``
    Target for the subscriber application.
``<folder name>_qos_profile_<lang>``
    Target to copy the USER_QOS_PROFILES.xml (if exists).
#]]

include_guard(DIRECTORY)

# Find the RTI Connext DDS libraries
if(NOT RTIConnextDDS_FOUND)
    find_package(RTIConnextDDS
        "5.3.0"
        REQUIRED
        COMPONENTS
            core
    )
endif()

include(CMakeParseArguments)
include(ConnextDdsArgumentChecks)
include(ConnextDdsCodegen)


function(connextdds_add_example)
    set(optional_args DISABLE_SUBSCRIBER REQUIRE_QOS)
    set(single_value_args IDL LANG PREFIX)
    set(multi_value_args CODEGEN_ARGS DEPENDENCIES)
    cmake_parse_arguments(_CONNEXT
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )
    connextdds_check_required_arguments(
        _CONNEXT_IDL
        _CONNEXT_LANG
    )


    # First, we call Codegen to generate the header and source files from the
    # IDL
    connextdds_rtiddsgen_run(
        IDL_FILE
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}.idl"
        OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/src"
        LANG ${_CONNEXT_LANG}
        ${_CONNEXT_CODEGEN_ARGS}
    )

    # If the GENERATE_EXAMPLE option was provided, Codegen will generate the
    # source code for the publisher and the subscriber applications
    # If we don't provide that parameter, we will use the source code provided
    # in the repository for the publisher and the subscriber
    connextdds_sanitize_language(LANG ${_CONNEXT_LANG} VAR lang_var)
    if(GENERATE_EXAMPLE IN_LIST _CONNEXT_CODEGEN_ARGS)
        set(publisher_src ${${_CONNEXT_IDL}_${lang_var}_PUBLISHER_SOURCES})
        set(subscriber_src ${${_CONNEXT_IDL}_${lang_var}_SUBSCRIBER_SOURCES})
    else()

        set(extension "c")
        if("${_CONNEXT_LANG}" STREQUAL "C++" OR
            "${_CONNEXT_LANG}" STREQUAL "C++03" OR
            "${_CONNEXT_LANG}" STREQUAL "C++11")
            set(extension "cxx")
        endif()

        set(publisher_src
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}_publisher.${extension}"
            ${${_CONNEXT_IDL}_${lang_var}_SOURCES}
        )
        set(subscriber_src
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}_subscriber.${extension}"
            ${${_CONNEXT_IDL}_${lang_var}_SOURCES}
        )
    endif()

    if(_CONNEXT_NO_REQUIRE_QOS)
        set(no_require_qos NO_REQUIRE_QOS)
    else()
        set(no_require_qos)
    endif()

    # Add the target for the publisher
    connextdds_add_application(
        TARGET "publisher"
        LANG ${_CONNEXT_LANG}
        SOURCES ${publisher_src}
        PREFIX ${_CONNEXT_PREFIX}
        OUTPUT_NAME "${_CONNEXT_IDL}_publisher"
        ${no_require_qos}
    )

    if(NOT _CONNEXT_DISABLE_SUBSCRIBER)
        # Add the target for the publisher
        connextdds_add_application(
            TARGET "subscriber"
            LANG ${_CONNEXT_LANG}
            SOURCES ${subscriber_src}
            PREFIX ${_CONNEXT_PREFIX}
            OUTPUT_NAME "${_CONNEXT_IDL}_subscriber"
            ${${no_require_qos}}
        )
    endif()
endfunction()


function(connextdds_copy_qos_profile)
    set(optional_args TARGET_PREFIX)
    set(single_value_args DEPENDANT_TARGET)
    set(multi_value_args)

    cmake_parse_arguments(_EXAMPLE_QOS
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    set(user_qos_profile_name "USER_QOS_PROFILES.xml")
    set(qos_file "${CMAKE_CURRENT_SOURCE_DIR}/${user_qos_profile_name}")

    if(NOT EXISTS ${qos_file})
        message(FATAL_ERROR "The ${user_qos_profile_name} was not found")
    endif()

    set(output_qos "${CMAKE_CURRENT_BINARY_DIR}/${user_qos_profile_name}")

    set(target_qos "${TARGET_PREFIX}qos")

    if(NOT TARGET ${target_qos})
        add_custom_target(${target_qos}
            DEPENDS
                ${output_qos}
        )

        add_custom_command(
            OUTPUT
                ${output_qos}
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    ${qos_file}
                    "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Copying USER_QOS_PROFILES.xml"
            DEPENDS
                ${qos_file}
            VERBATIM
        )
    endif()

    add_dependencies(${_EXAMPLE_QOS_DEPENDANT_TARGET}
        ${target_qos}
    )

endfunction()



function(connextdds_add_application)
    set(optional_args NO_REQUIRE_QOS)
    set(single_value_args TARGET LANG PREFIX OUTPUT_NAME)
    set(multi_value_args SOURCES DEPENDENCIES)

    cmake_parse_arguments(_CONNEXT
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )
    connextdds_check_required_arguments(
        _CONNEXT_TARGET
        _CONNEXT_LANG
        _CONNEXT_SOURCES
    )

    set(api "c")

    if("${_CONNEXT_LANG}" STREQUAL "C++")
        set(api "cpp")
    elseif("${_CONNEXT_LANG}" STREQUAL "C++03" OR
        "${_CONNEXT_LANG}" STREQUAL "C++11")
        set(api "cpp2")
        set(cxx_standard CXX_STANDARD 11)
    endif()

    if(_CONNEXT_PREFIX)
        set(target_name ${_CONNEXT_PREFIX}_${_CONNEXT_TARGET}_${api})
        set(qos_target ${_CONNEXT_PREFIX}_qos_${api})
    else()
        # If no prefix was given, the name of the example folder name is used
        # as prefix
        get_filename_component(
            folder_name
            "${CMAKE_CURRENT_SOURCE_DIR}"
            DIRECTORY)
        get_filename_component(
            folder_name
            "${folder_name}"
            NAME)
        set(target_name ${folder_name}_${_CONNEXT_TARGET}_${api})
        set(qos_target ${folder_name}_qos_${api})
    endif()

    # Create the target
    add_executable(${target_name} ${_CONNEXT_SOURCES})

    # Link all the dependencies and the RTI libraries for the API language
    target_link_libraries(${target_name}
        PRIVATE
            ${_CONNEXT_DEPENDENCIES}
            RTIConnextDDS::${api}_api
    )

    # Add the include directories
    target_include_directories(${target_name}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/src"
    )

    # Set output name for the binary
    set_target_properties(${target_name}
        PROPERTIES
            OUTPUT_NAME "${_CONNEXT_OUTPUT_NAME}"
            ${cxx_standard}
    )

    if(NOT _CONNEXT_NO_REQUIRE_QOS)
        connextdds_copy_qos_profile(
            TARGET_PREFIX "${_CONNEXT_IDL}_"
            DEPENDANT_TARGET "${target_name}"
        )
    endif()

endfunction()