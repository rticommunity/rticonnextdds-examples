/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "FileStorageUtils.h"

int RTI_fopen(FILE **file, const char *file_name, const char *mode)
{
    int error_code = 0;
#ifdef RTI_WIN32
    error_code = fopen_s(file, file_name, mode);
#else
    (*file) = fopen(file_name, mode);

    if ((*file) == NULL) {
        error_code = 1;
    }
#endif
    return error_code;
}
