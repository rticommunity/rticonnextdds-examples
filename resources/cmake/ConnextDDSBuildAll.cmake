# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.

set(build_types "Debug" "Release")
set(link_types OFF ON)

if(NOT build_dir_name)
    set(build_dir_name "build")
endif()

if(CLANG_TIDY)
    set(CLANG_TIDY_ARG "-DCLANG_TIDY=ON")
endif()

foreach(build_type IN LISTS build_types)
    foreach(link_type IN LISTS link_types)

        if(link_type)
            set(link_type_name "Dynamic")
        else()
            set(link_type_name "Static")
        endif()

        message(STATUS 
            "Building ${build_type} with ${link_type_name} linking...")

        set(BINARY_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${build_dir_name}/${build_type}-${link_type_name}")

        file(MAKE_DIRECTORY "${BINARY_DIR_PATH}")

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -DCMAKE_BUILD_TYPE=${build_type}
                -DBUILD_SHARED_LIBS=${link_type}
                ${CLANG_TIDY_ARG}
                ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")
        execute_process(
            COMMAND ${CMAKE_COMMAND}
                --build .
                --config ${build_type}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")

    endforeach()
endforeach()
