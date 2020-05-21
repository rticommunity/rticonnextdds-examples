# (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no warranty
# of any type, including any warranty for fitness for any purpose. RTI is under
# no obligation to maintain or support the Software.  RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability to
# use the software.

cmake_minimum_required(VERSION 3.7)
project(rtiexamples-dynamic-data-request_reply)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../resources/cmake"
)

# Find the RTI Connext DDS libraries
if(NOT RTIConnextDDS_FOUND)
    find_package(RTIConnextDDS
        "5.3.0"
        REQUIRED
        COMPONENTS
            core
            messaging_api
    )
endif()



add_executable(prime_number_requester_cxx
    "${CMAKE_CURRENT_SOURCE_DIR}/PrimeNumberRequester.cxx"
)

add_executable(prime_number_replier_cxx
    "${CMAKE_CURRENT_SOURCE_DIR}/PrimeNumberReplier.cxx"
)



#We add the library:
add_library(PrimesType "${CMAKE_CURRENT_SOURCE_DIR}/PrimesType.cxx")

target_link_libraries(PrimesType
    PRIVATE
        RTIConnextDDS::cpp_api
        RTIConnextDDS::messaging_cpp_api
)

list(APPEND EXTRA_LIBS PrimesType)

target_link_libraries(prime_number_requester_cxx
    PRIVATE
        RTIConnextDDS::cpp_api
        RTIConnextDDS::messaging_cpp_api
    PUBLIC
        ${EXTRA_LIBS}
)

target_link_libraries(prime_number_replier_cxx
    PRIVATE
        RTIConnextDDS::cpp_api
        RTIConnextDDS::messaging_cpp_api
    PUBLIC
        ${EXTRA_LIBS}
)


target_include_directories(prime_number_requester_cxx
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

target_include_directories(prime_number_replier_cxx
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set_target_properties(prime_number_requester_cxx
    PROPERTIES
        OUTPUT_NAME "PrimeNumberRequester")

set_target_properties(prime_number_replier_cxx
    PROPERTIES
        OUTPUT_NAME "PrimeNumberReplier")

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set_target_properties(prime_number_requester_cxx
        PROPERTIES
            LINK_FLAGS -Wl,--no-as-needed)
endif()