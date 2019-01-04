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
    set(optional_args DISABLE_SUBSCRIBER)
    set(single_value_args IDL LANG PREFIX)
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

    if(_CONNEXT_PREFIX)
        set(target_publisher ${_CONNEXT_PREFIX}_publisher_${api})
        set(target_subscriber ${_CONNEXT_PREFIX}_subscriber_${api})
        set(target_qos ${_CONNEXT_PREFIX}_qos_${api})
    else()
        set(target_publisher ${folder_name}_publisher_${api})
        set(target_subscriber ${folder_name}_subscriber_${api})
        set(target_qos ${folder_name}_qos_${api})
    endif()



    # Add the target for the publisher
    add_executable(${target_publisher} ${publisher_src})

    target_link_libraries(${target_publisher}
        PRIVATE
            RTIConnextDDS::${api}_api
    )

    target_include_directories(${target_publisher}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/src"
    )

    set_target_properties(${target_publisher}
        PROPERTIES
            OUTPUT_NAME "${_CONNEXT_IDL}_publisher"
    )

    # Add the target for the subscriber
    if(NOT _CONNEXT_DISABLE_SUBSCRIBER)
        add_executable(${target_subscriber} ${subscriber_src})

        target_link_libraries(${target_subscriber}
            PRIVATE
                RTIConnextDDS::${api}_api
        )

        target_include_directories(${target_subscriber}
            PRIVATE
                "${CMAKE_CURRENT_BINARY_DIR}/src"
        )

        set_target_properties(${target_subscriber}
            PROPERTIES
                OUTPUT_NAME "${_CONNEXT_IDL}_subscriber"
        )
    endif()

    set(user_qos_profile_name "USER_QOS_PROFILES.xml")
    set(qos_file "${CMAKE_CURRENT_SOURCE_DIR}/${user_qos_profile_name}")
    if(EXISTS ${qos_file})
        set(output_qos "${CMAKE_CURRENT_BINARY_DIR}/${user_qos_profile_name}")

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

        add_dependencies(${target_publisher}
            ${target_qos}
        )

        if(NOT _CONNEXT_DISABLE_SUBSCRIBER)
            add_dependencies(${target_subscriber}
                ${target_qos}
            )
        endif()
    endif()
endfunction()