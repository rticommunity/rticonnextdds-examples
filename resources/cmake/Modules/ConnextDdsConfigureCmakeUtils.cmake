# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS.  Licensee may
# redistribute copies of the software provided that all such copies are
# subject to this license. The software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is
# under no obligation to maintain or support the software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.

#[[.rst:
.. _connextdds_configure_cmake_utils:

ConnexDdsConfigureCmakeUtils
----------------------------

Macro that checks that the submodule is properly configured in order to setup
the CMake buildsystem.::

    connextdds_configure_cmake_utils()

Checks if `rticonnextdds-cmake-utils
<https://github.com/rticommunity/rticonnextdds-cmake-utils>` git-submodule is
configured. If not an error will be shown that contains the command with which
set it up. This repo contains multiple util modules to build the examples. Then,
it sets up the
`CMAKE_MODULE_PATH <https://cmake.org/cmake/help/latest/variable/CMAKE_MODULE_PATH.html>`
variable to use this new repository modules. If the rticonnextdds-examples
repository has been downloaded as a zip, or the git tool is unavailable, an
error will be risen with useful information about how to download it from GitHub
directly.

#]]

set(CONNEXTDDS_EXAMPLES_RESOURCES_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    CACHE PATH "rticonnextdds-examples resource dir" FORCE
)
set(CONNEXTDDS_CMAKE_UTILS_DIR
    "${CONNEXTDDS_EXAMPLES_RESOURCES_DIR}/cmake/rticonnextdds-cmake-utils"
    CACHE PATH "rticonnextdds-cmake-utils repository dir"
)

function(connextdds_configure_cmake_utils)
    if(NOT "${CONNEXTDDS_CMAKE_UTILS_DIR}/cmake/Modules/" IN_LIST CMAKE_MODULE_PATH)
        set(CMAKE_MODULE_PATH
            ${CMAKE_MODULE_PATH}
            "${CONNEXTDDS_CMAKE_UTILS_DIR}/cmake/Modules/"
            PARENT_SCOPE
        )
    endif()

    if(CONNEXTDDS_CMAKE_UTILS_CONFIGURED)
        return()
    endif()

    execute_process(
        COMMAND git submodule status "${CONNEXTDDS_CMAKE_UTILS_DIR}"
        WORKING_DIRECTORY "${CONNEXTDDS_EXAMPLES_RESOURCES_DIR}"
        RESULT_VARIABLE git_submodule_status_res
        OUTPUT_VARIABLE git_submodule_status_out
        ERROR_VARIABLE git_submodule_status_err
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    # This is a git repository and the submodule is not initialized.
    if(git_submodule_status_out AND git_submodule_status_out MATCHES "^-")
        message(FATAL_ERROR
            "The rticonnextdds-cmake-utils submodule is yet not configured,"
            " please, run the following command in order to clone it from the "
            " root of the repository:\n\tgit submodule update --init --"
            " resources/cmake/rticonnextdds-cmake-utils\n"
        )
    # This is not a git repository or the git tool is not installed we ensure
    # there is no FindPackage script in the submodule, to ensure it is not
    # downloaded yet.
    elseif((git_submodule_status_err
            OR git_submodule_status_res STREQUAL "No such file or directory")
        AND NOT EXISTS "${CONNEXTDDS_CMAKE_UTILS_DIR}/cmake/Modules/FindRTIConnextDDS.cmake"
    )
        message(FATAL_ERROR
            "The rticonnextdds-examples installation is not a git repository or"
            " `git` tool is not installed. Please, in order to be able to build"
            " the examples, donwload the corresponding release from the"
            " rticonnextdds-cmake-utils repository and unzip it in its"
            " directory `resources/cmake/rticonnextdds-cmake-utils`:\n\t"
            "https://github.com/rticommunity/rticonnextdds-cmake-utils/releases\n"
        )
    endif()

    set(CONNEXTDDS_CMAKE_UTILS_CONFIGURED TRUE CACHE BOOL "" FORCE)
endfunction()
