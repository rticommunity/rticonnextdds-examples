# (c) 2018 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.

#[[.rst:
.. _connextdds_scripts_writefile:

WriteFile
---------
::

    cmake -DOUTPUT=outputfile -DCONTENT=HelloWorld [-DAPPEND=1] -PWriteFile.cmake

Write a string in the output file. If the ``APPEND`` argument is not specified,
the file will be truncated with the new content, otherwise it will be appended.

Arguments:

``CONTENT`` (required)
  String content to write in the file.

``OUTPUT`` (required)
  The path to the output file.

``APPEND`` (optional)
  If set the content will be append.
#]]

if(NOT CONTENT)
    message(FATAL_ERROR "Missing CONTENT argument")
endif()

if(NOT OUTPUT)
    message(FATAL_ERROR "Missing OUTPUT argument")
endif()

set(write_mode WRITE)
if(APPEND)
    set(write_mode APPEND)
endif()

file(${write_mode} "${OUTPUT}" "${CONTENT}")
