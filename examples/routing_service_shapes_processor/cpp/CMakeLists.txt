cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 11)
project (ShapesProcessor)


# Find RTI Connext dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CONNEXTDDS_DIR}/resource/cmake")
find_package(
    RTIConnextDDS "6.0.0"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

# Define the library that will provide the  Processor plugin
add_library(shapesprocessor SHARED ShapesProcessor.cxx)

target_include_directories(shapesprocessor
    PUBLIC ${CONNEXTDDS_INCLUDE_DIRS})

target_compile_definitions(shapesprocessor
    PUBLIC
    PUBLIC ${CONNEXTDDS_COMPILE_DEFINITIONS})

target_link_libraries(shapesprocessor
    ${rtirsinfrastructure_library}
    RTIConnextDDS::cpp2_api
    RTIConnextDDS::c_api
    ${CONNEXTDDS_EXTERNAL_LIBS})
