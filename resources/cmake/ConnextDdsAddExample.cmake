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
``DISABLE_SUBSCRIBER``:
    Disable the subscriber build.
``CODEGEN_ARGS":
    Extra arguments for Codegen.

Output targets:
``<name>_publisher``
    Target for the publisher application.
``<name>_subscriber``
    Target for the subscriber application.
``qos_profile``
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
    set(optional_args DISABLE_SUBSCRIBER)
    set(single_value_args IDL LANG)
    set(multi_value_args CODEGEN_ARGS)
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

    get_filename_component(folder_name
        "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
        get_filename_component(folder_name
        "${folder_name}" NAME
    )

    connextdds_rtiddsgen_run(
        IDL_FILE
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}.idl"
        OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/src"
        LANG ${_CONNEXT_LANG}
        ${_CONNEXT_CODEGEN_ARGS}
    )

    connextdds_sanitize_language(LANG ${_CONNEXT_LANG} VAR lang_var)

    set(api "c")
    set(extension "c")
    if("${_CONNEXT_LANG}" STREQUAL "C++")
        set(api "cpp")
        set(extension "cxx")
    elseif("${_CONNEXT_LANG}" STREQUAL "C++03" OR
        "${_CONNEXT_LANG}" STREQUAL "C++11")
        set(api "cpp2")
        set(extension "cxx")
    endif()

    if(GENERATE_EXAMPLE IN_LIST _CONNEXT_CODEGEN_ARGS)
        set(publisher_src ${${_CONNEXT_IDL}_${lang_var}_PUBLISHER_SOURCES})
        set(subscriber_src ${${_CONNEXT_IDL}_${lang_var}_SUBSCRIBER_SOURCES})
    else()
        set(publisher_src
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}_publisher.${extension}"
            ${${_CONNEXT_IDL}_${lang_var}_SOURCES}
        )
        set(subscriber_src
            "${CMAKE_CURRENT_SOURCE_DIR}/${_CONNEXT_IDL}_subscriber.${extension}"
            ${${_CONNEXT_IDL}_${lang_var}_SOURCES}
        )
    endif()

    # Add the target for the publisher
    add_executable(${folder_name}_publisher_${api} ${publisher_src})

    target_link_libraries(${folder_name}_publisher_${api}
        PUBLIC
            RTIConnextDDS::${api}_api
    )

    target_include_directories(${folder_name}_publisher_${api}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/src"
    )

    set_target_properties(${folder_name}_publisher_${api}
        PROPERTIES
            OUTPUT_NAME "${_CONNEXT_IDL}_publisher"
    )

    # Add the target for the subscriber
    if(NOT _CONNEXT_DISABLE_SUBSCRIBER)
        add_executable(${folder_name}_subscriber_${api} ${subscriber_src})

        target_link_libraries(${folder_name}_subscriber_${api}
            PUBLIC
                RTIConnextDDS::${api}_api
        )

        target_include_directories(${folder_name}_subscriber_${api}
            PRIVATE
                "${CMAKE_CURRENT_BINARY_DIR}/src"
        )

        set_target_properties(${folder_name}_subscriber_${api}
            PROPERTIES
                OUTPUT_NAME "${_CONNEXT_IDL}_publisher"
        )
    endif()

    set(user_qos_profile_name "USER_QOS_PROFILES.xml")
    set(qos_file "${CMAKE_CURRENT_SOURCE_DIR}/${user_qos_profile_name}")
    if(EXISTS ${qos_file})
        set(output_qos "${CMAKE_CURRENT_BINARY_DIR}/${user_qos_profile_name}")

        add_custom_target(${folder_name}_qos_profiles_${api}
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

        add_dependencies(${folder_name}_publisher_${api}
            ${folder_name}_qos_profiles_${api}
        )

        if(NOT _CONNEXT_DISABLE_SUBSCRIBER)
            add_dependencies(${folder_name}_subscriber_${api}
                ${folder_name}_qos_profiles_${api}
            )
        endif()
    endif()
endfunction()