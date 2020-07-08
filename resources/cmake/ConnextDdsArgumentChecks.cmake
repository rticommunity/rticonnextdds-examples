# (c) 2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _connextdds_argument_checks:

ConnextDdsArgumentChecks
------------------------

Function helpers to check function arguments

``connextdds_check_required_arguments``
    Checks that all of the arguments are present and defined

#]]

function(connextdds_check_required_arguments)
    foreach(arg ${ARGN})
        if(NOT ${arg})
            message(FATAL_ERROR "Argument ${arg} is missing")
        endif()
    endforeach()
endfunction()

macro(connextdds_check_no_extra_arguments)
    if(ARGN)
        message(FATAL_ERROR "Function has more arguments than expected")
    endif()
endmacro()
