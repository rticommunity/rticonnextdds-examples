/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef RS_STREAM_WRITER_
#define RS_STREAM_WRITER_

#include "ConnectorMsqlDb.hpp"
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/sub/SampleInfo.hpp>
#include <rti/routing/PropertySet.hpp>
#include <rti/routing/StreamInfo.hpp>
#include <rti/routing/adapter/StreamWriter.hpp>

class RsStreamWriter : public rti::routing::adapter::DynamicDataStreamWriter {
public:
    RsStreamWriter(
            const rti::routing::StreamInfo &stream_info,
            const rti::routing::PropertySet &properties);
    virtual ~RsStreamWriter();

    auto write(
            const std::vector<dds::core::xtypes::DynamicData *> &sample_seq,
            const std::vector<dds::sub::SampleInfo *> &info_seq) -> int;

private:
    ConnectorBase *connWriter_ { nullptr };
};

#endif /* RS_STREAM_WRITER_ */
