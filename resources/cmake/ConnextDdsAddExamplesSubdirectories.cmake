# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS.  Licensee may
# redistribute copies of the software provided that all such copies are
# subject to this license. The software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is
# under no obligation to maintain or support the software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.

#[[.rst
.. connextdds_add_examples_subdirectories:

ConnextDdsAddExamplesSubdirectories
-----------------------------------

Function to build all the language directories under the provided examples.

::

    connextdds_add_examples_subdirectories(
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

function(connextdds_add_examples_subdirectories)
    set(optional_args SKIP)
    set(single_value_args SKIP_MESSAGE)
    set(multi_value_args EXAMPLES)
    cmake_parse_arguments(_ADD_EXAMPLES_ARGS
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )
    connextdds_check_required_arguments(
        _ADD_EXAMPLES_ARGS_EXAMPLES
    )

    set(LANGUAGE_FOLDER_NAMES c c++98 c++ c++03 c++11)

    # Check if all directories exist 
    foreach(example ${_ADD_EXAMPLES_ARGS_EXAMPLES})
        if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}")
            message(FATAL_ERROR "${example} does not exist")
        endif()
    endforeach()

    # Check if skip
    if(_ADD_EXAMPLES_ARGS_SKIP)
        string(REPLACE ";" "\n\t- " SKIPPED_EXAMPLES "${_ADD_EXAMPLES_ARGS_EXAMPLES}")
        message(
            STATUS
                "Skipping examples because ${_ADD_EXAMPLES_ARGS_SKIP_MESSAGE}. "
                "The skipped examples are: \n\t- ${SKIPPED_EXAMPLES}"
        )
        return()
    endif()

    # Add existing examples
    foreach(example_name ${_ADD_EXAMPLES_ARGS_EXAMPLES})
        foreach(language_folder ${LANGUAGE_FOLDER_NAMES})
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/${language_folder}")
                add_subdirectory(
                    "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}/${language_folder}"
                )
            endif()
        endforeach()
    endforeach()
endfunction()
