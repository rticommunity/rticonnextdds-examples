set(build_types "Debug" "Release")
set(link_types OFF ON)

if(NOT build_dir_name)
    set(build_dir_name "build")
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
                ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")
        execute_process(
            COMMAND ${CMAKE_COMMAND}
                --build .
                --config ${build_type}
            WORKING_DIRECTORY "${BINARY_DIR_PATH}")

    endforeach()
endforeach()
