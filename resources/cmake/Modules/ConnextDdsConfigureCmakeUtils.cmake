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
variable to use this new repository modules.

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
        execute_process(
            COMMAND git submodule update --init --recursive
            ERROR_VARIABLE submodule_update_err
        )
        if(submodule_update_err)
            message(FATAL_ERROR
                "There was a problem initializing a submodule:\n"
                "${submodule_update_err}"
            )
        endif()
        unset(submodule_update_err)
    # This is not a git repository or git tool is not installed, so we are going
    # to download the rticonnextdds-cmake-utils zip file
    endif(git_submodule_status_err
        OR git_submodule_status_res STREQUAL "No such file or directory"
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
        # ExternalProject_Add(downloadCmakeUtils
        #     URL "https://github.com/rticommunity/rticonnextdds-cmake-utils/archive/refs/heads/release/${CONNEXTDDS_VERSION}.zip"
        #     BUILD_IN_SOURCE 1
        #     CONFIGURE_COMMAND ""
        #     BUILD_COMMAND ""
        #     INSTALL_COMMAND
        #         ${CMAKE_COMMAND} -E copy_directory "." "${CONNEXTDDS_CMAKE_UTILS_DIR"
        # )
    endif()


    # # The repository was downloaded without git tool or it is not installed in
    # # the system, so we have to manage it manually.
    # elseif(git_submodule_status_res)


    #     find_path(CONNEXTDDS_CMAKE_UTILS_MODULES_DIR
    #         NAMES
    #             "FindRTIConnextDDS.cmake"
    #         PATHS
    #             "${CONNEXTDDS_CMAKE_UTILS_DIR}/"
    #             "${CONNEXTDDS_EXAMPLES_CMAKE_DIR}/rticonnextdds-cmake-utils/"
    #         PATH_SUFFIXES
    #             "cmake/Modules/"
    #         NO_DEFAULT_PATH
    #     )
    #     if(CONNEXTDDS_CMAKE_UTILS_MODULES_DIR
    #         STREQUAL "CONNEXTDDS_CMAKE_UTILS_MODULES_DIR-NOTFOUND"
    #     )
    #         message(FATAL_ERROR
    #             "Please, clone the desired version of rticonnextdds-cmake-utils"
    #             " repository from the branches page"
    #             " https://github.com/rticommunity/rticonnextdds-cmake-utils/branches"
    #             " into the resources/cmake/rticonnextdds-cmake-utils directory"
    #         )
    #     endif()
    #     list(APPEND CMAKE_MODULE_PATH 
    #         "${CONNEXTDDS_CMAKE_UTILS_MODULES_DIR}/"
    #     )
    #     set(CONNEXTDDS_CMAKE_UTILS_CONFIGURED
    #         TRUE CACHE BOOL
    #         "rticonnextdds-cmake-utils repository is configured correctly" FORCE)

    # endif()


    # unset(git_submodule_status_res)
    # unset(git_submodule_status_out)
    # unset(git_submodule_status_err)



endfunction()

