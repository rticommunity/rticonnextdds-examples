# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-connext-dds)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake"
)

include(ConnextDdsAddExamples)

find_package(RTIConnextDDS
    "6.0.0"
    REQUIRED
    COMPONENTS
        core
)

if(STATIC_ANALYSIS)
    find_program(CPPCHECK_EXE NAMES "cppcheck")

    if(CPPCHECK_EXE)
        set(CPPCHECK_CMD
            ${CPPCHECK_EXE}
                --enable=style,unusedFunction
                --suppress=*:*/ndds/*
                --suppress=*:*/src/*
                --quiet
                --relative-paths)
        set(CMAKE_C_CPPCHECK "${CPPCHECK_CMD}")
        set(CMAKE_CXX_CPPCHECK "${CPPCHECK_CMD}")
    endif()

    set(static_analysis_script
        "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/static_analysis/run_static_analysis.py")
    add_custom_target(static_analysis
        COMMAND
            ${static_analysis_script}
                --binary-dir
                ${CMAKE_CURRENT_BINARY_DIR}
                --includes
                ${CONNEXTDDS_INCLUDE_DIRS}
                --defines
                ${CONNEXTDDS_COMPILE_DEFINITIONS}
        COMMENT
            "Running static analysis...")
    set_target_properties(static_analysis
        PROPERTIES
            EXCLUDE_FROM_ALL TRUE)
endif()

add_examples(
    EXAMPLES
        "asynchronous_publication"
        "asyncwaitset"
        "batching"
        "builtin_topics"
        "coherent_presentation"
        "content_filtered_topic"
        "content_filtered_topic_string_filter"
        "custom_content_filter"
        "custom_flow_controller"
        "deadline_contentfilter"
        "dynamic_data_using_publisher_subscriber"
        "flat_data_api"
        "flat_data_latency"
        "get_publishers"
        "high_priority_first_flow_controller"
        "keyed_data"
        "keyed_data_advanced"
        "lambda_content_filter"
        "listeners"
        "logging_config"
        "multichannel"
        "ordered_presentation"
        "ordered_presentation_group"
        "partitions"
        "polling_querycondition"
        "polling_read"
        "property_qos"
        "real_time_wan_transport"
        "time_based_filter"
        "using_qos_profiles"
        "using_sequences"
        "using_typecodes"
        "waitset_query_cond"
        "waitset_status_cond"
        "waitsets"
        "xml_application_env"
        "build_systems"
        "compression"
        "detect_samples_dropped"
        "dynamic_data_access_union_discriminator"
        "dynamic_data_nested_structs"
        "dynamic_data_sequences"
        "fragmented_data_statistics"
        "group_coherent_presentation"
        "instance_statistics"
        "network_capture/01_shared_memory_and_udp"
        "network_capture/02_tcp"
        "network_capture/03_security"
        "network_capture/04_advanced_api"
        "printing_qos"
)

# These conditionals could be replaced once we have updated to CMake 3.18.
# To do this, we could replace the if inside the add_examples function
# for cmake_language(EVAL CODE ...) and pass the condition as a string
# parameter instead of passing a boolean.
# Take a look here https://tinyurl.com/cmakelanguage
if(WIN32 OR NOT BUILD_SHARED_LIBS)
    set(SKIP_CUSTOM_EXTRA_ARGS
     SKIP
     SKIP_MESSAGE "it is not compatible with Windows and requires DBUILD_SHARED_LIBS=ON"
    )
endif()

add_examples(
    EXAMPLES
        "custom_transport"
    ${SKIP_CUSTOM_EXTRA_ARGS}
)

if(NOT BUILD_SHARED_LIBS)
    set(SKIP_BUILTIN_QOS_EXTRA_ARGS
        SKIP
        SKIP_MESSAGE "it requires DBUILD_SHARED_LIBS=ON"
    )
endif()

add_examples(
    EXAMPLES
        "builtin_qos_profiles"
    ${SKIP_BUILTIN_QOS_EXTRA_ARGS}
)
