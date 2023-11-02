#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form
#  only for use with RTI products.  The Software is provided "as is", with no
#  warranty of any type, including any warranty for fitness for any purpose.
#  RTI is under no obligation to maintain or support the Software.  RTI shall
#  not be liable for any incidental or consequential damages arising out of the
#  use or inability to use the software.
#
cmake_minimum_required(VERSION 3.11)
project (DistLoggerExample)

# Find RTI Connext dependencies
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

find_package(
    RTIConnextDDS "7.2.0"
    REQUIRED
    COMPONENTS
      core
      distributed_logger
)

# Main application
add_executable(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/DistLoggerExample.cxx"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::distributed_logger_cpp2
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CONNEXTDDS_INCLUDE_DIRS}"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES CXX_STANDARD 11)
