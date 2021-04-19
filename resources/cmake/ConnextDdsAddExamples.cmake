# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst
.. connextdds_add_examples:

ConnextDdsAddExamples
---------------------

Function to build a bunch of examples.

::

    connextdds_add_examples(
        EXAMPLES example1 [...]
        [SKIP condition]
        [SKIP_MESSAGE message]
    )

Add a bunch of examples to be built. This method adds subdirectories that
contains an example with a CMakeList.txt and generates the build system
for all of them.

``EXAMPLES`` (required):
    List of examples names that can be compiled with the given MIN_VERSION
    of RTIConnextDDS.
``SKIP`` (optional):
    If defined, the examples will be skipped.
``SKIP_MESSAGE`` (optional):
    If the examples are going to be skipped, prints this message after
    ``Skipping examples because``.
]]

include(CMakeParseArguments)
include(ConnextDdsArgumentChecks)

function(add_examples)
    set(optional_args SKIP)
    set(single_value_args SKIP_MESSAGE)
    set(multi_value_args EXAMPLES)
    cmake_parse_arguments(_ADD_ARGUMENTS
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )
    connextdds_check_required_arguments(
        _ADD_ARGUMENTS_EXAMPLES
    )

    set(FOLDER_NAMES c c++98 c++ c++03 c++11 cmake)

    # Check if all directories exist 
    foreach(example ${_ADD_ARGUMENTS_EXAMPLES})
        if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}")
            message(FATAL_ERROR "${example} does not exist")
        endif()
    endforeach()

    # Check if skip
    if(_ADD_ARGUMENTS_SKIP)
        string(REPLACE ";" "\n\t- " SKIPPED_EXAMPLES "${_ADD_ARGUMENTS_EXAMPLES}")
        message(
            STATUS
                "Skipping examples because ${_ADD_ARGUMENTS_SKIP_MESSAGE}. "
                "The skipped examples are: \n\t- ${SKIPPED_EXAMPLES}"
        )
        return()
    endif()
    
    # Add existing examples
    foreach(example ${_ADD_ARGUMENTS_EXAMPLES})
        foreach(folder ${FOLDER_NAMES})
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}/${folder}")
                add_subdirectory(
                    "${CMAKE_CURRENT_SOURCE_DIR}/${example}/${folder}"
                )
            endif()
        endforeach()
    endforeach()
endfunction()
