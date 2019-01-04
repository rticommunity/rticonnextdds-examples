# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.5)
project(rtiexamples-connext-dds)

# Add the resources folder to the CMAKE_MODULE_PATH
if(NOT RTI_CMAKE_RESOURCES)
    find_package(Git)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
        OUTPUT_VARIABLE repository_root
        ERROR_VARIABLE command_error
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE return_value
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT return_value EQUAL 0)
        set(error
            "It was not possible to find the CMake resources folder needed to "
            "run this script. The folder is provided with the RTI Connext "
            "examples repository. You can point to it providing the "
            "-DRTI_CMAKE_RESOURCES= options to CMake")
        message(FATAL_ERROR ${error})
    endif()

    set(RTI_CMAKE_RESOURCES "${repository_root}/resources/cmake")

endif()

if(NOT EXISTS "${RTI_CMAKE_RESOURCES}")
    set(error
        "The directory ${RTI_CMAKE_RESOURCES} doesn't exists. Please, check "
        "that the repository was properly cloned or provide the path to the "
        "resources/cmake folder")
    message(FATAL_ERROR ${error})
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${RTI_CMAKE_RESOURCES}"
)

find_package(RTIConnextDDS
    "5.3.0"
    REQUIRED
    COMPONENTS
        core
)

# asynchronous_publication
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asynchronous_publication/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asynchronous_publication/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asynchronous_publication/c++03")

# asyncwaitset
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/asyncwaitset/c++03")

# batching
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/batching/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/batching/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/batching/c++03")

# builtin_qos_profiles
if(BUILD_SHARED_LIBS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_qos_profiles/c")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_qos_profiles/c++")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_qos_profiles/c++03")
endif()

# builtin_topics
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_topics/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_topics/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/builtin_topics/c++03")

# coherent_presentation
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/coherent_presentation/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/coherent_presentation/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/coherent_presentation/c++03")

# content_filtered_topic
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic/c++03")

# content_filtered_topic_string_filter
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic_string_filter/c")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic_string_filter/c++")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/content_filtered_topic_string_filter/c++03")

# custom_content_filter
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_content_filter/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_content_filter/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_content_filter/c++03")

# custom_flow_controller
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_flow_controller/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_flow_controller/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_flow_controller/c++03")

# custom_transport
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/custom_transport/c")

# deadline_contentfilter
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deadline_contentfilter/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deadline_contentfilter/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deadline_contentfilter/c++03")

# dynamic_data_access_union_discriminator
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_access_union_discriminator/c")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_access_union_discriminator/c++")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_access_union_discriminator/c++03")

# dynamic_data_nested_structs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_nested_structs/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_nested_structs/c++")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_nested_structs/c++03")

# dynamic_data_sequences
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_sequences/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_sequences/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_sequences/c++03")

# dynamic_data_using_publisher_subscriber
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_using_publisher_subscriber/c")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_using_publisher_subscriber/c++")
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_data_using_publisher_subscriber/c++11")

# get_publishers
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/get_publishers/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/get_publishers/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/get_publishers/c++03")

# high_priority_first_flow_controller
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/high_priority_first_flow_controller/c++")

# keyed_data
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data/c++03")

# keyed_data_advanced
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data_advanced/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data_advanced/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/keyed_data_advanced/c++03")

# lambda_content_filter
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lambda_content_filter/c++11")

# listeners
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/listeners/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/listeners/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/listeners/c++03")

# multichannel
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/multichannel/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/multichannel/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/multichannel/c++03")

# ordered_presentation
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation/c++11")

# ordered_presentation_group
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation_group/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation_group/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ordered_presentation_group/c++03")

# partitions
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/partitions/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/partitions/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/partitions/c++03")

# persistent_storage
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/persistent_storage/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/persistent_storage/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/persistent_storage/c++03")

# polling_querycondition
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_querycondition/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_querycondition/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_querycondition/c++03")

# polling_read
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_read/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_read/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/polling_read/c++03")

# property_qos
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/property_qos/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/property_qos/c++")

# recursive_data_type
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/recursive_data_type/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/recursive_data_type/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/recursive_data_type/c++03")

# time_based_filter
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/time_based_filter/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/time_based_filter/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/time_based_filter/c++03")

# using_qos_profiles
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_qos_profiles/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_qos_profiles/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_qos_profiles/c++03")

# using_sequences
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_sequences/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_sequences/c++")

# using_typecodes
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_typecodes/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_typecodes/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/using_typecodes/c++03")

# waitset_query_cond
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_query_cond/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_query_cond/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_query_cond/c++11")

# waitset_status_cond
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_status_cond/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_status_cond/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_status_cond/c++03")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitset_status_cond/c++11")

# waitsets
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitsets/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitsets/c++")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/waitsets/c++03")

# zero_copy
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/zero_copy/c++")