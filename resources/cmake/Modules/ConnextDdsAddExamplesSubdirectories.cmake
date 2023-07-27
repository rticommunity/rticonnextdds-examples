# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#

#[[.rst
.. connextdds_add_examples_subdirectories:

ConnextDdsAddExamplesSubdirectories
-----------------------------------

Function to build all the language directories under the provided examples.

::

    connextdds_add_examples_subdirectories(
        EXAMPLES example1 [...]
    )

Add a bunch of examples to be built. This method adds subdirectories that
contains an example with a CMakeList.txt and generates the build system
for all of them.

``EXAMPLES`` (required):
    List of examples names that will be compiled.
]]

include(CMakeParseArguments)

function(connextdds_add_examples_subdirectories)
    set(optional_args)
    set(single_value_args)
    set(multi_value_args EXAMPLES)
    cmake_parse_arguments(_ADD_EXAMPLES_ARGS
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    if(NOT _ADD_EXAMPLES_ARGS_EXAMPLES)
        message(WARNING "No examples specified, skipping directory.")
        return()
    endif()

    set(LANGUAGE_FOLDER_NAMES c c++98 c++11)

    # Check if all directories exist 
    foreach(example ${_ADD_EXAMPLES_ARGS_EXAMPLES})
        if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${example}")
            message(FATAL_ERROR "${example} does not exist")
        endif()
    endforeach()

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
