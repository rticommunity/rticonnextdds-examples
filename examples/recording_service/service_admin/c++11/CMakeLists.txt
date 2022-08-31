#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
project (RecorderRemoteAdminC++11)

# Find RTI Connext dependencies
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake/Modules"
)
include(ConnextDdsConfigureCmakeUtils)
connextdds_configure_cmake_utils()

include(ConnextDdsAddExample)

find_package(
    RTIConnextDDS "6.1.1"
    REQUIRED
    COMPONENTS
        core
        messaging_api
        routing_service
        recording_service
)

# Main application
add_executable(
    Requester
    "${CMAKE_CURRENT_SOURCE_DIR}/Requester.cxx"
)

target_compile_features(Requester PRIVATE cxx_std_11)

target_link_libraries(
    Requester
    RTIConnextDDS::messaging_cpp2_api
    RTIConnextDDS::recording_service
    ${CONNEXTDDS_EXTERNAL_LIBS}
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_DefaultXml_"
    DEPENDANT_TARGET Requester
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_RecorderXml_"
    DEPENDANT_TARGET Requester
    FILENAME "recorder_remote_admin.xml"
)

connextdds_copy_qos_profile(
    TARGET_PREFIX "${PROJECT_NAME}_ReplayXml_"
    DEPENDANT_TARGET Requester
    FILENAME "replay_remote_admin.xml"
)
