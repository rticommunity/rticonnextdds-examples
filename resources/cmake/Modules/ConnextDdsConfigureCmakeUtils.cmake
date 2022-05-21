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

Macro that performs the submodule initialization in order to setup the CMake
buildsystem.::

    connextdds_configure_cmake_utils()

Initializes the `rticonnextdds-cmake-utils
<https://github.com/rticommunity/rticonnextdds-cmake-utils>` git-submodule
which contains multiple util modules to build the examples. Then, it sets
up the `CMAKE_MODULE_PATH <https://cmake.org/cmake/help/latest/variable/CMAKE_MODULE_PATH.html>`
variable to use this new repository modules. If the rticonnextdds-examples
repository has been downloaded as a zip, or the git tool is unavailable, the
rticonnextdds-cmake-utils repository will be downloaded from GitHub directly.

#]]

set(CONNEXTDDS_CMAKE_UTILS_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../rticonnextdds-cmake-utils"
    CACHE PATH "rticonnextdds-cmake-utils repository dir" FORCE
)

function(connextdds_configure_cmake_utils)
    if(CONNEXTDDS_CMAKE_UTILS_CONFIGURED)
        return()
    endif()

    execute_process(
        COMMAND git submodule status "${CONNEXTDDS_CMAKE_UTILS_DIR}"
        RESULT_VARIABLE git_submodule_status_res
        OUTPUT_VARIABLE git_submodule_status_out
        ERROR_VARIABLE git_submodule_status_err
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )

    # This is a git repository and we can use the git submodules update command
    # in order to initialize the git-submodule rticonnextdds-cmake-utils
    if(git_submodule_status_out MATCHES "^-")
        message(STATUS
            "The rticonnextdds-cmake-utils submodule is yet not configured,"
            " initializing..."
        )
        execute_process(
            COMMAND git submodule update --init --recursive
            OUTPUT_VARIABLE submodule_update_out
            ERROR_VARIABLE submodule_update_err
        )
        if(NOT submodule_update_out)
            message(FATAL_ERROR
                "There was a problem initializing the submodule:\n"
                "${submodule_update_err}"
            )
        endif()
        unset(submodule_update_err)
    # This is not a git repository or git tool is not installed, so we are going
    # to download the rticonnextdds-cmake-utils zip file
    elseif(git_submodule_status_err
        OR git_submodule_status_res STREQUAL "No such file or directory"
    )
        message(STATUS
            "The rticonnextdds-examples installation is not a git repository or"
            " git tool is not installed. Downloading rticonnextdds-cmake-utils"
            " manually"
        )
        set(CONNEXTDDS_VERSION "6.1.1" CACHE STRING "")
        file(DOWNLOAD
            "https://github.com/rticommunity/rticonnextdds-cmake-utils/archive/refs/heads/release/${CONNEXTDDS_VERSION}.zip"
            "${CONNEXTDDS_CMAKE_UTILS_DIR}/rticonnextdds-cmake-utils.zip"
        )
        execute_process(
            COMMAND
                ${CMAKE_COMMAND} -E tar xf "${CONNEXTDDS_CMAKE_UTILS_DIR}/rticonnextdds-cmake-utils.zip" --format=zip "${CONNEXTDDS_CMAKE_UTILS_DIR}/"
            COMMAND
                ${CMAKE_COMMAND} -E remove "${CONNEXTDDS_CMAKE_UTILS_DIR}/rticonnextdds-cmake-utils.zip"
        )
    else()
        message(FATAL_ERROR
            "There was an unexpected outcome when trying to find the"
            " rticonnextdds-cmake-utils git-submodule")
    endif()

    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        "${CONNEXTDDS_CMAKE_UTILS_DIR}/cmake/Modules/"
        PARENT_SCOPE
    )
    set(CONNEXTDDS_CMAKE_UTILS_CONFIGURED TRUE CACHE BOOL "" FORCE)
endfunction()
