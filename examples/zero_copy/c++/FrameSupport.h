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
    explicit FrameSupport(int payload_size);

    // Gets the adress of the Frame Header located at given offset
    Frame* get_frame(int index);
    const Frame* get_const_frame(int index) const;

    // Sets the base address of the frame
    void set_frame_base_address(RTIOsapiSharedMemorySegmentHandle *handle);

    // Gets the frame size
    int get_frame_size();

private:
    // Points to the begining of a Frame header
    char *frame_base_address;

    // Size of the payload
    int buffer_size;

    // Size of Frame including Frame header, payload and alignment
    int frame_size;
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
        if ((index >= 0) && (index < frame_count)) {
            return frame_support.get_frame(index);
        } else {
            return NULL;
        }
    }

private:
    FrameSupport frame_support;

    // Shared memory handle
    RTIOsapiSharedMemorySegmentHandle handle;

    // Number of frames being mapped into shared memory
    int frame_count;
};

//
// Provides read-only access to a set of frames in shared memory
//
class FrameSetView {
public:
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
        if ((index >= 0) && (index < frame_count)) {
            return frame_support.get_const_frame(index);
        } else {
            return NULL;
        }
    }

    // Returns the key used to attach to the shared memory segment
    int get_key()
    {
        return key;
    }

private:
    FrameSupport frame_support;

    // Shared memory handle
    RTIOsapiSharedMemorySegmentHandle handle;

    // Number of frames being mapped into shared memory
    int frame_count;

    // Key used for attaching to shared memory segments
    int key;
};

// --- FrameSupport: ----------------------------------------------------------

FrameSupport::FrameSupport(int payload_size)
    : frame_base_address(NULL),
      buffer_size(payload_size)
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

void FrameSupport::set_frame_base_address(
        RTIOsapiSharedMemorySegmentHandle *handle) {
    frame_base_address = (char *)RTIOsapiSharedMemorySegment_getAddress(handle);
}

int FrameSupport::get_frame_size() {
    return frame_size;
}

// --- FrameSet: --------------------------------------------------------------

FrameSet::FrameSet(int key, int frame_count, int payload_size)
    : frame_support(payload_size), frame_count(frame_count)
{
    int frame_size = frame_support.get_frame_size();
    int total_size = frame_size * frame_count;
    RTI_UINT64 pid = RTIOsapiProcess_getId();
    int status = 0;
    RTIBool result = RTIOsapiSharedMemorySegment_createOrAttach(
        &handle,
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
        RTIOsapiSharedMemorySegment_delete(&handle);

        result = RTIOsapiSharedMemorySegment_create(
            &handle,
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

    frame_support.set_frame_base_address(&handle);
}

FrameSet::~FrameSet()
{
    if (!RTIOsapiSharedMemorySegment_delete(&handle)) {
        std::cout << "delete shared memory segments error" << std::endl;
    }
}

// --- FrameSetView: ----------------------------------------------------------

FrameSetView::FrameSetView(int key, int payload_size)
        : frame_support(payload_size), key(key)
{
    int status = 0;
    RTIBool result = RTIOsapiSharedMemorySegment_attach(
            &handle,
            &status,
            key);

    if (!result) {
        std::ostringstream status_to_str;
        status_to_str << "attach to shared memory segments error " << status;
        throw std::runtime_error(status_to_str.str().c_str());
    }

    frame_support.set_frame_base_address(&handle);
    int total_size = RTIOsapiSharedMemorySegment_getSize(&handle);
    int frame_size = frame_support.get_frame_size();
    frame_count = total_size/frame_size;
}

FrameSetView::~FrameSetView()
{
    if (!RTIOsapiSharedMemorySegment_detach(&handle)) {
        std::cout << "detach shared memory segments error" << std::endl;
    }
}
