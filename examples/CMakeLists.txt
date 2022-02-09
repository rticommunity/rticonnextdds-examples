# (c) 2022 Copyright, Real-Time Innovations, Inc. All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
cmake_minimum_required(VERSION 3.11)
project(rtiexamples)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/connext_dds")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/persistence_service")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/recording_service")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/routing_service")
