#!/usr/bin/env bash
#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved. RTI
# grants Licensee a license to use, modify, compile, and create derivative
# works of the Software.  Licensee has the right to distribute object form only
# for use with RTI products.  The Software is provided "as is", with no
# warranty of any type, including any warranty for fitness for any purpose. RTI
# is under no obligation to maintain or support the Software.  RTI shall not be
# liable for any incidental or consequential damages arising out of the use or
# inability to use the software.
#
# Run clang-format for all files passed as arguments.
# Example:
#
#     diff_clang_format.sh path/to/file1 path/to/file2
#
error=0

# Loop over every file in passed as argument
for current_file in "$@"; do
    printf 'Diffing %s file\n' "$current_file"
    diff -u1 $current_file <(clang-format $current_file)

    # If there was differences between the file and the clang-format output
    # exit with errors when the program ends
    if [ "$?" -eq "1" ]; then
        error=1
        printf '\n##################################################\n'
    fi
done

exit $error
