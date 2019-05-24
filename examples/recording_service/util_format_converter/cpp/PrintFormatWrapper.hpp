/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <fstream>
#include <iostream>
#include <list>

#include "dds_c/dds_c_printformat.h"


namespace rti { namespace recorder { namespace utils {


template <typename T> struct PrintFormatWrapper : public DDS_PrintFormat {
public:

    PrintFormatWrapper(T *user_data) :
            user_data_(user_data)
    {
        //init native implementation of DDS_PrintFormat
        DDS_PrintFormat_initialize(this, DDS_DEFAULT_PRINT_FORMAT);
    }

    ~PrintFormatWrapper()
    {
         DDS_PrintFormat_finalize(this, DDS_DEFAULT_PRINT_FORMAT);
    }


    T *user_data_;
};

} } }