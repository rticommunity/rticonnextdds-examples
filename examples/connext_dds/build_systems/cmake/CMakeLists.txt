# (c) 2019 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.5)
project(rtiexamples-hello-world)

# Add the folder where the FindRTIConnextDDS.cmake file is placed to the
# CMAKE_MODULE_PATH variable
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Find the ConnextDDS libraries. This will look fo the core and API libraries
# only
find_package(RTIConnextDDS
    "5.3.0"
    REQUIRED
    COMPONENTS
        core
)

# Run Codegen to generate the source code files for the types
include(ConnextDdsCodegen)
connextdds_rtiddsgen_run(
    IDL_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld.idl"
    OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/src"
    LANG C
)

# Create the target for the publisher executable
add_executable(HelloWorld_publisher
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld_publisher.c"
    ${HelloWorld_C_GENERATED_SOURCES}
)

# Link against the ConnextDDS libraries
target_link_libraries(
    HelloWorld_publisher
    PRIVATE
        RTIConnextDDS::c_api
)

# Include the folder with the type header files
target_include_directories(HelloWorld_publisher
    PRIVATE
        "${CMAKE_BINARY_DIR}/src"
)

# Create the target for the publisher executable
add_executable(HelloWorld_subscriber
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld_publisher.c"
    ${HelloWorld_C_GENERATED_SOURCES}
)

# Link against the ConnextDDS libraries
target_link_libraries(
    HelloWorld_subscriber
    PRIVATE
        RTIConnextDDS::c_api
)
# Include the folder with the type header files
target_include_directories(HelloWorld_subscriber
    PRIVATE
        "${CMAKE_BINARY_DIR}/src"
)


# Copy the USER_QOS_PROFILES.xml
add_custom_target(copy_qos
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/USER_QOS_PROFILES.xml"
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Copying USER_QOS_PROFILES.xml"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml"
    VERBATIM
)

# Copy the USER_QOS_PROFILES.XML when the publisher is built
add_dependencies(HelloWorld_publisher
    copy_qos
)

# Copy the USER_QOS_PROFILES.XML when the subscriber is built
add_dependencies(HelloWorld_subscriber
    copy_qos
)
