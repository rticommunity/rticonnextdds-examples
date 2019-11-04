#.rst:
# (c) 2017 Copyright, Real-Time Innovations, Inc. All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of this file solely for use with RTI Connext DDS.  Licensee may
# redistribute copies of this file provided that all such copies are subject
# to this license.
#
# FindRTIConnextDDS
# -----------------
#
# Find RTI Connext DDS libraries.
#
# Components
# ^^^^^^^^^^
# This module sets variables for the following components that are part of RTI
# Connext DDS:
#
# - core (default, always provided)
# - messaging_api
# - security_plugins
# - routing_service
# - monitoring_libraries
# - distributed_logger
#
# Core is always selected, because the rest of components depend on it.
# However, the rest of components must be explicitly selected in the
# find_package invocation for this module to set variables for the different
# libraries associated with them.
#
# Also, the version consistency across all the requested components will be
# checked. If the version between the components missmatch,
# `RTIConnextDDS_FOUND` will be set to `FALSE`. This feature can be disabled
# setting `CONNEXTDDS_DISABLE_VERSION_CHECK` to `TRUE`.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
# This module defines the following `IMPORTED` targets:
#
# - ``RTIConnextDDS::c_api``
#   The nddsc library if found (nddscore will be linked as part of this target).
# - ``RTIConnextDDS::cpp_api``
#   The nddscpp library if found (nddscore and nddsc will be linked as part of
#   this target).
# - ``RTIConnextDDS::cpp2_api``
#   The nddscpp2 library if found (nddscore and nddsc will be linked as part of
#   this target).
# - ``RTIConnextDDS::distributed_logger_c``
#   The C API library for Distributed Logger if found.
# - ``RTIConnextDDS::distributed_logger_cpp``
#   The CPP API library for Distributed Logger if found.
# - ``RTIConnextDDS::metp``
#   The METP library if found (nddsmetp).
# - ``RTIConnextDDS::routing_service_infrastructure``
#   The infrastructure library for Routing Service if found.
# - ``RTIConnextDDS::routing_service_c``
#   The C API library for Routing Service if found (includes rtiroutingservice,
#   rtirsinfrastructure, rtixml2 and rticonnextmsgc. Also nddsmetp and
#   rticonnextmsgc if found).
# - ``RTIConnextDDS::routing_service_cpp2``
#   The CPP2 API library for Routing Service if found (includes
#   rtiroutingservice, rtirsinfrastructure, rtixml2 and rticonnextmsgc. Also
#   nddsmetp and rticonnextmsgc if found).
# - ``RTIConnextDDS::routing_service``
#   The same as RTIConnextDDS::routing_service_c. Maintained for backward
#   compatibility.
# - ``RTIConnextDDS::assign_transformation``
#   The assign transformation library if found (includes rtirsassigntransf and
#   rtiroutingservice).
# - ``RTIConnextDDS::monitoring``
#   The Monitoring library if found.
# - ``RTIConnextDDS::security_plugins``
#   The security plugins libraries if found (nddssecurity).
# - ``RTIConnextDDS::messaging_c_api``
#   The Request Reply C API library if found (rticonnextmsgc).
# - ``RTIConnextDDS::messaging_cpp_api``
#   The Request Reply CPP API library if found (rticonnextmsgcpp).
# - ``RTIConnextDDS::messaging_cpp2_api``
#   The Request Reply C API library if found (rticonnextmsgcpp2).
# - ``RTIConnextDDS::nddstls``
#   The tls library if found (nddstls).
# - ``RTIConnextDDS::transport_tcp``
#   The Transport TCP library if found (nddstransporttcp).
# - ``RTIConnextDDS::transport_tls``
#   The Transport TLS library if found (nddstransporttls).
# - ``RTIConnextDDS::transport_wan``
#   The Transport WAN library if found (nddstransportwan).
# - ``RTIConnextDDS::recording_service``
#   The Recording Service library if found (includes rtirecordingservice,
#   rtiroutingservice, rtirsinfrastructure, nddscpp2, rtidlc, nddsmetp,
#   rticonnextmsgc and rtixml2).
# - ``RTIConnextDDS::rtixml2``
#   The RTI XML2 library if found (rtixml2).
# - ``RTIConnextDDS::low_bandwidth_discovery_static``
#   The Discovery Static library for Low Bandwidth Plugins if found
#   (nddsdiscoverystatic).
# - ``RTIConnextDDS::low_bandwidth_edisc``
#   The edisc library for Low Bandwidth Plugins if found (rtilbedisc).
# - ``RTIConnextDDS::low_bandwidth_pdisc``
#   The pdisc library for Low Bandwidth Plugins if found (rtilbpdisc).
# - ``RTIConnextDDS::low_bandwidth_rtps``
#   The rtps library for Low Bandwidth Plugins if found (rtilbrtps).
# - ``RTIConnextDDS::low_bandwidth_sm``
#   The sm library for Low Bandwidth Plugins if found (rtilbsm).
# - ``RTIConnextDDS::low_bandwidth_st``
#   The st library for Low Bandwidth Plugins if found (rtilbst).
# - ``RTIConnextDDS::rtizrtps``
#   The rtizrtps library if found (rtizrtps).
#
# Result Variables
# ^^^^^^^^^^^^^^^^
# This module will set the following variables in your project:
#
# - ``CONNEXTDDS_COMPILE_DEFINITIONS``
#   RTI Connext DDS Compiler definitions as a list.
# - ``CONNEXTDDS_DEFINITIONS``
#   RTI Connext DDS Compiler definitions as a string (deprecated).
# - ``CONNEXTDDS_EXTERNAL_LIBS``
#   RTI Connext DDS external dependencies.
# - ``CONNEXTDDS_INCLUDE_DIRS``
#   RTI Connext DDS include directories.
# - ``CONNEXTDDS_DLL_EXPORT_MACRO``
#   Macros to compile against RTI Connext DDS shared libraries on Windows.
# - ``RTICODEGEN_DIR``
#   Path to the directory where RTI Codegen is placed.
# - ``RTICODEGEN``
#   Path to the RTI Codegen executable.
# - ``RTICODEGEN_VERSION``
#   RTI Codegen version.
#
# This module will set the following variables for all the different libraries
# that are part of the components selected at configuration time.
#
# - ``<LIBRARY_NAME>_LIBRARIES_RELEASE_STATIC``
#   Release static libraries for <LIBRARY_NAME>.
#   (e.g., ``CONNEXTDDS_C_API_LIBRARIES_RELEASE_STATIC``).
# - ``<LIBRARY_NAME>_LIBRARIES_RELEASE_SHARED``
#   Release shared libraries for <LIBRARY_NAME>.
#   (e.g., ``CONNEXTDDS_C_API_LIBRARIES_RELEASE_SHARED``).
# - ``<LIBRARY_NAME>_LIBRARIES_DEBUG_STATIC``
#   Debug static libraries for <LIBRARY_NAME>.
#   (e.g., ``CONNEXTDDS_C_API_LIBRARIES_DEBUG_STATIC``).
# - ``<LIBRARY_NAME>_LIBRARIES_DEBUG_SHARED``
#   Debug shared libraries for <LIBRARY_NAME>.
#   (e.g., ``CONNEXTDDS_C_API_LIBRARIES_DEBUG_SHARED``).
# - ``<LIBRARY_NAME>_LIBRARIES``
#   Depending on the value of CMAKE_BUILD_TYPE and BUILD_SHARED_LIBS,
#   this variable will be set with the requested value.
#   (e.g., if ``CMAKE_BUILD_TYPE`` is ``Release`` and ``BUILD_SHARED_LIBS`` is
#   ``OFF``, will have the same value as
#   ``CONNEXTDDS_C_API_LIBRARIES_RELEASE_STATIC``).
#
#   Also, the ``<VAR>_FOUND`` variable is set for each one of the previous
#   variables.
#
# The list of libraries for each component is the following:
#
# - ``core`` component (always set by this module):
#   - ``CONNEXTDDS_C_API``
#     (e.g., ``CONNEXTDDS_C_API_LIBRARIES_RELEASE_STATIC``)
#   - ``CONNEXTDDS_CPP_API``
#     (e.g., ``CONNEXTDDS_CPP_API_LIBRARIES_RELEASE_STATIC``)
#   - ``CONNEXTDDS_CPP2_API``
#     (e.g., ``CONNEXTDDS_CPP2_API_LIBRARIES_RELEASE_STATIC``)
#
# - ``messaging_api`` component:
#   - ``MESSAGING_C``
#     (e.g., ``MESSAGING_C_API_LIBRARIES_RELEASE_STATIC``)
#   - ``MESSAGING_CPP``
#     (e.g., ``MESSAGING_CPP_API_LIBRARIES_RELEASE_STATIC``)
#   - ``MESSAGING_CPP2``
#     (e.g, ``MESSAGING_CPP2_API_LIBRARIES_RELEASE_STATIC``)
#
# - ``distributed_loger`` component:
#   - ``DISTRIBUTED_LOGGER_C``
#     (e.g., ``DISTRIBUTED_LOGGER_C_LIBRARIES_RELEASE_STATIC)
#   - ``DISTRIBUTED_LOGGER_CPP``
#     (e.g., ``DISTRIBUTED_LOGGER_CPP_LIBRARIES_RELEASE_STATIC)
#
# - ``metp`` component:
#   - ``METP``
#     (e.g., ``METP_LIBRARIES_RELEASE_STATIC``)
#
# - ``routing_service`` component:
#   - ``ROUTING_SERVICE_API``
#     (e.g., ``ROUTING_SERVICE_API_LIBRARIES_RELEASE_STATIC``)
#   - ``ROUTING_SERVICE_INFRASTRUCTURE``
#     (e.g., ``ROUTING_SERVICE_INFRASTRUCTURE_LIBRARIES_RELEASE_STATIC``)
#
# - ``assign_transformation`` component:
#   - ``ASSIGN_TRANSFORMATION``
#     (e.g., ``ASSIGN_TRANSFORMATION_LIBRARIES_RELEASE_STATIC``)
#
# - ``security_plugins`` component:
#   - ``SECURITY_PLUGINS``
#     (e.g., ``SECURITY_PLUGINS_LIBRARIES_RELEASE_STATIC``)
#
# - ``monitoring_libraries`` component:
#   - ``MONITORING_LIBRARIES``
#     (e.g., ``MONITORING_LIBRARIES_RELEASE_STATIC``)
#
# - ``nddstls`` component:
#   - ``NDDSTLS``
#     (e.g., ``NDDSTLS_LIBRARIES_RELEASE_STATIC``)
#
# - ``transport_tcp`` component:
#   - ``TRANSPORT_TCP``
#     (e.g., ``TRANSPORT_TCP_LIBRARIES_RELEASE_STATIC``)
#
# - ``transport_tls`` component:
#   - ``TRANSPORT_TLS``
#     (e.g., ``TRANSPORT_TLS_LIBRARIES_RELEASE_STATIC``)
#
# - ``transport_wan`` component:
#   - ``TRANSPORT_WAN``
#     (e.g., ``TRANSPORT_WAN_LIBRARIES_RELEASE_STATIC``)
#
# - ``recording_service`` component:
#   - ``RECORDING_SERVICE_API``
#     (e.g., ``RECORDING_SERVICE_API_LIBRARIES_RELEASE_STATIC``)
#
# - ``rtixml2`` component:
#   - ``RTIXML2``
#     (e.g., ``RTIXML2_LIBRARIES_RELEASE_STATIC``)
#
# - ``low_bandwidth_plugins`` component:
#   - ``LOW_BANDWIDTH_DISCOVERY_STATIC``
#     (e.g., ``LOW_BANDWIDTH_DISCOVERY_STATIC_LIBRARIES_RELEASE_STATIC``)
#   - ``LOW_BANDWIDTH_EDISC``
#     (e.g., ``LOW_BANDWIDTH_EDISC_LIBRARIES_RELEASE_STATIC``)
#   - ``LOW_BANDWIDTH_PDISC``
#     (e.g., ``LOW_BANDWIDTH_PDISC_LIBRARIES_RELEASE_STATIC``)
#   - ``LOW_BANDWIDTH_RTPS``
#     (e.g., ``LOW_BANDWIDTH_RTPS_LIBRARIES_RELEASE_STATIC``)
#   - ``LOW_BANDWIDTH_SM``
#     (e.g., ``LOW_BANDWIDTH_SM_LIBRARIES_RELEASE_STATIC``)
#   - ``LOW_BANDWIDTH_ST``
#     (e.g., ``LOW_BANDWIDTH_ST_LIBRARIES_RELEASE_STATIC``)
#
# - ``rtizrtps`` component:
#   - ``RTIZRTPS``
#     (e.g., ``RTIZRTPS_LIBRARIES_RELEASE_STATIC``)
#
#   Also, the ``<VAR>_FOUND`` variable is set for each one of the previous
#   variables (e.g., ``MONITORING_LIBRARIES_RELEASE_STATIC_FOUND`` and
#   ``MONITORING_LIBRARIES_FOUND`` are set).
#
# If you are building a simple ConnextDDS application (you are only using
# the  core libraries), use the following variables:
#
#  - For a C application: CONNEXTDDS_C_API
#  - For a Traditional C++ application: CONNEXTDDS_CPP_API
#  - For a Modern C++ application: CONNEXTDDS_CPP2_API
#
#
# Lastly, if you want to use the security plugins (or any other component),
# add the appropriate variables to your CMake script.
#
# Hints
# ^^^^^
# If the find_package invocation specifies a version, this module will try
# to find your Connext DDS installation in the default installation
# directories. Likewise, the module will try to guess the name of the
# architecture you are trying to build against, by looking for it under the
# rti_connext_dds-x.y.z/lib.
#
# However, in some cases you must provide the following hints by defining some
# variables in your cmake invocation:
#
# - If you don't specify a version or you have installed Connext DDS in a
#   non-default location, you must set the ``CONNEXTDDS_DIR`` pointing to your
#   RTI Connext DDS installation folder. For example:
#       cmake -DCONNEXTDDS_DIR=/home/rti/rti_connext_dds-x.y.z
#
# - If you have installed more than one architecture on your system (i.e., more
#   than one target rtipkg), you must set the ``CONNEXTDDS_ARCH`` to provide
#   the name of the architecture. For example:
#       cmake -DCONNEXTDDS_ARCH=x64Linux3gcc5.4.0
#
# - If you are building against the security_plugins compoment, this module
#   will try to find OpenSSL in your system using find_package(OpenSSL). If you
#   want to build against a specific installation of OpenSSL, you must set the
#   ``CONNEXTDDS_OPENSSL_DIR`` to provide this module with the path to your
#   installation of OpenSSL.
#
# - Likewise, if you are building against the security_plugins compoment and
#   you want to ensure that you are using a specific OpenSSL version, you must
#   set the ``CONNEXTDDS_OPENSSL_VERSION`` so that it can be added to the
#   find_package(OpenSSL) invocation.
#
# - If you want to build against debug versions of imported targets, you must
#   enable ``CONNEXTDDS_IMPORTED_TARGETS_DEBUG``. Example:
#       cmake -DCONNEXTDDS_IMPORTED_TARGETS_DEBUG=ON
#
# Note
# ^^^^
# Some flags related to the compiler, like (-std=c++11 needed when linking
# against the CPP2 libraries) will not be provided by this script. These flags
# should be provided by the build system.
#
# Examples
# ^^^^^^^^
# Simple Connext DDS application
# ::
#   cmake_minimum_required(VERSION 3.3.2)
#   project (example)
#   set(CMAKE_MODULE_PATH
#       ${CMAKE_MODULE_PATH}
#       "/home/rti/rti_connext_dds-5.3.0/resource/cmake")
#
#   find_package(RTIConnextDDS EXACT "5.3.0" REQUIRED)
#   add_definitions(${CONNEXTDDS_COMPILE_DEFINITIONS})
#   include_directories("src" ${CONNEXTDDS_INCLUDE_DIRS})
#
#   set(SOURCES_PUB
#       src/HelloWorld_publisher.c
#       src/HelloWorld.c
#       src/HelloWorldPlugin.c
#       src/HelloWorldSupport.c)
#
#   add_executable(HelloWorld_c_publisher ${SOURCES_PUB})
#   target_link_libraries(HelloWorld_c_publisher
#       PUBLIC
#           RTIConnextDDS::c_api)
#
#
# Simple Routing Service adapter
# ::
#   cmake_minimum_required(VERSION 3.3.0)
#   project (example)
#   set(CMAKE_MODULE_PATH
#       ${CMAKE_MODULE_PATH}
#       "/home/rti/rti_connext_dds-5.3.0/resource/cmake")
#
#   find_package(RTIConnextDDS
#       EXACT "5.3.0"
#       REQUIRED
#       COMPONENTS
#           routing_service)
#   add_definitions("${CONNEXTDDS_COMPILE_DEFINITIONS}")
#   include_directories("src" ${CONNEXTDDS_INCLUDE_DIRS})
#
#   set(LIBRARIES
#       ${ROUTING_SERVICE_API_LIBRARIES_RELEASE_STATIC}
#       ${CONNEXTDDS_EXTERNAL_LIBS})
#
#   set(SOURCES_LIB src/FileAdapter.c src/LineConversion.c src/osapi.c)
#
#   add_library(shapestransf ${SOURCES_LIB})
#   target_link_libraries(shapestransf
#       PUBLIC
#           RTIConnextDDS::routing_service)
#
# Supported platforms
# ^^^^^^^^^^^^^^^^^^^
# It is compatible with the following platforms listed in the
# RTI Connext DDS Core Libraries Platform Notes:
# - All the Linux i86 and x64 platforms
# - Raspbian Wheezy 7.0 (3.x kernel) on ARMv6 (armv6vfphLinux3.xgcc4.7.2)
# - Android 5.0 and 5.1 (armv7aAndroid5.0gcc4.9ndkr10e)
# - All the Windows i86 and x64 platforms
# - All the Darwin platforms (OS X 10.11-10.13)
#
# Logging in versions lower than CMake 3.15
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# For versions lower than CMake 3.15 ``CMAKE_MINOR_VERSION`` variable should be
# use to define lower logging levels than ``STATUS`` mode. All this modes will
# show messages of current level and higher. The following modes are available:
#
# - ``STATUS`` 
#   Default mode. This will olnly show messages with same or higher logging
#   level than CMake ``STATUS``.
# - ``VERBOSE``
#   This mode will show all messages from ``VERBOSE`` level and higher.
# - ``DEBUG``
#   This last mode will show messages from all previous levels and ``DEBUG``.

#####################################################################
# Logging Macros                                                    #
#####################################################################

# These two macros allow better code tracing with debug and verbose messages
# using new CMake 3.15 message types. If cmake 3.14 or lower is in use, default
# messages will be displayed starting with ``DEBUG`` or ``VERBOSE`` instead.
#
# Arguments:
# - message: provides the text message
if("${CMAKE_MINOR_VERSION}" GREATER_EQUAL "15")
    macro(connextdds_log_verbose message)
        message(VERBOSE "VERBOSE ${message}")
    endmacro()

    macro(connextdds_log_debug message)
        message(DEBUG   "  DEBUG ${message}")
    endmacro()
else()
    set(CONNEXTDDS_LOG_LEVEL_LIST "STATUS" "VERBOSE" "DEBUG")

    if(NOT CONNEXTDDS_LOG_LEVEL)
        set(CONNEXTDDS_LOG_LEVEL "STATUS")
    endif()

    if(NOT CONNEXTDDS_LOG_LEVEL IN_LIST CONNEXTDDS_LOG_LEVEL_LIST)
        string(REPLACE ";"
            " " LOG_LEVELS_STRING
            "${CONNEXTDDS_LOG_LEVEL_LIST}")
        message(FATAL_ERROR "Log level must be one of: ${LOG_LEVELS_STRING}. "
                "It's default value is STATUS.")
    endif()

    macro(connextdds_log_verbose message)
        if("${CONNEXTDDS_LOG_LEVEL}" MATCHES "VERBOSE|DEBUG")
            message(STATUS "VERBOSE ${message}")
        endif()
    endmacro()

    macro(connextdds_log_debug message)
        if("${CONNEXTDDS_LOG_LEVEL}" STREQUAL "DEBUG")
            message(STATUS "  DEBUG ${message}")
        endif()
    endmacro()
endif()

# This macro allow xml login with previous ``VERBOSE`` loger macros.
#
# Arguments:
# - XML_NAME: provides the name of the xml
# - XML: provides the xml contents
macro(connextdds_log_xml XML_NAME XML)
    string(REPLACE "\n"
        ";" lines
        ${XML})

    connextdds_log_verbose("~~~~~~~START ${XML_NAME}~~~~~~~")
    foreach(line ${lines})
        connextdds_log_verbose("\t${line}")
    endforeach()
    connextdds_log_verbose("~~~~~~~FINISH ${XML_NAME}~~~~~~~")
endmacro()

#####################################################################
# Preconditions Check                                               #
#####################################################################
#
# Find RTI Connext DDS installation. We provide some hints that include the
# CONNEXTDDS_DIR variable, the $NDDSHOME environment variable, and the
# default installation directories.

if(NOT CONNEXTDDS_DIR)
    connextdds_log_verbose("CONNEXTDDS_DIR not specified")

    # Is a patch
    if(PACKAGE_FIND_VERSION_COUNT EQUAL 4)
        set(folder_version
            "${PACKAGE_FIND_VERSION_MAJOR}.${PACKAGE_FIND_VERSION_MINOR}.${PACKAGE_FIND_VERSION_PATCH}")
        connextdds_log_debug("The required ConnextDDS version is a patch")
    else()
        set(folder_version ${RTIConnextDDS_FIND_VERSION})
    endif()
    connextdds_log_verbose("ConnextDDS version ${folder_version}")

    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        set(connextdds_root_hints
            "$ENV{HOME}/rti_connext_dds-${folder_version}")

    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(connextdds_root_hints
            "C:/Program Files (x86)/rti_connext_dds-${folder_version}"
            "C:/Program Files/rti_connext_dds-${folder_version}"
            "C:/rti_connext_dds-${folder_version}")

    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
        set(connextdds_root_hints
            "/Applications/rti_connext_dds-${folder_version}")
    endif()

    connextdds_log_debug("Root hints: ${connextdds_root_hints}")
endif()

# We require having an rti_versions file under the installation directory
# as we will use it to verify that the version is appropriate.
find_path(CONNEXTDDS_DIR
    NAMES rti_versions.xml
    HINTS
        ENV NDDSHOME
        ${connextdds_root_hints})

if(NOT CONNEXTDDS_DIR)
    set(error
        "CONNEXTDDS_DIR not specified. Please set -DCONNEXTDDS_DIR= to "
        "your RTI Connext DDS installation directory")
    message(FATAL_ERROR ${error})
endif()

message(STATUS "RTI Connext DDS installation directory: ${CONNEXTDDS_DIR}")

set(codegen_name "rtiddsgen")
if(WIN32)
    set(codegen_name "${codegen_name}.bat")
endif()
connextdds_log_debug("Codegen script ${codegen_name}")

find_path(RTICODEGEN_DIR
    NAME "${codegen_name}"
    HINTS
        "${CONNEXTDDS_DIR}/bin")

if(NOT RTICODEGEN_DIR)
    set(warning
        "Codegen was not found. Please, check if rtiddsgen is under your "
        "NDDSHOME/bin directory or provide it to CMake using -DRTICODEGEN_DIR")
        message(WARNING ${warning})
else()
    set(RTICODEGEN
        "${RTICODEGEN_DIR}/${codegen_name}"
        CACHE PATH
        "Path to RTI Codegen")

    # Execute RTI Code Generator to get the version
    connextdds_log_debug("Get the Codegen version: '${RTICODEGEN} -version'")
    execute_process(COMMAND ${RTICODEGEN} -version
        OUTPUT_VARIABLE codegen_version_string)
    connextdds_log_debug("Command output:")
    connextdds_log_debug("${codegen_version_string}")

    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
        RTICODEGEN_VERSION "${codegen_version_string}")
    connextdds_log_verbose("Codegen version: ${RTICODEGEN_VERSION}")
endif()

# Find RTI Connext DDS architecture if CONNEXTDDS_ARCH is unset
if(NOT DEFINED CONNEXTDDS_ARCH)
    connextdds_log_verbose("CONNEXTDDS_ARCH was not provided")
    connextdds_log_verbose("CONNEXTDDS_ARCH trying to guess the RTI Architecture")

    # Guess the RTI Connext DDS architecture
    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
        string(REGEX REPLACE "^([0-9]+).*$" "\\1"
            major_version
            ${CMAKE_CXX_COMPILER_VERSION})
        set(version_compiler "${major_version}.0")
        connextdds_log_debug("Compiler version: ${version_compiler}")

        string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1"
            kernel_version "${CMAKE_SYSTEM_VERSION}")
        connextdds_log_debug("Kernel version: ${kernel_version}")

        set(guessed_architecture
             "x64Darwin${kernel_version}${version_compiler}")

    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86")
            set(connextdds_host_arch "i86Win32")
        elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
            set(connextdds_host_arch "x64Win64")

        else()
            message(FATAL_ERROR
                "${CMAKE_HOST_SYSTEM} is not supported as host architecture")
        endif()
        connextdds_log_debug("Host architecture: ${connextdds_host_arch}")

        string(REGEX MATCH "[0-9][0-9][0-9][0-9]"
             vs_year
             "${CMAKE_GENERATOR}")
        connextdds_log_debug("Visual Studio year: ${vs_year}")
        set(guessed_architecture "${connextdds_host_arch}VS${vs_year}")
    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        if(CMAKE_COMPILER_VERSION VERSION_EQUAL "4.6.3")
            set(kernel_version "3.x")
        else()
            string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1"
                kernel_version
                "${CMAKE_SYSTEM_VERSION}")
        endif()

        connextdds_log_debug("Kernel version: ${kernel_version}")

        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
            set(connextdds_host_arch "x64Linux")

        elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686")
            set(connextdds_host_arch "i86Linux")
        endif()

        connextdds_log_debug("Host architecture: ${connextdds_host_arch}")
        set(guessed_architecture
            "${connextdds_host_arch}${kernel_version}gcc${CMAKE_C_COMPILER_VERSION}")
    endif()

    connextdds_log_verbose("Guessed RTI architecture: ${guessed_architecture}")

    if(DEFINED ENV{CONNEXTDDS_ARCH})
        set(CONNEXTDDS_ARCH $ENV{CONNEXTDDS_ARCH})
    elseif(EXISTS "${CONNEXTDDS_DIR}/lib/${guessed_architecture}")
        set(CONNEXTDDS_ARCH "${guessed_architecture}")
        connextdds_log_debug("${CONNEXTDDS_DIR}/lib/${guessed_architecture} exists")
    else()
        # If CONNEXTDDS_ARCH is unspecified, the module tries uses the first
        # architecture installed by looking under $CONNEXTDDS_DIR/lib.
        file(GLOB architectures_installed
            RELATIVE "${CONNEXTDDS_DIR}/lib"
            "${CONNEXTDDS_DIR}/lib/*")

        foreach(architecture_name ${architectures_installed})
            # Because the lib folder contains both target libraries and
            # java jar files, here we exclude the "java" in our algorithm
            # to guess the appropriate CONNEXTDDS_ARCH variable.
            if(architecture_name STREQUAL "java")
                continue()
            elseif("${architecture_name}" MATCHES ${CMAKE_HOST_SYSTEM_NAME})
                    if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
                        # Get the installed Darwin
                        set(CONNEXTDDS_ARCH "${architecture_name}")
                    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
                        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86" AND
                                "${architecture_name}" MATCHES "Win32")
                            # Get the x86Win32 architecture
                            set(CONNEXTDDS_ARCH "${architecture_name}")
                        elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64" AND
                                "${architecture_name}" MATCHES "Win64")
                            # Get the x64Win64 architecture
                            set(CONNEXTDDS_ARCH "${architecture_name}")
                        endif()
                    elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
                        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686" AND
                            "${architecture_name}" MATCHES "x86Linux")
                            # Get the x86Linux architecture
                            set(CONNEXTDDS_ARCH "${architecture_name}")

                        elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64" AND
                            "${architecture_name}" MATCHES "x64Linux")
                            # Get the x64Linux architecture
                            set(CONNEXTDDS_ARCH "${architecture_name}")
                        endif()
                    endif()
                endif()

            if(CONNEXTDDS_ARCH)
                break()
            endif()
        endforeach()

        if(NOT CONNEXTDDS_ARCH)
            message(WARNING
                "CONNEXTDDS_ARCH not specified. Please set "
                "-DCONNEXTDDS_ARCH= to specify your RTI Connext DDS "
                " architecture")
        endif()
    endif()
endif()

message(STATUS "RTI Connext DDS architecture: ${CONNEXTDDS_ARCH}")

#####################################################################
# Helper Functions                                                  #
#####################################################################

# Checks that a certain field associated with a component is installed under
# CONNEXTDDS_DIR and that its version is consistent with the version required
# for the rest of modules. If RTICONNEXTDDS_VERSION, the version variable, is
# specified, then it checks that the value is consistent. Otherwise,
# it sets the value of RTICONNEXTDDS_VERSION so that it can be checked in
# future calls of this function to validate that its value is consistent
# accross fields and components.
#
# At the end of the script CMake will check that RTICONNEXTDDS_VERSION is
# consistent with the version specified by the user in the find_package
# invocation, acording to the matching rules defined by CMake.
#
# Arguments:
# - field_name: provides the name of the
# Returns:
# - A warning is shown and the `rtiversion_error` variable will be set if the
#   component field is not present in rti_versions.xml or the version does not
#   match RTICONNEXTDDS_VERSION.
function(connextdds_check_component_field_version
    field_name
    rti_versions_file
    rti_architectures)
    connextdds_log_debug("connextdds_check_component_field_version called")
    connextdds_log_debug(
        "====================================================================")
    connextdds_log_debug("\tfield_name: ${field_name}")
    connextdds_log_debug("\trti_architectures: [${${rti_architectures}}]")
    connextdds_log_verbose("Checking version for ${field_name}")

    # Get the component info
    set(field_section_regex "<${field_name}>.*</${field_name}>")
    string(REGEX MATCH ${field_section_regex} field_xml "${rti_versions_file}")

    connextdds_log_xml("Component info:" "${field_xml}")

    # Check all the architectures defined in the component
    foreach(architecture IN LISTS ${rti_architectures})
        # Ignore java fro routing_service_host
        if(architecture STREQUAL "java" AND
            field_name STREQUAL "routing_service_host")
            continue()
        endif()

        string(CONCAT field_arch_regex
            "<installation>[\n\r\t ]*"
            "<architecture>${architecture}.*</architecture>"
            "[\n\r\t ]*<version>[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?"
            "</version>")
        string(REGEX MATCH ${field_arch_regex} field_arch "${field_xml}")
        if(field_arch)
            connextdds_log_xml("Field arch:" "${field_arch}")
            break()
        endif()
    endforeach()

    if(NOT field_arch)
        message(WARNING
            "${field_name} is not installed for ${CONNEXTDDS_ARCH} "
            "under ${CONNEXTDDS_DIR}")
        set(rtiversion_error TRUE PARENT_SCOPE)
    endif()

    # Get the <version> tag from the component
    set(field_version_regex
        "<version>[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+(\\.[0-9]+)?)?</version>")
    string(REGEX MATCH ${field_version_regex} field_version "${field_arch}")
    connextdds_log_xml("Component version tag:" "${field_version}")

    if(NOT field_version)
        set(rtiversion_error TRUE PARENT_SCOPE)
        message(WARNING
            "The version tag was not extracted from ${field_name} and "
            "${CONNEXTDDS_ARCH} under ${CONNEXTDDS_DIR}")
        return()
    endif()

    # Extract the version string from the tag
    set(field_version_number_regex
        "[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+(\\.[0-9]+)?)?")
    string(REGEX MATCH
        ${field_version_number_regex}
        field_version_number
        ${field_version})

    connextdds_log_xml("Component version:" "${field_version_number}")

    if(NOT field_version_number)
        set(rtiversion_error TRUE PARENT_SCOPE)
        message(WARNING
            "The version number was not extracted from ${field_name} and "
            "${CONNEXTDDS_ARCH} under ${CONNEXTDDS_DIR}")
        return()
    endif()

    if(NOT DEFINED RTICONNEXTDDS_VERSION)
        # If the variable version RTICONNEXTDDS_VERSION has not been set, we
        # set it here in the scope of the caller to store the value that has
        # been detected.
        set(RTICONNEXTDDS_VERSION ${field_version_number} PARENT_SCOPE)
        connextdds_log_verbose("Found ConnextDDS version: ${field_version_number}")
    elseif(NOT RTICONNEXTDDS_VERSION STREQUAL field_version_number)
        # Otherwise, if the detected version is inconsistent with the previous
        # value of RTICONNEXTDDS_VERSION, we throw an error.
        set(warning
            "The version of ${field} is ${field_version_number}, which "
            "is incosistent with the expected version... "
            "${RTICONNEXTDDS_VERSION}")
        message(WARNING ${warning})
        set(rtiversion_error TRUE PARENT_SCOPE)
    else()
        connextdds_log_verbose("${field_name} version.........OK")
    endif()
    connextdds_log_debug(
        "====================================================================")
endfunction()

# This method searches the libraries indicated in `library_names` under
# `<RTI Connext DDS directory>/lib/<architecture>. If one of the libraries in
# one of the modes is not found, the variable <result_var_name>_FOUND is set
# to `FALSE`.
#
# After finding the libraries, the libraries are stored in different variables
# following the structure <result_var_name>_<build_type>_<link_type>. Each
# variable contain the libraries for a build and link mode. If one of the
# libraries is not found, the <result_var_name>_<build_type>_<link_type>_FOUND
# is set to `FALSE`.
#
# Arguments:
# - library_names: names of the libraries to find (sorted)
# - result_var_name: name of the variable to set with the found libraries
# Returns:
# - <result_var_name>_RELEASE_STATIC: containing the release/static libraries
# - <result_var_name>_RELEASE_SHARED:  containing the release/shared libraries
# - <result_var_name>_DEBUG_STATIC:  containing the debug/static libraries
# - <result_var_name>_DEBUG_SHARED:  containing the debug/shared libraries
# - <result_var_name>_FOUND: `True` if all the libraries were found
function(get_all_library_variables
    library_names
    result_var_name)
    connextdds_log_debug("get_all_library_variables called")
    connextdds_log_debug(
        "====================================================================")
    connextdds_log_debug("\tlibrary_names: ${library_names}")
    connextdds_log_debug("\tresult_var_name: ${result_var_name}")

    set(mode_library_found TRUE)
    set(${result_var_name}_FOUND TRUE PARENT_SCOPE)

    foreach(build_mode "release" "debug")
        foreach(link_mode "static" "shared")
            set(libraries)
            foreach(library_name ${library_names})
                if(${link_mode} STREQUAL "static")
                    set(name "${library_name}z")
                else()
                    set(name "${library_name}")
                endif()

                if(${build_mode} STREQUAL "debug")
                    set(name "${name}d")
                else()
                    set(name "${name}")
                endif()
                connextdds_log_debug(
                    "\t\t[${build_mode}/${link_mode}]Library name: ${name}")

                find_library(lib${library_name}_${build_mode}_${link_mode}
                    NAMES
                        ${name}
                    PATHS
                        "${CONNEXTDDS_DIR}/lib/${CONNEXTDDS_ARCH}"
                    NO_DEFAULT_PATH
                    NO_CMAKE_PATH
                    NO_CMAKE_ENVIRONMENT_PATH
                    NO_SYSTEM_ENVIRONMENT_PATH
                    NO_CMAKE_SYSTEM_PATH
                )

                if(lib${library_name}_${build_mode}_${link_mode}-NOTFOUND)
                    set(mode_library_found FALSE)
                    break()
                else()
                    list(APPEND libraries
                        ${lib${library_name}_${build_mode}_${link_mode}})
                endif()
            endforeach()

            string(TOUPPER ${link_mode} upper_link_mode)
            string(TOUPPER ${build_mode} upper_build_mode)

            if(${mode_library_found})
                set(lib_var "${result_var_name}_LIBRARIES_${upper_build_mode}_${upper_link_mode}")
                set(${lib_var} ${libraries} PARENT_SCOPE)
                connextdds_log_debug("\t${lib_var} = ${libraries}")
                set(${lib_var}_FOUND TRUE PARENT_SCOPE)
            else()
                set(${lib_var}_FOUND FALSE PARENT_SCOPE)
                set(${result_var_name}_FOUND FALSE PARENT_SCOPE)
            endif()
            unset(lib${library_name}_${build_mode}_${link_mode} CACHE)
        endforeach()
    endforeach()

    if(CMAKE_BUILD_TYPE)
        string(TOUPPER "${CMAKE_BUILD_TYPE}" build_mode)
    else()
        set(build_mode "DEBUG")
    endif()

    if(${BUILD_SHARED_LIBS})
        set(link_mode "SHARED")
    else()
        set(link_mode "STATIC")
    endif()

    set(${result_var_name}_LIBRARIES
        ${result_var_name}_LIBRARIES_${build_mode}_${link_mode})
    connextdds_log_debug(
        "====================================================================")
endfunction()

#####################################################################
# Platform-specific Definitions                                     #
#####################################################################

set(connextdds_host_arch "java")

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    # connextdds_host_arch is usually a prefix of the CONNEXTDDS_ARCH (e.g.,
    # "x64Linux" is a prefix  of "x64Linux3gcc5.4.0"). However, in the case
    # of Darwin, connextdds_host_arch is simply "darwin".
    # To be able to match the CONNEXTDDS_ARCH in some parts of the script,
    # we need to be able to consider connextdds_host_arch with both names
    # "darwin" and the more natural "x64Darwin". Consequently, we assign a
    # list of names to CONNEXT_HOST_ARCH in the case of darwin.
    set(connextdds_host_arch ${connextdds_host_arch} "darwin" "x64Darwin")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86")
        set(connextdds_host_arch ${connextdds_host_arch} "i86Win32")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
        set(connextdds_host_arch ${connextdds_host_arch} "x64Win64")
    else()
        message(FATAL_ERROR
            "${CMAKE_HOST_SYSTEM} is not supported as host architecture")
    endif()
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
        set(connextdds_host_arch ${connextdds_host_arch} "x64Linux")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686")
        set(connextdds_host_arch ${connextdds_host_arch} "i86Linux")
    else()
        message(FATAL_ERROR
            "${CMAKE_HOST_SYSTEM} is not supported as host architecture")
    endif()
else()
    message(FATAL_ERROR
        "${CMAKE_HOST_SYSTEM} is not supported as host architecture")
endif()

# Platform-Specific Definitions
if(CONNEXTDDS_ARCH MATCHES "Linux")
    # Linux Platforms
    set(CONNEXTDDS_EXTERNAL_LIBS -ldl -lm -lpthread -lrt)
    set(CONNEXTDDS_COMPILE_DEFINITIONS RTI_UNIX RTI_LINUX)

    if(CONNEXTDDS_ARCH MATCHES "x64Linux")
        set(CONNEXTDDS_COMPILE_DEFINITIONS
            ${CONNEXTDDS_COMPILE_DEFINITIONS}
            RTI_64BIT)
    endif()
elseif(CONNEXTDDS_ARCH MATCHES "Win")
    # Windows Platforms
    set(CONNEXTDDS_EXTERNAL_LIBS ws2_32 netapi32 version)

    set(CONNEXTDDS_COMPILE_DEFINITIONS
        WIN32_LEAN_AND_MEAN
        WIN32
        _WINDOWS
        RTI_WIN32
        _BIND_TO_CURRENT_MFC_VERSION=1
        _BIND_TO_CURRENT_CRT_VERSION=1
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_WARNING)

    # When building against ConnextDDS's shared libraries, users need to also
    # add the CONNEXTDDS_DLL_EXPORT_MACRO to their definitions.
    set(CONNEXTDDS_DLL_EXPORT_MACRO NDDS_DLL_VARIABLE)

elseif(CONNEXTDDS_ARCH MATCHES "Darwin")
    # Darwin Platforms
    set(CONNEXTDDS_EXTERNAL_LIBS "")

    set(CONNEXTDDS_COMPILE_DEFINITIONS
        RTI_UNIX
        RTI_DARWIN
        RTI_DARWIN10
        RTI_64BIT)
elseif(CONNEXTDDS_ARCH MATCHES "Android")
    set(CONNEXTDDS_EXTERNAL_LIBS -llog -lc -lm)
    set(CONNEXTDDS_COMPILE_DEFINITIONS RTI_UNIX LINUX RTI_ANDROID)
else()
    message(FATAL_ERROR
        "${CONNEXTDDS_ARCH} architecture is unsupported by this module")
endif()


#####################################################################
# Core Component Variables                                          #
#####################################################################

# This script verifies the version of each of the following fields in
# ${CONNEXTDDS_DIR}/rti_versions.xml, as they are the basic components of the
# RTI Connext DDS core libraries, which are always part of an installation.
list(APPEND rti_versions_field_names_host
    "header_files"
    "host_files"
    "core_release_docs"
    "core_api_docs"
    "core_jars")

list(APPEND rti_versions_field_names_target
    "target_libraries")

# Define CONNEXTDDS_INCLUDE_DIRS
find_path(CONNEXTDDS_INCLUDE_DIRS
    NAMES
        ndds_c.h
    PATHS
        "${CONNEXTDDS_DIR}/include/ndds")

set(CONNEXTDDS_INCLUDE_DIRS
    "${CONNEXTDDS_DIR}/include"
    ${CONNEXTDDS_INCLUDE_DIRS}
    "${CONNEXTDDS_DIR}/include/ndds/hpp")

# Find all flavors of nddsc and nddscore
set(c_api_libs "nddsc" "nddscore")
get_all_library_variables("${c_api_libs}" "CONNEXTDDS_C_API")

# Find all flavors of nddscpp and nddscore
set(cpp_api_libs "nddscpp" "nddsc" "nddscore")
get_all_library_variables("${cpp_api_libs}" "CONNEXTDDS_CPP_API")

# Find all flavors of nddscpp2 and nddscore
set(cpp2_api_libs "nddscpp2" "nddsc" "nddscore")
get_all_library_variables("${cpp2_api_libs}" "CONNEXTDDS_CPP2_API")

if(CONNEXTDDS_C_API_FOUND AND CONNEXTDDS_CPP_API_FOUND AND
    CONNEXTDDS_CPP2_API_FOUND)
    set(RTIConnextDDS_core_FOUND TRUE)
else()
    set(RTIConnextDDS_core_FOUND FALSE)
endif()

#####################################################################
# Distributed Logger Component Variables                            #
#####################################################################
if(distributed_logger IN_LIST RTIConnextDDS_FIND_COMPONENTS)

    # Find all flavors of rtidlc
    set(distributed_logger_c_libs "rtidlc" "nddsc" "nddscore")
    get_all_library_variables("${distributed_logger_c_libs}"
        "DISTRIBUTED_LOGGER_C")

    # Find all flavors of rtidlcpp
    set(distributed_logger_cpp_libs
        "rtidlcpp"
        "librtidlc"
        "nddscpp"
        "nddsc"
        "nddscore")
    get_all_library_variables("${distributed_logger_cpp_libs}"
        "DISTRIBUTED_LOGGER_CPP")


    if(DISTRIBUTED_LOGGER_C_FOUND AND DISTRIBUTED_LOGGER_CPP_FOUND)
        set(RTIConnextDDS_distributed_logger_FOUND TRUE)

        if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND
            CMAKE_COMPILER_VERSION VERSION_GREATER "4.6.0")
           set(CONNEXTDDS_EXTERNAL_LIBS
               -Wl,--no-as-needed  ${CONNEXTDDS_EXTERNAL_LIBS})
        endif()
    else()
        set(RTIConnextDDS_distributed_logger_FOUND FALSE)
    endif()
endif()

#####################################################################
# METP Library Component Variables                                  #
#####################################################################
if(metp IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddsmetp
    set(metp_libs
        "nddsmetp"
        "nddsc"
        "nddscore")
    get_all_library_variables("${metp_libs}" "METP")

    if(METP_FOUND)
        set(RTIConnextDDS_metp_FOUND TRUE)
    else()
        set(RTIConnextDDS_metp_FOUND FALSE)
    endif()
endif()


#####################################################################
# Routing Service Component Variables                               #
#####################################################################
if(routing_service IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Add fields associated with the routing_service component
    list(APPEND rti_versions_field_names_host
            "routing_service_host"
            "routing_service")
    if(RTIConnextDDS_FIND_VERSION VERSION_GREATER 5.3.0.8)
        list(APPEND rti_versions_field_names_host
                "routing_service_sdk_jars")
    endif()

    list(APPEND rti_versions_field_names_target
            "routing_service_sdk")

    # Find all flavors of librtirsinfrastructure
    set(rtirsinfrastructure_libs
        "rtirsinfrastructure"
        "nddsc"
        "nddscore")
    get_all_library_variables(
        "${rtirsinfrastructure_libs}"
        "ROUTING_SERVICE_INFRASTRUCTURE")

    set(addon_dependencies)
    if(RTIConnextDDS_metp_FOUND)
        set(addon_dependencies "nddsmetp")
    endif()

    if(RTIConnextDDS_distributed_logger_FOUND)
        set(addon_dependencies "${addon_dependencies}" "rtidlc")
    endif()

    # Find all flavors of librtiroutingservice
    set(routing_service_libs
        "rtiroutingservice"
        "rtirsinfrastructure"
        ${addon_dependencies}
        "rtixml2"
        "rticonnextmsgc"
        "nddsc"
        "nddscore")
    get_all_library_variables("${routing_service_libs}" "ROUTING_SERVICE_API")

    if(WIN32 AND ROUTING_SERVICE_API_RELEASE_STATIC AND
            ROUTING_SERVICE_API_DEBUG_STATIC)
        # ROUTING-276: Routing Service is not available as a static library
        # for Windows
        set(ROUTING_SERVICE_API_FOUND TRUE)
    endif()

    if(ROUTING_SERVICE_API_FOUND)
        set(RTIConnextDDS_routing_service_FOUND TRUE)
    else()
        set(RTIConnextDDS_routing_service_FOUND FALSE)
    endif()

endif()

#####################################################################
# Assign Transformation Component Variables                         #
#####################################################################
if(assign_transformation IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of librtirsassigntransf
    set(assign_transformation_libs
        "rtirsassigntransf"
        "rtiroutingservice"
        "nddsc"
        "nddscore")
    get_all_library_variables(
        "${assign_transformation_libs}"
        "ASSIGN_TRANSFORMATION")

    if(ASSIGN_TRANSFORMATION_FOUND)
        set(RTIConnextDDS_assign_transformation_FOUND TRUE)
    else()
        set(RTIConnextDDS_assign_transformation_FOUND FALSE)
    endif()
endif()

#####################################################################
# Messaging Component Variables                                     #
#####################################################################
if(messaging_api IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    list(APPEND rti_versions_field_names_host
            "request_reply_host")
    list(APPEND rti_versions_field_names_target
            "request_reply")

    # Find all flavors of librticonnextmsgc
    set(messaging_c_api_libs "rticonnextmsgc" "nddsc" "nddscore")
    get_all_library_variables("${messaging_c_api_libs}" "MESSAGING_C_API")

    # Find all flavors of librticonnextmsgcpp
    set(messaging_cpp_api_libs "rticonnextmsgcpp" "nddscpp" "nddsc" "nddscore")
    get_all_library_variables("${messaging_cpp_api_libs}" "MESSAGING_CPP_API")

    # Find all flavors of librticonnextmsgcpp2
    set(messaging_cpp2_api_libs
        "rticonnextmsgcpp2"
        "nddscpp2"
        "nddsc"
        "nddscore")
    get_all_library_variables("${messaging_cpp2_api_libs}"
        "MESSAGING_CPP2_API")

    if(MESSAGING_C_API_FOUND AND MESSAGING_CPP_API_FOUND AND
        MESSAGING_CPP2_API_FOUND)
        set(RTIConnextDDS_messaging_api_FOUND TRUE)
    else()
        set(RTIConnextDDS_messaging_api_FOUND FALSE)
    endif()
endif()

#####################################################################
# Security Plugins Component Variables                              #
#####################################################################
if(security_plugins IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    list(APPEND rti_versions_field_names_host
        "secure_base"
        "secure_host")

    list(APPEND rti_versions_field_names_target
        "secure_target_libraries")

    # Find all flavors of libnddssecurity
    get_all_library_variables("nddssecurity" "SECURITY_PLUGINS")

    if(SECURITY_PLUGINS_FOUND)
        if(CONNEXTDDS_OPENSSL_DIR)
            find_package(OpenSSL
                REQUIRED ${CONNEXTDDS_OPENSSL_VERSION}
                PATHS
                    "${CONNEXTDDS_OPENSSL_DIR}")
        elseif(DEFINED CONNEXTDDS_OPENSSL_VERSION)
            find_package(OpenSSL REQUIRED ${CONNEXTDDS_OPENSSL_VERSION})
        else()
            find_package(OpenSSL REQUIRED)
        endif()

        # Add OpenSSL include directories to the list of
        # CONNEXTDDS_INCLUDE_DIRS
        set(CONNEXTDDS_INCLUDE_DIRS
            "${OPENSSL_INCLUDE_DIR}"
            ${CONNEXTDDS_INCLUDE_DIRS})

        # Add OpenSSL libraries to the list of CONNEXTDDS_EXTERNAL_LIBS
        set(CONNEXTDDS_EXTERNAL_LIBS
            ${OPENSSL_SSL_LIBRARY}
            ${OPENSSL_CRYPTO_LIBRARY}
            ${CONNEXTDDS_EXTERNAL_LIBS})

        set(RTIConnextDDS_security_plugins_FOUND TRUE)
    else()
        set(RTIConnextDDS_security_plugins_FOUND FALSE)
    endif()

endif()

#####################################################################
# Monitoring Liraries Component Variables                           #
#####################################################################
if(monitoring_libraries IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of librtimonitoring
    get_all_library_variables("rtimonitoring" "MONITORING")

    if(MONITORING_FOUND)
        if(CONNEXTDDS_ARCH MATCHES "Win")
            set(CONNEXTDDS_EXTERNAL_LIBS psapi ${CONNEXTDDS_EXTERNAL_LIBS})
        endif()
        set(RTIConnextDDS_monitoring_libraries_FOUND TRUE)
    else()
        set(RTIConnextDDS_monitoring_libraries_FOUND FALSE)
    endif()
endif()

#####################################################################
# TLS Static Component Variables                                    #
#####################################################################
if(nddstls IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddstls
    set(nddstls_libs
        "nddstls"
        "nddsc"
        "nddscore")
    get_all_library_variables("${nddstls_libs}" "NDDSTLS")

    if(NDDSTLS_FOUND)
        set(RTIConnextDDS_nddstls_FOUND TRUE)
    else()
        set(RTIConnextDDS_nddstls_FOUND FALSE)
    endif()
endif()

#####################################################################
# Transport TCP Library Component Variables                         #
#####################################################################
if(transport_tcp IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddstransporttcp
    set(transport_tcp_libs
        "nddstransporttcp"
        "nddsc"
        "nddscore")
    get_all_library_variables("${transport_tcp_libs}" "TRANSPORT_TCP")

    if(TRANSPORT_TCP_FOUND)
        set(RTIConnextDDS_transport_tcp_FOUND TRUE)
    else()
        set(RTIConnextDDS_transport_tcp_FOUND FALSE)
    endif()
endif()

#####################################################################
# Transport TLS Library Component Variables                         #
#####################################################################
if(transport_tls IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddstransporttls
    set(transport_tls_libs
        "nddstransporttls"
        "nddsc"
        "nddscore")
    get_all_library_variables("${transport_tls_libs}" "TRANSPORT_TLS")

    if(TRANSPORT_TLS_FOUND)
        set(RTIConnextDDS_transport_tls_FOUND TRUE)
    else()
        set(RTIConnextDDS_transport_tls_FOUND FALSE)
    endif()
endif()

#####################################################################
# Transport WAN Library Component Variables                         #
#####################################################################
if(transport_wan IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddstransportwan
    set(transport_wan_libs
        "nddstransportwan"
        "nddstransporttls"
        "nddsc"
        "nddscore")
    get_all_library_variables("${transport_wan_libs}" "TRANSPORT_WAN")

    if(TRANSPORT_WAN_FOUND)
        set(RTIConnextDDS_transport_wan_FOUND TRUE)
    else()
        set(RTIConnextDDS_transport_wan_FOUND FALSE)
    endif()
endif()

#####################################################################
# Recording Service API Component Variables                         #
#####################################################################
if(recording_service IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of librtirecordingservice
    set(recording_service_libs
        "rtirecordingservice"
        "rtiroutingservice"
        "rtirsinfrastructure"
        "nddscpp2"
        "rtidlc"
        "nddsmetp"
        "rticonnextmsgc"
        "rtixml2"
        "nddsc"
        "nddscore")
    get_all_library_variables(
        "${recording_service_libs}"
        "RECORDING_SERVICE_API")

    if(RECORDING_SERVICE_API_FOUND)
        set(RTIConnextDDS_recording_service_FOUND TRUE)
    else()
        set(RTIConnextDDS_recording_service_FOUND FALSE)
    endif()
endif()

#####################################################################
# rtixml2 Library Component Variables                               #
#####################################################################
if(rtixml2 IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    if(WIN32)
        message(FATAL_ERROR "rtixml2 component is not supported for Windows")
    endif()

    # Find all flavors of librtixml2
    set(rtixml2_libs
        "rtixml2"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtixml2_libs}" "RTIXML2")

    if(RTIXML2_FOUND)
        set(RTIConnextDDS_rtixml2_FOUND TRUE)
    else()
        set(RTIConnextDDS_rtixml2_FOUND FALSE)
    endif()
endif()

#####################################################################
# Low Bandwidth Pluggins Component Variables                        #
#####################################################################
if(low_bandwidth_plugins IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of libnddsdiscoverystatic
    set(discovery_static_libs
        "nddsdiscoverystatic"
        "nddsc"
        "nddscore")
    get_all_library_variables(
        "${discovery_static_libs}"
        "LOW_BANDWIDTH_DISCOVERY_STATIC")

        # Find all flavors of librtilbedisc
    set(rtilbedisc_libs
        "rtilbedisc"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtilbedisc_libs}" "LOW_BANDWIDTH_EDISC")

    # Find all flavors of librtilbpdisc
    set(rtilbpdisc_libs
        "rtilbpdisc"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtilbpdisc_libs}" "LOW_BANDWIDTH_PDISC")

    # Find all flavors of librtilbrtps
    set(rtilbrtps_libs
        "rtilbrtps"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtilbrtps_libs}" "LOW_BANDWIDTH_RTPS")

    # Find all flavors of librtilbsm
    set(rtilbsm_libs
        "rtilbsm"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtilbsm_libs}" "LOW_BANDWIDTH_SM")

    # Find all flavors of librtilbst
    set(rtilbst_libs
        "rtilbst"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtilbst_libs}" "LOW_BANDWIDTH_ST")

    if(LOW_BANDWIDTH_DISCOVERY_STATIC_FOUND AND LOW_BANDWIDTH_EDISC_FOUND AND
            LOW_BANDWIDTH_PDISC_FOUND AND LOW_BANDWIDTH_RTPS_FOUND AND
            LOW_BANDWIDTH_SM_FOUND AND LOW_BANDWIDTH_ST_FOUND)
        set(RTIConnextDDS_low_bandwidth_plugins_FOUND TRUE)
    else()
        set(RTIConnextDDS_low_bandwidth_plugins_FOUND FALSE)
    endif()
endif()

#####################################################################
# rtizrtps Component Variables                                      #
#####################################################################
if(rtizrtps IN_LIST RTIConnextDDS_FIND_COMPONENTS)
    # Find all flavors of librtizrtps
    set(rtizrtps_libs
        "rtizrtps"
        "nddsc"
        "nddscore")
    get_all_library_variables("${rtizrtps_libs}" "RTIZRTPS")

    if(RTIZRTPS_FOUND)
        set(RTIConnextDDS_rtizrtps_FOUND TRUE)
    else()
        set(RTIConnextDDS_rtizrtps_FOUND FALSE)
    endif()
endif()

#####################################################################
# Version Variables                                                 #
#####################################################################
# Verify that all the components specified have the same version
file(READ "${CONNEXTDDS_DIR}/rti_versions.xml" xml_file)

if(CONNEXTDDS_DISABLE_VERSION_CHECK)
    # If the version check is disabled, the method to check the component field
    # version is called one time. That method will return the version described
    # in the rti_versions.xml for a given component. When the check is
    # disabled, the returned version is the core libraries version.
    connextdds_check_component_field_version(
        "header_files"
        ${xml_file}
        connextdds_host_arch)
else()
    # Verify host components
    foreach(field IN LISTS rti_versions_field_names_host)
        connextdds_check_component_field_version(
            ${field}
            ${xml_file}
            connextdds_host_arch)
    endforeach()

    # Verify target components
    foreach(field IN LISTS rti_versions_field_names_target)
            connextdds_check_component_field_version(
                ${field}
                ${xml_file}
                CONNEXTDDS_ARCH)
    endforeach()
endif()

# Set CONNEXTDDS_DEFINITIONS for backward compatibility
foreach(definition ${CONNEXTDDS_COMPILE_DEFINITIONS})
    set(CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS} -D${definition}")
endforeach()

# If there were not errors checking the version, everything was fine
if(NOT rtiversion_error)
    set(RTICONNEXTDDS_VERSION_OK TRUE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RTIConnextDDS
    REQUIRED_VARS
        CONNEXTDDS_DIR
        RTICONNEXTDDS_VERSION
        RTICONNEXTDDS_VERSION_OK
    VERSION_VAR
        RTICONNEXTDDS_VERSION
    HANDLE_COMPONENTS)

#####################################################################
# Create imported targets                                           #
#####################################################################
set(location_property IMPORTED_LOCATION)
if(RTIConnextDDS_FOUND AND RTIConnextDDS_core_FOUND)
    if(${BUILD_SHARED_LIBS})
        set(link_type SHARED)
        set(target_definitions
            ${CONNEXTDDS_COMPILE_DEFINITIONS}
            ${CONNEXTDDS_DLL_EXPORT_MACRO})
        if(WIN32)
            set(location_property IMPORTED_IMPLIB)
        endif()
    else()
        set(link_type STATIC)
        set(target_definitions ${CONNEXTDDS_COMPILE_DEFINITIONS})
    endif()

    if(CONNEXTDDS_IMPORTED_TARGETS_DEBUG)
        set(build_type "DEBUG")
    else()
        set(build_type "RELEASE")
    endif()

    if(NOT TARGET RTIConnextDDS::c_api)
        list(GET CONNEXTDDS_C_API_LIBRARIES_${build_type}_${link_type} 0
            c_api_library)
        list(GET CONNEXTDDS_C_API_LIBRARIES_${build_type}_${link_type} 1
             core_library)

        set(dependencies
            ${core_library}
            ${CONNEXTDDS_EXTERNAL_LIBS})

        add_library(RTIConnextDDS::c_api ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::c_api
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${c_api_library}"
                INTERFACE_INCLUDE_DIRECTORIES
                    "${CONNEXTDDS_INCLUDE_DIRS}"
                INTERFACE_COMPILE_DEFINITIONS
                    "${target_definitions}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")

    endif()

    if(NOT TARGET RTIConnextDDS::cpp_api)
        list(GET CONNEXTDDS_CPP_API_LIBRARIES_${build_type}_${link_type} 0
            cpp_api_library)
        list(GET CONNEXTDDS_CPP_API_LIBRARIES_${build_type}_${link_type} 1
            c_api_library)
        list(GET CONNEXTDDS_CPP_API_LIBRARIES_${build_type}_${link_type} 2
            core_library)

        add_library(RTIConnextDDS::cpp_api ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::cpp_api
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${cpp_api_library}"
                INTERFACE_LINK_LIBRARIES
                    RTIConnextDDS::c_api)
    endif()

    if(NOT TARGET RTIConnextDDS::cpp2_api)
        list(GET CONNEXTDDS_CPP2_API_LIBRARIES_${build_type}_${link_type} 0
            cpp2_api_library)
        list(GET CONNEXTDDS_CPP2_API_LIBRARIES_${build_type}_${link_type} 1
            c_api_library)
        list(GET CONNEXTDDS_CPP2_API_LIBRARIES_${build_type}_${link_type} 2
            core_library)

        add_library(RTIConnextDDS::cpp2_api ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::cpp2_api
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${cpp2_api_library}"
                INTERFACE_LINK_LIBRARIES
                    RTIConnextDDS::c_api)
    endif()

    if(RTIConnextDDS_distributed_logger_FOUND)
        if(NOT TARGET RTIConnextDDS::distributed_logger_c)
            list(GET DISTRIBUTED_LOGGER_C_LIBRARIES_${build_type}_${link_type} 0
                distributed_logger_c_lib)

            add_library(RTIConnextDDS::distributed_logger_c
                ${link_type}
                IMPORTED)
            set_target_properties(RTIConnextDDS::distributed_logger_c
                PROPERTIES
                    IMPORTED_NO_SONAME TRUE
                    ${location_property}
                        "${distributed_logger_c_lib}"
                    INTERFACE_LINK_LIBRARIES
                        RTIConnextDDS::c_api)
        endif()

        if(NOT TARGET RTIConnextDDS::distributed_logger_cpp)
            list(GET
                DISTRIBUTED_LOGGER_CPP_LIBRARIES_${build_type}_${link_type} 0
                distributed_logger_cpp_lib)

            add_library(RTIConnextDDS::distributed_logger_cpp
                ${link_type}
                IMPORTED)
            set_target_properties(RTIConnextDDS::distributed_logger_cpp
                PROPERTIES
                    IMPORTED_NO_SONAME TRUE
                    ${location_property}
                        "${distributed_logger_cpp_lib}"
                    INTERFACE_LINK_LIBRARIES
                        RTIConnextDDS::cpp_api)
        endif()
    endif()

    if(RTIConnextDDS_metp_FOUND AND NOT TARGET RTIConnextDDS::metp)
        list(GET METP_LIBRARIES_${build_type}_${link_type} 0 rtimetp_library)

        add_library(RTIConnextDDS::metp ${link_type} IMPORTED)

        set_target_properties(RTIConnextDDS::metp
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtimetp_library}"
                INTERFACE_LINK_LIBRARIES
                    RTIConnextDDS::c_api)
    endif()

    if(RTIConnextDDS_routing_service_FOUND AND
            NOT TARGET RTIConnextDDS::routing_service_infrastructure)

        list(GET
            ROUTING_SERVICE_INFRASTRUCTURE_LIBRARIES_${build_type}_${link_type}
            0
            rtirsinfrastructure_library)

        add_library(
            RTIConnextDDS::routing_service_infrastructure
            ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::routing_service_infrastructure
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtirsinfrastructure_library}"
                INTERFACE_LINK_LIBRARIES
                    RTIConnextDDS::c_api)
    endif()


    if(RTIConnextDDS_routing_service_FOUND AND
        NOT TARGET RTIConnextDDS::routing_service_c)

        list(GET ROUTING_SERVICE_API_LIBRARIES_${build_type}_${link_type} 0
            rtirroutingservice_library)

        set(dependencies
            RTIConnextDDS::routing_service_infrastructure)

        if(RTIConnextDDS_metp_FOUND)
            set(dependencies ${dependencies} "RTIConnextDDS::metp")
        endif()

        if(RTIConnextDDS_distributed_loger_FOUND)
            set(dependencies
                ${dependencies}
                "RTIConnextDDS::distributed_logger_c")
        endif()

        add_library(RTIConnextDDS::routing_service_c ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::routing_service_c
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtirroutingservice_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")

        add_library(RTIConnextDDS::routing_service ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::routing_service
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtirroutingservice_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_routing_service_FOUND AND
            NOT TARGET RTIConnextDDS::routing_service_cpp2 AND
            RTICONNEXTDDS_VERSION VERSION_GREATER_EQUAL "6.0.0")

        list(GET ROUTING_SERVICE_API_LIBRARIES_${build_type}_${link_type} 0
            rtirroutingservice_library)

        set(dependencies
            RTIConnextDDS::routing_service_infrastructure
            RTIConnextDDS::cpp2_api)

        if(RTIConnextDDS_metp_FOUND)
            set(dependencies ${dependencies} "RTIConnextDDS::metp")
        endif()

        if(RTIConnextDDS_distributed_loger_FOUND)
            set(dependencies
                ${dependencies}
                "RTIConnextDDS::distributed_logger_cpp")
        endif()

        add_library(RTIConnextDDS::routing_service_cpp2 ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::routing_service_cpp2
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtirroutingservice_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_assign_transformation_FOUND AND
            NOT TARGET RTIConnextDDS::assign_transformation)
        list(GET ASSIGN_TRANSFORMATION_LIBRARIES_${build_type}_${link_type} 0
            rtiassigntransf_library)
        add_library(RTIConnextDDS::assign_transformation ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::routing_service_c
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::assign_transformation
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtiassigntransf_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_messaging_api_FOUND)
        if(NOT TARGET RTIConnextDDS::messaging_c_api)
            list(GET MESSAGING_C_API_LIBRARIES_${build_type}_${link_type} 0
                messaging_c_lib)
            add_library(RTIConnextDDS::messaging_c_api ${link_type} IMPORTED)
            set_target_properties(RTIConnextDDS::messaging_c_api
                PROPERTIES
                    INTERFACE_LINK_LIBRARIES
                        RTIConnextDDS::c_api
                    IMPORTED_NO_SONAME TRUE
                    ${location_property}
                        "${messaging_c_lib}")
        endif()

        if(NOT TARGET RTIConnextDDS::messaging_cpp_api)
            list(GET MESSAGING_CPP_API_LIBRARIES_${build_type}_${link_type} 0
                messaging_cpp_lib)
            add_library(RTIConnextDDS::messaging_cpp_api ${link_type} IMPORTED)
            set_target_properties(RTIConnextDDS::messaging_cpp_api
                PROPERTIES
                    INTERFACE_LINK_LIBRARIES
                        RTIConnextDDS::cpp_api
                    IMPORTED_NO_SONAME TRUE
                    ${location_property}
                        "${messaging_cpp_lib}")
        endif()

        if(NOT TARGET RTIConnextDDS::messaging_cpp2_api)
            list(GET MESSAGING_CPP2_API_LIBRARIES_${build_type}_${link_type} 0
                messaging_cpp2_lib)
            add_library(RTIConnextDDS::messaging_cpp2_api
                ${link_type}
                IMPORTED)
            set_target_properties(RTIConnextDDS::messaging_cpp2_api
                PROPERTIES
                    INTERFACE_LINK_LIBRARIES
                        RTIConnextDDS::cpp2_api
                    IMPORTED_NO_SONAME TRUE
                    ${location_property}
                        "${messaging_cpp2_lib}")
        endif()
    endif()

    if(RTIConnextDDS_security_plugins_FOUND AND
            NOT TARGET RTIConnextDDS::security_plugins)
        add_library(RTIConnextDDS::security_plugins ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::security_plugins
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${SECURITY_PLUGINS_LIBRARIES_${build_type}_${link_type}}")
    endif()

    if(RTIConnextDDS_monitoring_libraries_FOUND AND
            NOT TARGET RTIConnextDDS::monitoring)
        add_library(RTIConnextDDS::monitoring ${link_type} IMPORTED)
        set_target_properties(RTIConnextDDS::monitoring
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${MONITORING_LIBRARIES_${build_type}_${link_type}}")
    endif()

    if(RTIConnextDDS_nddstls_FOUND AND
            NOT TARGET RTIConnextDDS::nddstls)
        list(GET NDDSTLS_LIBRARIES_${build_type}_${link_type} 0
            nddstls_library)
        add_library(RTIConnextDDS::nddstls ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::nddstls
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${nddstls_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_transport_tcp_FOUND AND
            NOT TARGET RTIConnextDDS::transport_tcp)
        list(GET TRANSPORT_TCP_LIBRARIES_${build_type}_${link_type} 0
            transport_tcp_library)
        add_library(RTIConnextDDS::transport_tcp ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::transport_tcp
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${transport_tcp_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_transport_tls_FOUND AND
            NOT TARGET RTIConnextDDS::transport_tls)
        list(GET TRANSPORT_TLS_LIBRARIES_${build_type}_${link_type} 0
            transport_tls_library)
        add_library(RTIConnextDDS::transport_tls ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::transport_tls
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${transport_tls_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_transport_wan_FOUND AND
            NOT TARGET RTIConnextDDS::transport_wan)
        list(GET TRANSPORT_WAN_LIBRARIES_${build_type}_${link_type} 0
            transport_wan_library)
        add_library(RTIConnextDDS::transport_wan ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::transport_tls
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::transport_wan
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${transport_wan_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_recording_service_FOUND AND
            NOT TARGET RTIConnextDDS::recording_service)
        list(GET RECORDING_SERVICE_LIBRARIES_${build_type}_${link_type} 0
            recording_service_library)
        add_library(RTIConnextDDS::recording_service ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::routing_service_c
            RTIConnextDDS::routing_service_infrastructure
            RTIConnextDDS::cpp2_api
            RTIConnextDDS::distributed_logger_c
            RTIConnextDDS::metp
            RTIConnextDDS::messaging_c_api
            RTIConnextDDS::rtixml2
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::recording_service
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${recording_service_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_rtixml2_FOUND AND
            NOT TARGET RTIConnextDDS::rtixml2)
        list(GET RTIXML2_LIBRARIES_${build_type}_${link_type} 0
            rtixml2_library)
        add_library(RTIConnextDDS::rtixml2 ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::rtixml2
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtixml2_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_low_bandwidth_plugins_FOUND AND
            NOT TARGET RTIConnextDDS::low_bandwidth_plugins)
        list(GET LOW_BANDWIDTH_PLUGINS_LIBRARIES_${build_type}_${link_type} 0
            low_bandwidth_plugins_library)
        add_library(RTIConnextDDS::low_bandwidth_plugins ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::low_bandwidth_discovery_static
            RTIConnextDDS::low_bandwidth_edisc
            RTIConnextDDS::low_bandwidth_pdisc
            RTIConnextDDS::low_bandwidth_rtps
            RTIConnextDDS::low_bandwidth_sm
            RTIConnextDDS::low_bandwidth_st
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::low_bandwidth_plugins
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${low_bandwidth_plugins_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

    if(RTIConnextDDS_rtirtps_FOUND AND
            NOT TARGET RTIConnextDDS::rtirtps)
        list(GET RTIRTPS_LIBRARIES_${build_type}_${link_type} 0
            rtirtps_library)
        add_library(RTIConnextDDS::rtirtps ${link_type} IMPORTED)
        set(dependencies
            RTIConnextDDS::c_api)
        set_target_properties(RTIConnextDDS::rtirtps
            PROPERTIES
                IMPORTED_NO_SONAME TRUE
                ${location_property}
                    "${rtirtps_library}"
                INTERFACE_LINK_LIBRARIES
                    "${dependencies}")
    endif()

endif()
