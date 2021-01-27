# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

macro(connextdds_log_message)
    set(available_verbosities
        FATAL_ERROR
        SEND_ERROR
        WARNING
        AUTHOR_WARNING
        DEPRECATION
        NOTICE
        STATUS
        VERBOSE
        DEBUG
        TRACE
    )
    set(options)
    set(single_value_args)
    set(multi_value_args
        ${available_verbosities}
    )
    cmake_parse_arguments(_MESSAGE
        "${options}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )


    set(enable_color ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1)
    set(command_prefix ${enable_color} ${CMAKE_COMMAND} -E cmake_echo_color)

    set(red ${command_prefix} --red)
    set(black ${command_prefix} --black)
    set(green ${command_prefix} --green)
    set(yellow ${command_prefix} --yellow)
    set(blue ${command_prefix} --blue)
    set(magenta ${command_prefix} --magenta)
    set(cyan ${command_prefix} --cyan)
    set(white ${command_prefix} --white)

    # Set a default value
    if("${CMAKE_VERSION}" VERSION_LESS 3.15 AND NOT CMAKE_MESSAGE_LOG_LEVEL)
        set(CMAKE_MESSAGE_LOG_LEVEL STATUS)
    endif()

    set(verbosity)
    set(text_message)

    if(_MESSAGE_FATAL_ERROR) # FATAL_ERROR
        set(verbosity FATAL_ERROR)
        set(printable TRUE)

        execute_process(
            COMMAND
                ${red} --bold --no-newline "[ERROR] ${_MESSAGE_FATAL_ERROR}"
            OUTPUT_VARIABLE
                text_message
        )

        file(WRITE
            "${CMAKE_BINARY_DIR}/connextdds_error"
            "${text_message}"
        )
    elseif(_MESSAGE_SEND_ERROR) # SEND_ERROR
        set(verbosity SEND_ERROR)
        _connextdds_printable_log()
        if(printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity FATAL_ERROR)
                _connextdds_printable_log()
            endif()

            execute_process(
                COMMAND
                    ${red} --bold --no-newline "[SEND_ERROR] ${_MESSAGE_SEND_ERROR}"
                OUTPUT_VARIABLE
                    text_message
            )
        endif()
    elseif(_MESSAGE_DEPRECATION) # DEPRECATION
        set(verbosity WARNING)
        _connextdds_printable_log()
        if(NOT printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity DEPRECATION)
            endif()

            execute_process(
                COMMAND
                    ${yellow} --no-newline "[DEPRECATION] ${_MESSAGE_DEPRECATION}"
                OUTPUT_VARIABLE
                    text_message
            )
        endif()
    elseif(_MESSAGE_NOTICE) # NOTICE
        set(verbosity NOTICE)
        _connextdds_printable_log()
        if(printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity)
            endif()

            set(text_message "${_MESSAGE_NOTICE}")
        endif()
    elseif(_MESSAGE_WARNING) # WARNING
        set(verbosity WARNING)
        _connextdds_printable_log()
        if(printable)
            execute_process(
                COMMAND
                    ${yellow} --bold --no-newline "[WARNING] ${_MESSAGE_WARNING}"
                OUTPUT_VARIABLE
                    text_message
            )
        endif()
    elseif(_MESSAGE_AUTHOR_WARNING) # AUTHOR_WARNING
        set(verbosity AUTHOR_WARNING)
        _connextdds_printable_log()
        if(printable)
            execute_process(
                COMMAND
                    ${yellow} --bold --no-newline "[AUTHOR_WARNING] ${_MESSAGE_WARNING}"
                OUTPUT_VARIABLE
                    text_message
            )
        endif()
    elseif(_MESSAGE_SEND_ERROR) # SEND_ERROR
        set(verbosity SEND_ERROR)
        _connextdds_printable_log()
        if(printable)
            execute_process(
                COMMAND
                    ${red} --bold --no-newline "[SEND_ERROR] ${_MESSAGE_SEND_ERROR}"
                OUTPUT_VARIABLE
                    text_message
            )
        endif()
    elseif(_MESSAGE_VERBOSE) # VERBOSE
        set(verbosity VERBOSE)
        _connextdds_printable_log()
        if(printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity STATUS)
            endif()

            execute_process(
                COMMAND
                    ${green} --bold --no-newline "[VERBOSE] "
                OUTPUT_VARIABLE
                    fmt_verbosity
            )

            set(text_message "${fmt_verbosity} ${_MESSAGE_VERBOSE}")
        endif()
    elseif(_MESSAGE_DEBUG) # DEBUG
        set(verbosity DEBUG)
        _connextdds_printable_log()
        if(printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity STATUS)
            endif()

            execute_process(
                COMMAND
                    ${cyan} --bold --no-newline "[DEBUG] "
                OUTPUT_VARIABLE
                    fmt_verbosity
            )

            set(text_message "${fmt_verbosity} ${_MESSAGE_DEBUG}")
        endif()
    elseif(_MESSAGE_TRACE) # TRACE
        set(verbosity TRACE)
        _connextdds_printable_log()
        if(printable)
            if("${CMAKE_VERSION}" VERSION_LESS 3.15)
                set(verbosity STATUS)
            endif()

            execute_process(
                COMMAND
                    ${magenta} --bold --no-newline "[TRACE] "
                OUTPUT_VARIABLE
                    fmt_verbosity
            )

            execute_process(
                COMMAND
                    ${cyan} --bold --no-newline "${_MESSAGE_TRACE}"
                OUTPUT_VARIABLE
                    fmt_message
            )

            set(text_message "${fmt_verbosity} ${fmt_message}")
        endif()
    elseif(_MESSAGE_STATUS) # STATUS
        set(verbosity STATUS)
        _connextdds_printable_log()
        if(printable)
            execute_process(
                COMMAND
                    ${green} --no-newline "[STATUS] "
                OUTPUT_VARIABLE
                    fmt_verbosity
            )

            set(text_message "${fmt_verbosity} ${_MESSAGE_STATUS}")
        endif()
    endif()

    if(printable)
        string(REPLACE ";" " " sanitized_text_message "${text_message}")
        message(${verbosity} "${sanitized_text_message}")
    endif()

endmacro()


macro(_connextdds_printable_log)

    set(printable TRUE)

    if("${CMAKE_VERSION}" VERSION_LESS 3.15)
        list(FIND available_verbosities
            ${verbosity}
            message_verbosity_level
        )
        list(FIND
            available_verbosities
            ${CMAKE_MESSAGE_LOG_LEVEL}
            current_verbosity_level
        )
        if(${current_verbosity_level} GREATER_EQUAL ${message_verbosity_level})
            set(printable TRUE)
        else()
            set(printable FALSE)
        endif()

    endif()

endmacro()
