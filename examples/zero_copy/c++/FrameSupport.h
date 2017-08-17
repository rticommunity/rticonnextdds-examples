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

#include <iostream>
#include <stdexcept>

#include "ndds/osapi/osapi_sharedMemorySegment.h"
#include "ndds/osapi/osapi_process.h"
#include "ndds/osapi/osapi_alignment.h"
#include "ndds/osapi/osapi_utility.h"
#include "string.h"
#include "Frame.h"

//
// Implementation details to access frames in shared memory
//
class FrameSupport {
public:
    explicit FrameSupport(int payload_size, int count = 0);

    // Shared memory handle
    struct RTIOsapiSharedMemorySegmentHandle handle;

    // Points to the begining of a Frame header
    char *frame_base_address;

    // Size of the payload
    int buffer_size;

    // Size of Frame including Frame header, payload and alignment
    int frame_size;

    // Number of frames being mapped into shared memory
    int frame_count;

    // Gets the adress of the Frame Header located at given offset
    Frame* get_frame(int index);

    const Frame* get_const_frame(int index) const;
};

//
// Provides write access to a set of frames in shared memory
//
class FrameSet {
public:
    //
    // Creates a shared memory segment to contain frames of fixed-sized payload
    // identified by a key
    //
    FrameSet(int key, int frame_count, int payload_size);
    ~FrameSet();

    // Obtains a frame in the specified position
    Frame* operator[](int index)
    {
        return frame_support.get_frame(index);
    }

private:
    FrameSupport frame_support;
};

//
// Provides read-only access to a set of frames in shared memory
//
class FrameSetView {
public:
    int key;

    // Attaches to a shared memory segment that contains frames of a fixed-size
    // payload, identified by a key.
    //
    // The key and the payload_size must match those used to create a
    // FrameSet.
    FrameSetView(int key, int payload_size);
    ~FrameSetView();

    // Obtains a read-only frame in the specified position
    const Frame* operator[](int index) const
    {
        return frame_support.get_const_frame(index);
    }

private:
    FrameSupport frame_support;
};

// --- FrameSupport: ----------------------------------------------------------

FrameSupport::FrameSupport(int payload_size, int count)
    : frame_base_address(NULL),
      buffer_size(payload_size),
      frame_count(count)
{
    struct FrameOffset { char c; class Frame member; };
    int alignment = offsetof (FrameOffset, member);
    frame_size = sizeof(Frame) + buffer_size + alignment;
}

Frame* FrameSupport::get_frame(int index)
{
    Frame *frame =
            (Frame *)((char *)frame_base_address + (index * frame_size));
    frame->length = buffer_size;
    return frame;
}

const Frame* FrameSupport::get_const_frame(int index) const
{
    return (const Frame *)((char *)frame_base_address + (index * frame_size));
}

// --- FrameSet: --------------------------------------------------------------

FrameSet::FrameSet(int key, int frame_count, int payload_size)
        : frame_support(payload_size, frame_count)
{
    int total_size = frame_support.frame_size * frame_count;
    RTI_UINT64 pid = RTIOsapiProcess_getId();
    int status = 0;
    RTIBool result = RTIOsapiSharedMemorySegment_createOrAttach(
        &frame_support.handle,
        &status,
        key,
        total_size,
        pid);
    if (!result) {
        std::ostringstream status_to_str;
        status_to_str << "create shared memory segments error " << status;
        throw std::runtime_error(status_to_str.str().c_str());
    }

    if (status == RTI_OSAPI_SHARED_MEMORY_ATTACHED) {
        // The shared memory segement already exists. Because the settings
        // maybe different we have to destroy it a recreate it
        RTIOsapiSharedMemorySegment_delete(&frame_support.handle);

        result = RTIOsapiSharedMemorySegment_create(
            &frame_support.handle,
            &status,
            key,
            total_size,
            pid);
        if (!result) {
            std::ostringstream status_to_str;
            status_to_str << "create shared memory segments error " << status;
            throw std::runtime_error(status_to_str.str().c_str());
        }
    }
    frame_support.frame_base_address =
        (char *)RTIOsapiSharedMemorySegment_getAddress(&frame_support.handle);
}

FrameSet::~FrameSet()
{
    if (!RTIOsapiSharedMemorySegment_delete(&frame_support.handle)) {
        std::cout << "delete shared memory segments error" << std::endl;
    }
}

// --- FrameSetView: ----------------------------------------------------------

FrameSetView::FrameSetView(int key, int payload_size)
        : frame_support(payload_size)
{
    int status = 0;
    RTIBool result = RTIOsapiSharedMemorySegment_attach(
            &frame_support.handle,
            &status,
            key);

    if (!result) {
        std::ostringstream status_to_str;
        status_to_str << "create shared memory segments error " << status;
        throw std::runtime_error(status_to_str.str().c_str());
    }

    this->key = key;
    int total_size =
            RTIOsapiSharedMemorySegment_getSize(&frame_support.handle);
    frame_support.frame_count = total_size/frame_support.frame_size;
    frame_support.frame_base_address =
            (char *)RTIOsapiSharedMemorySegment_getAddress(&frame_support.handle);
}

FrameSetView::~FrameSetView()
{
    if (!RTIOsapiSharedMemorySegment_detach(&frame_support.handle)) {
        std::cout << "detach shared memory segments error" << std::endl;
    }
}
