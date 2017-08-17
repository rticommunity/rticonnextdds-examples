/*******************************************************************************
 (c) 2005-2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 ******************************************************************************/

struct Dimension {
    int x;
    int y;
};

class Frame {
public:
    char* get_buffer();
    const char* get_buffer() const;

    int length;
    unsigned int checksum;
    Dimension dimension;
};

char* Frame::get_buffer()
{
    return ((char *)this) + sizeof(Frame);
}

const char* Frame::get_buffer() const
{
    return ((char *)this) + sizeof(Frame);
}
