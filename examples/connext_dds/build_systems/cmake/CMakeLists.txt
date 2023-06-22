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
project(rtiexamples-hello-world)

# Add the folder where the FindRTIConnextDDS.cmake file is placed to the
# CMAKE_MODULE_PATH variable
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

# Find the ConnextDDS libraries. This will look fo the core and API libraries
# only
find_package(RTIConnextDDS
    "7.0.0"
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

add_custom_target(codegen_sources
    DEPENDS
        ${HelloWorld_C_GENERATED_SOURCES}
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
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

# Create the target for the subscriber executable
add_executable(HelloWorld_subscriber
    "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld_subscriber.c"
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
    "${CMAKE_CURRENT_BINARY_DIR}/src"
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

# Copy the USER_QOS_PROFILES.XML when the publisher is built and ensure
# Codegen is called
add_dependencies(HelloWorld_publisher
    copy_qos
    codegen_sources
)

# Copy the USER_QOS_PROFILES.XML when the subscriber is built and ensure
# Codegen is called
add_dependencies(HelloWorld_subscriber
    copy_qos
    codegen_sources
)
