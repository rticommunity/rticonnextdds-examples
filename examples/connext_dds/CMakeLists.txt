#
# (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form
# only for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose.
# RTI is under no obligation to maintain or support the Software.  RTI shall
# not be liable for any incidental or consequential damages arising out of the
# use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project(rtiexamples-connext-dds)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

include(ConnextDdsAddExamplesSubdirectories)

# Find the ConnextDDS libraries. This will look for core and API libraries
find_package(RTIConnextDDS
    "7.0.0"
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

if(NOT DEFINED CONNEXTDDS_CONNEXT_DDS_EXAMPLES)
    set(CONNEXTDDS_CONNEXT_DDS_EXAMPLES
        "asynchronous_publication"
        "asyncwaitset"
        "batching"
        "builtin_qos_profiles"
        "builtin_topics"
        "coherent_presentation"
        "content_filtered_topic"
        "content_filtered_topic_string_filter"
        "custom_content_filter"
        "custom_flow_controller"
        "custom_transport"
        "deadline_contentfilter"
        # "discovery_snapshot"
        "dynamic_data_using_publisher_subscriber"
        "flat_data_api"
        "flat_data_latency"
        "get_publishers"
        "high_priority_first_flow_controller"
        "keyed_data"
        "keyed_data_advanced"
        "lambda_content_filter"
        "lbediscovery_xml_app_creation"
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
        "dynamic_data_request_reply"
        "dynamic_data_sequences"
        "fragmented_data_statistics"
        "group_coherent_presentation"
        "instance_statistics"
        "network_capture/01_shared_memory_and_udp"
        "network_capture/02_tcp"
        "network_capture/03_security"
        "network_capture/04_advanced_api"
        "printing_qos"
        "required_subscription"
    )
endif()

connextdds_add_examples_subdirectories(
    EXAMPLES
        ${CONNEXTDDS_CONNEXT_DDS_EXAMPLES}
)
