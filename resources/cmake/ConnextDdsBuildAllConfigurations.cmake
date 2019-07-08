# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.

set(build_modes "Debug" "Release")
set(link_modes OFF ON)

if(NOT BUILD_DIR_NAME)
    set(BUILD_DIR_NAME "build")
endif()

if(STATIC_ANALYSIS)
    set(INTERCEPT_BUILD_CMD "intercept-build")
endif()

foreach(build_mode IN LISTS build_modes)
    foreach(link_mode IN LISTS link_modes)

        if(link_mode)
            set(link_type_name "Dynamic")
        else()
            set(link_type_name "Static")
        endif()

        message(STATUS "Building ${build_mode} with ${link_type_name} linking...")

        set(BINARY_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR_NAME}/${build_mode}-${link_type_name}")

        file(MAKE_DIRECTORY "${BINARY_DIR_PATH}")

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -DCMAKE_BUILD_TYPE=${build_mode}
                -DBUILD_SHARED_LIBS=${link_mode}
                ${CLANG_TIDY_ARG}
                ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")

        execute_process(
            COMMAND ${INTERCEPT_BUILD_CMD}
                ${CMAKE_COMMAND}
                    --build .
                    --config ${build_mode}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")

    endforeach()
endforeach()
